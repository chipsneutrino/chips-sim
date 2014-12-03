/*
 * WCSimUnitCell.cc
 *
 *  Created on: 11 Aug 2014
 *      Author: andy
 */

#include "WCSimUnitCell.hh"
#include <cassert>
#include <math.h>
#include <vector>

// PMT placement objects to be held by the unit cell
// Basically just a WCSimPMTConfig and its 2D position

WCSimPMTPlacement::WCSimPMTPlacement(WCSimPMTConfig pmt, double x, double y) :
		fPMTConfig(pmt), fX(x), fY(y) {
}

WCSimPMTPlacement::~WCSimPMTPlacement() {
}

void WCSimPMTPlacement::Print() const{
  std::cout << "-------------------------" << std::endl 
            << "PMT of type " << fPMTConfig.GetPMTName() 
            << " at (" << fX / m << "," << fY / m << "):" << std::endl;
  fPMTConfig.Print();
  std::cout << "-------------------------" << std::endl << std::endl;
}

double WCSimPMTPlacement::GetDistanceTo(WCSimPMTPlacement * pmt) const{
	double xdiff = (this->GetX() - pmt->GetX());
	double ydiff = (this->GetY() - pmt->GetY());
	return sqrt( xdiff * xdiff + ydiff * ydiff );
}

bool WCSimPMTPlacement::OverlapsWith(WCSimPMTPlacement * pmt, double side) const {
  double separation = this->GetDistanceTo(pmt) * side;
	return ( (this->GetPMTRadius() + pmt->GetPMTRadius()) > separation );
}

double WCSimPMTPlacement::GetX() const {
	return fX;
}

double WCSimPMTPlacement::GetY() const {
	return fY;
}

double WCSimPMTPlacement::GetPMTRadius() const {
	return fPMTConfig.GetRadius();
}

WCSimPMTConfig WCSimPMTPlacement::GetPMTConfig() const {
	return fPMTConfig;
}

//////////////////////////////////////////////////////////////////
// Unit cell to be tiled around the flat surfaces of the detector
//////////////////////////////////////////////////////////////////
WCSimUnitCell::WCSimUnitCell() {
	// Move along, nothing to see here
}

WCSimUnitCell::WCSimUnitCell(const WCSimPMTConfig &pmt, double x, double y) {
	AddPMT(pmt, x, y);
	return;
}

WCSimUnitCell::~WCSimUnitCell() {
	// TODO Auto-generated destructor stub
}


unsigned int WCSimUnitCell::GetNumPMTs() const {
  return fPMTs.size();
}

G4TwoVector WCSimUnitCell::GetPMTPos(unsigned int pmt, double side) const {
  assert(pmt < fPMTs.size());
  return G4TwoVector(side / m * fPMTs.at(pmt).GetX(), side / m *fPMTs.at(pmt).GetY());

}

std::vector<WCSimPMTPlacement> WCSimUnitCell::GetPMTPlacements() const {
  return fPMTs;
}

WCSimPMTPlacement WCSimUnitCell::GetPMTPlacement(unsigned int pmt) const {
  assert(pmt < fPMTs.size());
  return fPMTs.at(pmt);
}

void WCSimUnitCell::Print() const{
  std::cout << "-------------------" << std::endl;
  std::cout << "Unit cell contains " << fPMTs.size() << " PMTs" << std::endl;
  for( std::vector<WCSimPMTPlacement>::const_iterator itr = fPMTs.begin() ; 
       itr != fPMTs.end(); ++itr ){
    std::cout << (*itr).GetPMTConfig().GetPMTName() << " at (" << (*itr).GetX() / m 
              << "," << (*itr).GetY() / m << ")" << std::endl;
  }
  std::cout << "-----------------" << std::endl << std::endl;
}

void WCSimUnitCell::AddPMT(const WCSimPMTConfig &pmt, double x, double y) {
//  std::cout << "x = " << x << " y = " << y << " 1m = " << 1*m  << std::endl;
	assert(0.0 * m < x && "x position must be greater than 0 (and < 1)");
	assert(1.0 * m > x && "x position must be less than 1 (and > 0)");
	assert(0.0 * m < y && "y position must be greater than 0 (and < 1)");
	assert(1.0 * m > y && "y position must be less than 1 (and > 0)");
	fPMTs.push_back(WCSimPMTPlacement(pmt, x, y));
}

double WCSimUnitCell::GetPhotocathodeCoverage(double side) const {
	double cellarea = side * side;
	double cathodearea = this->GetPhotocathodeArea();
	return cathodearea / cellarea;
}

