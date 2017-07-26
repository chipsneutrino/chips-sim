/*
 * WCSimCherenkovBuilder.cc
 *
 *  Created on: Aug 15, 2014
 *      Author: aperch
 *
 *  PMT Rotation feature added by S. Germani on Dec 10, 2015
 */

#include "WCSimCherenkovBuilder.hh"
#include "WCSimDetectorConstruction.hh"
#include "WCSimGeoConfig.hh"
#include "WCSimGeoManager.hh"
#include "WCSimMaterialsBuilder.hh"
#include "WCSimPMTConfig.hh"
#include "WCSimPMTManager.hh"
#include "WCSimPolygonTools.hh"
#include "WCSimUnitCell.hh"
#include "WCSimWCSD.hh"

#include "G4Colour.hh"
#include "G4Material.hh"
#include "G4Polyhedra.hh"
#include "G4PVReplica.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SDManager.hh"
#include "G4Tubs.hh"
#include "G4ThreeVector.hh"
#include "G4TwoVector.hh"
#include "G4VisAttributes.hh"

#include "G4GeometryManager.hh"
#include "G4GeometryTolerance.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4Box.hh"

#include <cassert>
#include <math.h>
#include <cmath>

WCSimCherenkovBuilder::WCSimCherenkovBuilder(G4int DetConfig) :
	WCSimDetectorConstruction(DetConfig), fConstructed(false), fGeoConfig(NULL) {

	fBlacksheetThickness = 5 * mm;
	fWhitesheetThickness = fBlacksheetThickness;
	fPMTHolderLength = 154.8 * mm;
	//	fPMTHolderLength = 15.48 * mm;
        fPlanePipeRadius = 19.05 * mm;  // 1.5 Inch Pipes
        fPlanePipeStep   = 1850  * mm; // Approximate POM Length

	fDebugMode 			 = true;

  fPMTManager = new WCSimPMTManager();

	fLakeLogic = NULL;
	fBarrelLogic = NULL;
	fPrismLogic = NULL;
	fCapLogicTop = NULL;
	fCapLogicBottom = NULL;
	fCapLogicBottomRing = NULL;
	fNumPMTs = 0;

  fCapBSTopPhysics = NULL;
  fCapWSTopPhysics = NULL;
  fCapBSBottomPhysics = NULL;
  fCapWSBottomPhysics = NULL;

	// Initialize all the constants:
  fGotMeasurements = false;
	fBarrelRadius = -999 * m; // Barrel is a cylinder - this is its actual radius
	fBarrelHeight = -999 * m;
	fBarrelLengthForCells = -999 * m;

	fVetoRadiusInside = -999 * m; // Radius is to centre of wall not the vertex
	fVetoRadiusOutside = -999 * m;
	fVetoHeight = -999 * m;

	fPrismRadiusInside = -999 * m; // Radius is to centre of wall not the vertex
	fPrismRadiusOutside = -999 * m;
	fPrismHeight = -999 * m;

	fPrismRingRadiusInside = -999 * m; // Radius is to centre of wall not the vertex
	fPrismRingRadiusOutside = -999 * m;

	fPrismRingSegmentRadiusInside = -999 * m; // To centre of segment not edge
	fPrismRingSegmentRadiusInside = -999 * m;
	fPrismRingSegmentDPhi = -999 * m;

	fPrismRingSegmentBSRadiusInside = -999 * m; // To centre not edge
	fPrismRingSegmentBSRadiusOutside = -999 * m;

	fCapAssemblyHeight = -999 * m;
	fCapAssemblyRadius = -999 * m;

	fCapRingHeight = -999 * m;
	fCapRingRadiusInside = -999 * m;
	fCapRingRadiusOutside = -999 * m;

	fCapRingSegmentDPhi = -999 * m;
  fCapRingSegmentHeight = -999 * m;
	fCapRingSegmentRadiusInside = -999 * m;
	fCapRingSegmentRadiusOutside = -999 * m;

	fCapRingSegmentBSHeight = -999 * m;
	fCapRingSegmentBSRadiusInside = -999 * m;
	fCapRingSegmentBSRadiusOutside = -999 * m;

	fCapPolygonRadius = -999 * m;
	fCapPolygonHeight = -999 * m;
	
  fCapPolygonCentreRadius = -999 * m;
	fCapPolygonCentreHeight = -999 * m;

	fCapPolygonEdgeBSRadiusInside = -999 * m;
	fCapPolygonEdgeBSRadiusOutside = -999 * m;
	fCapPolygonEdgeBSHeight = -999 * m;

	fCapPolygonEndBSRadius = -999 * m;
	fCapPolygonEndBSHeight = -999 * m;

}

WCSimCherenkovBuilder::~WCSimCherenkovBuilder() {
	for(unsigned int iCell = 0; iCell < fWallUnitCells.size(); ++iCell)
	{
		delete fWallUnitCells.at(iCell);
	}
	fWallUnitCells.clear();

	for(unsigned int iCell = 0; iCell < fTopUnitCells.size(); ++iCell)
	{
		delete fTopUnitCells.at(iCell);
	}
	fTopUnitCells.clear();

	for(unsigned int iCell = 0; iCell < fBottomUnitCells.size(); ++iCell)
	{
		delete fBottomUnitCells.at(iCell);
	}
	fBottomUnitCells.clear();

	for(unsigned int iWall = 0; iWall < fSegmentLogics.size(); ++iWall)
	{
		if( fPrismWallLogics.at(iWall) != NULL ){ delete fPrismWallLogics.at(iWall);}
		if( fPrismWallPhysics.at(iWall) != NULL ){ delete fPrismWallPhysics.at(iWall);}
		if( fPrismRingLogics.at(iWall) != NULL ) { delete fPrismRingLogics.at(iWall); }
    // Get a glibc error when deleting fPrimsRingPhysics - probably something I don't understand about G4PVReplicas
	}


	for(unsigned int iSegment = 0; iSegment < fSegmentLogics.size(); ++iSegment)
	{
		if( fSegmentLogics.at(iSegment) != NULL ){ delete fSegmentLogics.at(iSegment);}
		// if( fSegmentPhysics.at(iSegment) != NULL ){ delete fSegmentPhysics.at(iSegment);}
	}

  if( fGeoConfig != NULL ) { delete fGeoConfig; }
	if(fCapLogicBottom != NULL ) { delete fCapLogicBottom;}
	if(fCapLogicBottomRing != NULL) { delete fCapLogicBottomRing; }
  if( fCapLogicTop != NULL ) { delete fCapLogicTop;}
  if( fPrismLogic != NULL ) { delete fPrismLogic;}
  if( fBarrelLogic != NULL ) { delete fBarrelLogic;}
  if( fLakeLogic != NULL ) { delete fLakeLogic;}
	if( fPMTManager != NULL ) { delete fPMTManager; }




}

void WCSimCherenkovBuilder::SetCustomGeometry()
{
  WCSimGeoManager * manager = new WCSimGeoManager();
  fGeoConfig = new WCSimGeoConfig(manager->GetGeometryByName(fDetectorName));

  fWallCellsX.resize(fGeoConfig->GetNSides());
  fWallCellsZ.resize(fGeoConfig->GetNSides());
  fWallCellLength.resize(fGeoConfig->GetNSides());
  fWallCellSize.resize(fGeoConfig->GetNSides());

  delete manager;

  ResetPMTConfigs();
  std::vector< std::string> pmtNames = fGeoConfig->GetPMTNamesUsed();
  for(std::vector<std::string>::const_iterator pmtItr = pmtNames.begin(); pmtItr != pmtNames.end(); ++pmtItr)
  {
    fPMTConfigs.push_back(fPMTManager->GetPMTByName(*pmtItr));
  }

  std::cout << "=== PMT LOG ===" << std::endl;
  std::cout << "PMT Vector Size = " << fPMTConfigs.size() << std::endl;
  fPMTConfigs[0].Print();
}

G4LogicalVolume * WCSimCherenkovBuilder::ConstructDetector() {
  std::cout << "*** WCSimCherenkovBuilder::ConstructDetector *** " << std::endl;
  std::cout << "Constructing " << fDetectorName << std::endl;

  ConstructDetectorWrapper();
	assert(fLakeLogic != NULL);
	return fLakeLogic;
}

void WCSimCherenkovBuilder::ConstructDetectorWrapper() {
	if (!fConstructed) {

		if( !fGotMeasurements ) { std::cout << "Getting measurements" << std::endl; GetMeasurements(); }
		ConstructUnitCells();
		ConstructEnvironment();
		ConstructFrame();
		ConstructVeto();
		ConstructInnerDetector();
		ConstructEndCaps();
		ConstructPMTs();
		PlacePMTs();
		CreateSensitiveDetector();

		// std::cout << "Top cap logical volume: " << fCapLogicTop->GetName() << std::endl;
	}
	fConstructed = true;
}

void WCSimCherenkovBuilder::ConstructEnvironment() {
	SetPositions();


	// The water barrel is placed in a cylinder tub of water, representing the lake
	G4double innerRadius = fGeoConfig->GetOuterRadius();
	G4double innerHeight= fGeoConfig->GetInnerHeight();
	G4double shoreDistance = 200; // Distance from detector to shore (m)

	// Define our world: a cylindrical lake in which that detector will sit
	// Constants used to configure it:
	G4double lakeIR     	   = 0.0; // inner radius of lake
	G4double lakeOR     	   = innerRadius + shoreDistance*m; // outer radius of lake
	G4double lakeHeight 	   = innerHeight + 20. * m;
	G4Material * lakeWater 	   = WCSimMaterialsBuilder::Instance()->GetMaterial("PitWater");  // Fill it with purified water
	G4VisAttributes* lakeColor = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0));

	// The geometry objects themselves:
	G4Tubs* lakeTubs = new G4Tubs("lake", lakeIR, lakeOR, lakeHeight,
								  0.0 * deg, 360.0 * deg);


	fLakeLogic = new G4LogicalVolume(lakeTubs, lakeWater, "lake", 0, 0, 0);
	fLakeLogic->SetVisAttributes(lakeColor);
	fLakeLogic->SetVisAttributes(G4VisAttributes::Invisible);
	std::cout << "Lake logical volume = " << std::endl;
	std::cout << fLakeLogic->GetName() << std::endl;


}

void WCSimCherenkovBuilder::ConstructFrame() {

	// Make a large cylinder to hold the PMTs and water
	G4Material * barrelWater = WCSimMaterialsBuilder::Instance()->GetMaterial("Water");  // Fill it with purified water

	G4Tubs* barrelTubs = new G4Tubs("barrelTubs", 0, fBarrelRadius, fBarrelHeight/2.0, 0. * deg, 360. * deg);
	fBarrelLogic = new G4LogicalVolume(barrelTubs, barrelWater, "barrelTubs", 0, 0, 0);
	G4VisAttributes* barrelColor = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0));
  fBarrelLogic->SetVisAttributes(barrelColor);

	// G4VPhysicalVolume* barrelPhysic = // Gets rid of compiler warning
	G4RotationMatrix	* prismRotation = new G4RotationMatrix;
  prismRotation->rotateZ(180.0 /(double)fGeoConfig->GetNSides() * deg);
	fBarrelPhysics = new G4PVPlacement(prismRotation, 
                            G4ThreeVector(0., 0., 0.),
														fBarrelLogic, "barrelTubs",
														fLakeLogic, false, 0);
}

void WCSimCherenkovBuilder::ConstructVeto() {
	//-----------------------------------------------------
	// Make the veto
	//----------------------------------------------------

	// + structure of the frame etc.
	// The radii are measured to the center of the surfaces
	// (tangent distance). Thus distances between the corner and the center are bigger.

	// Zip them up into arrays to pass to Geant
	G4double vetoAnnulusZ[2] = { -0.5 * fVetoHeight, 0.5 * fVetoHeight };
	G4double vetoAnnulusRmin[2] = { fVetoRadiusInside, fVetoRadiusInside };
	G4double vetoAnnulusRmax[2] = { fVetoRadiusOutside, fVetoRadiusOutside };

//  std::cout << "VETO VOLUME = " << fVetoRadiusInside << ", " << fVetoRadiusOutside << ", " << fVetoHeight << std::endl;

	// Now make the volumes
	G4Polyhedra* vetoSolid = new G4Polyhedra("vetoPrism",
											  0. * deg,  // phi start
											  360.0 * deg, // phi end
											  (G4int)fGeoConfig->GetNSides(), //number of sides
											  2, vetoAnnulusZ, // number and location of z planes
											  vetoAnnulusRmin, vetoAnnulusRmax); // inner and outer radii

	fVetoLogic = new G4LogicalVolume(vetoSolid, WCSimMaterialsBuilder::Instance()->GetMaterial("Whitesheet"),
									  "vetoPrism", 0, 0, 0);
	G4VisAttributes* vetoColour = new G4VisAttributes(G4Colour(0.0, 0.7, 0.7));
  fVetoLogic->SetVisAttributes(vetoColour);

	G4VPhysicalVolume *vBarrel = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.),
										fVetoLogic, "vetoPrism", fBarrelLogic, false, 0, true);

//  G4double topCapZ[2] = {0.5*fVetoHeight+0.0001*mm,0.5*fVetoHeight+fWhitesheetThickness};
  G4double topCapZ[2] = {-0.5*fWhitesheetThickness,0.5*fWhitesheetThickness};
  G4double topCapRMin[2] = {0.0,0.0};
  G4double topCapRMax[2] = {fVetoRadiusOutside,fVetoRadiusOutside};

  G4Polyhedra *vetoTopSolid = new G4Polyhedra("vetoTopCap",0.*deg,360.*deg,
                                             (G4int)fGeoConfig->GetNSides(),
                                             2,topCapZ,topCapRMin,topCapRMax); 
  fVetoTopLogic = new G4LogicalVolume(vetoTopSolid,WCSimMaterialsBuilder::Instance()->GetMaterial("Whitesheet"),
                                      "vetoTopCap",0,0,0);
  fVetoTopLogic->SetVisAttributes(vetoColour);
	G4VPhysicalVolume *vTop = new G4PVPlacement(0, G4ThreeVector(0., 0.,0.5*(fVetoHeight+fWhitesheetThickness)),
										fVetoTopLogic, "vetoTopCap", fBarrelLogic, false, 0, true);

//  G4double bottomCapZ[2] = {-(0.5*fVetoHeight+0.0001*mm),-(0.5*fVetoHeight+fWhitesheetThickness)};
  G4double bottomCapZ[2] = {-0.5*fWhitesheetThickness,0.5*fWhitesheetThickness};
  G4double bottomCapRMin[2] = {0.0,0.0};
  G4double bottomCapRMax[2] = {fVetoRadiusOutside,fVetoRadiusOutside};

//  std::cout << "Veto Cap Positions = " << 0.5*(fVetoHeight+fWhitesheetThickness) << ", " << -0.5*(fVetoHeight+fWhitesheetThickness) << std::endl;

  G4Polyhedra *vetoBottomSolid = new G4Polyhedra("vetoBottomCap",0.*deg,360.*deg,
                                             (G4int)fGeoConfig->GetNSides(),
                                             2,bottomCapZ,bottomCapRMin,bottomCapRMax); 
  fVetoBottomLogic = new G4LogicalVolume(vetoBottomSolid,WCSimMaterialsBuilder::Instance()->GetMaterial("Whitesheet"),
                                      "vetoBottomCap",0,0,0);
  fVetoBottomLogic->SetVisAttributes(vetoColour);
	G4VPhysicalVolume* vBottom = new G4PVPlacement(0, G4ThreeVector(0., 0.,-0.5*(fVetoHeight+fWhitesheetThickness)),
										fVetoBottomLogic, "vetoBottomCap", fBarrelLogic, false, 0, true);

  G4LogicalBorderSurface * WaterWSVetoBarrelSurface = NULL;
  WaterWSVetoBarrelSurface = new G4LogicalBorderSurface("WaterWSVetoBarrelSurface",
                                fBarrelPhysics,
                                vBarrel,
                                WCSimMaterialsBuilder::Instance()->GetOpticalSurface("WaterWSCellSurface"));

  G4LogicalBorderSurface * WaterWSVetoTopSurface = NULL;
  WaterWSVetoTopSurface = new G4LogicalBorderSurface("WaterWSVetoTopSurface",
                                fBarrelPhysics,
                                vTop,
                                WCSimMaterialsBuilder::Instance()->GetOpticalSurface("WaterWSCellSurface"));

  G4LogicalBorderSurface * WaterWSVetoBottomSurface = NULL;
  WaterWSVetoBottomSurface = new G4LogicalBorderSurface("WaterWSVetoBottomSurface",
                                fBarrelPhysics,
                                vBottom,
                                WCSimMaterialsBuilder::Instance()->GetOpticalSurface("WaterWSCellSurface"));


	return;
}

void WCSimCherenkovBuilder::ConstructInnerDetector() {
	CreatePrism();
	CreatePrismWalls();
	CreatePrismRings();
	CreateRingSegments();
	CreateSegmentCells();
}

void WCSimCherenkovBuilder::CreatePrism() {

	//-----------------------------------------------------
	// Form annular section of barrel to hold PMTs
	//----------------------------------------------------

	// + structure of the frame etc.
	// The radii are measured to the center of the surfaces
	// (tangent distance). Thus distances between the corner and the center are bigger.

	// Zip them up into arrays to pass to Geant
	G4double mainAnnulusZ[2] = { -0.5 * fPrismHeight, 0.5 * fPrismHeight };
	G4double mainAnnulusRmin[2] = { fPrismRadiusInside, fPrismRadiusInside };
	G4double mainAnnulusRmax[2] = { fPrismRadiusOutside, fPrismRadiusOutside };

  std::cout << "INNER VOLUME = " << fPrismRadiusInside << ", " << fPrismRadiusOutside << ", " << fPrismHeight << std::endl;

	// Now make the volumes
	G4Polyhedra* prismSolid = new G4Polyhedra("prism",
											  0. * deg,  // phi start
											  360.0 * deg, // phi end
											  (G4int)fGeoConfig->GetNSides(), //number of sides
											  2, mainAnnulusZ, // number and location of z planes
											  mainAnnulusRmin, mainAnnulusRmax); // inner and outer radii

	fPrismLogic = new G4LogicalVolume(prismSolid, WCSimMaterialsBuilder::Instance()->GetMaterial("Water"),
									  "prism", 0, 0, 0);
	G4VisAttributes* prismColor = new G4VisAttributes(G4Colour(0.0, 1.0, 1.0));
  fPrismLogic->SetVisAttributes(prismColor);

	// G4VPhysicalVolume* prismPhysic = // Fixes compiler warning
	new G4PVPlacement(0, G4ThreeVector(0., 0., 0.),
														fPrismLogic, "prism",
														fBarrelLogic, false, 0, true);
}

