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
		fConstructed(false), fGeoConfig(NULL), WCSimDetectorConstruction(DetConfig) {

	fBlacksheetThickness = 2 * mm;
	fDebugMode 			 = true;

  fPMTManager = new WCSimPMTManager();

	fLakeLogic = NULL;
	fBarrelLogic = NULL;
	fPrismLogic = NULL;
	fPrismRingLogic = NULL;
	fSegmentLogic = NULL;
	fCapLogicTop = NULL;
	fCapLogicBottom = NULL;
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
	fPrismRingHeight = -999 * m;

	fPrismRingSegmentRadiusInside = -999 * m; // To centre of segment not edge
	fPrismRingSegmentRadiusInside = -999 * m;
	fPrismRingSegmentHeight = -999 * m;
	fPrismRingSegmentDPhi = -999 * m;

	fPrismRingSegmentBSRadiusInside = -999 * m; // To centre not edge
	fPrismRingSegmentBSRadiusOutside = -999 * m;
	fPrismRingSegmentBSHeight = -999 * m;

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
	for(unsigned int iCell = 0; iCell < fUnitCells.size(); ++iCell)
	{
		delete fUnitCells[iCell];
	}
	fUnitCells.clear();

}



G4LogicalVolume * WCSimCherenkovBuilder::ConstructDetector() {
  std::cout << "*** WCSimCherenkovBuilder::ConstructDetector *** " << std::endl;
  std::cout << "Constructing " << fDetectorName << std::endl;
  WCSimGeoManager * manager = new WCSimGeoManager();
  fGeoConfig = new WCSimGeoConfig(manager->GetGeometryByName(fDetectorName));
  delete manager;
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

		std::cout << "Top cap logical volume: " << fCapLogicTop->GetName() << std::endl;
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
	G4VPhysicalVolume* barrelPhysic = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.),
														fBarrelLogic, "barrelTubs",
														fLakeLogic, false, 0);
}

void WCSimCherenkovBuilder::ConstructVeto() {
	return;
}

void WCSimCherenkovBuilder::ConstructInnerDetector() {
	CreatePrism();
	CreatePrismRings();
	CreateRingSegments();
	CreateSegmentCells();
}

void WCSimCherenkovBuilder::CreatePrism() {
	G4double WCPosition = 0.;	  //Set the WC tube offset to zero


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

	G4VPhysicalVolume* prismPhysic = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.),
														fPrismLogic, "prism",
														fBarrelLogic, false, 0, true);
}

void WCSimCherenkovBuilder::CreatePrismRings() {
	// Slice the prism up lengthways


	//-----------------------------------------------------
	// Subdivide the BarrelAnnulus into rings
	//-----------------------------------------------------
  std::cout << "Wall cells in z = " << fWallCellsZ << std::endl;
	G4Material * pureWater  = WCSimMaterialsBuilder::Instance()->GetMaterial("Water");

  G4double RingZ[2] 			= { -0.5 * fPrismRingHeight, 0.5 * fPrismRingHeight };
	G4double mainAnnulusRmin[2] = { fPrismRingRadiusInside, fPrismRingRadiusInside };
	G4double mainAnnulusRmax[2] = { fPrismRingRadiusOutside, fPrismRingRadiusOutside };

	// Now make all the volumes
    std::cout << "Making solid, nSides = " << fGeoConfig->GetNSides() << std::endl;
    std::cout << "fPrismRingHeight = " << fPrismRingHeight << "  fPrismRingSegmentRadiusInside = " << fPrismRingSegmentRadiusInside << " and fPrismRingSegmentRadiusOutside = " << fPrismRingRadiusOutside << std::endl;
	G4Polyhedra* prismRingSolid = new G4Polyhedra("prismRing", 0. * deg,// phi start
													 360.0 * deg, //phi end
													 (G4int) fGeoConfig->GetNSides(), //NPhi-gon
													 2, RingZ, mainAnnulusRmin, mainAnnulusRmax);

    std::cout << "Making logic" << std::endl;
	fPrismRingLogic = new G4LogicalVolume(prismRingSolid,
                                          pureWater, "prismRing", 0, 0, 0);

    std::cout << "Making physic" << std::endl;
	G4VPhysicalVolume* prismRingPhysic = new G4PVReplica("prismRing",
                                                         fPrismRingLogic, fPrismLogic, kZAxis,
		                          						 (G4int) fWallCellsZ, fPrismRingHeight);
    std::cout << "Made!" << std::endl;
	if (!fDebugMode)
		fPrismLogic->SetVisAttributes(G4VisAttributes::Invisible);
	else {
		G4VisAttributes* tmpVisAtt = new G4VisAttributes(G4Colour(0, 0.5, 1.));
		tmpVisAtt->SetForceWireframe(true);
		fPrismRingLogic->SetVisAttributes(tmpVisAtt);
	}
}

