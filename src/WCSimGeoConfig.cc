#include <iostream>

#include "globals.hh"
#include <cassert>
#include <math.h>
#include <map>

#include "WCSimGeoConfig.hh"
#include "WCSimDetectorZone.hh"
#include "WCSimGeometryEnums.hh"


// Default constructor
WCSimGeoConfig::WCSimGeoConfig(){
  fPMTLimit = WCSimGeometryEnums::PhotodetectorLimit_t::kPercentCoverage;
	fGeoName = "";
	fOuterRadius = 0.;
	fInnerHeight = 0.;
	fNSides = 0;
	fPercentCoverage = 0.;
}

// Copy constructor
WCSimGeoConfig::WCSimGeoConfig(const WCSimGeoConfig &rhs){

	fGeoName = rhs.GetGeoName();
	fOuterRadius = rhs.GetOuterRadius();
	fInnerHeight = rhs.GetInnerHeight();
	fNSides = rhs.GetNSides();
  fPercentCoverage = rhs.GetCoverage();

  fPMTLimit = rhs.fPMTLimit;
  fZoneMap = rhs.fZoneMap;

	fPMTNamesUsed = rhs.GetPMTNamesUsed();
	fCurrentRegions = rhs.fCurrentRegions;
	fCurrentZones = rhs.fCurrentZones;


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
  return fOuterRadius * cos(M_PI / fNSides);
}

double WCSimGeoConfig::GetOuterRadius() const{
	return fOuterRadius;
}