void WCSimCherenkovBuilder::CreatePrismWalls()
{
	fPrismWallLogics.resize(fGeoConfig->GetNSides());
	fPrismWallPhysics.resize(fGeoConfig->GetNSides());
	G4VisAttributes* prismWallColor = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0));
	for(unsigned int iZone = 0; iZone < fGeoConfig->GetNumZones(WCSimGeometryEnums::DetectorRegion_t::kWall); ++iZone)
	{
		std::cout << "Making wall " << iZone << std::endl;
		G4double prismWallZ[2] = { -0.5 * fPrismHeight, 0.5 * fPrismHeight };
		G4double prismWallRmin[2] = { fPrismWallRadiusInside, fPrismWallRadiusInside };
		G4double prismWallRmax[2] = { fPrismWallRadiusOutside, fPrismWallRadiusOutside };
		std::cout << "Wall z: " << prismWallZ[0] << "  " << prismWallZ[1] << std::endl;
		std::cout << "Wall rmin: " << prismWallRmin[0] << "  " << prismWallRmin[1] << std::endl;
		std::cout << "Wall rmax: " << prismWallRmax[0] << "  " << prismWallRmax[1] << std::endl;

		G4RotationMatrix* prismWallRotation = new G4RotationMatrix();
		prismWallRotation->rotateZ(360.0 * (fGeoConfig->GetNumZones(WCSimGeometryEnums::DetectorRegion_t::kWall) - iZone-0.5)/(double)fGeoConfig->GetNSides() * deg);

		// Now make the volumes
		std::stringstream ss;
		ss << "prism" << iZone;
    G4String elName = ss.str();
		G4Polyhedra* prismWallSolid = new G4Polyhedra(elName,
																								 -0.5 * 360.0 / ((double)fGeoConfig->GetNSides()) * deg,  // phi start
																									360.0 / ((double)fGeoConfig->GetNSides()) * deg,  // phi width
																  								1, //number of sides
																									2, prismWallZ, // number and location of z planes
																									prismWallRmin, prismWallRmax); // inner and outer radii
		if(fPrismWallLogics.at(iZone) != NULL) { delete fPrismWallLogics.at(iZone); }
		fPrismWallLogics.at(iZone) = new G4LogicalVolume(prismWallSolid, WCSimMaterialsBuilder::Instance()->GetMaterial("Water"),
																										 ss.str().c_str(), 0,0,0);
    fPrismWallLogics.at(iZone)->SetVisAttributes(prismWallColor);
		if(fPrismWallPhysics.at(iZone) != NULL) { delete fPrismWallPhysics.at(iZone); }
		fPrismWallPhysics.at(iZone) = new G4PVPlacement(prismWallRotation, G4ThreeVector(0,0,0), fPrismWallLogics.at(iZone),
																									  ss.str().c_str(), fPrismLogic, false, 0);
	}



}

void WCSimCherenkovBuilder::CreatePrismRings() {
	// Slice the prism up lengthways
	fPrismRingLogics.resize(fGeoConfig->GetNSides());
	fPrismRingPhysics.resize(fGeoConfig->GetNSides());
  for(unsigned int iZone = 0; iZone < fGeoConfig->GetNumZones(WCSimGeometryEnums::DetectorRegion_t::kWall); ++iZone)
  {
		//-----------------------------------------------------
		// Subdivide the BarrelAnnulus into rings
		//-----------------------------------------------------
		G4Material * pureWater  = WCSimMaterialsBuilder::Instance()->GetMaterial("Water");

		G4double RingZ[2] 			= { -0.5 * fPrismRingHeight.at(iZone), 0.5 * fPrismRingHeight.at(iZone) };
		G4double mainAnnulusRmin[2] = { fPrismRingRadiusInside, fPrismRingRadiusInside };
		G4double mainAnnulusRmax[2] = { fPrismRingRadiusOutside, fPrismRingRadiusOutside };

		// Now make all the volumes
		std::cout << "Making solid, nSides = " << fGeoConfig->GetNSides() << std::endl;
    std::cout << "GeoConfig thinks there are " << fGeoConfig->GetNumZones(WCSimGeometryEnums::DetectorRegion_t::kWall) << " zones " << std::endl;
    std::cout << "Current zone = " << iZone << std::endl;
		std::cout << "fPrismRingHeight = " << fPrismRingHeight.at(iZone) << "  fPrismRingSegmentRadiusInside = " << fPrismRingSegmentRadiusInside << " and fPrismRingSegmentRadiusOutside = " << fPrismRingRadiusOutside << std::endl;

		std::stringstream ss;
		ss << "prismRing" << iZone << std::endl;

		const char * name = ss.str().c_str();
		G4Polyhedra* prismRingSolid = new G4Polyhedra(name,
																								 -fPrismRingSegmentDPhi/2.0, // phi start
																								  fPrismRingSegmentDPhi, //total Phi
																									1, //One side of our NPhi-gon
																									2, RingZ, mainAnnulusRmin, mainAnnulusRmax);

		std::cout << "Making logic" << std::endl;
		if(fPrismRingLogics.at(iZone) != NULL) { delete fPrismRingLogics.at(iZone); }
		fPrismRingLogics.at(iZone) = new G4LogicalVolume(prismRingSolid,
																										 pureWater, name, 0, 0, 0);

		if(fPrismRingPhysics.at(iZone) != NULL) { delete fPrismRingPhysics.at(iZone); }
		std::cout << "Making physic" << std::endl;
//		fPrismRingPhysics.at(iZone) = new G4PVPlacement(0, G4ThreeVector(0,0,0), fPrismRingLogics.at(iZone),
//																									  ss.str().c_str(), false, 0, true);
//
		std::cout << "Region  = kWall, zone = " << iZone;
		std::cout << "  Z cells = " << fWallCellsZ.at(iZone);
		std::cout << "  height = " << fPrismRingHeight.at(iZone);
	  std::cout << "  num prismringlogics = " << fPrismRingLogics.size();
	  std::cout << "  num prismwalllogics = " << fPrismWallLogics.size() << std::endl;
		fPrismRingPhysics.at(iZone) = new G4PVReplica("prismRing",
																									fPrismRingLogics.at(iZone), fPrismWallLogics.at(iZone), kZAxis,
																									(G4int) fWallCellsZ.at(iZone), fPrismRingHeight.at(iZone));
																									std::cout << "Made!" << std::endl;

		if (!fDebugMode)
			fPrismLogic->SetVisAttributes(G4VisAttributes::Invisible);
		else {
			G4VisAttributes* tmpVisAtt = new G4VisAttributes(G4Colour(0, 0.5, 1.));
			tmpVisAtt->SetForceWireframe(true);
			fPrismRingLogics.at(iZone)->SetVisAttributes(tmpVisAtt);
		}
  }
}

void WCSimCherenkovBuilder::CreateRingSegments() {

	// Set up constants (factorise these somewhere else later)
	G4Material * pureWater = WCSimMaterialsBuilder::Instance()->GetMaterial("Water");

	// Now divide each ring (an n-gon prism) into n rectangular segments
	for(unsigned int iSide = 0; iSide < fGeoConfig->GetNSides(); ++iSide)
	{
		// Zip these up into arrays to pass to Geant
		G4double RingZ[2] 			= { -0.5 * fPrismRingSegmentHeight.at(iSide), 0.5 * fPrismRingSegmentHeight.at(iSide)};
		G4double mainAnnulusRmin[2] = { fPrismRingSegmentRadiusInside, fPrismRingSegmentRadiusInside};
		G4double mainAnnulusRmax[2] = { fPrismRingSegmentRadiusOutside, fPrismRingSegmentRadiusOutside };

		std::stringstream ss;
		ss << "segment" << iSide;
		G4String name = ss.str();

		G4Polyhedra* segmentSolid = new G4Polyhedra(name,
												-fPrismRingSegmentDPhi/2.0, // phi start
												fPrismRingSegmentDPhi, //total Phi
												1, //NPhi-gon
												2, RingZ,
												mainAnnulusRmin, mainAnnulusRmax);

		fSegmentLogics.push_back(new G4LogicalVolume(segmentSolid,
																								pureWater,
																								name,
																								0, 0, 0));

		G4RotationMatrix* WCSegmentRotation = new G4RotationMatrix;
		WCSegmentRotation->rotateY((0.5 * fGeoConfig->GetNSides() / 360.0) * deg);

    G4VPhysicalVolume* segmentPhysic = new G4PVPlacement(
    																											WCSegmentRotation,
    																											G4ThreeVector(0.,0., 0.),
    																											fSegmentLogics.at(iSide),
    																											name,
    																											fPrismRingLogics.at(iSide),
    																											false,
    																											iSide);
    fSegmentPhysics.push_back(segmentPhysic);

  	if (!fDebugMode)
  		fSegmentLogics.at(iSide)->SetVisAttributes(G4VisAttributes::Invisible);
  	else {
  		// G4VisAttributes* tmpVisAtt = new G4VisAttributes(
  		// 		G4Colour(1., 0.5, 0.5));
  		// tmpVisAtt->SetForceWireframe(false);
  		// fSegmentLogics.at(iSide)->SetVisAttributes(tmpVisAtt);
  	}

	}

  // Leigh: Add the whitesheet to the normal barrel cells
  for(unsigned int iSide = 0; iSide < fGeoConfig->GetNSides(); ++iSide)
	{
		G4double segmentWhitesheetZ[2] = { -fPrismRingSegmentBSHeight.at(iSide)/2.0,
																				fPrismRingSegmentBSHeight.at(iSide)/2.0 };
		G4double segmentWhitesheetRmax[2] = { fPrismRingSegmentWSRadiusOutside,
												  								fPrismRingSegmentWSRadiusOutside };
		G4double segmentWhitesheetRmin[2] = { fPrismRingSegmentWSRadiusInside,
		                                      fPrismRingSegmentWSRadiusInside};


		std::stringstream ss;
		ss << "segmentBlacksheet" << iSide;
		const char * name = ss.str().c_str();
		G4Polyhedra* segmentWhitesheetSolid = new G4Polyhedra(name,
																-0.5 * fPrismRingSegmentDPhi, // phi start
																fPrismRingSegmentDPhi, //total phi
																1, //NPhi-gon
																2,
																segmentWhitesheetZ,
																segmentWhitesheetRmin,
																segmentWhitesheetRmax);

		G4LogicalVolume * segmentWhitesheetLogic = new G4LogicalVolume( segmentWhitesheetSolid,
																		WCSimMaterialsBuilder::Instance()->GetMaterial("Whitesheet"),
																		name,
																		0, 0, 0);

		G4RotationMatrix* WCSegmentWSRotation = new G4RotationMatrix;
		WCSegmentWSRotation->rotateY(0. * deg);

    G4VPhysicalVolume* segmentWhitesheetPhysic = new G4PVPlacement(
    																											WCSegmentWSRotation,
    																											G4ThreeVector(0.,0., 0.),
    																											segmentWhitesheetLogic,
    																											name,
    																											fSegmentLogics.at(iSide),
    																											false,
    																											iSide);

		G4LogicalBorderSurface * WaterWSBarrelCellSurface = NULL;
		WaterWSBarrelCellSurface = new G4LogicalBorderSurface("WaterWSBarrelCellSurface",
																fSegmentPhysics.at(iSide),
																segmentWhitesheetPhysic,
																WCSimMaterialsBuilder::Instance()->GetOpticalSurface("WaterWSCellSurface"));

	G4VisAttributes* WCBarrelWhitesheetCellVisAtt = new G4VisAttributes(
			G4Colour(0.3, 0.3, 0.3));
	if (fDebugMode )
		segmentWhitesheetLogic->SetVisAttributes(WCBarrelWhitesheetCellVisAtt);
	else
		segmentWhitesheetLogic->SetVisAttributes(G4VisAttributes::Invisible);
	}

	//-------------------------------------------------------------
	// add barrel blacksheet to the normal barrel cells
	// ------------------------------------------------------------

	for(unsigned int iSide = 0; iSide < fGeoConfig->GetNSides(); ++iSide)
	{
		G4double segmentBlacksheetZ[2] = { -fPrismRingSegmentBSHeight.at(iSide)/2.0,
																				fPrismRingSegmentBSHeight.at(iSide)/2.0 };
		G4double segmentBlacksheetRmax[2] = { fPrismRingSegmentBSRadiusOutside,
												  								fPrismRingSegmentBSRadiusOutside };
		G4double segmentBlacksheetRmin[2] = { fPrismRingSegmentBSRadiusInside,
		                                      fPrismRingSegmentBSRadiusInside};


		std::stringstream ss;
		ss << "segmentBlacksheet" << iSide;
		const char * name = ss.str().c_str();
		G4Polyhedra* segmentBlacksheetSolid = new G4Polyhedra(name,
																-0.5 * fPrismRingSegmentDPhi, // phi start
																fPrismRingSegmentDPhi, //total phi
																1, //NPhi-gon
																2,
																segmentBlacksheetZ,
																segmentBlacksheetRmin,
																segmentBlacksheetRmax);

		G4LogicalVolume * segmentBlacksheetLogic = new G4LogicalVolume( segmentBlacksheetSolid,
																		WCSimMaterialsBuilder::Instance()->GetMaterial("Blacksheet"),
																		name,
																		0, 0, 0);


		G4RotationMatrix* WCSegmentBSRotation = new G4RotationMatrix;
		WCSegmentBSRotation->rotateY(0. * deg);

    G4VPhysicalVolume* segmentBlacksheetPhysic = new G4PVPlacement(
    																											WCSegmentBSRotation,
    																											G4ThreeVector(0.,0., 0.),
    																											segmentBlacksheetLogic,
    																											name,
    																											fSegmentLogics.at(iSide),
    																											false,
    																											iSide);

		G4LogicalBorderSurface * WaterBSBarrelCellSurface = NULL;
		WaterBSBarrelCellSurface = new G4LogicalBorderSurface("WaterBSBarrelCellSurface",
																fSegmentPhysics.at(iSide),
																segmentBlacksheetPhysic,
																WCSimMaterialsBuilder::Instance()->GetOpticalSurface("WaterBSCellSurface"));

	G4VisAttributes* WCBarrelBlacksheetCellVisAtt = new G4VisAttributes(
			G4Colour(0.2, 0.9, 0.2));
	if (fDebugMode )
		segmentBlacksheetLogic->SetVisAttributes(
				WCBarrelBlacksheetCellVisAtt);
	else
		segmentBlacksheetLogic->SetVisAttributes(
				G4VisAttributes::Invisible);
	}
}


void WCSimCherenkovBuilder::CreateSegmentCells() {
	// Divide each prism segment into properly scaled unit cells
	// Not used for now

}

void WCSimCherenkovBuilder::PlacePMTs() {

  PlaceBarrelPMTs(); //< Side wall PMTs
  std::cout << "Now placing end cap PMTs!" << std::endl;
  PlaceEndCapPMTs(1); //< Top PMTs
  PlaceEndCapPMTs(-1); //< Bottom PMTs
  return;
}

