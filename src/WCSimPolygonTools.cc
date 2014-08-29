/*
 * WCSimPolygonTools.cc
 *
 *  Created on: 27 Aug 2014
 *      Author: andy
 */

#include "WCSimPolygonTools.hh"
#include "G4TwoVector.hh"
#include <math.h>
#include <cassert>

namespace WCSimPolygonTools {

bool PolygonContains(unsigned int nSides, double radius, G4TwoVector point) {
	assert( nSides > 2 && std::cerr << "Polygon must have more than two sides!");
	assert( radius > 0 && std::cerr << "Radius must be > 0!");

	// Construct our n-gon:
	std::vector<double> vertX, vertY;
	for( unsigned int iVert = 0; iVert < nSides; ++iVert ){
		vertX.push_back(radius * cos(M_PI / nSides));
		vertY.push_back(radius * sin(M_PI / nSides));
	}

	bool contained = false;  // The polygon contains the point if you have to cross
							 // an odd number of walls to reach it.  So far we've crossed
							 // no walls, so initialise to false

	// Trace a ray from (-infinity, 0) to the desired point and count how many sides it crosses
	for( unsigned int vert = 0; vert < nSides; ++vert ){
		double x1 = vertX.at(vert);
		double y1 = vertY.at(vert);
		double x2 = vertX.at((vert+1) % nSides);
		double y2 = vertY.at((vert+1) % nSides);

		if(    ( point.x() > x1 || point.x() > x2 ) 			// Point lies to the right of the top or bottom of the side
			&& ( (y1 - point.y()) * (y2 - point.y()) < 0.0 ) ){ // && point height is between top and bottom of side
				contained = !contained; // Have to cross a side to reach the point -> flip the contained flag
		}
	}
	return contained;
}

double GetSideFromRadius(unsigned int nSides, double radius) {
	assert( nSides > 2 && std::cerr << "Polygon must have more than two sides!");
	assert( radius > 0 && std::cerr << "Radius must be > 0!");

	return 2.0 * radius * sin(M_PI/nSides);
}

double GetRadiusFromSide(unsigned int nSides, double side) {
	assert( nSides > 2 && std::cerr << "Polygon must have more than two sides!");
	assert( side > 0 && std::cerr << "Side length must be > 0!");

	return 0.5 * side / sin(M_PI/nSides);
}

double GetAreaFromRadius(unsigned int nSides, double radius) {
	assert( nSides > 2 && std::cerr << "Polygon must have more than two sides!");
	assert( radius > 0 && std::cerr << "Radius must be > 0!");

	return nSides * radius * radius * tan(M_PI/nSides);
}

double GetAreaFromSide(unsigned int nSides, double side) {
	assert( nSides > 2 && std::cerr << "Polygon must have more than two sides!");
	assert( side > 0 && std::cerr << "Side length must be > 0!");

	return 0.25 * nSides * side * side / tan( M_PI / nSides );
}

double ShiftVector( G4TwoVector vec, double x, double y){
	return G4TwoVector(vec.x() + x, vec.y() + y);
}
/* namespace WCSimPolygonTools */
