#ifndef WCSimDetectorMessenger_h
#define WCSimDetectorMessenger_h 1

class WCSimDetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithABool;

#include "G4UImessenger.hh"
#include "globals.hh"
#include "G4ios.hh"
#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithABool.hh"

class WCSimDetectorMessenger : public G4UImessenger
{
public:
	WCSimDetectorMessenger(WCSimDetectorConstruction *mpga);
	~WCSimDetectorMessenger();

public:
	void SetNewValue(G4UIcommand *command, G4String newValues);

private:
	WCSimDetectorConstruction *WCSimDetector;

private:
	//commands

	G4UIdirectory *WCSimDir;
	G4UIcmdWithAString *PMTConfig;
	G4UIcmdWithAString *PMTGeomDetails;
	G4UIcmdWithAString *PMTSize;
	G4UIcmdWithAString *SavePi0;
	G4UIcmdWithAString *PMTQEMethod;
	G4UIcmdWithAString *PMTCollEff;
	// Leigh: Add new parameter to decide which PMT simulation to use.
	// - "default" uses the standard one in WCSim
	// - "CHIPS" uses the more complex method based on the IceCube PMTs.
	G4UIcmdWithAString *PMTSim;

	// Andy: Add new parameter to decide whether we use the first or mean
	// photon hit time for the time recorded by the PMT
	G4UIcmdWithAString *PMTTime;

	// Andy: Flag to give the PMT perfect timing resolution (i.e. turn off time smearing)
	G4UIcmdWithABool *PMTPerfectTiming;

	G4UIcmdWithAString *tubeCmd;
	G4UIcmdWithAString *distortionCmd;
	G4UIcmdWithoutParameter *WCConstruct;
};

#endif
