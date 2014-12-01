#ifndef WCSimGeoManager_H
#define WCSimGeoManager_H

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
    bool GeometryExists( std::string name ) const;
	private:

		// Read the list of geometry types from the config file
		void ReadGeometryList();

		// Fill geometry object attribute from xml file
		void FillGeoAttribute(WCSimGeoConfig &geo, rapidxml::xml_attribute<> *attr);

		// Fill geometry object attribute for a subregion of the detector from the xml file
		void FillRegion(WCSimGeoConfig &geo, rapidxml::xml_node<> *node);

    // Fill unit cell object attribute from xml file
		void FillCell(WCSimGeoConfig &geo, rapidxml::xml_node<> *node);

		// Fill geometry object corresponding to a limit on available PMTs
		void FillPMTLimit(WCSimGeoConfig &geo, rapidxml::xml_node<> *node);

		bool MeansYes(std::string str);
		bool MeansNo(std::string str);

		// Vector to store the requested geometry types
		std::vector<WCSimGeoConfig> fGeoVector;

		std::string fConfigFile;
};

#endif
