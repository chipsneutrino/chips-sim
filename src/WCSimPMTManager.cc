#include <fstream>
#include <sstream>

// Use the rapidXML parser
#include <rapidxml-1.13/rapidxml.hpp>
#include <rapidxml-1.13/rapidxml_utils.hpp>

#include "WCSimPMTManager.hh"

// Default constructor
WCSimPMTManager::WCSimPMTManager(){

	// Default location for the pmt definitions file
	fConfigFile = getenv("WCSIMHOME");
	fConfigFile.append("/config/pmtDefinitions.xml");

	// Read the PMT types from the config file
	this->ReadPMTTypeList(); 

}

WCSimPMTManager::~WCSimPMTManager(){

}

void WCSimPMTManager::ReadPMTTypeList(){
   
	// Use the rapidxml xml parser.
	// It has some quirks, like using templates to pass options, so the code below looks strange.
	rapidxml::file<> xmlFile(fConfigFile.c_str());
	rapidxml::xml_document<> doc;
	doc.parse<0>(xmlFile.data());

	// The PMT definitions exist as "pmtDef" in the XML.
	for(rapidxml::xml_node<> *curNode = doc.first_node("pmtDef"); curNode; curNode = curNode->next_sibling("pmtDef")){
		// We have found a PMT definition, so create a PMT
		WCSimPMTConfig pmt;
		// Clear the temporary efficiency / wavelength vector
		fTempEffVec.clear();
		for (rapidxml::xml_attribute<> *curAttr = curNode->first_attribute(); curAttr; curAttr = curAttr->next_attribute()){
			this->FillPMTAttribute(pmt,curAttr);	
		}
		// We have filled the temporary efficiency vector. Now sort it and add to the pmt object.
		std::sort(fTempEffVec.begin(),fTempEffVec.end());
		pmt.SetEfficiencyVector(fTempEffVec);
		fPMTVector.push_back(pmt);
	}
 
}

void WCSimPMTManager::FillPMTAttribute(WCSimPMTConfig &pmt, rapidxml::xml_attribute<> *attr){

	std::string name = attr->name();
	// Define a stringstream to convert types conveniently
	std::stringstream ss; 
	ss << attr->value();

	if(name == "name"){
		std::string tempVal;
		ss >> tempVal;
		pmt.SetPMTName(tempVal);
	}
	else if(name == "radius"){
		double tempVal;
		ss >> tempVal;
		pmt.SetRadius(tempVal*m);
	}
	else if(name == "exposeHeight"){
		double tempVal;
		ss >> tempVal;
		pmt.SetExposeHeight(tempVal*m);
	}
	else if(name == "glassThickness"){
		double tempVal;
		ss >> tempVal;
		pmt.SetGlassThickness(tempVal*m);
	}
	else if(name.find("qe")!=std::string::npos){
		double tempWavelength, tempEff;
		ss >> tempWavelength >> tempEff;
		// Create a temporary pair and add it to the vector
		std::pair<double,double> tempPair(tempWavelength,tempEff);
		fTempEffVec.push_back(tempPair);
	}
	else{
		std::cerr << "WCSimPMTManager::FillPMTAttribute: Unexpected parameter " << attr->value() << std::endl;
	}

}

WCSimPMTConfig WCSimPMTManager::GetPMTByName(std::string name){

	unsigned int pmt = fPMTVector.size()+1;
	// Iterate through the PMTs to look for what we want.
	for(unsigned int p = 0; p < fPMTVector.size(); ++p){
		if(name == fPMTVector[p].GetPMTName()){
			pmt = p;
			break;
		}
	}

	WCSimPMTConfig curPMT;
	if(pmt < (fPMTVector.size()+1)){
		curPMT = fPMTVector[pmt];
	}
	else{
		std::cerr << "WCSIMPMTManager::GetPMTByName: PMT " << name << " does not exist. Returned default WCSimPMTConfig object." << std::endl;
    std::cout << "Available PMTs are: " << std::endl;
    for ( int i = 0 ; i < fPMTVector.size(); ++i ) 
    {
      std::cout << "    " << fPMTVector.at(i).GetPMTName() << std::endl;
	  }
  }

	return curPMT;		
}

