#include "WCSimDetectorConstruction.hh"
#include "WCSimDetectorMessenger.hh"
#include "WCSimMaterialsBuilder.hh"
#include "WCSimTuningParameters.hh"
#include "WCSimPMTManager.hh"

#include "G4Material.hh"
#include "G4Element.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "G4VisAttributes.hh"

#include "G4RunManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include <map>

std::map<int, G4Transform3D> WCSimDetectorConstruction::tubeIDMap;
//std::map<int, cyl_location>  WCSimDetectorConstruction::tubeCylLocation;
hash_map<std::string, int, hash<std::string> > 
WCSimDetectorConstruction::tubeLocationMap;

WCSimDetectorConstruction::WCSimDetectorConstruction(G4int DetConfig) : fPMTBuilder(){
	// Initialize daughter classes

	
  // Decide if (only for the case of !1kT detector) should be upright or horizontal
  isUpright = false;
  isMailbox = false;

  debugMode = false;
//-----------------------------------------------------
// Initilize SD pointers
//-----------------------------------------------------

  aWCPMT     = NULL;

  myConfiguration = DetConfig;

  // Create the PMT manager
  fPMTManager = new WCSimPMTManager();

  //-----------------------------------------------------
  // Create Materials
  //-----------------------------------------------------
    
  ConstructMaterials();

  //-----------------------------------------------------
  // Initialize things related to the tubeID
  //-----------------------------------------------------

  WCSimDetectorConstruction::tubeIDMap.clear();
  //WCSimDetectorConstruction::tubeCylLocation.clear();// (JF) Removed
  WCSimDetectorConstruction::tubeLocationMap.clear();
  totalNumPMTs = 0;
  //  WCPMTExposeHeight= 0.;
  //-----------------------------------------------------
  // Set the default WC geometry.  This can be changed later.
  //-----------------------------------------------------

  //SetSuperKGeometry();
  // DUSEL_100kton_10inch_40perCent();
  // DUSEL_100kton_10inch_HQE_12perCent();
  // DUSEL_100kton_10inch_HQE_30perCent();
  // DUSEL_100kton_10inch_HQE_30perCent_Gd();
  // DUSEL_150kton_10inch_HQE_30perCent();
  // DUSEL_200kton_10inch_HQE_12perCent();
  // DUSEL_200kton_12inch_HQE_10perCent();
  // DUSEL_200kton_12inch_HQE_14perCent();
  // SetMailBox300KTGeometry();
	// SetMailBox100x20x30Geometry();
	// CHIPS_25kton_10inch_HQE_10perCent();
	CHIPS_10kton_10inch_HQE_10perCent();

  //----------------------------------------------------- 
  // Set whether or not Pi0-specific info is saved
  //-----------------------------------------------------

  SavePi0Info(false);
  
  //-----------------------------------------------------
  // Set the default method for implementing the PMT QE
  //-----------------------------------------------------
  SetPMT_QE_Method(1);
  //default NOT to use collection efficiency
  SetPMT_Coll_Eff(0);

	//-----------------------------------------------------
	// Set the default method as the WCSim default for now
	//-----------------------------------------------------
	SetPMTSim(0);

  //----------------------------------------------------- 
  // Make the detector messenger to allow changing geometry
  //-----------------------------------------------------

  messenger = new WCSimDetectorMessenger(this);
}

void WCSimDetectorConstruction::SetDetectorName( const G4String &detName )
{
  fDetectorName = detName;
  this->SetCustomGeometry();
  return;
}

void WCSimDetectorConstruction::SetCustomGeometry()
{
  std::cerr << "Error: you are using the old interface to construct detectors, which only supports the hardcoded ones" << std::endl;
  std::cerr << "Cannot construct the custom detector " << fDetectorName << " - use config/geoConfig.xml and a name that isn't hardcoded" << std::endl;
  assert(false);
}

#include "G4GeometryManager.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"

// Put the call to update inside this function so we can call it with
// this->Update() to ensure that derived classes call their own implementation
void WCSimDetectorConstruction::UpdateGeometry()
{
  this->Update();
}

void WCSimDetectorConstruction::Update()
{
  G4bool geomChanged = true;
  G4RunManager::GetRunManager()->DefineWorldVolume(this->Construct(), geomChanged);
 
 }

WCSimDetectorConstruction::~WCSimDetectorConstruction(){
  for (int i=0;i<fpmts.size();i++){
    delete fpmts.at(i);
  }
  fpmts.clear();

}


