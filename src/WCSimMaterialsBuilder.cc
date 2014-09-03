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
#include "G4UnitsTable.hh"
#include <cassert>

static WCSimMaterialsBuilder* fgMaterialsBuilder = 0;

WCSimMaterialsBuilder::WCSimMaterialsBuilder() {
	BuildVacuum();
	BuildElements();
	BuildMaterials();
	BuildMaterialPropertiesTable();

}

WCSimMaterialsBuilder::~WCSimMaterialsBuilder() {
	// TODO Auto-generated destructor stub
}

static WCSimMaterialsBuilder* WCSimMaterialsBuilder::Instance() {
	if (!fgMaterialsBuilder) {
		fgMaterialsBuilder = new WCSimMaterialsBuilder();
	}

	if (!fgMaterialsBuilder) {
		assert(fgMaterialsBuilder);
	}

	return fgMaterialsBuilder;
}

void WCSimMaterialsBuilder::BuildVacuum() {
	G4double density;
	G4double a;

	// The vacuum
	density = universe_mean_density;              //from PhysicalConstants.h
	G4double pressure = 1.e-19 * pascal;
	G4double temperature = 0.1 * kelvin;
	a = 1.01 * g / mole;
	G4Material* Vacuum = new G4Material("Vacuum", 1., a, density, kStateGas,
			temperature, pressure);
}

void WCSimMaterialsBuilder::BuildElements() {
	G4double a = 0.0;

	// == Elements ==
	a = 1.01 * g / mole;
	G4String name = "Hydrogen";
	G4Element* elH = new G4Element(name, "H", 1, a);
	fElements.push_back(name);

	a = 10.81 * g / mole;
	name = "Boron";
	G4Element* elB = new G4Element(name, "B", 5, a);
	fElements.push_back(name);

	a = 12.01 * g / mole;
	name = "Carbon";
	G4Element* elC = new G4Element(name, "C", 6, a);
	fElements.push_back(name);

	a = 14.01 * g / mole;
	name = "Nitrogen";
	G4Element* elN = new G4Element(name, "N", 7, a);
	fElements.push_back(name);

	a = 16.00 * g / mole;
	name = "Oxygen";
	G4Element* elO = new G4Element(name, "O", 8, a);
	fElements.push_back(name);

	a = 22.99 * g / mole;
	name = "Sodium";
	G4Element* elNa = new G4Element(name, "Na", 11, a);
	fElements.push_back(name);

	a = 24.30 * g / mole;
	name = "Magnesium";
	G4Element* elMg = new G4Element(name, "Mg", 12, a);
	fElements.push_back(name);

	a = 26.98 * g / mole;
	name = "Aluminum";
	G4Element* elAl = new G4Element(name, "Al", 13, a);
	fElements.push_back(name);

	a = 28.09 * g / mole;
	name = "Silicon";
	G4Element* elSi = new G4Element(name, "Si", 14., a);
	fElements.push_back(name);

	a = 30.974 * g / mole;
	name = "Phosphore";
	G4Element* elP = new G4Element(name, "P", 15., a);
	fElements.push_back(name);

	a = 32.065 * g / mole;
	name = "Sulphur";
	G4Element* elS = new G4Element(name, "S", 16., a);
	fElements.push_back(name);

	a = 39.948 * g / mole;
	name = "Argon";
	G4Element* elAr = new G4Element(name, "Ar", 18., a);
	fElements.push_back(name);

	a = 39.10 * g / mole;
	name = "Potassium";
	G4Element* elK = new G4Element(name, "K", 19, a);
	fElements.push_back(name);

	a = 40.08 * g / mole;
	name = "Calcium";
	G4Element* elCa = new G4Element(name, "Ca", 20, a);
	fElements.push_back(name);

	a = 51.9961 * g / mole;
	name = "Chromium";
	G4Element* elCr = new G4Element(name, "Cr", 24., a);
	fElements.push_back(name);

	a = 54.938 * g / mole;
	name = "Manganese";
	G4Element* elMn = new G4Element(name, "Mn", 25., a);
	fElements.push_back(name);

	a = 55.85 * g / mole;
	name = "Iron";
	G4Element* elFe = new G4Element(name, "Fe", 26, a);
	fElements.push_back(name);

	a = 58.6934 * g / mole;
	name = "Nickel";
	G4Element* elNi = new G4Element(name, "Ni", 28., a);
	fElements.push_back(name);

	a = 157.25 * g / mole;
	name = "Gadolinium";
	G4Element* Gd = new G4Element(name, "Gd", 64, a);
	fElements.push_back(name);
	// -----------------------
}

