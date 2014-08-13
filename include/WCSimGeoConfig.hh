#ifndef WCSimGeoConfig_H
#define WCSimGeoConfig_H

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

	double GetCoverage() const;
	void SetCoverage(double coverage);

    void AddCellPMTName(std::string name);
    std::string GetCellPMTName(unsigned int pmt) const;
    std::vector<std::string> GetCellPMTName() const;

	void AddCellPMTX(double x);
    double GetCellPMTX(unsigned int pmt) const;
    std::vector<double> GetCellPMTX() const;

    void AddCellPMTY(double y);
    double GetCellPMTY(unsigned int pmt) const;
    std::vector<double> GetCellPMTY() const;

    bool IsGood() const;
	void Print() const;
	
private:

	std::string fGeoName;
	double fInnerRadius;
	double fInnerHeight;
	unsigned int fNSides;

	std::string fPMTName1;
	double fPercentCoverage;

	std::vector<std::string> fCellPMTName;
	std::vector<double> fCellPMTX;
	std::vector<double> fCellPMTY;

};

#endif
