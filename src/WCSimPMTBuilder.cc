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
#include "G4Tubs.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
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
WCSimGeantPMTWrapper::WCSimGeantPMTWrapper() : fPMTLogicalVolume(NULL), fGlassFaceLogicalVolume(NULL)
{
}

WCSimGeantPMTWrapper::WCSimGeantPMTWrapper(G4LogicalVolume *PMTLogicalVolume, G4LogicalVolume *glassFaceLogicalVolume) : fPMTLogicalVolume(PMTLogicalVolume), fGlassFaceLogicalVolume(glassFaceLogicalVolume)
{
}

void WCSimGeantPMTWrapper::Delete()
{
	if (fPMTLogicalVolume != NULL)
	{
		delete fPMTLogicalVolume;
	}
	if (fGlassFaceLogicalVolume != NULL)
	{
		delete fGlassFaceLogicalVolume;
	}
	Reset();
}

void WCSimGeantPMTWrapper::Reset()
{
	fPMTLogicalVolume = NULL;
	fGlassFaceLogicalVolume = NULL;
	return;
}

G4LogicalVolume *WCSimGeantPMTWrapper::GetPMTLogicalVolume() const
{
	assert(fPMTLogicalVolume != NULL);
	return fPMTLogicalVolume;
}

G4LogicalVolume *WCSimGeantPMTWrapper::GetGlassFaceLogicalVolume() const
{
	assert(fGlassFaceLogicalVolume != NULL);
	return fGlassFaceLogicalVolume;
}

// Class to construct all our PMTs
WCSimPMTBuilder::WCSimPMTBuilder()
{
}

WCSimPMTBuilder::~WCSimPMTBuilder()
{
	// TODO Auto-generated destructor stub
}

G4LogicalVolume *WCSimPMTBuilder::GetPMTLogicalVolume(WCSimPMTConfig config)
{

	G4LogicalVolume *vol = NULL;
	// std::cout << "Config name = " << config.GetPMTName() << std::endl;
	if (fPMTLogicalVolumes.find(config.GetPMTName()) != fPMTLogicalVolumes.end())
	{
		WCSimGeantPMTWrapper wrapper = (fPMTLogicalVolumes[config.GetPMTName()]);
		vol = wrapper.GetPMTLogicalVolume();
	}
	else
	{
		std::cerr << "Could not find PMT config in the list of PMT logical volumes" << std::endl;
		assert(0);
	}
	return vol;
}

G4LogicalVolume *WCSimPMTBuilder::GetGlassFaceLogicalVolume(WCSimPMTConfig config)
{

	G4LogicalVolume *vol = NULL;
	std::cout << "Config name = " << config.GetPMTName() << std::endl;
	if (fPMTLogicalVolumes.find(config.GetPMTName()) != fPMTLogicalVolumes.end())
	{
		vol = (fPMTLogicalVolumes[config.GetPMTName()]).GetGlassFaceLogicalVolume();
	}
	else
	{
		std::cerr << "Could not find PMT config in the list of PMT logical volumes" << std::endl;
		assert(0);
	}
	return vol;
}