void WCSimMaterialsBuilder::BuildMaterials() {
	G4double a = 0.0;
	G4double density = 0.0;
	G4String name = "";

	// == Materials ==

	density = 1.00 * g / cm3;
	name = "Water";
	G4Material* Water = new G4Material("Water", density, 2);
	Water->AddElement(GetElement("Hydrogen"), 2);
	Water->AddElement(GetElement("Oxygen"), 1);
	fMaterials.push_back(name);

	density = 1.00 * g / cm;
	name = "PitWater";
	G4Material* PitWater = new G4Material("PitWater", density, 2);
	PitWater->AddElement(GetElement("Hydrogen"), 2);
	PitWater->AddElement(GetElement("Oxygen"), 1);
	fMaterials.push_back(name);

	density = 1.00 * g / cm3;
	name = "Doped Water";
	G4Material* DopedWater = new G4Material("Doped Water", density, 2);
	DopedWater->AddMaterial(Water, 99.9 * perCent);
	DopedWater->AddElement(GetElement("Gadolinium"), 0.1 * perCent);
	fMaterials.push_back(name);

	density = 0.92 * g / cm3;
	name = "Ice";
	G4Material* Ice = new G4Material("Ice", density, 2);
	Ice->AddElement(GetElement("Hydrogen"), 2);
	Ice->AddElement(GetElement("Oxygen"), 1);
	fMaterials.push_back(name);

	density = 7.8 * g / cm3;
	name = "Steel";
	G4Material* Steel = new G4Material("Steel", density, 2);
	Steel->AddElement(GetElement("Carbon"), 1. * perCent);
	Steel->AddElement(GetElement("Iron"), 99. * perCent);
	fMaterials.push_back(name);

	// Stainless steel 304L
	density = 7.81 * g / cm3;
	name = "StainlessSteel";
	G4Material* StainlessSteel = new G4Material("StainlessSteel", density, 8);
	StainlessSteel->AddElement(GetElement("Iron"), 70.44 * perCent);
	StainlessSteel->AddElement(GetElement("Chromium"), 18 * perCent);
	StainlessSteel->AddElement(GetElement("Carbon"), 0.08 * perCent);
	StainlessSteel->AddElement(GetElement("Nickel"), 8 * perCent);
	StainlessSteel->AddElement(GetElement("Phosphore"), 0.45 * perCent);
	StainlessSteel->AddElement(GetElement("Silicon"), 1 * perCent);
	StainlessSteel->AddElement(GetElement("Manganese"), 2 * perCent);
	StainlessSteel->AddElement(GetElement("Sulphur"), 0.03 * perCent);
	fMaterials.push_back(name);

	density = 1.563 * g / cm3;
	name = "SolidDryIce";
	G4Material* DryIce = new G4Material("SolidDryIce", density, 2);
	DryIce->AddElement(GetElement("Carbon"), 1);
	DryIce->AddElement(GetElement("Oxygend"), 2);
	fMaterials.push_back(name);

	density = 1.290 * mg / cm3;
	name = "Air";
	G4Material* Air = new G4Material("Air", density, 3);
	Air->AddElement(GetElement("Nitrogen"), 78. * perCent);
	Air->AddElement(GetElement("Oxygen"), 21. * perCent);
	Air->AddElement(GetElement("Argon"), 1. * perCent);
	fMaterials.push_back(name);

	density = 0.95 * g / cm3;
	name = "Plastic";
	G4Material* Plastic = new G4Material("Plastic", density, 2);
	Plastic->AddElement(GetElement("Carbon"), 1);
	Plastic->AddElement(GetElement("Hydrogen"), 2);
	fMaterials.push_back(name);

	density = 2.7 * g / cm3;
	name = "Aluminum";
	G4Material* Aluminum = new G4Material("Aluminum", density, 1);
	Aluminum->AddElement(GetElement("Aluminum"), 1);
	fMaterials.push_back(name);

	density = 0.95 * g / cm3;
	name = "Blacksheet";
	G4Material* Blacksheet = new G4Material("Blacksheet", density, 2);
	Blacksheet->AddElement(GetElement("Carbon"), 1);
	Blacksheet->AddElement(GetElement("Hydrogen"), 2);
	fMaterials.push_back(name);

	density = 0.38 * g / cm3;  //cf. DuPont product handbook
	name = "Tyvek";
	G4Material* Tyvek = new G4Material("Tyvek", density, 2);
	Tyvek->AddElement(GetElement("Carbon"), 1);  //polyethylene
	Tyvek->AddElement(GetElement("Oxygen"), 2);
	fMaterials.push_back(name);

	// Components for making glass
	density = 2.20 * g / cm3;
	name = "SiO2";
	G4Material* SiO2 = new G4Material("SiO2", density, 2);
	SiO2->AddElement(GetElement("Silicon"), 1);
	SiO2->AddElement(GetElement("Oxygen"), 2);
	fMaterials.push_back(name);

	density = 2.46 * g / cm3;
	name = "B2O3";
	G4Material* B2O3 = new G4Material("B2O3", density, 2);
	B2O3->AddElement(GetElement("Boron"), 2);
	B2O3->AddElement(GetElement("Oxygen"), 3);
	fMaterials.push_back(name);

	density = 2.27 * g / cm3;
	name = "Na20";
	G4Material* Na2O = new G4Material("Na2O", density, 2);
	Na2O->AddElement(GetElement("Sodium"), 2);
	Na2O->AddElement(GetElement("Oxygen"), 1);
	fMaterials.push_back(name);

	density = 4.00 * g / cm3;
	name = "Al203";
	G4Material* Al2O3 = new G4Material("Al2O3", density, 2);
	Al2O3->AddElement(GetElement("Aluminum"), 2);
	Al2O3->AddElement(GetElement("Oxygen"), 3);
	fMaterials.push_back(name);

	density = 2.23 * g / cm3;
	name = "Glass";
	G4Material* Glass = new G4Material("Glass", density, 4);
	Glass->AddMaterial(GetMaterial("SiO2"), 80.6 * perCent);
	Glass->AddMaterial(GetMaterial("B2O3"), 13.0 * perCent);
	Glass->AddMaterial(GetMaterial("Na2O"), 4.0 * perCent);
	Glass->AddMaterial(GetMaterial("Al2O3"), 2.4 * perCent);
	fMaterials.push_back(name);

	density = 2.7 * g / cm3;
	name = "Rock";
	G4Material* Rock = new G4Material("Rock", density, 8);

	//From Daya-Bay
	Rock->AddElement(GetElement("Oxygen"), 48.50 * perCent);
	Rock->AddElement(GetElement("Silicon"), 34.30 * perCent);
	Rock->AddElement(GetElement("Aluminum"), 8.00 * perCent);
	Rock->AddElement(GetElement("Iron"), 2.00 * perCent);
	Rock->AddElement(GetElement("Calcium"), 0.20 * perCent);
	Rock->AddElement(GetElement("Sodium"), 2.40 * perCent);
	Rock->AddElement(GetElement("Potassium"), 4.50 * perCent);
	Rock->AddElement(GetElement("Magnesium"), 0.10 * perCent);
	fMaterials.push_back(name);
}

