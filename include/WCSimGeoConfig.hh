#ifndef WCSimGeoConfig_H
#define WCSimGeoConfig_H

#include <map>
#include <math.h>
#include <string>
#include <vector>

#include "WCSimGeometryEnums.hh"
#include "WCSimDetectorZone.hh"

// GEANT definitions
#include "globals.hh"


// This class is designed to store the information about a type of geometry.


class WCSimGeoConfig
{
public:
	// Default constructor
	WCSimGeoConfig();
	// Copy constructor
	WCSimGeoConfig(const WCSimGeoConfig &rhs);
	// Destructor
	~WCSimGeoConfig();

	// Getter and setter functions
	double GetOuterRadius() const; //< The radius from the centre to a corner
	double GetInnerRadius() const; //< The radius from the centre to the middle of a side
	void SetOuterRadius(double radius);

	double GetInnerHeight() const;
	void SetInnerHeight(double height);

	unsigned int GetNSides() const;
	void SetNSides(unsigned int nsides);

	std::string GetGeoName() const;
	void SetGeoName(std::string name);

	void SetCoverageType(std::string typeName);
	void SetCoverageType(WCSimGeometryEnums::PhotodetectorLimit_t type);

	double GetCoverage() const;
	double GetCoverageFraction() const;

	void SetUseOverallCoverage(bool doIt);
	bool GetUseOverallCoverage();
	double GetOverallCoverageFraction() const;
	void SetOverallCoverage(double coverage);

	void SetUseZonalCoverage(bool doIt);
	bool GetUseZonalCoverage();
	double GetZonalCoverageFraction(WCSimGeometryEnums::DetectorRegion_t region, int zone) const;
	void SetZonalCoverage( WCSimGeometryEnums::DetectorRegion_t region, int zone, double coverage );
	void SetZonalCoverage(double coverage);


	void AddCellPMTName(std::string name);
	void AddCellPMTName(WCSimGeometryEnums::DetectorRegion_t region, int zone, std::string name);
	std::string GetCellPMTName(WCSimGeometryEnums::DetectorRegion_t region, int zone,
			unsigned int pmt) const;
	std::vector<std::string> GetCellPMTName(WCSimGeometryEnums::DetectorRegion_t region,
			int zone) const;
	std::vector<std::string> GetPMTNamesUsed() const;

	void AddCellPMTX(double x);
	void AddCellPMTX(WCSimGeometryEnums::DetectorRegion_t region, int zone, double x);
	double GetCellPMTX(WCSimGeometryEnums::DetectorRegion_t region, int zone, unsigned int pmt) const;
	std::vector<double> GetCellPMTX(WCSimGeometryEnums::DetectorRegion_t region, int zone) const;

	void AddCellPMTY(double y);
	void AddCellPMTY(WCSimGeometryEnums::DetectorRegion_t region, int zone, double y);
	double GetCellPMTY(WCSimGeometryEnums::DetectorRegion_t region, int zone, unsigned int pmt) const;
	std::vector<double> GetCellPMTY(WCSimGeometryEnums::DetectorRegion_t region, int zone) const;

	void AddCellPMTFaceType(WCSimGeometryEnums::PMTDirection_t type);
	void AddCellPMTFaceType(std::string typeName);
	void AddCellPMTFaceType(WCSimGeometryEnums::DetectorRegion_t region, int zone, WCSimGeometryEnums::PMTDirection_t type);
	double GetCellPMTFaceType(WCSimGeometryEnums::DetectorRegion_t region, int zone, unsigned int pmt) const;
	std::vector<WCSimGeometryEnums::PMTDirection_t> GetCellPMTFaceType(WCSimGeometryEnums::DetectorRegion_t region, int zone) const;

	void AddCellPMTFaceTheta(double theta);
	void AddCellPMTFaceTheta(WCSimGeometryEnums::DetectorRegion_t region, int zone, double theta);
	double GetCellPMTFaceTheta(WCSimGeometryEnums::DetectorRegion_t region, int zone, unsigned int pmt) const;
	std::vector<double> GetCellPMTFaceTheta(WCSimGeometryEnums::DetectorRegion_t region, int zone) const;

	void AddCellPMTFacePhi(double phi);
	void AddCellPMTFacePhi(WCSimGeometryEnums::DetectorRegion_t region, int zone, double theta);
	double GetCellPMTFacePhi(WCSimGeometryEnums::DetectorRegion_t region, int zone, unsigned int pmt) const;
	std::vector<double> GetCellPMTFacePhi(WCSimGeometryEnums::DetectorRegion_t region, int zone) const;

	void AddCurrentRegion(std::string regionName);
	void AddCurrentZone(int zoneNum);
	void ResetCurrent();

	WCSimGeometryEnums::PhotodetectorLimit_t GetPMTLimit() const
	{
		return fPMTLimit;
	}

	bool CanBuildWithoutAngles(std::string faceName);
	bool IsGood() const;
	bool IsGoodZone(std::pair<WCSimGeometryEnums::DetectorRegion_t, int> zone) const;
	void Print() const;

	void SetLimitPMTNumbers(bool doIt = true);
	bool GetLimitPMTNumber() const;
	void SetPMTLimit(WCSimGeometryEnums::DetectorRegion_t region, int zone, std::string name, int limit);
	void SetPMTLimit(std::string name, int limit);
	int GetPMTLimit(WCSimGeometryEnums::DetectorRegion_t region, int zone, std::string name);
	int GetMaxZoneCells(WCSimGeometryEnums::DetectorRegion_t region, int zone) const;

	void SetZoneThetaStart(WCSimGeometryEnums::DetectorRegion_t region, int zoneNum, double theta);
	void SetZoneThetaEnd(WCSimGeometryEnums::DetectorRegion_t region, int zoneNum, double theta);
	double GetZoneThetaStart(WCSimGeometryEnums::DetectorRegion_t region, int zoneNum);
	double GetZoneThetaEnd(WCSimGeometryEnums::DetectorRegion_t region, int zoneNum);
	void SetZoneThetaStart(double theta);
	void SetZoneThetaEnd(double theta);

	unsigned int GetNumZones(WCSimGeometryEnums::DetectorRegion_t region) const;

private:

	void CreateMissingZone(WCSimGeometryEnums::DetectorRegion_t region, int zone);

	WCSimGeometryEnums::PhotodetectorLimit_t fPMTLimit;
	std::map<std::string, int> fPMTLimitMap;

	std::string fGeoName;
	double fOuterRadius;
	double fInnerHeight;
	unsigned int fNSides;

	std::string fPMTName1;
	double fPercentCoverage;
	std::map<std::pair<WCSimGeometryEnums::DetectorRegion_t, int>, WCSimDetectorZone> fZoneMap;

	std::vector<std::string> fPMTNamesUsed;
	std::vector<WCSimGeometryEnums::DetectorRegion_t> fCurrentRegions;
	std::vector<int> fCurrentZones;

};

#endif
