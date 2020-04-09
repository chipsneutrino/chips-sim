#pragma once

class WCSimPrimaryGeneratorAction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithABool;
class G4UIcmdWithADouble;

#include "G4UImessenger.hh"
#include "globals.hh"

class WCSimPrimaryGeneratorMessenger : public G4UImessenger
{
public:
	WCSimPrimaryGeneratorMessenger(WCSimPrimaryGeneratorAction *mpga);
	~WCSimPrimaryGeneratorMessenger();

public:
	void SetNewValue(G4UIcommand *command, G4String newValues);
	G4String GetCurrentValue(G4UIcommand *command);

private:
	WCSimPrimaryGeneratorAction *myAction;

private:
	//commands
	G4UIdirectory *mydetDirectory;
	G4UIcmdWithAString *genCmd;
	G4UIcmdWithAString *fileNameCmd;
	// Need a second vec file for overlaid events
	G4UIcmdWithAString *fOverlayNameCmd;
	// Option to enable random vertex positions
	G4UIcmdWithABool *fRandomVertexCmd;
	// Define the size of the gap to the wall - default = 1m.
	G4UIcmdWithADouble *fFiducialBorderCmd;
	// Toggle to swap X and Z for beam events generated along Z (ie with GENIE).
	G4UIcmdWithABool *fSwapXZCmd;
};
