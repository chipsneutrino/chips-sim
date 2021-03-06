/*
 * WCSimMaterialsBuilder.cc
 *
 *  Created on: 3 Sep 2014
 *      Author: andy
 */

#include "WCSimMaterialsBuilder.hh"
#include "WCSimTuningParameters.hh"
#include "G4Isotope.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4OpticalSurface.hh"
#include "G4UnitsTable.hh"
#include <cassert>
#include <map>
#include <vector>

static WCSimMaterialsBuilder *fgMaterialsBuilder = 0;

WCSimMaterialsBuilder::WCSimMaterialsBuilder() : fOpWaterBSSurface(NULL), fOpWaterWSSurface(NULL), fOpGlassCathodeSurface(NULL), fOpWaterTySurface(NULL), fOpWaterLCinSurface(
																																							  NULL),
												 fOpWaterLCoutSurface(NULL), fOpWaterPlanePipeSurface(NULL), fOpWaterPMTBackSurface(NULL)
{
	BuildVacuum();
	BuildElements();
	BuildMaterials();
	BuildSurfaces();
	BuildMaterialPropertiesTable();
}

WCSimMaterialsBuilder::~WCSimMaterialsBuilder()
{
	// TODO Auto-generated destructor stub
}

WCSimMaterialsBuilder *WCSimMaterialsBuilder::Instance()
{
	if (!fgMaterialsBuilder)
	{
		fgMaterialsBuilder = new WCSimMaterialsBuilder();
	}

	if (!fgMaterialsBuilder)
	{
		assert(fgMaterialsBuilder);
	}

	return fgMaterialsBuilder;
}

void WCSimMaterialsBuilder::BuildVacuum()
{
	G4double density;
	G4double a;

	// The vacuum
	density = CLHEP::universe_mean_density; //from PhysicalConstants.h
	G4double pressure = 1.e-19 * CLHEP::pascal;
	G4double temperature = 0.1 * CLHEP::kelvin;
	a = 1.01 * CLHEP::g / CLHEP::mole;
	G4String name = "Vacuum";
	new G4Material(name, 1., a, density, kStateGas, temperature, pressure);
	fMaterials.push_back(name);
}

void WCSimMaterialsBuilder::BuildElements()
{
	G4double a = 0.0;

	// == Elements ==
	a = 1.01 * CLHEP::g / CLHEP::mole;
	G4String name = "Hydrogen";
	G4Element *elH = new G4Element(name, "H", 1, a);
	fElements[name] = elH;

	a = 10.81 * CLHEP::g / CLHEP::mole;
	name = "Boron";
	G4Element *elB = new G4Element(name, "B", 5, a);
	fElements[name] = elB;

	a = 12.01 * CLHEP::g / CLHEP::mole;
	name = "Carbon";
	G4Element *elC = new G4Element(name, "C", 6, a);
	fElements[name] = elC;

	a = 14.01 * CLHEP::g / CLHEP::mole;
	name = "Nitrogen";
	G4Element *elN = new G4Element(name, "N", 7, a);
	fElements[name] = elN;

	a = 16.00 * CLHEP::g / CLHEP::mole;
	name = "Oxygen";
	G4Element *elO = new G4Element(name, "O", 8, a);
	fElements[name] = elO;

	a = 22.99 * CLHEP::g / CLHEP::mole;
	name = "Sodium";
	G4Element *elNa = new G4Element(name, "Na", 11, a);
	fElements[name] = elNa;

	a = 24.30 * CLHEP::g / CLHEP::mole;
	name = "Magnesium";
	G4Element *elMg = new G4Element(name, "Mg", 12, a);
	fElements[name] = elMg;

	a = 26.98 * CLHEP::g / CLHEP::mole;
	name = "Aluminum";
	G4Element *elAl = new G4Element(name, "Al", 13, a);
	fElements[name] = elAl;

	a = 28.09 * CLHEP::g / CLHEP::mole;
	name = "Silicon";
	G4Element *elSi = new G4Element(name, "Si", 14., a);
	fElements[name] = elSi;

	a = 30.974 * CLHEP::g / CLHEP::mole;
	name = "Phosphore";
	G4Element *elP = new G4Element(name, "P", 15., a);
	fElements[name] = elP;

	a = 32.065 * CLHEP::g / CLHEP::mole;
	name = "Sulphur";
	G4Element *elS = new G4Element(name, "S", 16., a);
	fElements[name] = elS;

	a = 35.453 * CLHEP::g / CLHEP::mole;
	name = "Chlorine";
	G4Element *elCl = new G4Element(name, "Cl", 17., a);
	fElements[name] = elCl;

	a = 39.948 * CLHEP::g / CLHEP::mole;
	name = "Argon";
	G4Element *elAr = new G4Element(name, "Ar", 18., a);
	fElements[name] = elAr;

	a = 39.10 * CLHEP::g / CLHEP::mole;
	name = "Potassium";
	G4Element *elK = new G4Element(name, "K", 19, a);
	fElements[name] = elK;

	a = 40.08 * CLHEP::g / CLHEP::mole;
	name = "Calcium";
	G4Element *elCa = new G4Element(name, "Ca", 20, a);
	fElements[name] = elCa;

	a = 51.9961 * CLHEP::g / CLHEP::mole;
	name = "Chromium";
	G4Element *elCr = new G4Element(name, "Cr", 24., a);
	fElements[name] = elCr;

	a = 54.938 * CLHEP::g / CLHEP::mole;
	name = "Manganese";
	G4Element *elMn = new G4Element(name, "Mn", 25., a);
	fElements[name] = elMn;

	a = 55.85 * CLHEP::g / CLHEP::mole;
	name = "Iron";
	G4Element *elFe = new G4Element(name, "Fe", 26, a);
	fElements[name] = elFe;

	a = 58.6934 * CLHEP::g / CLHEP::mole;
	name = "Nickel";
	G4Element *elNi = new G4Element(name, "Ni", 28., a);
	fElements[name] = elNi;

	a = 157.25 * CLHEP::g / CLHEP::mole;
	name = "Gadolinium";
	G4Element *elGd = new G4Element(name, "Gd", 64, a);
	fElements[name] = elGd;
	// -----------------------
}

