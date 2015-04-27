/*
 * WCSimCherenkovBuilder.cc
 *
 *  Created on: Aug 15, 2014
 *      Author: aperch
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

	fBlacksheetThickness = 2 * mm;
	fDebugMode 			 = true;

  fPMTManager = new WCSimPMTManager();

	fLakeLogic = NULL;
	fBarrelLogic = NULL;
	fPrismLogic = NULL;
	fCapLogicTop = NULL;
	fCapLogicBottom = NULL;
	fCapLogicBottomRing = NULL;
	fNumPMTs = 0;

	// Initialize all the constants:
  fGotMeasurements = false;
	fBarrelRadius = -999 * m; // Barrel is a cylinder - this is its actual radius
	fBarrelHeight = -999 * m;
	fBarrelLengthForCells = -999 * m;

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
	G4double shoreDistance = 20; // Distance from detector to shore (m)

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
	new G4PVPlacement(prismRotation, 
                            G4ThreeVector(0., 0., 0.),
														fBarrelLogic, "barrelTubs",
														fLakeLogic, false, 0);
}

void WCSimCherenkovBuilder::ConstructVeto() {
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
		                                      fPrismRingSegmentBSRadiusInside,};


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
				// G4VPhysicalVolume* physiWCBarrelPMT =
				new G4PVPlacement(WCPMTRotation,     // its rotation
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
			}
		}
		std::cout << "PMTs placed on walls!" << std::endl;
	}
}

WCSimGeoConfig * WCSimCherenkovBuilder::GetGeoConfig() const {
	return fGeoConfig;
}

void WCSimCherenkovBuilder::GetMeasurements()
{
    if( fGotMeasurements) { return; }
    double epsilon = 0.00001 * mm; // Add a tiny bit of gap between supposedly overlapping volumes 
                                   // to prevent Geant4 getting stuck at the tracking action
    int nEpsilons = 5;

    // Barrel measurements
    fBarrelRadius = fGeoConfig->GetOuterRadius() + 1*m; // Make it 1m bigger in radius
    fBarrelHeight = fGeoConfig->GetInnerHeight() + 2* fBlacksheetThickness + 2 * GetMaxCapExposeHeight() + 2*m;  // and half-height
    fBarrelLengthForCells = fGeoConfig->GetInnerHeight() - 2.0 * GetMaxCapExposeHeight();

    fPrismRadiusInside = fGeoConfig->GetInnerRadius() - epsilon;
    fPrismRadiusOutside = fGeoConfig->GetInnerRadius() + fBlacksheetThickness + GetMaxBarrelExposeHeight() + nEpsilons * epsilon;
    fPrismHeight = fBarrelLengthForCells;

    assert( --nEpsilons > 0);
    fPrismWallRadiusInside  = fPrismRadiusInside + epsilon;
    fPrismWallRadiusOutside = fPrismRadiusOutside - epsilon;

    assert(--nEpsilons > 0);
    fPrismRingRadiusInside = fPrismRadiusInside + epsilon;
    fPrismRingRadiusOutside = fPrismRadiusOutside - epsilon;

    assert(--nEpsilons > 0);
    fPrismRingSegmentRadiusInside = fPrismRadiusInside-1*mm + epsilon;
    fPrismRingSegmentRadiusOutside = fPrismRadiusOutside+1*mm - epsilon;
    fPrismRingSegmentDPhi = 360*deg / fGeoConfig->GetNSides();

    assert(--nEpsilons > 0);
    fPrismRingSegmentBSRadiusInside = fPrismRingSegmentRadiusOutside - fBlacksheetThickness + epsilon;
    fPrismRingSegmentBSRadiusOutside = fPrismRingSegmentRadiusOutside+1*mm - epsilon;
    

    // Cap measurements
	  fCapAssemblyHeight = fBlacksheetThickness + GetMaxCapExposeHeight() + 0.5 * (fGeoConfig->GetInnerHeight() - fBarrelLengthForCells);
    // nb. fCapAssemblyHeight = fCapRingHeight + fCapPolygonHeight
	  fCapAssemblyRadius = fBarrelRadius;

	  fCapRingRadiusInside = fPrismRadiusInside;
	  fCapRingRadiusOutside = fPrismRadiusOutside;
	  fCapRingHeight = 0.5 * (fGeoConfig->GetInnerHeight() - fBarrelLengthForCells);

	  fCapRingSegmentDPhi = fPrismRingSegmentDPhi;
	  fCapRingSegmentRadiusInside = fPrismRadiusInside;
	  fCapRingSegmentRadiusOutside = fPrismRadiusOutside;
	  fCapRingSegmentHeight = fCapRingHeight;

	  fCapRingSegmentBSRadiusInside = fCapRingSegmentRadiusOutside - fBlacksheetThickness - 2*epsilon;
	  fCapRingSegmentBSRadiusOutside = fCapRingSegmentRadiusOutside - epsilon;
	  fCapRingSegmentBSHeight = fCapRingSegmentHeight;

	  fCapPolygonRadius = fGeoConfig->GetInnerRadius() + GetMaxBarrelExposeHeight() + fBlacksheetThickness;
	  fCapPolygonHeight = GetMaxCapExposeHeight() + fBlacksheetThickness;

    fCapPolygonCentreRadius = fGeoConfig->GetInnerRadius() + GetMaxBarrelExposeHeight();
    fCapPolygonCentreHeight = GetMaxCapExposeHeight();

	  fCapPolygonEdgeBSRadiusInside = fGeoConfig->GetInnerRadius() + GetMaxBarrelExposeHeight();
	  fCapPolygonEdgeBSRadiusOutside = fCapPolygonRadius;
	  fCapPolygonEdgeBSHeight = fCapPolygonHeight;

	  fCapPolygonEndBSRadius = fGeoConfig->GetInnerRadius() + GetMaxBarrelExposeHeight();
	  fCapPolygonEndBSHeight = fBlacksheetThickness;

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
  double capPolygonOuterRadius = WCSimPolygonTools::GetOuterRadiusFromInner(fGeoConfig->GetNSides(), fCapPolygonCentreRadius);
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

	G4Tubs* capSolid = new G4Tubs("CapAssembly", 0.0*m,
								  fCapAssemblyRadius,
								  0.5 * fCapAssemblyHeight, // G4Tubs takes the half-height
								  0.*deg,
								  360.*deg);

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

	  std::cout << " Physics cap volume lives at " << (capAssemblyHeight/2.- GetBarrelLengthForCells()/2.)*zflip << std::endl;
 						std::cout <<  "Place phsyiEndCapRing at "
 								  << ((capAssemblyHeight/2.- (fGeoConfig->GetInnerHeight() - GetBarrelLengthForCells())/2.)*zflip)
 								  << " - height is " << borderRingZ[2] - borderRingZ[0]
 								  << " c.f. capAssemblyHeight = " << capAssemblyHeight << std::endl;
 	  // G4VPhysicalVolume* physiEndCapRing =
    new G4PVPlacement(endCapRotation,
    									G4ThreeVector(0.,0., (0.5 * fCapAssemblyHeight - 0.5 * fCapRingHeight) * zflip),
											logicEndCapRing,
											"EndCapRing",
											capLogic,
											false, 0,true);
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


    // Now we've placed the physical volume, let's add some blacksheet:

	G4double capSegmentBlacksheetZ[2] = { -fCapRingSegmentBSHeight/2.0, fCapRingSegmentBSHeight/2.0 };
	G4double capSegmentBlacksheetRmax[2] = { fCapRingSegmentBSRadiusOutside, fCapRingSegmentBSRadiusOutside };
	G4double capSegmentBlacksheetRmin[2] = { fCapRingSegmentBSRadiusInside, fCapRingSegmentBSRadiusInside,};

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

  std::cout << "But not here?" << std::endl;
	G4LogicalBorderSurface * WaterBSCapCellSurface = NULL;

	WaterBSCapCellSurface = new G4LogicalBorderSurface("WaterBSCapCellSurface",
														capSegmentPhysic,
														capSegmentBlacksheetPhysic,
														WCSimMaterialsBuilder::Instance()->GetOpticalSurface("WaterBSCellSurface"));

	G4VisAttributes* WCCellBlacksheetCellVisAtt = new G4VisAttributes(G4Colour(80./255.0, 51./255.0, 204./255.0));
	if (fDebugMode ){
		capSegmentBlacksheetLogic->SetVisAttributes(WCCellBlacksheetCellVisAtt);
	}
	else{
		capSegmentBlacksheetLogic->SetVisAttributes(G4VisAttributes::Invisible);
	}

	  return;
}

void WCSimCherenkovBuilder::ConstructEndCapSurfaces(G4int zflip){

	  G4LogicalVolume * capLogic = NULL;
	  if( zflip == -1 ) { capLogic = fCapLogicTop; }
	  else { capLogic = fCapLogicBottom; }

	  G4Material * pureWater = WCSimMaterialsBuilder::Instance()->GetMaterial("Water");
	  G4Material * blacksheet = WCSimMaterialsBuilder::Instance()->GetMaterial("Blacksheet");

	  //------------------------------------------------------------
	  // Add the flat section of the cap
	  // -----------------------------------------------------------
    std::cout << "Blacksheet thickness = " << fBlacksheetThickness << std::endl;

    std::cout << "fCapPolygonHeight = " << fCapPolygonHeight << std::endl;
    std::cout << "fCapAssemblyHeight = " << fCapAssemblyHeight << std::endl;
	  G4double capPolygonZ[2] = { -fCapPolygonHeight * zflip/2.0, fCapPolygonHeight * zflip/2.0 };
	  G4double capPolygonRMin[2] = {  0. , 0. } ;
	  G4double capPolygonRMax[2] = {fCapPolygonRadius, fCapPolygonRadius};
	  G4VSolid* solidCapPolygon = new G4Polyhedra("CapPolygon",
											                     0. * deg, // phi start
											                     360. * deg, //phi end
											                     fGeoConfig->GetNSides(), //NPhi-gon
											                     2, // 2 z-planes
											                     capPolygonZ, //position of the Z planes
											                     capPolygonRMin, // min radius at the z planes
											                     capPolygonRMax// max radius at the Z planes
											                    );

	  // G4cout << *solidCapPolygon << G4endl;
	  G4LogicalVolume* logicCapPolygon = new G4LogicalVolume(solidCapPolygon, pureWater, "CapPolygon",0,0,0);


    // First make the centre:
	  G4double capPolygonCentreZ[2] = { -fCapPolygonCentreHeight * zflip/2.0, fCapPolygonCentreHeight * zflip/2.0 };
	  G4double capPolygonCentreRMin[2] = {  0. , 0. } ;
	  G4double capPolygonCentreRMax[2] = {fCapPolygonCentreRadius, fCapPolygonCentreRadius};
    G4VSolid* solidCapPolygonCentre = new G4Polyhedra("CapPolygonCentre", 
                                                      0.0 * deg, 360.0 * deg,
                                                      fGeoConfig->GetNSides(),
                                                      2,
                                                      capPolygonCentreZ,
                                                      capPolygonCentreRMin,
                                                      capPolygonCentreRMax);
    G4LogicalVolume* logicCapPolygonCentre = new G4LogicalVolume(solidCapPolygonCentre, pureWater, "CapPolygonCentre",0,0,0);
    // G4VPhysicalVolume* physiCapPolygonCentre =
    new G4PVPlacement( 0,
    									 G4ThreeVector(0.,0., (-0.5 * fCapPolygonHeight + 0.5 * fCapPolygonCentreHeight) * zflip),
    									 logicCapPolygonCentre,
    									 "CapPolygonCentre",
    									 logicCapPolygon,
    									 false, 0, true );
    // std::cout << *solidCapPolygonCentre << std::endl;

    // Now the outside edges:




	  if(fDebugMode){
		  G4VisAttributes* tmpVisAtt2 = new G4VisAttributes(G4Colour(.6,0.5,0.5));
		  tmpVisAtt2->SetForceWireframe(true);
		  logicCapPolygon->SetVisAttributes(tmpVisAtt2);
	  }
	  else{
	    logicCapPolygon->SetVisAttributes(G4VisAttributes::Invisible);
	  }

	  //---------------------------------------------------------------------
	  // add cap blacksheet
	  // -------------------------------------------------------------------

	  // To the edge...
    std::cout << "fCapPolygonEdgeBSHeightInside = " << fCapPolygonEdgeBSHeight << std::endl
              << "fCapPolygonCentreHeight = " << fCapPolygonCentreHeight << std::endl
              << "fCapPolygonHeight = " << fCapPolygonHeight << std::endl;

	  G4double capEdgeBlacksheetZ[2] = {-0.5 * fCapPolygonEdgeBSHeight * zflip, 0.5 * fCapPolygonEdgeBSHeight * zflip};
	  G4double capEdgeBlacksheetRmin[2] = {fCapPolygonEdgeBSRadiusInside, fCapPolygonEdgeBSRadiusInside};
	  G4double capEdgeBlacksheetRmax[2] = {fCapPolygonEdgeBSRadiusOutside, fCapPolygonEdgeBSRadiusOutside};

	  G4VSolid* solidWCCapEdgeBlacksheet = new G4Polyhedra("WCCapEdgeBlacksheet",
														0.*deg, // phi start
														360.0, //total phi
														fGeoConfig->GetNSides(), //NPhi-gon
														2, //  z-planes
														capEdgeBlacksheetZ, //position of the Z planes
														capEdgeBlacksheetRmin, // min radius at the z planes
														capEdgeBlacksheetRmax// max radius at the Z planes
														);

	  G4LogicalVolume* logicWCCapEdgeBlacksheet = new G4LogicalVolume(solidWCCapEdgeBlacksheet,
																  blacksheet,
																  "WCCapEdgeBlacksheet",
																  0,0,0);
	  G4VPhysicalVolume* physiWCCapEdgeBlacksheet = new G4PVPlacement(0,
                                  G4ThreeVector(0.,0., (-0.5 * fCapPolygonHeight + 0.5 * fCapPolygonEdgeBSHeight) * zflip),
																  logicWCCapEdgeBlacksheet,
																  "WCCapEdgeBlacksheet",
																  logicCapPolygon,
																  false,
																  0,true);

	  // And to the end...
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
	  // G4VPhysicalVolume* physiWCCapEndBlacksheet =
	  new G4PVPlacement(0,
	  									G4ThreeVector(0.,0., (0.5 * fCapPolygonHeight - 0.5 * fCapPolygonEndBSHeight) * zflip),
	  									logicWCCapEndBlacksheet,
	  									"WCCapEndBlacksheet",
	  									logicCapPolygon,
	  									false,
	  									0,true);

	  // And finally place the whole container volume
    
    std::cout << "physical cap volume at " << (-capAssemblyHeight/2.+1*mm+fBlacksheetThickness)*zflip << std::endl;
	  G4VPhysicalVolume* physiCapPolygon = new G4PVPlacement(0,                           // no rotation
						  	  	  	  	  	  	  	  	G4ThreeVector(0.,0., (-0.5 * fCapAssemblyHeight + 0.5 * fCapPolygonHeight) * zflip),     // its position
													                                logicCapPolygon,  // its logical volume
													                                "CapPolygon",     // its name
													                                capLogic,    // its mother volume
													                                false,       // no boolean operations
													                                0,true);     // Copy #


	  G4LogicalBorderSurface * WaterBSBottomCapEdgeSurface = NULL;
	  WaterBSBottomCapEdgeSurface = new G4LogicalBorderSurface("WaterBSCapPolySurface",
	                                   	   	   	   	   	    physiCapPolygon,physiWCCapEdgeBlacksheet,
	                                   	   	   	   	   	    WCSimMaterialsBuilder::Instance()->GetOpticalSurface("WaterBSCellSurface"));

	  G4LogicalBorderSurface * WaterBSBottomCapEndSurface = NULL;
	  WaterBSBottomCapEndSurface = new G4LogicalBorderSurface("WaterBSCapPolySurface",
	                                   	   	   	   	   	    physiCapPolygon,physiWCCapEdgeBlacksheet,
	                                   	   	   	   	   	    WCSimMaterialsBuilder::Instance()->GetOpticalSurface("WaterBSCellSurface"));

	  G4VisAttributes* WCCapBlacksheetVisAtt = new G4VisAttributes(G4Colour(0.9,0.2,0.2));
	    if(fDebugMode || 1)
	    {
	    	logicWCCapEndBlacksheet->SetVisAttributes(WCCapBlacksheetVisAtt);
        	logicWCCapEdgeBlacksheet->SetVisAttributes(WCCapBlacksheetVisAtt);
	    }
	    else
	    {
	        logicWCCapEndBlacksheet->SetVisAttributes(G4VisAttributes::Invisible);
        	logicWCCapEdgeBlacksheet->SetVisAttributes(G4VisAttributes::Invisible);
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

		// Angled walls mean squares don't tile very well
		// We'll consider their positions, but we only need to get these vectors once
		// if they live outside the loop.;
		std::vector<Double_t> pmtX = fGeoConfig->GetCellPMTX(region, iZone);
		std::vector<Double_t> pmtY = fGeoConfig->GetCellPMTY(region, iZone);
		std::vector<std::string> pmtNames = fGeoConfig->GetCellPMTName(region,iZone);
		std::vector<Double_t> pmtRad;
		for (unsigned int iPMT = 0; iPMT < pmtNames.size(); ++iPMT) {
			pmtRad.push_back(fPMTManager->GetPMTByName(pmtNames.at(iPMT)).GetRadius());
		}

  	    double capPolygonOuterRadius = WCSimPolygonTools::GetOuterRadiusFromInner(fGeoConfig->GetNSides(), fCapPolygonCentreRadius);
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

						G4TwoVector pmtCellPosition = unitCell->GetPMTPos(iPMT, cellSizeVec->at(iZone)); // PMT position in cell, relative to top left of cell
						G4ThreeVector PMTPosition(topLeftCell.x() + pmtCellPosition.x(),
								topLeftCell.y() - pmtCellPosition.y(), 0.0);
						WCSimPMTConfig config = unitCell->GetPMTPlacement(iPMT).GetPMTConfig();
						// G4VPhysicalVolume* physiCapPMT =
						new G4PVPlacement(WCCapPMTRotation,     // its rotation
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
					}
				}

				yPos = yPos + cellSide;
			}
			xPos = xPos + cellSide;
		}

    }
      
    for (unsigned int i = 0; i < placedTop.size(); ++i)
    {
      std::cout << region.AsString() << ": placed " << placedTop.at(i) << " pmts in zone " << i << std::endl;
    }
}

void WCSimCherenkovBuilder::ConstructEndCapPhysicalVolumes(){
  // std::cout << "mainAnnulusHeight = " << mainAnnulusHeight << " (in m = " << mainAnnulusHeight/m << ")" << std::endl;
  // std::cout << "capAssemblyHeight = " << capAssemblyHeight << " (in m = " << capAssemblyHeight/m << ")" << std::endl;
  // std::cout << "InnerHeight=  " << fGeoConfig->GetInnerHeight() << " (in m = " << fGeoConfig->GetInnerHeight()/m << ")" << std::endl;
  // std::cout << "BarrelLengthForCells = " << GetBarrelLengthForCells() << " (in m = " << GetBarrelLengthForCells()/m << ")" << std::endl;
  // std::cout << "fBlacksheet thickness = " << fBlacksheetThickness << " (in m = " << fBlacksheetThickness/m << ")" << std::endl;
  // G4VPhysicalVolume* physiTopCapAssembly =
  new G4PVPlacement(0,
  									G4ThreeVector(0.,0.,0.5 * (fGeoConfig->GetInnerHeight() + fCapAssemblyHeight)),
  									fCapLogicTop,
  									"TopCapAssembly",
  									fBarrelLogic,
  									false, 0,true);
  // G4VPhysicalVolume* physiBottomCapAssembly =
  new G4PVPlacement(0,
  									G4ThreeVector(0.,0., -0.5 * (fGeoConfig->GetInnerHeight() + fCapAssemblyHeight)),
  									fCapLogicBottom,
										"BottomCapAssembly",
										fBarrelLogic,
										false, 0,true);
    // G4cout << "physiTopCapAssembly: " << physiTopCapAssembly << std::endl;
    // G4cout << "physiBottomCapAssembly: " << physiBottomCapAssembly << std::endl;
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
	 innerAnnulusRadius = fGeoConfig->GetOuterRadius() - fPMTConfigs[0].GetExposeHeight()
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
