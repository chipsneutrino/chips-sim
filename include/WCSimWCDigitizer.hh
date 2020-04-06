#ifndef WCSimWCDigitizer_h
#define WCSimWCDigitizer_h 1

#include "G4VDigitizerModule.hh"
#include "WCSimWCDigi.hh"
#include "WCSimWCHit.hh"
#include "globals.hh"
#include "Randomize.hh"
#include <map>
#include <vector>

class WCSimDetectorConstruction;
class WCSimCHIPSPMT;
class WCSimSK1pePMT;
class WCSimTOTPMT;

class WCSimWCDigitizer : public G4VDigitizerModule
{
public:
	// Constructors / Destructor
	WCSimWCDigitizer(G4String name, WCSimDetectorConstruction *myDet);
	~WCSimWCDigitizer();

	void SetPMTSize(G4float inputSize)
	{
		PMTSize = inputSize;
	}
	void ReInitialize()
	{
		DigiHitMap.clear();
		TriggerTimes.clear();
	}
	int NumberOfGatesInThisEvent()
	{
		return TriggerTimes.size();
	}

public:
	void MakeHitsHistogram(WCSimWCHitsCollection *);
	void FindNumberOfGatesFast();
	void FindTriggerWindows(WCSimWCHitsCollection *hits); // Leigh, new simple function to find trigger windows.
	void DigitizeGate(WCSimWCHitsCollection *WCHC, G4int G);
	void Digitize();
	G4double GetTriggerTime(int i)
	{
		return TriggerTimes[i];
	}

	static G4double GetLongTime()
	{
		return LongTime;
	}
	static G4double GetPMTDarkRate()
	{
		return PMTDarkRate;
	}
	static G4double GetEventGateDown()
	{
		return eventgatedown;
	}
	static G4double GetEventGateUp()
	{
		return eventgateup;
	}

private:
	static const double offset;		   // hit time offset
	static const double pmtgate;	   // ns
	static const double eventgateup;   // ns
	static const double eventgatedown; // ns
	static const double LongTime;	   // ns
	static const int GlobalThreshold;  //number of hit PMTs within an <=200ns sliding window that decides the global trigger t0
	static const double PMTDarkRate;   // kHz

	G4float RealOffset; // t0 = offset corrected for trigger start
	G4float MinTime;	// very first hit time
	G4float PMTSize;

	std::vector<G4double> TriggerTimes;
	std::map<G4int, G4int> GateMap;
	std::map<int, int> DigiHitMap; // need to check if a hit already exists..

	WCSimWCDigitsCollection *DigitsCollection;

	WCSimDetectorConstruction *fDet;
	WCSimCHIPSPMT *fPMTSim;
	WCSimSK1pePMT *fSK1peSim;
	WCSimTOTPMT *fTOTSim;
};

#endif
