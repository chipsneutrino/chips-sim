#ifndef WCSimPMTParams_H
#define WCSimPMTParams_H 1

#include <vector>
#include <string>

// GEANT definitions
#include "globals.hh"

// Leigh: This class is designed to store the information about the PMT.
// Separating it from the detector construction to allow for easier
// changes and to allow for multiple PMT definitions.

class WCSimPMTParams {
public:
	// Default constructor
	WCSimPMTParams();
	// Construct with a known type
	WCSimPMTParams(std::string type);
	// Destructor
	~WCSimPMTParams();

	// Getter functions
	double GetRadius();
	double GetExposeHeight();
	double GetGlassThickness();
	void GetEfficiency(G4float *eff, const unsigned int nBins);
	double GetMaxEfficiency();
	std::string GetPMTType();

	// Setter functions
	void SetPMTType(std::string type);

private:
	// Number of wavelength bins
	static const unsigned int fNBins;

	// Initialise the efficiency vectors
	void InitialiseEfficiencies();

	// Initialise the PMT list
	void InitialisePMTTypes();

	// Initialise the PMT radii and heights
	void InitialisePMTGeom();

	// The PMT type
	std::string fPMTType;
	// The list of all PMT types
	std::vector<std::string> fPMTTypeList;
	// The list of all the radii for the above PMTs
	std::vector<double> fRadii;
	// The list of all the exposed heights for the above PMTs
	std::vector<double> fExposeHeights;
	// The list of all the glass thicknesses for the above PMTs
	std::vector<double> fGlassThickness;
	// The efficiency as a function of wavelength
	// This is set when the PMT type is set.
	std::vector<std::vector<double> > fEfficiencies;
	// The maximum quantum efficiency
	std::vector<double> fMaxQEs;
	// The current PMT - this is the index of the above vectors
	int fCurrPMTType;

};

#endif
