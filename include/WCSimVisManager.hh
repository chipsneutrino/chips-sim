#pragma once

#ifdef G4VIS_USE

#include "G4VisManager.hh"

class WCSimVisManager : public G4VisManager
{

public:
	WCSimVisManager();

private:
	void RegisterGraphicsSystems();
};

#endif