void WCSimGeoConfig::SetOuterRadius(double radius){
	fOuterRadius = radius;
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



bool WCSimGeoConfig::IsGood() const{
    bool isGood = true;

    std::map<std::pair<WCSimGeometryEnums::DetectorRegion_t, int>, WCSimDetectorZone>::iterator zoneItr;
    std::vector<WCSimGeometryEnums::DetectorRegion_t> regions = WCSimGeometryEnums::DetectorRegion_t::GetAllTypes();
    for( std::vector<WCSimGeometryEnums::DetectorRegion_t>::const_iterator regItr = regions.begin(); regItr != regions.end(); ++regItr)
    {
    	for(unsigned int i = 0; i < fNSides; ++i)
    	{
    		std::pair<WCSimGeometryEnums::DetectorRegion_t, int> key = std::make_pair(*regItr, i);
    		if(fZoneMap.find(key) == fZoneMap.end())
    		{
    			std::cerr << "Error: could not find entry for region " << (*regItr).AsString() << " zone " << i << std::endl;
    			isGood = false;
    		}
    		else
    		{
    			isGood &= IsGoodZone(key);
    		}
    	}
    }

    if( !isGood ) { std::cerr << "Check the geometry xml file!" << std::endl; }
    return isGood;
}

bool WCSimGeoConfig::IsGoodZone(std::pair<WCSimGeometryEnums::DetectorRegion_t, int> zone) const
{
	bool isGoodZone = false;
  std::map<std::pair<WCSimGeometryEnums::DetectorRegion_t, int>, WCSimDetectorZone>::const_iterator zoneItr = fZoneMap.find(zone);
	if( zoneItr != fZoneMap.end())
	{
		isGoodZone = (*zoneItr).second.IsGood();
	}
	return isGoodZone;
}

// Print out the PMT details
void WCSimGeoConfig::Print() const{

	std::cout << "== WCSimGeoConfig object ==" << std::endl;
	std::cout << "\tName = " << this->GetGeoName() << std::endl;
	std::cout << "\tOuter Radius = " << this->GetOuterRadius()/m << "m" << std::endl;
	std::cout << "\tInner Height = " << this->GetInnerHeight()/m << "m" << std::endl;
	std::cout << "\t# of Sides   = " << this->GetNSides() << std::endl;
	std::cout << "\tCoverage = " << this->GetCoverage() << std::endl;
	assert(IsGood());

	std::map<std::pair<WCSimGeometryEnums::DetectorRegion_t, int>, WCSimDetectorZone>::const_iterator zoneItr = fZoneMap.begin();
	for( ; zoneItr != fZoneMap.end(); ++zoneItr)
	{
		std::cout << "\t\t Region is " << (*zoneItr).first.first.AsString() << " and zone number = " << (*zoneItr).first.second << std::endl;
		(*zoneItr).second.Print();
		std::cout << std::endl;
	}

}

void WCSimGeoConfig::AddCurrentRegion(std::string regionName)
{
	WCSimGeometryEnums::DetectorRegion_t region(regionName);
	std::vector<WCSimGeometryEnums::DetectorRegion_t>::iterator regionItr = std::find(fCurrentRegions.begin(), fCurrentRegions.end(), region);
	if( regionItr == fCurrentRegions.end())
	{
		fCurrentRegions.push_back(region);
	}
	else
	{
		std::cerr << "Warning: Current region is already being build.  Trying to continue..." << std::endl;
	}
}

void WCSimGeoConfig::AddCurrentZone(int zoneNum)
{
	std::vector<int>::iterator zoneItr = std::find(fCurrentZones.begin(), fCurrentZones.end(), zoneNum);
	if( zoneItr == fCurrentZones.end())
	{
		fCurrentZones.push_back(zoneNum);
	}
	else
	{
		std::cerr << "Warning: Current zone is already being build.  Trying to continue..." << std::endl;
	}
}

void WCSimGeoConfig::ResetCurrent()
{
	fPMTNamesUsed.clear();
	fCurrentRegions.clear();
	fCurrentZones.clear();
}

void WCSimGeoConfig::AddCellPMTName(std::string name)
{
	std::vector<WCSimGeometryEnums::DetectorRegion_t>::iterator regionItr = fCurrentRegions.begin();
	std::vector<int>::iterator zoneItr = fCurrentZones.begin();

	for( ; regionItr != fCurrentRegions.end(); ++regionItr)
	{
		for( zoneItr = fCurrentZones.begin(); zoneItr != fCurrentZones.end(); ++zoneItr )
		{
			AddCellPMTName(*regionItr, *zoneItr, name);
		}
	}

	if(std::find(fPMTNamesUsed.begin(), fPMTNamesUsed.end(), name) == fPMTNamesUsed.end())
	{
		fPMTNamesUsed.push_back(name);
	}
}

void WCSimGeoConfig::AddCellPMTX(double x)
{
	std::vector<WCSimGeometryEnums::DetectorRegion_t>::iterator regionItr = fCurrentRegions.begin();
	std::vector<int>::iterator zoneItr = fCurrentZones.begin();

	for( ; regionItr != fCurrentRegions.end(); ++regionItr)
	{
		for( zoneItr = fCurrentZones.begin(); zoneItr != fCurrentZones.end(); ++zoneItr )
		{
			AddCellPMTX(*regionItr, *zoneItr, x);
		}
	}
}

void WCSimGeoConfig::AddCellPMTY(double y)
{
	std::vector<WCSimGeometryEnums::DetectorRegion_t>::iterator regionItr = fCurrentRegions.begin();
	std::vector<int>::iterator zoneItr = fCurrentZones.begin();

	for( ; regionItr != fCurrentRegions.end(); ++regionItr)
	{
		for( zoneItr = fCurrentZones.begin(); zoneItr != fCurrentZones.end(); ++zoneItr )
		{
			AddCellPMTY(*regionItr, *zoneItr, y);
		}
	}
}

void WCSimGeoConfig::AddCellPMTFaceType(WCSimGeometryEnums::PMTDirection_t type)
{
	std::vector<WCSimGeometryEnums::DetectorRegion_t>::iterator regionItr = fCurrentRegions.begin();
	std::vector<int>::iterator zoneItr = fCurrentZones.begin();

	for( ; regionItr != fCurrentRegions.end(); ++regionItr)
	{
		for( zoneItr = fCurrentZones.begin(); zoneItr != fCurrentZones.end(); ++zoneItr )
		{
			AddCellPMTFaceType(*regionItr, *zoneItr, type);
		}
	}
}

void WCSimGeoConfig::AddCellPMTFaceTheta(double theta)
{
	std::vector<WCSimGeometryEnums::DetectorRegion_t>::iterator regionItr = fCurrentRegions.begin();
	std::vector<int>::iterator zoneItr = fCurrentZones.begin();

	for( ; regionItr != fCurrentRegions.end(); ++regionItr)
	{
		for( zoneItr = fCurrentZones.begin(); zoneItr != fCurrentZones.end(); ++zoneItr )
		{
			AddCellPMTFaceTheta(*regionItr, *zoneItr, theta);
		}
	}
}

void WCSimGeoConfig::AddCellPMTFacePhi(double phi)
{
	std::vector<WCSimGeometryEnums::DetectorRegion_t>::iterator regionItr = fCurrentRegions.begin();
	std::vector<int>::iterator zoneItr = fCurrentZones.begin();

	for( ; regionItr != fCurrentRegions.end(); ++regionItr)
	{
		for( zoneItr = fCurrentZones.begin(); zoneItr != fCurrentZones.end(); ++zoneItr )
		{
			AddCellPMTFacePhi(*regionItr, *zoneItr, phi);
		}
	}
}

std::vector<std::string> WCSimGeoConfig::GetPMTNamesUsed() const
{
	return fPMTNamesUsed;
}

void WCSimGeoConfig::AddCellPMTName(WCSimGeometryEnums::DetectorRegion_t region,
		int zone, std::string name)
{
	CreateMissingZone(region, zone);
	fZoneMap[std::make_pair(region, zone)].AddCellPMTName(name);
}

std::string WCSimGeoConfig::GetCellPMTName(
		WCSimGeometryEnums::DetectorRegion_t region, int zone,
		unsigned int pmt) const
{
	std::string name("");
	std::pair<WCSimGeometryEnums::DetectorRegion_t, int> myPair = std::make_pair(region, zone);
	std::map<std::pair<WCSimGeometryEnums::DetectorRegion_t, int>, WCSimDetectorZone>::const_iterator zoneItr = fZoneMap.find(myPair);
	if(zoneItr != fZoneMap.end())
	{
		name = (*zoneItr).second.GetCellPMTName(pmt);
	}
	else
	{
		std::cerr << "Error: Could not find PMT number " << pmt << " in region " << region.AsString() << ", zone " << zone << std::endl;
		assert(0);
	}
	return name;
}

std::vector<std::string> WCSimGeoConfig::GetCellPMTName(
		WCSimGeometryEnums::DetectorRegion_t region, int zone) const
{
	std::vector<std::string> pmtNames;
	std::pair<WCSimGeometryEnums::DetectorRegion_t, int> myPair = std::make_pair(region, zone);
	std::map<std::pair<WCSimGeometryEnums::DetectorRegion_t, int>, WCSimDetectorZone>::const_iterator zoneItr = fZoneMap.find(myPair);
	if(zoneItr != fZoneMap.end())
	{
		pmtNames = (*zoneItr).second.GetCellPMTName();
	}
	else
	{
		std::cerr << "Error: Could not find region " << region.AsString() << ", zone " << zone << std::endl;
		assert(0);
	}
	return pmtNames;
}

void WCSimGeoConfig::AddCellPMTX(WCSimGeometryEnums::DetectorRegion_t region,
		int zone, double x)
{
	CreateMissingZone(region, zone);
	fZoneMap[std::make_pair(region, zone)].AddCellPMTX(x);
}

double WCSimGeoConfig::GetCellPMTX(WCSimGeometryEnums::DetectorRegion_t region,
		int zone, unsigned int pmt) const
{
	double x = -999.9;
	std::pair<WCSimGeometryEnums::DetectorRegion_t, int> myPair = std::make_pair(region, zone);
	std::map<std::pair<WCSimGeometryEnums::DetectorRegion_t, int>, WCSimDetectorZone>::const_iterator zoneItr = fZoneMap.find(myPair);
	if(zoneItr != fZoneMap.end())
	{
		x = (*zoneItr).second.GetCellPMTX(pmt);
	}
	else
	{
		std::cerr << "Error: Could not find PMT number " << pmt << " in region " << region.AsString() << ", zone " << zone << std::endl;
		assert(0);
	}
	return x;
}

std::vector<double> WCSimGeoConfig::GetCellPMTX(
		WCSimGeometryEnums::DetectorRegion_t region, int zone) const
{
	std::vector<double> pmtX;
	std::pair<WCSimGeometryEnums::DetectorRegion_t, int> myPair = std::make_pair(region, zone);
	std::map<std::pair<WCSimGeometryEnums::DetectorRegion_t, int>, WCSimDetectorZone>::const_iterator zoneItr = fZoneMap.find(myPair);
	if(zoneItr != fZoneMap.end())
	{
		pmtX = (*zoneItr).second.GetCellPMTX();
	}
	else
	{
		std::cerr << "Error: Could not find region " << region.AsString() << ", zone " << zone << std::endl;
		assert(0);
	}
	return pmtX;
}

void WCSimGeoConfig::AddCellPMTY(WCSimGeometryEnums::DetectorRegion_t region,
		int zone, double y)
{
	CreateMissingZone(region, zone);
	fZoneMap[std::make_pair(region, zone)].AddCellPMTY(y);
}

double WCSimGeoConfig::GetCellPMTY(WCSimGeometryEnums::DetectorRegion_t region,
		int zone, unsigned int pmt) const
{
	double y = -999.9;
	std::pair<WCSimGeometryEnums::DetectorRegion_t, int> myPair = std::make_pair(region, zone);
	std::map<std::pair<WCSimGeometryEnums::DetectorRegion_t, int>, WCSimDetectorZone>::const_iterator zoneItr = fZoneMap.find(myPair);
	if(zoneItr != fZoneMap.end())
	{
		y = (*zoneItr).second.GetCellPMTY(pmt);
	}
	else
	{
		std::cerr << "Error: Could not find PMT number " << pmt << " in region " << region.AsString() << ", zone " << zone << std::endl;
		assert(0);
	}
	return y;
}

std::vector<double> WCSimGeoConfig::GetCellPMTY(
		WCSimGeometryEnums::DetectorRegion_t region, int zone) const
{
	std::vector<double> pmtY;
	std::pair<WCSimGeometryEnums::DetectorRegion_t, int> myPair = std::make_pair(region, zone);
	std::map<std::pair<WCSimGeometryEnums::DetectorRegion_t, int>, WCSimDetectorZone>::const_iterator zoneItr = fZoneMap.find(myPair);
	if(zoneItr != fZoneMap.end())
	{
		pmtY = (*zoneItr).second.GetCellPMTY();
	}
	else
	{
		std::cerr << "Error: Could not find region " << region.AsString() << ", zone " << zone << std::endl;
		assert(0);
	}
	return pmtY;
}

void WCSimGeoConfig::AddCellPMTFaceType(
		WCSimGeometryEnums::DetectorRegion_t region, int zone, WCSimGeometryEnums::PMTDirection_t type)
{
	CreateMissingZone(region, zone);
	fZoneMap[std::make_pair(region, zone)].AddCellPMTFaceType(type);
}

double WCSimGeoConfig::GetCellPMTFaceType(
		WCSimGeometryEnums::DetectorRegion_t region, int zone,
		unsigned int pmt) const
{
	WCSimGeometryEnums::PMTDirection_t dir;
	std::pair<WCSimGeometryEnums::DetectorRegion_t, int> myPair = std::make_pair(region, zone);
	std::map<std::pair<WCSimGeometryEnums::DetectorRegion_t, int>, WCSimDetectorZone>::const_iterator zoneItr = fZoneMap.find(myPair);
	if(zoneItr != fZoneMap.end())
	{
		WCSimDetectorZone zone = ((*zoneItr).second);
	  dir = zone.GetCellPMTFaceType(pmt);
	}
	else
	{
		std::cerr << "Error: Could not find PMT number " << pmt << " in region " << region.AsString() << ", zone " << zone << std::endl;
		assert(0);
	}
	return dir;
}

std::vector<WCSimGeometryEnums::PMTDirection_t> WCSimGeoConfig::GetCellPMTFaceType(
		WCSimGeometryEnums::DetectorRegion_t region, int zone) const
{
	std::vector<WCSimGeometryEnums::PMTDirection_t> pmtFace;
	std::pair<WCSimGeometryEnums::DetectorRegion_t, int> myPair = std::make_pair(region, zone);
	std::map<std::pair<WCSimGeometryEnums::DetectorRegion_t, int>, WCSimDetectorZone>::const_iterator zoneItr = fZoneMap.find(myPair);
	if(zoneItr != fZoneMap.end())
	{
		pmtFace = (*zoneItr).second.GetCellPMTFaceType();
	}
	else
	{
		std::cerr << "Error: Could not find region " << region.AsString() << ", zone " << zone << std::endl;
		assert(0);
	}
	return pmtFace;
}

void WCSimGeoConfig::AddCellPMTFaceTheta(
		WCSimGeometryEnums::DetectorRegion_t region, int zone, double theta)
{
	CreateMissingZone(region, zone);
	fZoneMap[std::make_pair(region, zone)].AddCellPMTFaceTheta(theta);
}

double WCSimGeoConfig::GetCellPMTFaceTheta(
		WCSimGeometryEnums::DetectorRegion_t region, int zone,
		unsigned int pmt) const
{
	double theta = -999.9;
	std::pair<WCSimGeometryEnums::DetectorRegion_t, int> myPair = std::make_pair(region, zone);
	std::map<std::pair<WCSimGeometryEnums::DetectorRegion_t, int>, WCSimDetectorZone>::const_iterator zoneItr = fZoneMap.find(myPair);
	if(zoneItr != fZoneMap.end())
	{
		theta = (*zoneItr).second.GetCellPMTFaceTheta(pmt);
	}
	else
	{
		std::cerr << "Error: Could not find PMT number " << pmt << " in region " << region.AsString() << ", zone " << zone << std::endl;
		assert(0);
	}
	return theta;
}

std::vector<double> WCSimGeoConfig::GetCellPMTFaceTheta(
		WCSimGeometryEnums::DetectorRegion_t region, int zone) const
{
	std::vector<double> pmtTheta;
	std::pair<WCSimGeometryEnums::DetectorRegion_t, int> myPair = std::make_pair(region, zone);
	std::map<std::pair<WCSimGeometryEnums::DetectorRegion_t, int>, WCSimDetectorZone>::const_iterator zoneItr = fZoneMap.find(myPair);
	if(zoneItr != fZoneMap.end())
	{
		WCSimDetectorZone zone = (*zoneItr).second;
		pmtTheta = zone.GetCellPMTFaceTheta();
	}
	else
	{
		std::cerr << "Error: Could not find region " << region.AsString() << ", zone " << zone << std::endl;
		assert(0);
	}
	return pmtTheta;
}

void WCSimGeoConfig::AddCellPMTFacePhi(
		WCSimGeometryEnums::DetectorRegion_t region, int zone, double phi)
{
	CreateMissingZone(region, zone);
	fZoneMap[std::make_pair(region, zone)].AddCellPMTFacePhi(phi);
}

double WCSimGeoConfig::GetCellPMTFacePhi(
		WCSimGeometryEnums::DetectorRegion_t region, int zone,
		unsigned int pmt) const
{
	double phi = -999.9;
	std::pair<WCSimGeometryEnums::DetectorRegion_t, int> myPair = std::make_pair(region, zone);
	std::map<std::pair<WCSimGeometryEnums::DetectorRegion_t, int>, WCSimDetectorZone>::const_iterator zoneItr = fZoneMap.find(myPair);
	if(zoneItr != fZoneMap.end())
	{
		return (*zoneItr).second.GetCellPMTFacePhi(pmt);
	}
	else
	{
		std::cerr << "Error: Could not find PMT number " << pmt << " in region " << region.AsString() << ", zone " << zone << std::endl;
		assert(0);
	}
	return phi;
}

std::vector<double> WCSimGeoConfig::GetCellPMTPhi(
		WCSimGeometryEnums::DetectorRegion_t region, int zone) const
{
	std::vector<double> pmtPhi;
	std::pair<WCSimGeometryEnums::DetectorRegion_t, int> myPair = std::make_pair(region, zone);
	std::map<std::pair<WCSimGeometryEnums::DetectorRegion_t, int>, WCSimDetectorZone>::const_iterator zoneItr = fZoneMap.find(myPair);
	if(zoneItr != fZoneMap.end())
	{
		pmtPhi = (*zoneItr).second.GetCellPMTPhi();
	}
	else
	{
		std::cerr << "Error: Could not find region " << region.AsString() << ", zone " << zone << std::endl;
		assert(0);
	}
	return pmtPhi;
}

bool WCSimGeoConfig::CanBuildWithoutAngles(std::string faceName)
{
	WCSimGeometryEnums::PMTDirection_t type = WCSimGeometryEnums::PMTDirection_t::FromString(faceName);
	return type.CanBuildWithoutAngles();
}

void WCSimGeoConfig::CreateMissingZone(
		WCSimGeometryEnums::DetectorRegion_t region, int zone)
{
	std::pair<WCSimGeometryEnums::DetectorRegion_t, int> myPair = std::make_pair(region, zone);
	if(fZoneMap.find(myPair) != fZoneMap.end())
	{
		WCSimDetectorZone zone;
		fZoneMap.insert(std::make_pair(myPair, zone));
	}
	return;
}

void WCSimGeoConfig::AddCellPMTFaceType(std::string typeName)
{
	WCSimGeometryEnums::PMTDirection_t faceType = WCSimGeometryEnums::PMTDirection_t::FromString(typeName);
	AddCellPMTFaceType(faceType);
	return;
}
