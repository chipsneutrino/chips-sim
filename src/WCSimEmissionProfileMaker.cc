/*
 * WCSimEmissionProfileMaker.cc
 *
 *  Created on: 5 May 2015
 *      Author: ajperch
 */

#include "G4TrajectoryContainer.hh"
#include "globals.hh"

#include "WCSimEmissionProfileMaker.hh"
#include "WCSimTrajectory.hh"
#include "WCSimTruthSummary.hh"

#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include "TTree.h"

#include <cassert>
#include <numeric>
#include <vector>

static WCSimEmissionProfileMaker* fgEmissionProfile = 0x0;

WCSimEmissionProfileMaker::WCSimEmissionProfileMaker( G4String filename) : fRho(0x0), fGFine(0x0), fGCoarse(0x0)
{
	// TODO Auto-generated constructor stub
    fNumEvents = 0;
    fNumPhotons = 0;
    fPhotonTree = 0x0;

	  fSaveFile = 0x0;
	  fSaveFileName = filename;
	  this->OpenFile(fSaveFileName.c_str());
}

WCSimEmissionProfileMaker::~WCSimEmissionProfileMaker()
{
	WCSimEmissionProfileMaker::Instance()->CloseFile();
	// TODO Auto-generated destructor stub
}


WCSimEmissionProfileMaker* WCSimEmissionProfileMaker::Instance(G4String str)
{
  if( fgEmissionProfile == 0x0){
	  fgEmissionProfile = new WCSimEmissionProfileMaker(str);
  }
  else{
    std::cerr << "Emission profile file already exists, cannot change the name" << std::endl;
    assert( !fgEmissionProfile );
  }

  return fgEmissionProfile;
}

WCSimEmissionProfileMaker* WCSimEmissionProfileMaker::Instance()
{
  if( !fgEmissionProfile ){
    fgEmissionProfile = new WCSimEmissionProfileMaker();
  }

  return fgEmissionProfile;
}

void WCSimEmissionProfileMaker::SetThetaBins()
{
	fThetaCoarseMin = -1.0;
	fThetaCoarseMax = -0.5;
	fNBinsThetaCoarse = 500; // 0.001 per bin

	fThetaFineMin = fThetaCoarseMax;
	fThetaFineMax = 1.0;
	fNBinsThetaFine = 1500; // 0.001 per bin

}

void WCSimEmissionProfileMaker::SetSBins()
{
	fSMin = 0.0;
	fSMax = 3250;
	fNBinsS = 1300;
}

void WCSimEmissionProfileMaker::Open(const char* filename)
{
	WCSimEmissionProfileMaker::Instance()->OpenFile(filename);
}

void WCSimEmissionProfileMaker::Close()
{
	WCSimEmissionProfileMaker::Instance()->CloseFile();
}

void WCSimEmissionProfileMaker::FileName(const char* filename)
{
	WCSimEmissionProfileMaker::Instance()->SetFileName(filename);
}

void WCSimEmissionProfileMaker::OpenFile(const char* filename)
{
	  TDirectory* tmpd = 0;

	  if( fSaveFile==0 ){
	    tmpd = gDirectory;
	    std::cout << " opening file: " << filename << std::endl;
	    fSaveFile = new TFile(filename,"recreate");
      fPhotonTree = new TTree("fPhotonTree","fPhotonTree");
      fPhotonTree->Branch("fNumEvents",&fNumEvents);
      fPhotonTree->Branch("fNumPhotons", &fNumPhotons);
	    SetThetaBins();
	    SetSBins();
	    MakeHistograms();
	    gDirectory = tmpd;
	  }

	  return;
}

void WCSimEmissionProfileMaker::CloseFile()
{
	  TDirectory* tmpd = 0;

	  if( fSaveFile ){
	    tmpd = gDirectory;
	    std::cout << " closing file: " << fSaveFile->GetName() << std::endl;
	    NormaliseHistograms();
	    fSaveFile->cd();
	    fRho->Write();
	    fGFine->Write();
	    fGCoarse->Write();
	    fS->Write();
	    fSCosThetaCoarse->Write();
	    fSCosThetaFine->Write();
      fPhotonTree->Write();
      fWavelengths->Write();
      fSSecTheta->Write();
      fSCosThetaCheckBinning->Write();
	    fSaveFile->Close();

	    gDirectory = tmpd;
	  }

	  return;
}

void WCSimEmissionProfileMaker::Fill(G4TrajectoryContainer * trajCont, WCSimTruthSummary * truth)
{
	WCSimEmissionProfileMaker::Instance()->FillEvent(trajCont, truth);
}

