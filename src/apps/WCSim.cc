#include "G4ios.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#include "WCSimCherenkovBuilder.hh"
#include "WCSimPhysicsListFactory.hh"
#include "WCSimPhysicsListFactoryMessenger.hh"
#include "WCSimTuningParameters.hh"
#include "WCSimTuningMessenger.hh"
#include "WCSimPrimaryGeneratorAction.hh"
#include "WCSimEventAction.hh"
#include "WCSimRunAction.hh"
#include "WCSimStackingAction.hh"
#include "WCSimTrackingAction.hh"
#include "WCSimSteppingAction.hh"
#include "WCSimVisManager.hh"
#include "WCSimRandomParameters.hh"
#include <iostream>
#include <cstring>
#include <string>

void usage();

int main(int argc, char **argv)
{
	// Setup the paths to the default files
	G4String geoMacFile = getenv("WCSIMHOME");
	geoMacFile.append("/config/example/example_geo_setup.mac");

	G4String macFile = getenv("WCSIMHOME");
	macFile.append("/config/example/example.mac");

	G4String tuningFile = getenv("WCSIMHOME");
	tuningFile.append("/config/tuning_parameters.mac");

	G4String jobOptionsFile = getenv("WCSIMHOME");
	jobOptionsFile.append("/config/job_options.mac");

	if (argc > 1)
	{
		for (int i = 1; i < argc; ++i)
		{
			std::string dash("-");

			// File name switch
			if (std::strcmp(argv[i], "-g") == 0)
			{
				if (argc >= i + 1)
				{
					geoMacFile = argv[i + 1];
					std::cout << "== Geometry mac file = " << argv[i + 1] << std::endl;
					++i;
				}
				else
				{
					std::cerr << "== Flag -g expects an argument == " << std::endl;
					usage();
					return 0;
				}
			}
			else if (!G4String(argv[i]).compare(0, dash.size(), dash))
			{
				std::cerr << "Unrecognised flag " << argv[i] << std::endl;
				usage();
				return 0;
			}
			else
			{
				macFile = argv[i];
				std::cout << "Setting macfile to " << macFile << std::endl;
			}
		}
	}
	std::cout << "Geometry macfile is " << geoMacFile << std::endl;
	std::cout << "Run configuration macfile is " << macFile << std::endl;

	// Setup 'execute' command to use with the files
	G4String execute = "/control/execute ";

	// Construct the run manager
	G4RunManager *runManager = new G4RunManager;

	// get the pointer to the UI manager
	G4UImanager *UI = G4UImanager::GetUIpointer();

	// Get the tuning parameters from file
	WCSimTuningParameters::Instance();
	UI->ApplyCommand(execute + tuningFile);

	// Setup the detector geometry and register with the run manager
	WCSimCherenkovBuilder *WCSimdetector = new WCSimCherenkovBuilder(2);
	runManager->SetUserInitialization(WCSimdetector);
	UI->ApplyCommand(execute + geoMacFile);

	// Setup the physics list, initialize and register with the run manager
	WCSimPhysicsListFactory *WCSimPhysics = new WCSimPhysicsListFactory();
	UI->ApplyCommand(execute + jobOptionsFile);
	WCSimPhysics->InitializeList();
	runManager->SetUserInitialization(WCSimPhysics);

	// Setup and initialise the Visualization manager
	G4VisManager *visManager = new WCSimVisManager;
	visManager->Initialize();

	// Set the WCSim user action classes
	WCSimPrimaryGeneratorAction *myGeneratorAction = new WCSimPrimaryGeneratorAction(WCSimdetector);
	runManager->SetUserAction(myGeneratorAction);

	WCSimRunAction *myRunAction = new WCSimRunAction(WCSimdetector);
	runManager->SetUserAction(myRunAction);

	runManager->SetUserAction(new WCSimEventAction(myRunAction, WCSimdetector, myGeneratorAction));
	runManager->SetUserAction(new WCSimTrackingAction);
	runManager->SetUserAction(new WCSimStackingAction(WCSimdetector));
	runManager->SetUserAction(new WCSimSteppingAction);

	runManager->Initialize(); // Initialize G4 kernel, this actually sets everything up

	if (macFile.contains("vis")) // Define UI terminal for interactive mode
	{
		std::cout << "Using a visualisation macro..." << std::endl;
		G4UIsession *session = new G4UIterminal(new G4UItcsh); // Start UI Session
		UI->ApplyCommand(execute + macFile);				   // Visualisation Macro
		session->SessionStart();							   // Start Interactive Mode
		delete session;
	}
	else // Normal batch mode
	{
		std::cout << "Using a batch macro..." << std::endl;
		UI->ApplyCommand(execute + macFile);
	}

	delete visManager;
	delete runManager; // This will delete everything registered with it aswell
	return 0;
}

void usage()
{
	std::cout << "--- WCSim usage instructions ---" << std::endl;
	std::cout << "WCSim [-g arg] path_to_some_file.mac (where the mac file contains the GEANT4 run configuration)"
			  << std::endl;
	std::cout << " -- Optional flags -- " << std::endl;
	std::cout << "   -g path_to_geo_file.mac" << std::endl
			  << "       Use a different mac file to specify geometry options" << std::endl
			  << "       such as which geometry or PMT simulation to use (default " << std::endl
			  << "       is $WCSIMHOME/config/example/example_geo_setup.mac" << std::endl;
}
