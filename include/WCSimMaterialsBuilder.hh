/*
 * WCSimMaterialsBuilder.hh
 *
 *  Created on: 3 Sep 2014
 *      Author: andy
 */

#ifndef WCSIMMATERIALSBUILDER_HH_
#define WCSIMMATERIALSBUILDER_HH_
#include <map>
#include <vector>
#include "G4String.hh"
class G4OpticalSurface;
class G4Material;
class G4Element;

class WCSimMaterialsBuilder {
public:
	static WCSimMaterialsBuilder * Instance();
	G4OpticalSurface * GetOpticalSurface(const G4String &name) const;
	G4Material* GetVacuum() const;
	G4Element* GetElement(const G4String &name) const;
	G4Material* GetMaterial(const G4String &name) const;

	G4OpticalSurface* GetOpGlassCathodeSurface() const {
		return fOpGlassCathodeSurface;
	}

	G4OpticalSurface* GetOpWaterBsSurface() const {
		return fOpWaterBSSurface;
	}

	G4OpticalSurface* GetOpWaterWsSurface() const {
		return fOpWaterWSSurface;
	}

	G4OpticalSurface* GetOpWaterTySurface() const {
		return fOpWaterTySurface;
	}

private:
	WCSimMaterialsBuilder();
	virtual ~WCSimMaterialsBuilder();


	void BuildVacuum();
	void BuildElements();
	void BuildMaterials();
	void BuildMaterialPropertiesTable();
	void BuildSurfacePropertiesTable();
	void BuildSurfaces();

	std::map<G4String, G4Element*> fElements;
	std::vector<G4String> fMaterials;

	// Surfaces:
	std::map<G4String, G4OpticalSurface*> fOpticalSurfaces;
	G4OpticalSurface * fOpWaterBSSurface;
	G4OpticalSurface * fOpWaterWSSurface;
	G4OpticalSurface * fOpGlassCathodeSurface;
	G4OpticalSurface * fOpWaterTySurface;
        G4OpticalSurface * fOpWaterLCinSurface;
        G4OpticalSurface * fOpWaterLCoutSurface;
        G4OpticalSurface * fOpWaterPlanePipeSurface;
        G4OpticalSurface * fOpWaterPMTBackSurface;
};

#endif /* WCSIMMATERIALSBUILDER_HH_ */
