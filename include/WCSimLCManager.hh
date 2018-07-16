#ifndef WCSimLCManager_H
#define WCSimLCManager_H

#include "WCSimLCConfig.hh"

#include <TObject.h>
#include <string>
// This is a class that builds a list of defined LC types and
// provides the access interface to them.

class WCSimLCManager: public TObject {

	public:

		// Default constructor
		WCSimLCManager();

		// Destructor
		~WCSimLCManager();

		// Get a LC object by the LC name
		WCSimLCConfig GetLCByName(std::string name);

	private:

		// Read the list of LC types from the config file
		void ReadLCTypeList();

		// Fill LC object attribute from xml file
		void FillLCAttribute(WCSimLCConfig &lc, const std::string &attrName, const std::string &attrValue);

		// Vector to store the requested LC types
		std::vector<WCSimLCConfig> fLCVector;

		// Temporary shape Z-R vector. Filled and used for each LC in turn.
		std::vector<std::pair<double, double> > fTempShapeVec;

		std::string fConfigFile;ClassDef(WCSimLCManager,1)
};

#endif