void WCSimMaterialsBuilder::BuildMaterials()
{
	G4double density = 0.0;
	G4String name = "";

	// == Materials ==

	density = 1.00 * CLHEP::g / CLHEP::cm3;
	name = "Water";
	G4Material *Water = new G4Material(name, density, 2);
	Water->AddElement(GetElement("Hydrogen"), 2);
	Water->AddElement(GetElement("Oxygen"), 1);
	fMaterials.push_back(name);

	density = 1.00 * CLHEP::g / CLHEP::cm3;
	name = "PitWater";
	G4Material *PitWater = new G4Material(name, density, 2);
	PitWater->AddElement(GetElement("Hydrogen"), 2);
	PitWater->AddElement(GetElement("Oxygen"), 1);
	fMaterials.push_back(name);

	density = 1.00 * CLHEP::g / CLHEP::cm3;
	name = "DopedWater";
	G4Material *DopedWater = new G4Material(name, density, 2);
	DopedWater->AddMaterial(Water, 99.9 * CLHEP::perCent);
	DopedWater->AddElement(GetElement("Gadolinium"), 0.1 * CLHEP::perCent);
	fMaterials.push_back(name);

	density = 0.92 * CLHEP::g / CLHEP::cm3;
	name = "Ice";
	G4Material *Ice = new G4Material(name, density, 2);
	Ice->AddElement(GetElement("Hydrogen"), 2);
	Ice->AddElement(GetElement("Oxygen"), 1);
	fMaterials.push_back(name);

	density = 7.8 * CLHEP::g / CLHEP::cm3;
	name = "Steel";
	G4Material *Steel = new G4Material(name, density, 2);
	Steel->AddElement(GetElement("Carbon"), 1. * CLHEP::perCent);
	Steel->AddElement(GetElement("Iron"), 99. * CLHEP::perCent);
	fMaterials.push_back(name);

	// Stainless steel 304L
	density = 7.81 * CLHEP::g / CLHEP::cm3;
	name = "StainlessSteel";
	G4Material *StainlessSteel = new G4Material(name, density, 8);
	StainlessSteel->AddElement(GetElement("Iron"), 70.44 * CLHEP::perCent);
	StainlessSteel->AddElement(GetElement("Chromium"), 18 * CLHEP::perCent);
	StainlessSteel->AddElement(GetElement("Carbon"), 0.08 * CLHEP::perCent);
	StainlessSteel->AddElement(GetElement("Nickel"), 8 * CLHEP::perCent);
	StainlessSteel->AddElement(GetElement("Phosphore"), 0.45 * CLHEP::perCent);
	StainlessSteel->AddElement(GetElement("Silicon"), 1 * CLHEP::perCent);
	StainlessSteel->AddElement(GetElement("Manganese"), 2 * CLHEP::perCent);
	StainlessSteel->AddElement(GetElement("Sulphur"), 0.03 * CLHEP::perCent);
	fMaterials.push_back(name);

	density = 1.563 * CLHEP::g / CLHEP::cm3;
	name = "SolidDryIce";
	G4Material *DryIce = new G4Material(name, density, 2);
	DryIce->AddElement(GetElement("Carbon"), 1);
	DryIce->AddElement(GetElement("Oxygen"), 2);
	fMaterials.push_back(name);

	density = 1.290 * CLHEP::mg / CLHEP::cm3;
	name = "Air";
	G4Material *Air = new G4Material(name, density, 3);
	Air->AddElement(GetElement("Nitrogen"), 78. * CLHEP::perCent);
	Air->AddElement(GetElement("Oxygen"), 21. * CLHEP::perCent);
	Air->AddElement(GetElement("Argon"), 1. * CLHEP::perCent);
	fMaterials.push_back(name);

	density = 0.95 * CLHEP::g / CLHEP::cm3;
	name = "Plastic";
	G4Material *Plastic = new G4Material(name, density, 2);
	Plastic->AddElement(GetElement("Carbon"), 1);
	Plastic->AddElement(GetElement("Hydrogen"), 2);
	fMaterials.push_back(name);

	density = 1.4 * CLHEP::g / CLHEP::cm3;
	name = "PVC";
	G4Material *PVC = new G4Material(name, density, 3);
	PVC->AddElement(GetElement("Carbon"), 2);
	PVC->AddElement(GetElement("Hydrogen"), 3);
	PVC->AddElement(GetElement("Chlorine"), 1);
	fMaterials.push_back(name);

	density = 2.7 * CLHEP::g / CLHEP::cm3;
	name = "Aluminum";
	G4Material *Aluminum = new G4Material(name, density, 1);
	Aluminum->AddElement(GetElement("Aluminum"), 1);
	fMaterials.push_back(name);

	density = 0.95 * CLHEP::g / CLHEP::cm3;
	name = "Blacksheet";
	G4Material *Blacksheet = new G4Material(name, density, 2);
	Blacksheet->AddElement(GetElement("Carbon"), 1);
	Blacksheet->AddElement(GetElement("Hydrogen"), 2);
	fMaterials.push_back(name);

	density = 0.95 * CLHEP::g / CLHEP::cm3;
	name = "Whitesheet";
	G4Material *Whitesheet = new G4Material(name, density, 2);
	Whitesheet->AddElement(GetElement("Carbon"), 1);
	Whitesheet->AddElement(GetElement("Hydrogen"), 2);
	fMaterials.push_back(name);

	density = 0.38 * CLHEP::g / CLHEP::cm3; //cf. DuPont product handbook
	name = "Tyvek";
	G4Material *Tyvek = new G4Material(name, density, 2);
	Tyvek->AddElement(GetElement("Carbon"), 1); //polyethylene
	Tyvek->AddElement(GetElement("Oxygen"), 2);
	fMaterials.push_back(name);

	// Components for making glass
	density = 2.20 * CLHEP::g / CLHEP::cm3;
	name = "SiO2";
	G4Material *SiO2 = new G4Material(name, density, 2);
	SiO2->AddElement(GetElement("Silicon"), 1);
	SiO2->AddElement(GetElement("Oxygen"), 2);
	fMaterials.push_back(name);

	density = 2.46 * CLHEP::g / CLHEP::cm3;
	name = "B2O3";
	G4Material *B2O3 = new G4Material(name, density, 2);
	B2O3->AddElement(GetElement("Boron"), 2);
	B2O3->AddElement(GetElement("Oxygen"), 3);
	fMaterials.push_back(name);

	density = 2.27 * CLHEP::g / CLHEP::cm3;
	name = "Na2O";
	G4Material *Na2O = new G4Material(name, density, 2);
	Na2O->AddElement(GetElement("Sodium"), 2);
	Na2O->AddElement(GetElement("Oxygen"), 1);
	fMaterials.push_back(name);

	density = 4.00 * CLHEP::g / CLHEP::cm3;
	name = "Al2O3";
	G4Material *Al2O3 = new G4Material(name, density, 2);
	Al2O3->AddElement(GetElement("Aluminum"), 2);
	Al2O3->AddElement(GetElement("Oxygen"), 3);
	fMaterials.push_back(name);

	density = 2.23 * CLHEP::g / CLHEP::cm3;
	name = "Glass";
	G4Material *Glass = new G4Material(name, density, 4);
	Glass->AddMaterial(GetMaterial("SiO2"), 80.6 * CLHEP::perCent);
	Glass->AddMaterial(GetMaterial("B2O3"), 13.0 * CLHEP::perCent);
	Glass->AddMaterial(GetMaterial("Na2O"), 4.0 * CLHEP::perCent);
	Glass->AddMaterial(GetMaterial("Al2O3"), 2.4 * CLHEP::perCent);
	fMaterials.push_back(name);

	density = 2.7 * CLHEP::g / CLHEP::cm3;
	name = "Rock";
	G4Material *Rock = new G4Material(name, density, 8);

	//From Daya-Bay
	Rock->AddElement(GetElement("Oxygen"), 48.50 * CLHEP::perCent);
	Rock->AddElement(GetElement("Silicon"), 34.30 * CLHEP::perCent);
	Rock->AddElement(GetElement("Aluminum"), 8.00 * CLHEP::perCent);
	Rock->AddElement(GetElement("Iron"), 2.00 * CLHEP::perCent);
	Rock->AddElement(GetElement("Calcium"), 0.20 * CLHEP::perCent);
	Rock->AddElement(GetElement("Sodium"), 2.40 * CLHEP::perCent);
	Rock->AddElement(GetElement("Potassium"), 4.50 * CLHEP::perCent);
	Rock->AddElement(GetElement("Magnesium"), 0.10 * CLHEP::perCent);
	fMaterials.push_back(name);
}

