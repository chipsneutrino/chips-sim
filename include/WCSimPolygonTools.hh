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
	 * \param outerRadius Distance from the centre of polygon to the corners
	 * \param point Cartesian coordinates of point to test
	 * \return True if the polygon contains the point
	 */
	bool PolygonContains(unsigned int nSides, double outerRadius, G4TwoVector point);

  /**
   * \brief Test is a regular polygon centred on (0,0) contains every corner of a certain square
   * \param nSides Number of sides for the polygon
   * \param outerRadius Distance from the centre of the polygon to the corners
   * \param squareCorner The bottom left corner of the square
   * \param squareSide The side length of the square
   * \return True if the polygon contains every point on the square
   */
	bool PolygonContainsSquare(unsigned int nSides, double outerRadius, G4TwoVector squareCorner, double squareSide);

	/**
	 * \brief Take a regular n-sided polygon centred on (0,0), cut it into slices along the lines
	 * joining every corner to the centre, and check if the jth slice contains every corner of a square
	 * \param nSides Number of sides for the polygon
	 * \param thetaStart Angle at which the slice begins (counting anticlockwise from theta = 0)
	 * \param thetaEnd Angle at which the slice ends (counting anticlockwise from theta = 0)
	 * \param outerRadius Distance from the centre of the polygon to the corners
	 * \param squareCorner The bottom left corner of the square
	 * \param squareSide The side length of the square
	 * \return True if the polygon contains every point on the square
	 */
	bool PolygonSliceContainsSquare(unsigned int nSides, double thetaStart, double thetaEnd,
																	double outerRadius, G4TwoVector squareCorner, double squareSide);

	/**
	 * \brief Take a regular n-sided polygon centred on (0,0), cut it into slices along the lines
	 * joining every corner to the centre, and check if the jth slice contains every corner of a square
	 * \param nSides Number of sides for the polygon
	 * \param thetaStart Angle at which the slice begins (counting anticlockwise from theta = 0)
	 * \param thetaEnd Angle at which the slice ends (counting anticlockwise from theta = 0)
	 * \param outerRadius Distance from the centre of the polygon to the corners
	 * \param point Cartesian coordinates of point to test
	 * \return True if the polygon contains every point on the square
	 */
	bool PolygonSliceContains(unsigned int nSides, double thetaStart, double thetaEnd,
														double outerRadius, G4TwoVector point);


	double GetSideFromRadius( unsigned int nSides, double outerRadius );
	double GetRadiusFromSide( unsigned int nSides, double side);
	double GetAreaFromRadius( unsigned int nSides, double outerRadius);
	double GetAreaFromSide( unsigned int nSides, double side);
	double GetSliceAreaFromRadius( unsigned int nSides, double outerRadius);
	double GetSliceAreaFromSide( unsigned int nSides, double side);
  double GetInnerRadiusFromOuter( unsigned int nSides, double outerRadius);
  double GetOuterRadiusFromInner( unsigned int nSides, double innerRadius);
  int IsInSliceNumber(unsigned int nSides, G4TwoVector point);
	G4TwoVector ShiftVector( G4TwoVector vec, double x, double y );
  bool CheckPolygonSide( unsigned int nSides, double side );
  bool CheckPolygon( unsigned int nSides, double outerRadius );

  double NormaliseAngle(double angle);
  bool IsAngleBetween(double testAngle, double startAngle, double endAngle);
} /* namespace WCSimPolygonTools */
#endif /* WCSIMPOLYGONTOOLS_HH_ */
