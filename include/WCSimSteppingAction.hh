#ifndef WCSimSteppingAction_h
#define WCSimSteppingAction_h 1

#include "G4UserSteppingAction.hh"

class G4Step;
class G4Event;

// Leigh: I don't think we need the class at the moment. Will
// leave it as unimplemented for now in case a use for it arises
class WCSimSteppingAction : public G4UserSteppingAction
{

public:
	WCSimSteppingAction()
	{
	}

	~WCSimSteppingAction()
	{
	}

	void UserSteppingAction(const G4Step *);

private:
};

#endif
