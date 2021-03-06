#pragma once

class WCSimTrajectory;

#include "G4VTrajectory.hh"
#include "G4Allocator.hh"
#include <stdlib.h>				   // Include from 'system'
#include "G4ios.hh"				   // Include from 'system'
#include <vector>				   // G4RWTValOrderedVector
#include "globals.hh"			   // Include from 'global'
#include "G4ParticleDefinition.hh" // Include from 'particle+matter'
#include "G4TrajectoryPoint.hh"	   // Include from 'tracking'
#include "G4Track.hh"
#include "G4Step.hh"
#include "CLHEP/Units/SystemOfUnits.h"

class G4Polyline;
// Forward declaration.

typedef std::vector<G4VTrajectoryPoint *> TrajectoryPointContainer;
///////////////////
class WCSimTrajectory : public G4VTrajectory
///////////////////
{

	//--------
public:
	// with description
	//--------

	// Constructor/Destuctor

	WCSimTrajectory();

	WCSimTrajectory(const G4Track *aTrack);
	WCSimTrajectory(WCSimTrajectory &);
	virtual ~WCSimTrajectory();

	// Operators
	inline void *operator new(size_t);
	inline void operator delete(void *);
	inline int operator==(const WCSimTrajectory &right) const
	{
		return (this == &right);
	}

	// Get/Set functions
	inline G4int GetTrackID() const
	{
		return fTrackID;
	}
	inline G4int GetParentID() const
	{
		return fParentID;
	}
	inline G4int GetProcessID() const
	{
		return fProcessID;
	}
	inline G4String GetParticleName() const
	{
		return ParticleName;
	}
	inline G4double GetCharge() const
	{
		return PDGCharge;
	}
	inline G4int GetPDGEncoding() const
	{
		return PDGEncoding;
	}
	inline G4ThreeVector GetInitialMomentum() const
	{
		return initialMomentum;
	}
	inline G4String GetCreatorProcessName() const
	{
		return creatorProcess;
	}
	inline G4double GetWavelength() const
	{
		return (2.0 * M_PI * 197.3) / (fEnergy / CLHEP::eV);
	}

	inline G4double GetGlobalTime() const
	{
		return globalTime;
	}
	inline G4bool GetSaveFlag() const
	{
		return SaveIt;
	}

	// AJP added for making optical photon ntuple
	inline G4bool IsOpticalPhoton() const
	{
		return fIsOpticalPhoton;
	}
	inline G4bool IsScatteredPhoton() const
	{
		return fIsScatteredPhoton;
	}
	inline void SetSaveFlag(G4bool value)
	{
		SaveIt = value;
	}
	inline G4double GetVtxX() const
	{
		return fVtxX;
	}
	inline G4double GetVtxY() const
	{
		return fVtxY;
	}
	inline G4double GetVtxZ() const
	{
		return fVtxZ;
	}
	inline G4double GetVtxTime() const
	{
		return fVtxTime;
	}
	inline G4double GetVtxDirX() const
	{
		return fVtxDirX;
	}
	inline G4double GetVtxDirY() const
	{
		return fVtxDirY;
	}
	inline G4double GetVtxDirZ() const
	{
		return fVtxDirZ;
	}
	inline G4double GetEnergy() const
	{
		return fEnergy / CLHEP::MeV;
	}

	// New function we have added
	inline G4ThreeVector GetStoppingPoint() const
	{
		return stoppingPoint;
	}
	inline G4double GetEndX() const
	{
		return stoppingPoint.x();
	}
	inline G4double GetEndY() const
	{
		return stoppingPoint.y();
	}
	inline G4double GetEndZ() const
	{
		return stoppingPoint.z();
	}
	inline G4double GetEndTime() const
	{
		return fEndTime;
	}

	inline G4VPhysicalVolume *GetStoppingVolume() const
	{
		return stoppingVolume;
	}
	inline void SetStoppingPoint(G4ThreeVector &currentPosition)
	{
		stoppingPoint = currentPosition;
	}
	inline void SetStoppingVolume(G4VPhysicalVolume *currentVolume)
	{
		stoppingVolume = currentVolume;
	}
	inline void SetEndTime(G4double &endTime)
	{
		fEndTime = endTime;
	}

	// Other member functions
	virtual void ShowTrajectory(std::ostream &os = G4cout) const;
	virtual void DrawTrajectory(G4int i_mode = 0) const;
	virtual void AppendStep(const G4Step *aStep);
	virtual int GetPointEntries() const
	{
		return positionRecord->size();
	}
	virtual G4VTrajectoryPoint *GetPoint(G4int i) const
	{
		return (*positionRecord)[i];
	}
	virtual void MergeTrajectory(G4VTrajectory *secondTrajectory);

	G4ParticleDefinition *GetParticleDefinition();

	virtual const std::map<G4String, G4AttDef> *GetAttDefs() const;
	virtual std::vector<G4AttValue> *CreateAttValues() const;

	//---------
private:
	//---------

	TrajectoryPointContainer *positionRecord;
	G4int fTrackID;
	G4int fParentID;
	G4int fProcessID;
	G4int PDGEncoding;
	G4double PDGCharge;
	G4String ParticleName;
	G4ThreeVector initialMomentum;

	G4double fEnergy;
	G4ThreeVector fParticleDirection;
	G4ThreeVector fParticlePosition;
	G4double fVtxX;
	G4double fVtxY;
	G4double fVtxZ;
	G4double fVtxTime;

	G4double fVtxDirX;
	G4double fVtxDirY;
	G4double fVtxDirZ;

	// These are new variables
	G4ThreeVector stoppingPoint;
	G4VPhysicalVolume *stoppingVolume;
	G4double fEndTime;

	// Flags for optical photons
	G4bool fIsOpticalPhoton;
	G4bool fIsScatteredPhoton;

	// M Fechner : new saving mechanism
	G4bool SaveIt;
	G4String creatorProcess;
	G4double globalTime;
};

/***            TEMP  : M FECHNER ***********
 ** modification by Chris Walter that works for geant4 >= 4.6.2p01
 ** does not compile with 4.6.1
 #if defined G4TRACKING_ALLOC_EXPORT
 extern G4DLLEXPORT G4Allocator<WCSimTrajectory> myTrajectoryAllocator;
 #else
 extern G4DLLIMPORT G4Allocator<WCSimTrajectory> myTrajectoryAllocator;
 #endif
 */

extern G4Allocator<WCSimTrajectory> myTrajectoryAllocator;

inline void *WCSimTrajectory::operator new(size_t)
{
	void *aTrajectory;
	aTrajectory = (void *)myTrajectoryAllocator.MallocSingle();
	return aTrajectory;
}

inline void WCSimTrajectory::operator delete(void *aTrajectory)
{
	myTrajectoryAllocator.FreeSingle((WCSimTrajectory *)aTrajectory);
}
