#include "WCSimDetectorMessenger.hh"

#include "WCSimDetectorConstruction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"

WCSimDetectorMessenger::WCSimDetectorMessenger(WCSimDetectorConstruction *WCSimDet) : WCSimDetector(WCSimDet)
{
	WCSimDir = new G4UIdirectory("/WCSim/");
	WCSimDir->SetGuidance("Commands to change the geometry of the simulation");

	PMTConfig = new G4UIcmdWithAString("/WCSim/WCgeom", this);
	PMTConfig->SetGuidance("Set the geometry configuration for the WC.");
	PMTConfig->SetParameterName("PMTConfig", false);

	PMTConfig->AvailableForStates(G4State_PreInit, G4State_Idle);
	PMTConfig->SetDefaultValue("CHIPS_10kton_10inch_HQE_10perCent ");

	PMTSize = new G4UIcmdWithAString("/WCSim/WCPMTsize", this);
	PMTSize->SetGuidance("Set alternate PMT size for the WC (Must be entered after geometry details is set).");
	PMTSize->SetGuidance("Available options 20inch 10inch");
	PMTSize->SetParameterName("PMTSize", false);
	PMTSize->SetCandidates("20inch 10inch");
	PMTSize->AvailableForStates(G4State_PreInit, G4State_Idle);

	SavePi0 = new G4UIcmdWithAString("/WCSim/SavePi0", this);
	SavePi0->SetGuidance("true or false");
	SavePi0->SetParameterName("SavePi0", false);
	SavePi0->SetCandidates("true false");
	SavePi0->AvailableForStates(G4State_PreInit, G4State_Idle);

	PMTQEMethod = new G4UIcmdWithAString("/WCSim/PMTQEMethod", this);
	PMTQEMethod->SetGuidance("Set the PMT configuration.");
	PMTQEMethod->SetGuidance("Available options are:\n"
							 "Stacking_Only\n"
							 "Stacking_And_SensitiveDetector\n"
							 "SensitiveDetector_Only\n");
	PMTQEMethod->SetParameterName("PMTQEMethod", false);
	PMTQEMethod->SetCandidates("Stacking_Only "
							   "Stacking_And_SensitiveDetector "
							   "SensitiveDetector_Only ");
	PMTQEMethod->AvailableForStates(G4State_PreInit, G4State_Idle);

	PMTCollEff = new G4UIcmdWithAString("/WCSim/PMTCollEff", this);
	PMTCollEff->SetGuidance("Set the PMT configuration.");
	PMTCollEff->SetGuidance("Available options are:\n"
							"on\n"
							"off\n");
	PMTCollEff->SetParameterName("PMTCollEff", false);
	PMTCollEff->SetCandidates("on "
							  "off ");
	PMTCollEff->AvailableForStates(G4State_PreInit, G4State_Idle);

	PMTSim = new G4UIcmdWithAString("/WCSim/PMTSim", this);
	PMTSim->SetGuidance("Set the PMT Simulation required");
	PMTSim->SetGuidance(
		"Available options are:\n default (SuperK) \n CHIPS (based on IceCube PMTs) \n TOT (tries to replicate TOT)");
	PMTSim->SetParameterName("PMTSim", false);
	PMTSim->SetCandidates("default CHIPS chips TOT tot");
	PMTSim->AvailableForStates(G4State_PreInit, G4State_Idle);

	PMTTime = new G4UIcmdWithAString("/WCSim/PMTTime", this);
	PMTTime->SetGuidance("Set the time recorded by the PMT");
	PMTTime->SetGuidance(
		"Available options are:\
          \n first (use the time at which the first photon hits - default)\
          \n mean (use the mean of all the photon hit times - debugging option)");
	PMTTime->SetParameterName("PMTTime", true); // Omittable, default to first
	PMTTime->SetDefaultValue("first ");
	PMTTime->SetCandidates("first mean");
	PMTTime->AvailableForStates(G4State_PreInit, G4State_Idle);

	PMTPerfectTiming = new G4UIcmdWithABool("/WCSim/PMTPerfectTiming", this);
	PMTPerfectTiming->SetGuidance("Bool to toggle perfect time resolution in the PMT\n"
								  " - The default value is false.\n");
	PMTPerfectTiming->SetParameterName("PMTPerfectTiming", true); // Omittable, default to false
	PMTPerfectTiming->SetDefaultValue(false);

	WCConstruct = new G4UIcmdWithoutParameter("/WCSim/Construct", this);
	WCConstruct->SetGuidance("Update detector construction with new settings.");
}

