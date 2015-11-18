#include "WCSimRunActionMessenger.hh"

#include "WCSimRunAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"


WCSimRunActionMessenger::WCSimRunActionMessenger(WCSimRunAction* WCSimRA)
:WCSimRun(WCSimRA)
{ 
  WCSimIODir = new G4UIdirectory("/WCSimIO/");
  WCSimIODir->SetGuidance("Commands to select I/O options");

  SaveRootFile = new G4UIcmdWithABool("/WCSimIO/SaveRootFile",this);
  SaveRootFile->SetGuidance("Save a tree with event information");
  SaveRootFile->SetGuidance("Enter 'true' to save events information");
  SaveRootFile->SetParameterName("SaveRootFile",true);
  SaveRootFile->SetDefaultValue(false);

  RootFile = new G4UIcmdWithAString("/WCSimIO/RootFile",this);
  RootFile->SetGuidance("Set the root file name");
  RootFile->SetGuidance("Enter the name of the output ROOT file");
  RootFile->SetParameterName("RootFileName",true);
  RootFile->SetDefaultValue("wcsim.root");
  
  SavePhotonNtuple = new G4UIcmdWithABool("/WCSimIO/SavePhotonNtuple",this);
  SavePhotonNtuple->SetGuidance("Save a tree with every optical photon track created");
  SavePhotonNtuple->SetGuidance("Enter 'true' to save photon information");
  SavePhotonNtuple->SetParameterName("SavePhotonNtuple",true);
  SavePhotonNtuple->SetDefaultValue(false);
  
  PhotonNtuple = new G4UIcmdWithAString("/WCSimIO/PhotonNtuple",this);
  PhotonNtuple->SetGuidance("Set the photon ntuple file name");
  PhotonNtuple->SetGuidance("Enter the name of the photon ntuple ROOT file");
  PhotonNtuple->SetParameterName("PhotonNtupleName",true);
  PhotonNtuple->SetDefaultValue("wcsim_photons.root");

  SaveEmissionProfile = new G4UIcmdWithABool("/WCSimIO/SaveEmissionProfile",this);
  SaveEmissionProfile->SetGuidance("Save only the information needed to build emission profiles");
  SaveEmissionProfile->SetGuidance("Enter 'true' to save emission profile information only");
  SaveEmissionProfile->SetParameterName("SaveEmissionProfile",true);
  SaveEmissionProfile->SetDefaultValue(false);

  EmissionProfile = new G4UIcmdWithAString("/WCSimIO/EmissionProfile",this);
  EmissionProfile->SetGuidance("Set the emission profile file name");
  EmissionProfile->SetGuidance("Enter the name of the emission profile ROOT file");
  EmissionProfile->SetParameterName("EmissionProfileName",true);
  EmissionProfile->SetDefaultValue("wcsim_emission_profile.root");

}

WCSimRunActionMessenger::~WCSimRunActionMessenger()
{
  delete SaveRootFile;
  delete RootFile;
  delete SavePhotonNtuple;
  delete PhotonNtuple;
  delete SaveEmissionProfile;
  delete EmissionProfile;
  delete WCSimIODir;

}

void WCSimRunActionMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
  if( command == SaveRootFile )
  {
	  WCSimRun->SetSaveRootFile(SaveRootFile->GetNewBoolValue(newValue));
	  G4cout << "Save ROOT file set to " << newValue << std::endl;
  }
  if ( command == RootFile)
  {
      WCSimRun->SetRootFileName(newValue);
      G4cout << "Output ROOT file set to " << newValue << G4endl;
  }
  if( command == SavePhotonNtuple )
    {
        WCSimRun->SetSavePhotonNtuple(SavePhotonNtuple->GetNewBoolValue(newValue));
        G4cout << "Save photon ntuple set to " << newValue << G4endl;
    }
    if ( command == PhotonNtuple )
    {
        WCSimRun->SetPhotonNtupleName(newValue);
        G4cout << "Outut photon ntuple file set to " << newValue << G4endl;
    }
    if( command == SaveEmissionProfile )
    {
    	WCSimRun->SetSaveEmissionProfile(SaveEmissionProfile->GetNewBoolValue(newValue));
    	G4cout << "Save emission profile set to " << newValue << G4endl;
    }
    if ( command == EmissionProfile )
    {
    	WCSimRun->SetEmissionProfileName(newValue);
    	G4cout << "Outut emission profiel file basename set to " << newValue << G4endl;
    }

}
