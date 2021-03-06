#pragma once

#include "globals.hh"
#include "G4VModularPhysicsList.hh"
#include "G4PhysListFactory.hh"
#include "G4UnitsTable.hh"
#include "G4OpticalPhysics.hh"

#include "WCSimPhysicsListFactoryMessenger.hh"

class WCSimPhysicsListFactory : public G4VModularPhysicsList
{
public:
	WCSimPhysicsListFactory();
	~WCSimPhysicsListFactory();

	void SetList(G4String newvalue); // called by messenger
	void InitializeList();

	//G4String GetPhysicsListName() {return PhysicsListName;}

	void ConstructParticle();
	void ConstructProcess();
	void SetCuts();

private:
	G4String PhysicsListName;
	G4String ValidListsString;

	WCSimPhysicsListFactoryMessenger *PhysicsMessenger;
	G4PhysListFactory *factory;
};
