/*
 * WCSimPMTBuilder.cc
 *
 *  Created on: 3 Sep 2014
 *      Author: andy
 */
#include "WCSimMaterialsBuilder.hh"
#include "WCSimPMTBuilder.hh"
#include "WCSimPMTConfig.hh"
#include "WCSimWCSD.hh"
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
#include <cassert>
#include <vector>
#include <map>

// Wrapper used to access the Geant4 objects we create for each PMT
WCSimGeantPMTWrapper::WCSimGeantPMTWrapper() : fPMTLogicalVolume(NULL), fGlassFaceLogicalVolume(NULL){
}


WCSimGeantPMTWrapper::WCSimGeantPMTWrapper(G4LogicalVolume* PMTLogicalVolume,
		G4LogicalVolume* glassFaceLogicalVolume) :
		fPMTLogicalVolume(PMTLogicalVolume),
		fGlassFaceLogicalVolume(glassFaceLogicalVolume){
}

void WCSimGeantPMTWrapper::Delete()
{
	if( fPMTLogicalVolume != NULL ) { delete fPMTLogicalVolume; }
	if( fGlassFaceLogicalVolume != NULL) { delete fGlassFaceLogicalVolume; }
	Reset();
}

void WCSimGeantPMTWrapper::Reset()
{
	fPMTLogicalVolume = NULL;
	fGlassFaceLogicalVolume = NULL;
	return;
}

G4LogicalVolume* WCSimGeantPMTWrapper::GetPMTLogicalVolume() const {
	assert(fPMTLogicalVolume != NULL);
	return fPMTLogicalVolume;
}

G4LogicalVolume* WCSimGeantPMTWrapper::GetGlassFaceLogicalVolume() const {
	assert(fGlassFaceLogicalVolume != NULL);
	return fGlassFaceLogicalVolume;
}


// Class to construct all our PMTs
WCSimPMTBuilder::WCSimPMTBuilder(){
}

WCSimPMTBuilder::~WCSimPMTBuilder() {
	// TODO Auto-generated destructor stub
}

G4LogicalVolume* WCSimPMTBuilder::GetPMTLogicalVolume(WCSimPMTConfig config) {

	G4LogicalVolume * vol = NULL;
    // std::cout << "Config name = " << config.GetPMTName() << std::endl;
	if( fPMTLogicalVolumes.find(config.GetPMTName()) != fPMTLogicalVolumes.end()){
		WCSimGeantPMTWrapper wrapper = (fPMTLogicalVolumes[config.GetPMTName()]);
		vol = wrapper.GetPMTLogicalVolume();
	}
	else{
		std::cerr << "Could not find PMT config in the list of PMT logical volumes" << std::endl;
		assert(0);
	}
	return vol;
}

G4LogicalVolume* WCSimPMTBuilder::GetGlassFaceLogicalVolume(WCSimPMTConfig config) {

	G4LogicalVolume * vol = NULL;
    std::cout << "Config name = " << config.GetPMTName() << std::endl;
	if( fPMTLogicalVolumes.find(config.GetPMTName()) != fPMTLogicalVolumes.end()){
		vol = (fPMTLogicalVolumes[config.GetPMTName()]).GetGlassFaceLogicalVolume();
	}
	else{
		std::cerr << "Could not find PMT config in the list of PMT logical volumes" << std::endl;
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

  // Use the PMT name to modify the standard geometry name such that we can
  // tell the difference between different types of PMT later on.
  std::string pmtName = config.GetPMTName();

	G4Polycone* solidWCPMT = new G4Polycone(("WCPMT_"+pmtName).c_str(), 0.0*deg, 360.0*deg, 2,
		  	  	  	  	  	  	  	  	    PMTHolderZ, PMTHolderR, PMTHolderr);

	G4Material * water = WCSimMaterialsBuilder::Instance()->GetMaterial("Water");
	G4LogicalVolume* logicWCPMT = new G4LogicalVolume(solidWCPMT, water, ("WCPMT_"+pmtName).c_str(), 0,0,0);

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
                                      
	fPMTLogicalVolumes[config.GetPMTName()] = WCSimGeantPMTWrapper( logicWCPMT, logicGlassFaceWCPMT);
	std::cout << "PMT object constructed ... " << config.GetPMTName() << "   " << logicWCPMT->GetName() << std::endl;
}

void WCSimPMTBuilder::ConstructPMTs(std::vector<WCSimPMTConfig> configVec) {
    std::cout << "Number of CONFIGS = " << configVec.size() << std::endl;

    Reset();
    for( std::vector<WCSimPMTConfig>::iterator confItr = configVec.begin(); confItr != configVec.end(); ++confItr){
        std::cout << "Config name = " << (*confItr).GetPMTName() << std::endl;
		ConstructPMT((*confItr));
	}
	return;
}

void WCSimPMTBuilder::SetSensitiveDetector(WCSimWCSD * sensDet){
  std::map<std::string, WCSimGeantPMTWrapper>::iterator pmtItr;
  for( pmtItr = fPMTLogicalVolumes.begin(); pmtItr != fPMTLogicalVolumes.end(); ++pmtItr){
    WCSimGeantPMTWrapper wrapper = (*pmtItr).second;
    wrapper.GetGlassFaceLogicalVolume()->SetSensitiveDetector(sensDet);
  }
  return;
}

void WCSimPMTBuilder::Reset() {
	std::map<std::string, WCSimGeantPMTWrapper>::iterator mapItr;
	for(mapItr = fPMTLogicalVolumes.begin(); mapItr != fPMTLogicalVolumes.end(); ++mapItr)
	{
		(*mapItr).second.Reset();
	}
	fPMTLogicalVolumes.clear();
}
