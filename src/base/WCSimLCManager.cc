#include <fstream>
#include <sstream>

// Use the rapidXML parser
#include <rapidxml-1.13/rapidxml.hpp>
#include <rapidxml-1.13/rapidxml_utils.hpp>

#include "WCSimLCManager.hh"
#include <iostream>
#include "globals.hh"

#ifndef REFLEX_DICTIONARY
ClassImp(WCSimLCManager)
#endif

	// Default constructor
	WCSimLCManager::WCSimLCManager()
{

	// Default location for the lc definitions file
	fConfigFile = getenv("CHIPSSIM");
	fConfigFile.append("/config/lc_definitions.xml");

	// Read the LC types from the config file
	this->ReadLCTypeList();
}

WCSimLCManager::~WCSimLCManager()
{
}

void WCSimLCManager::ReadLCTypeList()
{

	// Use the rapidxml xml parser.
	// It has some quirks, like using templates to pass options, so the code below looks strange.
	rapidxml::file<> xmlFile(fConfigFile.c_str());
	rapidxml::xml_document<> doc;
	doc.parse<0>(xmlFile.data());

	// The LC definitions exist as "lcDef" in the XML.
	for (rapidxml::xml_node<> *curNode = doc.first_node("lcDef"); curNode; curNode = curNode->next_sibling("lcDef"))
	{
		// We have found a LC definition, so create a LC
		WCSimLCConfig lc;
		// Clear the temporary Z / R vector
		fTempShapeVec.clear();
		for (rapidxml::xml_attribute<> *curAttr = curNode->first_attribute(); curAttr; curAttr =
																						   curAttr->next_attribute())
		{
			this->FillLCAttribute(lc, curAttr->name(), curAttr->value());
		}
		// We have filled the temporary LC Z-R vector. Now sort it and add to the lc object.
		std::sort(fTempShapeVec.begin(), fTempShapeVec.end());
		lc.SetShapeVector(fTempShapeVec);
		fLCVector.push_back(lc);
	}
}

void WCSimLCManager::FillLCAttribute(WCSimLCConfig &lc, const std::string &attrName, const std::string &attrValue)
{

	// Define a stringstream to convert types conveniently
	std::stringstream ss;
	ss << attrValue;

	if (attrName == "name")
	{
		std::string tempVal;
		ss >> tempVal;
		lc.SetName(tempVal);
	}
	else if (attrName.find("lc") != std::string::npos)
	{
		double tempZ, tempR;
		ss >> tempZ >> tempR;
		// Create a temporary pair and add it to the vector
		std::pair<double, double> tempPair(tempZ, tempR);
		fTempShapeVec.push_back(tempPair);
	}
	else
	{
		std::cerr << "WCSimLCManager::FillLCAttribute: Unexpected parameter " << attrValue << std::endl;
	}
}

WCSimLCConfig WCSimLCManager::GetLCByName(std::string name)
{

	unsigned int lc = fLCVector.size() + 1;
	// Iterate through the LCs to look for what we want.
	for (unsigned int p = 0; p < fLCVector.size(); ++p)
	{
		if (name == fLCVector[p].GetLCName())
		{
			lc = p;
			break;
		}
	}

	WCSimLCConfig curLC;
	if (lc < (fLCVector.size() + 1))
	{
		curLC = fLCVector[lc];
	}
	else
	{
		std::cerr << "WCSIMLCManager::GetLCByName: LC " << name
				  << " does not exist. Returned default WCSimLCConfig object." << std::endl;
		std::cout << "Available LCs are: " << std::endl;
		for (unsigned int i = 0; i < fLCVector.size(); ++i)
		{
			std::cout << "    " << fLCVector.at(i).GetLCName() << std::endl;
		}
	}

	return curLC;
}