void WCSimCherenkovBuilder::PlaceBarrelPMTs()
{
	///////////////   Barrel PMT placement
	G4RotationMatrix* WCPMTRotation = new G4RotationMatrix;
	WCPMTRotation->rotateY(90. * deg);

	// We want to fit a given number of unit cells on each prism segment
	// First we'll work out where to put the top left corner of each square unit cell
	// (we may have to leave some space between them)
	// Then we'll place each PMT type relative to that

	G4double segmentWidth = 2. * (fPrismRingSegmentBSRadiusInside) * tan(fPrismRingSegmentDPhi / 2.);

	for(unsigned int iZone = 0; iZone < fGeoConfig->GetNumZones(WCSimGeometryEnums::DetectorRegion_t::kWall); ++iZone)
	{


	        // Clear vectors with extreme PMT positions used to compute Plane Pipes length (zone dependent)
                fBarrelExtremePMTPos_Min.clear();
		fBarrelExtremePMTPos_Max.clear();

		G4double widthPerCell = segmentWidth / fWallCellsX.at(iZone);
		G4double heightPerCell = GetBarrelLengthForCells() / fWallCellsZ.at(iZone);
		std::cout << "segment width = " << segmentWidth << std::endl;
		std::cout << "width per cell = " << widthPerCell << std::endl;
		std::cout << "height per cell " << heightPerCell << std::endl;
		std::cout << "wall cell size = " << fWallCellSize.at(iZone) << std::endl;
		std::cout << "x cells" << fWallCellsX.at(iZone) << std::endl;
		std::cout << "z cells" << fWallCellsZ.at(iZone) << std::endl;
		G4TwoVector unitCellOffset = G4TwoVector(0.5 * (widthPerCell - fWallCellSize.at(iZone)) ,
																						 0.5 * (heightPerCell - fWallCellSize.at(iZone)) );
		std::cout << unitCellOffset << std::endl;
		/* Go-go gadget ascii drawing!
		 * |-----------------------------------------------------|
		 * |             |             |            |            |    /|\
		 * |  /=======\  |             |            |            |     |
		 * |  | unit  |  |             |            |            | heightPerCell
		 * |  | cell  |  |             |            |            |     |
		 * |  \=======/  |             |            |            |     |
		 * |             |             |            |            |    \|/
		 * ------------------------------------------------------|
		 * <-------------------- SEGMENT WIDTH ------------------>
		 *               <------------->
		 *    <------>     widthPerCell
		 *  fWallCellSize
		*/



		std::vector<Double_t> pmtFaceTheta = fGeoConfig->GetCellPMTFaceTheta(WCSimGeometryEnums::DetectorRegion_t::kWall, iZone);
		std::vector<Double_t> pmtFacePhi   = fGeoConfig->GetCellPMTFacePhi(  WCSimGeometryEnums::DetectorRegion_t::kWall, iZone);
		std::vector<WCSimGeometryEnums::PMTDirection_t> pmtFaceType  = fGeoConfig->GetCellPMTFaceType(WCSimGeometryEnums::DetectorRegion_t::kWall, iZone);

		for (G4double i = 0; i < fWallCellsX.at(iZone); i++) {

			// Note that these coordinates get rotated around
			G4double expandedX = fPrismRingSegmentBSRadiusInside;
			G4double expandedY = -0.5 * segmentWidth + i * widthPerCell;
			//G4double expandedY = -0.5 * widthPerCell * i + unitCellOffset.x();
			G4double expandedZ = -0.5 * heightPerCell;  // 0.5 * heightPerCell + unitCellOffset.y(); // Only one row by construction
			std::cout << "Expanded y = " << expandedY << "/" << 0.5*segmentWidth << std::endl;
			//G4double expandedZ = 0.5 * heightPerCell + unitCellOffset.y(); // Only one row by construction
			G4ThreeVector expandedCellPos = G4ThreeVector( expandedX, expandedY, expandedZ); // Bottom-left corner of region containing unit cell
			G4ThreeVector offsetToUnitCell = G4ThreeVector(0, 0.5*(widthPerCell - fWallCellSize.at(iZone)), 0.5*(heightPerCell - fWallCellSize.at(iZone))); // Offset to the bottom left corner of the unit cell itself

			WCSimUnitCell * unitCell = GetWallUnitCell(iZone);
			for(unsigned int nPMT = 0; nPMT < unitCell->GetNumPMTs(); ++nPMT){
				G4TwoVector pmtCellPosition = unitCell->GetPMTPos(nPMT, fWallCellSize.at(iZone)); // PMT position in cell, relative to top left of cell
				std::cout << std::endl << "Placing PMT " << nPMT << "in wall cell " << i << std::endl;
				std::cout << "PMT position in cell = " << pmtCellPosition.x() / m << "," << pmtCellPosition.y() / m << "in m" << std::endl;
				std::cout << "Cell size = " << fWallCellSize.at(iZone) << std::endl;// pmtCellPosition.x() << "," << pmtCellPosition.y() << std::endl;
				G4ThreeVector PMTPosition = expandedCellPos + offsetToUnitCell   // bottom left of unit cell
																		+ G4ThreeVector(0, 0, fWallCellSize.at(iZone)) // bottom left to top left of cell
																		+ G4ThreeVector(0, pmtCellPosition.x(), -1.0*pmtCellPosition.y()); // top left of cell to PMT
				std::cout << "Position = " << PMTPosition << "   rotation = " << WCPMTRotation << std::endl;
				std::cout << "Cell mother height and width: " << heightPerCell/2. << "  "  << segmentWidth/2. << std::endl;
				WCSimPMTConfig config = unitCell->GetPMTPlacement(nPMT).GetPMTConfig();
				std::cout << " PMT logical volume name = " << fPMTBuilder.GetPMTLogicalVolume(config)->GetName() << std::endl;

				//----->  PMT rotation and shift ...
				G4RotationMatrix * tmpWCPMTRotation =  new G4RotationMatrix(*WCPMTRotation); // tmpWCPMTRotation is needed to avoid satring rotation drifting
														
				if(pmtFaceType.at(nPMT) == WCSimGeometryEnums::PMTDirection_t::kArbitrary) 
				      *tmpWCPMTRotation *= GetArbitraryPMTFaceRotation( pmtFaceTheta.at(nPMT), pmtFacePhi.at(nPMT));
				else  *tmpWCPMTRotation *= GetBarrelPMTFaceRotation(    pmtFaceType.at(nPMT),  iZone);


	//Stefano : Plane Pipes
	double pomD = ( fGeoConfig->UsePMTSupport( WCSimGeometryEnums::PMTSupport_t::kPOM ) ) ? 2*fPlanePipeRadius + fPMTHolderLength + 5*mm : 0;
	 
        // Leigh: Veto specialities.
        G4ThreeVector vetoShift;
        if(pmtFaceType.at(nPMT) == WCSimGeometryEnums::PMTDirection_t::kVeto){
          G4RotationMatrix rotation;
          rotation.rotateZ(180.*deg);
          *tmpWCPMTRotation *= rotation;
          vetoShift.setX(fBlacksheetThickness + fWhitesheetThickness);

	  pomD = 0;
        }	

       PMTPosition += vetoShift;


				//	double radius = std::max((config.GetLCConfig()).GetMaxRadius(), config.GetRadius()       );			
				double radius = config.GetMaxRadius();			
				double dTheta = fabs( tmpWCPMTRotation->thetaY()/deg -90 );
				double dR     = radius*sin(dTheta*deg);
			
                                // Shift PMT postion for Roation and Plane Pipes, shift should be zero if not needed
                                G4ThreeVector PMTShift( -dR -pomD, 0, 0);
                                PMTPosition += PMTShift;
				// <<----                               <---------
				
				// G4VPhysicalVolume* physiWCBarrelPMT =
				new G4PVPlacement(tmpWCPMTRotation,     // its rotation
													PMTPosition,
													fPMTBuilder.GetPMTLogicalVolume(config),        // its logical volume //
													("WCPMT_"+config.GetPMTName()).c_str(),           // its name
													fSegmentLogics.at(iZone),      // its mother volume
													false,             // no boolean operations
													fNumPMTs, true);
				fNumPMTs++;

				// logicWCPMT->GetDaughter(0),physiCapPMT is the glass face. If you add more
				// daugter volumes to the PMTs (e.g. a acryl cover) you have to check, if
				// this is still the case.

                                //Stefano: Plane Pipes

                                // loop over Z pos: only need one min and maz per Z
                                G4ThreeVector PipeBackShift( pomD/2-fPlanePipeRadius, 0, 0);
                                bool isNewZ = true;
                                int iZ = -1;
                                for(int zit = 0; zit< fBarrelExtremePMTPos_Max.size(); ++zit){
                                  if( PMTPosition.z() == (fBarrelExtremePMTPos_Max.at(zit)).z() ){

                                    isNewZ = false;
                                    iZ = zit;

                                  }  // if zit.Z
				}// for zit

                                if(isNewZ) {
				  // std::cout << "Plane Pipe : New Position! " << std::endl;
                                  fBarrelExtremePMTPos_Max.push_back(PMTPosition+PipeBackShift);
                                  fBarrelExtremePMTPos_Min.push_back(PMTPosition+PipeBackShift);
                                } else if( PMTPosition.y() < (fBarrelExtremePMTPos_Min.at(iZ)).y() ){
				  // std::cout << "Plane Pipe : New Min Y! " << std::endl;
                                  fBarrelExtremePMTPos_Min[iZ] = PMTPosition+PipeBackShift;
                                } else if( PMTPosition.y() > (fBarrelExtremePMTPos_Max.at(iZ)).y() ){
				  // std::cout << "Plane Pipe : New Max Y! " << std::endl;
                                  fBarrelExtremePMTPos_Max[iZ] = PMTPosition+PipeBackShift;
                                }// if isNewZ

			}
		}
		std::cout << "PMTs placed on walls!" << std::endl;

		/// Plane Pipes placement needs to be just after the corresponding PMT placement
		if( fGeoConfig->UsePMTSupport( WCSimGeometryEnums::PMTSupport_t::kPOM  ) )  PlaceBarrelPlanePipes(iZone);

	} // for iZone
}

WCSimGeoConfig * WCSimCherenkovBuilder::GetGeoConfig() const {
	return fGeoConfig;
}

void WCSimCherenkovBuilder::GetMeasurements()
{
    if( fGotMeasurements) { return; }
    double epsilon = 0.00001 * mm; // Add a tiny bit of gap between supposedly overlapping volumes 
                                   // to prevent Geant4 getting stuck at the tracking action
    int nEpsilons = 6;

    // Barrel measurements
    fBarrelRadius = fGeoConfig->GetOuterRadius() + 1*m; // Make it 1m bigger in radius
    fBarrelHeight = fGeoConfig->GetInnerHeight() + 2* fBlacksheetThickness + 2 * GetMaxCapExposeHeight() + 2*m;  // and half-height
    fBarrelLengthForCells = fGeoConfig->GetInnerHeight() - 2.0 * GetMaxCapExposeHeight();

    fPrismRadiusInside = fGeoConfig->GetInnerRadius() - epsilon;
    fPrismRadiusOutside = fGeoConfig->GetInnerRadius() + fBlacksheetThickness + fWhitesheetThickness + 2*GetMaxBarrelExposeHeight() + (2*fPlanePipeRadius + fPMTHolderLength + 5*mm) + nEpsilons * epsilon;   
    fPrismHeight = fBarrelLengthForCells;

    // Leigh: Veto considerations
    fVetoSize = fGeoConfig->GetVetoSize();
    // Make sure we leave room for veto PMTs if fVetoSize wasn't set.
    if(fVetoSize < GetMaxBarrelExposeHeight()){
      fVetoSize = GetMaxBarrelExposeHeight();
    }
    if(fVetoSize < GetMaxCapExposeHeight()){
      fVetoSize = GetMaxCapExposeHeight();
    }

    fBarrelRadius += fVetoSize;
    fBarrelHeight += 2*fVetoSize;
    fVetoRadiusInside = fPrismRadiusOutside + fVetoSize;
    fVetoRadiusOutside = fVetoRadiusInside + fWhitesheetThickness;
    fVetoHeight = fBarrelHeight - 2*fVetoSize;//fPrismHeight + 2*vetoSize;
    assert( --nEpsilons > 0);

    fPrismWallRadiusInside  = fPrismRadiusInside + epsilon;
    fPrismWallRadiusOutside = fPrismRadiusOutside - epsilon;

    assert(--nEpsilons > 0);
    fPrismRingRadiusInside = fPrismRadiusInside + epsilon;
    fPrismRingRadiusOutside = fPrismRadiusOutside - epsilon;

    assert(--nEpsilons > 0);
//    fPrismRingSegmentRadiusInside = fPrismRadiusInside-1*mm + epsilon;
//    fPrismRingSegmentRadiusOutside = fPrismRadiusOutside+1*mm - epsilon;
    fPrismRingSegmentRadiusInside = fPrismRadiusInside+1*mm + epsilon;
    fPrismRingSegmentRadiusOutside = fPrismRadiusOutside-1*mm - epsilon;
    fPrismRingSegmentDPhi = 360*deg / fGeoConfig->GetNSides();

    assert(--nEpsilons > 0);
    fPrismRingSegmentBSRadiusInside = fPrismRingSegmentRadiusInside + GetMaxBarrelExposeHeight() + (2*fPlanePipeRadius + fPMTHolderLength + 5*mm);  // make room for pmt/LC  and  pipes
    fPrismRingSegmentBSRadiusOutside = fPrismRingSegmentBSRadiusInside + fBlacksheetThickness - epsilon;

    assert(--nEpsilons > 0);
    fPrismRingSegmentWSRadiusInside = fPrismRingSegmentBSRadiusOutside + epsilon;
    fPrismRingSegmentWSRadiusOutside = fPrismRingSegmentWSRadiusInside + fWhitesheetThickness;
/*    
    // Cap measurements
	  fCapAssemblyHeight = fBlacksheetThickness + fWhitesheetThickness + 2*GetMaxCapExposeHeight() + 0.5 * (fGeoConfig->GetInnerHeight() - fBarrelLengthForCells);
    // nb. fCapAssemblyHeight = fCapRingHeight + fCapPolygonHeight
//	  fCapAssemblyRadius = fBarrelRadius;
	  fCapAssemblyRadius = fBarrelRadius - fVetoSize;

    fVetoCapHeight = fCapAssemblyHeight + 2*fVetoSize;
    fVetoCapRadius = fCapAssemblyRadius;

	  fCapRingRadiusInside = fPrismRadiusInside;
	  fCapRingRadiusOutside = fPrismRadiusOutside;
//	  fCapRingHeight = 0.5 * (fGeoConfig->GetInnerHeight() - fBarrelLengthForCells);
	  fCapRingHeight = GetMaxCapExposeHeight();

	  fCapRingSegmentDPhi = fPrismRingSegmentDPhi;
//	  fCapRingSegmentRadiusInside = fPrismRadiusInside;
//	  fCapRingSegmentRadiusOutside = fPrismRadiusOutside;
	  fCapRingSegmentRadiusInside = fPrismRingSegmentRadiusInside;
	  fCapRingSegmentRadiusOutside = fPrismRingSegmentRadiusOutside;
	  fCapRingSegmentHeight = fCapRingHeight;

//	  fCapRingSegmentBSRadiusInside = fCapRingSegmentRadiusOutside - fBlacksheetThickness - 2*epsilon;
//	  fCapRingSegmentBSRadiusOutside = fCapRingSegmentRadiusOutside - epsilon;
//	  fCapRingSegmentBSHeight = fCapRingSegmentHeight;

	  fCapRingSegmentBSRadiusInside = fPrismRingSegmentBSRadiusInside;
	  fCapRingSegmentBSRadiusOutside = fPrismRingSegmentBSRadiusOutside - epsilon;
	  fCapRingSegmentBSHeight = fCapRingSegmentHeight;

	  fCapRingSegmentWSRadiusInside = fPrismRingSegmentWSRadiusInside + epsilon;
	  fCapRingSegmentWSRadiusOutside = fPrismRingSegmentWSRadiusOutside;
	  fCapRingSegmentWSHeight = fCapRingSegmentHeight;

	  fCapPolygonRadius = fGeoConfig->GetInnerRadius() + 2*GetMaxBarrelExposeHeight() + fBlacksheetThickness + fWhitesheetThickness;
	  fCapPolygonHeight = 2*GetMaxCapExposeHeight() + fBlacksheetThickness + fWhitesheetThickness;

    fCapPolygonCentreRadius = fCapRingSegmentBSRadiusInside - epsilon;
//fGeoConfig->GetInnerRadius() + GetMaxBarrelExposeHeight();
    fCapPolygonCentreHeight = GetMaxCapExposeHeight();

//	  fCapPolygonEdgeBSRadiusInside = fGeoConfig->GetInnerRadius() + GetMaxBarrelExposeHeight();
//	  fCapPolygonEdgeBSRadiusOutside = fCapPolygonRadius;
	  fCapPolygonEdgeBSRadiusInside = fCapRingSegmentBSRadiusInside;
	  fCapPolygonEdgeBSRadiusOutside = fCapRingSegmentBSRadiusOutside;
	  fCapPolygonEdgeBSHeight = 0.5*fCapPolygonHeight;


	  fCapPolygonEdgeWSRadiusInside = fCapRingSegmentWSRadiusInside;
	  fCapPolygonEdgeWSRadiusOutside = fCapRingSegmentWSRadiusOutside;
	  fCapPolygonEdgeWSHeight = 0.5*fCapPolygonHeight;

//	  fCapPolygonEndBSRadius = fGeoConfig->GetInnerRadius() + GetMaxBarrelExposeHeight();
	  fCapPolygonEndBSRadius = fCapPolygonCentreRadius;
	  fCapPolygonEndBSHeight = fBlacksheetThickness;

	  fCapPolygonEndWSRadius = fCapPolygonCentreRadius;
	  fCapPolygonEndWSHeight = fWhitesheetThickness;
*/
    // Leigh: New cap method, simplifies things a lot.
    fCapAssemblyRadius = fPrismRingSegmentWSRadiusOutside + GetMaxBarrelExposeHeight() - epsilon;
    fCapAssemblyHeight = 2*GetMaxCapExposeHeight() + fWhitesheetThickness + fBlacksheetThickness;

    // The cap ring, segments and black and white sheet layers
	  fCapRingRadiusInside = fPrismRingRadiusInside;
	  fCapRingRadiusOutside = fPrismRingRadiusOutside;
	  fCapRingHeight = GetMaxCapExposeHeight() - epsilon;

    std::cout << "-Radii: " << fCapAssemblyRadius << ", " << fCapRingRadiusInside << ", " << fCapRingRadiusOutside << std::endl;

	  fCapRingSegmentDPhi = fPrismRingSegmentDPhi;
	  fCapRingSegmentRadiusInside = fPrismRingSegmentRadiusInside;
	  fCapRingSegmentRadiusOutside = fPrismRingSegmentRadiusOutside;
	  fCapRingSegmentHeight = fCapRingHeight;

	  fCapRingSegmentBSRadiusInside = fPrismRingSegmentBSRadiusInside;
	  fCapRingSegmentBSRadiusOutside = fPrismRingSegmentBSRadiusOutside;
	  fCapRingSegmentBSHeight = fCapRingSegmentHeight;

	  fCapRingSegmentWSRadiusInside = fPrismRingSegmentWSRadiusInside;
	  fCapRingSegmentWSRadiusOutside = fPrismRingSegmentWSRadiusOutside;
	  fCapRingSegmentWSHeight = fCapRingSegmentHeight;

    std::cout << "-Radii: " << std::setprecision(15) << fPrismRingSegmentRadiusInside << ", " 
                            << std::setprecision(15) << fPrismRingSegmentRadiusOutside << ", "
                            << std::setprecision(15) << fPrismRingSegmentBSRadiusInside << ", "
                            << std::setprecision(15) << fPrismRingSegmentBSRadiusOutside << ", "
                            << std::setprecision(15) << fPrismRingSegmentWSRadiusInside << ", "
                            << std::setprecision(15) << fPrismRingSegmentWSRadiusOutside << std::endl;

    std::cout << "-Radii: " << std::setprecision(15) << fCapRingSegmentRadiusInside << ", " 
                            << std::setprecision(15) << fCapRingSegmentRadiusOutside << ", "
                            << std::setprecision(15) << fCapRingSegmentBSRadiusInside << ", "
                            << std::setprecision(15) << fCapRingSegmentBSRadiusOutside << ", "
                            << std::setprecision(15) << fCapRingSegmentWSRadiusInside << ", "
                            << std::setprecision(15) << fCapRingSegmentWSRadiusOutside << std::endl;

    // The actual flat layers of black and white sheet that form the caps
	  fCapPolygonEndBSRadius = fCapRingSegmentBSRadiusOutside;
	  fCapPolygonEndBSHeight = fBlacksheetThickness - epsilon;

	  fCapPolygonEndWSRadius = fCapRingSegmentWSRadiusOutside;
	  fCapPolygonEndWSHeight = fWhitesheetThickness - epsilon;

    // Now optimize the cell sizes to work out the prism ring heights:
    // Note that CalculateCellSizes needs fCapPolygonRadius
    CalculateCellSizes();
    fPrismRingHeight.resize(fGeoConfig->GetNSides());
    fPrismRingSegmentHeight.resize(fGeoConfig->GetNSides());
    fPrismRingSegmentBSHeight.resize(fGeoConfig->GetNSides());
    for(unsigned int iZone = 0; iZone < fGeoConfig->GetNumZones(WCSimGeometryEnums::DetectorRegion_t::kWall); ++iZone)
    {
      // std::cout << "I think there are " << fWallCellsZ.at(iZone) * fWallCellsX.at(iZone) << " wall cells in zone " << iZone << std::endl;
    	fPrismRingHeight.at(iZone) = fPrismHeight / fWallCellsZ.at(iZone);
    	fPrismRingSegmentHeight.at(iZone) = fPrismRingHeight.at(iZone) - epsilon;
    	fPrismRingSegmentBSHeight.at(iZone) = fPrismRingSegmentHeight.at(iZone) - epsilon;
    }

    fGotMeasurements = true;
}

