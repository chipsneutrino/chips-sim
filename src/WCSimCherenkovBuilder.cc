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

WCSimCherenkovBuilder::WCSimCherenkovBuilder(G4int DetConfig) :
		fConstructed(false), fGeoConfig(NULL), WCSimDetectorConstruction(DetConfig) {
	fBlacksheetThickness = 2 * mm;
	fDebugMode 			 = false;

  WCSimGeoManager * manager = new WCSimGeoManager();
  fPMTManager = new WCSimPMTManager();
  temp = manager->GetGeometryByName("CHIPS_25kton_10inch_HQE_10perCent");
  fGeoConfig = &temp;

}

WCSimCherenkovBuilder::~WCSimCherenkovBuilder() {
	for(unsigned int iCell = 0; iCell < fUnitCells.size(); ++iCell)
	{
		delete fUnitCells[iCell];
	}
	fUnitCells.clear();

}

G4LogicalVolume * WCSimCherenkovBuilder::ConstructDetector() {
	if (!fConstructed) {
		CalculateCellSizes();
		ConstructUnitCells();
		ConstructEnvironment();
		ConstructFrame();
		ConstructVeto();
		ConstructInnerDetector();
		//ConstructEndCaps();
		ConstructPMTs();
		PlacePMTs();
		CreateSensitiveDetector();
	}
	fConstructed = true;
	return fLakeLogic;
}

void WCSimCherenkovBuilder::ConstructEnvironment() {
  SetPositions();

	// The water barrel is placed in a cylinder tub of water, representing the lake
	G4double innerRadius = fGeoConfig->GetInnerRadius();
	G4double innerHeight= fGeoConfig->GetInnerHeight();
	G4double shoreDistance = 20; // Distance from detector to shore (m)

	// Define our world: a cylindrical lake in which that detector will sit
	// Constants used to configure it:
	G4double lakeIR     	   = 0.0; // inner radius of lake
	G4double lakeOR     	   = innerRadius + shoreDistance*m; // outer radius of lake
	G4double lakeHeight 	   = innerHeight + 20. * m;
	G4Material * lakeWater 	   = G4Material::GetMaterial("PitWater");  // Fill it with purified water
	G4VisAttributes* lakeColor = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0));

	// The geometry objects themselves:
	G4Tubs* lakeTubs = new G4Tubs("lake", lakeIR, lakeOR, lakeHeight,
								  0.0 * deg, 360.0 * deg);

	fLakeLogic = new G4LogicalVolume(lakeTubs, lakeWater, "lake", 0, 0, 0);
	fLakeLogic->SetVisAttributes(lakeColor);
	fLakeLogic->SetVisAttributes(G4VisAttributes::Invisible);



}