void WCSimPMTBuilder::ConstructPMT(WCSimPMTConfig config)
{

	// Don't recreate if it already exists:
	if (fPMTLogicalVolumes.find(config.GetPMTName()) != fPMTLogicalVolumes.end())
	{
		return;
	}

	double tmpRadius = config.GetRadius();
	double pmtExpose = config.GetExposeHeight();
	double unsensitiveHeight = config.GetUnsensitiveHeight();
	double sphereRadius = (pmtExpose * pmtExpose + tmpRadius * tmpRadius) / (2 * pmtExpose);
	double tmpExpose = pmtExpose + unsensitiveHeight;

	double PMTCut = sphereRadius - pmtExpose;
	double PMTOffset = sphereRadius - tmpExpose;

	// Light   Collector
	int lcPoints = (config.GetLCConfig()).GetNShapePoints();

	// This limits/forces the number of points for the Light Collector polycone to 100!! Need to be improved.
	G4double LCZ[100];
	G4double LCr[100];
	G4double LCrb[100];
	G4double LCR[100];

	G4double lcReflector_thickness = 0.2, lcSupport_thickness = 0.1; // Hard coded for now!

	for (int ipoint = 0; ipoint < lcPoints; ipoint++)
	{
		LCZ[ipoint] = (config.GetLCConfig()).GetShapeVector()[ipoint].first;
		LCr[ipoint] = (config.GetLCConfig()).GetShapeVector()[ipoint].second;
		LCrb[ipoint] = LCr[ipoint] + lcReflector_thickness;
		LCR[ipoint] = LCrb[ipoint] + lcSupport_thickness;
	}

	G4double LCZmax = (config.GetLCConfig()).GetExposeHeight();
	G4double LCRmax = (config.GetLCConfig()).GetMaxRadius() + lcReflector_thickness + lcSupport_thickness;

	if (LCRmax > tmpRadius)
		tmpRadius = LCRmax;
	if (LCZmax > tmpExpose)
		tmpExpose = LCZmax;

	//All components of the PMT are now contained in a single logical volume logicWCPMT.
	//Origin is on the blacksheet, faces positive z-direction.

	G4double PMTHolderZ[2] = {0, tmpExpose};
	G4double PMTHolderR[2] = {tmpRadius, tmpRadius};
	G4double PMTHolderr[2] = {0, 0};

	// Use the PMT name to modify the standard geometry name such that we can
	// tell the difference between different types of PMT later on.
	std::string pmtName = config.GetPMTName();

	G4Polycone *solidWCPMT = new G4Polycone(("WCPMT_" + pmtName).c_str(), 0.0 * CLHEP::deg, 360.0 * CLHEP::deg, 2, PMTHolderZ,
											PMTHolderr, PMTHolderR);
	//		  	  	  	  	  	  	  	  	    PMTHolderZ, PMTHolderR, PMTHolderr);

	G4Material *water = WCSimMaterialsBuilder::Instance()->GetMaterial("Water");
	G4LogicalVolume *logicWCPMT = new G4LogicalVolume(solidWCPMT, water, ("WCPMT_" + pmtName).c_str(), 0, 0, 0);

	G4VisAttributes *WCPMTVisAtt = new G4VisAttributes(G4Colour(0.2, 0.2, 0.2));
	WCPMTVisAtt->SetForceWireframe(true);
	logicWCPMT->SetVisAttributes(WCPMTVisAtt);

	//Need a volume to cut away excess behind blacksheet
	G4Box *solidCutOffTubs = new G4Box("cutOffTubs", sphereRadius + 1. * CLHEP::cm, sphereRadius + 1. * CLHEP::cm, PMTCut);

	//Create PMT Interior
	G4Sphere *tmpSolidInteriorWCPMT = new G4Sphere("tmpInteriorWCPMT", 0.0 * CLHEP::m,
												   (sphereRadius - config.GetGlassThickness()), 0.0 * CLHEP::deg, 360.0 * CLHEP::deg, 0.0 * CLHEP::deg, 90.0 * CLHEP::deg);

	G4SubtractionSolid *solidInteriorWCPMT = new G4SubtractionSolid("InteriorWCPMT", tmpSolidInteriorWCPMT,
																	solidCutOffTubs);

	G4Material *vacuum = WCSimMaterialsBuilder::Instance()->GetMaterial("Vacuum");
	G4LogicalVolume *logicInteriorWCPMT = new G4LogicalVolume(solidInteriorWCPMT, vacuum, "InteriorWCPMT", 0, 0, 0);

	G4VPhysicalVolume *physiInteriorWCPMT = new G4PVPlacement(0, G4ThreeVector(0, 0, -1.0 * PMTOffset),
															  logicInteriorWCPMT, "InteriorWCPMT", logicWCPMT, false, 0);
	//Create PMT Glass Face
	G4Sphere *tmpGlassFaceWCPMT = new G4Sphere("tmpGlassFaceWCPMT", (sphereRadius - config.GetGlassThickness()),
											   sphereRadius, 0.0 * CLHEP::deg, 360.0 * CLHEP::deg, 0.0 * CLHEP::deg, 90.0 * CLHEP::deg);

	G4SubtractionSolid *solidGlassFaceWCPMT = new G4SubtractionSolid("GlassFaceWCPMT", tmpGlassFaceWCPMT,
																	 solidCutOffTubs);

	G4Material *glass = WCSimMaterialsBuilder::Instance()->GetMaterial("Glass");
	G4LogicalVolume *logicGlassFaceWCPMT = new G4LogicalVolume(solidGlassFaceWCPMT, glass, "GlassFaceWCPMT", 0, 0, 0);

	G4VPhysicalVolume *physiGlassFaceWCPMT = new G4PVPlacement(0, G4ThreeVector(0, 0, -1.0 * PMTOffset),
															   logicGlassFaceWCPMT, "GlassFaceWCPMT", logicWCPMT, false, 0);

	logicGlassFaceWCPMT->SetVisAttributes(G4VisAttributes::Invisible);

	//Add Logical Border Surface
	G4SurfaceProperty *OpGlassCathodeSurface =
		(G4SurfaceProperty *)(WCSimMaterialsBuilder::Instance()->GetOpticalSurface("GlassCathodeSurface"));
	G4LogicalBorderSurface *GlassCathodeSurface = new G4LogicalBorderSurface("GlassCathodeSurface", physiGlassFaceWCPMT,
																			 physiInteriorWCPMT, OpGlassCathodeSurface);

	//Need a volume to the back of the PMT galss and photocathode, to shield back reflections
	if (unsensitiveHeight > 0.1 * CLHEP::cm)
	{

		G4Tubs *solidBackTubs = new G4Tubs("backTubs", 0, config.GetRadius(), 0.05 * CLHEP::cm, 0 * CLHEP::deg, 360 * CLHEP::deg);

		G4Material *blacksheet = WCSimMaterialsBuilder::Instance()->GetMaterial("Blacksheet");

		G4LogicalVolume *logicPMTBack = new G4LogicalVolume(solidBackTubs, glass, "PMTBack", 0, 0, 0);

		G4VPhysicalVolume *physiPMTBack = new G4PVPlacement(0, G4ThreeVector(0, 0, unsensitiveHeight - 0.05 * CLHEP::cm),
															logicPMTBack, ("PMTBack" + pmtName).c_str(), logicWCPMT, false, 0);

		//Add Optical Surfaces
		G4SurfaceProperty *OpWaterPMTBackSurface =
			(G4SurfaceProperty *)(WCSimMaterialsBuilder::Instance()->GetOpticalSurface("WaterPMTBackSurface"));

		G4LogicalSkinSurface *WaterPMTBackSurface = new G4LogicalSkinSurface("WaterPMTBackSurface", logicPMTBack,
																			 OpWaterPMTBackSurface);
	}

	// Light Collector

	if (lcPoints > 0)
	{
		G4Polycone *solidWCLCSupport = new G4Polycone(("LightCollectorSupport" + pmtName).c_str(), 0.0 * CLHEP::deg,
													  360.0 * CLHEP::deg, 100, LCZ, LCrb, LCR);

		G4Material *blacksheet = WCSimMaterialsBuilder::Instance()->GetMaterial("Blacksheet");
		G4LogicalVolume *logicWCLCSupport = new G4LogicalVolume(solidWCLCSupport, blacksheet,
																("LightCollectorSupport" + pmtName).c_str(), 0, 0, 0);
		logicWCLCSupport->SetVisAttributes(G4VisAttributes::Invisible);

		G4VPhysicalVolume *physiWCLCSupport = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), logicWCLCSupport,
																("LightCollectorSupport" + pmtName).c_str(), logicWCPMT, false, 0);

		//Add Optical Surfaces

		G4SurfaceProperty *OpWaterLCoutSurface =
			(G4SurfaceProperty *)(WCSimMaterialsBuilder::Instance()->GetOpticalSurface("WaterLCoutSurface"));

		G4LogicalSkinSurface *WaterLCoutSurface = new G4LogicalSkinSurface("WaterLCoutSurface", logicWCLCSupport,
																		   OpWaterLCoutSurface);

		//  Reflector

		G4Polycone *solidWCLC = new G4Polycone(("LightCollector" + pmtName).c_str(), 0.0 * CLHEP::deg, 360.0 * CLHEP::deg, 100, LCZ,
											   LCr, LCrb);

		G4Material *aluminum = WCSimMaterialsBuilder::Instance()->GetMaterial("Aluminum");
		G4LogicalVolume *logicWCLC = new G4LogicalVolume(solidWCLC, aluminum, ("LightCollector" + pmtName).c_str(), 0,
														 0, 0);

		G4VisAttributes *WCLCVisAtt = new G4VisAttributes(G4Colour(0.0, 0.6, 1.0));
		WCLCVisAtt->SetForceWireframe(true);
		logicWCLC->SetVisAttributes(WCLCVisAtt);

		G4VPhysicalVolume *physiWCLC = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), logicWCLC,
														 ("LightCollector" + pmtName).c_str(), logicWCPMT, false, 0);

		//Add Optical Surfaces

		G4SurfaceProperty *OpWaterLCinSurface =
			(G4SurfaceProperty *)(WCSimMaterialsBuilder::Instance()->GetOpticalSurface("WaterLCinSurface"));

		G4LogicalSkinSurface *WaterLCinSurface = new G4LogicalSkinSurface("WaterLCinSurface", logicWCLC,
																		  OpWaterLCinSurface);

	} // Light Collector

	fPMTLogicalVolumes[config.GetPMTName()] = WCSimGeantPMTWrapper(logicWCPMT, logicGlassFaceWCPMT);
	std::cout << "PMT object constructed ... " << config.GetPMTName() << "   " << logicWCPMT->GetName() << std::endl;
}

