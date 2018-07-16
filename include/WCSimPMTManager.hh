#ifndef WCSimPMTManager_H
#define WCSimPMTManager_H

#include "WCSimPMTConfig.hh"

#include <TObject.h>
// This is a class that builds a list of defined PMT types and
// provides the access interface to them.
class WCSimPMTManager: public TObject {

	public:

		// Default constructor
		WCSimPMTManager();

		// Destructor
		~WCSimPMTManager();

		// Get a PMT object by the PMT name
		WCSimPMTConfig GetPMTByName(std::string name);

	private:

		// Read the list of PMT types from the config file
		void ReadPMTTypeList();

		// Fill PMT object attribute from xml file
		void FillPMTAttribute(WCSimPMTConfig &pmt, const std::string &attrName, const std::string &attrValue);

		// Vector to store the requested PMT types
		std::vector<WCSimPMTConfig> fPMTVector;

		// Temporary efficiency vector. Filled and used for each PMT in turn.
		std::vector<std::pair<double, double> > fTempEffVec;

		std::string fConfigFile;ClassDef(WCSimPMTManager,1);
};

#endif
