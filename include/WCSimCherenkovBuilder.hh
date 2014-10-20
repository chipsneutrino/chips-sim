/**
 * \class WCSimCherenkovBuilder
 * Main class for constructing the final water Cherenkov detector object.
 *
 *  Created on: Aug 15, 2014
 *      Author: aperch
 */

#ifndef WCSIMCHERENKOVBUILDER_HH_
#define WCSIMCHERENKOVBUILDER_HH_

#include "WCSimDetectorConstruction.hh"
#include "WCSimGeoConfig.hh"
class WCSimPMTManager;
class WCSimUnitCell;
class G4LogicalVolume;

class WCSimCherenkovBuilder : public WCSimDetectorConstruction{
public:
	WCSimCherenkovBuilder(G4int DetConfig);
	virtual ~WCSimCherenkovBuilder();
	void SetGeoConfig(WCSimGeoConfig * config);
	WCSimGeoConfig * GetGeoConfig() const;
	G4LogicalVolume * ConstructDetector(); //< Main function to build the detector - has to return a pointer for compatibility with old code

protected:
  void UpdateGeometry(); //< Reset a bunch of things if we change the geometry to avoid pointer awkwardness

private:
  //G4VPhysicalVolume* Construct(); //< Overload this for now TODO: reorganise
  G4LogicalVolume* ConstructWC(); //< Overload from WCSimConstructWC TODO: reorganise
  void SetPositions(); /*< Sets a bunch of physical constants in the parent WCSimDetectorContruction
                        * object.  Has to run before making any geometry objects TODO: reorganise */


 	void ConstructDetectorWrapper(); //< Main function to build the detector
	void ConstructEnvironment(); //< Build the lake for the detector to sit in
	void ConstructFrame(); //< The external metal frame
	void ConstructVeto(); //< The veto region (currently not implemented)
	void ConstructInnerDetector(); //< The inner part of the detector
	void CreatePrism(); //< Build the inner wall (an n-sided prism)
	void CreatePrismRings(); //< Divide it into slices along the long axis
	void CreateRingSegments(); //< Divide each slice into n segments
	void CreateSegmentCells(); //< Fill each segment with WCSimUnitCells


	void ConstructEndCaps(); //< Build the top and bottom caps of the detector
	void ConstructEndCap(G4int zflip); //< Construct top (zflip = true) and bottom (false) caps
	void ConstructEndCapFrame(G4int zflip);
	void ConstructEndCapRings( G4int zflip );
	void ConstructEndCapRingSegments( G4int zflip );
	void ConstructEndCapSurfaces(G4int zflip);
  void ConstructEndCapPhysicalVolumes();

  void PlacePMTs(); //< Put the PMTs into each unit cell
	void PlaceEndCapPMTs(G4int zflip);
  void PlaceBarrelPMTs();

	void CreateSensitiveDetector(); //< Make the photocathodes responsive

    void GetMeasurements();
	double GetBarrelLengthForCells(); //< Work out much of the barrel wall can hold PMTs without overlapping the top
	double GetMaxTopExposeHeight(); //< Work out how far PMTs extend down from the top cap
	double GetMaxBarrelExposeHeight(); //< Work out how far PMTs extend inwars from the blacksheet of the walls

	// n.b. Close packing algorithms are tricky.  These methods are quick and simple but certainly not optimal
	void CalculateCellSizes(); //< Optimise the sizes of the unit cells on the cap and walls to get close to the desired coverage
	double GetOptimalTopCellSize(); //< Optimise the cell placement on the endcaps
	double GetOptimalWallCellSize(); //< Optimise the cell placement on the detector walls

	void ConstructUnitCells(); //< Construct the unit cell objects specified in the geometry config
	WCSimUnitCell * GetTopUnitCell(); //< For now we only have one type all over the detector
	WCSimUnitCell * GetBarrelUnitCell(); //< For now we only have one type all over the detector

	void ConstructPMTs();

	bool fConstructed;
	double fWallCellSize;
	double fTopCellSize;
	int fNumPMTs;

