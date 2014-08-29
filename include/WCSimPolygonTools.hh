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
	 * \param radius Distance from centre of polygon the corners
	 * \param point Cartesian coordinates of point to test
	 * \return True if the polygon contains the point
	 */
	bool PolygonContains(unsigned int nSides, double radius, G4TwoVector point);

	double GetSideFromRadius( unsigned int nSides, double radius );
	double GetRadiusFromSide( unsigned int nSides, double side);
	double GetAreaFromRadius( unsigned int nSides, double radius);
	double GetAreaFromSide( unsigned int nSides, double side);
	G4TwoVector ShiftVector( G4TwoVector vec, double x, double y );
} /* namespace WCSimPolygonTools */
#endif /* WCSIMPOLYGONTOOLS_HH_ */
