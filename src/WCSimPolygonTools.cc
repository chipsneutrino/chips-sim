/*
 * WCSimPolygonTools.cc
 *
 *  Created on: 27 Aug 2014
 *      Author: andy
 */

#include "WCSimPolygonTools.hh"
#include "G4TwoVector.hh"
#include <cassert>
#include <math.h>
#include <vector>
#include <iostream>

namespace WCSimPolygonTools {
  
  bool PolygonContains(unsigned int nSides, double radius, G4TwoVector point) {
  	
    assert(CheckPolygon( nSides, radius )); 
  	// Construct our n-gon:
  	std::vector<double> vertX, vertY;
  	for( unsigned int iVert = 0; iVert < nSides; ++iVert ){
  		double theta = (2. * iVert + 1.) * M_PI / nSides;
  		vertX.push_back(radius * cos(theta));
  		vertY.push_back(radius * sin(theta));
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
  

  bool PolygonContainsSquare(unsigned int nSides, double radius, G4TwoVector squareCorner, double squareSide){
    assert(CheckPolygon( nSides, radius));
    // std::cout << "Checking square" << std::endl;
    std::vector<G4TwoVector> squareCorners;
    squareCorners.push_back( squareCorner );
    squareCorners.push_back( squareCorner + G4TwoVector( 0.0, squareSide ) );
    squareCorners.push_back( squareCorner + G4TwoVector( squareSide, squareSide ) );
    squareCorners.push_back( squareCorner + G4TwoVector( squareSide, 0.0 ) );
    bool contained = true;

    for( std::vector<G4TwoVector>::const_iterator itr = squareCorners.begin() ;
         itr != squareCorners.end(); ++itr ){
      contained = ( contained && PolygonContains(nSides, radius, (*itr)) );
      if( !contained ){ break; }
    }
    return contained;
  }
  
  double GetSideFromRadius(unsigned int nSides, double radius) {
    assert(CheckPolygon(nSides, radius));
  	return 2.0 * radius * sin(M_PI/nSides);
  }
  
  double GetRadiusFromSide(unsigned int nSides, double side) {
    assert(CheckPolygonSide(nSides, side)); 
  	return 0.5 * side / sin(M_PI/nSides);
  }
  
  double GetAreaFromRadius(unsigned int nSides, double radius) {
    assert(CheckPolygon(nSides, radius)); 
  	return nSides * radius * radius * tan(M_PI/nSides);
  }
  
  double GetAreaFromSide(unsigned int nSides, double side) {
  	assert(CheckPolygonSide( nSides, side ));
    return 0.25 * nSides * side * side / tan( M_PI / nSides );
  }
  
  G4TwoVector ShiftVector( G4TwoVector vec, double x, double y){
  	return G4TwoVector(vec.x() + x, vec.y() + y);
  }

  bool CheckPolygon( unsigned int nSides, double radius ){
    if( !(nSides > 2)){ 
      std::cerr << "Polygon must have more than 2 sides!" << std::endl;
    }
    if( !(radius > 0) ){
      std::cerr << "Radius must be > 0!" << std::endl;
    }
    return (nSides > 2 && radius > 0);
  }

  bool CheckPolygonSide( unsigned int nSides, double side ){
    if( !(nSides > 2)){ 
      std::cerr << "Polygon must have more than 2 sides!" << std::endl;
    }
    if( !(side > 0) ){
      std::cerr << "Side length must be > 0!" << std::endl;
    }

    return (nSides > 2 && side > 0);
  }
}
/* namespace WCSimPolygonTools */
