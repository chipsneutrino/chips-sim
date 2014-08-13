#include <fstream>
#include <sstream>

// Use the rapidXML parser
#include <rapidxml-1.13/rapidxml.hpp>
#include <rapidxml-1.13/rapidxml_utils.hpp>

#include "WCSimGeoManager.hh"

// Default constructor
WCSimGeoManager::WCSimGeoManager(){

	// Default location for the pmt definitions file
	fConfigFile = getenv("WCSIMHOME");
	fConfigFile.append("/config/geometryDefinitions.xml");

	// Read the geometry definitions from the config file
	this->ReadGeometryList(); 

}

WCSimGeoManager::~WCSimGeoManager(){

}

void WCSimGeoManager::ReadGeometryList(){
   
	// Use the rapidxml xml parser.
	// It has some quirks, like using templates to pass options, so the code below looks strange.
	rapidxml::file<> xmlFile(fConfigFile.c_str());
	rapidxml::xml_document<> doc;
	doc.parse<0>(xmlFile.data());

	// The geometry definitions exist as "geoDef" in the XML.
	for(rapidxml::xml_node<> *curNode = doc.first_node("geoDef"); curNode; curNode = curNode->next_sibling("geoDef")){
		// We have found a geometry definition, so create a geometry config object
		WCSimGeoConfig geo;
		for (rapidxml::xml_attribute<> *curAttr = curNode->first_attribute(); curAttr; curAttr = curAttr->next_attribute()){
			this->FillGeoAttribute(geo,curAttr);	
		}

		for(rapidxml::xml_node<> *childNode = curNode->first_node("cellPMTDef"); childNode; childNode = childNode->next_sibling("cellPMTDef"))
	    {
	        for (rapidxml::xml_attribute<> *curAttr = childNode->first_attribute(); curAttr; curAttr = curAttr->next_attribute()){
	            this->FillCellAttribute(geo, curAttr);
	        }
	    }
		assert(geo.IsGood());
		fGeoVector.push_back(geo);
	}
 
}

void WCSimGeoManager::FillGeoAttribute(WCSimGeoConfig &geo, rapidxml::xml_attribute<> *attr){

	std::string name = attr->name();
	// Define a stringstream to convert types conveniently
	std::stringstream ss; 
	ss << attr->value();

	if(name == "name"){
		std::string tempVal;
		ss >> tempVal;
		geo.SetGeoName(tempVal);
	}
	else if(name == "innerRadius"){
		double tempVal;
		ss >> tempVal;
		geo.SetInnerRadius(tempVal*m);
	}
	else if(name == "innerHeight"){
		double tempVal;
		ss >> tempVal;
		geo.SetInnerHeight(tempVal*m);
	}
	else if(name == "nSides"){
		unsigned int tempVal;
		ss >> tempVal;
		geo.SetNSides(tempVal);
	}
  else if(name == "perCentCoverage"){
    double tempVal;
    ss >> tempVal;
    geo.SetCoverage(tempVal);
  }

	else{
		std::cerr << "WCSimGeoManager::FillGeoAttribute: Unexpected parameter " << attr->name() << ", " << attr->value() << std::endl;
	}

}

void WCSimGeoManager::FillCellAttribute(WCSimGeoConfig &geo, rapidxml::xml_attribute<> *attr){

    std::string name = attr->name();
    // Define a stringstream to convert types conveniently
    std::stringstream ss;
    ss << attr->value();

	  if(name.find("cellPMTName") != std::string::npos){
	  	std::string tempVal;
	  	ss >> tempVal;
	  	geo.AddCellPMTName(tempVal);
	  }
	  else if(name.find("cellPMTX") != std::string::npos){
	  	double tempVal;
	  	ss >> tempVal;
	  	geo.AddCellPMTX(tempVal * m);
	  }
	  else if(name.find("cellPMTY") != std::string::npos){
	          double tempVal;
	          ss >> tempVal;
	          geo.AddCellPMTY(tempVal * m);
	  }
    else{
        std::cerr << "WCSimGeoManager::FillCellAttribute: Unexpected parameter " << attr->name() << ", " << attr->value() << std::endl;
    }
    return;
}

WCSimGeoConfig WCSimGeoManager::GetGeometryByName(std::string name) const{

	unsigned int geo = fGeoVector.size()+1;
	// Iterate through the PMTs to look for what we want.
	for(unsigned int g = 0; g < fGeoVector.size(); ++g){
		if(name == fGeoVector[g].GetGeoName()){
			geo = g;
			break;
		}
	}

	WCSimGeoConfig curGeo;
	if(geo < (fGeoVector.size()+1)){
		curGeo = fGeoVector[geo];
	}
	else{
		std::cerr << "WCSIMGeoManager::GetGeometryByName: Geometry " << name << " does not exist. Returned default WCSimGeoConfig object." << std::endl;
	}

	return curGeo;		
}