void WCSimCherenkovBuilder::ConstructFrame() {

	// Make a large cylinder to hold the PMTs and water
	G4double innerRadius     = fGeoConfig->GetInnerRadius();
	G4double innerHeight     = fGeoConfig->GetInnerHeight();

	G4double barrelIR 	     = 0.0 * m;
	G4double barrelOR 	     = innerRadius + 1.0 * m; // Allow an extra metre of space
	G4double barrelHeight    = 0.5 * innerHeight;
	G4Material * barrelWater = G4Material::GetMaterial("Water");  // Fill it with purified water

	G4Tubs* barrelTubs = new G4Tubs("barrelTubs", barrelIR, barrelOR, barrelHeight,
									0. * deg, 360. * deg);

	fBarrelLogic = new G4LogicalVolume(barrelTubs, barrelWater,
													   "barrelTubs", 0, 0, 0);


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

	G4double WCIDRadius = fGeoConfig->GetInnerRadius();
	G4double WCIDHeight = fGeoConfig->GetInnerHeight();
	G4double WCBarrelRingNPhi = fGeoConfig->GetNSides();

	// the part of one ring, that is covered by the regular cells:
	G4double totalAngle = 2.0 * pi * rad;

	// angle per regular cell:
	G4double dPhi = totalAngle / WCBarrelRingNPhi;

	//-----------------------------------------------------
	// Form annular section of barrel to hold PMTs
	//----------------------------------------------------

	G4double innerAnnulusRadius = fGeoConfig->GetInnerRadius()
								  - GetMaxBarrelExposeHeight() - 1. * mm;
	G4double outerAnnulusRadius = fGeoConfig->GetInnerRadius()
								  + fBlacksheetThickness + 1. * mm;
	// + structure of the frame etc.
	// The radii are measured to the center of the surfaces
	// (tangent distance). Thus distances between the corner and the center are bigger.

	// Zip them up into arrays to pass to Geant
	G4double mainAnnulusZ[2] = { -0.5 * GetBarrelLengthForCells(),
								  0.5 * GetBarrelLengthForCells() };
	G4double mainAnnulusRmin[2] = { innerAnnulusRadius, innerAnnulusRadius };
	G4double mainAnnulusRmax[2] = { outerAnnulusRadius, outerAnnulusRadius };



	// Now make the volumes
	G4Polyhedra* prismSolid = new G4Polyhedra("prism",
											  0. * deg,  // phi start
											  360.0 * deg, // phi end
											  (G4int)fGeoConfig->GetNSides(), //number of sides
											  2, mainAnnulusZ, // number and location of z planes
											  mainAnnulusRmin, mainAnnulusRmax); // inner and outer radii

	fPrismLogic = new G4LogicalVolume(
											  prismSolid, G4Material::GetMaterial("Water"),
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
	G4double innerAnnulusRadius = fGeoConfig->GetInnerRadius()
								  - (GetMaxBarrelExposeHeight() / cos(M_PI/fGeoConfig->GetNSides())) - 1. * mm;
	G4double outerAnnulusRadius = fGeoConfig->GetInnerRadius()
								  + fBlacksheetThickness + 1. * mm;

	double ringHeight 			= GetBarrelLengthForCells() / fWallCellsZ;
	G4Material * pureWater  = G4Material::GetMaterial("Water");

	// Zip them up into arrays to pass to Geant
  std::cout << "Ring height = " << ringHeight << std::endl;
  std::cout << "Barrel lenght = " << GetBarrelLengthForCells() << std::endl;
	std::cout << "Inner radius = " << innerAnnulusRadius << std::endl;
	std::cout << "Outer radius = " << outerAnnulusRadius << std::endl;
  G4double RingZ[2] 			= { -0.5 * ringHeight, 0.5 * ringHeight };
	G4double mainAnnulusZ[2] = { -0.5 * GetBarrelLengthForCells(),
								  0.5 * GetBarrelLengthForCells() };
	G4double mainAnnulusRmin[2] = { innerAnnulusRadius, innerAnnulusRadius };
	G4double mainAnnulusRmax[2] = { outerAnnulusRadius, outerAnnulusRadius };

	// Now make all the volumes
  std::cout << "Making solid" << fGeoConfig->GetNSides() << std::endl;
	G4Polyhedra* prismRingSolid = new G4Polyhedra("prismRing", 0. * deg,// phi start
													 360.0 * deg, //phi end
													 (G4int) fGeoConfig->GetNSides(), //NPhi-gon
													 2, RingZ, mainAnnulusRmin, mainAnnulusRmax);

  std::cout << "Making logic" << std::endl;
	fPrismRingLogic = new G4LogicalVolume(prismRingSolid,
														  pureWater, "prismRing",
														  0, 0, 0);

  std::cout << "Making physic" << std::endl;
	G4VPhysicalVolume* prismRingPhysic = new G4PVReplica("prismRing",
														 fPrismRingLogic, fPrismLogic, kZAxis,
														 (G4int) fWallCellsZ, ringHeight);
  std::cout << "What-what?!" << std::endl;
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
	double dPhi = (360.0 / fGeoConfig->GetNSides() ) * deg;
	G4double innerAnnulusRadius = fGeoConfig->GetInnerRadius()
								  - GetMaxBarrelExposeHeight() - 1. * mm;
	G4double outerAnnulusRadius = fGeoConfig->GetInnerRadius()
								  + fBlacksheetThickness + 1. * mm;
  std::cout << "Inner annulus radius = " << innerAnnulusRadius << std::endl
            << "Outer annulus radius = " << outerAnnulusRadius << std::endl
            << "dPhi = " << dPhi << std::endl;


	double ringHeight 			= GetBarrelLengthForCells() / fWallCellsZ;
	G4Material * pureWater = G4Material::GetMaterial("Water");

	// Zip them up into arrays to pass to Geant
	G4double RingZ[2] 			= { -0.5 * ringHeight, 0.5 * ringHeight };
	G4double mainAnnulusZ[2] = { -0.5 * GetBarrelLengthForCells(),
								  0.5 * GetBarrelLengthForCells() };
	G4double mainAnnulusRmin[2] = { innerAnnulusRadius, innerAnnulusRadius };
	G4double mainAnnulusRmax[2] = { outerAnnulusRadius, outerAnnulusRadius };


	// Now divide each ring (an n-gon prism) into n rectangular segments
	G4Polyhedra* segmentSolid = new G4Polyhedra("segment",
												-dPhi / 2. + 0. * deg, // phi start
												dPhi, //total Phi
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
													   dPhi, 0.);

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
	G4double annulusBlacksheetRmax[2] = { (fGeoConfig->GetInnerRadius() + fBlacksheetThickness),
										   fGeoConfig->GetInnerRadius() + fBlacksheetThickness };
	G4double annulusBlacksheetRmin[2] = { fGeoConfig->GetInnerRadius(), fGeoConfig->GetInnerRadius() };

	G4Polyhedra* segmentBlacksheetSolid = new G4Polyhedra("segmentBlacksheet",
														  -0.5 * dPhi, // phi start
														  dPhi, //total phi
														  1, //NPhi-gon
														  2, RingZ,
														  annulusBlacksheetRmin,
														  annulusBlacksheetRmax);

	G4LogicalVolume * segmentBlacksheetLogic = new G4LogicalVolume( segmentBlacksheetSolid,
																	G4Material::GetMaterial("Blacksheet"),
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
	if (fDebugMode)
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

	G4double dPhi = 2 * pi * rad / fGeoConfig->GetNSides();
	G4double segmentWidth = 2. * fGeoConfig->GetInnerRadius() * sin(dPhi / 2.);
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
		G4double expandedX = fGeoConfig->GetInnerRadius();
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
			G4VPhysicalVolume* physiWCBarrelPMT = new G4PVPlacement(WCPMTRotation,     // its rotation
																	PMTPosition,
																	fPMTBuilder.GetPMTLogicalVolume(config),        // its logical volume // TODO: GET THIS SOMEHOW/
																	"WCPMT",           // its name
																	fSegmentLogic,      // its mother volume
																	false,             // no boolean operations
																	(int) (i*unitCell->GetNumPMTs() + nPMT), true);

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
  std::cout << "optimising the top cell" << std::endl;

	// Fit our n-gon inside a square:
	double squareSide = fGeoConfig->GetInnerRadius();


	WCSimUnitCell topCell = *(GetTopUnitCell());
	double defaultSide = topCell.GetCellSizeForCoverage(fGeoConfig->GetCoverageFraction());

	unsigned int cellRows = (unsigned int)(squareSide/defaultSide); // Round down

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

	for( unsigned int iteration = 0; iteration < 5; ++iteration ){

		// Use this to translate between coordinate systems where (0,0) is the middle of
		// the square enclosing the polygon, and where it is the top left of that square
		G4TwoVector centreToTopLeft = G4TwoVector(-0.5*squareSide, 0.5 * squareSide);
		unsigned int cellsInPolygon = 0;
		for( unsigned int iHoriz = 0; iHoriz < cellRows; ++iHoriz ){
			for( unsigned int iVert = 0; iVert < cellRows; ++iVert ){
				// Unit cell corners.  Use coordinates where (0,0) is the centre of the square enclosing our polygon
				// because that's what WCSimPolygonTools expects
				G4TwoVector topLeft     = G4TwoVector(iHoriz * cellSide,     -iVert * cellSide) + centreToTopLeft;
				G4TwoVector topRight    = G4TwoVector((iHoriz+1) * cellSide, -iVert * cellSide) + centreToTopLeft;
				G4TwoVector bottomLeft  = G4TwoVector(iHoriz * cellSide,     -(iVert+1) * cellSide) + centreToTopLeft;
				G4TwoVector bottomRight = G4TwoVector((iHoriz+1) * cellSide, -(iVert+1) * cellSide) + centreToTopLeft;

				bool cellInPolygon = true;
				if(    !(WCSimPolygonTools::PolygonContains(nSides, fGeoConfig->GetInnerRadius(), topLeft))
					|| !(WCSimPolygonTools::PolygonContains(nSides, fGeoConfig->GetInnerRadius(), topRight))
					|| !(WCSimPolygonTools::PolygonContains(nSides, fGeoConfig->GetInnerRadius(), bottomLeft))
					|| !(WCSimPolygonTools::PolygonContains(nSides, fGeoConfig->GetInnerRadius(), bottomRight)) ){
					cellInPolygon = false;
				}
				cellsInPolygon += cellInPolygon;
			}
		}
		double topCoverage =   (cellsInPolygon * topCell.GetPhotocathodeCoverage(cellSide) * cellSide * cellSide)
							 / (WCSimPolygonTools::GetAreaFromRadius(nSides, fGeoConfig->GetInnerRadius()));

		if( fabs(topCoverage-fGeoConfig->GetCoverageFraction()) < bestCoverage ){
			bestCoverage = topCoverage;
			bestSide = cellSide;
		}

		// Scale the cell by the difference and iterate again
		cellSide = cellSide * sqrt(fGeoConfig->GetCoverageFraction() / topCoverage);
	}
  std::cout << "Best top size = " << bestSide << std::endl;
	return bestSide;
}

double WCSimCherenkovBuilder::GetOptimalWallCellSize() {
	double coverage 		= fGeoConfig->GetCoverageFraction(); // Photocathode coverage
	double wallHeight 		= GetBarrelLengthForCells(); // Height of walls
	double wallSide 		= 2.0 * fGeoConfig->GetInnerRadius() *
					  	  	  sin(M_PI / fGeoConfig->GetNSides()); // Side length of walls
	double wallCentreRadius = fGeoConfig->GetInnerRadius() *
							  cos(M_PI / fGeoConfig->GetNSides()); // Distance from centre of detector to centre of wall
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
	ConstructEndCap(+1);
	ConstructEndCap(-1);
}

void WCSimCherenkovBuilder::ConstructEndCap(G4int zflip) {
	ConstructEndCapFrame(zflip);
	ConstructEndCapAnnuli(zflip);
	ConstructEndCapWalls(zflip);

}

void WCSimCherenkovBuilder::ConstructEndCapFrame(G4int zflip){
	// TODO: Set these variables once in a single function, then get them from there

	// angle per regular cell:
	G4double totalAngle = 2.0 * pi * rad;
	G4double dPhi = totalAngle / fGeoConfig->GetNSides();

	//-----------------------------------------------------
	// Form annular section of barrel to hold PMTs
	//----------------------------------------------------

	G4double innerAnnulusRadius = fGeoConfig->GetInnerRadius()
								  - GetMaxBarrelExposeHeight() - 1. * mm;
	G4double outerAnnulusRadius = fGeoConfig->GetInnerRadius()
								  + fBlacksheetThickness + 1. * mm;
	// + structure of the frame etc.
	// The radii are measured to the center of the surfaces
	// (tangent distance). Thus distances between the corner and the center are bigger.

	// Zip them up into arrays to pass to Geant
	//	G4double mainAnnulusZ[2] = { -0.5 * GetBarrelLengthForCells(),
	//								  0.5 * GetBarrelLengthForCells() };
	//	G4double mainAnnulusRmin[2] = { innerAnnulusRadius, innerAnnulusRadius };
	//	G4double mainAnnulusRmax[2] = { outerAnnulusRadius, outerAnnulusRadius };

	G4Material * pureWater = G4Material::GetMaterial("Water");

	G4double capAssemblyHeight = (fGeoConfig->GetInnerHeight() - GetBarrelLengthForCells())/2
			  	  	  	  	  	   +1*mm + fBlacksheetThickness;

	G4Tubs* capSolid = new G4Tubs("cap", 0.0*m,
								  outerAnnulusRadius/cos(dPhi/2.),
								  capAssemblyHeight/2,
								  0.*deg,
								  360.*deg);

    G4LogicalVolume* fCapLogic = new G4LogicalVolume(capSolid,
    														pureWater,
															"cap",
															0,0,0);

    return;
}

void WCSimCherenkovBuilder::ConstructEndCapAnnuli( G4int zflip ){


	  //----------------------------------------------------
	  // extra rings for the top and bottom of the annulus
	  //---------------------------------------------------
	  G4double innerAnnulusRadius = fGeoConfig->GetInnerRadius()
								    - GetMaxBarrelExposeHeight() - 1. * mm;
	  G4double outerAnnulusRadius = fGeoConfig->GetInnerRadius()
									  + fBlacksheetThickness + 1. * mm;
	  G4double totalAngle = 2.0 * pi * rad;
	  G4double capAssemblyHeight = (fGeoConfig->GetInnerHeight()-GetBarrelLengthForCells())/2.0
			  		  	  	  	   + 1*mm + fBlacksheetThickness;
	  G4Material * pureWater = G4Material::GetMaterial("Water");
	  G4Material * blacksheet = G4Material::GetMaterial("Blacksheet");

	  G4double borderAnnulusZ[3] = {-0.5 * GetMaxBarrelExposeHeight()*zflip,
	                                (-0.5 * GetBarrelLengthForCells() +
	                                (fGeoConfig->GetInnerRadius() - innerAnnulusRadius))*zflip,
	                                0.5 * GetBarrelLengthForCells() *zflip};
	  G4double borderAnnulusRmin[3] = { fGeoConfig->GetInnerRadius(), innerAnnulusRadius, innerAnnulusRadius};
	  G4double borderAnnulusRmax[3] = {outerAnnulusRadius, outerAnnulusRadius,outerAnnulusRadius};

    for(int i = 0; i < 3; ++i){
      std::cout << "borderAnnulusZ" << borderAnnulusZ[i] << std::endl;
      std::cout << "borderAnnulusRmin" << borderAnnulusRmin[i] << std::endl;
      std::cout << "borderAnnulusRmax" << borderAnnulusRmax[i] << std::endl;
    }

	  G4Polyhedra* solidWCBarrelBorderRing = new G4Polyhedra("WCBarrelBorderRing",
	                                                   0.*deg, // phi start
	                                                   totalAngle,
	                                                   fGeoConfig->GetNSides(), //NPhi-gon
	                                                   3,
	                                                   borderAnnulusZ,
	                                                   borderAnnulusRmin,
	                                                   borderAnnulusRmax);
	  G4LogicalVolume* logicWCBarrelBorderRing = new G4LogicalVolume(solidWCBarrelBorderRing,
																	 pureWater,
																	 "WCBarrelRing",
																	 0,0,0);

 	  G4VPhysicalVolume* physiWCBarrelBorderRing =
       new G4PVPlacement(0,
 						G4ThreeVector(0.,0.,(capAssemblyHeight/2.- GetBarrelLengthForCells()/2.)*zflip),
 						logicWCBarrelBorderRing,
 						"WCBarrelBorderRing",
 						fCapLogic,
 						false, 0,true);
// 
// 
// 
// 	  // These have no PMTs by construction so we just need to back them out with blacksheet
// 	  // TODO: check this section thoroughly - I've rejigged it from old WCSim - AJP 27/08/2014
// 	  G4LogicalVolume* logicWCBarrelBorderBlacksheet = new G4LogicalVolume(solidWCBarrelBorderRing,
// 																	 	   blacksheet,
// 																	 	   "WCBarrelBorderBlacksheet",
// 																	 	   0,0,0);
// 
// 
	  G4VPhysicalVolume* physiWCBarrelBorderCellBlacksheet = new G4PVPlacement(0,
																			   G4ThreeVector(0.,0.,0.),
																			   logicWCBarrelBorderRing,
																			   "WCBarrelBorderBlacksheet",
																			   fCapLogic,
																			   false,
																			   0,true);

	  G4LogicalBorderSurface * WaterBSBarrelBorderCellSurface
	    = new G4LogicalBorderSurface("WaterBSBarrelCellSurface",
	                                 physiWCBarrelBorderRing,
	                                 physiWCBarrelBorderCellBlacksheet,
	                                 OpWaterBSSurface);

	  return;
}

void WCSimCherenkovBuilder::ConstructEndCapWalls(G4int zflip)
{
	// TODO: put these into a function
	  G4double innerAnnulusRadius = fGeoConfig->GetInnerRadius()
								    - GetMaxBarrelExposeHeight() - 1. * mm;
	  G4double outerAnnulusRadius = fGeoConfig->GetInnerRadius()
									  + fBlacksheetThickness + 1. * mm;
	  G4double totalAngle = 2.0 * pi * rad;
	  G4double capAssemblyHeight = (fGeoConfig->GetInnerHeight()-GetBarrelLengthForCells())/2.0
			  		  	  	  	   + 1*mm + fBlacksheetThickness;
	  G4Material * pureWater = G4Material::GetMaterial("Water");
	  G4Material * blacksheet = G4Material::GetMaterial("Blacksheet");

	  G4double WCBarrelPMTOffset = 0.0; // I think the new placement method takes care of this already (AJP 27/8/14)
	  	  	  	  	  	  	  	  	  	// Leaving it here in case not - don't want to lose where it's used
	 //------------------------------------------------------------
	 // add caps
	 // -----------------------------------------------------------

	  G4double capZ[4] = { (-fBlacksheetThickness-1.*mm)*zflip,
	                      WCBarrelPMTOffset*zflip,
	                      WCBarrelPMTOffset*zflip,
	                      (WCBarrelPMTOffset+(fGeoConfig->GetInnerRadius()-innerAnnulusRadius))*zflip} ;
	  G4double capRmin[4] = {  0. , 0., 0., 0.} ;
	  G4double capRmax[4] = {outerAnnulusRadius, outerAnnulusRadius,  fGeoConfig->GetInnerRadius(), innerAnnulusRadius};
	  G4VSolid* solidWCCap = new G4Polyhedra("WCCap",
											 0.*deg, // phi start
											 totalAngle, //phi end
											 fGeoConfig->GetNSides(), //NPhi-gon
											 4, // 2 z-planes
											 capZ, //position of the Z planes
											 capRmin, // min radius at the z planes
											 capRmax// max radius at the Z planes
											);

	  // G4cout << *solidWCCap << G4endl;
	  G4LogicalVolume* logicWCCap = new G4LogicalVolume(solidWCCap,
														pureWater,
														"WCCapPolygon",
														0,0,0);

	  G4VPhysicalVolume* physiWCCap =
	    new G4PVPlacement(0,                           // no rotation
						  G4ThreeVector(0.,0.,(-capAssemblyHeight/2.+1*mm+fBlacksheetThickness)*zflip),     // its position
						  logicWCCap,          // its logical volume
						  "WCCap",             // its name
						  fCapLogic,                  // its mother volume
						  false,                       // no boolean operations
						  0,true);                          // Copy #



	  if(fDebugMode){
		  G4VisAttributes* tmpVisAtt2 = new G4VisAttributes(G4Colour(.6,0.5,0.5));
		  tmpVisAtt2->SetForceWireframe(true);
		  logicWCCap->SetVisAttributes(tmpVisAtt2);
	  }
	  else{
	    logicWCCap->SetVisAttributes(G4VisAttributes::Invisible);
	  }

	  //---------------------------------------------------------------------
	  // add cap blacksheet
	  // -------------------------------------------------------------------

	  G4double capBlacksheetZ[4] = {-fBlacksheetThickness*zflip, 0., 0., WCBarrelPMTOffset*zflip};
	  G4double capBlacksheetRmin[4] = {0., 0.,
			  	  	  	  	  	  	   fGeoConfig->GetInnerRadius(),
			  	  	  	  	  	  	   fGeoConfig->GetInnerRadius()};
	  G4double capBlacksheetRmax[4] = {fGeoConfig->GetInnerRadius()+fBlacksheetThickness,
	                                   fGeoConfig->GetInnerRadius()+fBlacksheetThickness,
									   fGeoConfig->GetInnerRadius()+fBlacksheetThickness,
									   fGeoConfig->GetInnerRadius()+fBlacksheetThickness};
	  G4VSolid* solidWCCapBlacksheet = new G4Polyhedra("WCCapBlacksheet",
														0.*deg, // phi start
														totalAngle, //total phi
														fGeoConfig->GetNSides(), //NPhi-gon
														4, //  z-planes
														capBlacksheetZ, //position of the Z planes
														capBlacksheetRmin, // min radius at the z planes
														capBlacksheetRmax// max radius at the Z planes
														);

	  G4LogicalVolume* logicWCCapBlacksheet = new G4LogicalVolume(solidWCCapBlacksheet,
																  blacksheet,
																  "WCCapBlacksheet",
																  0,0,0);
	  G4VPhysicalVolume* physiWCCapBlacksheet = new G4PVPlacement(0,
																  G4ThreeVector(0.,0.,0.),
																  logicWCCapBlacksheet,
																  "WCCapBlacksheet",
																  logicWCCap,
																  false,
																  0,true);
	   G4LogicalBorderSurface * WaterBSBottomCapSurface = NULL;
	   WaterBSBottomCapSurface = new G4LogicalBorderSurface("WaterBSCapPolySurface",
	                                   	   	   	   	   	    physiWCCap,physiWCCapBlacksheet,
	                                   	   	   	   	   	    OpWaterBSSurface);

	   G4VisAttributes* WCCapBlacksheetVisAtt
	      = new G4VisAttributes(G4Colour(0.9,0.2,0.2));
	    if(fDebugMode)
	        logicWCCapBlacksheet->SetVisAttributes(WCCapBlacksheetVisAtt);
	    else
	        logicWCCapBlacksheet->SetVisAttributes(G4VisAttributes::Invisible);

	    return;
}

void WCSimCherenkovBuilder::PlaceEndCapPMTs(G4int zflip)
{

	  //---------------------------------------------------------
	  // Add top and bottom PMTs
	  // -----------------------------------------------------
    std::cout << " *** PlaceEndCapPMTs ***    zflip = " << zflip << std::endl;
	  G4double xoffset;
	  G4double yoffset;
	  G4int    icopy = 0;

	  G4RotationMatrix* WCCapPMTRotation = new G4RotationMatrix;
	  if(zflip==-1){
	    WCCapPMTRotation->rotateY(180.*deg);
	  }

	  // loop over the cap
	  // Build a square that contains the cap:
	  G4double squareEdge = fGeoConfig->GetInnerRadius();
	  G4TwoVector squareCentreToTopLeft(-0.5 * squareEdge, 0.5 * squareEdge);

	  G4double xpos = -0.5 * squareEdge;
	  G4double ypos = -0.5 * squareEdge;
    std::cout << "Starting loop!" << std::endl;
	  while( fabs(ypos) <= 0.5 * squareEdge  ){
      xpos = -0.5 * squareEdge;
		  while(fabs(xpos) <= squareEdge){
        assert( fTopCellSize > 0.2);
        std::cout << "xpos = " << xpos << "   ypos = " << ypos << "  top cell size = " << fTopCellSize << std::endl;
			  G4ThreeVector cellpos = G4ThreeVector(xpos, ypos, 0.0); // Coordinates of the bottom left corner of the unit cell

			  if( WCSimPolygonTools::PolygonContainsSquare(5, fGeoConfig->GetInnerRadius(), G4TwoVector(cellpos.x(), cellpos.y()), fTopCellSize)){
				  // TODO: loop through all PMTs in the unit cell here
				  G4VPhysicalVolume* physiCapPMT = new G4PVPlacement(WCCapPMTRotation,
																   cellpos,     // its position
																   logicWCPMT,  // its logical volume
																   "WCPMT",     // its name
																   fCapLogic,  // its mother volume
																   false,       // no boolean os
																   icopy);      // every PMT need a unique id.
        std::cout << "Placed PMT!" << std::endl;
				// logicWCPMT->GetDaughter(0),physiCapPMT is the glass face. If you add more
				// daugter volumes to the PMTs (e.g. a acryl cover) you have to check, if
				// this is still the case.
			  }
			  xpos += fTopCellSize;
		  }
		  ypos += fTopCellSize;
	  }
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


G4VPhysicalVolume* WCSimCherenkovBuilder::Construct()
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

  G4LogicalVolume* logicWCBox;
  // Select between cylinder and mailbox
  if (isMailbox) logicWCBox = ConstructMailboxWC();
  else logicWCBox = ConstructWC(); 

  G4cout << " WCLength       = " << WCLength/m << " m"<< G4endl;

  //-------------------------------

  // Now make the detector Hall.  The lengths of the subdectors 
  // were set above.

  G4double expHallLength = 3.*WCLength; //jl145 - extra space to simulate cosmic muons more easily

  G4cout << " expHallLength = " << expHallLength / m << G4endl;
  G4double expHallHalfLength = 0.5*expHallLength;

  G4Box* solidExpHall = new G4Box("expHall",
				  expHallHalfLength,
				  expHallHalfLength,
				  expHallHalfLength);
  
  G4LogicalVolume* logicExpHall = 
    new G4LogicalVolume(solidExpHall,
			G4Material::GetMaterial("Vacuum"),
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

	  //G4RotationMatrix* rotationMatrix = new G4RotationMatrix;
	  //rotationMatrix->rotateX(90.*deg);
	  //rotationMatrix->rotateZ(90.*deg);

  G4ThreeVector genPosition = G4ThreeVector(0., 0., WCPosition);
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
	           &WCSimCherenkovBuilder::DescribeAndRegisterPMT) ;
  
  
  TraverseReplicas(physiWCBox, 0, G4Transform3D(), 
		   &WCSimCherenkovBuilder::GetWCGeom) ;
  
  DumpGeometryTableToFile();
  
  // Return the pointer to the physical experimental hall
  return physiExpHall;
}

G4LogicalVolume * WCSimCherenkovBuilder::ConstructWC()
{
  G4LogicalVolume * logicWC = NULL;
  logicWC = ConstructDetector();
  
  std::cout << "This works "<< std::endl;
  return logicWC;
}

void WCSimCherenkovBuilder::SetPositions()
{
	 //-----------------------------------------------------
	 // Positions
	 //-----------------------------------------------------
	 fDebugMode = false;
	 WCPosition = 0.;	  //Set the WC tube offset to zero

	 WCIDRadius = (G4int)fGeoConfig->GetInnerRadius();

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

	 //  innerAnnulusRadius = fGeoConfig->GetInnerRadius() - WCPMTExposeHeight-1.*mm;
	 innerAnnulusRadius = fGeoConfig->GetInnerRadius() - fPMTConfigs[0].GetExposeHeight()
	 - 1. * mm;
	 outerAnnulusRadius = fGeoConfig->GetInnerRadius() + fBlacksheetThickness + 1. * mm;//+ Stealstructure etc.
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

void WCSimCherenkovBuilder::ConstructPMTs()
{
  std::cout << "SIZE OF CONFIGS VECTOR = " << fPMTConfigs.size() << std::endl;
	fPMTBuilder.ConstructPMTs( fPMTConfigs );
}
