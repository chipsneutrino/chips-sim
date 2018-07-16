#include "WCSimPhotonNtuple.hh"

#include "TFile.h"
#include "TTree.h"
#include "TDirectory.h"

#include <cassert>
#include <iostream>

static WCSimPhotonNtuple* fgNtuple = 0;

WCSimPhotonNtuple* WCSimPhotonNtuple::Instance() {
	if (!fgNtuple) {
		fgNtuple = new WCSimPhotonNtuple();
	}

	return fgNtuple;
}

WCSimPhotonNtuple* WCSimPhotonNtuple::Instance(G4String str) {
	if (!fgNtuple) {
		fgNtuple = new WCSimPhotonNtuple(str);
	} else {
		std::cerr << "Photon ntuple already exists, cannot change the name" << std::endl;
		assert(!fgNtuple);
	}

	return fgNtuple;
}

WCSimPhotonNtuple::WCSimPhotonNtuple() {
	fWCFile = 0;
	fWCTree = 0;
	fWCFileName = "localfile_photons.root";
	this->OpenFile(fWCFileName.Data());
	fHalfWidthZ = 0.0;
	fHalfWidthXY = 0.0;
}

WCSimPhotonNtuple::WCSimPhotonNtuple(G4String str) {
	fWCFile = 0;
	fWCTree = 0;
	fWCFileName = str;
	this->OpenFile(fWCFileName.Data());
	fHalfWidthZ = 0.0;
	fHalfWidthXY = 0.0;
}

WCSimPhotonNtuple::~WCSimPhotonNtuple() {
	WCSimPhotonNtuple::Instance()->CloseFile();
}

void WCSimPhotonNtuple::Open(const char* filename) {
	WCSimPhotonNtuple::Instance()->OpenFile(filename);
}

void WCSimPhotonNtuple::Close() {
	WCSimPhotonNtuple::Instance()->CloseFile();
}

void WCSimPhotonNtuple::FileName(const char* filename) {
	WCSimPhotonNtuple::Instance()->SetFileName(filename);
}

void WCSimPhotonNtuple::Fill(Int_t eventID, Int_t pdgCode, Int_t trackID, Int_t parentID, Int_t processID,
		Float_t energy, Float_t momentum, Float_t lambda, Bool_t opticalPhoton, Bool_t scatteredPhoton, Float_t vtxX,
		Float_t vtxY, Float_t vtxZ, Float_t vtxTime, Float_t endX, Float_t endY, Float_t endZ, Float_t endTime,
		Float_t vtxdirX, Float_t vtxdirY, Float_t vtxdirZ) {
	WCSimPhotonNtuple::Instance()->WriteEvent(eventID, pdgCode, trackID, parentID, processID, energy, momentum, lambda,
			opticalPhoton, scatteredPhoton, vtxX, vtxY, vtxZ, vtxTime, endX, endY, endZ, endTime, vtxdirX, vtxdirY,
			vtxdirZ);
}

void WCSimPhotonNtuple::WriteEvent(Int_t eventID, Int_t pdgCode, Int_t trackID, Int_t parentID, Int_t processID,
		Float_t energy, Float_t momentum, Float_t lambda, Bool_t opticalPhoton, Bool_t scatteredPhoton, Float_t vtxX,
		Float_t vtxY, Float_t vtxZ, Float_t vtxTime, Float_t endX, Float_t endY, Float_t endZ, Float_t endTime,
		Float_t vtxdirX, Float_t vtxdirY, Float_t vtxdirZ) {
	fEventID = eventID;
	fPdgCode = pdgCode;
	fTrackID = trackID;
	fParentID = parentID;
	fProcessID = processID;
	fEnergy = energy;
	fMomentum = momentum;
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

	if (fOpticalPhoton && fProcessID == 2) { // Added the || 1 - AJP
		this->WriteEventToFile();
	}
}

void WCSimPhotonNtuple::WriteEventToFile() {

	this->OpenFile(fWCFileName.Data());

	this->WriteToFile();

	return;
}

void WCSimPhotonNtuple::OpenFile(const char* filename) {
	TDirectory* tmpd = 0;

	if (fWCFile == 0) {
		tmpd = gDirectory;
		std::cout << " opening file: " << filename << std::endl;
		fWCFile = new TFile(filename, "recreate");
		fWCTree = new TTree("ntuple", "my analysis ntuple");
		fWCTree->Branch("eventID", &fEventID, "eventID/I");
		fWCTree->Branch("pdgCode", &fPdgCode, "pdgCode/I");
		fWCTree->Branch("trackID", &fTrackID, "trackID/I");
		fWCTree->Branch("parentID", &fParentID, "parentID/I");
		fWCTree->Branch("processID", &fProcessID, "processID/I");
		fWCTree->Branch("energy", &fEnergy, "energy/F");
		fWCTree->Branch("momentum", &fMomentum, "momentum/F");
		fWCTree->Branch("wavelength", &fLambda, "wavelength/F");
		fWCTree->Branch("optical", &fOpticalPhoton, "optical/I");
		fWCTree->Branch("scattered", &fScatteredPhoton, "scattered/I");
		fWCTree->Branch("vtxX", &fVtxX, "vtxX/F");
		fWCTree->Branch("vtxY", &fVtxY, "vtxY/F");
		fWCTree->Branch("vtxZ", &fVtxZ, "vtxZ/F");
		//fWCTree->Branch("vtxTime",&fVtxTime,"vtxTime/F");
		fWCTree->Branch("endX", &fEndX, "endX/F");
		fWCTree->Branch("endY", &fEndY, "endY/F");
		fWCTree->Branch("endZ", &fEndZ, "endZ/F");
//    fWCTree->Branch("endU",&fEndU,"endU/F");
//    fWCTree->Branch("endV",&fEndV,"endV/F");
//    //fWCTree->Branch("endTime",&fEndTime,"endTime/F");
		fWCTree->Branch("vtxdirX", &fVtxDirX, "vtxdirX/F");
		fWCTree->Branch("vtxdirY", &fVtxDirY, "vtxdirY/F");
		fWCTree->Branch("vtxdirZ", &fVtxDirZ, "vtxdirZ/F");
		//fWCTree->Branch("isDetected",&fIsDetected,"isDetected/I");
		gDirectory = tmpd;
	}

	return;
}

void WCSimPhotonNtuple::WriteToFile() {
	TDirectory* tmpd = 0;

	if (fWCFile) {
		tmpd = gDirectory;
		fWCFile->cd();
		fWCTree->Fill();
		gDirectory = tmpd;
	}

	return;
}

void WCSimPhotonNtuple::CloseFile() {
	TDirectory* tmpd = 0;

	if (fWCFile) {
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
