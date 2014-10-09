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
    // We have a regular polygon which means we don't need to do raytracing or winding numbers
    assert(CheckPolygon( nSides, radius )); 
    
    double innerRadius = radius * cos(M_PI/nSides);
    if( point.x() * point.x() + point.y() * point.y() <= innerRadius * innerRadius ) { return true; } // Point is inside the circle described by middle of the sides
    
    if( point.x() * point.x() + point.y() * point.y() >= radius * radius ) { return false; } // Point is outside the circle described by the corners
  	
    // It's in the overlap between these two circles; let's be more careful
    
    // Construct our n-gon:
  	std::vector<double> vertX, vertY;
  	for( unsigned int iVert = 0; iVert < nSides; ++iVert ){
  		double theta = (2. * iVert) * M_PI / nSides;
  		vertX.push_back(radius * cos(theta));
  		vertY.push_back(radius * sin(theta));
  	}

    // std::cout << "Point = (" << point.x() << "," << point.y() << std::endl;
    double angleToPoint = point.phi();
    angleToPoint = (angleToPoint > 0.0) ? angleToPoint : angleToPoint + 2 * M_PI;
    std::cout << std::endl << "angleToPoint = " << angleToPoint << std::endl;
    int whichSide = (int)floor(angleToPoint * nSides / (2 * M_PI)) % nSides; // The appropriate side joins whichSide and whichSide+1
    std::cout << "whichSide = " << whichSide << std::endl;

    // Line containing our point and the origin: y = mPoint * x + cPoint, cPoint = 0
    double mPoint = point.y() / point.x();

    // Line containing the required polygon side: y = mSide * x + cSide
    double mSide = ( vertY.at((whichSide+1) % nSides) - vertY.at(whichSide) ) / ( vertX.at((whichSide+1) % nSides) - vertX.at(whichSide) );
    double cSide = vertY.at(whichSide) - mSide * vertX.at(whichSide);

    // Intersect where these two functions for y are the same:
    double intersectX = cSide / (mPoint - mSide);
    double intersectY = mPoint * intersectX;

    return ((intersectX * intersectX + intersectY * intersectY) <= (point.x() * point.x() + point.y() * point.y())); // Is the point before the intersection?
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
      // std::cout << "itr = ( " << (*itr).x() << ", " << (*itr).y() << std::endl;
      contained = ( contained && PolygonContains(nSides, radius, (*itr)) );
      if( !contained ){ break; }
    }
    
    for( std::vector<G4TwoVector>::const_iterator itr = squareCorners.begin() ;
         itr != squareCorners.end(); ++itr ){
      std::cout << (*itr).x() << " " << (*itr).y() << " " << contained << std::endl;
    }
//     if( squareCorner.x() < -19000 )
//     { 
//       std::cout << squareCorners.at(0) << ", " << squareCorners.at(1) << ", " << squareCorners.at(2) << ", " << squareCorners.at(3) << "   Contained? " << contained << std::endl;
//     }
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
