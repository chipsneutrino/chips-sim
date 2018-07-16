#ifndef WCSimRunAction_h
#define WCSimRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "G4String.hh"

#include "TFile.h"
#include "TTree.h"
#include "WCSimRootEvent.hh"
#include "WCSimRootGeom.hh"
#include "WCSimDetectorConstruction.hh"

class G4Run;
class WCSimRunActionMessenger;

class WCSimRunAction: public G4UserRunAction {
	public:
		WCSimRunAction(WCSimDetectorConstruction*);
		~WCSimRunAction();

	public:
		void BeginOfRunAction(const G4Run*);
		void EndOfRunAction(const G4Run*);

		void SetSaveRootFile(const G4bool &saveIt) {
			SaveRootFile = saveIt;
		}
		G4bool GetSaveRootFile() const {
			return SaveRootFile;
		}
		void SetRootFileName(G4String fname) {
			RootFileName = fname;
		}
		G4String GetRootFileName() {
			return RootFileName;
		}

		void SetSavePhotonNtuple(const G4bool &saveIt) {
			SavePhotonNtuple = saveIt;
		}
		G4bool GetSavePhotonNtuple() const {
			return SavePhotonNtuple;
		}
		void SetPhotonNtupleName(const G4String &fname) {
			PhotonNtupleName = fname;
		}
		G4String GetPhotonNtupleName() const {
			return PhotonNtupleName;
		}

		void SetSaveEmissionProfile(const G4bool &saveIt) {
			SaveEmissionProfile = saveIt;
		}
		G4bool GetSaveEmissionProfile() const {
			return SaveEmissionProfile;
		}
		void SetEmissionProfileName(const G4String &fname) {
			EmissionProfileName = fname;
		}
		G4String GetEmissionProfileName() const {
			return EmissionProfileName;
		}

		void FillGeoTree();
		TTree* GetTree() {
			return WCSimTree;
		}
		TTree* GetGeoTree() {
			return geoTree;
		}
		WCSimRootGeom* GetRootGeom() {
			return wcsimrootgeom;
		}
		WCSimRootEvent* GetRootEvent() {
			return wcsimrootsuperevent;
		}

		void SetTree(TTree* tree) {
			WCSimTree = tree;
		}
		void SetGeoTree(TTree* tree) {
			geoTree = tree;
		}
		void SetRootEvent(WCSimRootEvent* revent) {
			wcsimrootsuperevent = revent;
		}
		void SetRootGeom(WCSimRootGeom* rgeom) {
			wcsimrootgeom = rgeom;
		}
		int GetNumberOfEventsGenerated() {
			return numberOfEventsGenerated;
		}
		int GetNtuples() {
			return ntuples;
		}

		void incrementEventsGenerated() {
			numberOfEventsGenerated++;
		}
		void incrementWaterTubeHits() {
			numberOfTimesWaterTubeHit++;
		}
		void incrementFVWaterTubeHits() {
			numberOfTimesFVWaterTubeHit++;
		}
		void incrementCatcherHits() {
			numberOfTimesCatcherHit++;
		}
		void SetNtuples(int ntup) {
			ntuples = ntup;
		}

	private:
		// MFechner : set by the messenger
		std::string RootFileName;
		std::string PhotonNtupleName;
		std::string EmissionProfileName;
		bool SaveRootFile;
		bool SavePhotonNtuple;
		bool SaveEmissionProfile;
		//
		TTree* WCSimTree;
		TTree* geoTree;
		TFile* hfile;
		WCSimRootEvent* wcsimrootsuperevent;
		WCSimRootGeom* wcsimrootgeom;
		WCSimDetectorConstruction* wcsimdetector;

		int numberOfEventsGenerated;
		int numberOfTimesWaterTubeHit;
		int numberOfTimesFVWaterTubeHit;
		int numberOfTimesCatcherHit;

		WCSimRunActionMessenger* messenger;
		int ntuples;  // 1 for ntuples to be written
};

#endif