G4Material *WCSimMaterialsBuilder::GetVacuum() const
{
	G4String name = "Vacuum";
	return GetMaterial(name);
}

G4Element *WCSimMaterialsBuilder::GetElement(const G4String &name) const
{
	std::map<G4String, G4Element *>::const_iterator mapItr = fElements.find(name);
	G4Element *element = NULL;
	if (mapItr != fElements.end())
	{
		element = (*mapItr).second;
	}
	else
	{
		std::cerr << "Could not find an element called " << name << std::endl;
		assert(mapItr != fElements.end());
	}
	return element;
}

G4Material *WCSimMaterialsBuilder::GetMaterial(const G4String &name) const
{
	if (std::find(fMaterials.begin(), fMaterials.end(), name) == fMaterials.end())
	{
		std::cerr << "Could not find material: " << name << std::endl;
		assert(false);
	}
	return G4Material::GetMaterial(name);
}

void WCSimMaterialsBuilder::BuildMaterialPropertiesTable()
{
	// -------------------------------------------------------------
	// Generate & Add Material Properties Table
	// -------------------------------------------------------------

	G4MaterialPropertiesTable *mpt = new G4MaterialPropertiesTable();

	const G4int nEntries = 2;
	G4double photonEnergy[nEntries] = {1. * CLHEP::eV, 7. * CLHEP::eV};

	// Stainless steel
	G4double abslength_Steel[nEntries] = {.001 * CLHEP::mm, .001 * CLHEP::mm};
	mpt->AddProperty("ABSLENGTH", photonEnergy, abslength_Steel, nEntries);
	GetMaterial("StainlessSteel")->SetMaterialPropertiesTable(mpt);

	const G4int NUMENTRIES = 32;
	G4double PPCKOV[NUMENTRIES] = {2.034E-9 * CLHEP::GeV, 2.068E-9 * CLHEP::GeV, 2.103E-9 * CLHEP::GeV, 2.139E-9 * CLHEP::GeV, 2.177E-9 * CLHEP::GeV,
								   2.216E-9 * CLHEP::GeV, 2.256E-9 * CLHEP::GeV, 2.298E-9 * CLHEP::GeV, 2.341E-9 * CLHEP::GeV, 2.386E-9 * CLHEP::GeV, 2.433E-9 * CLHEP::GeV, 2.481E-9 * CLHEP::GeV, 2.532E-9 * CLHEP::GeV, 2.585E-9 * CLHEP::GeV, 2.640E-9 * CLHEP::GeV, 2.697E-9 * CLHEP::GeV, 2.757E-9 * CLHEP::GeV, 2.820E-9 * CLHEP::GeV, 2.885E-9 * CLHEP::GeV, 2.954E-9 * CLHEP::GeV, 3.026E-9 * CLHEP::GeV, 3.102E-9 * CLHEP::GeV, 3.181E-9 * CLHEP::GeV, 3.265E-9 * CLHEP::GeV, 3.353E-9 * CLHEP::GeV, 3.446E-9 * CLHEP::GeV, 3.545E-9 * CLHEP::GeV, 3.649E-9 * CLHEP::GeV, 3.760E-9 * CLHEP::GeV, 3.877E-9 * CLHEP::GeV, 4.002E-9 * CLHEP::GeV, 4.136E-9 * CLHEP::GeV};

	//From SFDETSIM water absorption
	const G4int NUMENTRIES_water = 60;

	G4double ENERGY_water[NUMENTRIES_water] =
		{1.56962e-09 * CLHEP::GeV, 1.58974e-09 * CLHEP::GeV, 1.61039e-09 * CLHEP::GeV, 1.63157e-09 * CLHEP::GeV, 1.65333e-09 * CLHEP::GeV, 1.67567e-09 * CLHEP::GeV, 1.69863e-09 * CLHEP::GeV, 1.72222e-09 * CLHEP::GeV, 1.74647e-09 * CLHEP::GeV, 1.77142e-09 * CLHEP::GeV, 1.7971e-09 * CLHEP::GeV,
		 1.82352e-09 * CLHEP::GeV, 1.85074e-09 * CLHEP::GeV, 1.87878e-09 * CLHEP::GeV, 1.90769e-09 * CLHEP::GeV, 1.93749e-09 * CLHEP::GeV,
		 1.96825e-09 * CLHEP::GeV, 1.99999e-09 * CLHEP::GeV, 2.03278e-09 * CLHEP::GeV, 2.06666e-09 * CLHEP::GeV, 2.10169e-09 * CLHEP::GeV,
		 2.13793e-09 * CLHEP::GeV, 2.17543e-09 * CLHEP::GeV, 2.21428e-09 * CLHEP::GeV, 2.25454e-09 * CLHEP::GeV, 2.29629e-09 * CLHEP::GeV,
		 2.33962e-09 * CLHEP::GeV, 2.38461e-09 * CLHEP::GeV, 2.43137e-09 * CLHEP::GeV, 2.47999e-09 * CLHEP::GeV, 2.53061e-09 * CLHEP::GeV,
		 2.58333e-09 * CLHEP::GeV, 2.63829e-09 * CLHEP::GeV, 2.69565e-09 * CLHEP::GeV, 2.75555e-09 * CLHEP::GeV, 2.81817e-09 * CLHEP::GeV,
		 2.88371e-09 * CLHEP::GeV, 2.95237e-09 * CLHEP::GeV, 3.02438e-09 * CLHEP::GeV, 3.09999e-09 * CLHEP::GeV, 3.17948e-09 * CLHEP::GeV,
		 3.26315e-09 * CLHEP::GeV, 3.35134e-09 * CLHEP::GeV, 3.44444e-09 * CLHEP::GeV, 3.54285e-09 * CLHEP::GeV, 3.64705e-09 * CLHEP::GeV,
		 3.75757e-09 * CLHEP::GeV, 3.87499e-09 * CLHEP::GeV, 3.99999e-09 * CLHEP::GeV, 4.13332e-09 * CLHEP::GeV, 4.27585e-09 * CLHEP::GeV,
		 4.42856e-09 * CLHEP::GeV, 4.59258e-09 * CLHEP::GeV, 4.76922e-09 * CLHEP::GeV, 4.95999e-09 * CLHEP::GeV, 5.16665e-09 * CLHEP::GeV,
		 5.39129e-09 * CLHEP::GeV, 5.63635e-09 * CLHEP::GeV, 5.90475e-09 * CLHEP::GeV, 6.19998e-09 * CLHEP::GeV};

	// Air
	G4double RINDEX_air[NUMENTRIES_water] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
											 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
											 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
											 1.0, 1.0, 1.0};

	// M Fechner : new ; define the water refraction index using refsg.F
	//from skdetsim using the whole range.
	G4double RINDEX1[NUMENTRIES_water] = {1.32885, 1.32906, 1.32927, 1.32948, 1.3297, 1.32992, 1.33014, 1.33037,
										  1.3306, 1.33084, 1.33109, 1.33134, 1.3316, 1.33186, 1.33213, 1.33241, 1.3327, 1.33299, 1.33329, 1.33361,
										  1.33393, 1.33427, 1.33462, 1.33498, 1.33536, 1.33576, 1.33617, 1.3366, 1.33705, 1.33753, 1.33803, 1.33855,
										  1.33911, 1.3397, 1.34033, 1.341, 1.34172, 1.34248, 1.34331, 1.34419, 1.34515, 1.3462, 1.34733, 1.34858,
										  1.34994, 1.35145, 1.35312, 1.35498, 1.35707, 1.35943, 1.36211, 1.36518, 1.36872, 1.37287, 1.37776, 1.38362,
										  1.39074, 1.39956, 1.41075, 1.42535};

	G4double ABWFF = 1.0;

	// Get from the tuning parameters
	ABWFF = (WCSimTuningParameters::Instance())->GetAbwff();

	//T. Akiri: Values from Skdetsim
	G4double ABSORPTION_water[NUMENTRIES_water] = {16.1419 * CLHEP::cm * ABWFF, 18.278 * CLHEP::cm * ABWFF, 21.0657 * CLHEP::cm * ABWFF,
												   24.8568 * CLHEP::cm * ABWFF, 30.3117 * CLHEP::cm * ABWFF, 38.8341 * CLHEP::cm * ABWFF, 54.0231 * CLHEP::cm * ABWFF, 81.2306 * CLHEP::cm * ABWFF, 120.909 * CLHEP::cm * ABWFF, 160.238 * CLHEP::cm * ABWFF, 193.771 * CLHEP::cm * ABWFF, 215.017 * CLHEP::cm * ABWFF,
												   227.747 * CLHEP::cm * ABWFF, 243.85 * CLHEP::cm * ABWFF, 294.036 * CLHEP::cm * ABWFF, 321.647 * CLHEP::cm * ABWFF, 342.81 * CLHEP::cm * ABWFF,
												   362.827 * CLHEP::cm * ABWFF, 378.041 * CLHEP::cm * ABWFF, 449.378 * CLHEP::cm * ABWFF, 739.434 * CLHEP::cm * ABWFF, 1114.23 * CLHEP::cm * ABWFF, 1435.56 * CLHEP::cm * ABWFF, 1611.06 * CLHEP::cm * ABWFF, 1764.18 * CLHEP::cm * ABWFF, 2100.95 * CLHEP::cm * ABWFF,
												   2292.9 * CLHEP::cm * ABWFF, 2431.33 * CLHEP::cm * ABWFF, 3053.6 * CLHEP::cm * ABWFF, 4838.23 * CLHEP::cm * ABWFF, 6539.65 * CLHEP::cm * ABWFF,
												   7682.63 * CLHEP::cm * ABWFF, 9137.28 * CLHEP::cm * ABWFF, 12220.9 * CLHEP::cm * ABWFF, 15270.7 * CLHEP::cm * ABWFF, 19051.5 * CLHEP::cm * ABWFF, 23671.3 * CLHEP::cm * ABWFF, 29191.1 * CLHEP::cm * ABWFF, 35567.9 * CLHEP::cm * ABWFF, 42583 * CLHEP::cm * ABWFF,
												   49779.6 * CLHEP::cm * ABWFF, 56465.3 * CLHEP::cm * ABWFF, 61830 * CLHEP::cm * ABWFF, 65174.6 * CLHEP::cm * ABWFF, 66143.7 * CLHEP::cm * ABWFF,
												   64820 * CLHEP::cm * ABWFF, 61635 * CLHEP::cm * ABWFF, 57176.2 * CLHEP::cm * ABWFF, 52012.1 * CLHEP::cm * ABWFF, 46595.7 * CLHEP::cm * ABWFF,
												   41242.1 * CLHEP::cm * ABWFF, 36146.3 * CLHEP::cm * ABWFF, 31415.4 * CLHEP::cm * ABWFF, 27097.8 * CLHEP::cm * ABWFF, 23205.7 * CLHEP::cm * ABWFF, 19730.3 * CLHEP::cm * ABWFF, 16651.6 * CLHEP::cm * ABWFF, 13943.6 * CLHEP::cm * ABWFF, 11578.1 * CLHEP::cm * ABWFF,
												   9526.13 * CLHEP::cm * ABWFF};

	// M Fechner: Rayleigh scattering -- as of version 4.6.2 of GEANT,
	// one may use one's own Rayleigh scattering lengths (the buffer is no
	// longer overwritten for "water", see 4.6.2 release notes)

	// RAYFF = 1/ARAS, for those of you who know SKdetsim...
	// actually that's not quite right because the scattering models
	// are different; in G4 there is no scattering depolarization
	// std value at SK = 0.6. But Mie scattering is implemented
	// in SKdetsim and not in G4

	// april 2005 : reduced reflections, let's increase scattering...
	// sep 09: for the large detector like superK the old values are muc better
	//G4double RAYFF = 1.0/1.65;  //old
	//    G4double RAYFF = 1.0/1.5;

	G4double RAYFF = 0.625;

	// Get from the tuning parameters
	RAYFF = (WCSimTuningParameters::Instance())->GetRayff();
	//    printf("RAYFF: %f\n",RAYFF);

	//T. Akiri: Values from Skdetsim
	G4double RAYLEIGH_water[NUMENTRIES_water] = {386929 * CLHEP::cm * RAYFF, 366249 * CLHEP::cm * RAYFF, 346398 * CLHEP::cm * RAYFF, 327355 * CLHEP::cm * RAYFF, 309097 * CLHEP::cm * RAYFF, 291603 * CLHEP::cm * RAYFF, 274853 * CLHEP::cm * RAYFF, 258825 * CLHEP::cm * RAYFF, 243500 * CLHEP::cm * RAYFF, 228856 * CLHEP::cm * RAYFF, 214873 * CLHEP::cm * RAYFF, 201533 * CLHEP::cm * RAYFF, 188816 * CLHEP::cm * RAYFF, 176702 * CLHEP::cm * RAYFF, 165173 * CLHEP::cm * RAYFF, 154210 * CLHEP::cm * RAYFF, 143795 * CLHEP::cm * RAYFF, 133910 * CLHEP::cm * RAYFF, 124537 * CLHEP::cm * RAYFF, 115659 * CLHEP::cm * RAYFF, 107258 * CLHEP::cm * RAYFF, 99318.2 * CLHEP::cm * RAYFF, 91822.2 * CLHEP::cm * RAYFF, 84754 * CLHEP::cm * RAYFF, 78097.3 * CLHEP::cm * RAYFF, 71836.5 * CLHEP::cm * RAYFF, 65956 * CLHEP::cm * RAYFF, 60440.6 * CLHEP::cm * RAYFF, 55275.4 * CLHEP::cm * RAYFF, 50445.6 * CLHEP::cm * RAYFF, 45937 * CLHEP::cm * RAYFF, 41735.2 * CLHEP::cm * RAYFF, 37826.6 * CLHEP::cm * RAYFF, 34197.6 * CLHEP::cm * RAYFF, 30834.9 * CLHEP::cm * RAYFF, 27725.4 * CLHEP::cm * RAYFF, 24856.6 * CLHEP::cm * RAYFF, 22215.9 * CLHEP::cm * RAYFF,
												 19791.3 * CLHEP::cm * RAYFF, 17570.9 * CLHEP::cm * RAYFF, 15543 * CLHEP::cm * RAYFF, 13696.6 * CLHEP::cm * RAYFF, 12020.5 * CLHEP::cm * RAYFF,
												 10504.1 * CLHEP::cm * RAYFF, 9137.15 * CLHEP::cm * RAYFF, 7909.45 * CLHEP::cm * RAYFF, 6811.3 * CLHEP::cm * RAYFF, 5833.25 * CLHEP::cm * RAYFF,
												 4966.2 * CLHEP::cm * RAYFF, 4201.36 * CLHEP::cm * RAYFF, 3530.28 * CLHEP::cm * RAYFF, 2944.84 * CLHEP::cm * RAYFF, 2437.28 * CLHEP::cm * RAYFF,
												 2000.18 * CLHEP::cm * RAYFF, 1626.5 * CLHEP::cm * RAYFF, 1309.55 * CLHEP::cm * RAYFF, 1043.03 * CLHEP::cm * RAYFF, 821.016 * CLHEP::cm * RAYFF,
												 637.97 * CLHEP::cm * RAYFF, 488.754 * CLHEP::cm * RAYFF};

	// Get from the tuning parameters
	G4double MIEFF = (WCSimTuningParameters::Instance())->GetMieff();
	//G4double MIEFF = 0.0;
	//    printf("MIEFF: %f\n",MIEFF);

	//Values extracted from Skdetsim
	G4double MIE_water[NUMENTRIES_water] = {7790020 * CLHEP::cm * MIEFF, 7403010 * CLHEP::cm * MIEFF, 7030610 * CLHEP::cm * MIEFF, 6672440 * CLHEP::cm * MIEFF, 6328120 * CLHEP::cm * MIEFF, 5997320 * CLHEP::cm * MIEFF, 5679650 * CLHEP::cm * MIEFF, 5374770 * CLHEP::cm * MIEFF,
											5082340 * CLHEP::cm * MIEFF, 4802000 * CLHEP::cm * MIEFF, 4533420 * CLHEP::cm * MIEFF, 4276280 * CLHEP::cm * MIEFF, 4030220 * CLHEP::cm * MIEFF, 3794950 * CLHEP::cm * MIEFF, 3570120 * CLHEP::cm * MIEFF, 3355440 * CLHEP::cm * MIEFF, 3150590 * CLHEP::cm * MIEFF,
											2955270 * CLHEP::cm * MIEFF, 2769170 * CLHEP::cm * MIEFF, 2592000 * CLHEP::cm * MIEFF, 2423470 * CLHEP::cm * MIEFF, 2263300 * CLHEP::cm * MIEFF, 2111200 * CLHEP::cm * MIEFF, 1966900 * CLHEP::cm * MIEFF, 1830120 * CLHEP::cm * MIEFF, 1700610 * CLHEP::cm * MIEFF,
											1578100 * CLHEP::cm * MIEFF, 1462320 * CLHEP::cm * MIEFF, 1353040 * CLHEP::cm * MIEFF, 1250000 * CLHEP::cm * MIEFF, 1152960 * CLHEP::cm * MIEFF, 1061680 * CLHEP::cm * MIEFF, 975936 * CLHEP::cm * MIEFF, 895491 * CLHEP::cm * MIEFF, 820125 * CLHEP::cm * MIEFF, 749619 * CLHEP::cm * MIEFF, 683760 * CLHEP::cm * MIEFF, 622339 * CLHEP::cm * MIEFF, 565152 * CLHEP::cm * MIEFF, 512000 * CLHEP::cm * MIEFF,
											462688 * CLHEP::cm * MIEFF, 417027 * CLHEP::cm * MIEFF, 374832 * CLHEP::cm * MIEFF, 335923 * CLHEP::cm * MIEFF, 300125 * CLHEP::cm * MIEFF,
											267267 * CLHEP::cm * MIEFF, 237184 * CLHEP::cm * MIEFF, 209715 * CLHEP::cm * MIEFF, 184704 * CLHEP::cm * MIEFF, 162000 * CLHEP::cm * MIEFF,
											141456 * CLHEP::cm * MIEFF, 122931 * CLHEP::cm * MIEFF, 106288 * CLHEP::cm * MIEFF, 91395.2 * CLHEP::cm * MIEFF, 78125 * CLHEP::cm * MIEFF,
											66355.2 * CLHEP::cm * MIEFF, 55968.2 * CLHEP::cm * MIEFF, 46851.2 * CLHEP::cm * MIEFF, 38896.2 * CLHEP::cm * MIEFF, 32000 * CLHEP::cm * MIEFF};

	G4double MIE_water_const[3] = {0.4, 0., 1}; // gforward, gbackward, forward backward ratio

	// M Fechner : unphysical, I want to reduce reflections
	// back to the old value 1.55

	G4double RINDEX_glass[NUMENTRIES_water] = {1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600,
											   1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600,
											   1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600,
											   1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600,
											   1.600, 1.600, 1.600, 1.600, 1.600};

	// M Fechner : the quantum efficiency already takes glass abs into account

	G4double ABSORPTION_glass[NUMENTRIES_water] = {1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm, 1.0e9 * CLHEP::cm};

	G4double BLACKABS_blacksheet[NUMENTRIES_water] = {1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm,
													  1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm,
													  1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm,
													  1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm,
													  1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm,
													  1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm,
													  1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm,
													  1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm};

	//July 1, 2010, F. Beroz: changed SK1SK2FF to BSRFF to avoid confusion,
	// since this parameter is not from SK.

	G4double BSRFF = 1.0;

	// Get from the tuning parameters
	BSRFF = (WCSimTuningParameters::Instance())->GetBsrff();

	G4double REFLECTIVITY_blacksheet[NUMENTRIES_water] = {0.055 * BSRFF, 0.055 * BSRFF, 0.055 * BSRFF, 0.055 * BSRFF,
														  0.055 * BSRFF, 0.055 * BSRFF, 0.055 * BSRFF, 0.055 * BSRFF, 0.055 * BSRFF, 0.055 * BSRFF, 0.055 * BSRFF,
														  0.055 * BSRFF, 0.055 * BSRFF, 0.055 * BSRFF, 0.055 * BSRFF, 0.055 * BSRFF, 0.055 * BSRFF, 0.055 * BSRFF,
														  0.055 * BSRFF, 0.055 * BSRFF, 0.055 * BSRFF, 0.055 * BSRFF, 0.055 * BSRFF, 0.055 * BSRFF, 0.055 * BSRFF,
														  0.055 * BSRFF, 0.055 * BSRFF, 0.055 * BSRFF, 0.055 * BSRFF, 0.055 * BSRFF, 0.055 * BSRFF, 0.057 * BSRFF,
														  0.059 * BSRFF, 0.060 * BSRFF, 0.059 * BSRFF, 0.058 * BSRFF, 0.057 * BSRFF, 0.055 * BSRFF, 0.050 * BSRFF,
														  0.045 * BSRFF, 0.045 * BSRFF, 0.045 * BSRFF, 0.045 * BSRFF, 0.045 * BSRFF, 0.045 * BSRFF, 0.045 * BSRFF,
														  0.045 * BSRFF, 0.045 * BSRFF, 0.045 * BSRFF, 0.045 * BSRFF, 0.045 * BSRFF, 0.045 * BSRFF, 0.045 * BSRFF,
														  0.045 * BSRFF, 0.045 * BSRFF, 0.045 * BSRFF, 0.045 * BSRFF, 0.045 * BSRFF, 0.045 * BSRFF, 0.045 * BSRFF};

	G4double REFLECTIVITY_whitesheet[NUMENTRIES_water] = {0};
	for (int i = 0; i < NUMENTRIES_water; ++i)
	{
		REFLECTIVITY_whitesheet[i] = 0.9; // Make it shiny!
	}

	//utter fiction at this stage
	G4double EFFICIENCY[NUMENTRIES_water] = {0.001 * CLHEP::m};

	//utter fiction at this stage, does not matter
	G4double RAYLEIGH_air[NUMENTRIES_water] = {0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m,
											   0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m, 0.001 * CLHEP::m};

	//utter fiction at this stage, does not matter
	G4double MIE_air[NUMENTRIES_water] = {0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m, 0.1 * CLHEP::m};

	G4double MIE_air_const[3] = {0.99, 0.99, 0.8}; // gforward, gbackward, forward backward ratio

	G4double EFFICIENCY_blacksheet[NUMENTRIES_water] = {0.0};

	// Water - make them all the same for now

	G4MaterialPropertiesTable *myMPT1 = new G4MaterialPropertiesTable();
	// M Fechner : new   ; wider range for lambda
	myMPT1->AddProperty("RINDEX", ENERGY_water, RINDEX1, NUMENTRIES_water);
	myMPT1->AddProperty("ABSLENGTH", ENERGY_water, ABSORPTION_water, NUMENTRIES_water);
	// M Fechner: new, don't let G4 compute it.
	myMPT1->AddProperty("RAYLEIGH", ENERGY_water, RAYLEIGH_water, NUMENTRIES_water);

	//  myMPT1->AddProperty("MIEHG",ENERGY_water,MIE_water,NUMENTRIES_water);
	//    myMPT1->AddConstProperty("MIEHG_FORWARD",MIE_water_const[0]);
	//    myMPT1->AddConstProperty("MIEHG_BACKWARD",MIE_water_const[1]);
	//    myMPT1->AddConstProperty("MIEHG_FORWARD_RATIO",MIE_water_const[2]);

	GetMaterial("Water")->SetMaterialPropertiesTable(myMPT1);
	// Leigh: For now, make pit water the same as the pure water
	GetMaterial("PitWater")->SetMaterialPropertiesTable(myMPT1);
	//Gd doped water has the same optical properties as pure water
	GetMaterial("DopedWater")->SetMaterialPropertiesTable(myMPT1);
	// myMPT1->DumpTable();

	// Air
	////////////////////////////////
	G4MaterialPropertiesTable *myMPT2 = new G4MaterialPropertiesTable();
	myMPT2->AddProperty("RINDEX", ENERGY_water, RINDEX_air, NUMENTRIES_water);
	// M Fechner : what is that ?????
	myMPT2->AddProperty("ABSLENGTH", ENERGY_water, BLACKABS_blacksheet, NUMENTRIES_water);
	myMPT2->AddProperty("RAYLEIGH", ENERGY_water, RAYLEIGH_air, NUMENTRIES_water);

	// myMPT2->AddProperty("MIEHG",ENERGY_water,MIE_air,NUMENTRIES_water);
	//       myMPT2->AddConstProperty("MIEHG_FORWARD",MIE_air_const[0]);
	//       myMPT2->AddConstProperty("MIEHG_BACKWARD",MIE_air_const[1]);
	//       myMPT2->AddConstProperty("MIEHG_FORWARD_RATIO",MIE_air_const[2]);

	GetMaterial("Air")->SetMaterialPropertiesTable(myMPT2);

	// Plastic
	////////////////////

	G4MaterialPropertiesTable *myMPT3 = new G4MaterialPropertiesTable();
	myMPT3->AddProperty("ABSLENGTH", ENERGY_water, BLACKABS_blacksheet, NUMENTRIES_water);
	myMPT3->AddProperty("REFLECTIVITY", ENERGY_water, REFLECTIVITY_blacksheet, NUMENTRIES_water);
	myMPT3->AddProperty("EFFICIENCY", ENERGY_water, EFFICIENCY, NUMENTRIES_water);
	GetMaterial("Plastic")->SetMaterialPropertiesTable(myMPT3);

	// Set PVC with same properties as Plastic, Water/PVC optical surface defined below
	GetMaterial("PVC")->SetMaterialPropertiesTable(myMPT3);

	// Blacksheet
	/////////////////////

	G4MaterialPropertiesTable *myMPT4 = new G4MaterialPropertiesTable();
	myMPT4->AddProperty("ABSLENGTH", ENERGY_water, BLACKABS_blacksheet, NUMENTRIES_water);
	GetMaterial("Blacksheet")->SetMaterialPropertiesTable(myMPT4);

	// Whitesheet
	G4MaterialPropertiesTable *whiteMPT1 = new G4MaterialPropertiesTable();
	whiteMPT1->AddProperty("ABSLENGTH", ENERGY_water, BLACKABS_blacksheet, NUMENTRIES_water);
	GetMaterial("Whitesheet")->SetMaterialPropertiesTable(whiteMPT1);

	// Glass
	////////////////

	G4MaterialPropertiesTable *myMPT5 = new G4MaterialPropertiesTable();
	myMPT5->AddProperty("RINDEX", ENERGY_water, RINDEX_glass, NUMENTRIES_water);
	myMPT5->AddProperty("ABSLENGTH", ENERGY_water, ABSORPTION_glass, NUMENTRIES_water);
	GetMaterial("Glass")->SetMaterialPropertiesTable(myMPT5);

	// Tyvek
	////////////////

	// jl145 ----
	// Abs legnth is same as blacksheet, very small.
	G4MaterialPropertiesTable *myMPT6 = new G4MaterialPropertiesTable();
	myMPT6->AddProperty("ABSLENGTH", ENERGY_water, BLACKABS_blacksheet, NUMENTRIES_water);
	GetMaterial("Tyvek")->SetMaterialPropertiesTable(myMPT6);

	// SURFACES
	//	------------- Surfaces --------------

	// Blacksheet
	const G4int NUM = 2;
	//   G4double PP[NUM] =
	//{ 2.038E-9*CLHEP::GeV, 4.144E-9*CLHEP::GeV };

	G4double PP[NUM] = {1.4E-9 * CLHEP::GeV, 6.2E-9 * CLHEP::GeV};
	G4double RINDEX_blacksheet[NUM] = {1.6, 1.6};

	G4double SPECULARLOBECONSTANT[NUM] = {0.3, 0.3};
	G4double SPECULARSPIKECONSTANT[NUM] = {0.2, 0.2};
	G4double BACKSCATTERCONSTANT[NUM] = {0.2, 0.2};
	G4MaterialPropertiesTable *myST1 = new G4MaterialPropertiesTable();
	myST1->AddProperty("RINDEX", ENERGY_water, RINDEX_blacksheet, NUMENTRIES_water);
	myST1->AddProperty("SPECULARLOBECONSTANT", PP, SPECULARLOBECONSTANT, NUM);
	myST1->AddProperty("SPECULARSPIKECONSTANT", PP, SPECULARSPIKECONSTANT, NUM);
	myST1->AddProperty("BACKSCATTERCONSTANT", PP, BACKSCATTERCONSTANT, NUM);
	myST1->AddProperty("REFLECTIVITY", ENERGY_water, REFLECTIVITY_blacksheet, NUMENTRIES_water);
	myST1->AddProperty("EFFICIENCY", ENERGY_water, EFFICIENCY_blacksheet, NUMENTRIES_water);
	fOpWaterBSSurface->SetMaterialPropertiesTable(myST1);

	// Whitesheet
	// Mostly the same as blacksheet, just shinier.
	G4MaterialPropertiesTable *whiteMPT = new G4MaterialPropertiesTable();
	whiteMPT->AddProperty("RINDEX", ENERGY_water, RINDEX_blacksheet, NUMENTRIES_water);
	whiteMPT->AddProperty("SPECULARLOBECONSTANT", PP, SPECULARLOBECONSTANT, NUM);
	whiteMPT->AddProperty("SPECULARSPIKECONSTANT", PP, SPECULARSPIKECONSTANT, NUM);
	whiteMPT->AddProperty("BACKSCATTERCONSTANT", PP, BACKSCATTERCONSTANT, NUM);
	whiteMPT->AddProperty("REFLECTIVITY", ENERGY_water, REFLECTIVITY_whitesheet, NUMENTRIES_water);
	whiteMPT->AddProperty("EFFICIENCY", ENERGY_water, EFFICIENCY_blacksheet, NUMENTRIES_water);
	fOpWaterWSSurface->SetMaterialPropertiesTable(whiteMPT);

	//Glass to Cathode surface inside PMTs
	G4double RINDEX_cathode[NUM] = {1.0, 1.0};

	G4double RGCFF = 0.0;
	RGCFF = (WCSimTuningParameters::Instance())->GetRgcff();

	G4double REFLECTIVITY_glasscath[NUM] = {1.0 * RGCFF, 1.0 * RGCFF};

	G4double EFFICIENCY_glasscath[NUM] = {0.0, 0.0};
	G4MaterialPropertiesTable *myST2 = new G4MaterialPropertiesTable();
	myST2->AddProperty("RINDEX", PP, RINDEX_cathode, NUM);
	//   myST2->AddProperty("SPECULARLOBECONSTANT", PP, SPECULARLOBECONSTANT_glasscath, NUM);
	//   myST2->AddProperty("SPECULARSPIKECONSTANT", PP, SPECULARSPIKECONSTANT_glasscath, NUM);
	//   myST2->AddProperty("BACKSCATTERCONSTANT", PP, BACKSCATTERCONSTANT_glasscath, NUM);
	myST2->AddProperty("REFLECTIVITY", PP, REFLECTIVITY_glasscath, NUM);
	myST2->AddProperty("EFFICIENCY", PP, EFFICIENCY_glasscath, NUM);
	//myST2->AddProperty("ABSLENGTH", PP, abslength_paint , NUM);
	fOpGlassCathodeSurface->SetMaterialPropertiesTable(myST2);

	//Tyvek - jl145
	G4double RINDEX_tyvek[NUM] = {1.5, 1.5};			 // polyethylene permittivity is ~2.25
	G4double TySPECULARLOBECONSTANT[NUM] = {0.75, 0.75}; // crudely estimated from A. Chavarria's thesis
	G4double TySPECULARSPIKECONSTANT[NUM] = {0.0, 0.0};
	G4double TyBACKSCATTERCONSTANT[NUM] = {0.0, 0.0};
	// Lambertian prob is therefore 0.25
	G4double TyREFLECTIVITY[NUM] = {0.94, 0.94}; //cf. DuPont
	G4MaterialPropertiesTable *myST3 = new G4MaterialPropertiesTable();
	myST3->AddProperty("RINDEX", PP, RINDEX_tyvek, NUM);
	myST3->AddProperty("SPECULARLOBECONSTANT", PP, TySPECULARLOBECONSTANT, NUM);
	myST3->AddProperty("SPECULARSPIKECONSTANT", PP, TySPECULARSPIKECONSTANT, NUM);
	myST3->AddProperty("BACKSCATTERCONSTANT", PP, TyBACKSCATTERCONSTANT, NUM);
	myST3->AddProperty("REFLECTIVITY", PP, TyREFLECTIVITY, NUM);
	myST3->AddProperty("EFFICIENCY", PP, EFFICIENCY_blacksheet, NUM);
	//use same efficiency as blacksheet, which is 0
	fOpWaterTySurface->SetMaterialPropertiesTable(myST3);

	// Light Collector Reflecting Surface
	G4double ENERGY_LC[NUM] = {1.7e-9 * CLHEP::GeV, 6.2e-9 * CLHEP::GeV};
	G4double LCREREFLECTIVITY[NUM] = {0.95, 0.95}; // Constant for the moment (probably needs some refinemnet)
	G4MaterialPropertiesTable *myST4 = new G4MaterialPropertiesTable();
	myST4->AddProperty("REFLECTIVITY", ENERGY_LC, LCREREFLECTIVITY, NUM);
	fOpWaterLCinSurface->SetMaterialPropertiesTable(myST4);

	// Give Light Collector Outer Surface same properties as Blak Sheet (myST1)
	fOpWaterLCoutSurface->SetMaterialPropertiesTable(myST1);

	G4double ENERGY_PVC[NUM] = {1.7e-9 * CLHEP::GeV, 6.2e-9 * CLHEP::GeV};
	G4double PVCREFLECTIVITY[NUM] = {0.75, 0.75}; // Constant for the moment (probably needs some refinemnet)
	///G4double PVCREFLECTIVITY[NUM]      = { 0.15, 0.15 }; // Constant for the moment (probably needs some refinemnet)  || form Karan Measurmensts
	G4MaterialPropertiesTable *myST5 = new G4MaterialPropertiesTable();
	myST5->AddProperty("REFLECTIVITY", ENERGY_PVC, PVCREFLECTIVITY, NUM);
	fOpWaterPlanePipeSurface->SetMaterialPropertiesTable(myST5);

	G4double ENERGY_PMTBACK[NUM] = {1.7e-9 * CLHEP::GeV, 6.2e-9 * CLHEP::GeV};
	G4double PMTBACKREFLECTIVITY[NUM] = {0.0, 0.0};
	G4double PMTBACKABSLEN[NUM] = {1.0e-9 * CLHEP::cm, 1.0e-9 * CLHEP::cm};
	G4MaterialPropertiesTable *myST6 = new G4MaterialPropertiesTable();
	myST6->AddProperty("REFLECTIVITY", ENERGY_PMTBACK, PMTBACKREFLECTIVITY, NUM);
	myST6->AddProperty("ABSLENGTH", ENERGY_PMTBACK, PMTBACKABSLEN, NUM);
	fOpWaterPMTBackSurface->SetMaterialPropertiesTable(myST6);
}