double WCSimCherenkovBuilder::GetBarrelLengthForCells() {
	return fGeoConfig->GetInnerHeight() - 2.0 * GetMaxCapExposeHeight();
}

double WCSimCherenkovBuilder::GetMaxCapExposeHeight() {
	double maxHeight = 0.0;
	for(unsigned int iZone = 0; iZone < fGeoConfig->GetNumZones(WCSimGeometryEnums::DetectorRegion_t::kTop); ++iZone)
	{
		WCSimUnitCell * cell = GetTopUnitCell(iZone);
		double expHeight = cell->GetCellExposeHeight();
		if(expHeight > maxHeight) { maxHeight = expHeight; }
	}

	for(unsigned int iZone = 0; iZone < fGeoConfig->GetNumZones(WCSimGeometryEnums::DetectorRegion_t::kBottom); ++iZone)
	{
		WCSimUnitCell * cell2 = GetBottomUnitCell(iZone);
		double expHeight2 = cell2->GetCellExposeHeight();
		if(expHeight2 > maxHeight) { maxHeight = expHeight2; }
	}

	//Stefano: make room for pipes and PMT holder
	if(fGeoConfig->UsePMTSupport( WCSimGeometryEnums::PMTSupport_t::kPOM ) ) maxHeight += 2*fPlanePipeRadius + fPMTHolderLength;

	return maxHeight;
}

double WCSimCherenkovBuilder::GetMaxBarrelExposeHeight() {
	double maxHeight = 0.0;
	for(unsigned int iZone = 0; iZone < fGeoConfig->GetNumZones(WCSimGeometryEnums::DetectorRegion_t::kWall); ++iZone)
	{
		WCSimUnitCell * cell = GetWallUnitCell(iZone);
		double expHeight = cell->GetCellExposeHeight();
		if( expHeight > maxHeight ) { maxHeight = expHeight; }
	}
		return maxHeight;
}

void WCSimCherenkovBuilder::ConstructUnitCells() {
  fGeoConfig->Print();

  // Wall cells:
  fWallUnitCells.clear();
	for(unsigned int iZone = 0; iZone < fGeoConfig->GetNumZones(WCSimGeometryEnums::DetectorRegion_t::kWall); ++iZone)
  {
  	std::vector<double> pmtX = fGeoConfig->GetCellPMTX(WCSimGeometryEnums::DetectorRegion_t::kWall, iZone);
  	std::vector<double> pmtY = fGeoConfig->GetCellPMTY(WCSimGeometryEnums::DetectorRegion_t::kWall, iZone);
    std::vector<std::string> pmtNames = fGeoConfig->GetCellPMTName(WCSimGeometryEnums::DetectorRegion_t::kWall, iZone);
    assert( pmtX.size() == pmtY.size() && pmtX.size() == pmtNames.size() );

		WCSimUnitCell * cell = new WCSimUnitCell();
		for(unsigned int i = 0; i < pmtNames.size(); ++i){
			std::cout << "PMT = " << pmtNames.at(i) << "  X = " << pmtX.at(i) << " / m = " << pmtX.at(i) / m << std::endl;
			WCSimPMTConfig config = fPMTManager->GetPMTByName(pmtNames.at(i));
			cell->AddPMT(config, pmtX.at(i), pmtY.at(i));
		}
	  cell->Print();
		fWallUnitCells.push_back(cell);
	}

  // Top cells:
  fTopUnitCells.clear();
	for(unsigned int iZone = 0; iZone < fGeoConfig->GetNumZones(WCSimGeometryEnums::DetectorRegion_t::kTop); ++iZone)
  {
  	std::vector<double> pmtX = fGeoConfig->GetCellPMTX(WCSimGeometryEnums::DetectorRegion_t::kTop, iZone);
  	std::vector<double> pmtY = fGeoConfig->GetCellPMTY(WCSimGeometryEnums::DetectorRegion_t::kTop, iZone);
    std::vector<std::string> pmtNames = fGeoConfig->GetCellPMTName(WCSimGeometryEnums::DetectorRegion_t::kTop, iZone);
    assert( pmtX.size() == pmtY.size() && pmtX.size() == pmtNames.size() );

		WCSimUnitCell * cell = new WCSimUnitCell();
		for(unsigned int i = 0; i < pmtNames.size(); ++i){
			std::cout << "PMT = " << pmtNames.at(i) << "  X = " << pmtX.at(i) << " / m = " << pmtX.at(i) / m << std::endl;
			WCSimPMTConfig config = fPMTManager->GetPMTByName(pmtNames.at(i));
			cell->AddPMT(config, pmtX.at(i), pmtY.at(i));
		}
	  cell->Print();
		fTopUnitCells.push_back(cell);
	}

  // Bottom cells:
  fBottomUnitCells.clear();
	for(unsigned int iZone = 0; iZone < fGeoConfig->GetNumZones(WCSimGeometryEnums::DetectorRegion_t::kBottom); ++iZone)
  {
  	std::vector<double> pmtX = fGeoConfig->GetCellPMTX(WCSimGeometryEnums::DetectorRegion_t::kBottom, iZone);
  	std::vector<double> pmtY = fGeoConfig->GetCellPMTY(WCSimGeometryEnums::DetectorRegion_t::kBottom, iZone);
    std::vector<std::string> pmtNames = fGeoConfig->GetCellPMTName(WCSimGeometryEnums::DetectorRegion_t::kBottom, iZone);
    assert( pmtX.size() == pmtY.size() && pmtX.size() == pmtNames.size() );

		WCSimUnitCell * cell = new WCSimUnitCell();
		for(unsigned int i = 0; i < pmtNames.size(); ++i){
			std::cout << "PMT = " << pmtNames.at(i) << "  X = " << pmtX.at(i) << " / m = " << pmtX.at(i) / m << std::endl;
			WCSimPMTConfig config = fPMTManager->GetPMTByName(pmtNames.at(i));
			cell->AddPMT(config, pmtX.at(i), pmtY.at(i));
		}
	  cell->Print();
		fBottomUnitCells.push_back(cell);
	}

	assert(fWallUnitCells.size() != 0);
	assert(fTopUnitCells.size() != 0);
	assert(fBottomUnitCells.size() != 0);
	return;
}

WCSimUnitCell* WCSimCherenkovBuilder::GetUnitCell(
		WCSimGeometryEnums::DetectorRegion_t region, int zone)
{
	WCSimUnitCell * cell = NULL;
	if( region == WCSimGeometryEnums::DetectorRegion_t::kTop ){ cell = GetTopUnitCell(zone); }
	if( region == WCSimGeometryEnums::DetectorRegion_t::kBottom ){ cell = GetBottomUnitCell(zone); }
	if( region == WCSimGeometryEnums::DetectorRegion_t::kWall ){ cell = GetWallUnitCell(zone); }
	assert(cell != NULL);
	return cell;
}

WCSimUnitCell* WCSimCherenkovBuilder::GetTopUnitCell(int zoneNum) {
	if( fTopUnitCells.size() == 0 ){
		this->ConstructUnitCells();
	}

	return fTopUnitCells.at(zoneNum);
}

WCSimUnitCell* WCSimCherenkovBuilder::GetWallUnitCell(int zoneNum) {
	if( fWallUnitCells.size() == 0 ){ this->ConstructUnitCells(); }
	return fWallUnitCells.at(zoneNum);
}

WCSimUnitCell* WCSimCherenkovBuilder::GetBottomUnitCell(int zone)
{
	if( fBottomUnitCells.size() == 0 ){
		this->ConstructUnitCells();
	}
	return fBottomUnitCells.at(zone);
}

void WCSimCherenkovBuilder::CalculateCellSizes() {
	fWallCellSize.clear();
	fTopCellSize.clear();
	fBottomCellSize.clear();
	fWallCellSize.resize(fGeoConfig->GetNumZones(WCSimGeometryEnums::DetectorRegion_t::kWall), 0.0);
	fTopCellSize.resize(fGeoConfig->GetNumZones(WCSimGeometryEnums::DetectorRegion_t::kTop), 0.0);
	fBottomCellSize.resize(fGeoConfig->GetNumZones(WCSimGeometryEnums::DetectorRegion_t::kBottom), 0.0);

	for(unsigned int iZone = 0; iZone < fGeoConfig->GetNumZones(WCSimGeometryEnums::DetectorRegion_t::kWall); ++iZone)
	{
		fWallCellSize.at(iZone) = GetOptimalWallCellSize(iZone);
	}

	for(unsigned int iZone = 0; iZone < fGeoConfig->GetNumZones(WCSimGeometryEnums::DetectorRegion_t::kTop); ++iZone)
	{
		fTopCellSize.at(iZone) = GetOptimalTopCellSize(iZone);
	}

	for(unsigned int iZone = 0; iZone < fGeoConfig->GetNumZones(WCSimGeometryEnums::DetectorRegion_t::kBottom); ++iZone)
	{
		fBottomCellSize.at(iZone) = GetOptimalBottomCellSize(iZone);
	}
	assert(fWallCellSize.size() > 0);
	assert(fTopCellSize.size() > 0);
	assert(fBottomCellSize.size() > 0);
}

double WCSimCherenkovBuilder::GetOptimalEndcapCellSize(WCSimGeometryEnums::DetectorRegion_t region, int zoneNum) {
	// Optimizing how many squares you can fit in a polygon
	// is in general non-trivial.  Here we'll lay them out in a regular
	// grid and then iterate making the cells slightly smaller or larger
	// to get close to the desired coverage.
  // std::cout << " *** WCSimCherenkovBuilder::GetOptimalTopCellSide *** " << std::endl;
  std::cout << "Getting cell size for " << region.AsString() << ", zone " << zoneNum << std::endl;


	// Fit our n-gon inside a square:
	WCSimUnitCell endcapCell = *(GetUnitCell(region, zoneNum));
  std::vector<double> *endcapCellSize;

  if( region == WCSimGeometryEnums::DetectorRegion_t::kTop)
  {
		endcapCellSize = &fTopCellSize;
  }
  else if( region == WCSimGeometryEnums::DetectorRegion_t::kBottom )
  {
		endcapCellSize = &fBottomCellSize;
  }
  else
  {
    std::cerr << "Error: GetOptimalEndcapCellSize must only process kTop and kBottom regions" << std::endl;
    assert(kFALSE);
  }
	

  int maxNumCells = -1;
  double bestCoverage = 0.0;
  double targetCoverage = 0.0;
  bool canHaveMorePMTs = true;
  double defaultSide = 0.0;
  double bestSide = 0.0;
  int bestIter = 0;

  if(fGeoConfig->GetLimitPMTNumber())
	{
		double pmtArea = endcapCell.GetPhotocathodeArea();
		maxNumCells = fGeoConfig->GetMaxZoneCells(region, zoneNum);
		double maxCoverage =   pmtArea
							 * maxNumCells
							 / WCSimPolygonTools::GetSliceAreaFromAngles(
									 fGeoConfig->GetNSides(),
									 fGeoConfig->GetOuterRadius(),
									 fGeoConfig->GetZoneThetaStart(region, zoneNum),
									 fGeoConfig->GetZoneThetaEnd(region, zoneNum));
    
	  std::cout << "Maximum coverage with available PMTs for region " << region.AsString() << ", zone " << zoneNum << " = " << maxCoverage << std::endl;
      std::cout << "Uses " << maxNumCells << " cells, where each cell is " << endcapCell.GetCellSizeForCoverage(maxCoverage) << " in size" << std::endl;
		targetCoverage = maxCoverage;
		defaultSide = endcapCell.GetCellSizeForCoverage(targetCoverage);
		canHaveMorePMTs = false;
	}
	else
	{
		if(fGeoConfig->GetUseOverallCoverage() || fGeoConfig->GetUseZonalCoverage()){
			targetCoverage = GetZoneCoverage(region, zoneNum);
			defaultSide =  endcapCell.GetCellSizeForCoverage(targetCoverage);
		  // Just make sure it's a high number
		  maxNumCells = (int)(  targetCoverage
							  * WCSimPolygonTools::GetSliceAreaFromAngles(fGeoConfig->GetNSides(),
																	fGeoConfig->GetOuterRadius(),
																	fGeoConfig->GetZoneThetaStart(region, zoneNum),
																	fGeoConfig->GetZoneThetaEnd(region, zoneNum))
							  / endcapCell.GetPhotocathodeArea());
	    std::cout << "Finding coverage for region " << region.AsString() << ", zone " << zoneNum << " = " << targetCoverage << std::endl;
      std::cout << "Target coverage = " << targetCoverage << " with a cell containing " << endcapCell.GetNumPMTs() << " PMTs" << std::endl;
      std::cout << "Cell photocathode area = " << endcapCell.GetPhotocathodeArea() << " so we need " ;
      std::cout << maxNumCells << " to cover a zone area of " << WCSimPolygonTools::GetSliceAreaFromAngles(fGeoConfig->GetNSides(),
																	                                  fGeoConfig->GetOuterRadius(),
																	                                  fGeoConfig->GetZoneThetaStart(region, zoneNum),
																	                                  fGeoConfig->GetZoneThetaEnd(region, zoneNum)) << std::endl;

		}
		else{
//			std::cerr << "Error: can't tell what coverage type fGeoConfig should use - bailing out" << std::endl;
			assert(0);
		}
		canHaveMorePMTs = true;
	}

  int cellsInPolygon = 0; 
  int bestNumCells = 0;
  double thetaStart = fGeoConfig->GetZoneThetaStart(region, zoneNum);
  double thetaEnd = fGeoConfig->GetZoneThetaEnd(region, zoneNum);
//  double capPolygonOuterRadius = WCSimPolygonTools::GetOuterRadiusFromInner(fGeoConfig->GetNSides(), fCapPolygonCentreRadius);
  double capPolygonOuterRadius = WCSimPolygonTools::GetOuterRadiusFromInner(fGeoConfig->GetNSides(), fCapRingRadiusInside-0.0001*mm);
	double squareSide = 2.0*(capPolygonOuterRadius);
	double cellSide = defaultSide;
  
  // Angled walls mean squares don't tile very well
  // We'll consider their positions, but we only need to get these vectors once
  // if they live outside the loop.;
  std::vector<Double_t> pmtX = fGeoConfig->GetCellPMTX(region, zoneNum);
  std::vector<Double_t> pmtY = fGeoConfig->GetCellPMTY(region, zoneNum);
  std::vector<std::string> pmtNames = fGeoConfig->GetCellPMTName(region, zoneNum);
  std::vector<Double_t> pmtRad;
  for (unsigned int iPMT = 0; iPMT < pmtNames.size(); ++iPMT)
  {
	  pmtRad.push_back(fPMTManager->GetPMTByName(pmtNames.at(iPMT)).GetRadius());
  }
  
  int sinceImprovement = 0;
  for(int iIter = 0; iIter < 100; ++iIter)
  {
    ++sinceImprovement;
    cellsInPolygon = 0;
	  double xPos = 0.0;
	  while( xPos < squareSide )
	  {
		  double yPos = 0.0;
		  while( fabs(yPos) < squareSide )
		  {
			  G4TwoVector centreToTopLeftSquare = G4TwoVector(-0.5*squareSide, 0.5 * squareSide);
			  G4TwoVector topLeftCell     = G4TwoVector(xPos, -yPos) + centreToTopLeftSquare;
			  G4TwoVector topRightCell     = G4TwoVector(xPos+cellSide, -yPos) + centreToTopLeftSquare;
			  G4TwoVector bottomLeftCell     = G4TwoVector(xPos, -yPos-cellSide) + centreToTopLeftSquare;
			  G4TwoVector bottomRightCell     = G4TwoVector(xPos+cellSide, -yPos-cellSide) + centreToTopLeftSquare;

			  bool cellInPolygonSlice = true;
			  for( unsigned int iPMT = 0; iPMT < pmtNames.size(); ++iPMT)
			  {
				// Coordinates the corner of a square containing the PMT itself, relative to the top-left corner of the unit cell
				G4TwoVector pmtTopLeft(     cellSide * pmtX.at(iPMT)/m - pmtRad.at(iPMT), cellSide * pmtY.at(iPMT)/m + pmtRad.at(iPMT));
				G4TwoVector pmtTopRight(    cellSide * pmtX.at(iPMT)/m + pmtRad.at(iPMT), cellSide * pmtY.at(iPMT)/m + pmtRad.at(iPMT));
				G4TwoVector pmtBottomLeft(  cellSide * pmtX.at(iPMT)/m - pmtRad.at(iPMT), cellSide * pmtY.at(iPMT)/m - pmtRad.at(iPMT));
				G4TwoVector pmtBottomRight( cellSide * pmtX.at(iPMT)/m + pmtRad.at(iPMT), cellSide * pmtY.at(iPMT)/m - pmtRad.at(iPMT));

				//	if(    !(WCSimPolygonTools::PolygonSliceContains(fGeoConfig->GetNSides(), thetaStart, thetaEnd, capPolygonOuterRadius, topLeftCell + pmtTopLeft))
				//		  || !(WCSimPolygonTools::PolygonSliceContains(fGeoConfig->GetNSides(), thetaStart, thetaEnd, capPolygonOuterRadius, topLeftCell + pmtTopRight))
				//		  || !(WCSimPolygonTools::PolygonSliceContains(fGeoConfig->GetNSides(), thetaStart, thetaEnd, capPolygonOuterRadius, topLeftCell + pmtBottomLeft))
				//		  || !(WCSimPolygonTools::PolygonSliceContains(fGeoConfig->GetNSides(), thetaStart, thetaEnd, capPolygonOuterRadius, topLeftCell + pmtBottomRight)) ){
					if(    !(WCSimPolygonTools::PolygonSliceContains(fGeoConfig->GetNSides(), thetaStart, thetaEnd, capPolygonOuterRadius, topLeftCell))
						  || !(WCSimPolygonTools::PolygonSliceContains(fGeoConfig->GetNSides(), thetaStart, thetaEnd, capPolygonOuterRadius, bottomLeftCell))
						  || !(WCSimPolygonTools::PolygonSliceContains(fGeoConfig->GetNSides(), thetaStart, thetaEnd, capPolygonOuterRadius, topRightCell))
						  || !(WCSimPolygonTools::PolygonSliceContains(fGeoConfig->GetNSides(), thetaStart, thetaEnd, capPolygonOuterRadius, bottomRightCell)) ){
					  cellInPolygonSlice = false;
					}
			  }
			  cellsInPolygon += cellInPolygonSlice;
			  yPos = yPos + cellSide;
		  }
		  xPos = xPos + cellSide;
	  }

	  if(fGeoConfig->GetLimitPMTNumber())
	  {
		  if( cellsInPolygon <= maxNumCells && cellsInPolygon >= bestNumCells )
		  {
//			  std::cout << "Updating best number of endcap cells in zone " << zoneNum << " to ";
			  bestNumCells = cellsInPolygon;
			  bestSide = cellSide;
        sinceImprovement = 0;
        if(iIter != 100)
        {
          bestIter = iIter;
        }
//			  std::cout << bestNumCells << " with cell size = " << bestSide << std::endl;
		  }
      if( cellsInPolygon == maxNumCells && !canHaveMorePMTs)
      {
        break;
      }
		  if( cellsInPolygon < maxNumCells )
		  {
        cellSide *= ( 1. - 1./maxNumCells - cellsInPolygon/(100.0*maxNumCells));
		  }
		  if( cellsInPolygon > maxNumCells)
		  {
			  cellSide /= (1. - 1./maxNumCells - cellsInPolygon/(200.0*maxNumCells));
		  }
	  }
	  else if( !fGeoConfig->GetLimitPMTNumber() )
	  {
		  double pmtArea = endcapCell.GetPhotocathodeArea();
		  double coverage =   pmtArea * cellsInPolygon
							/ WCSimPolygonTools::GetSliceAreaFromAngles(
							  fGeoConfig->GetNSides(),
							  fGeoConfig->GetOuterRadius(),
							  fGeoConfig->GetZoneThetaStart(region, zoneNum),
							  fGeoConfig->GetZoneThetaEnd(region, zoneNum));
		  if( fabs(targetCoverage - coverage) < fabs(targetCoverage - bestCoverage) )
		  {
        if(iIter != 100)
        {
			    bestCoverage = coverage;
			    bestSide = cellSide;
		      bestIter = iIter;
          sinceImprovement = 0;
        }
      }
		  if(coverage > 0.0)
		  {
			  // std::cout << "Coverage = " << coverage << "/" << targetCoverage << " with cellSide = " << cellSide;
        cellSide /= sqrt(targetCoverage/coverage);
        // std::cout << " so update to " << cellSide << std::endl;
		  }
		  else
		  {
			  cellSide *= 0.995;
		  }
      
	  }
//	  std::cout << "Iteration " << iIter << ": Finished and updated cellSide to " << cellSide << std::endl;
    if(sinceImprovement > 10){ break; }
  }


  std::cout << "Setting endcapCellSize->at(" << zoneNum << ") = " << bestSide << std::endl;
  endcapCellSize->at(zoneNum) = bestSide;
  if( fGeoConfig->GetLimitPMTNumber() )
  {
    std::cout << "bestNumCells = " << bestNumCells << " (maxNumCells = " << maxNumCells << ") - chosen on iteration " << bestIter << " side = " << bestSide << std::endl << std::endl;
  }
  else
  {
    std::cout << "bestCoverage = " << bestCoverage << " (targetCoverage = " << targetCoverage << ") - chosen on iteration " << bestIter << " side = " << bestSide << std::endl << std::endl;
  }

	return bestSide;
}

