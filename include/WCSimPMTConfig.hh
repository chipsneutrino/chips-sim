#pragma once

#include <vector>
#include <string>
#include "TObject.h"

// GEANT definitions
#include "WCSimLCConfig.hh"
#include "WCSimLCManager.hh"

// This class is designed to store the information about a type of PMT.

class WCSimPMTConfig : public TObject
{
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

	double GetArea() const;

	double GetExposeHeight() const;
	void SetExposeHeight(double height);

	double GetUnsensitiveHeight() const;
	void SetUnsensitiveHeight(double height);

	double GetGlassThickness() const;
	void SetGlassThickness(double thick);

	double GetTimeConstant() const;
	void SetTimeConstant(double timeConst);

	std::vector<std::pair<double, double>> GetEfficiencyVector() const;
	void SetEfficiencyVector(std::vector<std::pair<double, double>> effVec);

	// Not set function for this, performed by SetEfficiency.
	double GetMaxEfficiency() const;

	// No set function for this, performed by SetEfficiency
	unsigned int GetNEfficiencyBins() const;

	std::string GetPMTName() const;
	void SetPMTName(std::string name);

	double GetMaxRadius() const;

	double GetMaxExposeHeight() const;

	std::string GetLCName() const;
	void SetLCName(std::string name);

	WCSimLCConfig GetLCConfig() const;
	void SetLCConfig(std::string name);

	void MyPrint() const;

private:
	double fRadius;
	double fExposeHeight;
	double fUnsensitiveHeight;
	double fGlassThickness;
	double fTimeConstant;
	// Efficency stored in a pair of <wavelength,efficiency>
	std::vector<std::pair<double, double>> fEffVec;
	double fMaxEff;
	unsigned int fEffBins;
	std::string fPMTName;
	std::string fLCName;

	WCSimLCConfig fLCConfig;
	WCSimLCManager fLCManager;

	ClassDef(WCSimPMTConfig, 1)
};
