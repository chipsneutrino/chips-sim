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
  fPMTLimit = WCSimGeometryEnums::PhotodetectorLimit_t::kUnknown;
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

bool WCSimGeoConfig::IsGood() const{
    bool isGood = true;

    std::map<std::pair<WCSimGeometryEnums::DetectorRegion_t, int>, WCSimDetectorZone>::iterator zoneItr;
    std::vector<WCSimGeometryEnums::DetectorRegion_t> regions = WCSimGeometryEnums::DetectorRegion_t::GetAllTypes();
    for( std::vector<WCSimGeometryEnums::DetectorRegion_t>::const_iterator regItr = regions.begin(); regItr != regions.end(); ++regItr)
    {
			for(unsigned int i = 0; i < GetNumZones(*regItr); ++i)
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

    assert(GetNumZones(WCSimGeometryEnums::DetectorRegion_t::kWall) == fNSides);


    if( !isGood ) { std::cerr << "Check the geometry xml file!" << std::endl; }
    return isGood;
}

bool WCSimGeoConfig::IsGoodZone(std::pair<WCSimGeometryEnums::DetectorRegion_t, int> zone) const
{
	bool isGoodZone = false;
  std::map<std::pair<WCSimGeometryEnums::DetectorRegion_t, int>, WCSimDetectorZone>::const_iterator zoneItr = fZoneMap.find(zone);
	if( zoneItr != fZoneMap.end())
	{
		isGoodZone = (*zoneItr).second.IsGood((zoneItr->first).first);
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
	std::cout << "\tCoverage = " << this->GetCoverage() << std::endl << std::endl;
	std::cout << "\t Limits on PMT numbers:" << std::endl;

	for( std::map<std::string, int>::const_iterator limItr = fPMTLimitMap.begin(); limItr != fPMTLimitMap.end(); ++limItr)
	{
		std::cout << "\t\t" << (*limItr).first << " limited to " << (*limItr).second << " PMTs" << std::endl;
	}

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

void WCSimGeoConfig::SetZonalCoverage(
		WCSimGeometryEnums::DetectorRegion_t region, int zone,
		double coverage)
{
	CreateMissingZone(region, zone);
	fZoneMap[std::make_pair(region, zone)].SetCoverage(coverage);
}

void WCSimGeoConfig::SetZonalCoverage(double coverage)
{
	std::vector<WCSimGeometryEnums::DetectorRegion_t>::iterator regionItr = fCurrentRegions.begin();
	std::vector<int>::iterator zoneItr = fCurrentZones.begin();

	for( ; regionItr != fCurrentRegions.end(); ++regionItr)
	{
		for( zoneItr = fCurrentZones.begin(); zoneItr != fCurrentZones.end(); ++zoneItr )
		{
			SetZonalCoverage(*regionItr, *zoneItr, coverage);
		}
	}
}

void WCSimGeoConfig::SetPMTLimit(WCSimGeometryEnums::DetectorRegion_t region,
		int zone, std::string name, int limit)
{
	CreateMissingZone(region, zone);
	fZoneMap[std::make_pair(region, zone)].SetPMTLimit(name, limit);
}
int WCSimGeoConfig::GetMaxZoneCells(WCSimGeometryEnums::DetectorRegion_t region,
		int zone) const
{
	int nCells = -1;
	std::pair<WCSimGeometryEnums::DetectorRegion_t, int> myPair = std::make_pair(region, zone);
	std::map<std::pair<WCSimGeometryEnums::DetectorRegion_t, int>, WCSimDetectorZone>::const_iterator zoneItr = fZoneMap.find(myPair);
	if(zoneItr != fZoneMap.end())
	{
		nCells = (*zoneItr).second.GetMaxNumCells();
	}
	else
	{
		std::cerr << "Error: Could not find region " << region.AsString() << ", zone " << zone << std::endl;
		assert(0);
	}
	return nCells;
}

void WCSimGeoConfig::SetZoneThetaStart(
		WCSimGeometryEnums::DetectorRegion_t region, int zoneNum, double theta)
{
	CreateMissingZone(region, zoneNum);
	if( region == WCSimGeometryEnums::DetectorRegion_t::kTop || region == WCSimGeometryEnums::DetectorRegion_t::kBottom)
	{
		// std::cout << "Setting zone map theta start = " << theta << std::endl;
		fZoneMap[std::make_pair(region, zoneNum)].SetThetaStart(theta);
	}
	else
	{
		std::cerr << "Warning: Trying to set end angle for a zone not on the end cap.  " << std::endl
							<< "         Will default to the detector wall for that side" << std::endl;
		double defaultTheta = GetNSides() / 2*M_PI * ((zoneNum+1) % GetNSides());
		fZoneMap[std::make_pair(region, zoneNum)].SetThetaEnd(defaultTheta);
	}
}

void WCSimGeoConfig::SetZoneThetaEnd(
		WCSimGeometryEnums::DetectorRegion_t region, int zoneNum, double theta)
{
	CreateMissingZone(region, zoneNum);
	if( region == WCSimGeometryEnums::DetectorRegion_t::kTop || region == WCSimGeometryEnums::DetectorRegion_t::kBottom)
	{
//		std::cout << "Setting zone map theta end = " << theta << std::endl;
		fZoneMap[std::make_pair(region, zoneNum)].SetThetaEnd(theta);
	}
	else
	{
		std::cerr << "Warning: Trying to set start angle for a zone not on the end cap.  " << std::endl
							<< "         Will default to the detector wall for that side" << std::endl;
		double defaultTheta = GetNSides() / 2*M_PI * (zoneNum % GetNSides());
		fZoneMap[std::make_pair(region, zoneNum)].SetThetaEnd(defaultTheta);
	}
}

double WCSimGeoConfig::GetZoneThetaStart(
		WCSimGeometryEnums::DetectorRegion_t region, int zoneNum)
{
	double thetaStart = 0.0;
	std::pair<WCSimGeometryEnums::DetectorRegion_t, int> myPair = std::make_pair(region, zoneNum);
	std::map<std::pair<WCSimGeometryEnums::DetectorRegion_t, int>, WCSimDetectorZone>::const_iterator zoneItr = fZoneMap.find(myPair);
	if(zoneItr != fZoneMap.end())
	{
		WCSimDetectorZone zone = (*zoneItr).second;
		thetaStart = zone.GetThetaStart();
	}
	else
	{
		std::cerr << "Error: Could not find region " << region.AsString() << ", zone " << zoneNum << std::endl;
		assert(0);
	}
	return thetaStart;
}

double WCSimGeoConfig::GetZoneThetaEnd(
		WCSimGeometryEnums::DetectorRegion_t region, int zoneNum)
{
	double thetaEnd = 0.0;
	std::pair<WCSimGeometryEnums::DetectorRegion_t, int> myPair = std::make_pair(region, zoneNum);
	std::map<std::pair<WCSimGeometryEnums::DetectorRegion_t, int>, WCSimDetectorZone>::const_iterator zoneItr = fZoneMap.find(myPair);
	if(zoneItr != fZoneMap.end())
	{
		WCSimDetectorZone zone = (*zoneItr).second;
		thetaEnd = zone.GetThetaEnd();
	}
	else
	{
		std::cerr << "Error: Could not find region " << region.AsString() << ", zone " << zoneNum << std::endl;
		assert(0);
	}
	return thetaEnd;

}

void WCSimGeoConfig::SetZoneThetaStart(double theta)
{
	std::vector<WCSimGeometryEnums::DetectorRegion_t>::iterator regionItr = fCurrentRegions.begin();
	std::vector<int>::iterator zoneItr = fCurrentZones.begin();

	for( ; regionItr != fCurrentRegions.end(); ++regionItr)
	{
		for( zoneItr = fCurrentZones.begin(); zoneItr != fCurrentZones.end(); ++zoneItr )
		{
//			std::cout << "Geoconfig setting start angle to " << theta << std::endl;
			SetZoneThetaStart(*regionItr, *zoneItr, theta);
		}
	}
}

void WCSimGeoConfig::SetZoneThetaEnd(double theta)
{
	std::vector<WCSimGeometryEnums::DetectorRegion_t>::iterator regionItr = fCurrentRegions.begin();
	std::vector<int>::iterator zoneItr = fCurrentZones.begin();

	for( ; regionItr != fCurrentRegions.end(); ++regionItr)
	{
		for( zoneItr = fCurrentZones.begin(); zoneItr != fCurrentZones.end(); ++zoneItr )
		{
//			std::cout << "Geoconfig setting end angle to " << theta << std::endl;
			SetZoneThetaEnd(*regionItr, *zoneItr, theta);
		}
	}
}

unsigned int WCSimGeoConfig::GetNumZones(
		WCSimGeometryEnums::DetectorRegion_t region) const
{
	unsigned int count = 0;
	std::map<std::pair<WCSimGeometryEnums::DetectorRegion_t, int>, WCSimDetectorZone>::const_iterator mapItr;
	for( mapItr = fZoneMap.begin(); mapItr != fZoneMap.end(); ++mapItr)
	{
		if( (mapItr->first).first == region ){ count++; }
	}
	return count;
}

void WCSimGeoConfig::SetCoverageType(std::string typeName)
{
	WCSimGeometryEnums::PhotodetectorLimit_t type = WCSimGeometryEnums::PhotodetectorLimit_t(typeName);
	SetCoverageType(type);

}

void WCSimGeoConfig::SetCoverageType(
		WCSimGeometryEnums::PhotodetectorLimit_t type)
{
  // std::cout << "Setting coverage type to " << type.AsString() << std::endl;
	if(type == WCSimGeometryEnums::PhotodetectorLimit_t::kPercentCoverage){ SetUseOverallCoverage(true); }
	else if(type == WCSimGeometryEnums::PhotodetectorLimit_t::kZonalCoverage){ SetUseZonalCoverage(true); }
	else if(type == WCSimGeometryEnums::PhotodetectorLimit_t::kTotalNumber){ SetLimitPMTNumbers(true); }
	else{
		std::cerr << "Error: unknown coverage type: " << type.AsString() << std::endl;
		assert(0);
	}
}

void WCSimGeoConfig::CreateMissingZone(
		WCSimGeometryEnums::DetectorRegion_t region, int zone)
{
	std::pair<WCSimGeometryEnums::DetectorRegion_t, int> myPair = std::make_pair(region, zone);
	if(fZoneMap.find(myPair) == fZoneMap.end())
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

void WCSimGeoConfig::SetLimitPMTNumbers(bool doIt)
{
	if(   fPMTLimit != WCSimGeometryEnums::PhotodetectorLimit_t::kUnknown
		 && fPMTLimit != WCSimGeometryEnums::PhotodetectorLimit_t::kTotalNumber
		 && doIt)
	{
		std::cerr << "Error: can't set photodetector limit to fixed PMT number - is already set to " << fPMTLimit.AsString();
	}
	if(doIt){ fPMTLimit = WCSimGeometryEnums::PhotodetectorLimit_t::kTotalNumber; }
}

bool WCSimGeoConfig::GetLimitPMTNumber() const
{
	return (fPMTLimit == WCSimGeometryEnums::PhotodetectorLimit_t::kTotalNumber);
}

void WCSimGeoConfig::SetPMTLimit(std::string name, int limit)
{
	if(fPMTLimit != WCSimGeometryEnums::PhotodetectorLimit_t::kTotalNumber){
		std::cerr << "Error: Cannot fix PMT numbers, because the limit type is currently " << fPMTLimit.AsString() << std::endl;
		std::cerr << "       Use limitPMTNumbers=\"yes\" as a geoDef attribute in the geometry file" << std::endl;
	}

	std::vector<WCSimGeometryEnums::DetectorRegion_t>::iterator regionItr = fCurrentRegions.begin();
	std::vector<int>::iterator zoneItr = fCurrentZones.begin();

	for( ; regionItr != fCurrentRegions.end(); ++regionItr)
	{
		for( zoneItr = fCurrentZones.begin(); zoneItr != fCurrentZones.end(); ++zoneItr )
		{
			SetPMTLimit(*regionItr, *zoneItr, name, limit);
		}
	}
}

int WCSimGeoConfig::GetPMTLimit(WCSimGeometryEnums::DetectorRegion_t region, int zone, std::string name)
{
	int pmtLimit;
	std::pair<WCSimGeometryEnums::DetectorRegion_t, int> myPair = std::make_pair(region, zone);
	std::map<std::pair<WCSimGeometryEnums::DetectorRegion_t, int>, WCSimDetectorZone>::const_iterator zoneItr = fZoneMap.find(myPair);
	if(zoneItr != fZoneMap.end())
	{
		pmtLimit = ((*zoneItr).second).GetPMTLimit(name);
	}
	else
	{
		std::cerr << "Error: Could not find region " << region.AsString() << ", zone " << zone << std::endl;
		assert(0);
	}
	return pmtLimit;
}

void WCSimGeoConfig::SetOverallCoverage(double coverage)
{
	fPMTLimit = WCSimGeometryEnums::PhotodetectorLimit_t::kPercentCoverage;
	fPercentCoverage = coverage;
}

void WCSimGeoConfig::SetUseOverallCoverage(bool doIt)
{
	if(   fPMTLimit != WCSimGeometryEnums::PhotodetectorLimit_t::kUnknown
		 && fPMTLimit != WCSimGeometryEnums::PhotodetectorLimit_t::kPercentCoverage
		 && doIt)
	{
		std::cerr << "Error: can't set photodetector limit to a fixed global coverage - is already set to " << fPMTLimit.AsString();
	}
	if(doIt){ fPMTLimit = WCSimGeometryEnums::PhotodetectorLimit_t::kPercentCoverage; }
}

bool WCSimGeoConfig::GetUseOverallCoverage()
{
	return (fPMTLimit == WCSimGeometryEnums::PhotodetectorLimit_t::kPercentCoverage);
}

double WCSimGeoConfig::GetOverallCoverageFraction() const
{
	return fPercentCoverage;
}

void WCSimGeoConfig::SetUseZonalCoverage(bool doIt)
{
	if(   fPMTLimit != WCSimGeometryEnums::PhotodetectorLimit_t::kUnknown
		 && fPMTLimit != WCSimGeometryEnums::PhotodetectorLimit_t::kZonalCoverage
		 && doIt)
	{
		std::cerr << "Error: can't set photodetector limit to a fixed global coverage - is already set to " << fPMTLimit.AsString();
	}
	if(doIt){ fPMTLimit = WCSimGeometryEnums::PhotodetectorLimit_t::kZonalCoverage; }
}

bool WCSimGeoConfig::GetUseZonalCoverage()
{
	return (fPMTLimit == WCSimGeometryEnums::PhotodetectorLimit_t::kZonalCoverage);
}

double WCSimGeoConfig::GetZonalCoverageFraction(
		WCSimGeometryEnums::DetectorRegion_t region, int zone) const
{
	double fraction = 0.0;
	std::pair<WCSimGeometryEnums::DetectorRegion_t, int> myPair = std::make_pair(region, zone);
	std::map<std::pair<WCSimGeometryEnums::DetectorRegion_t, int>, WCSimDetectorZone>::const_iterator zoneItr = fZoneMap.find(myPair);
	if(zoneItr != fZoneMap.end())
	{
		fraction = (*zoneItr).second.GetCoverage();
	}
	else
	{
		std::cerr << "Error: Could not find region " << region.AsString() << ", zone " << zone << std::endl;
		assert(0);
	}
	return fraction;
}
