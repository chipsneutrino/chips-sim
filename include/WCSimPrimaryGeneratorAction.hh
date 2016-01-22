#ifndef WCSimPrimaryGeneratorAction_h
#define WCSimPrimaryGeneratorAction_h

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

#include "WCSimTruthSummary.hh"

#include <fstream>
#include <vector>

class WCSimDetectorConstruction;
class G4ParticleGun;
class G4GeneralParticleSource;
class G4Event;
class WCSimPrimaryGeneratorMessenger;

class WCSimPrimaryGeneratorAction: public G4VUserPrimaryGeneratorAction
{
public:
	WCSimPrimaryGeneratorAction(WCSimDetectorConstruction*);
	~WCSimPrimaryGeneratorAction();

public:
	void GeneratePrimaries(G4Event* anEvent);

  // Return the truth summary object
  WCSimTruthSummary* GetTruthSummaryPointer() {return &fTruthSummary;};
  WCSimTruthSummary GetTruthSummary() const {return fTruthSummary;};

  // Fiducial vertex functions
  void SetRandomVertex(bool val) {fUseRandomVertex = val;};
  bool GetRandomVertex() {return fUseRandomVertex;};
  void SetFiducialBorder(double val) {fFidBorder = val;};
  bool GetFiducialBorder() {return fFidBorder;};

  // Swap X and Z coordinates for events generated with GENIE
  void SetUseXAxisForBeam(bool val) {fUseXAxisForBeam = val;};
  bool GetUseXAxisForBeam() {return fUseXAxisForBeam;};

private:
  WCSimDetectorConstruction*      myDetector;
  G4ParticleGun*                  particleGun;
  G4GeneralParticleSource*        MyGPS;  //T. Akiri: GPS to run Laser
  WCSimPrimaryGeneratorMessenger* messenger;

  // Truth Summary object
  WCSimTruthSummary fTruthSummary;

	// Variables set by the messenger
	G4bool useMulineEvt;
	G4bool useNormalEvt;
	G4bool useLaserEvt;  //T. Akiri: Laser flag
	G4bool useGpsEvt;
	G4bool useOverlayEvt;
	std::fstream inputFile;
	G4String vectorFileName;
	std::vector<G4String> vectorFileVec;
	std::vector<G4String>::const_iterator vectorFileIterator;
	G4bool GenerateVertexInRock;

  // Overlay variables and functions
  std::fstream fOverlayFile;
  G4String fOverlayFileName;
	std::vector<G4String> fOverlayFileVec;
	std::vector<G4String>::const_iterator fOverlayFileIterator;
  void GenerateOverlayEvents(G4Event *evt);
  // For the overlay events, need to find a fake vertex just inside the detector, and adjust the energy correspondingly.
  bool UpdateOverlayVertexAndEnergy(G4ThreeVector &vtx, double &timeOffset, G4ThreeVector dir, double &energy);

  // Take a tokenised "track" line from a .vec file and contact the particle gun
  // .vec files typically need to swap x and z coordinates for use here.
  void FireParticleGunFromTrackLine(G4Event *evt, G4ThreeVector &vtx, double& vtxTime,std::vector<std::string> &tokens, bool swapXZ, bool isOverlay);

  // Function to pull an event time out of a flat distribution
  // that simulates the beam spill.
  double GetBeamSpillEventTime() const;

  // Function to get a random vertex within the detector volume
  // The fiducial flag gives metre space to the wall.
  G4ThreeVector GenerateRandomVertex() const;

  // Random vertex variables
  bool fUseRandomVertex;
  double fFidBorder;

  // Bool to determine whether we want to swap the X and Z coordinates.
  // This is because GENIE uses Z as the beam axis, and we use X here.
  bool fUseXAxisForBeam;

public:

	inline void SetMulineEvtGenerator(G4bool choice)
	{
		useMulineEvt = choice;
	}
	inline G4bool IsUsingMulineEvtGenerator()
	{
		return useMulineEvt;
	}

	inline void SetNormalEvtGenerator(G4bool choice)
	{
		useNormalEvt = choice;
	}
	inline G4bool IsUsingNormalEvtGenerator()
	{
		return useNormalEvt;
	}

	//T. Akiri: Addition of function for the laser flag
	inline void SetLaserEvtGenerator(G4bool choice)
	{
		useLaserEvt = choice;
	}
	inline G4bool IsUsingLaserEvtGenerator()
	{
		return useLaserEvt;
	}

	inline void SetGpsEvtGenerator(G4bool choice)
	{
		useGpsEvt = choice;
	}
	inline G4bool IsUsingGpsEvtGenerator()
	{
		return useGpsEvt;
	}
  // Add the overlay generator
	inline void SetOverlayEvtGenerator(G4bool choice)
	{
		useOverlayEvt = choice;
	}
	inline G4bool IsUsingOverlayEvtGenerator()
	{
		return useOverlayEvt;
	}

	inline void OpenVectorFile(G4String fileName)
	{
		if (inputFile.is_open())
		{
			inputFile.close();
		}
		vectorFileName = fileName;
		inputFile.open(vectorFileName, std::fstream::in);
	}

	inline void AddVectorFile(G4String fileName)
	{
		vectorFileVec.push_back(fileName);
		if(vectorFileVec.size() == 1)
		{
			vectorFileIterator = vectorFileVec.begin();
			LoadNextVectorFile();
		}
		return;
	}

	inline bool LoadNextVectorFile()
	{
		bool nextExists = false;
		if (vectorFileIterator != vectorFileVec.end())
		{
			OpenVectorFile(*vectorFileIterator);
			nextExists = true;
		}
		vectorFileIterator++;
		return nextExists;
	}
  // Functions for dealing with the overlay files in the same way 
  // as the standard vector files.
	inline void OpenOverlayFile(G4String fileName){
		if (fOverlayFile.is_open()){
			fOverlayFile.close();
		}
		fOverlayFileName = fileName;
		fOverlayFile.open(fOverlayFileName, std::fstream::in);
	}

	inline void AddOverlayFile(G4String fileName){
		fOverlayFileVec.push_back(fileName);
		if(fOverlayFileVec.size() == 1)
		{
			fOverlayFileIterator = fOverlayFileVec.begin();
			LoadNextOverlayFile();
		}
		return;
	}
	inline bool LoadNextOverlayFile(){
		bool nextExists = false;
		if (fOverlayFileIterator != fOverlayFileVec.end())
		{
			OpenOverlayFile(*fOverlayFileIterator);
			nextExists = true;
		}
		fOverlayFileIterator++;
		return nextExists;
	}

	inline G4bool IsGeneratingVertexInRock()
	{
		return GenerateVertexInRock;
	}
	inline void SetGenerateVertexInRock(G4bool choice)
	{
		GenerateVertexInRock = choice;
	}

};

#endif


