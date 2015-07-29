#ifndef WCSimLCConfig_H
#define WCSimLCConfig_H 1

#include <vector>
#include <string>


#include <TObject.h>

// This class is designed to store the information about a type of LC.

class WCSimLCConfig : public TObject {
public:
	// Default constructor
	WCSimLCConfig();
	// Copy constructor
	WCSimLCConfig(const WCSimLCConfig &rhs);
	// Destructor
	~WCSimLCConfig();

        std::vector<std::pair<double,double> > GetShapeVector() const;
	void SetShapeVector(std::vector<std::pair<double,double> > shapeVec);

	// Not set function for this, performed by SetEfficiency.
	double GetExposeHeight() const;
	double GetMaxRadius() const;

	// No set function for this, performed by SetEfficiency
	unsigned int GetNShapePoints() const;

	std::string GetLCName() const;
	void SetName(std::string name);

	void Print() const;
	
private:

  	double fMaxRadius;
	double fExposeHeight;
	// Polycone shape  stored in a pair of <Z,R>
	std::vector<std::pair<double,double> > fShapeVec;
        unsigned int fShapePoints;
	std::string fName;
  ClassDef(WCSimLCConfig,1)  
};

#endif