WCSimDetectorMessenger::~WCSimDetectorMessenger()
{
	delete PMTConfig;
	delete SavePi0;
	delete PMTQEMethod;
	delete PMTCollEff;
	delete PMTSim;
	delete PMTTime;
	delete PMTPerfectTiming;
	delete tubeCmd;
	delete distortionCmd;
	delete WCSimDir;
}

void WCSimDetectorMessenger::SetNewValue(G4UIcommand *command, G4String newValue)
{
	if (command == PMTConfig)
	{
		WCSimDetector->SetIsMailbox(false);
		WCSimDetector->SetIsUpright(false);
		if (newValue == "150kTMailbox_10inch_HQE_30perCent")
		{
			WCSimDetector->SetIsMailbox(true);
			WCSimDetector->SetMailBox150kTGeometry_10inch_HQE_30perCent(); //aah
		}
		else if (newValue == "150kTMailbox_10inch_40perCent")
		{
			WCSimDetector->SetIsMailbox(true);
			WCSimDetector->SetMailBox150kTGeometry_10inch_40perCent(); //aah
		}
		// Leigh's stuff
		else if (newValue == "100kTMailbox_10perCent")
		{
			WCSimDetector->SetIsMailbox(true);
			WCSimDetector->SetMailBox100kTGeometry();
		}
		else if (newValue == "100kTMailbox_20perCent")
		{
			WCSimDetector->SetIsMailbox(true);
			WCSimDetector->SetMailBox100kTGeometry_20perCent();
		}
		else if (newValue == "100kTMailbox_30perCent")
		{
			WCSimDetector->SetIsMailbox(true);
			WCSimDetector->SetMailBox100kTGeometry_30perCent();
		}
		else if (newValue == "100kTMailbox_40perCent")
		{
			WCSimDetector->SetIsMailbox(true);
			WCSimDetector->SetMailBox100kTGeometry_40perCent();
		}
		else if (newValue == "100kTMailbox_50perCent")
		{
			WCSimDetector->SetIsMailbox(true);
			WCSimDetector->SetMailBox100kTGeometry_50perCent();
		}
		// End Leigh's stuff
		// Begin Andy P's stuff
		else if (newValue == "Mailbox_100x20x30")
		{
			WCSimDetector->SetIsMailbox(true);
			WCSimDetector->SetMailBox100x20x30Geometry();
		}
		else if (newValue == "CHIPS_25kton_10inch_HQE_10perCent")
		{
			WCSimDetector->CHIPS_25kton_10inch_HQE_10perCent();
		}
		else if (newValue == "CHIPS_10kton_10inch_HQE_10perCent")
		{
			WCSimDetector->CHIPS_10kton_10inch_HQE_10perCent();
		}
		else if (newValue == "GiantPhotonTest")
		{
			WCSimDetector->GiantPhotonTest();
		}
		else if (newValue == "SuperK")
		{
			WCSimDetector->SetSuperKGeometry();
		}
		else if (newValue == "DUSEL_100kton_10inch_40perCent")
		{
			WCSimDetector->DUSEL_100kton_10inch_40perCent();
		}
		else if (newValue == "DUSEL_100kton_10inch_HQE_12perCent")
		{
			WCSimDetector->DUSEL_100kton_10inch_HQE_12perCent();
		}
		else if (newValue == "DUSEL_100kton_10inch_HQE_30perCent")
		{
			WCSimDetector->DUSEL_100kton_10inch_HQE_30perCent();
		}
		else if (newValue == "DUSEL_100kton_10inch_HQE_30perCent_Gd")
		{
			WCSimDetector->DUSEL_100kton_10inch_HQE_30perCent_Gd();
		}
		else if (newValue == "DUSEL_150kton_10inch_HQE_30perCent")
		{
			WCSimDetector->DUSEL_150kton_10inch_HQE_30perCent();
		}
		else if (newValue == "DUSEL_200kton_10inch_HQE_12perCent")
		{
			WCSimDetector->DUSEL_200kton_10inch_HQE_12perCent();
		}
		else if (newValue == "DUSEL_200kton_12inch_HQE_10perCent")
		{
			WCSimDetector->DUSEL_200kton_12inch_HQE_10perCent();
		}
		else if (newValue == "DUSEL_200kton_12inch_HQE_14perCent")
		{
			WCSimDetector->DUSEL_200kton_12inch_HQE_14perCent();
		}
		std::cout << "WCSimDetectorMessenger: setting detector name to " << newValue << std::endl;
		WCSimDetector->SetDetectorName(newValue);
	}

	if (command == SavePi0)
	{
		G4cout << "Set the flag for saving pi0 info " << newValue << G4endl;
		if (newValue == "true")
		{
			WCSimDetector->SavePi0Info(true);
		}
		else if (newValue == "false")
		{
			WCSimDetector->SavePi0Info(false);
		}
		else
		{
		}
	}

	if (command == PMTQEMethod)
	{
		G4cout << "Set PMT QE Method " << newValue << " ";
		if (newValue == "Stacking_Only")
		{
			WCSimDetector->SetPMT_QE_Method(1);
			G4cout << "1";
		}
		else if (newValue == "Stacking_And_SensitiveDetector")
		{
			WCSimDetector->SetPMT_QE_Method(2);
			G4cout << "2";
		}
		else if (newValue == "SensitiveDetector_Only")
		{
			WCSimDetector->SetPMT_QE_Method(3);
			G4cout << "3";
		}
		else
		{
		}
		G4cout << G4endl;
	}

	if (command == PMTCollEff)
	{
		G4cout << "Set PMT Collection Efficiency " << newValue << " ";
		if (newValue == "on")
		{
			WCSimDetector->SetPMT_Coll_Eff(1);
			G4cout << "1";
		}
		else if (newValue == "off")
		{
			WCSimDetector->SetPMT_Coll_Eff(0);
			G4cout << "0";
		}
		G4cout << G4endl;
	}

	if (command == WCConstruct)
	{
		std::cout << "Updating geometry" << std::endl;
		WCSimDetector->UpdateGeometry();
	}

	// Leigh: PMT simulation choice
	if (command == PMTSim)
	{
		// WCSim default method
		if (newValue == "default")
		{
			WCSimDetector->SetPMTSim(0);
		}
		else if (newValue == "CHIPS" || newValue == "chips")
		{
			WCSimDetector->SetPMTSim(1);
		}
		else if (newValue == "TOT" || newValue == "tot")
		{
			WCSimDetector->SetPMTSim(2);
		}
		else
		{
			G4cout << "That PMT Sim value does not exist." << std::endl;
		}
	}
	// Andy: PMT timing choice
	if (command == PMTTime)
	{
		// WCSim default method
		if (newValue == "first")
		{
			WCSimDetector->SetPMTTime(0);
		}
		else if (newValue == "mean")
		{
			WCSimDetector->SetPMTTime(1);
		}
		else
		{
			G4cout << "That PMT time value does not exist." << std::endl;
		}
	}
	if (command == PMTPerfectTiming)
	{
		bool val = false;
		if (newValue == "true")
		{
			val = true;
		}
		WCSimDetector->SetPMTPerfectTiming(val);
	}
}
