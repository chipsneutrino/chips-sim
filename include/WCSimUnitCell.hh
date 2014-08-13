/*
 * WCSimUnitCell.hh
 *
 *  Created on: 11 Aug 2014
 *      Author: aperch
 *
 */

#ifndef WCSIMUNITCELL_HH_
#define WCSIMUNITCELL_HH_
#include "WCSimPMTConfig.hh"
#include <vector>

/**
 * \class WCSimPMTPlacement
 * This class contains a single WCSimPMTConfig, which defines the PMT type, 
 * and the coordinates of its centre in a 2D unit cell.  A WCSimUnitCell object 
 * will contain a collection of these objects
 */
class WCSimPMTPlacement
{
public:
    /** \brief Constructor
     * @param pmt Pointer to PMT config that describes the PMT
     * @param x The PMT centre's x position in the unit cell (0.0 < x < 1.0) - Units must be METRES.
     * @param y The PMT centre's y position in the unit cell (0.0 < y < 1.0) - Units must be METRES
     */
    WCSimPMTPlacement(WCSimPMTConfig * pmt, double x, double y);
    virtual ~WCSimPMTPlacement();

    /** \brief Calculate the distance between two PMT placments
     * @param PMT placement to compare
     * @return Absolute distance between the two PMT centres, assuming they both live in a 1m x 1m square
     */
    double GetDistanceTo(WCSimPMTPlacement * pmt) const;  // Return the distance between the centres of two PMTs in the unit cell
    
    /**
     * \brief If the 1m x 1m unit square is scaled up to a square with a 
     * different side length, but the PMTs keep their relative positions,
     * do any of them overlap?
     * @param Side length of scaled square cell - Units must be METRES
     * @return True if any PMTs overlap.  False is no PMTs overlap.
     */
    bool OverlapsWith(WCSimPMTPlacement * pmt, double side) const; // Do the photocathodes overlap if this PMT is placed in a side x side square with another one?

    double GetX() const; //< @return PMT x-coordinate in metres
    double GetY() const; //< @return PMT y-coordinate in metres
    double GetPMTRadius() const; //< @return PMT radius in metres
    WCSimPMTConfig * GetPMTConfig() const; //< @return PMT config object
    void Print() const;
private:
    WCSimPMTConfig * fPMTConfig; //< Pointer to config object that specifies PMT type
    double fX; //< PMT centre x-position in the 1m x 1m unit cell (units are METRES)
    double fY; //< PMT centre y-position in the 1m x 1m unit cell (units are METRES)
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
    WCSimUnitCell(WCSimPMTConfig * pmt, double x, double y); // Constructor that adds the first PMT
    virtual ~WCSimUnitCell();
    
    /** \brief Place a new PMT in the cell
     * @param pmt Pointer to config object describing PMT type
     * @param x PMT centre's x-coordinate (0.0 < x < 1.0) - units are METRES
     * @param y PMT centre's y-coordinate (0.0 < y < 1.0) - units are METRES
     */
    void AddPMT(WCSimPMTConfig * pmt, double x, double y);  // Add a new PMT

    /** \brief If the 1m x 1m unit cell is scaled to have a different side length, 
     * but the PMT radii do not change, what percentage of the new cell is covered?
     * @param side Side length of scaled cell - units are METRES
     * @return Fraction of cell covered by all the PMTs
     */
    double GetPhotocathodeCoverage(double side) const;  // What fraction of a side x side square is PMT?

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
    double GetCellSizeForCoverage(double coverage) const;  // Returns the side length of the square cell that has the desired PMT coverage
    
    /**
     * \brief If the 1m x 1m cell is scaled to a different side length, do
     * any ofof the PMTs overlap one another?
     * @param side Length of side of square cell - units are METRES
     * @return True if any two PMTs overlap, false if no PMTs overlap
     */
    bool ContainsOverlaps(double side) const; // Do any PMTs overlap if the cell is a square of size side x side?  
    void Print() const;
private:
    std::vector<WCSimPMTPlacement> fPMTs; //< Collection of PMTs and their relative positions in the cell
};

#endif /* WCSIMUNITCELL_HH_ */
