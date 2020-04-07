#include "WCSimTrackingActionMessenger.hh"

#include "WCSimTrackingAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithADouble.hh"

WCSimTrackingActionMessenger::WCSimTrackingActionMessenger(WCSimTrackingAction *WCSimTA) : fTrackingAction(WCSimTA)
{
	WCSimIODir = new G4UIdirectory("/WCSimTrack/");
	WCSimIODir->SetGuidance("Commands to select tracking options");

	PercentCherenkovPhotonsToDraw = new G4UIcmdWithADouble("/WCSimTrack/PercentCherenkovPhotonsToDraw", this);
	PercentCherenkovPhotonsToDraw->SetGuidance("Set the percentage of Cherenkov photon tracks to store (0 - 100)");
	PercentCherenkovPhotonsToDraw->SetParameterName("PercentCherenkovPhotonsToDraw", true);
	PercentCherenkovPhotonsToDraw->SetDefaultValue(0.0);
}

WCSimTrackingActionMessenger::~WCSimTrackingActionMessenger()
{
	delete PercentCherenkovPhotonsToDraw;
	delete WCSimIODir;
}

void WCSimTrackingActionMessenger::SetNewValue(G4UIcommand *command, G4String newValue)
{
	if (command == PercentCherenkovPhotonsToDraw)
	{
		double newPercent = PercentCherenkovPhotonsToDraw->GetNewDoubleValue(newValue);
		if (newPercent > 100.0)
		{
			std::cerr << "You've asked to save > 100% of Cherenkov photons.  Setting to 100%." << std::endl;
			newPercent = 100.0;
		}
		if (newPercent < 0.0)
		{
			std::cerr << "You've asked to save < 0% of Cherenkov photons.  Setting to 0%." << std::endl;
			newPercent = 0.0;
		}
		if (0.0 < newPercent && 1.0 >= newPercent)
		{
			std::cerr
				<< "You've asked to save < 1% of Cherenkov photons.  Are you sure you specified a percentage and not a decimal?"
				<< std::endl;
			std::cerr << "I'm going to assume you meant what you asked for and save " << newPercent << "% of photons"
					  << std::endl;
		}
		fTrackingAction->SetPercentCherenkovPhotonsToDraw(newPercent);
	}
}
