#include "WCSimDetectorConstruction.hh"
#include "WCSimMaterialsBuilder.hh"
#include "WCSimPMTConfig.hh"

#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4Material.hh"
#include "G4Polycone.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalBorderSurface.hh"
#include "CLHEP/Units/SystemOfUnits.h"

//PMT logical volume construction.

void WCSimDetectorConstruction::ConstructPMT()
{

	//  sphereRadius = (WCPMTExposeHeight*WCPMTExposeHeight+ WCPMTRadius*WCPMTRadius)
	//  			/(2*WCPMTExposeHeight);
	//  PMTOffset =  sphereRadius - WCPMTExposeHeight;
	double tmpRadius = fPMTConfigs[0].GetRadius();
	double tmpExpose = fPMTConfigs[0].GetExposeHeight();
	sphereRadius = (tmpExpose * tmpExpose + tmpRadius * tmpRadius) / (2 * tmpExpose);
	PMTOffset = sphereRadius - tmpExpose;

	//All components of the PMT are now contained in a single logical volume logicWCPMT.
	//Origin is on the blacksheet, faces positive z-direction.
	G4Polycone *solidWCPMT;

	//  G4double PMTHolderZ[2] = {0, WCPMTExposeHeight};
	//  G4double PMTHolderR[2] = {WCPMTRadius, WCPMTRadius};
	G4double PMTHolderZ[2] = {0, tmpExpose};
	G4double PMTHolderR[2] = {tmpRadius, tmpRadius};
	G4double PMTHolderr[2] = {0, 0};

	solidWCPMT = new G4Polycone("WCPMT", 0.0 * CLHEP::deg, 360.0 * CLHEP::deg, 2, PMTHolderZ, PMTHolderR, PMTHolderr);

	logicWCPMT = new G4LogicalVolume(solidWCPMT, G4Material::GetMaterial("Water"), "WCPMT", 0, 0, 0);

	G4VisAttributes *WCPMTVisAtt = new G4VisAttributes(G4Colour(0.2, 0.2, 0.2));
	WCPMTVisAtt->SetForceWireframe(true);
	logicWCPMT->SetVisAttributes(WCPMTVisAtt);

	//Need a volume to cut away excess behind blacksheet
	G4Box *solidCutOffTubs = new G4Box("cutOffTubs", sphereRadius + 1. * CLHEP::cm, sphereRadius + 1. * CLHEP::cm, PMTOffset);

	//Create PMT Interior
	G4Sphere *tmpSolidInteriorWCPMT = new G4Sphere("tmpInteriorWCPMT", 0.0 * CLHEP::m,
												   (sphereRadius - fPMTConfigs[0].GetGlassThickness()), 0.0 * CLHEP::deg, 360.0 * CLHEP::deg, 0.0 * CLHEP::deg, 90.0 * CLHEP::deg);

	G4SubtractionSolid *solidInteriorWCPMT = new G4SubtractionSolid("InteriorWCPMT", tmpSolidInteriorWCPMT,
																	solidCutOffTubs);

	G4LogicalVolume *logicInteriorWCPMT = new G4LogicalVolume(solidInteriorWCPMT, G4Material::GetMaterial("Vacuum"),
															  "InteriorWCPMT", 0, 0, 0);

	G4VPhysicalVolume *physiInteriorWCPMT = new G4PVPlacement(0, G4ThreeVector(0, 0, -1.0 * PMTOffset),
															  logicInteriorWCPMT, "InteriorWCPMT", logicWCPMT, false, 0);

	//Create PMT Glass Face
	G4Sphere *tmpGlassFaceWCPMT = new G4Sphere("tmpGlassFaceWCPMT", (sphereRadius - fPMTConfigs[0].GetGlassThickness()),
											   sphereRadius, 0.0 * CLHEP::deg, 360.0 * CLHEP::deg, 0.0 * CLHEP::deg, 90.0 * CLHEP::deg);

	G4SubtractionSolid *solidGlassFaceWCPMT = new G4SubtractionSolid("GlassFaceWCPMT", tmpGlassFaceWCPMT,
																	 solidCutOffTubs);

	logicGlassFaceWCPMT = new G4LogicalVolume(solidGlassFaceWCPMT, G4Material::GetMaterial("Glass"), "GlassFaceWCPMT",
											  0, 0, 0);

	G4VPhysicalVolume *physiGlassFaceWCPMT = new G4PVPlacement(0, G4ThreeVector(0, 0, -1.0 * PMTOffset),
															   logicGlassFaceWCPMT, "GlassFaceWCPMT", logicWCPMT, false, 0);

	logicGlassFaceWCPMT->SetVisAttributes(G4VisAttributes::Invisible);

	//Add Logical Border Surface
	G4LogicalBorderSurface *GlassCathodeSurface = new G4LogicalBorderSurface("GlassCathodeSurface", physiGlassFaceWCPMT,
																			 physiInteriorWCPMT, WCSimMaterialsBuilder::Instance()->GetOpticalSurface("GlassCathodeSurface"));
}