void WCSimEmissionProfileMaker::FillEvent(G4TrajectoryContainer * trajCont, WCSimTruthSummary * truth)
{
  std::cout << "Filling event " << fNumEvents << std::endl;
  fNumPhotons = 0;
	const float mm_to_cm = 0.1;
	int nTraj = trajCont->size();
	int primaryPDG = truth->GetBeamPDG();
	TVector3 primaryDir = truth->GetBeamDir().Unit();
	TVector3 primaryVtx = truth->GetVertex() * mm_to_cm;

	fPhotonDirections.clear();
	fPhotonVertices.clear();
	for(int i = 0; i < nTraj; ++i)
	{
		WCSimTrajectory* trj = (WCSimTrajectory*)( (*trajCont)[i] );

		// We only want optical photons and if the primary particle was a muon we want them to have the muon as their parent
		if(trj->IsOpticalPhoton() && !trj->IsScatteredPhoton() && (abs(primaryPDG) != 13 || trj->GetParentID() == 1) )
		{
			fPhotonDirections.push_back(TVector3(trj->GetVtxDirX(), trj->GetVtxDirY(), trj->GetVtxDirZ()));
			fPhotonVertices.push_back(TVector3(trj->GetVtxX(), trj->GetVtxY(), trj->GetVtxZ()) * (mm_to_cm));
    
      double scale = (joule/MeV) * (nm/m) * (1.0/(h_Planck*c_light));
      float wavelength = ((h_Planck*c_light) / (trj->GetEnergy())) / nm;
      fWavelengths->Fill(wavelength);
		}
	}

	TVector3 meanDirection = AverageVector(fPhotonVertices).Unit();
  std::cout << "Mean direction:" << std::endl;
  meanDirection.Print();

	// Now loop through and fill the histograms
	assert(fPhotonDirections.size() == fPhotonVertices.size());
	std::vector<TVector3>::const_iterator vtxItr = fPhotonVertices.begin();
	std::vector<TVector3>::const_iterator dirItr = fPhotonDirections.begin();
  std::cout << "Primary vtx = " << std::endl;
  primaryVtx.Print();
	while(vtxItr != fPhotonVertices.end() && dirItr != fPhotonDirections.end())
	{
		float distance = (*vtxItr).Z();
		float cosDirection = TMath::Cos((*dirItr).Angle(meanDirection));

		fS->Fill(distance);
		if( cosDirection < fSCosThetaFine->GetXaxis()->GetXmin())
		{
			fSCosThetaCoarse->Fill(cosDirection, distance);
		}
		else
		{
			fSCosThetaFine->Fill(cosDirection, distance);
		}

    if( fabs(cosDirection) > 0.01 )
    {
      double secTheta = 1.0/cosDirection;
      if(secTheta > 100 || secTheta < -100){
        std::cout << "cosDirection = " << cosDirection << " and secTheta = " << secTheta << std::endl;
      }
      fSSecTheta->Fill(1.0/cosDirection, distance);
      fSCosThetaCheckBinning->Fill(cosDirection, distance);
    }
    else
    {
      fSSecTheta->Fill(fSSecTheta->GetXaxis()->GetXmax() + 1, distance);
      fSCosThetaCheckBinning->Fill(2, distance);
    }

		++vtxItr;
		++dirItr;
    fNumPhotons++;
	}
  fNumEvents++;
}

void WCSimEmissionProfileMaker::NormaliseHistograms()
{
  SmoothRho();
  SmoothG();
	NormaliseRho();
	NormaliseG();
	return;

}

void WCSimEmissionProfileMaker::NormaliseRho()
{

	float normFact = 1.0 / fS->Integral("width");
	assert(!isinf(normFact));
	for(int i = 1; i <= fS->GetNbinsX(); ++i)
	{
		fRho->SetBinContent(i, fS->GetBinContent(i) * normFact);
	}
	return;
}