double WCSimUnitCell::GetMinimumCellSize() const {
	// Need the closest two PMTs to be separated by exactly the sum of their radii
//  std::cout << "fPMTs.size() = " << fPMTs.size() << std::endl;
	uint toCompare = fPMTs.size() / 2 + fPMTs.size() % 2;
	// Need to compare each pair of PMTs so n/2 for even size, n/2 + 1 for odd

	// Measures the separation of PMTs in units of the sum of their radii
	// divided by their separation.  
  // Is the size of an a x a box required so that PMTs don't overlap one another
	double closenessScale = 0.0;


	// Loop over each pair
	for (uint i = 0; i < toCompare; ++i) {
		WCSimPMTPlacement first = fPMTs.at(i);
		for (uint j = i + 1; (j < fPMTs.size() && j != i); ++j) {
			WCSimPMTPlacement second = fPMTs.at(j);
//      std::cout << "First radius = " << first.GetPMTRadius() << "  Second = " << second.GetPMTRadius() << std::endl;
			// Is the distance between the PMTs less than the sum of their radii?
			double distance = first.GetDistanceTo(&second);
			double sumRadii = first.GetPMTRadius() + second.GetPMTRadius();
			assert( sumRadii > 0 && "PMT radii are not larger than 0");
			assert( distance > 0 && "Distance between PMTs must be > 0 in the unit square");
			if( (sumRadii / distance) > closenessScale ){
				closenessScale = sumRadii/distance;
			}
		}
	}

  // Size of a x a box required so that all PMTs fit inside it
  double scale = 0;
  for( std::vector<WCSimPMTPlacement>::const_iterator itr = fPMTs.begin() ; 
       itr != fPMTs.end(); ++itr )
  {
    double radius = (*itr).GetPMTRadius();
    double x = (*itr).GetX();
    double y = (*itr).GetY();

    double scaleFacs[4] = { radius/x, radius/(1*m-x), radius/y, radius/(1*m-y) };
    double scaleBy = *(std::max_element(scaleFacs, scaleFacs + 4));
    if( scaleBy > scale ){ scale = scaleBy; }
  }

  // Add a millimetre to ensure some separation 
  if( closenessScale > scale ) { 
    return closenessScale*m + 1 * mm; 
  }
  return scale*m + 1 * mm;
}


double WCSimUnitCell::GetCellSizeForCoverage(double coverage) const {
	double minSize     = GetMinimumCellSize();
	double maxCoverage = GetPhotocathodeCoverage(minSize);
	if( maxCoverage < coverage)
	{
		std::cerr << "Error: desired coverage is too large for unit cell - PMTs would overlap" << std::endl;
		std::cerr << "\tDesired = " << coverage << "  Max = " << maxCoverage << std::endl;
		assert( maxCoverage >= coverage);
	}
//	std::cout << "MinSize = " << minSize << std::endl;
//	std::cout << "maxCoverage = " << maxCoverage << std::endl;
//	std::cout << "desired coverage = " << coverage << std::endl;
//	std::cout << "returning " << sqrt(maxCoverage / coverage) * minSize << std::endl;
	return sqrt(maxCoverage / coverage) * minSize;
}

bool WCSimUnitCell::ContainsOverlaps(double side) const {
	// Calculate the distance between the centres
	// and compare it to the sum of the two PMT radii
	// for each pair of DOMs

	uint toCompare = fPMTs.size() / 2 + fPMTs.size() % 2;
	// Need to compare each pair of PMTs
	// So n/2 for even size, n/2 + 1 for odd

	for (uint i = 0; i < toCompare; ++i) {
		WCSimPMTPlacement first = fPMTs.at(i);
		for (uint j = i + 1; (j < fPMTs.size() && j != i); ++j) {
			WCSimPMTPlacement second = fPMTs.at(j);

			// Is the distance between the PMTs less than the sum of their radii?
			double distance = first.GetDistanceTo(&second);
			if(distance * (side) < (first.GetPMTRadius() + second.GetPMTRadius()) ){
				return true;
			}
		}
	}

	return false;
}

double WCSimUnitCell::GetCellExposeHeight() const {
	double maxExpose = 0.0;
	std::vector<WCSimPMTPlacement>::const_iterator pmtItr = fPMTs.begin();
	for( ; pmtItr != fPMTs.end(); ++pmtItr ){
		double expose = (*pmtItr).GetPMTConfig().GetExposeHeight();
		if( expose > maxExpose) { maxExpose = expose; }
	}
	return maxExpose;
}

double WCSimUnitCell::GetPhotocathodeArea() const
{
	double area = 0.0;
	for(std::vector<WCSimPMTPlacement>::const_iterator pmtItr = fPMTs.begin(); pmtItr != fPMTs.end(); ++pmtItr){
		area += pmtItr->GetPMTConfig().GetArea();
	}
	return area;
}