void WCSimPMTBuilder::ConstructPMTs(std::vector<WCSimPMTConfig> configVec)
{
	std::cout << "Number of CONFIGS = " << configVec.size() << std::endl;

	Reset();
	for (std::vector<WCSimPMTConfig>::iterator confItr = configVec.begin(); confItr != configVec.end(); ++confItr)
	{
		std::cout << "Config name = " << (*confItr).GetPMTName() << std::endl;
		ConstructPMT((*confItr));
	}
	return;
}

void WCSimPMTBuilder::SetSensitiveDetector(WCSimWCSD *sensDet)
{
	std::map<std::string, WCSimGeantPMTWrapper>::iterator pmtItr;
	for (pmtItr = fPMTLogicalVolumes.begin(); pmtItr != fPMTLogicalVolumes.end(); ++pmtItr)
	{
		WCSimGeantPMTWrapper wrapper = (*pmtItr).second;
		wrapper.GetGlassFaceLogicalVolume()->SetSensitiveDetector(sensDet);
	}
	return;
}

void WCSimPMTBuilder::Reset()
{
	std::map<std::string, WCSimGeantPMTWrapper>::iterator mapItr;
	for (mapItr = fPMTLogicalVolumes.begin(); mapItr != fPMTLogicalVolumes.end(); ++mapItr)
	{
		(*mapItr).second.Reset();
	}
	fPMTLogicalVolumes.clear();
}
