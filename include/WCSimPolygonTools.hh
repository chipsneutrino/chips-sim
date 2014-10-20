/*
 * WCSimPolygonTools.hh
 *
 *  Created on: 27 Aug 2014
 *      Author: andy
 */

#ifndef WCSIMPOLYGONTOOLS_HH_
#define WCSIMPOLYGONTOOLS_HH_
#include "G4TwoVector.hh"

namespace WCSimPolygonTools {
	/**
	 * \brief Test if a regular polygon centred on (0,0) contains a certain point
	 * \param nSides Number of sides for the polygon
	 * \param radius Distance from the centre of polygon to the corners
	 * \param point Cartesian coordinates of point to test
	 * \return True if the polygon contains the point
	 */
	bool PolygonContains(unsigned int nSides, double radius, G4TwoVector point);

  /**
   * \brief Test is a regular polygon centred on (0,0) contains every corner of a certain square
   * \param nSides Number of sides for the polygon
   * \param radius Distance from the centre of the polygon to the corners
   * \param squareCorner The bottom left corner of the square
   * \param squareSide The side length of the square
   * \return True of the polygon contains every point on the square
   */
	bool PolygonContainsSquare(unsigned int nSides, double radius, G4TwoVector squareCorner, double squareSide);

	double GetSideFromRadius( unsigned int nSides, double radius );
	double GetRadiusFromSide( unsigned int nSides, double side);
	double GetAreaFromRadius( unsigned int nSides, double radius);
	double GetAreaFromSide( unsigned int nSides, double side);
	G4TwoVector ShiftVector( G4TwoVector vec, double x, double y );
  bool CheckPolygonSide( unsigned int nSides, double side );
  bool CheckPolygon( unsigned int nSides, double radius );
} /* namespace WCSimPolygonTools */
#endif /* WCSIMPOLYGONTOOLS_HH_ */
