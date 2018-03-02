#include "G4ios.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#include "WCSimCherenkovBuilder.hh"
#include "WCSimPhysicsList.hh"
#include "WCSimPhysicsMessenger.hh"
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

int main(int argc,char** argv)
{
  // Check to see if we need to use a custom geometry setup macfile
  // Means we can submit batch jobs with different geometries concurrently
  G4String geoMacFile("example/geoSetup.mac");
  G4String macFile("example/generic_chips.mac");
  if(argc > 1){
  	for(int i = 1; i < argc; ++i){
      std::string dash("-");

  	  // File name switch 
  	  if(std::strcmp(argv[i],"-g") == 0){
        if(argc >= i+1){
  	      geoMacFile = argv[i+1];
  	  	std::cout << "== Geometry mac file = " << argv[i+1] << std::endl;
  	  	++i;
  	    }
        else
        {
            std::cerr << "== Flag -g expects an argument == " << std::endl;
            usage();
            return 0;
        }
      }
      else if (!G4String(argv[i]).compare(0, dash.size(), dash)){
        std::cerr << "Unregcognised flag " << argv[i] << std::endl;
        usage();
        return 0;
      }       
      else{ 
        macFile = argv[i];
        std::cout << "Setting macfile to " << macFile << std::endl;
  	  }
  	}
  }
  std::cout << "Geometry macfile is " << geoMacFile << std::endl;
  std::cout << "Run config macfile is " << macFile << std::endl;

  // Construct the default run manager
  G4RunManager* runManager = new G4RunManager;

  // get the pointer to the UI manager
  G4UImanager* UI = G4UImanager::GetUIpointer();

  // Get the tuning parameters
  WCSimTuningParameters::Instance();
  UI->ApplyCommand("/control/execute config/tuning_parameters.mac");

  // define random number generator parameters
  WCSimRandomParameters *randomparameters = new WCSimRandomParameters();

  // UserInitialization classes (mandatory)
  enum DetConfiguration {wfm=1,fwm=2};
  G4int WCSimConfiguration = fwm;

  WCSimCherenkovBuilder* WCSimdetector = new 
  WCSimCherenkovBuilder(WCSimConfiguration);

  runManager->SetUserInitialization(WCSimdetector);
  G4String geoCommand = "/control/execute ";
  UI->ApplyCommand(geoCommand + geoMacFile);

  // Added selectable physics lists 2010-07 by DMW
  // Set up the messenger hooks here, initialize the actual list after loading jobOptions.mac
  WCSimPhysicsListFactory *physFactory = new WCSimPhysicsListFactory();

  // Currently, default model is set to BINARY
  UI->ApplyCommand("/control/execute config/jobOptions.mac");

  // Initialize the physics factory to register the selected physics.
  physFactory->InitializeList();
  runManager->SetUserInitialization(physFactory);

  // If the WCSim physics list was chosen in jobOptions.mac,
  // then it's hadronic model needs to be selected in jobOptions2.mac
  //=================================
  // Added by JLR 2005-07-05
  //=================================
  // Choice of hadronic interaction model for 
  // protons & neutrons. This file must be read in
  // by the program BEFORE the runManager is initialized.
  // If file does not exist, default model will be used.
  // Currently, default model is set to BINARY.
  UI->ApplyCommand("/control/execute config/jobOptions2.mac");

  // Visualization
  G4VisManager* visManager = new WCSimVisManager;
  visManager->Initialize();

  // Set user action classes
  WCSimPrimaryGeneratorAction* myGeneratorAction = new 
    WCSimPrimaryGeneratorAction(WCSimdetector);
  runManager->SetUserAction(myGeneratorAction);


  WCSimRunAction* myRunAction = new WCSimRunAction(WCSimdetector);
  runManager->SetUserAction(myRunAction);

  runManager->SetUserAction(new WCSimEventAction(myRunAction, WCSimdetector,
        myGeneratorAction));
  runManager->SetUserAction(new WCSimTrackingAction);

  runManager->SetUserAction(new WCSimStackingAction(WCSimdetector));

  runManager->SetUserAction(new WCSimSteppingAction);

  // Initialize G4 kernel
  runManager->Initialize();


  if (macFile.compare("example/generic_chips_vis.mac") == 0)   // Define UI terminal for interactive mode
  { 

    // Start UI Session
    G4UIsession* session =  new G4UIterminal(new G4UItcsh);

    // Visualization Macro
    UI->ApplyCommand("/control/execute example/generic_chips_vis.mac");

    // Start Interactive Mode
    session->SessionStart();

    delete session;
  }
  else           // Batch mode
  { 
    G4String command = "/control/execute ";
    G4String fileName = macFile;

    UI->ApplyCommand(command+fileName);
  }

  delete visManager;

  delete runManager;
  return 0;
}

void usage()
{
    std::cout << "--- WCSim usage instructions ---" << std::endl;
    std::cout << "WCSim [-g arg] path_to_some_file.mac (where the mac file contains the GEANT4 run configuration)" << std::endl;
    std::cout << " -- Optional flags -- " << std::endl;
    std::cout << "   -g path_to_geo_file.mac" << std::endl
              << "       Use a different mac file to specify geometry options" << std::endl
              << "       such as which geometry or PMT simulation to use (default " << std::endl
              << "       is $WCSIMHOME/geoSetup.mac" << std::endl;
}
