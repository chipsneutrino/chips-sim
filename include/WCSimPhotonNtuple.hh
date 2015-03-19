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
                   Float_t energy, Float_t lambda,
                   Bool_t opticalPhoton,  Bool_t scatteredPhoton,
                   Float_t vtxX, Float_t vtxY, Float_t vtxZ, Float_t vtxTime,
                   Float_t endX, Float_t endY, Float_t endZ, Float_t endTime,
                   Float_t vtxdirX, Float_t vtxdirY, Float_t vtxdirZ 
);
  static void FileName(const char* filename);

  void SetFileName(const char* filename) { fWCFileName = filename; }
  void OpenFile(const char* filename);
  void CloseFile();

  void WriteEvent( Int_t eventID,
                   Int_t pdgCode, Int_t trackID, Int_t parentID, Int_t processID,
                   Float_t energy, Float_t lambda,
                   Bool_t opticalPhoton,  Bool_t scatteredPhoton,
                   Float_t vtxX, Float_t vtxY, Float_t vtxZ, Float_t vtxTime,
                   Float_t endX, Float_t endY, Float_t endZ, Float_t endTime,
                   Float_t vtxdirX, Float_t vtxdirY, Float_t vtxdirZ );

  void SetGeometry( Float_t halfWidthXY, Float_t halfWidthZ ) {
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
  Float_t fEnergy; 
  Float_t fLambda;
  Int_t fOpticalPhoton;
  Int_t fScatteredPhoton;
  Float_t fVtxX;
  Float_t fVtxY;
  Float_t fVtxZ;
  Float_t fVtxTime;
  Float_t fVtxDirX; 
  Float_t fVtxDirY; 
  Float_t fVtxDirZ;  
  Float_t fEndX;
  Float_t fEndY;
  Float_t fEndZ;
  Float_t fEndTime;
  
  Int_t fIsDetected;
  Float_t fEndU;
  Float_t fEndV;

  Float_t fHalfWidthXY;
  Float_t fHalfWidthZ;
         
} ;


#endif
