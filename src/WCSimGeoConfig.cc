#include <iostream>

#include "globals.hh"

#include "WCSimGeoConfig.hh"

// Default constructor
WCSimGeoConfig::WCSimGeoConfig(){

	fGeoName = "";
	fInnerRadius = 0.;
	fInnerHeight = 0.;
	fNSides = 0;

	fPMTName1 = "";
	fPercentCoverage = 0.;
}

// Copy constructor
WCSimGeoConfig::WCSimGeoConfig(const WCSimGeoConfig &rhs){

	fGeoName = rhs.GetGeoName();
	fInnerRadius = rhs.GetInnerRadius();
	fInnerHeight = rhs.GetInnerHeight();
	fNSides = rhs.GetNSides();

	fPMTName1 = rhs.GetPMTName1();
	fPercentCoverage = rhs.GetCoverage();

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

// First PMT Name

std::string WCSimGeoConfig::GetPMTName1() const{
	return fPMTName1;
}

void WCSimGeoConfig::SetPMTName1(std::string name){
	fPMTName1 = name;
}

// PMT Coverage

double WCSimGeoConfig::GetCoverage() const{
	return fPercentCoverage;
}

void WCSimGeoConfig::SetCoverage(double coverage){
	fPercentCoverage = coverage;
}

// Print out the PMT details
void WCSimGeoConfig::Print() const{

	std::cout << "== WCSimGeoConfig object ==" << std::endl;
	std::cout << "\tName = " << this->GetGeoName() << std::endl;
	std::cout << "\tInner Radius = " << this->GetInnerRadius()/m << "m" << std::endl;
	std::cout << "\tInner Height = " << this->GetInnerHeight()/m << "m" << std::endl;
	std::cout << "\t# of Sides   = " << this->GetNSides() << std::endl;
	std::cout << "\tPMT 1 = " << this->GetPMTName1() << std::endl;
	std::cout << "\tCoverage = " << this->GetCoverage() << std::endl;
}


