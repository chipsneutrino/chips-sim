#include "WCSimPhotonNtuple.hh"

#include "TFile.h"
#include "TTree.h"
#include "TDirectory.h"

#include <iostream>

static WCSimPhotonNtuple* fgNtuple = 0;

WCSimPhotonNtuple* WCSimPhotonNtuple::Instance()
{
  if( !fgNtuple ){
    fgNtuple = new WCSimPhotonNtuple();
  }

  return fgNtuple;
}

WCSimPhotonNtuple::WCSimPhotonNtuple() 
{
  fWCFile = 0;
  fWCTree = 0;  
  fWCFileName = "/unix/fnu/ajperch/numu_5mrad_LE_1000_photons.root";
  this->OpenFile(fWCFileName.Data());
  fHalfWidthZ = 0.0;
  fHalfWidthXY = 0.0;
}

WCSimPhotonNtuple::~WCSimPhotonNtuple()
{
  WCSimPhotonNtuple::Instance()->CloseFile();
}

void WCSimPhotonNtuple::Open(const char* filename)
{
  WCSimPhotonNtuple::Instance()->OpenFile(filename);
}

void WCSimPhotonNtuple::Close()
{
  WCSimPhotonNtuple::Instance()->CloseFile();
}

void WCSimPhotonNtuple::FileName(const char* filename)
{
  WCSimPhotonNtuple::Instance()->SetFileName(filename);
}

void WCSimPhotonNtuple::Fill( Int_t eventID, Int_t pdgCode, Int_t trackID, Int_t parentID, Int_t processID, Double_t energy, Double_t lambda, Bool_t opticalPhoton,  Bool_t scatteredPhoton, Double_t vtxX, Double_t vtxY, Double_t vtxZ, Double_t vtxTime, Double_t endX, Double_t endY, Double_t endZ, Double_t endTime, Double_t vtxdirX, Double_t vtxdirY, Double_t vtxdirZ ) 
{
  WCSimPhotonNtuple::Instance()->WriteEvent(eventID, pdgCode, trackID, parentID, processID, 
                                         energy, lambda, 
                                         opticalPhoton, scatteredPhoton,
                                         vtxX, vtxY, vtxZ, vtxTime,
                                         endX, endY, endZ, endTime,
				                             vtxdirX, vtxdirY, vtxdirZ );
}

void WCSimPhotonNtuple::WriteEvent( Int_t eventID, Int_t pdgCode, Int_t trackID, Int_t parentID, Int_t processID, Double_t energy, Double_t lambda, Bool_t opticalPhoton,  Bool_t scatteredPhoton, Double_t vtxX, Double_t vtxY, Double_t vtxZ, Double_t vtxTime, Double_t endX, Double_t endY, Double_t endZ, Double_t endTime, Double_t vtxdirX, Double_t vtxdirY, Double_t vtxdirZ ) 
{
  fEventID = eventID;
  fPdgCode = pdgCode;
  fTrackID = trackID;
  fParentID = parentID;
  fProcessID = processID;
  fEnergy = energy;
  fLambda = lambda;
  fOpticalPhoton = opticalPhoton;
  fScatteredPhoton = scatteredPhoton;
  fVtxX = vtxX;
  fVtxY = vtxY;
  fVtxZ = vtxZ;
  fVtxTime = vtxTime;
  fEndX = endX;
  fEndY = endY;
  fEndZ = endZ;
  fEndTime = endTime;

  fVtxDirX = vtxdirX;
  fVtxDirY = vtxdirY;
  fVtxDirZ = vtxdirZ;
  
  fIsDetected = 0;

  if( fOpticalPhoton && fParentID==1 && fProcessID == 2 ){ // Added the || 1 - AJP
    this->WriteEventToFile();
  }
}

void WCSimPhotonNtuple::WriteEventToFile()
{
   
  this->OpenFile(fWCFileName.Data());

  this->WriteToFile();

  return;
}

void WCSimPhotonNtuple::OpenFile(const char* filename)
{
  TDirectory* tmpd = 0;

  if( fWCFile==0 ){
    tmpd = gDirectory;
    std::cout << " opening file: " << filename << std::endl;
    fWCFile = new TFile(filename,"recreate");
    fWCTree = new TTree("ntuple","my analysis ntuple");
    fWCTree->Branch("eventID",&fEventID,"eventID/I");
    fWCTree->Branch("pdgCode",&fPdgCode,"pdgCode/I");
    fWCTree->Branch("trackID",&fTrackID,"trackID/I");
    fWCTree->Branch("parentID",&fParentID,"parentID/I");
    fWCTree->Branch("processID",&fProcessID,"processID/I");
    fWCTree->Branch("energy",&fEnergy,"energy/D");
    fWCTree->Branch("wavelength",&fLambda,"wavelength/D");
    fWCTree->Branch("optical",&fOpticalPhoton,"optical/I");
    fWCTree->Branch("scattered",&fScatteredPhoton,"scattered/I");
    fWCTree->Branch("vtxX",&fVtxX,"vtxX/D");
    fWCTree->Branch("vtxY",&fVtxY,"vtxY/D");
    fWCTree->Branch("vtxZ",&fVtxZ,"vtxZ/D");
    //fWCTree->Branch("vtxTime",&fVtxTime,"vtxTime/D");
    fWCTree->Branch("endX",&fEndX,"endX/D");
    fWCTree->Branch("endY",&fEndY,"endY/D");
    fWCTree->Branch("endZ",&fEndZ,"endZ/D");
//    fWCTree->Branch("endU",&fEndU,"endU/D");
//    fWCTree->Branch("endV",&fEndV,"endV/D");
//    //fWCTree->Branch("endTime",&fEndTime,"endTime/D");
    fWCTree->Branch("vtxdirX",&fVtxDirX,"vtxdirX/D");
    fWCTree->Branch("vtxdirY",&fVtxDirY,"vtxdirY/D");
    fWCTree->Branch("vtxdirZ",&fVtxDirZ,"vtxdirZ/D");
    //fWCTree->Branch("isDetected",&fIsDetected,"isDetected/I");
    gDirectory = tmpd;
  }

  return;
}

void WCSimPhotonNtuple::WriteToFile()
{
  TDirectory* tmpd = 0;

  if( fWCFile ){
    tmpd = gDirectory;
    fWCFile->cd();
    fWCTree->Fill();
    gDirectory = tmpd;
  }

  return;
}

void WCSimPhotonNtuple::CloseFile()
{
  TDirectory* tmpd = 0;

  if( fWCFile ){
    tmpd = gDirectory;
    std::cout << " closing file: " << fWCFile->GetName() << std::endl;
    fWCFile->cd();
    fWCTree->Write();
    fWCFile->Close();
    fWCTree = 0;
    fWCFile = 0;
    gDirectory = tmpd;
  }

  return;
}
