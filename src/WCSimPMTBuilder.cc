/*
 * WCSimPMTBuilder.cc
 *
 *  Created on: 3 Sep 2014
 *      Author: andy
 */
#include "WCSimMaterialsBuilder.hh"
#include "WCSimPMTBuilder.hh"
#include "WCSimPMTConfig.hh"
#include "G4Box.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Polycone.hh"
#include "G4PVPlacement.hh"
#include "G4Sphere.hh"
#include "G4SubtractionSolid.hh"
#include "G4SurfaceProperty.hh"
#include "G4ThreeVector.hh"
#include "G4VisAttributes.hh"
#include <vector>
#include <map>

WCSimPMTBuilder::WCSimPMTBuilder(){
}

WCSimPMTBuilder::~WCSimPMTBuilder() {
	// TODO Auto-generated destructor stub
}

G4LogicalVolume* WCSimPMTBuilder::GetPMTLogicalVolume(WCSimPMTConfig config) {

	G4LogicalVolume * vol = NULL;
  std::cout << "Config name = " << config.GetPMTName() << std::endl;
	if( fPMTLogicalVolumes.find(config.GetPMTName()) != fPMTLogicalVolumes.end()){
		vol = fPMTLogicalVolumes[config.GetPMTName()];
	}
	else{
		std::cerr << "Could not find PMT config in the list of PMT logical volumes" << std::endl;
    std::map<std::string, G4LogicalVolume*>::iterator mapItr2 = fPMTLogicalVolumes.begin();
    for( ; mapItr2 != fPMTLogicalVolumes.end(); ++mapItr2){
      std::cout << (*mapItr2).first << std::endl;
    }
		assert(0);
	}
	return vol;
}

void WCSimPMTBuilder::ConstructPMT(WCSimPMTConfig config) {

	// Don't recreate if it already exists:
	if(fPMTLogicalVolumes.find(config.GetPMTName()) != fPMTLogicalVolumes.end()){ return; }

	double tmpRadius = config.GetRadius();
	double tmpExpose = config.GetExposeHeight();
	double sphereRadius = (tmpExpose*tmpExpose + tmpRadius*tmpRadius)/(2*tmpExpose);
	double PMTOffset =  sphereRadius - tmpExpose;

	//All components of the PMT are now contained in a single logical volume logicWCPMT.
	//Origin is on the blacksheet, faces positive z-direction.

	G4double PMTHolderZ[2] = {0, tmpExpose};
	G4double PMTHolderR[2] = {tmpRadius, tmpRadius};
	G4double PMTHolderr[2] = {0,0};

	G4Polycone* solidWCPMT = new G4Polycone("WCPMT", 0.0*deg, 360.0*deg, 2,
		  	  	  	  	  	  	  	  	    PMTHolderZ, PMTHolderR, PMTHolderr);

	G4Material * water = WCSimMaterialsBuilder::Instance()->GetMaterial("Water");
	G4LogicalVolume* logicWCPMT = new G4LogicalVolume(solidWCPMT, water, "WCPMT", 0,0,0);

	G4VisAttributes* WCPMTVisAtt = new G4VisAttributes(G4Colour(0.2,0.2,0.2));
	WCPMTVisAtt->SetForceWireframe(true);
	logicWCPMT->SetVisAttributes(WCPMTVisAtt);

	//Need a volume to cut away excess behind blacksheet
	G4Box* solidCutOffTubs = new G4Box("cutOffTubs", sphereRadius+1.*cm, sphereRadius+1.*cm, PMTOffset);

	//Create PMT Interior
	G4Sphere* tmpSolidInteriorWCPMT = new G4Sphere("tmpInteriorWCPMT",
												   0.0*m,(sphereRadius-config.GetGlassThickness()),
												   0.0*deg,360.0*deg, 0.0*deg,90.0*deg);

	G4SubtractionSolid* solidInteriorWCPMT = new G4SubtractionSolid("InteriorWCPMT",
																	tmpSolidInteriorWCPMT,
																	solidCutOffTubs);

	G4Material * vacuum = WCSimMaterialsBuilder::Instance()->GetMaterial("Vacuum");
	G4LogicalVolume* logicInteriorWCPMT = new G4LogicalVolume(solidInteriorWCPMT, vacuum,
															  "InteriorWCPMT",0,0,0);

	G4VPhysicalVolume* physiInteriorWCPMT =	new G4PVPlacement(0,
	  														  G4ThreeVector(0, 0, -1.0*PMTOffset),
	  														  logicInteriorWCPMT, "InteriorWCPMT",
	  														  logicWCPMT,	false, 0);
	//Create PMT Glass Face
	G4Sphere* tmpGlassFaceWCPMT = new G4Sphere(	"tmpGlassFaceWCPMT",
												(sphereRadius-config.GetGlassThickness()),
												sphereRadius, 0.0*deg, 360.0*deg,
												0.0*deg, 90.0*deg);

	G4SubtractionSolid* solidGlassFaceWCPMT = new G4SubtractionSolid("GlassFaceWCPMT",tmpGlassFaceWCPMT,
																	 solidCutOffTubs);

	G4Material * glass = WCSimMaterialsBuilder::Instance()->GetMaterial("Glass");
	G4LogicalVolume * logicGlassFaceWCPMT = new G4LogicalVolume(solidGlassFaceWCPMT, glass,
															  "GlassFaceWCPMT", 0, 0, 0);

	G4VPhysicalVolume* physiGlassFaceWCPMT = new G4PVPlacement(0, G4ThreeVector(0, 0, -1.0 * PMTOffset),
															   logicGlassFaceWCPMT, "GlassFaceWCPMT",
															   logicWCPMT, false, 0);

	logicGlassFaceWCPMT->SetVisAttributes(G4VisAttributes::Invisible);

	//Add Logical Border Surface
	G4SurfaceProperty * OpGlassCathodeSurface = (G4SurfaceProperty*)(WCSimMaterialsBuilder::Instance()->GetOpticalSurface("GlassCathodeSurface"));
	G4LogicalBorderSurface* GlassCathodeSurface = new G4LogicalBorderSurface(
																			 "GlassCathodeSurface",
																			 physiGlassFaceWCPMT,
																			 physiInteriorWCPMT,
																			 OpGlassCathodeSurface);

	fPMTLogicalVolumes[config.GetPMTName()] = logicWCPMT;

}

void WCSimPMTBuilder::ConstructPMTs(std::vector<WCSimPMTConfig> configVec) {
	std::vector<WCSimPMTConfig>::const_iterator confItr = configVec.begin();
  std::cout << "Number of CONFIGS = " << configVec.size() << std::endl;
  std::cout << "Config name = " << configVec.at(0).GetPMTName() << std::endl;
	for( ; confItr != configVec.end(); ++confItr){
		ConstructPMT((*confItr));
	}
	return;
}