double WCSimCherenkovBuilder::GetOptimalWallCellSize(int iZone) {

  std::cout << "Getting optimal cell size for wall zone " << iZone << std::endl;
	double coverage 		= -999.9; // Photocathode coverage
	double wallHeight 	= GetBarrelLengthForCells(); // Height of walls
	double wallSide 		= 2.0 * fPrismRingSegmentBSRadiusInside * sin(M_PI / fGeoConfig->GetNSides()); // Side length of walls
	WCSimUnitCell *wallCell  = (GetWallUnitCell(iZone)); // Make a copy because we'll mess with it

	/////////////////

	double defaultSide = 0.0;
	bool canHaveMorePMTs = true;
	if(fGeoConfig->GetLimitPMTNumber())
	{

		double pmtArea = wallCell->GetPhotocathodeArea();
		double maxNumCells = fGeoConfig->GetMaxZoneCells(WCSimGeometryEnums::DetectorRegion_t::kWall, iZone);
		double maxCoverage = pmtArea * maxNumCells / (wallHeight * wallSide);

		std::cout << "Maximum coverage with available PMTs for region " << " kWall, zone " << iZone << " = " << maxCoverage << std::endl;
    std::cout << "Uses " << maxNumCells << " cells" << std::endl;
		defaultSide = wallCell->GetCellSizeForCoverage(maxCoverage);
		coverage = maxCoverage;
		canHaveMorePMTs = false;
	}
	else
	{
		if(fGeoConfig->GetUseOverallCoverage() || fGeoConfig->GetUseZonalCoverage()){
			coverage = GetZoneCoverage(WCSimGeometryEnums::DetectorRegion_t::kWall, iZone);
			defaultSide = wallCell->GetCellSizeForCoverage(coverage);
      std::cout << "Target coverage = " << coverage << std::endl;
		}
		else{
			std::cerr << "Error: can't tell what coverage type fGeoConfig should use - bailing out" << std::endl;
			assert(0);
		}
		canHaveMorePMTs = true;
	}

	double bestLength = 0.0;
	if(!canHaveMorePMTs)
	{
		int maxNumCells = fGeoConfig->GetMaxZoneCells(WCSimGeometryEnums::DetectorRegion_t::kWall, iZone);
		double minSize = wallCell->GetCellSizeForCoverage(wallCell->GetPhotocathodeArea() * maxNumCells / (wallHeight * wallSide));

		// We want X cells wide by Y cells high
		// Subject to constraints: (X x Y) <= maxNumCells
		//												      X  <= wallSide/minSize
		//															Y  <= wallHeight/minSize
		// These place (X,Y) inside a rectangle so we can just evaluate all the
		// allowed points and it will only take quadratic time (also our PMTs are not tiny
		// compared to the wall size so it won't be many calculations)
		int bestX = 0, bestY = 0;
		for(int iX = 1; iX <= (int)(ceil(wallSide/minSize)); ++iX)
		{
			for(int iY = 1; iY <= (int)ceil((wallHeight/minSize)); ++iY)
			{
//        std::cout << "Testing " << iX << " x " << iY << " cells( = " << iX * iY << "/" << maxNumCells << std::endl;
				int nCells = iX * iY;
				if(nCells > maxNumCells){ break; }
				if(nCells > bestX * bestY){
					bestX = iX;
					bestY = iY;
				}
			}
		}

		bestLength = wallSide / bestX;
		if( wallHeight / bestY < bestLength ){ bestLength = wallHeight / bestY; }
		fWallCellLength.at(iZone) = bestLength;
		fWallCellsX.at(iZone) 	  = bestX;
		fWallCellsZ.at(iZone) 	  = bestY;
	}
	else
	{

		///////////////////



//		std::cout << "Wall height = " << wallHeight << " ( * m = " << wallHeight * m << ")" << std::endl;
//		std::cout << "Wall side = " << wallSide << std::endl;
//		wallCell->Print();
//		std::cout << "Minimum side length allowed = " << wallCell->GetMinimumCellSize() << std::endl;

		// Number of unit cells spanning across the width of the wall
//		std::cout << "Before casting: " << (wallSide / (this->GetWallUnitCell(iZone)->GetCellSizeForCoverage(coverage)));
		int tileX = (int)(wallSide / (this->GetWallUnitCell(iZone)->GetCellSizeForCoverage(coverage)));
		// Number of unit cells to span the height of the detector
		int tileZ = (int)(wallHeight / (this->GetWallUnitCell(iZone)->GetCellSizeForCoverage(coverage)));
		if(tileX == 0){ tileX = 1; }
		if(tileZ == 0){ tileZ = 1; }

		// In general the number of cells needed is non-integral
		// We need an m x n arrangement of cells that gives coverage close
		// to the desired percentage.  So round the number of cells up and down
		// and see which arrangement gets closest
		int xNums[2] = { tileX, tileX+1 };
		int zNums[2] = { tileZ, tileZ+1 };

		double bestCoverage = 0.0;
		double minCoverageDiff = coverage;

		for( int xIndex = 0; xIndex < 2; ++xIndex ){
			for( int zIndex = 0; zIndex < 2; ++zIndex){
				// How big does the cell have to be to fit a given number
				// of them horizontally or vertically on the detector wall?
				double lengthForSide = wallSide / xNums[xIndex];
				double lengthForHeight = wallHeight / zNums[zIndex];

				// Pick the smallest cell and plan to put them slightly
				// further apart in the other dimension
				double length = lengthForHeight;
				if ( lengthForSide < lengthForHeight ){
					length = lengthForSide;
				}

				// Does this beat the last configuration?
				double newCoverage = wallCell->GetPhotocathodeCoverage(length) * (xNums[xIndex] * zNums[zIndex]) * length * length / (wallSide * wallHeight);
				double coverageDiff = fabs(coverage - newCoverage);
//				std::cout << "Test: wall coverage is " << newCoverage << std::endl;
//				std::cout << "Difference from ideal = " << coverageDiff << std::endl;
//				std::cout << "Side = " << length << std::endl;
//				std::cout << "In x: " << xNums[xIndex] << std::endl;
//				std::cout << "In z: " << zNums[zIndex] << std::endl;
//				std::cout << "Length = " << length << std::endl;
//				std::cout << "Coverage = " << wallCell->GetPhotocathodeCoverage(length) << std::endl;
//				std::cout << "Are there overlaps? Computer says... " << wallCell->ContainsOverlaps(length) << std::endl;
//				std::cout << "Minimum side length allowed = " << wallCell->GetMinimumCellSize() << std::endl;
//				std::cout << "Wall side = " << wallSide << " and wallHeight = " << wallHeight << std::endl;

				if( (!wallCell->ContainsOverlaps(length)) && coverageDiff < minCoverageDiff){
					minCoverageDiff = coverageDiff;
					bestCoverage    = newCoverage;
					bestLength		= length;

					fWallCellLength.at(iZone) = bestLength;
					fWallCellsX.at(iZone) 	  = xNums[xIndex];
					fWallCellsZ.at(iZone) 	  = zNums[zIndex];
				}
			}
		}
		std::cout << "Best wall coverage is " << bestCoverage << " (c.f. " << coverage << ")" << std::endl;
    std::cout << "Used " << fWallCellsX.at(iZone) * fWallCellsZ.at(iZone) << " cells" << std::endl << std::endl;;
	}
	return bestLength;
}

void WCSimCherenkovBuilder::ConstructEndCaps(){
	ConstructEndCap(-1);
	ConstructEndCap(+1);
  ConstructEndCapPhysicalVolumes();
}

void WCSimCherenkovBuilder::ConstructEndCap(G4int zflip) {
	ConstructEndCapFrame(zflip);
	ConstructEndCapRings(zflip);
	ConstructEndCapRingSegments(zflip);
	ConstructEndCapSurfaces(zflip);

}

void WCSimCherenkovBuilder::ConstructEndCapFrame(G4int zflip){

	G4LogicalVolume * capLogic = NULL;
	assert( capLogic == NULL);

	G4Material * pureWater = WCSimMaterialsBuilder::Instance()->GetMaterial("Water");

	G4double capAssemblyZ[2] = {-0.5 * fCapAssemblyHeight * zflip, 0.5 * fCapAssemblyHeight * zflip };
	G4double capAssemblyRMin[2] = {0.0,0.0};
	G4double capAssemblyRMax[2] = {fCapAssemblyRadius,fCapAssemblyRadius};
//	G4Tubs* capSolid = new G4Tubs("CapAssembly", 0.0*m,
	G4Polyhedra* capSolid = new G4Polyhedra("CapAssembly",
												  0.0, // phi start
												  360*deg, //total Phi
												  fGeoConfig->GetNSides(), //NPhi-gon
												  2, capAssemblyZ,
												  capAssemblyRMin, capAssemblyRMax);

    capLogic = new G4LogicalVolume(capSolid, pureWater,"CapAssembly",0,0,0);


    if( zflip == -1) { fCapLogicTop = capLogic; std::cout << "TOP " << fCapLogicTop->GetName() << std::endl;}
	else { fCapLogicBottom = capLogic; std::cout << "BOTTOM " << fCapLogicBottom->GetName() << std::endl; }
    return;
}

void WCSimCherenkovBuilder::ConstructEndCapRings( G4int zflip ){
	G4VisAttributes* WCCapRingVisAtt = new G4VisAttributes(G4Colour(64./255.0, 123./255.0, 21./255.0));

	G4LogicalVolume * capLogic = NULL;
	if( zflip == -1 ) { capLogic = fCapLogicTop; }
	else { capLogic = fCapLogicBottom; }
    G4cout << "G4cout capLogic" << capLogic << std::endl;

	G4Material * pureWater = WCSimMaterialsBuilder::Instance()->GetMaterial("Water");

	G4double borderRingZ[2] = {-0.5 * fCapRingHeight * zflip, 0.5 * fCapRingHeight * zflip };
	G4double borderRingRMin[2] = { fCapRingRadiusInside, fCapRingRadiusInside};
	G4double borderRingRMax[2] = { fCapRingRadiusOutside, fCapRingRadiusOutside};

  G4RotationMatrix* endCapRotation = new G4RotationMatrix();

    for(int i = 0; i < 2; ++i){
      std::cout << "borderRingZ  [" << i << "] = " << borderRingZ[i] << std::endl;
      std::cout << "borderRingRmin  [" << i << "] = " << borderRingRMin[i] << std::endl;
      std::cout << "borderRingRmax  [" << i << "] = " << borderRingRMax[i] << std::endl;
    }
    std::cout << "fCapRingHeight = " << fCapRingHeight << std::endl;
    
	  G4Polyhedra* solidEndCapRing = new G4Polyhedra("EndCapRing",
														   	 0.*deg, // phi start
														   	 360.0 * deg,
														   	 fGeoConfig->GetNSides(), //NPhi-gon
														   	 2,
														   	 borderRingZ,
														   	 borderRingRMin,
														   	 borderRingRMax);
	  G4LogicalVolume* logicEndCapRing = new G4LogicalVolume(solidEndCapRing,
																	 pureWater,
																	 "WCBarrelRing",
																	 0,0,0);

    logicEndCapRing->SetVisAttributes(WCCapRingVisAtt);

	  if( zflip == -1 ){ fCapLogicTopRing = logicEndCapRing; }
	  else{ fCapLogicBottomRing = logicEndCapRing; }

//	  std::cout << " Physics cap volume lives at " << (capAssemblyHeight/2.- GetBarrelLengthForCells()/2.)*zflip << std::endl;
// 						std::cout <<  "Place phsyiEndCapRing at "
// 								  << ((capAssemblyHeight/2.- (fGeoConfig->GetInnerHeight() - GetBarrelLengthForCells())/2.)*zflip)
// 								  << " - height is " << borderRingZ[2] - borderRingZ[0]
// 								  << " c.f. capAssemblyHeight = " << capAssemblyHeight << std::endl;
 	  // G4VPhysicalVolume* physiEndCapRing =
    new G4PVPlacement(endCapRotation,
    									G4ThreeVector(0.,0., (0.5 * fCapAssemblyHeight - 0.5 * fCapRingHeight) * zflip),
											logicEndCapRing,
											"EndCapRing",
											capLogic,
											false, 0,true);
    std::cout << "endCapRing placed in capLogic at z = " << (0.5 * fCapAssemblyHeight - 0.5 * fCapRingHeight) * zflip << " with height " << fCapRingHeight << std::endl;
}

