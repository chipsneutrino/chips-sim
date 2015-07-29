#include <iostream>

#include "globals.hh"
#include "math.h"

#include "WCSimPMTConfig.hh"
#include "WCSimLCConfig.hh"
#include "WCSimLCManager.hh"

#ifndef REFLEX_DICTIONARY
ClassImp(WCSimPMTConfig)
#endif
// Default (and only) constructor
WCSimPMTConfig::WCSimPMTConfig(){

	fRadius = 0.;
	fExposeHeight = 0.;
	fGlassThickness = 0.;
	fEffBins = 0;
	fMaxEff = 0;
	fPMTName = "";
	fLCName = "";

}

WCSimPMTConfig::WCSimPMTConfig(const WCSimPMTConfig &rhs){

	fRadius = rhs.GetRadius();
	fExposeHeight = rhs.GetExposeHeight();
	fGlassThickness = rhs.GetGlassThickness();
	this->SetEfficiencyVector(rhs.GetEfficiencyVector());
	fPMTName = rhs.GetPMTName();
	fLCName = rhs.GetLCName();
	fLCConfig = rhs.GetLCConfig();

}

// Destructor
WCSimPMTConfig::~WCSimPMTConfig(){

}

// Radius

double WCSimPMTConfig::GetRadius() const{
	return fRadius;
}

void WCSimPMTConfig::SetRadius(double radius){
	fRadius = radius;
}

// Photocathode area
double WCSimPMTConfig::GetArea() const{
  return M_PI * fRadius * fRadius;
}

// Exposed height 

double WCSimPMTConfig::GetExposeHeight() const{
	return fExposeHeight;
}

void WCSimPMTConfig::SetExposeHeight(double height){
	fExposeHeight = height;
}

// Glass Thickness

double WCSimPMTConfig::GetGlassThickness() const{
	return fGlassThickness;
}

void WCSimPMTConfig::SetGlassThickness(double glass){
	fGlassThickness = glass;
}

// Timing Resolution

double WCSimPMTConfig::GetTimeConstant() const{
	return fTimeConstant;
}

void WCSimPMTConfig::SetTimeConstant(double timeConst){
	fTimeConstant = timeConst;
}

// Efficiency

std::vector<std::pair<double,double> > WCSimPMTConfig::GetEfficiencyVector() const{
	return fEffVec; 
}

void WCSimPMTConfig::SetEfficiencyVector(std::vector<std::pair<double,double> > eff){
	fEffVec = eff;

	// Now loop over and get the maximum value
	fEffBins = fEffVec.size();
	fMaxEff = -999.;
	for(unsigned int v = 0; v < fEffBins; ++v){
		if(fEffVec[v].second > fMaxEff){
			fMaxEff = fEffVec[v].second;
		}
	}
}

double WCSimPMTConfig::GetMaxEfficiency() const{
	return fMaxEff;
}

unsigned int WCSimPMTConfig::GetNEfficiencyBins() const{
	return fEffBins;
}

// Name

std::string WCSimPMTConfig::GetPMTName() const{
	return fPMTName;
}

void WCSimPMTConfig::SetPMTName(std::string name){
	fPMTName = name;
}

std::string WCSimPMTConfig::GetLCName() const{
	return fLCName;
}

void WCSimPMTConfig::SetLCName(std::string name){
	fLCName = name;
}

WCSimLCConfig WCSimPMTConfig::GetLCConfig() const{
	return fLCConfig;
}

void WCSimPMTConfig::SetLCConfig(std::string name){
        this->SetLCName(name);

	fLCConfig = fLCManager.GetLCByName(name);

}

// Print out the PMT details
void WCSimPMTConfig::Print() const{

	std::cout << "== WCSimPMTConfig object ==" << std::endl;
	std::cout << "\tName = " << this->GetPMTName() << std::endl;
	std::cout << "\tRadius = " << this->GetRadius()/m << "m" << std::endl;
	std::cout << "\tExpose Height = " << this->GetExposeHeight()/m << "m" << std::endl;
	std::cout << "\tGlass Thickness = " << this->GetGlassThickness()/m << "m" << std::endl;
	if(this->GetLCName() != "") 
	  std::cout << "\tLight Collector = " << this->GetLCName() << std::endl;
	std::cout << "\tEfficiency @ Wavelength(nm):" << std::endl;
	for(unsigned int i = 0; i < this->GetNEfficiencyBins(); ++i){
		std::cout << "\t\t" << fEffVec[i].second << " at " << fEffVec[i].first << "nm" << std::endl;
	}
}