void WCSimCherenkovBuilder::CreateRingSegments() {

	// Set up constants (factorise these somewhere else later)
	G4Material * pureWater = WCSimMaterialsBuilder::Instance()->GetMaterial("Water");

	// Zip these up into arrays to pass to Geant
	G4double RingZ[2] 			= { -0.5 * fPrismRingSegmentHeight, 0.5 * fPrismRingSegmentHeight};
	G4double mainAnnulusRmin[2] = { fPrismRingSegmentRadiusInside, fPrismRingSegmentRadiusInside};
	G4double mainAnnulusRmax[2] = { fPrismRingSegmentRadiusOutside, fPrismRingSegmentRadiusOutside };


	// Now divide each ring (an n-gon prism) into n rectangular segments
	G4Polyhedra* segmentSolid = new G4Polyhedra("segment",
												-fPrismRingSegmentDPhi/2.0, // phi start
												fPrismRingSegmentDPhi, //total Phi
												1, //NPhi-gon
												2, RingZ,
												mainAnnulusRmin, mainAnnulusRmax);

	fSegmentLogic = new G4LogicalVolume(segmentSolid,
										pureWater,
										"segment",
										0, 0, 0);

    G4VPhysicalVolume* segmentPhysic = new G4PVReplica("segment",
													   fSegmentLogic, fPrismRingLogic, kPhi,
													   fGeoConfig->GetNSides(),
													   fPrismRingSegmentDPhi, 0.);

	if (!fDebugMode)
		fSegmentLogic->SetVisAttributes(G4VisAttributes::Invisible);
	else {
		G4VisAttributes* tmpVisAtt = new G4VisAttributes(
				G4Colour(1., 0.5, 0.5));
		tmpVisAtt->SetForceWireframe(true);
		fSegmentLogic->SetVisAttributes(tmpVisAtt);
	}

	//-------------------------------------------------------------
	// add barrel blacksheet to the normal barrel cells
	// ------------------------------------------------------------
	G4double segmentBlacksheetZ[2] = { -fPrismRingSegmentBSHeight/2.0,
                                        fPrismRingSegmentBSHeight/2.0 };
	G4double segmentBlacksheetRmax[2] = { fPrismRingSegmentBSRadiusOutside,
										  fPrismRingSegmentBSRadiusOutside };
	G4double segmentBlacksheetRmin[2] = { fPrismRingSegmentBSRadiusInside,
                                          fPrismRingSegmentBSRadiusInside,};

	G4Polyhedra* segmentBlacksheetSolid = new G4Polyhedra("segmentBlacksheet",
														  -0.5 * dPhi, // phi start
														  dPhi, //total phi
														  1, //NPhi-gon
														  2,
														  segmentBlacksheetZ,
														  segmentBlacksheetRmin,
														  segmentBlacksheetRmax);

	G4LogicalVolume * segmentBlacksheetLogic = new G4LogicalVolume( segmentBlacksheetSolid,
																	WCSimMaterialsBuilder::Instance()->GetMaterial("Blacksheet"),
																	"segmentBlacksheet",
																	0, 0, 0);

	G4VPhysicalVolume* segmentBlacksheetPhysic = new G4PVPlacement( 0,
																	G4ThreeVector(0., 0., 0.),
																	segmentBlacksheetLogic,
																	"segmentBlacksheet",
																	fSegmentLogic,
																	false, 0, true);

	G4LogicalBorderSurface * WaterBSBarrelCellSurface = NULL;
	// TODO: rip this out of WCSimConstructMaterials
	WaterBSBarrelCellSurface = new G4LogicalBorderSurface("WaterBSBarrelCellSurface",
														  segmentPhysic,
														  segmentBlacksheetPhysic,
														  OpWaterBSSurface);

	G4VisAttributes* WCBarrelBlacksheetCellVisAtt = new G4VisAttributes(
			G4Colour(0.2, 0.9, 0.2));
	if (fDebugMode )
		segmentBlacksheetLogic->SetVisAttributes(
				WCBarrelBlacksheetCellVisAtt);
	else
		segmentBlacksheetLogic->SetVisAttributes(
				G4VisAttributes::Invisible);

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

	G4double segmentWidth = 2. * (fPrismRingSegmentBSRadiusInside) * sin(fPrismRingSegmentDPhi / 2.);
	G4double widthPerCell = segmentWidth / fWallCellsX;
	G4double heightPerCell = GetBarrelLengthForCells() / fWallCellsZ;
  std::cout << "segment width = " << segmentWidth << std::endl;
  std::cout << "width per cell = " << widthPerCell << std::endl;
  std::cout << "height per cell " << heightPerCell << std::endl;
  std::cout << "wall cell size = " << fWallCellSize << std::endl;
  std::cout << "x cells" << fWallCellsX << std::endl;
  std::cout << "z cells" << fWallCellsZ << std::endl;
	G4TwoVector unitCellOffset = G4TwoVector(0.5 * (widthPerCell - fWallCellSize) ,
											                     0.5 * (heightPerCell - fWallCellSize) );
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

	for (G4double i = 0; i < fWallCellsX; i++) {

		// Note that these coordinates get rotated around
		G4double expandedX = fPrismRingSegmentBSRadiusInside;
    G4double expandedY = -0.5 * segmentWidth + i * widthPerCell;
		//G4double expandedY = -0.5 * widthPerCell * i + unitCellOffset.x();
		G4double expandedZ = -0.5 * heightPerCell;  // 0.5 * heightPerCell + unitCellOffset.y(); // Only one row by construction
    std::cout << "Expanded y = " << expandedY << "/" << 0.5*segmentWidth << std::endl;
		//G4double expandedZ = 0.5 * heightPerCell + unitCellOffset.y(); // Only one row by construction
		G4ThreeVector expandedCellPos = G4ThreeVector( expandedX, expandedY, expandedZ); // Bottom-left corner of region containing unit cell
    G4ThreeVector offsetToUnitCell = G4ThreeVector(0, 0.5*(widthPerCell - fWallCellSize), 0.5*(heightPerCell - fWallCellSize)); // Offset to the bottom left corner of the unit cell itself

		WCSimUnitCell * unitCell = GetBarrelUnitCell();
		for(unsigned int nPMT = 0; nPMT < unitCell->GetNumPMTs(); ++nPMT){
			G4TwoVector pmtCellPosition = unitCell->GetPMTPos(nPMT, fWallCellSize); // PMT position in cell, relative to top left of cell
      std::cout << std::endl << "Placing PMT " << nPMT << "in wall cell " << i << std::endl;
      std::cout << "PMT position in cell = " << pmtCellPosition.x() / m << "," << pmtCellPosition.y() / m << "in m" << std::endl;
      std::cout << "Cell size = " << fWallCellSize << std::endl;// pmtCellPosition.x() << "," << pmtCellPosition.y() << std::endl;
			G4ThreeVector PMTPosition = expandedCellPos + offsetToUnitCell   // bottom left of unit cell
										              + G4ThreeVector(0, 0, fWallCellSize) // bottom left to top left of cell
                                  + G4ThreeVector(0, pmtCellPosition.x(), -1.0*pmtCellPosition.y()); // top left of cell to PMT
      std::cout << "Position = " << PMTPosition << "   rotation = " << WCPMTRotation << std::endl;
      std::cout << "Cell mother height and width: " << heightPerCell/2. << "  "  << segmentWidth/2. << std::endl;
      WCSimPMTConfig config = unitCell->GetPMTPlacement(nPMT).GetPMTConfig();
      std::cout << " PMT logical volume name = " << fPMTBuilder.GetPMTLogicalVolume(config)->GetName() << std::endl;
			G4VPhysicalVolume* physiWCBarrelPMT = new G4PVPlacement(WCPMTRotation,     // its rotation
																	PMTPosition,
																	fPMTBuilder.GetPMTLogicalVolume(config),        // its logical volume // TODO: GET THIS SOMEHOW/
																	"WCPMT",           // its name
																	fSegmentLogic,      // its mother volume
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

void WCSimCherenkovBuilder::SetGeoConfig(WCSimGeoConfig* config) {
}

WCSimGeoConfig * WCSimCherenkovBuilder::GetGeoConfig() const {
	return fGeoConfig;
}

void WCSimCherenkovBuilder::GetMeasurements()
{
    if( fGotMeasurements) { return; }

    // Barrel measurements
    fBarrelRadius = fGeoConfig->GetOuterRadius() + 1*m; // Make it 1m bigger in radius
    fBarrelHeight = fGeoConfig->GetInnerHeight() + 2* fBlacksheetThickness + 2 * GetMaxTopExposeHeight() + 2*m;  // and half-height
    fBarrelLengthForCells = fGeoConfig->GetInnerHeight() - 2.0 * GetMaxTopExposeHeight();

    fPrismRadiusInside = fGeoConfig->GetInnerRadius();
    fPrismRadiusOutside = fGeoConfig->GetInnerRadius() + fBlacksheetThickness + GetMaxBarrelExposeHeight();
    fPrismHeight = fBarrelLengthForCells;

    fPrismRingRadiusInside = fPrismRadiusInside;
    fPrismRingRadiusOutside = fPrismRadiusOutside;

    fPrismRingSegmentRadiusInside = fPrismRadiusInside;
    fPrismRingSegmentRadiusOutside = fPrismRadiusOutside;
    fPrismRingSegmentDPhi = 360*deg / fGeoConfig->GetNSides();

    fPrismRingSegmentBSRadiusInside = fPrismRadiusOutside - fBlacksheetThickness;
    fPrismRingSegmentBSRadiusOutside = fPrismRadiusOutside;
    
    // Now optimize the cell sizes to work out the prism ring heights:
    CalculateCellSizes();
    
    fPrismRingHeight = fPrismHeight / fWallCellsZ;
    fPrismRingSegmentHeight = fPrismRingHeight;
    fPrismRingSegmentBSHeight = fPrismRingHeight;

    // Cap measurements
	  fCapAssemblyHeight = fBlacksheetThickness + GetMaxTopExposeHeight() + 0.5 * (fGeoConfig->GetInnerHeight() - fBarrelLengthForCells);
    // nb. fCapAssemblyHeight = fCapRingHeight + fCapPolygonHeight
	  fCapAssemblyRadius = fBarrelRadius;

	  fCapRingRadiusInside = fPrismRadiusInside;
	  fCapRingRadiusOutside = fPrismRadiusOutside;
	  fCapRingHeight = 0.5 * (fGeoConfig->GetInnerHeight() - fBarrelLengthForCells);

	  fCapRingSegmentDPhi = fPrismRingSegmentDPhi;
	  fCapRingSegmentRadiusInside = fPrismRadiusInside;
	  fCapRingSegmentRadiusOutside = fPrismRadiusOutside;
	  fCapRingSegmentHeight = fCapRingHeight;

	  fCapRingSegmentBSRadiusInside = fPrismRingSegmentBSRadiusInside;
	  fCapRingSegmentBSRadiusOutside = fPrismRingSegmentBSRadiusOutside;
	  fCapRingSegmentBSHeight = fCapRingHeight;

	  fCapPolygonRadius = fGeoConfig->GetInnerRadius() + GetMaxBarrelExposeHeight() + fBlacksheetThickness;
	  fCapPolygonHeight = GetMaxTopExposeHeight() + fBlacksheetThickness;

    fCapPolygonCentreRadius = fGeoConfig->GetInnerRadius() + GetMaxBarrelExposeHeight();
    fCapPolygonCentreHeight = GetMaxTopExposeHeight();

	  fCapPolygonEdgeBSRadiusInside = fGeoConfig->GetInnerRadius() + GetMaxBarrelExposeHeight();
	  fCapPolygonEdgeBSRadiusOutside = fCapPolygonRadius;
	  fCapPolygonEdgeBSHeight = fCapPolygonHeight;

	  fCapPolygonEndBSRadius = fGeoConfig->GetInnerRadius() + GetMaxBarrelExposeHeight();
	  fCapPolygonEndBSHeight = fBlacksheetThickness;

    fGotMeasurements = true;
}

double WCSimCherenkovBuilder::GetBarrelLengthForCells() {
	return fGeoConfig->GetInnerHeight() - 2.0 * GetMaxTopExposeHeight();
}

double WCSimCherenkovBuilder::GetMaxTopExposeHeight() {
	WCSimUnitCell * cell = GetTopUnitCell();
	return cell->GetCellExposeHeight();
}

double WCSimCherenkovBuilder::GetMaxBarrelExposeHeight() {
	WCSimUnitCell * cell = GetBarrelUnitCell();
	return cell->GetCellExposeHeight();
}

void WCSimCherenkovBuilder::ConstructUnitCells() {
  fGeoConfig->Print();
  std::vector<double> pmtX     = fGeoConfig->GetCellPMTX();
  std::vector<double> pmtY     = fGeoConfig->GetCellPMTY();
  std::vector<std::string> pmtNames = fGeoConfig->GetCellPMTName();
  assert( pmtX.size() == pmtY.size() && pmtX.size() == pmtNames.size() );

  WCSimUnitCell * cell = new WCSimUnitCell();
  for(unsigned int i = 0; i < pmtNames.size(); ++i){
    std::cout << "PMT = " << pmtNames.at(i) << "  X = " << pmtX.at(i) << " / m = " << pmtX.at(i) / m << std::endl;
		WCSimPMTConfig config = fPMTManager->GetPMTByName(pmtNames.at(i));
    cell->AddPMT(config, pmtX.at(i), pmtY.at(i));
	}
  cell->Print();
	fUnitCells.push_back(cell);
	assert(fUnitCells.size() != 0);
	return;
}

WCSimUnitCell* WCSimCherenkovBuilder::GetTopUnitCell() {
	if( fUnitCells.size() == 0 ){
		this->ConstructUnitCells();
	}

	return fUnitCells.at(0);
}

void WCSimCherenkovBuilder::CalculateCellSizes() {
	fWallCellSize = GetOptimalWallCellSize();
	fTopCellSize = GetOptimalTopCellSize();
  assert(fTopCellSize > 0);
}

double WCSimCherenkovBuilder::GetOptimalTopCellSize() {
	// Optimizing how many squares you can fit in a polygon
	// is in general non-trivial.  Here we'll lay them out in a regular
	// grid and then iterate making the cells slightly smaller or larger
	// to get close to the desired coverage.
  std::cout << " *** WCSimCherenkovBuilder::GetOptimalTopCellSide *** " << std::endl;

	// Fit our n-gon inside a square:
	double squareSide = 2.0*(fCapPolygonCentreRadius);


	WCSimUnitCell topCell = *(GetTopUnitCell());
	double defaultSide = topCell.GetCellSizeForCoverage(fGeoConfig->GetCoverageFraction());
  std::cout << "Default side = " << defaultSide << " Coverage = " << topCell.GetPhotocathodeCoverage(defaultSide) << std::endl;

	int cellRows = (int)(squareSide/defaultSide); // Round down
  std::cout << "cellRows = " << cellRows << std::endl;

/*	TODO: implement this feature
    G4TwoVector centreOffset;
	if( (cellRows % 2) != 0){ // An odd number of cells
		centreOffset = G4TwoVector(0.5 * defaultSide, 0.5 * defaultSide)
		// Make sure the middle of the square contains the middle
		// of a unit cell
	}
	else{ // An even number
		centreOffset = G4TwoVector(0.,0.);
		// Make sure the middle of the square contains a gap
		// where four unit cells meet
	}
*/

	// Now construct a grid containing the corners of each square cell
	// We have a regular polygon containing squares - both are convex
	// so it's sufficient to test if each corner of a square is inside
	// the polygon.
	double cellSide = squareSide / cellRows;
	unsigned int nSides = fGeoConfig->GetNSides();

	double bestSide = cellSide;
	double bestCoverage = 0;

	for( int iteration = 0; iteration < 10; ++iteration ){

		// Use this to translate between coordinate systems where (0,0) is the middle of
		// the square enclosing the polygon, and where it is the top left of that square
		G4TwoVector centreToTopLeft = G4TwoVector(-0.5*squareSide, 0.5 * squareSide);
		unsigned int cellsInPolygon = 0;
    // std::cout << "Iteration... " << iteration << std::endl;
		for( int iHoriz = 0; iHoriz < cellRows; ++iHoriz ){
			for( int iVert = 0; iVert < cellRows; ++iVert ){
				// Unit cell corners.  Use coordinates where (0,0) is the centre of the square enclosing our polygon
				// because that's what WCSimPolygonTools expects
				G4TwoVector topLeft     = G4TwoVector(iHoriz * cellSide,     -iVert * cellSide) + centreToTopLeft;
				G4TwoVector topRight    = G4TwoVector((iHoriz+1) * cellSide, -iVert * cellSide) + centreToTopLeft;
				G4TwoVector bottomLeft  = G4TwoVector(iHoriz * cellSide,     -(iVert+1) * cellSide) + centreToTopLeft;
				G4TwoVector bottomRight = G4TwoVector((iHoriz+1) * cellSide, -(iVert+1) * cellSide) + centreToTopLeft;

				bool cellInPolygon = true;
        // std::cout << "iHoriz = " << iHoriz << "  " << "iVert = " << iVert << std::endl
        //           << "(" << topLeft.x() << ", " << topLeft.y() << ") (" << topRight.x() << ", " << topRight.y()
        //           << ") (" << bottomLeft.x() << ", " << bottomLeft.y() << ") (" << bottomRight.x() << bottomRight.y() << ")" << std::endl;
				if(    !(WCSimPolygonTools::PolygonContains(nSides, fCapPolygonCentreRadius, topLeft))
					|| !(WCSimPolygonTools::PolygonContains(nSides,   fCapPolygonCentreRadius, topRight))
					|| !(WCSimPolygonTools::PolygonContains(nSides,   fCapPolygonCentreRadius, bottomLeft))
					|| !(WCSimPolygonTools::PolygonContains(nSides,   fCapPolygonCentreRadius, bottomRight)) ){
					cellInPolygon = false;
				}
				cellsInPolygon += cellInPolygon;
			}
		}
		double topCoverage =   (cellsInPolygon * topCell.GetPhotocathodeCoverage(cellSide) * cellSide * cellSide)
							 / (WCSimPolygonTools::GetAreaFromRadius(nSides, fGeoConfig->GetOuterRadius()));
		// std::cout << "Coverage = " << (cellsInPolygon * topCell.GetPhotocathodeCoverage(cellSide) * cellSide * cellSide)
		// 					<< " / " << (WCSimPolygonTools::GetAreaFromRadius(nSides, fGeoConfig->GetOuterRadius()))
    //           << " = " << topCoverage << std::endl;
    // std::cout << "Cells in polygon = " << cellsInPolygon << std::endl;
    // std::cout << "cellSide = " << cellSide << std::endl;
    // std::cout << "innerRadius = " << fGeoConfig->GetOuterRadius() << std::endl;

		if( fabs(topCoverage-fGeoConfig->GetCoverageFraction()) < bestCoverage ){
			bestCoverage = topCoverage;
			bestSide = cellSide;
		}

		// Scale the cell by the difference and iterate again
    // std::cout << "topCoverage = " << topCoverage << std::endl;
		if(topCoverage > 0){
      // std::cout << "cellSide was " << cellSide << std::endl;
      cellSide = cellSide * sqrt(topCoverage / fGeoConfig->GetCoverageFraction() );
      // std::cout << "now cellSide is " << cellSide << std::endl;
    }
	}
  // std::cout << "Best top size = " << bestSide << std::endl;
	return bestSide;
}

double WCSimCherenkovBuilder::GetOptimalWallCellSize() {
	double coverage 		= fGeoConfig->GetCoverageFraction(); // Photocathode coverage
	double wallHeight 		= GetBarrelLengthForCells(); // Height of walls
	double wallSide 		= 2.0 * fPrismRingSegmentBSRadiusInside * sin(M_PI / fGeoConfig->GetNSides()); // Side length of walls
	double wallCentreRadius = fPrismRingSegmentBSRadiusInside;; // Distance from centre of detector to centre of wall
	WCSimUnitCell *wallCell  = (GetBarrelUnitCell()); // Make a copy because we'll mess with it
  
  std::cout << "Wall height = " << wallHeight << " ( * m = " << wallHeight * m << ")" << std::endl;
  std::cout << "Wall side = " << wallSide << std::endl;
  wallCell->Print();
  std::cout << "Minimum side length allowed = " << wallCell->GetMinimumCellSize() << std::endl;

	// Number of unit cells spanning across the width of the wall
	int tileX = (int)(wallSide / (this->GetBarrelUnitCell()->GetCellSizeForCoverage(coverage)));
	// Number of unit cells to span the height of the detector
	int tileZ = (int)(wallHeight / (this->GetBarrelUnitCell()->GetCellSizeForCoverage(coverage)));

	// In general the number of cells needed is non-integral
	// We need an m x n arrangement of cells that gives coverage close
	// to the desired percentage.  So round the number of cells up and down
	// and see which arrangement gets closest
	int xNums[2] = { tileX, tileX+1 };
	int zNums[2] = { tileZ, tileZ+1 };

	double bestCoverage = 0.0;
	double minCoverageDiff = coverage;
	double bestLength = 0.0;

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
      std::cout << "Test: wall coverage is " << newCoverage << std::endl;
      std::cout << "Difference from ideal = " << coverageDiff << std::endl;
      std::cout << "Side = " << length << std::endl;
      std::cout << "Are there overlaps? Computer says... " << wallCell->ContainsOverlaps(length) << std::endl;
      std::cout << "Minimum side length allowed = " << wallCell->GetMinimumCellSize() << std::endl;

			if( (!wallCell->ContainsOverlaps(length)) && coverageDiff < minCoverageDiff){
				minCoverageDiff = coverageDiff;
				bestCoverage    = newCoverage;
				bestLength		= length;

				fWallCellLength = bestLength;
				fWallCellsX 	= xNums[xIndex];
				fWallCellsZ 	= zNums[zIndex];
			}
		}
	}
	std::cout << "Best wall coverage is " << bestCoverage << " (c.f. " << coverage << ")" << std::endl;

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
	// TODO: Set these variables once in a single function, then get them from there
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

	G4LogicalVolume * capLogic = NULL;
	if( zflip == -1 ) { capLogic = fCapLogicTop; }
	else { capLogic = fCapLogicBottom; }
    G4cout << "G4cout capLogic" << capLogic << std::endl;

	G4Material * pureWater = WCSimMaterialsBuilder::Instance()->GetMaterial("Water");
	G4Material * blacksheet = WCSimMaterialsBuilder::Instance()->GetMaterial("Blacksheet");



	G4double borderRingZ[2] = {-0.5 * fCapRingHeight * zflip, 0.5 * fCapRingHeight * zflip };
	G4double borderRingRMin[2] = { fCapRingRadiusInside, fCapRingRadiusInside};
	G4double borderRingRMax[2] = { fCapRingRadiusOutside, fCapRingRadiusOutside};


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

	  if( zflip == -1 ){ fCapLogicTopRing = logicEndCapRing; }
	  else{ fCapLogicBottomRing = logicEndCapRing; }

	  std::cout << " Physics cap volume lives at " << (capAssemblyHeight/2.- GetBarrelLengthForCells()/2.)*zflip << std::endl;
 						std::cout <<  "Place phsyiEndCapRing at "
 								  << ((capAssemblyHeight/2.- (fGeoConfig->GetInnerHeight() - GetBarrelLengthForCells())/2.)*zflip)
 								  << " - height is " << borderRingZ[2] - borderRingZ[0]
 								  << " c.f. capAssemblyHeight = " << capAssemblyHeight << std::endl;
 	  G4VPhysicalVolume* physiEndCapRing =
       new G4PVPlacement(0,
 						G4ThreeVector(0.,0., (0.5 * fCapAssemblyHeight - 0.5 * fCapRingHeight) * zflip),
 						logicEndCapRing,
 						"EndCapRing",
 						capLogic,
 						false, 0,true);
}

void WCSimCherenkovBuilder::ConstructEndCapRingSegments( G4int zflip )
{
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
	// TODO: rip this out of WCSimConstructMaterials
	WaterBSCapCellSurface = new G4LogicalBorderSurface("WaterBSCapCellSurface",
														capSegmentPhysic,
														capSegmentBlacksheetPhysic,
														OpWaterBSSurface);

	G4VisAttributes* WCCellBlacksheetCellVisAtt = new G4VisAttributes(G4Colour(0.2, 0.9, 0.2));
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
    G4VPhysicalVolume* physiCapPolygonCentre = new G4PVPlacement( 0,
                                                                 G4ThreeVector(0.,0., (-0.5 * fCapPolygonHeight + 0.5 * fCapPolygonCentreHeight) * zflip),
                                                                 logicCapPolygonCentre,
                                                                 "CapPolygonCentre",
                                                                 logicCapPolygon,
                                                                 false, 0, true );
    std::cout << *solidCapPolygonCentre << std::endl;

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
	  G4VPhysicalVolume* physiWCCapEndBlacksheet = new G4PVPlacement(0,
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
	                                   	   	   	   	   	    OpWaterBSSurface);

	  G4LogicalBorderSurface * WaterBSBottomCapEndSurface = NULL;
	  WaterBSBottomCapEndSurface = new G4LogicalBorderSurface("WaterBSCapPolySurface",
	                                   	   	   	   	   	    physiCapPolygon,physiWCCapEdgeBlacksheet,
	                                   	   	   	   	   	    OpWaterBSSurface);

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
	  if( zflip == -1 ) { capLogic = fCapLogicTop; }
	  else { capLogic = fCapLogicBottom; }
    G4cout << "G4cout capLogic again" << capLogic << std::endl;
    G4cout << "capLogic top = " << fCapLogicTop << "  and bottom ... " << fCapLogicBottom << std::endl;

	  //---------------------------------------------------------
	  // Add top and bottom PMTs
	  // -----------------------------------------------------
    std::cout << " *** PlaceEndCapPMTs ***    zflip = " << zflip << std::endl;

	  G4RotationMatrix* WCCapPMTRotation = new G4RotationMatrix;
	  if(zflip==1){
	    WCCapPMTRotation->rotateY(180.*deg);
	  }

	  // loop over the cap
	  // Build a square that contains the cap:
	  G4double squareEdge = 2.0*(fCapPolygonCentreRadius);
    squareEdge = squareEdge + fmod(squareEdge, 2*fTopCellSize);

	  G4double xpos = -0.5 * squareEdge;
	  G4double ypos = -0.5 * squareEdge;
    std::cout << "Starting loop!" << std::endl;
    std::cout << "The unit cell on the top has " << GetTopUnitCell()->GetNumPMTs() << " PMTs in it"  << std::endl;

	  while( fabs(ypos) <= 0.5 * squareEdge  ){
      xpos = -0.5 * squareEdge;
		  while(fabs(xpos) <= 0.5 * squareEdge){
			  G4ThreeVector cellpos = G4ThreeVector(xpos, ypos, 0.0); // Coordinates of the bottom left corner of the unit cell


			  WCSimUnitCell * unitCell = GetTopUnitCell();
			  if( WCSimPolygonTools::PolygonContainsSquare(fGeoConfig->GetNSides(), fCapPolygonCentreRadius, G4TwoVector(cellpos.x(), cellpos.y()), fTopCellSize)){

				  for(unsigned int nPMT = 0; nPMT < unitCell->GetNumPMTs(); ++nPMT){
					  G4TwoVector pmtCellPosition = unitCell->GetPMTPos(nPMT, fTopCellSize); // PMT position in cell, relative to top left of cell
					  G4ThreeVector PMTPosition = cellpos   // bottom left of unit cell
							  	  	  	  	  	      + G4ThreeVector(pmtCellPosition.x(),
							  	  	  	  	  			          	  	  fTopCellSize - pmtCellPosition.y(),
							  	  	  	  	  			  	  	          0.0); // top left of cell to PMT
					  WCSimPMTConfig config = unitCell->GetPMTPlacement(nPMT).GetPMTConfig();
					  G4VPhysicalVolume* physiCapPMT = new G4PVPlacement(WCCapPMTRotation,     // its rotation
							  	  	  	  	  	  	  	  	  	  	  	 PMTPosition,
							  	  	  	  	  	  	  	  	  	  	  	 fPMTBuilder.GetPMTLogicalVolume(config),        // its logical volume
							  	  	  	  	  	  	  	  	  	  	  	 "WCPMT",           // its name
							  	  	  	  	  	  	  	  	  	  	  	 capLogic,      // its mother volume
							  	  	  	  	  	  	  	  	  	  	  	 false,             // no boolean operations
							  	  	  	  	  	  	  	  	  	  	  	 fNumPMTs);
            //std::cout << "name of capLogic = " << capLogic->GetName() << std::endl;
					  if( fNumPMTs == 0){
               //std::cout << "inner radius = " << fGeoConfig->GetOuterRadius() << "   xpos = " << xpos << "   ypos = " << ypos << "  top cell size = " << fTopCellSize << "   wall cell size = " << fWallCellSize << std::endl;
               //std::cout << "PMT position in cell = " << pmtCellPosition.x() / m << "," << pmtCellPosition.y() / m << "in m" << std::endl;
					     //std::cout << "Cell size = " << fTopCellSize << std::endl;// pmtCellPosition.x() << "," << pmtCellPosition.y() << std::endl;
					     //std::cout << "PMT logical volume name = " << fPMTBuilder.GetPMTLogicalVolume(config)->GetName() << std::endl;
					     //std::cout << "Placed endcap PMT!" << std::endl;
            }
					  fNumPMTs++;
				// logicWCPMT->GetDaughter(0),physiCapPMT is the glass face. If you add more
				// daughter volumes to the PMTs (e.g. a acryl cover) you have to check, if
				// this is still the case.
				  }
			  }
        else
        {
        }
			  xpos += fTopCellSize;
		  }
		  ypos += fTopCellSize;
	  }

}

void WCSimCherenkovBuilder::ConstructEndCapPhysicalVolumes(){
  // std::cout << "mainAnnulusHeight = " << mainAnnulusHeight << " (in m = " << mainAnnulusHeight/m << ")" << std::endl;
  // std::cout << "capAssemblyHeight = " << capAssemblyHeight << " (in m = " << capAssemblyHeight/m << ")" << std::endl;
  // std::cout << "InnerHeight=  " << fGeoConfig->GetInnerHeight() << " (in m = " << fGeoConfig->GetInnerHeight()/m << ")" << std::endl;
  // std::cout << "BarrelLengthForCells = " << GetBarrelLengthForCells() << " (in m = " << GetBarrelLengthForCells()/m << ")" << std::endl;
  // std::cout << "fBlacksheet thickness = " << fBlacksheetThickness << " (in m = " << fBlacksheetThickness/m << ")" << std::endl;
  G4VPhysicalVolume* physiTopCapAssembly = new G4PVPlacement(0,
                                                             G4ThreeVector(0.,0.,0.5 * (fGeoConfig->GetInnerHeight() + fCapAssemblyHeight)),
                                                             fCapLogicTop,
                                                             "TopCapAssembly",
                                                             fBarrelLogic,
                                                             false, 0,true);
  G4VPhysicalVolume* physiBottomCapAssembly = new G4PVPlacement(0,
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

WCSimUnitCell* WCSimCherenkovBuilder::GetBarrelUnitCell() {
	if( fUnitCells.size() == 0 ){ this->ConstructUnitCells(); }
	return fUnitCells.at(0);
}

G4LogicalVolume * WCSimCherenkovBuilder::ConstructWC()
{
	std::cout << " *** In WCSimCherenkovBuilder::ConstructWC() *** " << std::endl;
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
	 / cos(dPhi / 2.); // TODO: OD

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
	for(unsigned int iCell = 0; iCell < fUnitCells.size(); ++iCell)
	{
		delete fUnitCells[iCell];
	}
	fUnitCells.clear();

  if( fGeoConfig != NULL ) { delete fGeoConfig; }
  WCSimGeoManager * manager = new WCSimGeoManager();
  fGeoConfig = new WCSimGeoConfig(manager->GetGeometryByName(fDetectorName));
  delete manager;
  std::cout << "Update geometry to " << fDetectorName << std::endl;

	fPMTBuilder.Reset();
	delete fCapLogicBottom;
  delete fCapLogicTop;
  delete fSegmentLogic;
  delete fPrismRingLogic;
  delete fPrismLogic;
  delete fBarrelLogic;
  delete fLakeLogic;
	fLakeLogic = NULL;
	fBarrelLogic = NULL;
	fPrismLogic = NULL;
	fPrismRingLogic = NULL;
	fSegmentLogic = NULL;
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
	fPrismRingHeight = -999 * m;

	fPrismRingSegmentRadiusInside = -999 * m; // To centre of segment not edge
	fPrismRingSegmentRadiusInside = -999 * m;
	fPrismRingSegmentHeight = -999 * m;
	fPrismRingSegmentDPhi = -999 * m;

	fPrismRingSegmentBSRadiusInside = -999 * m; // To centre not edge
	fPrismRingSegmentBSRadiusOutside = -999 * m;
	fPrismRingSegmentBSHeight = -999 * m;

	WCSimDetectorConstruction::Update();
}

void WCSimCherenkovBuilder::ConstructPMTs()
{
	std::cout << " *** In WCSimCherenkovBuilder::ConstructPMTs *** " << std::endl;
    std::cout << "SIZE OF CONFIGS VECTOR = " << fPMTConfigs.size() << std::endl;
	fPMTBuilder.ConstructPMTs( fPMTConfigs );
}