void WCSimEmissionProfileMaker::NormaliseG()
{
	// Rows are s bins, columns are theta bins
	float minEntries = 0.0;
	assert(fSCosThetaCoarse->GetYaxis()->GetNbins() == fSCosThetaFine->GetYaxis()->GetNbins());
	for(int iRow = 1; iRow <= fSCosThetaCoarse->GetYaxis()->GetNbins(); ++iRow)
	{
		double sumTheta = 0.0;  // Sum over theta bins weighted by their width
		double sumWidth = 0.0;
    for(int iCol = 1; iCol <= fSCosThetaCoarse->GetXaxis()->GetNbins(); ++iCol)
		{
			if(fSCosThetaCoarse->GetBinContent(iCol, iRow) > 0 && fSCosThetaCoarse->GetBinContent(iCol,iRow) < minEntries)
			{
				fSCosThetaCoarse->SetBinContent(iCol, iRow, 0);
			}
			sumTheta += fSCosThetaCoarse->GetBinContent(iCol, iRow) / fSCosThetaCoarse->GetXaxis()->GetBinWidth(iCol);
			sumWidth += fSCosThetaCoarse->GetXaxis()->GetBinWidth(iCol);
		}
		for(int iCol = 1; iCol <= fSCosThetaFine->GetXaxis()->GetNbins(); ++iCol)
		{
			if(fSCosThetaFine->GetBinContent(iCol, iRow) > 0 && fSCosThetaFine->GetBinContent(iCol,iRow) < minEntries)
			{
				fSCosThetaFine->SetBinContent(iCol, iRow, 0);
			}
			sumTheta += fSCosThetaFine->GetBinContent(iCol, iRow) / fSCosThetaFine->GetXaxis()->GetBinWidth(iCol);
			sumWidth += fSCosThetaFine->GetXaxis()->GetBinWidth(iCol);
		}

		// Now we have the totals we can set the bins in the emission profiles
		for(int jCol = 1; jCol <= fSCosThetaCoarse->GetXaxis()->GetNbins(); ++jCol)
		{
			if(fSCosThetaCoarse->GetBinContent(jCol, iRow) > 0)
			{
				fGCoarse->SetBinContent(jCol, iRow, (fSCosThetaCoarse->GetBinContent(jCol, iRow) /
													fSCosThetaCoarse->GetXaxis()->GetBinWidth(jCol))
													/ (sumTheta*sumWidth));
			}
		}
		for(int jCol = 1; jCol <= fSCosThetaFine->GetXaxis()->GetNbins(); ++jCol)
		{
			if(fSCosThetaFine->GetBinContent(jCol, iRow) > 0)
			{

				fGFine->SetBinContent(jCol, iRow, fSCosThetaFine->GetBinContent(jCol,iRow) /
												  fSCosThetaFine->GetXaxis()->GetBinWidth(jCol)
												  / (sumTheta*sumWidth));
			}
		}
	}
}

void WCSimEmissionProfileMaker::MakeHistograms()
{
	fRho = new TH1F("fRho","",fNBinsS, fSMin, fSMax);
	fGFine = new TH2F("fGFine","",fNBinsThetaFine, fThetaFineMin, fThetaFineMax, fNBinsS, fSMin, fSMax);
	fGCoarse = new TH2F("fGCoarse","",fNBinsThetaCoarse, fThetaCoarseMin, fThetaCoarseMax, fNBinsS, fSMin, fSMax);
	fS = new TH1F("fS","",fNBinsS, fSMin, fSMax);
	fSCosThetaFine = new TH2F("fSCosThetaFine","",fNBinsThetaFine, fThetaFineMin, fThetaFineMax, fNBinsS, fSMin, fSMax);
	fSCosThetaCoarse = new TH2F("fSCosThetaCoarse","",fNBinsThetaCoarse, fThetaCoarseMin, fThetaCoarseMax, fNBinsS, fSMin, fSMax);
  fWavelengths = new TH1F("fWavelengths",";Wavelength (nm); Photons",1000,0,1000);


  const int nSecBins = 41; // Need an odd number of bins to keep away from having 1/cos(pi/2) at a bin boundart
  double cosThetaBins[nSecBins+1] = { -1.0, -0.75, -0.5, -0.25, -0.01, 0.01,
                                    0.125, 0.250, 0.375, 0.50,
                                    0.533, 0.567, 0.60, 0.633, 0.667, 0.7,
                                    0.705, 0.71, 0.715, 0.72, 0.725, 0.73, 0.735, 0.74, 0.745, 0.75,
                                    0.755, 0.76, 0.765, 0.77, 0.775, 0.78, 0.785, 0.79, 0.795, 0.80,
                                    0.833, 0.867, 0.9, 0.933, 0.967, 1.0 };
  

  double secBins[nSecBins+1];
  for(int iCosTheta = 0; iCosTheta <= nSecBins; ++iCosTheta)
  {
    secBins[iCosTheta] = 1.0/cosThetaBins[iCosTheta];
  }
  std::sort(secBins, secBins+nSecBins+1);
  fSSecTheta = new TH2F("fSSecTheta",";sec#theta;s (cm);Photons",nSecBins, secBins, 40, fSMin, fSMax); 
  fSCosThetaCheckBinning = new TH2F("fSCosThetaCheckBinning",";cos#theta;s (cm);photons", nSecBins, cosThetaBins, 40, fSMin, fSMax);
}

template <class T>
T WCSimEmissionProfileMaker::AverageVector(const std::vector<T> &vec)
{
	if(vec.size() == 0) { return T(); }
	T total = std::accumulate(vec.begin(), vec.end(), T());
	return total * (1.0/vec.size());
}

void WCSimEmissionProfileMaker::SmoothRho()
{
  fS->Smooth(1);
}

void WCSimEmissionProfileMaker::SmoothG()
{
  fSCosThetaCoarse->Smooth(1);
  fSCosThetaFine->Smooth(1);
}
