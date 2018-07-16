#include "WCSimPrimaryGeneratorMessenger.hh"
#include "WCSimPrimaryGeneratorAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4ios.hh"

WCSimPrimaryGeneratorMessenger::WCSimPrimaryGeneratorMessenger(WCSimPrimaryGeneratorAction* pointerToAction) :
		myAction(pointerToAction) {
	mydetDirectory = new G4UIdirectory("/mygen/");
	mydetDirectory->SetGuidance("WCSim detector control commands.");

	genCmd = new G4UIcmdWithAString("/mygen/generator", this);
	genCmd->SetGuidance("Select primary generator.");
	//T. Akiri: Addition of laser
	genCmd->SetGuidance(" Available generators : muline, normal, laser, gps, overlay");
	genCmd->SetParameterName("generator", true);
	genCmd->SetDefaultValue("muline");
	//T. Akiri: Addition of laser
	genCmd->SetCandidates("muline normal laser gps overlay");

	fileNameCmd = new G4UIcmdWithAString("/mygen/vecfile", this);
	fileNameCmd->SetGuidance("Select the file of vectors.");
	fileNameCmd->SetGuidance(" Enter the file name of the vector file");
	fileNameCmd->SetParameterName("fileName", true);
	fileNameCmd->SetDefaultValue("inputvectorfile");

	fOverlayNameCmd = new G4UIcmdWithAString("/mygen/overlayfile", this);
	fOverlayNameCmd->SetGuidance("Select the .vec file to be used for overlays");
	fOverlayNameCmd->SetParameterName("overlayName", true);
	fOverlayNameCmd->SetDefaultValue("");

	fRandomVertexCmd = new G4UIcmdWithABool("/mygen/enableRandomVtx", this);
	fRandomVertexCmd->SetGuidance("Bool to toggle random vertices\n"
			" - The default value is false.\n"
			" - To limit the vertices away from the walls use the /mygen/fiducialDist \n");
	fRandomVertexCmd->SetParameterName("enableRandomVtx", true);
	fRandomVertexCmd->SetDefaultValue(false);

	fFiducialBorderCmd = new G4UIcmdWithADouble("/mygen/fiducialDist", this);
	fFiducialBorderCmd->SetGuidance("Distance from the wall to define the fiducial volume.\n"
			" - Requires /mygen/enableRandomVtx true in order to be used.\n"
			" - Defaults to 0.0m.\n"
			" - Units in m.");
	fFiducialBorderCmd->SetParameterName("fiducialDist", true);
	fFiducialBorderCmd->SetDefaultValue(0.0);

	fSwapXZCmd = new G4UIcmdWithABool("/mygen/useXAxisForBeam", this);
	fSwapXZCmd->SetGuidance("Bool to toggle using the X axis for the beam\n"
			" - Use when events are generated assuming beam in the Z direction (ie from GENIE).\n"
			" - Default value is true.");
	fSwapXZCmd->SetParameterName("useXAxisForBeam", true);
	fSwapXZCmd->SetDefaultValue(true);
}

WCSimPrimaryGeneratorMessenger::~WCSimPrimaryGeneratorMessenger() {
	delete genCmd;
	delete mydetDirectory;
}

void WCSimPrimaryGeneratorMessenger::SetNewValue(G4UIcommand * command, G4String newValue) {
	if (command == genCmd) {
		// If it is one of the allowed options then set everything to false.
		if (newValue == "muline" || newValue == "normal" || newValue == "laser" || newValue == "gps"
				|| newValue == "overlay") {
			myAction->SetMulineEvtGenerator(false);
			myAction->SetNormalEvtGenerator(false);
			myAction->SetLaserEvtGenerator(false);
			myAction->SetGpsEvtGenerator(false);
			myAction->SetOverlayEvtGenerator(false);
		}

		// Now set the correct option to true.
		if (newValue == "muline") {
			myAction->SetMulineEvtGenerator(true);
		} else if (newValue == "normal") {
			myAction->SetNormalEvtGenerator(true);
		} else if (newValue == "laser") {   //T. Akiri: Addition of laser{
			myAction->SetLaserEvtGenerator(true);
		} else if (newValue == "gps") {
			myAction->SetGpsEvtGenerator(true);
		} else if (newValue == "overlay") {
			myAction->SetOverlayEvtGenerator(true);
		}
	}
	// Vector file
	if (command == fileNameCmd) {
		myAction->AddVectorFile(newValue);
		G4cout << "Added new input vector file from " << newValue << G4endl;
	}
	// Overlay file
	if (command == fOverlayNameCmd) {
		myAction->AddOverlayFile(newValue);
		G4cout << "Added new overlay vector file from " << newValue << G4endl;
	}
	if (command == fRandomVertexCmd) {
		bool val = false;
		if (newValue == "true") {
			val = true;
		}
		myAction->SetRandomVertex(val);
	}
	// Fiducial distance
	if (command == fFiducialBorderCmd) {
		myAction->SetFiducialBorder(atof(newValue));
	}
	// Swap the X and Z coordinates of the vec file particles
	if (command == fSwapXZCmd) {
		bool val = true;
		if (newValue == "false") {
			val = false;
		}
		myAction->SetUseXAxisForBeam(val);
	}
}

G4String WCSimPrimaryGeneratorMessenger::GetCurrentValue(G4UIcommand* command) {
	G4String cv;

	if (command == genCmd) {
		if (myAction->IsUsingMulineEvtGenerator()) {
			cv = "muline";
		} else if (myAction->IsUsingNormalEvtGenerator()) {
			cv = "normal";
		} else if (myAction->IsUsingLaserEvtGenerator()) {
			cv = "laser";
		}   //T. Akiri: Addition of laser
		else if (myAction->IsUsingGpsEvtGenerator()) {
			cv = "gps";
		} else if (myAction->IsUsingOverlayEvtGenerator()) {
			cv = "overlay";
		}
	}

	return cv;
}