G4Material* WCSimMaterialsBuilder::GetVacuum(const G4String& name) const {
	return G4Material::GetMaterial("name");
}

G4Element* WCSimMaterialsBuilder::GetElement(const G4String& name) const {
	assert(
			std::find(fElements.begin(), fElements.end(), name) != fElements.end());
	return G4Material::GetMaterial(name);
}

G4Material* WCSimMaterialsBuilder::GetMaterial(const G4String& name) const {
	assert(
			std::find(fMaterials.begin(), fMaterials.end(), name) != fMaterials.end());
	return G4Material::GetMaterial(name);
}

void WCSimMaterialsBuilder::BuildMaterialPropertiesTable() {
	// -------------------------------------------------------------
	// Generate & Add Material Properties Table
	// -------------------------------------------------------------

	G4MaterialPropertiesTable *mpt = new G4MaterialPropertiesTable();

	const G4int nEntries = 2;
	G4double photonEnergy[nEntries] = { 1. * eV, 7. * eV };

	// Stainless steel
	G4double abslength_Steel[nEntries] = { .001 * mm, .001 * mm };
	mpt->AddProperty("ABSLENGTH", photonEnergy, abslength_Steel, nEntries);
	GetMaterial("StainlessSteel")->SetMaterialPropertiesTable(mpt);

	const G4int NUMENTRIES = 32;
	G4double PPCKOV[NUMENTRIES] = { 2.034E-9 * GeV, 2.068E-9 * GeV, 2.103E-9
			* GeV, 2.139E-9 * GeV, 2.177E-9 * GeV, 2.216E-9 * GeV, 2.256E-9
			* GeV, 2.298E-9 * GeV, 2.341E-9 * GeV, 2.386E-9 * GeV, 2.433E-9
			* GeV, 2.481E-9 * GeV, 2.532E-9 * GeV, 2.585E-9 * GeV, 2.640E-9
			* GeV, 2.697E-9 * GeV, 2.757E-9 * GeV, 2.820E-9 * GeV, 2.885E-9
			* GeV, 2.954E-9 * GeV, 3.026E-9 * GeV, 3.102E-9 * GeV, 3.181E-9
			* GeV, 3.265E-9 * GeV, 3.353E-9 * GeV, 3.446E-9 * GeV, 3.545E-9
			* GeV, 3.649E-9 * GeV, 3.760E-9 * GeV, 3.877E-9 * GeV, 4.002E-9
			* GeV, 4.136E-9 * GeV };

	//From SFDETSIM water absorption
	const G4int NUMENTRIES_water = 60;

	G4double ENERGY_water[NUMENTRIES_water] = { 1.56962e-09 * GeV, 1.58974e-09
			* GeV, 1.61039e-09 * GeV, 1.63157e-09 * GeV, 1.65333e-09 * GeV,
			1.67567e-09 * GeV, 1.69863e-09 * GeV, 1.72222e-09 * GeV, 1.74647e-09
					* GeV, 1.77142e-09 * GeV, 1.7971e-09 * GeV, 1.82352e-09
					* GeV, 1.85074e-09 * GeV, 1.87878e-09 * GeV, 1.90769e-09
					* GeV, 1.93749e-09 * GeV, 1.96825e-09 * GeV, 1.99999e-09
					* GeV, 2.03278e-09 * GeV, 2.06666e-09 * GeV, 2.10169e-09
					* GeV, 2.13793e-09 * GeV, 2.17543e-09 * GeV, 2.21428e-09
					* GeV, 2.25454e-09 * GeV, 2.29629e-09 * GeV, 2.33962e-09
					* GeV, 2.38461e-09 * GeV, 2.43137e-09 * GeV, 2.47999e-09
					* GeV, 2.53061e-09 * GeV, 2.58333e-09 * GeV, 2.63829e-09
					* GeV, 2.69565e-09 * GeV, 2.75555e-09 * GeV, 2.81817e-09
					* GeV, 2.88371e-09 * GeV, 2.95237e-09 * GeV, 3.02438e-09
					* GeV, 3.09999e-09 * GeV, 3.17948e-09 * GeV, 3.26315e-09
					* GeV, 3.35134e-09 * GeV, 3.44444e-09 * GeV, 3.54285e-09
					* GeV, 3.64705e-09 * GeV, 3.75757e-09 * GeV, 3.87499e-09
					* GeV, 3.99999e-09 * GeV, 4.13332e-09 * GeV, 4.27585e-09
					* GeV, 4.42856e-09 * GeV, 4.59258e-09 * GeV, 4.76922e-09
					* GeV, 4.95999e-09 * GeV, 5.16665e-09 * GeV, 5.39129e-09
					* GeV, 5.63635e-09 * GeV, 5.90475e-09 * GeV, 6.19998e-09
					* GeV };

	// Air
	G4double RINDEX_air[NUMENTRIES_water] =
			{ 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
					1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
					1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
					1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
					1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 };

	// M Fechner : new ; define the water refraction index using refsg.F
	//from skdetsim using the whole range.
	G4double RINDEX1[NUMENTRIES_water] = { 1.32885, 1.32906, 1.32927, 1.32948,
			1.3297, 1.32992, 1.33014, 1.33037, 1.3306, 1.33084, 1.33109,
			1.33134, 1.3316, 1.33186, 1.33213, 1.33241, 1.3327, 1.33299,
			1.33329, 1.33361, 1.33393, 1.33427, 1.33462, 1.33498, 1.33536,
			1.33576, 1.33617, 1.3366, 1.33705, 1.33753, 1.33803, 1.33855,
			1.33911, 1.3397, 1.34033, 1.341, 1.34172, 1.34248, 1.34331, 1.34419,
			1.34515, 1.3462, 1.34733, 1.34858, 1.34994, 1.35145, 1.35312,
			1.35498, 1.35707, 1.35943, 1.36211, 1.36518, 1.36872, 1.37287,
			1.37776, 1.38362, 1.39074, 1.39956, 1.41075, 1.42535 };

	G4double ABWFF = 1.0;

	// Get from the tuning parameters
	ABWFF = (WCSimTuningParameters::Instance())->GetAbwff();

	//T. Akiri: Values from Skdetsim
	G4double ABSORPTION_water[NUMENTRIES_water] = { 16.1419 * cm * ABWFF, 18.278
			* cm * ABWFF, 21.0657 * cm * ABWFF, 24.8568 * cm * ABWFF, 30.3117
			* cm * ABWFF, 38.8341 * cm * ABWFF, 54.0231 * cm * ABWFF, 81.2306
			* cm * ABWFF, 120.909 * cm * ABWFF, 160.238 * cm * ABWFF, 193.771
			* cm * ABWFF, 215.017 * cm * ABWFF, 227.747 * cm * ABWFF, 243.85
			* cm * ABWFF, 294.036 * cm * ABWFF, 321.647 * cm * ABWFF, 342.81
			* cm * ABWFF, 362.827 * cm * ABWFF, 378.041 * cm * ABWFF, 449.378
			* cm * ABWFF, 739.434 * cm * ABWFF, 1114.23 * cm * ABWFF, 1435.56
			* cm * ABWFF, 1611.06 * cm * ABWFF, 1764.18 * cm * ABWFF, 2100.95
			* cm * ABWFF, 2292.9 * cm * ABWFF, 2431.33 * cm * ABWFF, 3053.6 * cm
			* ABWFF, 4838.23 * cm * ABWFF, 6539.65 * cm * ABWFF, 7682.63 * cm
			* ABWFF, 9137.28 * cm * ABWFF, 12220.9 * cm * ABWFF, 15270.7 * cm
			* ABWFF, 19051.5 * cm * ABWFF, 23671.3 * cm * ABWFF, 29191.1 * cm
			* ABWFF, 35567.9 * cm * ABWFF, 42583 * cm * ABWFF, 49779.6 * cm
			* ABWFF, 56465.3 * cm * ABWFF, 61830 * cm * ABWFF, 65174.6 * cm
			* ABWFF, 66143.7 * cm * ABWFF, 64820 * cm * ABWFF, 61635 * cm
			* ABWFF, 57176.2 * cm * ABWFF, 52012.1 * cm * ABWFF, 46595.7 * cm
			* ABWFF, 41242.1 * cm * ABWFF, 36146.3 * cm * ABWFF, 31415.4 * cm
			* ABWFF, 27097.8 * cm * ABWFF, 23205.7 * cm * ABWFF, 19730.3 * cm
			* ABWFF, 16651.6 * cm * ABWFF, 13943.6 * cm * ABWFF, 11578.1 * cm
			* ABWFF, 9526.13 * cm * ABWFF };

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
	G4double RAYLEIGH_water[NUMENTRIES_water] = { 386929 * cm * RAYFF, 366249
			* cm * RAYFF, 346398 * cm * RAYFF, 327355 * cm * RAYFF, 309097 * cm
			* RAYFF, 291603 * cm * RAYFF, 274853 * cm * RAYFF, 258825 * cm
			* RAYFF, 243500 * cm * RAYFF, 228856 * cm * RAYFF, 214873 * cm
			* RAYFF, 201533 * cm * RAYFF, 188816 * cm * RAYFF, 176702 * cm
			* RAYFF, 165173 * cm * RAYFF, 154210 * cm * RAYFF, 143795 * cm
			* RAYFF, 133910 * cm * RAYFF, 124537 * cm * RAYFF, 115659 * cm
			* RAYFF, 107258 * cm * RAYFF, 99318.2 * cm * RAYFF, 91822.2 * cm
			* RAYFF, 84754 * cm * RAYFF, 78097.3 * cm * RAYFF, 71836.5 * cm
			* RAYFF, 65956 * cm * RAYFF, 60440.6 * cm * RAYFF, 55275.4 * cm
			* RAYFF, 50445.6 * cm * RAYFF, 45937 * cm * RAYFF, 41735.2 * cm
			* RAYFF, 37826.6 * cm * RAYFF, 34197.6 * cm * RAYFF, 30834.9 * cm
			* RAYFF, 27725.4 * cm * RAYFF, 24856.6 * cm * RAYFF, 22215.9 * cm
			* RAYFF, 19791.3 * cm * RAYFF, 17570.9 * cm * RAYFF, 15543 * cm
			* RAYFF, 13696.6 * cm * RAYFF, 12020.5 * cm * RAYFF, 10504.1 * cm
			* RAYFF, 9137.15 * cm * RAYFF, 7909.45 * cm * RAYFF, 6811.3 * cm
			* RAYFF, 5833.25 * cm * RAYFF, 4966.2 * cm * RAYFF, 4201.36 * cm
			* RAYFF, 3530.28 * cm * RAYFF, 2944.84 * cm * RAYFF, 2437.28 * cm
			* RAYFF, 2000.18 * cm * RAYFF, 1626.5 * cm * RAYFF, 1309.55 * cm
			* RAYFF, 1043.03 * cm * RAYFF, 821.016 * cm * RAYFF, 637.97 * cm
			* RAYFF, 488.754 * cm * RAYFF };

	// Get from the tuning parameters
	G4double MIEFF = (WCSimTuningParameters::Instance())->GetMieff();
	//G4double MIEFF = 0.0;
	//    printf("MIEFF: %f\n",MIEFF);

	//Values extracted from Skdetsim
	G4double MIE_water[NUMENTRIES_water] = { 7790020 * cm * MIEFF, 7403010 * cm
			* MIEFF, 7030610 * cm * MIEFF, 6672440 * cm * MIEFF, 6328120 * cm
			* MIEFF, 5997320 * cm * MIEFF, 5679650 * cm * MIEFF, 5374770 * cm
			* MIEFF, 5082340 * cm * MIEFF, 4802000 * cm * MIEFF, 4533420 * cm
			* MIEFF, 4276280 * cm * MIEFF, 4030220 * cm * MIEFF, 3794950 * cm
			* MIEFF, 3570120 * cm * MIEFF, 3355440 * cm * MIEFF, 3150590 * cm
			* MIEFF, 2955270 * cm * MIEFF, 2769170 * cm * MIEFF, 2592000 * cm
			* MIEFF, 2423470 * cm * MIEFF, 2263300 * cm * MIEFF, 2111200 * cm
			* MIEFF, 1966900 * cm * MIEFF, 1830120 * cm * MIEFF, 1700610 * cm
			* MIEFF, 1578100 * cm * MIEFF, 1462320 * cm * MIEFF, 1353040 * cm
			* MIEFF, 1250000 * cm * MIEFF, 1152960 * cm * MIEFF, 1061680 * cm
			* MIEFF, 975936 * cm * MIEFF, 895491 * cm * MIEFF, 820125 * cm
			* MIEFF, 749619 * cm * MIEFF, 683760 * cm * MIEFF, 622339 * cm
			* MIEFF, 565152 * cm * MIEFF, 512000 * cm * MIEFF, 462688 * cm
			* MIEFF, 417027 * cm * MIEFF, 374832 * cm * MIEFF, 335923 * cm
			* MIEFF, 300125 * cm * MIEFF, 267267 * cm * MIEFF, 237184 * cm
			* MIEFF, 209715 * cm * MIEFF, 184704 * cm * MIEFF, 162000 * cm
			* MIEFF, 141456 * cm * MIEFF, 122931 * cm * MIEFF, 106288 * cm
			* MIEFF, 91395.2 * cm * MIEFF, 78125 * cm * MIEFF, 66355.2 * cm
			* MIEFF, 55968.2 * cm * MIEFF, 46851.2 * cm * MIEFF, 38896.2 * cm
			* MIEFF, 32000 * cm * MIEFF };

	G4double MIE_water_const[3] = { 0.4, 0., 1 };// gforward, gbackward, forward backward ratio

	// M Fechner : unphysical, I want to reduce reflections
	// back to the old value 1.55

	G4double RINDEX_glass[NUMENTRIES_water] = { 1.600, 1.600, 1.600, 1.600,
			1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600,
			1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600,
			1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600,
			1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600,
			1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600,
			1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600,
			1.600, 1.600 };

	// M Fechner : the quantum efficiency already takes glass abs into account

	G4double ABSORPTION_glass[NUMENTRIES_water] = { 1.0e9 * cm, 1.0e9 * cm,
			1.0e9 * cm, 1.0e9 * cm, 1.0e9 * cm, 1.0e9 * cm, 1.0e9 * cm, 1.0e9
					* cm, 1.0e9 * cm, 1.0e9 * cm, 1.0e9 * cm, 1.0e9 * cm, 1.0e9
					* cm, 1.0e9 * cm, 1.0e9 * cm, 1.0e9 * cm, 1.0e9 * cm, 1.0e9
					* cm, 1.0e9 * cm, 1.0e9 * cm, 1.0e9 * cm, 1.0e9 * cm, 1.0e9
					* cm, 1.0e9 * cm, 1.0e9 * cm, 1.0e9 * cm, 1.0e9 * cm, 1.0e9
					* cm, 1.0e9 * cm, 1.0e9 * cm, 1.0e9 * cm, 1.0e9 * cm, 1.0e9
					* cm, 1.0e9 * cm, 1.0e9 * cm, 1.0e9 * cm, 1.0e9 * cm, 1.0e9
					* cm, 1.0e9 * cm, 1.0e9 * cm, 1.0e9 * cm, 1.0e9 * cm, 1.0e9
					* cm, 1.0e9 * cm, 1.0e9 * cm, 1.0e9 * cm, 1.0e9 * cm, 1.0e9
					* cm, 1.0e9 * cm, 1.0e9 * cm, 1.0e9 * cm, 1.0e9 * cm, 1.0e9
					* cm, 1.0e9 * cm, 1.0e9 * cm, 1.0e9 * cm, 1.0e9 * cm, 1.0e9
					* cm, 1.0e9 * cm, 1.0e9 * cm };

	G4double BLACKABS_blacksheet[NUMENTRIES_water] = { 1.0e-9 * cm, 1.0e-9 * cm,
			1.0e-9 * cm, 1.0e-9 * cm, 1.0e-9 * cm, 1.0e-9 * cm, 1.0e-9 * cm,
			1.0e-9 * cm, 1.0e-9 * cm, 1.0e-9 * cm, 1.0e-9 * cm, 1.0e-9 * cm,
			1.0e-9 * cm, 1.0e-9 * cm, 1.0e-9 * cm, 1.0e-9 * cm, 1.0e-9 * cm,
			1.0e-9 * cm, 1.0e-9 * cm, 1.0e-9 * cm, 1.0e-9 * cm, 1.0e-9 * cm,
			1.0e-9 * cm, 1.0e-9 * cm, 1.0e-9 * cm, 1.0e-9 * cm, 1.0e-9 * cm,
			1.0e-9 * cm, 1.0e-9 * cm, 1.0e-9 * cm, 1.0e-9 * cm, 1.0e-9 * cm,
			1.0e-9 * cm, 1.0e-9 * cm, 1.0e-9 * cm, 1.0e-9 * cm, 1.0e-9 * cm,
			1.0e-9 * cm, 1.0e-9 * cm, 1.0e-9 * cm, 1.0e-9 * cm, 1.0e-9 * cm,
			1.0e-9 * cm, 1.0e-9 * cm, 1.0e-9 * cm, 1.0e-9 * cm, 1.0e-9 * cm,
			1.0e-9 * cm, 1.0e-9 * cm, 1.0e-9 * cm, 1.0e-9 * cm, 1.0e-9 * cm,
			1.0e-9 * cm, 1.0e-9 * cm, 1.0e-9 * cm, 1.0e-9 * cm, 1.0e-9 * cm,
			1.0e-9 * cm, 1.0e-9 * cm, 1.0e-9 * cm };

	//July 1, 2010, F. Beroz: changed SK1SK2FF to BSRFF to avoid confusion,
	// since this parameter is not from SK.

	G4double BSRFF = 1.0;

	// Get from the tuning parameters
	BSRFF = (WCSimTuningParameters::Instance())->GetBsrff();

	G4double REFLECTIVITY_blacksheet[NUMENTRIES_water] = { 0.055 * BSRFF, 0.055
			* BSRFF, 0.055 * BSRFF, 0.055 * BSRFF, 0.055 * BSRFF, 0.055 * BSRFF,
			0.055 * BSRFF, 0.055 * BSRFF, 0.055 * BSRFF, 0.055 * BSRFF, 0.055
					* BSRFF, 0.055 * BSRFF, 0.055 * BSRFF, 0.055 * BSRFF, 0.055
					* BSRFF, 0.055 * BSRFF, 0.055 * BSRFF, 0.055 * BSRFF, 0.055
					* BSRFF, 0.055 * BSRFF, 0.055 * BSRFF, 0.055 * BSRFF, 0.055
					* BSRFF, 0.055 * BSRFF, 0.055 * BSRFF, 0.055 * BSRFF, 0.055
					* BSRFF, 0.055 * BSRFF, 0.055 * BSRFF, 0.055 * BSRFF, 0.055
					* BSRFF, 0.057 * BSRFF, 0.059 * BSRFF, 0.060 * BSRFF, 0.059
					* BSRFF, 0.058 * BSRFF, 0.057 * BSRFF, 0.055 * BSRFF, 0.050
					* BSRFF, 0.045 * BSRFF, 0.045 * BSRFF, 0.045 * BSRFF, 0.045
					* BSRFF, 0.045 * BSRFF, 0.045 * BSRFF, 0.045 * BSRFF, 0.045
					* BSRFF, 0.045 * BSRFF, 0.045 * BSRFF, 0.045 * BSRFF, 0.045
					* BSRFF, 0.045 * BSRFF, 0.045 * BSRFF, 0.045 * BSRFF, 0.045
					* BSRFF, 0.045 * BSRFF, 0.045 * BSRFF, 0.045 * BSRFF, 0.045
					* BSRFF, 0.045 * BSRFF };

	//utter fiction at this stage
	G4double EFFICIENCY[NUMENTRIES_water] = { 0.001 * m };

	//utter fiction at this stage, does not matter
	G4double RAYLEIGH_air[NUMENTRIES_water] = { 0.001 * m, 0.001 * m, 0.001 * m,
			0.001 * m, 0.001 * m, 0.001 * m, 0.001 * m, 0.001 * m, 0.001 * m,
			0.001 * m, 0.001 * m, 0.001 * m, 0.001 * m, 0.001 * m, 0.001 * m,
			0.001 * m, 0.001 * m, 0.001 * m, 0.001 * m, 0.001 * m, 0.001 * m,
			0.001 * m, 0.001 * m, 0.001 * m, 0.001 * m, 0.001 * m, 0.001 * m,
			0.001 * m, 0.001 * m, 0.001 * m, 0.001 * m, 0.001 * m, 0.001 * m,
			0.001 * m, 0.001 * m, 0.001 * m, 0.001 * m, 0.001 * m, 0.001 * m,
			0.001 * m, 0.001 * m, 0.001 * m, 0.001 * m, 0.001 * m, 0.001 * m,
			0.001 * m, 0.001 * m, 0.001 * m, 0.001 * m, 0.001 * m, 0.001 * m,
			0.001 * m, 0.001 * m, 0.001 * m, 0.001 * m, 0.001 * m, 0.001 * m,
			0.001 * m, 0.001 * m, 0.001 * m };

	//utter fiction at this stage, does not matter
	G4double MIE_air[NUMENTRIES_water] = { 0.1 * m, 0.1 * m, 0.1 * m, 0.1 * m,
			0.1 * m, 0.1 * m, 0.1 * m, 0.1 * m, 0.1 * m, 0.1 * m, 0.1 * m, 0.1
					* m, 0.1 * m, 0.1 * m, 0.1 * m, 0.1 * m, 0.1 * m, 0.1 * m,
			0.1 * m, 0.1 * m, 0.1 * m, 0.1 * m, 0.1 * m, 0.1 * m, 0.1 * m, 0.1
					* m, 0.1 * m, 0.1 * m, 0.1 * m, 0.1 * m, 0.1 * m, 0.1 * m,
			0.1 * m, 0.1 * m, 0.1 * m, 0.1 * m, 0.1 * m, 0.1 * m, 0.1 * m, 0.1
					* m, 0.1 * m, 0.1 * m, 0.1 * m, 0.1 * m, 0.1 * m, 0.1 * m,
			0.1 * m, 0.1 * m, 0.1 * m, 0.1 * m, 0.1 * m, 0.1 * m, 0.1 * m, 0.1
					* m, 0.1 * m, 0.1 * m, 0.1 * m, 0.1 * m, 0.1 * m, 0.1 * m };

	G4double MIE_air_const[3] = { 0.99, 0.99, 0.8 };// gforward, gbackward, forward backward ratio

	G4double EFFICIENCY_blacksheet[NUMENTRIES_water] = { 0.0 };

	// Water - make them all the same for now

	G4MaterialPropertiesTable *myMPT1 = new G4MaterialPropertiesTable();
	// M Fechner : new   ; wider range for lambda
	myMPT1->AddProperty("RINDEX", ENERGY_water, RINDEX1, NUMENTRIES_water);
	myMPT1->AddProperty("ABSLENGTH", ENERGY_water, ABSORPTION_water,
			NUMENTRIES_water);
	// M Fechner: new, don't let G4 compute it.
	myMPT1->AddProperty("RAYLEIGH", ENERGY_water, RAYLEIGH_water,
			NUMENTRIES_water);

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
	myMPT2->AddProperty("ABSLENGTH", ENERGY_water, BLACKABS_blacksheet,
			NUMENTRIES_water);
	myMPT2->AddProperty("RAYLEIGH", ENERGY_water, RAYLEIGH_air,
			NUMENTRIES_water);

	// myMPT2->AddProperty("MIEHG",ENERGY_water,MIE_air,NUMENTRIES_water);
	//       myMPT2->AddConstProperty("MIEHG_FORWARD",MIE_air_const[0]);
	//       myMPT2->AddConstProperty("MIEHG_BACKWARD",MIE_air_const[1]);
	//       myMPT2->AddConstProperty("MIEHG_FORWARD_RATIO",MIE_air_const[2]);

	GetMaterial("Air")->SetMaterialPropertiesTable(myMPT2);

	// Plastic
	////////////////////

	G4MaterialPropertiesTable *myMPT3 = new G4MaterialPropertiesTable();
	myMPT3->AddProperty("ABSLENGTH", ENERGY_water, BLACKABS_blacksheet,
			NUMENTRIES_water);
	myMPT3->AddProperty("REFLECTIVITY", ENERGY_water, REFLECTIVITY_blacksheet,
			NUMENTRIES_water);
	myMPT3->AddProperty("EFFICIENCY", ENERGY_water, EFFICIENCY,
			NUMENTRIES_water);
	GetMaterial("Plastic")->SetMaterialPropertiesTable(myMPT3);

	// Blacksheet
	/////////////////////

	G4MaterialPropertiesTable *myMPT4 = new G4MaterialPropertiesTable();
	myMPT4->AddProperty("ABSLENGTH", ENERGY_water, BLACKABS_blacksheet,
			NUMENTRIES_water);
	GetMaterial("Blacksheet")->SetMaterialPropertiesTable(myMPT4);

	// Glass
	////////////////

	G4MaterialPropertiesTable *myMPT5 = new G4MaterialPropertiesTable();
	myMPT5->AddProperty("RINDEX", ENERGY_water, RINDEX_glass, NUMENTRIES_water);
	myMPT5->AddProperty("ABSLENGTH", ENERGY_water, ABSORPTION_glass,
			NUMENTRIES_water);
	GetMaterial("Glass")->SetMaterialPropertiesTable(myMPT5);

	// Tyvek
	////////////////

	// jl145 ----
	// Abs legnth is same as blacksheet, very small.
	G4MaterialPropertiesTable *myMPT6 = new G4MaterialPropertiesTable();
	myMPT6->AddProperty("ABSLENGTH", ENERGY_water, BLACKABS_blacksheet,
			NUMENTRIES_water);
	GetMaterial("Tyvek")->SetMaterialPropertiesTable(myMPT6);


	// SURFACES
	//	------------- Surfaces --------------

	// Blacksheet
	const G4int NUM = 2;
	//   G4double PP[NUM] =
	//{ 2.038E-9*GeV, 4.144E-9*GeV };

	G4double PP[NUM] = { 1.4E-9 * GeV, 6.2E-9 * GeV };
	G4double RINDEX_blacksheet[NUM] = { 1.6, 1.6 };

	G4double SPECULARLOBECONSTANT[NUM] = { 0.3, 0.3 };
	G4double SPECULARSPIKECONSTANT[NUM] = { 0.2, 0.2 };
	G4double BACKSCATTERCONSTANT[NUM] = { 0.2, 0.2 };
	G4MaterialPropertiesTable *myST1 = new G4MaterialPropertiesTable();
	myST1->AddProperty("RINDEX", ENERGY_water, RINDEX_blacksheet,
			NUMENTRIES_water);
	myST1->AddProperty("SPECULARLOBECONSTANT", PP, SPECULARLOBECONSTANT, NUM);
	myST1->AddProperty("SPECULARSPIKECONSTANT", PP, SPECULARSPIKECONSTANT, NUM);
	myST1->AddProperty("BACKSCATTERCONSTANT", PP, BACKSCATTERCONSTANT, NUM);
	myST1->AddProperty("REFLECTIVITY", ENERGY_water, REFLECTIVITY_blacksheet,
			NUMENTRIES_water);
	myST1->AddProperty("EFFICIENCY", ENERGY_water, EFFICIENCY_blacksheet,
			NUMENTRIES_water);
	fOpWaterBSSurface->SetMaterialPropertiesTable(myST1);

	//Glass to Cathode surface inside PMTs
	G4double RINDEX_cathode[NUM] = { 1.0, 1.0 };

	G4double RGCFF = 0.0;
	RGCFF = (WCSimTuningParameters::Instance())->GetRgcff();

	G4double REFLECTIVITY_glasscath[NUM] = { 1.0 * RGCFF, 1.0 * RGCFF };

	G4double EFFICIENCY_glasscath[NUM] = { 0.0, 0.0 };
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
	G4double RINDEX_tyvek[NUM] = { 1.5, 1.5 }; // polyethylene permittivity is ~2.25
	G4double TySPECULARLOBECONSTANT[NUM] = { 0.75, 0.75 }; // crudely estimated from A. Chavarria's thesis
	G4double TySPECULARSPIKECONSTANT[NUM] = { 0.0, 0.0 };
	G4double TyBACKSCATTERCONSTANT[NUM] = { 0.0, 0.0 };
	// Lambertian prob is therefore 0.25
	G4double TyREFLECTIVITY[NUM] = { 0.94, 0.94 }; //cf. DuPont
	G4MaterialPropertiesTable *myST3 = new G4MaterialPropertiesTable();
	myST3->AddProperty("RINDEX", PP, RINDEX_tyvek, NUM);
	myST3->AddProperty("SPECULARLOBECONSTANT", PP, TySPECULARLOBECONSTANT, NUM);
	myST3->AddProperty("SPECULARSPIKECONSTANT", PP, TySPECULARSPIKECONSTANT,
			NUM);
	myST3->AddProperty("BACKSCATTERCONSTANT", PP, TyBACKSCATTERCONSTANT, NUM);
	myST3->AddProperty("REFLECTIVITY", PP, TyREFLECTIVITY, NUM);
	myST3->AddProperty("EFFICIENCY", PP, EFFICIENCY_blacksheet, NUM);
	//use same efficiency as blacksheet, which is 0
	fOpWaterTySurface->SetMaterialPropertiesTable(myST3);


}

