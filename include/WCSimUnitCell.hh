/*
 * WCSimUnitCell.hh
 *
 *  Created on: 11 Aug 2014
 *      Author: aperch
 *
 *  PMT rotation feature added on: 10 Dec 2015
 *      by  S. Germani
 */

#ifndef WCSIMUNITCELL_HH_
#define WCSIMUNITCELL_HH_
#include "WCSimPMTConfig.hh"
#include "WCSimGeometryEnums.hh"
#include "G4TwoVector.hh"
#include <vector>

/**
 * \class WCSimPMTPlacement
 * This class contains a single WCSimPMTConfig, which defines the PMT type, 
 * and the coordinates of its centre in a 2D unit cell.  A WCSimUnitCell object 
 * will contain a collection of these objects.  I use the convention that (0,0)
 * is the top-left corner of the unit cell because that's how I was taught 2D
 * crystallography in Condensed Matter Physics.
 */
class WCSimPMTPlacement
{
public:
	/** \brief Constructor
		 * @param pmt Pointer to PMT config that describes the PMT
		 * @param x The PMT centre's x position in the unit cell (0.0 < x < 1.0) - Units must be METRES.
		 * @param y The PMT centre's y position in the unit cell (0.0 < y < 1.0) - Units must be METRES
		 */
	WCSimPMTPlacement(WCSimPMTConfig pmt, double x, double y);
	WCSimPMTPlacement(WCSimPMTConfig pmt, double x, double y, WCSimGeometryEnums::PMTDirection_t dir, double theta,
					  double phi);
	virtual ~WCSimPMTPlacement();

	/** \brief Calculate the distance between two PMT placments
		 * @param PMT placement to compare
		 * @return Absolute distance between the two PMT centres, assuming they both live in a 1m x 1m square
		 */
	double GetDistanceTo(WCSimPMTPlacement *pmt) const; // Return the distance between the centres of two PMTs in the unit cell

	/**
		 * \brief If the 1m x 1m unit square is scaled up to a square with a
		 * different side length, but the PMTs keep their relative positions,
		 * do any of them overlap?
		 * @param Side length of scaled square cell - Units must be METRES
		 * @return True if any PMTs overlap.  False is no PMTs overlap.
		 */
	bool OverlapsWith(WCSimPMTPlacement *pmt, double side) const; // Do the photocathodes overlap if this PMT is placed in a side x side square with another one?

	double GetX() const;							   //< @return PMT x-coordinate in metres
	double GetY() const;							   //< @return PMT y-coordinate in metres
	double GetTheta() const;						   //< @return PMT theta in radians
	double GetPhi() const;							   //< @return PMT phi   in radians
	WCSimGeometryEnums::PMTDirection_t GetDir() const; //< @return PMT direction

	double GetPMTRadius() const;		 //< @return PMT radius in metres
	WCSimPMTConfig GetPMTConfig() const; //< @return PMT config object
	void Print() const;

	void SetTheta(double theta);						 //< set PMT theta in radians
	void SetPhi(double phi);							 //< set PMT phi   in radians
	void SetDir(WCSimGeometryEnums::PMTDirection_t dir); //< set PMT direction

private:
	WCSimPMTConfig fPMTConfig; //< Pointer to config object that specifies PMT type
	double fX;				   //< PMT centre x-position in the 1m x 1m unit cell (units are METRES)
	double fY;				   //< PMT centre y-position in the 1m x 1m unit cell (units are METRES)

	WCSimGeometryEnums::PMTDirection_t fDir;
	double fTheta; //< PMT Theta angle in the unit cell (units are RADIANS)
	double fPhi;   //< PMT Phi   angle in the unit cell (units are RADIANS)
};

class WCSimUnitCell
{
public:
	WCSimUnitCell(); //< \brief Default constructor

	/** \brief Constructor that places the first PMT
		 * @param pmt Pointer to config object describing PMT type
		 * @param x PMT centre's x-coordinate (0.0 < x < 1.0) - units are METRES
		 * @param y PMT centre's y-coordinate (0.0 < y < 1.0) - units are METRES
		 */
	WCSimUnitCell(const WCSimPMTConfig &pmt, double x, double y); // Constructor that adds the first PMT
	WCSimUnitCell(const WCSimPMTConfig &pmt, double x, double y, WCSimGeometryEnums::PMTDirection_t dir,
				  double theta, double phi); // Constructor that adds the first PMT
	virtual ~WCSimUnitCell();

