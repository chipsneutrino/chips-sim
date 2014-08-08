#ifndef WCSimPMTConfig_H
#define WCSimPMTConfig_H 1

#include <vector>
#include <string>

// GEANT definitions
#include "globals.hh"

// This class is designed to store the information about a type of PMT.

class WCSimPMTConfig {
public:
	// Default constructor
	WCSimPMTConfig();
	// Copy constructor
	WCSimPMTConfig(const WCSimPMTConfig &rhs);
	// Destructor
	~WCSimPMTConfig();

	// Getter and setter functions
	double GetRadius() const;
	void SetRadius(double radius);

	double GetExposeHeight() const;
	void SetExposeHeight(double height);

	double GetGlassThickness() const;
	void SetGlassThickness(double thick);

	std::vector<std::pair<double,double> > GetEfficiency() const;
	void SetEfficiency(std::vector<std::pair<double,double> > effVec);

	// Not set function for this, performed by SetEfficiency.
	double GetMaxEfficiency() const;

	// No set function for this, performed by SetEfficiency
	unsigned int GetNEfficiencyBins() const;

	std::string GetPMTName() const;
	void SetPMTName(std::string name);

	void Print() const;
	
private:

	double fRadius;
	double fExposeHeight;
	double fGlassThickness;
	// Efficency stored in a pair of <wavelength,efficiency>
	std::vector<std::pair<double,double> > fEffVec;
	double fMaxEff;
	unsigned int fEffBins;
	std::string fPMTName;
};

#endif
