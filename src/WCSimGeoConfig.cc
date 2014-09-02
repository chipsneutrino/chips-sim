#include <iostream>

#include "globals.hh"
#include <cassert>

#include "WCSimGeoConfig.hh"

// Default constructor
WCSimGeoConfig::WCSimGeoConfig(){

	fGeoName = "";
	fInnerRadius = 0.;
	fInnerHeight = 0.;
	fNSides = 0;
	fPercentCoverage = 0.;
}

// Copy constructor
WCSimGeoConfig::WCSimGeoConfig(const WCSimGeoConfig &rhs){

	fGeoName = rhs.GetGeoName();
	fInnerRadius = rhs.GetInnerRadius();
	fInnerHeight = rhs.GetInnerHeight();
	fNSides = rhs.GetNSides();
    fPercentCoverage = rhs.GetCoverage();

	fCellPMTName = rhs.GetCellPMTName();
	fCellPMTX = rhs.GetCellPMTX();
	fCellPMTY = rhs.GetCellPMTY();
}

// Destructor
WCSimGeoConfig::~WCSimGeoConfig(){

}

// Geometry Name

std::string WCSimGeoConfig::GetGeoName() const{
	return fGeoName;
}

void WCSimGeoConfig::SetGeoName(std::string name){
	fGeoName = name;
}

// Inner Radius

double WCSimGeoConfig::GetInnerRadius() const{
	return fInnerRadius;
}

void WCSimGeoConfig::SetInnerRadius(double radius){
	fInnerRadius = radius;
}

// Inner height 

double WCSimGeoConfig::GetInnerHeight() const{
	return fInnerHeight;
}

void WCSimGeoConfig::SetInnerHeight(double height){
	fInnerHeight = height;
}

// Number of sides of the polygon forming the cross-sectional area

unsigned int WCSimGeoConfig::GetNSides() const{
	return fNSides;
}

void WCSimGeoConfig::SetNSides(unsigned int nsides){
	fNSides = nsides;
}

// PMT Coverage

double WCSimGeoConfig::GetCoverage() const{
	return fPercentCoverage;
}

double WCSimGeoConfig::GetCoverageFraction() const{
	return fPercentCoverage / 100.;
}

void WCSimGeoConfig::SetCoverage(double coverage){
	fPercentCoverage = coverage;
}

// Unit cell PMTs

void WCSimGeoConfig::AddCellPMTName(std::string name){
    fCellPMTName.push_back(name);
}

std::string WCSimGeoConfig::GetCellPMTName(unsigned int pmt) const{
    assert( pmt < fCellPMTName.size() && "PMT number is out of range");
    return fCellPMTName.at(pmt);
}

std::vector<std::string> WCSimGeoConfig::GetCellPMTName() const{
    std::cout << "Getting PMTName vector, of size " << std::cout << fCellPMTName.size() << std::endl;
    return fCellPMTName;
}

void WCSimGeoConfig::AddCellPMTX(double x){
    fCellPMTX.push_back(x);
}

double WCSimGeoConfig::GetCellPMTX(unsigned int pmt) const{
    assert( pmt < fCellPMTX.size() && "PMT number is out of range");
    return fCellPMTX.at(pmt);
}

std::vector<double> WCSimGeoConfig::GetCellPMTX() const{
    std::cout << "Getting PMTX vector, of size " << std::cout << (unsigned int)fCellPMTX.size() << std::endl;
    std::vector<double> myvec;
    myvec.push_back(2.0);
    std::cout << "Size of this weird vector = " << myvec.size() << std::endl;
    return fCellPMTX;
}


void WCSimGeoConfig::AddCellPMTY(double y){
    fCellPMTY.push_back(y);
}

double WCSimGeoConfig::GetCellPMTY(unsigned int pmt) const{
    assert( pmt < fCellPMTX.size() && "PMT number is out of range");
    return fCellPMTX.at(pmt);
}

std::vector<double> WCSimGeoConfig::GetCellPMTY() const{
    return fCellPMTY;
}

bool WCSimGeoConfig::IsGood() const{
    bool isGood = true;

    if( fCellPMTX.size() != fCellPMTY.size()){
            std::cerr << "Must have the same number of PMT x and y coordinates ("
                      << fCellPMTX.size() << " vs " << fCellPMTY.size() << std::endl;
            isGood = false;
    }
    if( fCellPMTX.size() != fCellPMTName.size()){
            std::cerr << "Must have the same number of PMT x coordinates as names ("
                      << fCellPMTX.size() << " vs " << fCellPMTName.size() << std::endl;
            isGood = false;
    }
    if( fCellPMTY.size() != fCellPMTName.size()){
            std::cerr << "Must have the same number of PMT y coordinates as names ("
                      << fCellPMTY.size() << " vs " << fCellPMTName.size() << std::endl;
            isGood = false;
    }


    if( !isGood ) { std::cerr << "Check the geometry xml file!" << std::endl; }
    return isGood;
}

// Print out the PMT details
void WCSimGeoConfig::Print() const{

	std::cout << "== WCSimGeoConfig object ==" << std::endl;
	std::cout << "\tName = " << this->GetGeoName() << std::endl;
	std::cout << "\tInner Radius = " << this->GetInnerRadius()/m << "m" << std::endl;
	std::cout << "\tInner Height = " << this->GetInnerHeight()/m << "m" << std::endl;
	std::cout << "\t# of Sides   = " << this->GetNSides() << std::endl;
	std::cout << "\tCoverage = " << this->GetCoverage() << std::endl;

	assert(IsGood());
    std::cout << "\tPMT locations in the unit cell:" << std::endl;
    for(unsigned int iPMT = 0; iPMT < fCellPMTName.size(); ++iPMT)
    {
        std::cout << "\t\t" << fCellPMTName.at(iPMT) << " at (" << fCellPMTX.at(iPMT) / m
                  << ", " << fCellPMTY.at(iPMT) / m << ")" << std::endl;
    }
}