G4OpticalSurface *WCSimMaterialsBuilder::GetOpticalSurface(const G4String &name) const
{
	G4OpticalSurface *surf = NULL;
	std::map<G4String, G4OpticalSurface *>::const_iterator mapItr = fOpticalSurfaces.find(name);
	if (mapItr != fOpticalSurfaces.end())
	{
		surf = (*mapItr).second;
	}
	else
	{
		std::cerr << "Could not find surface " << name << std::endl;
		assert(mapItr != fOpticalSurfaces.end());
	}
	return surf;
}

void WCSimMaterialsBuilder::BuildSurfacePropertiesTable()
{
	// Should move the surface properties here...
}

void WCSimMaterialsBuilder::BuildSurfaces()
{

	//	------------- Surfaces --------------
	if (fOpWaterBSSurface == NULL)
	{
		G4String name = "WaterBSCellSurface";
		fOpWaterBSSurface = new G4OpticalSurface(name);
		fOpWaterBSSurface->SetType(dielectric_dielectric);
		fOpWaterBSSurface->SetModel(unified);
		fOpWaterBSSurface->SetFinish(groundfrontpainted);
		fOpWaterBSSurface->SetSigmaAlpha(0.1);
		fOpticalSurfaces[name] = fOpWaterBSSurface;
	}

	if (fOpWaterWSSurface == NULL)
	{
		G4String name = "WaterWSCellSurface";
		fOpWaterWSSurface = new G4OpticalSurface(name);
		fOpWaterWSSurface->SetType(dielectric_dielectric);
		fOpWaterWSSurface->SetModel(unified);
		fOpWaterWSSurface->SetFinish(groundfrontpainted);
		fOpWaterWSSurface->SetSigmaAlpha(0.1);
		fOpticalSurfaces[name] = fOpWaterWSSurface;
	}

	if (fOpGlassCathodeSurface == NULL)
	{
		G4String name = "GlassCathodeSurface";
		fOpGlassCathodeSurface = new G4OpticalSurface(name);
		fOpGlassCathodeSurface->SetType(dielectric_dielectric);
		fOpGlassCathodeSurface->SetModel(unified);
		//   fOpGlassCathodeSurface->SetFinish(groundbackpainted);
		fOpGlassCathodeSurface->SetFinish(polished);
		//fOpGlassCathodeSurface->SetSigmaAlpha(0.002);
		// was 1.0
		// totally unphysical anyway
		fOpticalSurfaces[name] = fOpGlassCathodeSurface;
	}

	// jl145 ----
	//
	if (fOpWaterTySurface == NULL)
	{
		G4String name = "WaterTyCellSurface";
		fOpWaterTySurface = new G4OpticalSurface(name);
		fOpWaterTySurface->SetType(dielectric_dielectric);
		fOpWaterTySurface->SetModel(unified);
		fOpWaterTySurface->SetFinish(groundbackpainted); //a guess, but seems to work
		fOpWaterTySurface->SetSigmaAlpha(0.5);			 //cf. A. Chavarria's ~30deg
		fOpticalSurfaces[name] = fOpWaterTySurface;
	}

	// --------- Light Collector Reflecting Surface
	if (fOpWaterLCinSurface == NULL)
	{
		G4String name = "WaterLCinSurface";

		fOpWaterLCinSurface = new G4OpticalSurface(name);
		fOpWaterLCinSurface->SetType(dielectric_metal);
		fOpWaterLCinSurface->SetModel(unified);
		fOpWaterLCinSurface->SetFinish(polished);
		fOpWaterLCinSurface->SetSigmaAlpha(0.01); // Guessed!!! To be checked
		fOpticalSurfaces[name] = fOpWaterLCinSurface;
	}

	// --------- Light Collector Outer Surface
	if (fOpWaterLCoutSurface == NULL)
	{
		G4String name = "WaterLCoutSurface";

		fOpWaterLCoutSurface = new G4OpticalSurface(name);
		fOpWaterLCoutSurface->SetType(dielectric_dielectric);
		fOpWaterLCoutSurface->SetModel(unified);
		fOpWaterLCoutSurface->SetFinish(groundfrontpainted);
		fOpWaterLCoutSurface->SetSigmaAlpha(0.1);
		fOpticalSurfaces[name] = fOpWaterLCoutSurface;
	}

	// --------- Plane Pipes  Surface
	if (fOpWaterPlanePipeSurface == NULL)
	{

		G4String name = "WaterPlanePipeSurface";

		fOpWaterPlanePipeSurface = new G4OpticalSurface(name);
		fOpWaterPlanePipeSurface->SetType(dielectric_dielectric);
		fOpWaterPlanePipeSurface->SetModel(unified);
		fOpWaterPlanePipeSurface->SetFinish(groundfrontpainted);
		fOpWaterPlanePipeSurface->SetSigmaAlpha(0.1);
		fOpticalSurfaces[name] = fOpWaterPlanePipeSurface;
	}

	// --------- PMT Back  Surface
	if (fOpWaterPMTBackSurface == NULL)
	{

		G4String name = "WaterPMTBackSurface";

		fOpWaterPMTBackSurface = new G4OpticalSurface(name);
		fOpWaterPMTBackSurface->SetType(dielectric_dielectric);
		fOpWaterPMTBackSurface->SetModel(unified);
		fOpWaterPMTBackSurface->SetFinish(groundfrontpainted);
		fOpWaterPMTBackSurface->SetSigmaAlpha(0.1);
		fOpticalSurfaces[name] = fOpWaterPMTBackSurface;
	}
}