G4OpticalSurface* WCSimMaterialsBuilder::GetOpticalSurface(
		const G4String& name) const {
	G4OpticalSurface * surf = NULL;
	if (fOpticalSurfaces.find(name) != fOpticalSurfaces.end()) {
		surf = fOpticalSurfaces[name];
	} else {
		std::cerr << "Could not find surface " << name << std::endl;
		assert( fOpticalSurfaces.find(name) != fOpticalSurfaces.end());
	}
	return surf;
}

void WCSimMaterialsBuilder::BuildSurfacePropertiesTable() {
// Should move the surface properties here...
}

void WCSimMaterialsBuilder::BuildSurfaces() {

	//	------------- Surfaces --------------
	if (fOpWaterBSSurface == NULL) {
		fOpWaterBSSurface = new G4OpticalSurface("WaterBSCellSurface");
		fOpWaterBSSurface->SetType(dielectric_dielectric);
		fOpWaterBSSurface->SetModel(unified);
		fOpWaterBSSurface->SetFinish(groundfrontpainted);
		fOpWaterBSSurface->SetSigmaAlpha(0.1);
	}

	if (fOpGlassCathodeSurface == NULL) {
		fOpGlassCathodeSurface = new G4OpticalSurface("GlassCathodeSurface");
		fOpGlassCathodeSurface->SetType(dielectric_dielectric);
		fOpGlassCathodeSurface->SetModel(unified);
		//   fOpGlassCathodeSurface->SetFinish(groundbackpainted);
		fOpGlassCathodeSurface->SetFinish(polished);
		//fOpGlassCathodeSurface->SetSigmaAlpha(0.002);
		// was 1.0
		// totally unphysical anyway
	}

	// jl145 ----
	//
	if (fOpWaterTySurface == NULL) {
		fOpWaterTySurface = new G4OpticalSurface("WaterTyCellSurface");
		fOpWaterTySurface->SetType(dielectric_dielectric);
		fOpWaterTySurface->SetModel(unified);
		fOpWaterTySurface->SetFinish(groundbackpainted); //a guess, but seems to work
		fOpWaterTySurface->SetSigmaAlpha(0.5); //cf. A. Chavarria's ~30deg
	}
}

