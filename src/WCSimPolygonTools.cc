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
#include "TMath.h"

namespace WCSimPolygonTools {
  
  bool PolygonContains(unsigned int nSides, double outerRadius, G4TwoVector point) {
    // We have a regular polygon which means we don't need to do raytracing or winding numbers
    assert(CheckPolygon( nSides, outerRadius )); 
    
    double innerRadius = outerRadius * cos(M_PI/nSides);
//     if( point.x() * point.x() + point.y() * point.y() <= innerRadius * innerRadius ) { return true; } // Point is inside the circle described by middle of the sides
//     
//     if( point.x() * point.x() + point.y() * point.y() >= outerRadius * outerRadius ) { return false; } // Point is outside the circle described by the corners
//   	
    // It's in the overlap between these two circles; let's be more careful
    
    // Construct our n-gon:
  	std::vector<double> vertX, vertY;
  	for( unsigned int iVert = 0; iVert < nSides; ++iVert ){
  		double theta = (2. * iVert) * M_PI / nSides;
  		vertX.push_back(outerRadius * cos(theta));
  		vertY.push_back(outerRadius * sin(theta));
  	}

    // std::cout << "Point = (" << point.x() << "," << point.y() << std::endl;
    double angleToPoint = point.phi();
    angleToPoint = (angleToPoint > 0.0) ? angleToPoint : angleToPoint + 2 * M_PI;
    // std::cout << std::endl << "angleToPoint = " << angleToPoint << std::endl;
    int whichSide = (int)floor(angleToPoint * nSides / (2 * M_PI)) % nSides; // The appropriate side joins whichSide and whichSide+1
    // std::cout << "whichSide = " << whichSide << std::endl;

    // Line containing our point and the origin: y = mPoint * x + cPoint, cPoint = 0
    double mPoint = point.y() / point.x();

    // Line containing the required polygon side: y = mSide * x + cSide
    double mSide = ( vertY.at((whichSide+1) % nSides) - vertY.at(whichSide) ) / ( vertX.at((whichSide+1) % nSides) - vertX.at(whichSide) );
    double cSide = vertY.at(whichSide) - mSide * vertX.at(whichSide);

    // Intersect where these two functions for y are the same:
    double intersectX = cSide / (mPoint - mSide);
    double intersectY = mPoint * intersectX;

    return ((intersectX * intersectX + intersectY * intersectY) >= (point.x() * point.x() + point.y() * point.y())); // Is the point before the intersection?
  }
  

  bool PolygonContainsSquare(unsigned int nSides, double outerRadius, G4TwoVector squareCorner, double squareSide){
    assert(CheckPolygon( nSides, outerRadius));
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
      contained = ( contained && PolygonContains(nSides, outerRadius, (*itr)) );
      if( !contained ){ break; }
    }
    