G4VPhysicalVolume* WCSimDetectorConstruction::Construct()
{  
  G4GeometryManager::GetInstance()->OpenGeometry();

  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();
  G4LogicalBorderSurface::CleanSurfaceTable();
  G4LogicalSkinSurface::CleanSurfaceTable();

  totalNumPMTs = 0;
  
  //-----------------------------------------------------
  // Create Logical Volumes
  //-----------------------------------------------------

  // First create the logical volumes of the sub detectors.  After they are 
  // created their size will be used to make the world volume.
  // Note the order is important because they rearrange themselves depending
  // on their size and detector ordering.

  G4LogicalVolume* logicWCBox = NULL;
  // Select between cylinder and mailbox
  if (isMailbox) { logicWCBox = ConstructMailboxWC(); }
  else { logicWCBox = ConstructWC(); }

  G4cout << " WCLength (base)      = " << WCLength/m << " m"<< G4endl;

  //-------------------------------

  // Now make the detector Hall.  The lengths of the subdectors 
  // were set above.


  // We want a big lake so that CRY can generate cosmics above it
  // - it does that in a 300m x 300m square so we need at least that
  // much space
  G4double expHallWidth = WCDiameter + 300*m;
  G4double expHallLength = WCLength + 2*40*m; // Depth is 40m - for now just have it floating in water

  G4cout << " expHallLength = " << expHallLength / m << G4endl;
  G4cout << " expHallWidth  = " << expHallWidth  / m << G4endl;
  G4double expHallHalfWidth = 0.5*expHallWidth;
  G4double expHallHalfLength = 0.5*expHallLength;

  G4Box* solidExpHall = new G4Box("expHall",
				  expHallHalfWidth,
				  expHallHalfWidth,
				  expHallHalfLength);
  
  G4LogicalVolume* logicExpHall = 
    new G4LogicalVolume(solidExpHall,
			WCSimMaterialsBuilder::Instance()->GetMaterial("Vacuum"),
			"expHall",
			0,0,0);

  // Now set the visualization attributes of the logical volumes.

  //   logicWCBox->SetVisAttributes(G4VisAttributes::Invisible);
  logicExpHall->SetVisAttributes(G4VisAttributes::Invisible);

  //-----------------------------------------------------
  // Create and place the physical Volumes
  //-----------------------------------------------------

  // Experimental Hall
  G4VPhysicalVolume* physiExpHall = 
    new G4PVPlacement(0,G4ThreeVector(),
  		      logicExpHall,
  		      "expHall",
  		      0,false,0,true);

  // Water Cherenkov Detector (WC) mother volume
  // WC Box, nice to turn on for x and y views to provide a frame:

	  G4RotationMatrix* rotationMatrix = new G4RotationMatrix;
	  rotationMatrix->rotateX(90.*deg);
	  rotationMatrix->rotateZ(90.*deg);

  G4ThreeVector genPosition = G4ThreeVector(0., 0., WCPosition);
  std::cout << "logicWCBox name = " << logicWCBox->GetName() << std::endl;
  std::cout << "logicExpHall name = " << logicExpHall->GetName() << std::endl;
  G4VPhysicalVolume* physiWCBox = 
    new G4PVPlacement(0,
		      genPosition,
		      logicWCBox,
		      "WCBox",
		      logicExpHall,
		      false,
		      0);

  // Traverse and print the geometry Tree
  
  //  TraverseReplicas(physiWCBox, 0, G4Transform3D(), 
  //	   &WCSimDetectorConstruction::PrintGeometryTree) ;

  TraverseReplicas(physiWCBox, 0, G4Transform3D(), 
	           &WCSimDetectorConstruction::DescribeAndRegisterPMT) ;
  
  
  TraverseReplicas(physiWCBox, 0, G4Transform3D(), 
		   &WCSimDetectorConstruction::GetWCGeom) ;
  
  DumpGeometryTableToFile();
  
  // Return the pointer to the physical experimental hall
  return physiExpHall;
}

WCSimPMTManager* WCSimDetectorConstruction::GetPMTManager() const{
	return fPMTManager;
}

std::vector<WCSimPMTConfig> WCSimDetectorConstruction::GetPMTVector() const{
	return fPMTConfigs;
}

void WCSimDetectorConstruction::ResetPMTConfigs()
{
	fPMTConfigs.clear();
	return;
}