void WCSimCherenkovBuilder::ConstructEndCapRingSegments( G4int zflip )
{
	G4VisAttributes* WCCapRingSegmentVisAtt = new G4VisAttributes(G4Colour(98./255.0, 250./255.0, 46./255.0));
	// Set up constants (factorise these somewhere else later)
	G4Material * pureWater = WCSimMaterialsBuilder::Instance()->GetMaterial("Water");

	// Zip these up into arrays to pass to Geant
	G4double capRingZ[2] 			= { -0.5 * fCapRingSegmentHeight, 0.5 * fCapRingSegmentHeight};
	G4double capRingRMin[2] = { fCapRingSegmentRadiusInside, fCapRingSegmentRadiusInside};
	G4double capRingRMax[2] = { fCapRingSegmentRadiusOutside, fCapRingSegmentRadiusOutside };
	// Now divide each ring (an n-gon prism) into n rectangular segments
	G4Polyhedra* capSegmentSolid = new G4Polyhedra("EndCapSegment",
												   -fCapRingSegmentDPhi/2.0, // phi start
												   fCapRingSegmentDPhi, //total Phi
												   1, //NPhi-gon
												   2, capRingZ,
												   capRingRMin, capRingRMax);

	G4LogicalVolume * capSegmentLogic = new G4LogicalVolume(capSegmentSolid,
														    pureWater,
														    "EndCapSegment",
														    0, 0, 0);
  capSegmentLogic->SetVisAttributes(WCCapRingSegmentVisAtt);

	G4LogicalVolume * capRingLogic = NULL;
	if( zflip == -1 ) { capRingLogic = fCapLogicTopRing; }
	else{ capRingLogic = fCapLogicBottomRing; }
	assert( capRingLogic != NULL );

    G4VPhysicalVolume* capSegmentPhysic = new G4PVReplica("EndCapSegment",
    													  capSegmentLogic, capRingLogic, kPhi,
    													  fGeoConfig->GetNSides(),
    													  fCapRingSegmentDPhi, 0.);

  std::cout << "EndCapSegment placed " << std::setprecision(15) << capRingRMin[0] << ", "
                                       << std::setprecision(15) << capRingRMax[0] << std::endl;


    // Now we've placed the physical volume, let's add some blacksheet:
	G4double capSegmentBlacksheetZ[2] = { -fCapRingSegmentBSHeight/2.0, fCapRingSegmentBSHeight/2.0 };
	G4double capSegmentBlacksheetRmax[2] = { fCapRingSegmentBSRadiusOutside, fCapRingSegmentBSRadiusOutside };
	G4double capSegmentBlacksheetRmin[2] = { fCapRingSegmentBSRadiusInside, fCapRingSegmentBSRadiusInside};

	G4Polyhedra* capSegmentBlacksheetSolid = new G4Polyhedra("EndCapSegmentBlacksheet",
														  -0.5 * fCapRingSegmentDPhi, // phi start
														  fCapRingSegmentDPhi, //total phi
														  1, //NPhi-gon
														  2,
														  capSegmentBlacksheetZ,
														  capSegmentBlacksheetRmin,
														  capSegmentBlacksheetRmax);

	G4LogicalVolume * capSegmentBlacksheetLogic = new G4LogicalVolume( capSegmentBlacksheetSolid,
																	   WCSimMaterialsBuilder::Instance()->GetMaterial("Blacksheet"),
																	   "EndCapSegmentBlacksheet",
																	   0, 0, 0);

  std::cout << "Here" << std::endl;
  std::cout << "capSegmentBlacksheetLogic = " << capSegmentBlacksheetLogic->GetName() << std::endl;
  std::cout << "capSegmentLogic = " << capSegmentLogic->GetName() << std::endl;
	G4VPhysicalVolume* capSegmentBlacksheetPhysic = new G4PVPlacement( 0,
																	   G4ThreeVector(0., 0., 0.),
																	   capSegmentBlacksheetLogic,
																	   "EndCapSegmentBlacksheet",
																	   capSegmentLogic,
																	   false, 0, true);

  std::cout << "EndCapSegment placed " << std::setprecision(15) << capSegmentBlacksheetRmin[0] << ", "
                                       << std::setprecision(15) << capSegmentBlacksheetRmax[0] << std::endl;

	G4LogicalBorderSurface * WaterBSCapCellSurface = NULL;

	WaterBSCapCellSurface = new G4LogicalBorderSurface("WaterBSCapCellSurface",
														capSegmentPhysic,
														capSegmentBlacksheetPhysic,
														WCSimMaterialsBuilder::Instance()->GetOpticalSurface("WaterBSCellSurface"));

  // Add the whitesheet too
	G4double capSegmentWSZ[2] = { -fCapRingSegmentWSHeight/2.0, fCapRingSegmentWSHeight/2.0 };
	G4double capSegmentWSRmax[2] = { fCapRingSegmentWSRadiusOutside, fCapRingSegmentWSRadiusOutside };
	G4double capSegmentWSRmin[2] = { fCapRingSegmentWSRadiusInside, fCapRingSegmentWSRadiusInside};

	G4Polyhedra* capSegmentWhitesheetSolid = new G4Polyhedra("EndCapSegmentWhitesheet",
														  -0.5 * fCapRingSegmentDPhi, // phi start
														  fCapRingSegmentDPhi, //total phi
														  1, //NPhi-gon
														  2,capSegmentWSZ,capSegmentWSRmin,capSegmentWSRmax);

	G4LogicalVolume * capSegmentWhitesheetLogic = new G4LogicalVolume( capSegmentWhitesheetSolid,
																	   WCSimMaterialsBuilder::Instance()->GetMaterial("Whitesheet"),
																	   "EndCapSegmentWhitesheet",
																	   0, 0, 0);

	G4VPhysicalVolume* capSegmentWhitesheetPhysic = new G4PVPlacement( 0,
																	   G4ThreeVector(0., 0., 0.),
																	   capSegmentWhitesheetLogic,
																	   "EndCapSegmentWhitesheet",
																	   capSegmentLogic,
																	   false, 0, true);

  std::cout << "EndCapSegment placed " << std::setprecision(15) << capSegmentWSRmin[0] << ", "
                                       << std::setprecision(15) << capSegmentWSRmax[0] << std::endl;

	G4LogicalBorderSurface * WaterWSCapCellSurface = NULL;

	WaterBSCapCellSurface = new G4LogicalBorderSurface("WaterWSCapCellSurface",
														capSegmentPhysic,
														capSegmentWhitesheetPhysic,
														WCSimMaterialsBuilder::Instance()->GetOpticalSurface("WaterWSCellSurface"));

	G4VisAttributes* WCCellBlacksheetCellVisAtt = new G4VisAttributes(G4Colour(80./255.0, 51./255.0, 204./255.0));
	if (fDebugMode ){
		capSegmentBlacksheetLogic->SetVisAttributes(WCCellBlacksheetCellVisAtt);
		capSegmentWhitesheetLogic->SetVisAttributes(WCCellBlacksheetCellVisAtt);
	}
	else{
		capSegmentBlacksheetLogic->SetVisAttributes(G4VisAttributes::Invisible);
		capSegmentWhitesheetLogic->SetVisAttributes(G4VisAttributes::Invisible);
	}

	  return;
}

void WCSimCherenkovBuilder::ConstructEndCapSurfaces(G4int zflip){

	  G4LogicalVolume * capLogic = NULL;
    G4VPhysicalVolume * capBSPhysics;
    G4VPhysicalVolume * capWSPhysics;
	  if( zflip == -1 ) { 
      capLogic = fCapLogicTop; 
      capBSPhysics = fCapBSTopPhysics;
      capWSPhysics = fCapWSTopPhysics;
    }
	  else { 
      capLogic = fCapLogicBottom; 
      capBSPhysics = fCapBSBottomPhysics;
      capWSPhysics = fCapWSBottomPhysics;
    }

	  G4Material * pureWater = WCSimMaterialsBuilder::Instance()->GetMaterial("Water");
	  G4Material * blacksheet = WCSimMaterialsBuilder::Instance()->GetMaterial("Blacksheet");
	  G4Material * whitesheet = WCSimMaterialsBuilder::Instance()->GetMaterial("Whitesheet");

	  //------------------------------------------------------------
	  // Add the flat section of the cap
	  // -----------------------------------------------------------
    std::cout << "Blacksheet thickness = " << fBlacksheetThickness << std::endl;

    std::cout << "fCapPolygonHeight = " << fCapPolygonHeight << std::endl;
    std::cout << "fCapAssemblyHeight = " << fCapAssemblyHeight << std::endl;

	  //---------------------------------------------------------------------
	  // add cap blacksheet
	  // -------------------------------------------------------------------

	  G4double capEndBlacksheetZ[2] = {-0.5 * fCapPolygonEndBSHeight * zflip, 0.5 * fCapPolygonEndBSHeight * zflip};
	  G4double capEndBlacksheetRmin[2] = {0, 0};
	  G4double capEndBlacksheetRmax[2] = {fCapPolygonEndBSRadius, fCapPolygonEndBSRadius};

	  G4VSolid* solidWCCapEndBlacksheet = new G4Polyhedra("WCCapEndBlacksheet",
														0.*deg, // phi start
														360.0 * deg, //total phi
														fGeoConfig->GetNSides(), //NPhi-gon
														2, //  z-planes
														capEndBlacksheetZ, //position of the Z planes
														capEndBlacksheetRmin, // min radius at the z planes
														capEndBlacksheetRmax// max radius at the Z planes
														);

	  G4LogicalVolume* logicWCCapEndBlacksheet = new G4LogicalVolume(solidWCCapEndBlacksheet,
																  blacksheet,
																  "WCCapEndBlacksheet",
																  0,0,0);
	  capBSPhysics = new G4PVPlacement(0,
	  									G4ThreeVector(0.,0., (0.5 * fCapPolygonEndBSHeight + 0.00001*mm) * zflip),
	  									logicWCCapEndBlacksheet,
	  									"WCCapEndBlacksheet",
                      capLogic,
	  									false,
	  									0,true);

    // ---------------------------------------
    // Now the same for the white sheet
    // ---------------------------------------

	  G4double capEndWSZ[2] = {-0.5 * fCapPolygonEndWSHeight * zflip, 0.5 * fCapPolygonEndWSHeight * zflip};
	  G4double capEndWSRmin[2] = {0, 0};
	  G4double capEndWSRmax[2] = {fCapPolygonEndWSRadius, fCapPolygonEndWSRadius};

	  G4VSolid* solidWCCapEndWhitesheet = new G4Polyhedra("WCCapEndWhitesheet",
														0.*deg, // phi start
														360.0 * deg, //total phi
														fGeoConfig->GetNSides(), //NPhi-gon
														2, capEndWSZ, capEndWSRmin, capEndWSRmax);

	  G4LogicalVolume* logicWCCapEndWhitesheet = new G4LogicalVolume(solidWCCapEndWhitesheet,
																  whitesheet,
																  "WCCapEndWhitesheet",
																  0,0,0);
	  capWSPhysics = new G4PVPlacement(0,
	  									G4ThreeVector(0.,0., (-0.5 * fCapPolygonEndWSHeight - 0.00001*mm) * zflip),
	  									logicWCCapEndWhitesheet,
	  									"WCCapEndWhitesheet",
	  									capLogic,
	  									false,0,true);

	  G4VisAttributes* WCCapBlacksheetVisAtt = new G4VisAttributes(G4Colour(0.9,0.2,0.2));
	    if(fDebugMode || 1)
	    {
	    	logicWCCapEndBlacksheet->SetVisAttributes(WCCapBlacksheetVisAtt);
	    	logicWCCapEndWhitesheet->SetVisAttributes(WCCapBlacksheetVisAtt);
	    }
	    else
	    {
	        logicWCCapEndBlacksheet->SetVisAttributes(G4VisAttributes::Invisible);
	        logicWCCapEndWhitesheet->SetVisAttributes(G4VisAttributes::Invisible);
	    }

	    return;
}

