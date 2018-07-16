#ifndef WCSimRunActionMessenger_h
#define WCSimRunActionMessenger_h 1

class WCSimRunAction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithABool;
#include "G4UImessenger.hh"
#include "globals.hh"

class WCSimRunActionMessenger: public G4UImessenger {
	public:
		WCSimRunActionMessenger(WCSimRunAction* mpga);
		~WCSimRunActionMessenger();

	public:
		void SetNewValue(G4UIcommand* command, G4String newValues);

	private:
		WCSimRunAction* WCSimRun;

	private:
		//commands
		G4UIdirectory* WCSimIODir;
		G4UIcmdWithABool* SaveRootFile;
		G4UIcmdWithAString* RootFile;
		G4UIcmdWithABool* SavePhotonNtuple;
		G4UIcmdWithAString* PhotonNtuple;
		G4UIcmdWithABool* SaveEmissionProfile;
		G4UIcmdWithAString* EmissionProfile;
};

#endif

