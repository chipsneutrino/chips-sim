/*
 * WCSimMaterialsBuilder.hh
 *
 *  Created on: 3 Sep 2014
 *      Author: andy
 */

#ifndef WCSIMMATERIALSBUILDER_HH_
#define WCSIMMATERIALSBUILDER_HH_

class WCSimMaterialsBuilder {
public:
	static WCSimMaterialsBuilder * Instance();
	G4OpticalSurface * GetOpticalSurface(const G4String &name) const;
	G4Material* GetVacuum(const G4String &name) const;
	G4Element * GetElement(const G4String &name) const;
	G4Material* GetMaterial(const G4String &name) const;

	G4OpticalSurface* GetOpGlassCathodeSurface() const {
		return fOpGlassCathodeSurface;
	}

	G4OpticalSurface* GetOpWaterBsSurface() const {
		return fOpWaterBSSurface;
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

	static std::vector<G4String> fElements;
	static std::vector<G4String> fMaterials;

	// Surfaces:
	std::map<G4String, G4OpticalSurface*> fOpticalSurfaces;
	G4OpticalSurface * fOpWaterBSSurface;
	G4OpticalSurface * fOpGlassCathodeSurface;
	G4OpticalSurface * fOpWaterTySurface;

};

#endif /* WCSIMMATERIALSBUILDER_HH_ */