void WCSimCherenkovBuilder::PlaceEndCapPMTs(G4int zflip){

	  G4LogicalVolume * capLogic = NULL;
	  G4RotationMatrix* WCCapPMTRotation = new G4RotationMatrix;
	  WCSimGeometryEnums::DetectorRegion_t region;
    // Top PMTs point downwards
	  std::vector<double> * cellSizeVec = NULL;
	  if( zflip == -1 )
    { 
	    WCCapPMTRotation->rotateY(180.*deg);
      capLogic = fCapLogicTop; 
      region = WCSimGeometryEnums::DetectorRegion_t::kTop;
      cellSizeVec = &(fTopCellSize);
    }
	  else
	  {
	  	capLogic = fCapLogicBottom;
	  	region = WCSimGeometryEnums::DetectorRegion_t::kBottom;
	  	cellSizeVec = &(fBottomCellSize);
	  }

    std::cout << "Cell sizes for region " << region.AsString() << std::endl;
    for(unsigned int i = 0; i < cellSizeVec->size(); ++i)
    {
      std::cout << "zone " << i << " size = " << cellSizeVec->at(i) << std::endl;
    }

//    G4cout << "G4cout capLogic again" << capLogic << std::endl;
//    G4cout << "capLogic top = " << fCapLogicTop << "  and bottom ... " << fCapLogicBottom << std::endl;

	  //---------------------------------------------------------
	  // Add top and bottom PMTs
	  // -----------------------------------------------------
    std::cout << " *** PlaceEndCapPMTs ***    zflip = " << zflip << std::endl;

    std::vector<Int_t> placedTop;

    for( unsigned int iZone = 0; iZone < fGeoConfig->GetNumZones(region); ++iZone)
    {
      std::cout << "Placing zone " << iZone << std::endl;
      placedTop.push_back(0);

      // Clear vecotors with PMT extreme positions used to compute Plane Pipes lenght and position (zone dependent)
      fEndCapExtremePMTPos_Min.clear();
      fEndCapExtremePMTPos_Max.clear();

		// Angled walls mean squares don't tile very well
		// We'll consider their positions, but we only need to get these vectors once
		// if they live outside the loop.;
		std::vector<Double_t> pmtX = fGeoConfig->GetCellPMTX(region, iZone);
		std::vector<Double_t> pmtY = fGeoConfig->GetCellPMTY(region, iZone);

		std::vector<Double_t> pmtFaceTheta = fGeoConfig->GetCellPMTFaceTheta(region, iZone);
		std::vector<Double_t> pmtFacePhi   = fGeoConfig->GetCellPMTFacePhi(  region, iZone);
		std::vector<WCSimGeometryEnums::PMTDirection_t> pmtFaceType  = fGeoConfig->GetCellPMTFaceType(region, iZone);

		std::vector<std::string> pmtNames = fGeoConfig->GetCellPMTName(region,iZone);
		std::vector<Double_t> pmtRad;
		for (unsigned int iPMT = 0; iPMT < pmtNames.size(); ++iPMT) {
			pmtRad.push_back(fPMTManager->GetPMTByName(pmtNames.at(iPMT)).GetRadius());
		}

//  	    double capPolygonOuterRadius = WCSimPolygonTools::GetOuterRadiusFromInner(fGeoConfig->GetNSides(), fCapPolygonCentreRadius);
        double capPolygonOuterRadius = WCSimPolygonTools::GetOuterRadiusFromInner(fGeoConfig->GetNSides(), fCapRingRadiusInside-0.0001*mm);
  	    double thetaStart = fGeoConfig->GetZoneThetaStart(region, iZone);
  	    double thetaEnd = fGeoConfig->GetZoneThetaEnd(region, iZone);
		    G4double squareSide = 2.0 * capPolygonOuterRadius;
        assert(cellSizeVec->at(iZone) > 0);
		    G4double cellSide = cellSizeVec->at(iZone);
        assert(cellSide > 1.0);

		double xPos = 0.0;
		while (xPos < squareSide) {
			double yPos = 0.0;
			while (fabs(yPos) < squareSide) {

        // std::cout << "xPos = " << xPos << "  yPos = " << yPos << "   cellSide = " << cellSide << std::endl;
				G4TwoVector centreToTopLeftSquare = G4TwoVector(-0.5 * squareSide, 0.5 * squareSide);
				G4TwoVector topLeftCell = G4TwoVector(xPos, -yPos) + centreToTopLeftSquare;
			  G4TwoVector topRightCell     = G4TwoVector(xPos+cellSide, -yPos) + centreToTopLeftSquare;
			  G4TwoVector bottomLeftCell     = G4TwoVector(xPos, -yPos-cellSide) + centreToTopLeftSquare;
			  G4TwoVector bottomRightCell     = G4TwoVector(xPos+cellSide, -yPos-cellSide) + centreToTopLeftSquare;
			  // std::cout << "centreToTopLeftSquare = " << centreToTopLeftSquare << std::endl;
				// << "topLeftCell = " << topLeftCell << std::endl;

				bool cellInPolygonSlice = true;
				for (unsigned int iPMT = 0; iPMT < pmtNames.size(); ++iPMT) {
					// Coordinates the corner of a square containing the PMT itself,
					// relative to the top-left corner of the unit cell
					// std::cout << "PMT " << iPMT << " at " << pmtX.at(iPMT)/m << ", " << pmtY.at(iPMT)/m << " (r = " << pmtRad.at(iPMT) << ")" << std::endl;
					G4TwoVector pmtTopLeft(cellSide * (pmtX.at(iPMT)/m) - pmtRad.at(iPMT),
							cellSide * (pmtY.at(iPMT)/m) + pmtRad.at(iPMT));
//					std::cout << "pmtTopLeft = " << pmtTopLeft << std::endl;
					G4TwoVector pmtTopRight(cellSide * pmtX.at(iPMT)/m + pmtRad.at(iPMT),
							cellSide * pmtY.at(iPMT)/m + pmtRad.at(iPMT));
					G4TwoVector pmtBottomLeft(cellSide * pmtX.at(iPMT)/m - pmtRad.at(iPMT),
							cellSide * pmtY.at(iPMT)/m - pmtRad.at(iPMT));
					G4TwoVector pmtBottomRight(cellSide * pmtX.at(iPMT)/m + pmtRad.at(iPMT),
							cellSide * pmtY.at(iPMT)/m - pmtRad.at(iPMT));

					// if (!(WCSimPolygonTools::PolygonSliceContains(fGeoConfig->GetNSides(), thetaStart, thetaEnd,
					// 		capPolygonOuterRadius, topLeftCell + pmtTopLeft))
					// 		|| !(WCSimPolygonTools::PolygonSliceContains(fGeoConfig->GetNSides(), thetaStart, thetaEnd,
					// 				capPolygonOuterRadius, topLeftCell + pmtTopRight))
					// 		|| !(WCSimPolygonTools::PolygonSliceContains(fGeoConfig->GetNSides(), thetaStart, thetaEnd,
					// 				capPolygonOuterRadius, topLeftCell + pmtBottomLeft))
					// 		|| !(WCSimPolygonTools::PolygonSliceContains(fGeoConfig->GetNSides(), thetaStart, thetaEnd,
					// 				capPolygonOuterRadius, topLeftCell + pmtBottomRight))) {
					if (!(WCSimPolygonTools::PolygonSliceContains(fGeoConfig->GetNSides(), thetaStart, thetaEnd,
							capPolygonOuterRadius, topLeftCell))
							|| !(WCSimPolygonTools::PolygonSliceContains(fGeoConfig->GetNSides(), thetaStart, thetaEnd,
									capPolygonOuterRadius, topRightCell))
							|| !(WCSimPolygonTools::PolygonSliceContains(fGeoConfig->GetNSides(), thetaStart, thetaEnd,
									capPolygonOuterRadius, bottomLeftCell))
							|| !(WCSimPolygonTools::PolygonSliceContains(fGeoConfig->GetNSides(), thetaStart, thetaEnd,
									capPolygonOuterRadius, bottomRightCell))) {
						cellInPolygonSlice = false;
					}
					WCSimUnitCell * unitCell = GetUnitCell(region, iZone);
					if (cellInPolygonSlice) {
						//						std::cout << "Placing cap PMT at (" << cellpos.x() << ", " << cellpos.y() << std::endl;
						//						std::cout << "Here, phi = " << cellpos.phi() << " and start = " << thetaStart << ", end = " << thetaEnd << std::endl;


					        G4RotationMatrix * tmpWCCapPMTRotation =  new G4RotationMatrix(*WCCapPMTRotation); // tmpWCCapPMTRotation is needed to avoid satring rotation drifting

						if(pmtFaceType.at(iPMT) == WCSimGeometryEnums::PMTDirection_t::kArbitrary) 
						  *tmpWCCapPMTRotation *= GetArbitraryPMTFaceRotation( pmtFaceTheta.at(iPMT), pmtFacePhi.at(iPMT));
						else  *tmpWCCapPMTRotation *= GetEndcapPMTFaceRotation( pmtFaceType.at(iPMT),  zflip);

					  WCSimPMTConfig config = unitCell->GetPMTPlacement(iPMT).GetPMTConfig();
						double radius = config.GetMaxRadius();
						double dZ     = radius*fabs(sin(tmpWCCapPMTRotation->getTheta()));

						G4TwoVector pmtCellPosition = unitCell->GetPMTPos(iPMT, cellSizeVec->at(iZone)); // PMT position in cell, relative to top left of cell
						G4ThreeVector PMTPosition(topLeftCell.x() + pmtCellPosition.x(),
								topLeftCell.y() - pmtCellPosition.y(), zflip*(dZ+fBlacksheetThickness));//(dZ+(-0.5 * fCapAssemblyHeight + 0.5 * fCapPolygonHeight + fBlacksheetThickness)));	

	     //Stefano : Plane Pipes
	     double pomD = ( fGeoConfig->UsePMTSupport( WCSimGeometryEnums::PMTSupport_t::kPOM ) ) ? 2*fPlanePipeRadius + fPMTHolderLength + 5*mm : 0;


            // Leigh: Veto specialities.
            G4ThreeVector vetoShift;
            if(pmtFaceType.at(iPMT) == WCSimGeometryEnums::PMTDirection_t::kVeto){
              G4RotationMatrix rotation;
              rotation.rotateY(180.*deg);
              *tmpWCCapPMTRotation *= rotation;
              vetoShift.setZ(-1*(fBlacksheetThickness + fWhitesheetThickness)*zflip);
            }	

	    G4ThreeVector pomShift( 0, 0, -1*zflip*pomD);
	    G4ThreeVector PipeBackShift( 0, 0, zflip*(pomD/2-fPlanePipeRadius));

            PMTPosition += vetoShift + pomShift;

//            std::cout << " PMT POSITION = " << PMTPosition.x() << ", " << PMTPosition.y() << ", " << PMTPosition.z() << " :: " << zflip << std::endl;

						// G4VPhysicalVolume* physiCapPMT =
						new G4PVPlacement(tmpWCCapPMTRotation,     // its rotation
								PMTPosition, fPMTBuilder.GetPMTLogicalVolume(config),        // its logical volume
								("WCPMT_"+config.GetPMTName()).c_str(),           // its name
								capLogic,      // its mother volume
								false,             // no boolean operations
								fNumPMTs);
						//std::cout << "name of capLogic = " << capLogic->GetName() << std::endl;
						if (fNumPMTs == 0 || 1) {
            // std::cout << "XXplacingXX " << xPos << topLeftCell.x() << " " << topLeftCell.y() << std::endl;
            // std::cout << "inner radius = " << fGeoConfig->GetOuterRadius() << "   xpos = " << xPos << "   ypos = " << yPos << "  top cell size = " << cellSizeVec->at(iZone) << std::endl;
            // std::cout << "PMT position in cell = " << pmtCellPosition.x() / m << "," << pmtCellPosition.y() / m << "in m" << std::endl;
            // std::cout << "Cell size = " << cellSizeVec->at(iZone) << std::endl;// pmtCellPosition.x() << "," << pmtCellPosition.y() << std::endl;
            // std::cout << "PMT logical volume name = " << fPMTBuilder.GetPMTLogicalVolume(config)->GetName() << std::endl;
            //  std::cout << "Placed endcap PMT!  Have now placed " << fNumPMTs+1 << std::endl;
						}
						fNumPMTs++;
            placedTop.at(iZone)++;
	    
	    // logicWCPMT->GetDaughter(0),physiCapPMT is the glass face. If you add more
	    // daughter volumes to the PMTs (e.g. a acryl cover) you have to check, if
	    // this is still the case.

	    if(pmtFaceType.at(iPMT) != WCSimGeometryEnums::PMTDirection_t::kVeto){ // check PMT position for Plane Pipes only for non Veto PMT

	      // Check PMT postion along beam axis :X direction.
	      // WARNING !!!! This may not work as expected if axis orientation is significantly changed (like beam along Z as usual in HEP)
	      bool isNewX = true;
	      int iX = -1;
	      for(int xit = 0; xit< fEndCapExtremePMTPos_Max.size(); ++xit){

		if( PMTPosition.x() == (fEndCapExtremePMTPos_Max.at(xit)).x() ){

		  isNewX = false;
		  iX = xit;

		}  // if xit.X
	      }// for xit

	      if(isNewX) {
		// std::cout << "Endcap Plane Pipe : New Position! " << std::endl;
		fEndCapExtremePMTPos_Max.push_back(PMTPosition+PipeBackShift);
		fEndCapExtremePMTPos_Min.push_back(PMTPosition+PipeBackShift);
	      } else if( PMTPosition.y() < (fEndCapExtremePMTPos_Min.at(iX)).y() ){
		// std::cout << "Plane Pipe : New Min Y! " << std::endl;
		fEndCapExtremePMTPos_Min[iX] = PMTPosition;
	      } else if( PMTPosition.y() > (fEndCapExtremePMTPos_Max.at(iX)).y() ){
		// std::cout << "Plane Pipe : New Max Y! " << std::endl;
		fEndCapExtremePMTPos_Max[iX] = PMTPosition;
	      }// if isNewX

	    }// if not kVeto

					}
				}

				yPos = yPos + cellSide;
			}
			xPos = xPos + cellSide;
		}
		
                /// Plane Pipes placement needs to be just after the corresponding PMT placement
		if( fGeoConfig->UsePMTSupport( WCSimGeometryEnums::PMTSupport_t::kPOM ) ) PlaceEndCapPlanePipes(zflip, iZone);
    }
      
    for (unsigned int i = 0; i < placedTop.size(); ++i)
    {
      std::cout << region.AsString() << ": placed " << placedTop.at(i) << " pmts in zone " << i << std::endl;
    }
}




void WCSimCherenkovBuilder::PlaceEndCapPlanePipes(G4int zflip, G4int zone){

  // Check Position Vectors size is the same
  if(fEndCapExtremePMTPos_Max.size() != fEndCapExtremePMTPos_Min.size() ){
    
    std::cerr << "Error: mismatch in PMT array positions for Endcap  Plane Pipes - bailing out" << std::endl;
    assert(0);

  } // if vector size

  G4LogicalVolume * capLogic = NULL;  
  WCSimGeometryEnums::DetectorRegion_t region;
  // Top PMTs point downwards
  if( zflip == -1 )
    {
      capLogic = fCapLogicTop;
      region = WCSimGeometryEnums::DetectorRegion_t::kTop;
    }
  else
    {
      capLogic = fCapLogicBottom;
      region = WCSimGeometryEnums::DetectorRegion_t::kBottom;
    }

  G4Material *PVC = WCSimMaterialsBuilder::Instance()->GetMaterial("PVC");

  G4RotationMatrix* PlanePipeRotation = new G4RotationMatrix;
  PlanePipeRotation->rotateX( 90 * deg );


  G4RotationMatrix* PlanePipeShortRotation = new G4RotationMatrix;
  PlanePipeShortRotation->rotateX( 90 * deg );
  PlanePipeShortRotation->rotateY( 90 * deg );

  for(int xit = 0; xit< fEndCapExtremePMTPos_Max.size(); ++xit){  

    double  pipeLength = (fEndCapExtremePMTPos_Max.at(xit) - fEndCapExtremePMTPos_Min.at(xit)).mag();

    if(pipeLength <= 0 ) continue;  // No pipe if min and max extreme position are identical

    //    std::cout << " Pipe Length " << pipeLength << fEndCapExtremePMTPos_Max.at(xit) << " " <<  fEndCapExtremePMTPos_Min.at(xit) << std::endl;
    std::stringstream ss;
    ss << "EndCapPlanePipe" << zone << "_" << xit;
    G4String name = ss.str();
    
    G4Tubs *planePipeTube = new G4Tubs(name,
				       0,     fPlanePipeRadius,
				       pipeLength/2,
				       0 * deg,  360 * deg);



  
    fPlanePipeLogics.push_back(new G4LogicalVolume( planePipeTube, PVC, name ));
    int iLogicPipe = fPlanePipeLogics.size() -1;


    G4SurfaceProperty * OpWaterPlanePipeSurface = (G4SurfaceProperty*)(WCSimMaterialsBuilder::Instance()->GetOpticalSurface("WaterPlanePipeSurface"));

    ss << "EndCapWaterPlanePipeSurface" << zone << "_" << xit;
    G4String sname = ss.str();
    G4LogicalSkinSurface* EcpWaterPlanePipeSurface = new G4LogicalSkinSurface(
									      sname,
									      fPlanePipeLogics.at(iLogicPipe),
									      OpWaterPlanePipeSurface);
    

    
    fPlanePipePhysics.push_back( new G4PVPlacement(PlanePipeRotation,
						  (fEndCapExtremePMTPos_Max.at(xit) + fEndCapExtremePMTPos_Min.at(xit))/2,
						   fPlanePipeLogics.at(iLogicPipe), name,
						   capLogic,   false, 0) );


    if(xit<fEndCapExtremePMTPos_Max.size()-1){

      double  shortPipeLength = ( (fEndCapExtremePMTPos_Max.at(xit+1))[0] - (fEndCapExtremePMTPos_Min.at(xit))[0] )
	- 2* fPlanePipeRadius -0.1*mm;  // Distance between Parallel Pipes (alon X dir)

      if(shortPipeLength <= 0) continue;

      int nPlanes =  int(pipeLength/fPlanePipeStep) +1 ;

      double  shortPipeExcess = pipeLength - fPlanePipeStep*(nPlanes-1);

      ss << "EndCapPlanePipeShort" << zone << "_" << xit;
      name = ss.str();

      G4Tubs *shortPlanePipeTube = new G4Tubs(name,
					      0,     fPlanePipeRadius,
					      shortPipeLength/2,
					      0 * deg,  360 * deg);



      fPlanePipeLogics.push_back(new G4LogicalVolume( shortPlanePipeTube, PVC, name ));
      iLogicPipe = fPlanePipeLogics.size() -1;
      
      ss << "EndCapWaterPlanePipeShortSurface" << zone << "_" << xit;
      sname = ss.str();
      G4LogicalSkinSurface* EcpWaterPlanePipeShortSurface = new G4LogicalSkinSurface(
										     sname,
										     fPlanePipeLogics.at(iLogicPipe),
										     OpWaterPlanePipeSurface);
     
      G4ThreeVector deltaShift(0,0,0);
      int nPipes = 0;
      for(int sit=0; sit<nPlanes; ++sit){	

	G4ThreeVector shortShift( ((fEndCapExtremePMTPos_Max.at(xit+1))[0] - (fEndCapExtremePMTPos_Min.at(xit))[0]), shortPipeExcess/2 + sit*fPlanePipeStep, 0);

	if( (fEndCapExtremePMTPos_Max.at(xit+1))[1] < (fEndCapExtremePMTPos_Min.at(xit)+shortShift)[1]) break;

	if(sit>0 && sit<nPlanes-1 )  deltaShift = G4ThreeVector(0, -2*fPlanePipeRadius, 0);
	 
	fPlanePipePhysics.push_back( new G4PVPlacement(PlanePipeShortRotation,
						       (fEndCapExtremePMTPos_Min.at(xit)+shortShift+deltaShift),
						       fPlanePipeLogics.at(iLogicPipe), name,
						       capLogic,   false, nPipes ) );

	nPipes++;
				     
	if(sit>0 && sit<nPlanes-1 ) {
	  deltaShift = G4ThreeVector(0, 2*fPlanePipeRadius, 0);
	  fPlanePipePhysics.push_back( new G4PVPlacement(PlanePipeShortRotation,
							 (fEndCapExtremePMTPos_Min.at(xit)+shortShift+deltaShift),
							 fPlanePipeLogics.at(iLogicPipe), name,
							 capLogic,   false, nPipes ));
	  nPipes++;
	}
      } // for sit
    }// if xit
  }// for xit

} // PlaceEndCapPlanePipes



void WCSimCherenkovBuilder::PlaceBarrelPlanePipes(G4int zone){

  // Check Position Vectors size is the same
  if(fBarrelExtremePMTPos_Max.size() != fBarrelExtremePMTPos_Min.size() ){
    
    std::cerr << "Error: mismatch in PMT array positions for Plane Pipes - bailing out" << std::endl;
    assert(0);

  } // if vector size
    
  double  pipeLength = (fBarrelExtremePMTPos_Max.at(0) - fBarrelExtremePMTPos_Min.at(0)).mag();
  
  if(pipeLength <= 0) {
    std::cout << " WARNING - No POM pipes placed for Barrel zone " << zone << std::endl;
    return;
  }
 
  std::stringstream ss;
  ss << "BrlPlanePipe" << zone;
  G4String name = ss.str();
 
  G4Tubs *planePipeTube = new G4Tubs(name,
				     0,     fPlanePipeRadius,
				     pipeLength/2,
				     0 * deg,  360 * deg);


  G4Material *PVC = WCSimMaterialsBuilder::Instance()->GetMaterial("PVC");
  
  fPlanePipeLogics.push_back(new G4LogicalVolume( planePipeTube, PVC, name ));
  int iLogicPipe = fPlanePipeLogics.size() -1;


  G4SurfaceProperty * OpWaterPlanePipeSurface = (G4SurfaceProperty*)(WCSimMaterialsBuilder::Instance()->GetOpticalSurface("WaterPlanePipeSurface"));

  ss << "BrlWaterPlanePipeSurface" << zone;
  G4String sname = ss.str();
  G4LogicalSkinSurface* BrlWaterPlanePipeSurface = new G4LogicalSkinSurface(
									    sname,
									    fPlanePipeLogics.at(iLogicPipe),
									    OpWaterPlanePipeSurface);

  G4RotationMatrix* PlanePipeRotation = new G4RotationMatrix;
  PlanePipeRotation->rotateX( 90 * deg ); // 

  G4RotationMatrix* PlanePipeShortRotation = new G4RotationMatrix;
  PlanePipeShortRotation->rotateZ( 90 * deg );
  
  for(int zit = 0; zit< fBarrelExtremePMTPos_Max.size(); ++zit){  
    
    double checkPipeLength = (fBarrelExtremePMTPos_Max.at(zit) - fBarrelExtremePMTPos_Min.at(zit)).mag();
    
    // Check Pipe Length is always the same inside the same Zone
    assert(checkPipeLength == pipeLength);

    fPlanePipePhysics.push_back( new G4PVPlacement(PlanePipeRotation,
						   (fBarrelExtremePMTPos_Max.at(zit) + fBarrelExtremePMTPos_Min.at(zit))/2,
						   fPlanePipeLogics.at(iLogicPipe), name,
						   fSegmentLogics.at(zone),   false, zit) );
    

    double wallSide         = 2.0 * fPrismRingSegmentBSRadiusInside * sin(M_PI / fGeoConfig->GetNSides()); // Side length of walls (copyed from above ... )
    double shortPipeLength  = (wallSide / fWallCellsX.at(zone) ) /2  - fPlanePipeRadius - 0.1*mm;


    if(shortPipeLength <= 0)  continue;


    int nPlanes =  int(pipeLength/fPlanePipeStep) +1 ;


    double  shortPipeExcess = pipeLength - fPlanePipeStep*(nPlanes-1);

    ss << "BarrelPlanePipeShort" << zone << "_" << zit;
    name = ss.str();
   
    G4Tubs *shortPlanePipeTube = new G4Tubs(name,
					    0,     fPlanePipeRadius,
					      shortPipeLength/2,
					      0 * deg,  360 * deg);

    

    fPlanePipeLogics.push_back(new G4LogicalVolume( shortPlanePipeTube, PVC, name ));
    iLogicPipe = fPlanePipeLogics.size() -1;
    
    ss << "BarrelWaterPlanePipeShortSurface" << zone << "_" << zit;
    sname = ss.str();
    G4LogicalSkinSurface* BrlWaterPlanePipeShortSurface = new G4LogicalSkinSurface(
										     sname,
										     fPlanePipeLogics.at(iLogicPipe),
										     OpWaterPlanePipeSurface);
     
    G4ThreeVector deltaShift(0,0,0);
    int nPipes = 0;
    for(int sit=0; sit<nPlanes; ++sit){	
      
      G4ThreeVector shortShift1(  0,  shortPipeExcess/2 + sit*fPlanePipeStep,  shortPipeLength/2 + fPlanePipeRadius + 0.1*mm);
      G4ThreeVector shortShift2(  0,  shortPipeExcess/2 + sit*fPlanePipeStep, -shortPipeLength/2 - fPlanePipeRadius - 0.1*mm);
      
      deltaShift = G4ThreeVector(0, -2*fPlanePipeRadius, 0);
	 
      fPlanePipePhysics.push_back( new G4PVPlacement(PlanePipeShortRotation,
						     (fBarrelExtremePMTPos_Min.at(zit)+shortShift1+deltaShift),
						     fPlanePipeLogics.at(iLogicPipe), name,
						     fSegmentLogics.at(zone),   false, nPipes ) );
      
      nPipes++;
      
      fPlanePipePhysics.push_back( new G4PVPlacement(PlanePipeShortRotation,
						     (fBarrelExtremePMTPos_Min.at(zit)+shortShift2+deltaShift),
						       fPlanePipeLogics.at(iLogicPipe), name,
						     fSegmentLogics.at(zone),   false, nPipes ) );
      
      nPipes++;

      deltaShift = G4ThreeVector(0, 2*fPlanePipeRadius, 0);
      fPlanePipePhysics.push_back( new G4PVPlacement(PlanePipeShortRotation,
						     (fBarrelExtremePMTPos_Min.at(zit)+shortShift1+deltaShift),
						     fPlanePipeLogics.at(iLogicPipe), name,
						     fSegmentLogics.at(zone),   false, nPipes ));
      nPipes++;
      
      fPlanePipePhysics.push_back( new G4PVPlacement(PlanePipeShortRotation,
						     (fBarrelExtremePMTPos_Min.at(zit)+shortShift2+deltaShift),
						     fPlanePipeLogics.at(iLogicPipe), name,
						     fSegmentLogics.at(zone),   false, nPipes ));
      nPipes++;

    }// for sit    
  }// for zit
}




