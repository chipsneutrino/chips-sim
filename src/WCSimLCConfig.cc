#include <iostream>

#include "globals.hh"
#include "math.h"

#include "WCSimLCConfig.hh"

// Default (and only) constructor
WCSimLCConfig::WCSimLCConfig(){

	fMaxRadius = 0.;
	fExposeHeight = 0.;
	fShapePoints = 0;
	fName = "";

}

WCSimLCConfig::WCSimLCConfig(const WCSimLCConfig &rhs){

	fMaxRadius = rhs.GetMaxRadius();
	fExposeHeight = rhs.GetExposeHeight();
	this->SetShapeVector(rhs.GetShapeVector());
	fName = rhs.GetName();
}

// Destructor
WCSimLCConfig::~WCSimLCConfig(){

}


// Polycone Shape

std::vector<std::pair<double,double> > WCSimLCConfig::GetShapeVector() const{
	return fShapeVec; 
}

void WCSimLCConfig::SetShapeVector(std::vector<std::pair<double,double> > shape){
	fShapeVec = shape;

	// Now loop over and get the maximum value
	fShapePoints = fShapeVec.size();
	fMaxRadius     = -999.;
	fExposeHeight = -999.;
	for(unsigned int v = 0; v < fShapePoints; ++v){
	        if(fShapeVec[v].first > fExposeHeight){
			fExposeHeight = fShapeVec[v].first;
		}
		if(fShapeVec[v].second > fMaxRadius){
			fMaxRadius = fShapeVec[v].second;
		}
	}
}


// Max Radius

double WCSimLCConfig::GetMaxRadius() const{
	return fMaxRadius;
}

// Exposed height 

double WCSimLCConfig::GetExposeHeight() const{
	return fExposeHeight;
}

// N Shape Points
unsigned int WCSimLCConfig::GetNShapePoints() const{
	return fShapePoints;
}

// Name

std::string WCSimLCConfig::GetName() const{
	return fName;
}

void WCSimLCConfig::SetName(std::string name){
	fName = name;
}

// Print out the LC details
void WCSimLCConfig::Print() const{

	std::cout << "== WCSimLCConfig object ==" << std::endl;
	std::cout << "\tName = " << this->GetName() << std::endl;
	std::cout << "\tMax Radius = " << this->GetMaxRadius()/m << "m" << std::endl;
	std::cout << "\tExpose Height = " << this->GetExposeHeight()/m << "m" << std::endl;
	std::cout << "\tHeight @ R (m):" << std::endl;
	for(unsigned int i = 0; i < this->GetNShapePoints(); ++i){
		std::cout << "\t\t" << fShapeVec[i].second/m << " - " << fShapeVec[i].first/m << "m" << std::endl;
	}
}


