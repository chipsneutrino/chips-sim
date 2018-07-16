/*
 * WCSimPMTZone.cc
 *
 *  Created on: 24 Nov 2014
 *      Author: ajperch
 */
#include "G4ThreeVector.hh"
#include "WCSimDetectorZone.hh"
#include "WCSimGeometryEnums.hh"

#include <cassert>
#include <vector>

using namespace WCSimGeometryEnums;

WCSimDetectorZone::WCSimDetectorZone() {
	fCoverage = 1.;
	fThetaStart = -999.9;
	fThetaEnd = -999.9;
	// TODO Auto-generated constructor stub

}

WCSimDetectorZone::~WCSimDetectorZone() {
	// TODO Auto-generated destructor stub
}

// Unit cell PMTs

void WCSimDetectorZone::AddCellPMTName(std::string name) {
	fCellPMTName.push_back(name);
}

std::string WCSimDetectorZone::GetCellPMTName(unsigned int pmt) const {
	assert(pmt < fCellPMTName.size() && "PMT number is out of range");
	return fCellPMTName.at(pmt);
}

std::vector<std::string> WCSimDetectorZone::GetCellPMTName() const {
//    std::cout << "Getting PMTName vector, of size " << std::cout << fCellPMTName.size() << std::endl;
	return fCellPMTName;
}

void WCSimDetectorZone::AddCellPMTX(double x) {
	fCellPMTX.push_back(x);
}

double WCSimDetectorZone::GetCellPMTX(unsigned int pmt) const {
	assert(pmt < fCellPMTX.size() && "PMT number is out of range");
	return fCellPMTX.at(pmt);
}

std::vector<double> WCSimDetectorZone::GetCellPMTX() const {
//    std::cout << "Getting PMTX vector, of size " << std::cout << (unsigned int)fCellPMTX.size() << std::endl;
	std::vector<double> myvec;
	myvec.push_back(2.0);
	return fCellPMTX;
}

void WCSimDetectorZone::AddCellPMTY(double y) {
	fCellPMTY.push_back(y);
}

double WCSimDetectorZone::GetCellPMTY(unsigned int pmt) const {
	assert(pmt < fCellPMTX.size() && "PMT number is out of range");
	return fCellPMTX.at(pmt);
}

std::vector<double> WCSimDetectorZone::GetCellPMTY() const {
	return fCellPMTY;
}

bool WCSimDetectorZone::IsGood(WCSimGeometryEnums::DetectorRegion_t region) const {
	bool isGood = true;
	if (fCellPMTX.size() != fCellPMTY.size()) {
		std::cerr << "Must have the same number of PMT x and y coordinates (" << fCellPMTX.size() << " vs "
				<< fCellPMTY.size() << std::endl;
		isGood = false;
	}
	if (fCellPMTX.size() != fCellPMTName.size()) {
		std::cerr << "Must have the same number of PMT x coordinates as names (" << fCellPMTX.size() << " vs "
				<< fCellPMTName.size() << std::endl;
		isGood = false;
	}
	if (fCellPMTY.size() != fCellPMTName.size()) {
		std::cerr << "Must have the same number of PMT y coordinates as names (" << fCellPMTY.size() << " vs "
				<< fCellPMTName.size() << std::endl;
		isGood = false;
	}
	if (fDirectionType.size() != fCellPMTName.size()) {
		std::cerr << "Must have the same number of direction types as names (" << fDirectionType.size() << " vs "
				<< fCellPMTName.size() << std::endl;
		isGood = false;
	}
	if (fCellDirTheta.size() != fCellPMTName.size()) {
		std::cerr << "Must have the same number of PMT theta directions as names (" << fCellDirTheta.size() << " vs "
				<< fCellPMTName.size() << std::endl;
		isGood = false;
	}
	if (fCellDirPhi.size() != fCellPMTName.size()) {
		std::cerr << "Must have the same number of PMT phi directions as names (" << fCellDirPhi.size() << " vs "
				<< fCellPMTName.size() << std::endl;
		isGood = false;
	}

	if (region == WCSimGeometryEnums::DetectorRegion_t::kTop
			|| region == WCSimGeometryEnums::DetectorRegion_t::kBottom) {
		if (fThetaStart == fThetaEnd || fThetaStart == -999.9 || fThetaEnd == -999.9) {
			std::cerr << "Must set start and end theta values for top and bottom zones" << std::endl;
			std::cerr << "fThetaStart = " << fThetaStart << "   fThetaEnd = " << fThetaEnd << std::endl;
			isGood = false;
		}
	}

	return isGood;
}

