#include "WCSimRunActionMessenger.hh"

#include "WCSimRunAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithAString.hh"

WCSimRunActionMessenger::WCSimRunActionMessenger(WCSimRunAction* WCSimRA)
:WCSimRun(WCSimRA)
{ 
  WCSimIODir = new G4UIdirectory("/WCSimIO/");
  WCSimIODir->SetGuidance("Commands to select I/O options");

  RootFile = new G4UIcmdWithAString("/WCSimIO/RootFile",this);
  RootFile->SetGuidance("Set the root file name");
  RootFile->SetGuidance("Enter the name of the output ROOT file");
  RootFile->SetParameterName("RootFileName",true);
  RootFile->SetDefaultValue("wcsim.root");
  
  SavePhotonNtuple = new G4UIcmdWithAString("/WCSimIO/SavePhotonNtuple",this);
  SavePhotonNtuple->SetGuidance("Save a tree with every optical photon track created");
  SavePhotonNtuple->SetGuidance("Enter 'true' to save photon information");
  SavePhotonNtuple->SetParameterName("SavePhotonNtuple",true);
  SavePhotonNtuple->SetDefaultValue("false");
  
  PhotonNtuple = new G4UIcmdWithAString("/WCSimIO/PhotonNtuple",this);
  PhotonNtuple->SetGuidance("Set the photon ntuple file name");
  PhotonNtuple->SetGuidance("Enter the name of the photon ntuple ROOT file");
  PhotonNtuple->SetParameterName("PhotonNtupleName",true);
  PhotonNtuple->SetDefaultValue("wcsim_photons.root");

}

WCSimRunActionMessenger::~WCSimRunActionMessenger()
{
  delete RootFile;
  delete SavePhotonNtuple;
  delete PhotonNtuple;
  delete WCSimIODir;
}

void WCSimRunActionMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{

  if ( command == RootFile)
  {
      WCSimRun->SetRootFileName(newValue);
      G4cout << "Output ROOT file set to " << newValue << G4endl;
  }
  if( command == SavePhotonNtuple )
  {
	  if (newValue=="true"){
	    WCSimRun->SetSavePhotonNtuple(true);
	  }else if (newValue == "false"){
	    WCSimRun->SetSavePhotonNtuple(false);
	  }else{
	    assert( newValue == "true" || newValue == "false" ); 
	  }
    G4cout << "Save photon ntuple set to " << newValue << G4endl;
  }
  if ( command == PhotonNtuple )
  {
      WCSimRun->SetPhotonNtupleName(newValue);
      G4cout << "Outut photon ntuple file set to " << newValue << G4endl;  
  }
}