    //for( std::vector<G4TwoVector>::const_iterator itr = squareCorners.begin() ;
    //     itr != squareCorners.end(); ++itr ){
    //std::cout << (*itr).x() << " " << (*itr).y() << " " << contained << std::endl;
    //}
    return contained;
  }
  
  double GetSideFromRadius(unsigned int nSides, double outerRadius) {
    assert(CheckPolygon(nSides, outerRadius));
  	return 2.0 * outerRadius * sin(M_PI/nSides);
  }
  
  double GetRadiusFromSide(unsigned int nSides, double side) {
    assert(CheckPolygonSide(nSides, side)); 
  	return 0.5 * side / sin(M_PI/nSides);
  }
  
  double GetAreaFromRadius(unsigned int nSides, double outerRadius) {
    assert(CheckPolygon(nSides, outerRadius)); 
  	return nSides * outerRadius * outerRadius * tan(M_PI/nSides);
  }
  
  double GetAreaFromSide(unsigned int nSides, double side) {
  	assert(CheckPolygonSide( nSides, side ));
    return 0.25 * nSides * side * side / tan( M_PI / nSides );
  }
  
  G4TwoVector ShiftVector( G4TwoVector vec, double x, double y){
  	return G4TwoVector(vec.x() + x, vec.y() + y);
  }

  bool CheckPolygon( unsigned int nSides, double outerRadius ){
    if( !(nSides > 2)){ 
      std::cerr << "Polygon must have more than 2 sides!" << std::endl;
    }
    if( !(outerRadius > 0) ){
      std::cerr << "Radius must be > 0!" << std::endl;
    }
    return (nSides > 2 && outerRadius > 0);
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

  double GetInnerRadiusFromOuter( unsigned int nSides, double outerRadius )
  {
    return outerRadius * cos(M_PI/nSides);
  }

  double GetOuterRadiusFromInner( unsigned int nSides, double innerRadius )
  {
    return innerRadius / (cos(M_PI/nSides)) ; 
  }

  bool PolygonSliceContainsSquare(unsigned int nSides, double thetaStart, double thetaEnd,
		double outerRadius, G4TwoVector squareCorner, double squareSide)
  {
     assert(CheckPolygon( nSides, outerRadius));
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
       contained = ( contained && (PolygonSliceContains(nSides, thetaStart, thetaEnd, outerRadius, (*itr))));

       if( !contained ){ break; }
     }

     //for( std::vector<G4TwoVector>::const_iterator itr = squareCorners.begin() ;
     //     itr != squareCorners.end(); ++itr ){
     //std::cout << (*itr).x() << " " << (*itr).y() << " " << contained << std::endl;
     //}
     return contained;
  }

  bool PolygonSliceContains(unsigned int nSides, double thetaStart, double thetaEnd,
		double outerRadius, G4TwoVector point)
  {

  	// We have a regular polygon which means we don't need to do raytracing or winding numbers
  	assert(CheckPolygon( nSides, outerRadius ));
  	double phi = point.phi();
  	if(phi < 0) { phi = phi + 2*M_PI; }
  	bool contained =    (PolygonContains(nSides, outerRadius, point) && IsAngleBetween(phi, thetaStart, thetaEnd));
//  	std::cout << "Point is... " << point.x() << " " << point.y() << " " << contained << " " << (PolygonContains(nSides, outerRadius, point)) << " " << IsAngleBetween(phi, thetaStart, thetaEnd) << std::endl;

  	return contained;
  }

  int IsInSliceNumber(unsigned int nSides, G4TwoVector point)
  {
  	double phi = point.phi();
  	if( phi < 0 ) { phi += 2*M_PI; }
  	return TMath::FloorNint(fmod(phi, (2. * M_PI)/nSides));
  }

  double GetSliceAreaFromAngles(unsigned int nSides, double outerRadius, double startAngle, double endAngle)
  {
     return GetAreaFromRadius(nSides, outerRadius) * (fmod((endAngle-startAngle) / 2*TMath::Pi(), 1.0));

     double area = 0.0;
     
     double startNorm = NormaliseAngle(startAngle);
     double endNorm = NormaliseAngle(endAngle);
     double delta = endNorm - startNorm;
    
     int startSide = IsInSliceNumber(nSides, G4TwoVector(TMath::Sin(startAngle), TMath::Cos(startAngle)));
     int endSide = IsInSliceNumber(nSides, G4TwoVector(TMath::Sin(endAngle), TMath::Cos(endAngle)));

     if( startSide == endSide )
     {
      // Calculate area of the triangle
     }

     else
     {
       // Calculate two triangle areas
     }


  }

	double GetSliceAreaFromRadius(unsigned int nSides, double outerRadius)
	{
		return GetAreaFromRadius(nSides, outerRadius) / nSides;
	}

	double GetSliceAreaFromSide(unsigned int nSides, double side)
	{
		return GetAreaFromSide(nSides, side) / nSides;
	}

	double NormaliseAngle(double angle)
	{
		// Puts angle into the range [0, 2pi]
		float numCircles = (angle / (2*M_PI));
		angle = angle - 2*M_PI*(int)numCircles;
		if( 0.0 > angle )
		{
			angle += 2*M_PI;
		}
		return angle;
	}

	bool IsAngleBetween(double testAngle, double startAngle, double endAngle)
	{
		double twoPi = 2 * M_PI;

		// Is allowed range >= a full circle?
		if( fabs( endAngle - startAngle ) >= twoPi )
		{
//			std::cout << "Difference > 2pi for " << startAngle << " and " << endAngle << std::endl;
			return true;
		}

		double normTest = NormaliseAngle(testAngle);

		// Make sure the start angle is < test
		double normStart = NormaliseAngle( startAngle );
		if( normStart > normTest )
		{
			normStart -= twoPi;
		}

		// Do the same with the end angle, to make sure it comes after the start one
		double normEnd = NormaliseAngle(endAngle);
		if( normEnd < normStart ){ normEnd += twoPi; }
		else if( (normEnd - normStart) >= twoPi ) { normEnd -= twoPi; }

		// Now is our angle between these two?
		if( (normTest < normStart) || (normTest > normEnd) )
		{
			return false;
		}

//		std::cout << "I think this is inside... " << std::endl
//				 << testAngle << " between " << startAngle << " and " << endAngle << std::endl
//				 << "Becomes" << std::endl
//				 << normTest << " between " << normStart << " and " << normEnd << std::endl;

		return true;
	}

}
/* namespace WCSimPolygonTools */
