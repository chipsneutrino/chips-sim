#ifndef WCSimPhotonNtuple_h
#define WCSimPhotonNtuple_h


#include "TString.h"
#include "G4String.hh"

class TFile;
class TTree;

class WCSimPhotonNtuple
{      
 public :
            
  WCSimPhotonNtuple();         
  WCSimPhotonNtuple( G4String str );         
  ~WCSimPhotonNtuple();
     
  static WCSimPhotonNtuple* Instance();  
  static WCSimPhotonNtuple* Instance( G4String str );  
  
  static void Open(const char* filename);
  static void Close();
  static void Fill(Int_t eventID,
                   Int_t pdgCode, Int_t trackID, Int_t parentID, Int_t processID,
                   Double_t energy, Double_t lambda,
                   Bool_t opticalPhoton,  Bool_t scatteredPhoton,
                   Double_t vtxX, Double_t vtxY, Double_t vtxZ, Double_t vtxTime,
                   Double_t endX, Double_t endY, Double_t endZ, Double_t endTime,
                   Double_t vtxdirX, Double_t vtxdirY, Double_t vtxdirZ 
);
  static void FileName(const char* filename);

  void SetFileName(const char* filename) { fWCFileName = filename; }
  void OpenFile(const char* filename);
  void CloseFile();

  void WriteEvent( Int_t eventID,
                   Int_t pdgCode, Int_t trackID, Int_t parentID, Int_t processID,
                   Double_t energy, Double_t lambda,
                   Bool_t opticalPhoton,  Bool_t scatteredPhoton,
                   Double_t vtxX, Double_t vtxY, Double_t vtxZ, Double_t vtxTime,
                   Double_t endX, Double_t endY, Double_t endZ, Double_t endTime,
                   Double_t vtxdirX, Double_t vtxdirY, Double_t vtxdirZ );

  void SetGeometry( Double_t halfWidthXY, Double_t halfWidthZ ) {
    fHalfWidthXY = halfWidthXY; fHalfWidthZ = halfWidthZ;
  }

 private :
    
  void WriteEventToFile();
  void WriteToFile();
  

  TFile* fWCFile;
  TTree* fWCTree;
  TString fWCFileName;

  Int_t fEventID;
  Int_t fPdgCode;
  Int_t fTrackID;
  Int_t fParentID;
  Int_t fProcessID;
  Double_t fEnergy; 
  Double_t fLambda;
  Int_t fOpticalPhoton;
  Int_t fScatteredPhoton;
  Double_t fVtxX;
  Double_t fVtxY;
  Double_t fVtxZ;
  Double_t fVtxTime;
  Double_t fVtxDirX; 
  Double_t fVtxDirY; 
  Double_t fVtxDirZ;  
  Double_t fEndX;
  Double_t fEndY;
  Double_t fEndZ;
  Double_t fEndTime;
  
  Int_t fIsDetected;
  Double_t fEndU;
  Double_t fEndV;

  Double_t fHalfWidthXY;
  Double_t fHalfWidthZ;
         
} ;


#endif
