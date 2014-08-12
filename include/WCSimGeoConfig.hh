#ifndef WCSimPMTConfig_H
#define WCSimPMTConfig_H 1

#include <vector>
#include <string>

// GEANT definitions
#include "globals.hh"

// This class is designed to store the information about a type of PMT.

class WCSimGeoConfig {
public:
	// Default constructor
	WCSimGeoConfig();
	// Copy constructor
	WCSimGeoConfig(const WCSimGeoConfig &rhs);
	// Destructor
	~WCSimGeoConfig();

	// Getter and setter functions
	double GetInnerRadius() const;
	void SetInnerRadius(double radius);

	double GetInnerHeight() const;
	void SetInnerHeight(double height);

	unsigned int GetNSides() const;
	void SetNSides(unsigned int nsides);

	std::string GetGeoName() const;
	void SetGeoName(std::string name);

	std::string GetPMTName1() const;
	void SetPMTName1(std::string name); 

	double GetCoverage() const;
	void SetCoverage(double coverage);

	void Print() const;
	
private:

	std::string fGeoName;
	double fInnerRadius;
	double fInnerHeight;
	unsigned int fNSides;

	std::string fPMTName1;
	double fPercentCoverage;

};

#endif