  WCSimGeoConfig temp; // TODO: deal with this in a much better way
	WCSimGeoConfig * fGeoConfig;
	WCSimPMTManager * fPMTManager;
	std::vector<WCSimUnitCell*> fUnitCells;

	double fWallCellLength;  //< Size length of a unit cell on the detector wall
	int fWallCellsX; //< Number of unit cells fitting across the wall
	int fWallCellsZ; //< Number of unit cells fitting vertically along the wall

	// TODO:  Constants that should be moved into xml files and GeoConfig
	double fBlacksheetThickness;
	bool   fDebugMode;

	// Geant objects:
	G4LogicalVolume* fLakeLogic;
	G4LogicalVolume* fBarrelLogic;
	G4LogicalVolume* fPrismLogic;
	G4LogicalVolume* fPrismRingLogic;
	G4LogicalVolume* fSegmentLogic;
	G4LogicalVolume* fCapLogicTop;
	G4LogicalVolume* fCapLogicTopRing;
	G4LogicalVolume* fCapLogicBottom;
	G4LogicalVolume* fCapLogicBottomRing;

	// Constants used to specify the geometry
	G4bool fGotMeasurements;

	// First the main barrel of the detector
	G4double fBarrelRadius; // Barrel is a cylinder - this is its actual radius
	G4double fBarrelHeight;
	G4double fBarrelLengthForCells;

	// Inside the main barrel lives an n-gon prism
	G4double fPrismRadiusInside; // Radius is to centre of wall not the vertex
	G4double fPrismRadiusOutside;
	G4double fPrismHeight;

	// We chop the prism into a whole number of rings
	G4double fPrismRingRadiusInside; // Radius is to centre of wall not the vertex
	G4double fPrismRingRadiusOutside;
	G4double fPrismRingHeight;

	// And chop each n-gon ring into n flat sides
	G4double fPrismRingSegmentRadiusInside; // To centre of segment not edge
	G4double fPrismRingSegmentRadiusOutside;
	G4double fPrismRingSegmentHeight;
	G4double fPrismRingSegmentDPhi;

	// Then we have a layer of blacksheet on each side
	G4double fPrismRingSegmentBSRadiusInside; // To centre not edge
	G4double fPrismRingSegmentBSRadiusOutside;
	G4double fPrismRingSegmentBSHeight;

	// Now the caps
	///////////////

	// A tubs for the cap volume to sit in
	G4double fCapAssemblyHeight;
	G4double fCapAssemblyRadius;

	// We made a whole number of barrel rings to hold PMTs
	// Here we two extras to plug the hole between the top of those rings and the cap
	G4double fCapRingRadiusInside;
	G4double fCapRingRadiusOutside;
	G4double fCapRingHeight;

	// We break the extra rings into n flat sides again
	G4double fCapRingSegmentDPhi;
	G4double fCapRingSegmentRadiusInside;
	G4double fCapRingSegmentRadiusOutside;
	G4double fCapRingSegmentHeight;

	// And put blacksheet on the edge of them
	G4double fCapRingSegmentBSRadiusInside;
	G4double fCapRingSegmentBSRadiusOutside;
	G4double fCapRingSegmentBSHeight;

	// The actual top/bottom of the detector is a polygon - 
  // we'll make a container that holds the top and edge blacksheet, and the water in the middle
	G4double fCapPolygonRadius;
	G4double fCapPolygonHeight;

  // Here's the centre section
  G4double fCapPolygonCentreRadius;
  G4double fCapPolygonCentreHeight;

	// We put a layer of blacksheet on the edge
	G4double fCapPolygonEdgeBSRadiusInside;
	G4double fCapPolygonEdgeBSRadiusOutside;
	G4double fCapPolygonEdgeBSHeight;

	// And a layer on top
	G4double fCapPolygonEndBSRadius;
	G4double fCapPolygonEndBSHeight;

	// And we're done!

	/* CapSurface looks like this:
	 *
	 *					 outside----->
	 * 		__________________________
	 * 	   |_						 _|
	 *		 |			 inside---->|
	 *		 |______________________|
	 */
};

#endif /* WCSIMCHERENKOVBUILDER_HH_ */
