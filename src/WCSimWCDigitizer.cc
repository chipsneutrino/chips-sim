#include "WCSimWCDigitizer.hh"
#include "WCSimWCDigi.hh"
#include "WCSimWCHit.hh"

#include "G4EventManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4DigiManager.hh"
#include "G4ios.hh"

#include "WCSimDetectorConstruction.hh"
#include "WCSimPMTManager.hh"
#include "WCSimPMTConfig.hh"
#include "WCSimCHIPSPMT.hh"
#include "WCSimSK1pePMT.hh"
#include "WCSimTOTPMT.hh"

#include <vector>
// for memset
#include <cstring>
#include <iostream>

// changed from 940 (april 2005) by MF
// 960 is unsuitable

const double WCSimWCDigitizer::offset = 950.0; // ns
const double WCSimWCDigitizer::pmtgate = 200.0; // ns
const double WCSimWCDigitizer::eventgateup = 950.0; // ns
const double WCSimWCDigitizer::eventgatedown = -400.0; // ns
const double WCSimWCDigitizer::LongTime = 100000.0; // ns
// value in skdetsim
const int WCSimWCDigitizer::GlobalThreshold = 25; // # hit PMTs
//const int WCSimWCDigitizer::GlobalThreshold = 12 ; // # hit PMTs
//const int WCSimWCDigitizer::GlobalThreshold = 10 ; // # hit PMTs

const double WCSimWCDigitizer::PMTDarkRate = 4.0; // kHz

extern "C" void skrn1pe_(float*);
//extern "C" void rn1pe_(float* ); // 1Kton

WCSimWCDigitizer::WCSimWCDigitizer(G4String name, WCSimDetectorConstruction *myDet) :
		G4VDigitizerModule(name) {
	G4String colName = "WCDigitizedCollection";
	collectionName.push_back(colName);
	DigiHitMap.clear();
	fDet = myDet;
	fPMTSim = new WCSimCHIPSPMT();
	fSK1peSim = new WCSimSK1pePMT();
	fTOTSim = new WCSimTOTPMT();
}

WCSimWCDigitizer::~WCSimWCDigitizer() {
	delete fPMTSim;
	delete fSK1peSim;
	delete fTOTSim;
}

void WCSimWCDigitizer::Digitize() {
	DigitsCollection = new WCSimWCDigitsCollection("/WCSim/glassFaceWCPMT", collectionName[0]);

	G4DigiManager* DigiMan = G4DigiManager::GetDMpointer();

	// Get the Associated Hit collection IDs
	G4int WCHCID = DigiMan->GetHitsCollectionID("glassFaceWCPMT");

	// The Hits collection
	WCSimWCHitsCollection* WCHC = (WCSimWCHitsCollection*) (DigiMan->GetHitsCollection(WCHCID));

	if (WCHC) {

//		MakeHitsHistogram(WCHC); 
		//FindNumberOfGates(); //get list of t0 and number of triggers.
//		FindNumberOfGatesFast(); //get list of t0 and number of triggers.
		this->FindTriggerWindows(WCHC);

		for (int i = 0; i < this->NumberOfGatesInThisEvent(); i++) {
			DigitizeGate(WCHC, i);
		}
	}

	StoreDigiCollection (DigitsCollection);

}

