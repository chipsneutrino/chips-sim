#ifndef WCSimRunActionMessenger_h
#define WCSimRunActionMessenger_h 1

class WCSimTrackingAction;
class G4UIdirectory;
class G4UIcmdWithADouble;

#include "G4UImessenger.hh"
#include "globals.hh"

class WCSimTrackingActionMessenger : public G4UImessenger
{
public:
	WCSimTrackingActionMessenger(WCSimTrackingAction *mpga);
	~WCSimTrackingActionMessenger();

public:
	void SetNewValue(G4UIcommand *command, G4String newValues);

private:
	WCSimTrackingAction *fTrackingAction;

private:
	//commands
	G4UIdirectory *WCSimIODir;

	// How many Cherenkov photons should we save?
	// A double between 0 and 100.0 (i.e. %)
	G4UIcmdWithADouble *PercentCherenkovPhotonsToDraw;
};

#endif