	/** \brief Place a new PMT in the cell
		 * @param pmt Pointer to config object describing PMT type
		 * @param x PMT centre's x-coordinate (0.0 < x < 1.0) - units are METRES
		 * @param y PMT centre's y-coordinate (0.0 < y < 1.0) - units are METRES
		 */
	void AddPMT(const WCSimPMTConfig &pmt, double x, double y); // Add a new PMT
	void AddPMT(const WCSimPMTConfig &pmt, double x, double y, WCSimGeometryEnums::PMTDirection_t dir, double theta,
				double phi); // Add a new PMT

	void SetPMTTheta(unsigned int pmt, double theta);						  //< set PMT theta in radians
	void SetPMTPhi(unsigned int pmt, double phi);							  //< set PMT phi   in radians
	void SetPMTDir(unsigned int pmt, WCSimGeometryEnums::PMTDirection_t dir); //< set PMT direction

	/** \brief If the 1m x 1m unit cell is scaled to have a different side length,
		 * but the PMT radii do not change, what percentage of the new cell is covered?
		 * @param side Side length of scaled cell - units are METRES
		 * @return Fraction of cell covered by all the PMTs
		 */
	double GetPhotocathodeCoverage(double side) const; // What fraction of a side x side square is PMT?

	double GetPhotocathodeArea() const;

	/**
		 * \brief Get the smalled possible square side length so that no PMTs
		 * overlap with each other or the edge of the cell
		 * @param side Side length of the scaled cell - units are METRES
		 * @return Minimum side length - units are METRES
		 */
	double GetMinimumCellSize() const; // Returns side length of the smallest square that will fit all the PMTs without overlapping

	/**
		 * \brief Get the side length for a square cell that has the desired PMT
		 * coverage.  Will check that this is larger than the minimum allowed size
		 * @param coverage Fraction of cell to be covered by PMTs
		 * @return Side length required - units are METRES
		 */
	double GetCellSizeForCoverage(double coverage) const; // Returns the side length of the square cell that has the desired PMT coverage

	/**
		 * \brief If the 1m x 1m cell is scaled to a different side length, do
		 * any ofof the PMTs overlap one another?
		 * @param side Length of side of square cell - units are METRES
		 * @return True if any two PMTs overlap, false if no PMTs overlap
		 */
	bool ContainsOverlaps(double side) const; // Do any PMTs overlap if the cell is a square of size side x side?

	/**
		 * \brief Get the expose height of the most protruding PMT in this cell
		 * @return The largest PMT expose heigh in the cell - units are METRES
		 */
	double GetCellExposeHeight() const;

	/**
		 * \brief Get the number of PMTs in the cell
		 * @return Number of PMTs in the cell
		 */
	unsigned int GetNumPMTs() const;

	/**
		 * \brief Get the position of a given PMT (in the order they were added) relative
		 * 		  to the top-left corner of a square cell with a given side length
		 * @param pmt Which PMT (by vector index) to consider
		 * @param side Side length to scale the square cell up to - units are METRES
		 * @return The (x,y) coordinates of the PMT - units are METRES
		 */
	G4TwoVector GetPMTPos(unsigned int pmt, double side = 1.0) const;

	/**
		 * \brief Get the vector of all the PMT placements in the cell
		 * @return Vector of all the PMTs and their relative positions in a 1x1m cell
		 */
	std::vector<WCSimPMTPlacement> GetPMTPlacements() const;

	/**
		 * \brief Get the placement object corresponding to a given PMT
		 * @param pmt Which PMT (by vector index) to return
		 * @return Placement object corresponding to the chosen PMT
		 */
	WCSimPMTPlacement GetPMTPlacement(unsigned int pmt) const;
	void Print() const;

private:
	std::vector<WCSimPMTPlacement> fPMTs; //< Collection of PMTs and their relative positions in the cell
};

#endif /* WCSIMUNITCELL_HH_ */