void WCSimWCDigitizer::FindTriggerWindows(WCSimWCHitsCollection* hits) {

	std::vector<double> pmtHitTimes;

	// Remember the index of this vector is tubeID - 1.
	std::vector<WCSimPmtInfo*> *pmtInfoVec = fDet->Get_Pmts();

	for (int i = 0; i < hits->entries(); ++i) {
		WCSimWCHit* hit = (*hits)[i];
		WCSimPmtInfo *pmtInfo = pmtInfoVec->at(hit->GetTubeID() - 1);
		if (pmtInfo->Get_cylocation() == 3) {
			continue; // Don't consider veto PMTs in the window finding.
		}
		// Make sure that we get the first hit on the PMT
		hit->SortHitTimes();
		pmtHitTimes.push_back(hit->GetTime(0));
	}

	// Now we want to sort out times into ascending order.
	std::sort(pmtHitTimes.begin(), pmtHitTimes.end());

	// Iterate over the times and look for continuous chains of hits without
	// a gap of great than 200ns, with at least GlobalThreshold hits.
	if (pmtHitTimes.size() > 0) {
		double firstHit = pmtHitTimes[0];
		unsigned int nHits = 1;
		for (unsigned int v = 1; v < pmtHitTimes.size(); ++v) {
//      std::cout << firstHit << ", " << pmtHitTimes[v-1] << ", " << pmtHitTimes[v] << " :: " << nHits << std::endl;
			if ((pmtHitTimes[v] - pmtHitTimes[v - 1]) < 200) {
				++nHits;
			} else {
				if (nHits >= WCSimWCDigitizer::GlobalThreshold) {
					// Save this time and start over.
					TriggerTimes.push_back(firstHit);
				}
				// Either way, start the process again
				firstHit = pmtHitTimes[v];
				nHits = 1;
			}
		}

		// Make a trigger time out of whatever is left if we need to.
		if (nHits >= WCSimWCDigitizer::GlobalThreshold) {
			TriggerTimes.push_back(firstHit);
		}
	}

	for (unsigned int t = 0; t < TriggerTimes.size(); ++t) {
		std::cout << "- Trigger time = " << TriggerTimes[t] << std::endl;
	}
}

void WCSimWCDigitizer::MakeHitsHistogram(WCSimWCHitsCollection* WCHC) {

	GateMap.clear();

	// Put the hits into an integer map
	for (G4int i = 0; i < WCHC->entries(); i++) {
		(*WCHC)[i]->SortHitTimes(); // VERY IMPORTANT!!!		
		GateMap[int(floor((*WCHC)[i]->GetTime(0)))]++;
	}

}

void WCSimWCDigitizer::FindNumberOfGatesFast() {
	//GateMap is indexed by time bin and stores the number of hits in that nanosecond.
	G4int acc = 0; // accumulated # hits within time window...
	std::map<G4int, G4int>::iterator _mGateKeeper, _mNextGate;

	for (_mGateKeeper = GateMap.begin(); _mGateKeeper != GateMap.end(); _mGateKeeper++) {
		acc = 0;
		_mNextGate = _mGateKeeper;
		RealOffset = 0.0; 				// will need to add the offset later
		// 40 means + 200ns
		// so check 39 bins ahead in the histogram..
		int offset200 = 199; // Leigh: Offset in number of bins corresponding to 200ns (was 39).
		while (_mNextGate != GateMap.lower_bound(_mGateKeeper->first + offset200)
				&& _mNextGate->first <= _mGateKeeper->first + offset200) // but not more than 200ns away though!
		{

			acc += _mNextGate->second;
			if (acc > WCSimWCDigitizer::GlobalThreshold) {
				//RealOffset = _mGateKeeper->first*5.0;
				//RealOffset = _mNextGate->first*5.0;
				RealOffset = _mNextGate->first;
				TriggerTimes.push_back(RealOffset);
				//std::cerr << "found a trigger..." << RealOffset/5.0  <<"\n";
				std::cerr << "found a trigger..." << RealOffset << "\n";
				//		    _mGateKeeper = GateMap.lower_bound( _mNextGate->first + G4int(WCSimWCDigitizer::eventgateup )/5. ); 
				_mGateKeeper = GateMap.lower_bound(_mNextGate->first + G4int(WCSimWCDigitizer::eventgateup));
				std::cerr.flush();
				break;
			}
			_mNextGate++;							// look at the next time bin with hits
		}
	}
}