void WCSimDetectorZone::Print() const {
	std::cout << "\t\t Photocathode coverage " << fCoverage << std::endl;
	std::cout << "\t\tPMT locations in the unit cell:" << std::endl;
	for (unsigned int iPMT = 0; iPMT < fCellPMTName.size(); ++iPMT) {
		std::cout << "\t\t\t" << fCellPMTName.at(iPMT) << " at (" << fCellPMTX.at(iPMT) / m << ", "
				<< fCellPMTY.at(iPMT) / m << ")" << std::endl;
		std::cout << "\t\t\t" << "fDirectionType = " << fDirectionType.at(iPMT).AsString() << " facing ("
				<< fCellDirTheta.at(iPMT) << "," << fCellDirPhi.at(iPMT) << ")" << std::endl << std::endl;
	}
}

void WCSimDetectorZone::AddCellPMTFaceType(WCSimGeometryEnums::PMTDirection_t face) {
	fDirectionType.push_back(face);
	return;
}

WCSimGeometryEnums::PMTDirection_t WCSimDetectorZone::GetCellPMTFaceType(unsigned int pmt) const {
	return fDirectionType.at(pmt);
}

std::vector<PMTDirection_t> WCSimDetectorZone::GetCellPMTFaceType() const {
	return fDirectionType;
}

void WCSimDetectorZone::AddCellPMTFaceTheta(double theta) {
	fCellDirTheta.push_back(theta);
	return;
}

double WCSimDetectorZone::GetCellPMTFaceTheta(unsigned int pmt) const {
	return fCellDirTheta.at(pmt);
}

std::vector<double> WCSimDetectorZone::GetCellPMTFaceTheta() const {
	return fCellDirTheta;
}

void WCSimDetectorZone::AddCellPMTFacePhi(double phi) {
	fCellDirPhi.push_back(phi);
	return;
}

double WCSimDetectorZone::GetCellPMTFacePhi(unsigned int pmt) const {
	return fCellDirPhi.at(pmt);
}

std::vector<double> WCSimDetectorZone::GetCellPMTPhi() const {
	return fCellDirPhi;
}

void WCSimDetectorZone::SetPMTLimit(std::string name, int limit) {
	std::vector<std::string>::iterator limItr = std::find(fCellPMTName.begin(), fCellPMTName.end(), name);
	if (limItr != fCellPMTName.end()) {
		fPMTLimit[name] = limit;
	} else {
		std::cerr << "Error: could not find PMT called " << name << " to apply limit to" << std::endl;
		assert(limItr != fCellPMTName.end());
	}
	fCoverage = 1.0;
}

int WCSimDetectorZone::GetPMTLimit(std::string name) const {
	std::map<std::string, int>::const_iterator limItr = fPMTLimit.find(name);
	int limit = 0;
	if (limItr != fPMTLimit.end()) {
		limit = limItr->second;
	} else {
		std::cerr << "Error: could not find limit for PMT named " << name << std::endl;
		assert(0);
	}
	return limit;
}

int WCSimDetectorZone::GetMaxNumCells() const {
	std::map<std::string, int> numPMTs;
	std::vector<std::string>::const_iterator pmtIter = fCellPMTName.begin();
	for (; pmtIter != fCellPMTName.end(); pmtIter++) {
		if (numPMTs.find(*pmtIter) == numPMTs.end()) {
			numPMTs[*pmtIter] = 0;
		}
		numPMTs[*pmtIter] = numPMTs[*pmtIter] + 1;
	}

	int maxCells = 0;
	std::map<std::string, int>::const_iterator limitItr = fPMTLimit.begin();
	for (; limitItr != fPMTLimit.end(); ++limitItr) {
		int numCells = (*limitItr).second / (numPMTs[(*limitItr).first]);
		if (maxCells == 0 || numCells < maxCells) {
			maxCells = numCells;
		}
	}

	return maxCells;
}