void WCSimCherenkovBuilder::ConstructEndCapPhysicalVolumes(){
  // std::cout << "mainAnnulusHeight = " << mainAnnulusHeight << " (in m = " << mainAnnulusHeight/m << ")" << std::endl;
  // std::cout << "capAssemblyHeight = " << capAssemblyHeight << " (in m = " << capAssemblyHeight/m << ")" << std::endl;
  // std::cout << "InnerHeight=  " << fGeoConfig->GetInnerHeight() << " (in m = " << fGeoConfig->GetInnerHeight()/m << ")" << std::endl;
  // std::cout << "BarrelLengthForCells = " << GetBarrelLengthForCells() << " (in m = " << GetBarrelLengthForCells()/m << ")" << std::endl;
  // std::cout << "fBlacksheet thickness = " << fBlacksheetThickness << " (in m = " << fBlacksheetThickness/m << ")" << std::endl;
  // G4VPhysicalVolume* physiTopCapAssembly =
  //
  
  G4VPhysicalVolume* physiTopCapAssembly = new G4PVPlacement(0,
//  									G4ThreeVector(0.,0.,0.5 * (fGeoConfig->GetInnerHeight() + fCapAssemblyHeight)),
  									G4ThreeVector(0.,0.,0.5 * (fBarrelLengthForCells + fCapAssemblyHeight)),
  									fCapLogicTop,
  									"TopCapAssembly",
  									fBarrelLogic,
  									false, 0,true);
  G4VPhysicalVolume* physiBottomCapAssembly = new G4PVPlacement(0,
//  									G4ThreeVector(0.,0., -0.5 * (fGeoConfig->GetInnerHeight() + fCapAssemblyHeight)),
  									G4ThreeVector(0.,0., -0.5 * (fBarrelLengthForCells + fCapAssemblyHeight)),
  									fCapLogicBottom,
										"BottomCapAssembly",
										fBarrelLogic,
										false, 0,true);
    // G4cout << "physiTopCapAssembly: " << physiTopCapAssembly << std::endl;
    // G4cout << "physiBottomCapAssembly: " << physiBottomCapAssembly << std::endl;

  std::cout << " Placed cap assembly at " << -0.5 * (fGeoConfig->GetInnerHeight() + fCapAssemblyHeight);

	  G4LogicalBorderSurface * WaterBSTopCapEndSurface = NULL;
	  WaterBSTopCapEndSurface = new G4LogicalBorderSurface("WaterBSTopCapPolySurface",
	                                   	   	   	   	   	    physiTopCapAssembly,fCapBSTopPhysics,
	                                   	   	   	   	   	    WCSimMaterialsBuilder::Instance()->GetOpticalSurface("WaterBSCellSurface"));

	  G4LogicalBorderSurface * WaterWSTopCapEndSurface = NULL;
	  WaterWSTopCapEndSurface = new G4LogicalBorderSurface("WaterWSTopCapPolySurface",
	                                   	   	   	   	   	    physiTopCapAssembly,fCapWSTopPhysics,
	                                   	   	   	   	   	    WCSimMaterialsBuilder::Instance()->GetOpticalSurface("WaterWSCellSurface"));
	  G4LogicalBorderSurface * WaterBSBottomCapEndSurface = NULL;
	  WaterBSBottomCapEndSurface = new G4LogicalBorderSurface("WaterBSBottomCapPolySurface",
	                                   	   	   	   	   	    physiBottomCapAssembly,fCapBSBottomPhysics,
	                                   	   	   	   	   	    WCSimMaterialsBuilder::Instance()->GetOpticalSurface("WaterBSCellSurface"));

	  G4LogicalBorderSurface * WaterWSBottomCapEndSurface = NULL;
	  WaterWSBottomCapEndSurface = new G4LogicalBorderSurface("WaterWSBottomCapPolySurface",
	                                   	   	   	   	   	    physiBottomCapAssembly,fCapWSBottomPhysics,
	                                   	   	   	   	   	    WCSimMaterialsBuilder::Instance()->GetOpticalSurface("WaterWSCellSurface"));
    return;
}

void WCSimCherenkovBuilder::CreateSensitiveDetector() {

	  G4SDManager* SDman = G4SDManager::GetSDMpointer();
	  if (!aWCPMT)
	  {
	    aWCPMT = new WCSimWCSD( "/WCSim/glassFaceWCPMT",this );
	    SDman->AddNewDetector( aWCPMT );
	  }

    fPMTBuilder.SetSensitiveDetector( aWCPMT );
}


G4LogicalVolume * WCSimCherenkovBuilder::ConstructWC()
{
	std::cout << " *** In WCSimCherenkovBuilder::ConstructWC() *** " << std::endl;
  WCSimGeoManager * manager = new WCSimGeoManager();
  fGeoConfig = new WCSimGeoConfig(manager->GetGeometryByName(fDetectorName));
  assert(manager->GeometryExists(fDetectorName));
  delete manager;
  return ConstructDetector();
}

void WCSimCherenkovBuilder::SetPositions()
{
	 //-----------------------------------------------------
	 // Positions
	 //-----------------------------------------------------
	 fDebugMode = true;
	 WCPosition = 0.;	  //Set the WC tube offset to zero

	 WCIDRadius = (G4int)fGeoConfig->GetOuterRadius();

	 // the number of regular cell in phi direction:
	 WCBarrelRingNPhi = fGeoConfig->GetNSides();
	 // the part of one ring, that is covered by the regular cells:
	 totalAngle = 2.0 * pi * rad
	 * (WCBarrelRingNPhi * WCPMTperCellHorizontal
	 / WCBarrelNumPMTHorizontal);
	 // angle per regular cell:
	 dPhi = totalAngle / WCBarrelRingNPhi;
	 // it's hight:
	 barrelCellHeight = (WCIDHeight - 2. * WCBarrelPMTOffset) / WCBarrelNRings;
	 // the hight of all regular cells together:
	 mainAnnulusHeight = WCIDHeight - 2. * WCBarrelPMTOffset
	 - 2. * barrelCellHeight;

	 //  innerAnnulusRadius = fGeoConfig->GetOuterRadius() - WCPMTExposeHeight-1.*mm;
	 //==>	 innerAnnulusRadius = fGeoConfig->GetOuterRadius() - fPMTConfigs[0].GetExposeHeight()
	 innerAnnulusRadius = fGeoConfig->GetOuterRadius() - fPMTConfigs[0].GetMaxExposeHeight()
	 - 1. * mm;
	 outerAnnulusRadius = fGeoConfig->GetOuterRadius() + fBlacksheetThickness + 1. * mm;//+ Stealstructure etc.
	 // the radii are measured to the center of the surfaces
	 // (tangent distance). Thus distances between the corner and the center are bigger.
	 WCLength = WCIDHeight + 2 * 2.3 * m;//jl145 - reflects top veto blueprint, cf. Farshid Feyzi
	 WCRadius = (WCIDDiameter / 2. + fBlacksheetThickness + 1.5 * m)
	 / cos(dPhi / 2.);

	 // now we know the extend of the detector and are able to tune the tolerance
	 G4GeometryManager::GetInstance()->SetWorldMaximumExtent( WCLength > WCRadius ? WCLength : WCRadius);
	 G4cout << "Computed tolerance = "
	 << G4GeometryTolerance::GetInstance()->GetSurfaceTolerance() / mm
	 << " mm" << G4endl;

	 //Decide if adding Gd
	 water = "Water";
	 if (WCAddGd) {
	 water = "Doped Water";
	 }
}

void WCSimCherenkovBuilder::Update() {
  std::cout << " *** WCSimCherenkovBuilder::Update *** " << std::endl;

        for(unsigned int iPipe = 0; iPipe < fPlanePipeLogics.size(); ++iPipe)
	  {
	    // std::cout << "Deleting Pipe " << iPipe << std::endl;
	    if( fPlanePipeLogics.at(iPipe) != NULL ){  delete fPlanePipeLogics.at(iPipe); }
	  }
	fPlanePipeLogics.clear();
	fPlanePipePhysics.clear();
	// std::cout << "All Pipes  -  deleted! " << std::endl;


	for(unsigned int iCell = 0; iCell < fWallUnitCells.size(); ++iCell)
	{
		delete fWallUnitCells.at(iCell);
	}
	fWallUnitCells.clear();

	for(unsigned int iCell = 0; iCell < fTopUnitCells.size(); ++iCell)
	{
		delete fTopUnitCells.at(iCell);
	}
	fTopUnitCells.clear();

	for(unsigned int iCell = 0; iCell < fBottomUnitCells.size(); ++iCell)
	{
		delete fBottomUnitCells.at(iCell);
	}
	fBottomUnitCells.clear();


	for(unsigned int iWall = 0; iWall < fSegmentLogics.size(); ++iWall)
	{
		if( fPrismWallLogics.at(iWall) != NULL ){ delete fPrismWallLogics.at(iWall);}
		if( fPrismWallPhysics.at(iWall) != NULL ){ delete fPrismWallPhysics.at(iWall);}
		if( fPrismRingLogics.at(iWall) != NULL ) { delete fPrismRingLogics.at(iWall); }
    // Can't do the same with fPrismRingPhysics... get a glibc error when clearing the vector.
    // Maybe GEANT is doing something behind the scenes and deleting it.
	}
  fPrismWallLogics.clear();
  fPrismWallPhysics.clear();
  fPrismRingLogics.clear();
  fPrismRingPhysics.clear();

	for(unsigned int iSegment = 0; iSegment < fSegmentLogics.size(); ++iSegment)
	{
		if( fSegmentLogics.at(iSegment) != NULL ){ delete fSegmentLogics.at(iSegment);}
		if( fSegmentPhysics.at(iSegment) != NULL ){ delete fSegmentPhysics.at(iSegment);}
	}
  fSegmentLogics.clear();
  fSegmentPhysics.clear();


	if( fGeoConfig != NULL ) { delete fGeoConfig; }
  WCSimGeoManager * manager = new WCSimGeoManager();
  fGeoConfig = new WCSimGeoConfig(manager->GetGeometryByName(fDetectorName));
  delete manager;
  std::cout << "Update geometry to " << fDetectorName << std::endl;

	fPMTBuilder.Reset();
	delete fCapLogicBottom;
  delete fCapLogicTop;
  delete fPrismLogic;
  delete fBarrelLogic;
  delete fLakeLogic;
	fLakeLogic = NULL;
	fBarrelLogic = NULL;
	fPrismLogic = NULL;
	fCapLogicTop = NULL;
	fCapLogicBottom = NULL;
	fConstructed = false;
  fGotMeasurements = false;
	fNumPMTs = 0;
  
  fGotMeasurements = false;
	fBarrelRadius = -999 * m; // Barrel is a cylinder - this is its actual radius
	fBarrelHeight = -999 * m;
	fBarrelLengthForCells = -999 * m;

	fVetoRadiusInside = -999 * m; // Radius is to centre of wall not the vertex
	fVetoRadiusOutside = -999 * m;
	fVetoHeight = -999 * m;

	fPrismRadiusInside = -999 * m; // Radius is to centre of wall not the vertex
	fPrismRadiusOutside = -999 * m;
	fPrismHeight = -999 * m;

	fPrismRingRadiusInside = -999 * m; // Radius is to centre of wall not the vertex
	fPrismRingRadiusOutside = -999 * m;

	fPrismRingSegmentRadiusInside = -999 * m; // To centre of segment not edge
	fPrismRingSegmentRadiusInside = -999 * m;
	fPrismRingSegmentDPhi = -999 * m;

	fPrismRingSegmentBSRadiusInside = -999 * m; // To centre not edge
	fPrismRingSegmentBSRadiusOutside = -999 * m;

	WCSimDetectorConstruction::Update();
}

double WCSimCherenkovBuilder::GetZoneCoverage(WCSimGeometryEnums::DetectorRegion_t region, int zone)
{
	double coverage = 0.0;
	if(fGeoConfig->GetUseOverallCoverage()){
		std::cout << "Getting straight from geoConfig " << fGeoConfig->GetCoverageFraction() << std::endl;
		return fGeoConfig->GetCoverageFraction(); }
	else if(fGeoConfig->GetUseZonalCoverage()){
		std::cout << "Getting zonal coverage from geoConfig " << fGeoConfig->GetCoverageFraction() << std::endl;
		return fGeoConfig->GetZonalCoverageFraction(region, zone); }
	else {
		std::cout << "Still getting zonal coverage from geoConfig " << fGeoConfig->GetCoverageFraction() << std::endl;
		return fGeoConfig->GetZonalCoverageFraction(region, zone); } // Default to same as above for now - tries for max coverage with a fixed number of PMTs
	assert(coverage != 0.0);
	return coverage;
}

void WCSimCherenkovBuilder::SetCustomDetectorName()
{
}

double WCSimCherenkovBuilder::GetOptimalTopCellSize(int zoneNum)
{
	return GetOptimalEndcapCellSize(WCSimGeometryEnums::DetectorRegion_t::kTop, zoneNum);
}

double WCSimCherenkovBuilder::GetOptimalBottomCellSize(int zoneNum)
{
	return GetOptimalEndcapCellSize(WCSimGeometryEnums::DetectorRegion_t::kBottom, zoneNum);
}

void WCSimCherenkovBuilder::ConstructPMTs()
{
	std::cout << " *** In WCSimCherenkovBuilder::ConstructPMTs *** " << std::endl;
    std::cout << "SIZE OF CONFIGS VECTOR = " << fPMTConfigs.size() << std::endl;
	fPMTBuilder.ConstructPMTs( fPMTConfigs );
}

G4RotationMatrix WCSimCherenkovBuilder::GetArbitraryPMTFaceRotation(double theta, double phi){

  G4RotationMatrix rotation;    
    rotation.setTheta(theta * deg);
    rotation.setPhi(  phi   * deg);

  return rotation;
}


G4RotationMatrix WCSimCherenkovBuilder::GetEndcapPMTFaceRotation(WCSimGeometryEnums::PMTDirection_t type, G4int zflip){

  G4RotationMatrix rotation;
  if     ( type == WCSimGeometryEnums::PMTDirection_t::kInwards );  
  else if( type == WCSimGeometryEnums::PMTDirection_t::kVeto );
  else if( type == WCSimGeometryEnums::PMTDirection_t::kAngledUpstream && zflip == 1) rotation.rotateY(( 45) * deg);   // 
  else if( type == WCSimGeometryEnums::PMTDirection_t::kAngledUpstream && zflip ==-1) rotation.rotateY((-45) * deg);  //
  else G4cerr << "PMT Rotation Option NOT Defined or Implemented"  << G4endl;

  return rotation;
}


G4RotationMatrix WCSimCherenkovBuilder::GetBarrelPMTFaceRotation(WCSimGeometryEnums::PMTDirection_t type, G4int zone){

  G4RotationMatrix rotation;
  if     ( type == WCSimGeometryEnums::PMTDirection_t::kInwards );
  else if( type == WCSimGeometryEnums::PMTDirection_t::kVeto );
  else if( type == WCSimGeometryEnums::PMTDirection_t::kAngledUpstream){

    G4RotationMatrix *segmentRotation = (fPrismWallPhysics.at(zone))->GetRotation();
    if( fabs( segmentRotation->phiX()/deg ) < 90 )                     // Rotate PMTs for backward facing segments only
      rotation.rotateZ(-0.55*segmentRotation->phiX());  
    
  }
  else G4cerr << "PMT Rotation Option NOT Defined or Implemented"  << G4endl;

  return rotation;
}