void WCSimWCDigitizer::DigitizeGate(WCSimWCHitsCollection* WCHC, G4int G) {
	// First we figure out how the times of the gate...
	G4double EvtG8Down = WCSimWCDigitizer::eventgatedown;
	G4double EvtG8Up = WCSimWCDigitizer::eventgateup;  // this is a negative number...

	G4double lowerbound;
	if (G == 0)
		lowerbound = TriggerTimes[G] + EvtG8Down;
	else {
		lowerbound =
				(TriggerTimes[G] + EvtG8Down >= TriggerTimes[G - 1] + EvtG8Up) ?
						TriggerTimes[G] + EvtG8Down : TriggerTimes[G - 1] + EvtG8Up;
	}
	G4double upperbound = TriggerTimes[G] + EvtG8Up;

	// Before looping over the hits, get hold of the PMTManager
	WCSimPMTManager *pmtMan = fDet->GetPMTManager();
	for (G4int i = 0; i < WCHC->entries(); i++) {

		// What type of PMT do we have?
		WCSimPMTConfig pmtConfig = pmtMan->GetPMTByName((*WCHC)[i]->GetTubeName());
		G4int timingConstant = pmtConfig.GetTimeConstant(); // In ns

		// Get the tube ID and hit time
		G4int tube = (*WCHC)[i]->GetTubeID();
		G4float trueHitTime =
				(fDet->GetPMTTime() == 1) ?
						(*WCHC)[i]->GetMeanHitTimeInGate(lowerbound, upperbound) :
						(*WCHC)[i]->GetFirstHitTimeInGate(lowerbound, upperbound);

		// Check to see if the hit is in the gate
		if (trueHitTime < 0.) { //PMT not hit in this gate
			continue; // move on to the next Hit PMT
		}

		// Find the total number of hits (integer number) in the gate for the PMT
		double bound1 = trueHitTime + WCSimWCDigitizer::pmtgate;
		G4float totalPe = (*WCHC)[i]->GetPeInGate(lowerbound, upperbound, bound1);

		// Now digitize this hit
		G4double peSmeared = 0.0;

		// Check which method we should be using to measure the PE
		// Standard WCSim method based on SuperK (I think)
		if (fDet->GetPMTSim() == 0) {
			peSmeared = fSK1peSim->CalculateCharge(totalPe);
		}

		// CHIPS method based on a simulation of the IceCube PMTs, takes account
		// of non-linearity and saturation.
		else if (fDet->GetPMTSim() == 1) {
			// Firstly, we need to get the time spread of the photon arrival times.
			double minTime = trueHitTime;
			double maxTime = -1e20;
			if (totalPe == 1)
				maxTime = minTime;
			else
				maxTime = (*WCHC)[i]->GetLastHitTimeInGate(lowerbound, upperbound);
			peSmeared = fPMTSim->CalculateCharge(totalPe, minTime, maxTime);
		}

		// Time over threshold method.
		else if (fDet->GetPMTSim() == 2) {
			peSmeared = fTOTSim->CalculateCharge(totalPe, (*WCHC)[i]->GetTubeName());
		}

		else {
			std::cout << "Digi method not recognised!" << std::endl;
			peSmeared = 0.0;
		}

		// Once we have the smeared PE apply the time resolution smearing and save to the digi hit collection
		if (peSmeared > 0.0) {
			G4double digihittime = trueHitTime;

			// Add on a Gaussian resolution effect if the PMT resolution is switched on
			if (!fDet->GetPMTPerfectTiming()) {
				float Q = (peSmeared > 0.5) ? peSmeared : 0.5;
				float timingResolution = 0.33 + sqrt(timingConstant / Q);

				// looking at SK's jitter function for 20" tubes
				if (timingResolution < 0.58)
					timingResolution = 0.58;
				digihittime += G4RandGauss::shoot(0.0, timingResolution);
			}

			if (digihittime > 0.0) {
				if (DigiHitMap[tube] == 0) {
					WCSimWCDigi* Digi = new WCSimWCDigi();
					Digi->SetTubeID(tube);
					Digi->AddGate(G, TriggerTimes[G]);
					Digi->SetPe(G, peSmeared);
					Digi->SetTime(G, digihittime);
					DigiHitMap[tube] = DigitsCollection->insert(Digi);
				} else {
					(*DigitsCollection)[DigiHitMap[tube] - 1]->AddGate(G, TriggerTimes[G]);
					(*DigitsCollection)[DigiHitMap[tube] - 1]->SetPe(G, peSmeared);
					(*DigitsCollection)[DigiHitMap[tube] - 1]->SetTime(G, digihittime);
				}
			} else {
				G4cout << "discarded negative time hit\n";
			}

		}

	} // Loop over hits

}

