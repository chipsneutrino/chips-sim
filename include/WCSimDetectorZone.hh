/*
 * WCSimDetectorZone.hh
 *
 *  Created on: 24 Nov 2014
 *      Author: ajperch
 */

#ifndef WCSIMDETECTORZONE_HH_
#define WCSIMDETECTORZONE_HH_

#include "G4ThreeVector.hh"
#include "WCSimGeometryEnums.hh"
#include <map>
#include <vector>

class WCSimDetectorZone
{
public:
	WCSimDetectorZone();
	virtual ~WCSimDetectorZone();

	void AddCellPMTName(std::string name);
	std::string GetCellPMTName(unsigned int pmt) const;
	std::vector<std::string> GetCellPMTName() const;

	void AddCellPMTX(double x);
	double GetCellPMTX(unsigned int pmt) const;
	std::vector<double> GetCellPMTX() const;

	void AddCellPMTY(double y);
	double GetCellPMTY(unsigned int pmt) const;
	std::vector<double> GetCellPMTY() const;

	void AddCellPMTFaceType(WCSimGeometryEnums::PMTDirection_t face);
	WCSimGeometryEnums::PMTDirection_t GetCellPMTFaceType(unsigned int pmt) const;
	std::vector<WCSimGeometryEnums::PMTDirection_t> GetCellPMTFaceType() const;

	void AddCellPMTFaceTheta(double theta);
	double GetCellPMTFaceTheta(unsigned int pmt) const;
	std::vector<double> GetCellPMTFaceTheta() const;

	void AddCellPMTFacePhi(double phi);
	double GetCellPMTFacePhi(unsigned int pmt) const;
	std::vector<double> GetCellPMTPhi() const;

	void SetPMTLimit(std::string name, int limit);
	int GetPMTLimit(std::string name) const;
	int GetMaxNumCells() const;

	bool IsGood(WCSimGeometryEnums::DetectorRegion_t region) const;

	double GetCoverage() const
	{
		return fCoverage;
	}

	void SetCoverage(double coverage)
	{
		this->fCoverage = coverage;
	}

	void Print() const;

	double GetThetaEnd() const
	{
		return fThetaEnd;
	}

	void SetThetaEnd(double thetaEnd)
	{
		this->fThetaEnd = thetaEnd;
	}

	double GetThetaStart() const
	{
		return fThetaStart;
	}

	void SetThetaStart(double thetaStart)
	{
		this->fThetaStart = thetaStart;
	}

private:
	double fCoverage;

	std::vector<std::string> fCellPMTName;
	std::map<std::string, int> fPMTLimit;
	std::vector<double> fCellPMTX;
	std::vector<double> fCellPMTY;

	std::vector<WCSimGeometryEnums::PMTDirection_t> fDirectionType;
	std::vector<double> fCellDirTheta;
	std::vector<double> fCellDirPhi;

	double fThetaStart;
	double fThetaEnd;
};

#endif /* WCSIMDETECTORZONE_HH_ */
