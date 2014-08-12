#ifndef WCSimPMTManager_H
#define WCSimPMTManager_H 1

#include "WCSimGeoConfig.hh"

#include <rapidxml-1.13/rapidxml.hpp>

// This is a class that builds a list of defined PMT types and
// provides the access interface to them.
class WCSimGeoManager {

	public:

		// Default constructor
		WCSimGeoManager();

		// Destructor
		~WCSimGeoManager();

		WCSimGeoConfig GetGeometryByName(std::string name) const;
	private:

		// Read the list of PMT types from the config file
		void ReadGeometryList();

		// Fill PMT object attribute from xml file
		void FillGeoAttribute(WCSimGeoConfig &pmt, rapidxml::xml_attribute<> *attr);

		// Vector to store the requested PMT types
		std::vector<WCSimGeoConfig> fGeoVector;

		std::string fConfigFile;
};

#endif