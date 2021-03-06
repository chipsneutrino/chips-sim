#pragma once

#include <set>
#include "G4UserTrackingAction.hh"
#include "globals.hh"

class WCSimTrackingActionMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class G4Track;

class WCSimTrackingAction : public G4UserTrackingAction
{
public:
	WCSimTrackingAction();
	~WCSimTrackingAction();

	void PreUserTrackingAction(const G4Track *aTrack);
	void PostUserTrackingAction(const G4Track *);
	void SetPercentCherenkovPhotonsToDraw(const double &percent);
	void SetFractionCherenkovPhotonsToDraw(const double &frac);

private:
	std::set<G4String> ProcessList;
	std::set<G4int> ParticleList;
	std::set<G4int> pi0List;
	WCSimTrackingActionMessenger *fMessenger;

	double fFractionCherenkovPhotonsToDraw;
};
