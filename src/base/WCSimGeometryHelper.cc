/*
 * WCSimGeometryHelper.cc
 *
 *  Created on: 3 Dec 2014
 *      Author: ajperch
 */
#include "rapidxml-1.13/rapidxml.hpp"
#include "rapidxml-1.13/rapidxml_print.hpp"
#include "WCSimGeometryHelper.hh"
#include "WCSimGeometryEnums.hh"
#include <string>
#include <iostream>
#include <sstream>
#include <math.h>
#include <fstream>
#include <TString.h>
#include <algorithm>

WCSimGeometryHelper::WCSimGeometryHelper()
{
	// TODO Auto-generated constructor stub
	fPercentCoverage = 0.0;
}

WCSimGeometryHelper::~WCSimGeometryHelper()
{
	// TODO Auto-generated destructor stub
}

void WCSimGeometryHelper::Run()
{
	printf("Welcome to the Geometry Helper\n");
	printf("This program is designed to help you build a geometry xml file by answering simple questions\n");

	while (MakeGeometry())
	{
		rapidxml::xml_node<> *detector = fDoc.allocate_node(rapidxml::node_element, "geoDef", "");

		const char *name = fDoc.allocate_string(GetName().c_str());
		rapidxml::xml_attribute<> *nameAttr = fDoc.allocate_attribute("name", name);

		const char *innerRadius = fDoc.allocate_string(GetInnerRadius().c_str());
		rapidxml::xml_attribute<> *innerRadiusAttr = fDoc.allocate_attribute("innerRadius", innerRadius);

		const char *innerHeight = fDoc.allocate_string(GetInnerHeight().c_str());

		rapidxml::xml_attribute<> *innerHeightAttr = fDoc.allocate_attribute("innerHeight", innerHeight);

		const char *nSides = fDoc.allocate_string(GetNSides().c_str());
		rapidxml::xml_attribute<> *nSidesAttr = fDoc.allocate_attribute("nSides", nSides);

		const char *coverageType = fDoc.allocate_string(GetCoverageType().c_str());
		rapidxml::xml_attribute<> *coverageTypeAttr = fDoc.allocate_attribute("coverageType", coverageType);

		const char *vetoSize = fDoc.allocate_string(GetVetoSize().c_str());
		rapidxml::xml_attribute<> *vetoSizeAttr = fDoc.allocate_attribute("vetoSize", vetoSize);

		detector->append_attribute(nameAttr);
		detector->append_attribute(innerRadiusAttr);
		detector->append_attribute(innerHeightAttr);
		detector->append_attribute(nSidesAttr);
		detector->append_attribute(coverageTypeAttr);

		if (coverageType == WCSimGeometryEnums::PhotodetectorLimit_t::AsString(
								WCSimGeometryEnums::PhotodetectorLimit_t::kPercentCoverage))
		{
			const char *coverage = fDoc.allocate_string(GetCoverage().c_str());
			rapidxml::xml_attribute<> *coverageAttr = fDoc.allocate_attribute("coverage", coverage);
			detector->append_attribute(coverageAttr);
		}
		detector->append_attribute(vetoSizeAttr);

		bLRsymmetry = AssumeLRsymmetry() ? true : false;
		bTBsymmetry = AssumeTBsymmetry() ? true : false;
		SetWallZones(detector);
		SetTopZones(detector);
		if (!bTBsymmetry)
			SetBottomZones(detector);
		fDoc.append_node(detector);

		std::cout << fDoc << std::endl;

		SaveGeometry();
	}
}

bool WCSimGeometryHelper::AskYesNo()
{
	bool yesNo = false;

	std::ostringstream optionText;
	optionText << "\tEnter yes (y) or no (n): ";

	bool gotOption = false;
	while (!gotOption)
	{
		std::cout << optionText.str() << "\t";
		std::string input;
		getline(std::cin, input);
		std::cout << std::endl;

		// This code converts from string to number safely.
		std::stringstream myStream(input);
		std::string choice;
		if (myStream >> choice)
		{
			if (choice == "yes" || choice == "y" || choice == "YES" || choice == "Y")
			{
				yesNo = true;
				gotOption = true;
			}
			else if (choice == "no" || choice == "n" || choice == "NO" || choice == "N")
			{
				yesNo = false;
				gotOption = true;
			}
		}
		else if (!gotOption)
		{
			std::cout << "\tInvalid entry, please try again" << std::endl;
		}
	}
	return yesNo;
}

int WCSimGeometryHelper::AskPositiveInteger(unsigned int min, unsigned int max)
{
	int myInt = -999;

	std::ostringstream optionText;

	// Can we take any integer or a range?
	if (min != max)
	{
		optionText << "\tEnter an integer between " << min << " and " << max << ":";
	}
	else
	{
		optionText << "\tEnter a positive integer: ";
	}

	bool gotOption = false;
	while (!gotOption)
	{
		std::cout << optionText.str() << "\t";
		std::string input;
		getline(std::cin, input);
		std::cout << std::endl;

		// This code converts from string to number safely.
		std::stringstream myStream(input);

		if (input.find_first_of(".") == input.npos && myStream >> myInt)
		{
			if (myInt > 0 && (min == max || ((unsigned int)(myInt) >= min && (unsigned int)(myInt) <= max)))
			{
				gotOption = true;
			}
		}
		else if (!gotOption)
		{
			std::cout << "\tInvalid entry, please try again" << std::endl;
		}
	}
	return myInt;
}

int WCSimGeometryHelper::AskInteger(int min, int max)
{
	int myInt = -999;

	std::ostringstream optionText;

	// Can we take any integer or a range?
	if (min != max)
	{
		optionText << "\tEnter an integer between " << min << " and " << max << ":";
	}
	else
	{
		optionText << "\tEnter an integer: ";
	}

	bool gotOption = false;
	while (!gotOption)
	{
		std::cout << optionText.str() << "\t";
		std::string input;
		getline(std::cin, input);
		std::cout << std::endl;

		// This code converts from string to number safely.
		std::stringstream myStream(input);

		if (input.find_first_of(".") == input.npos && myStream >> myInt)
		{
			if (min == max || (myInt >= min && myInt <= max))
			{
				gotOption = true;
			}
		}
		else if (!gotOption)
		{
			std::cout << "\tInvalid entry, please try again" << std::endl;
		}
	}
	return myInt;
}

double WCSimGeometryHelper::AskDouble(double min, double max)
{
	double myDouble = -999.9;

	std::ostringstream optionText;

	// Can we take any integer or a range?
	if (min != max)
	{
		optionText << "\tEnter a number between " << min << " and " << max << ":";
	}
	else
	{
		optionText << "\tEnter a number: ";
	}

	bool gotOption = false;
	while (!gotOption)
	{
		std::cout << optionText.str() << "\t";
		std::string input;
		getline(std::cin, input);
		std::cout << std::endl;

		// This code converts from string to number safely.
		std::stringstream myStream(input);
		if (myStream >> myDouble)
		{
			if (min == max || (myDouble >= min || myDouble <= max))
			{
				gotOption = true;
			}
		}
		else if (!gotOption)
		{
			std::cout << "\tInvalid entry, please try again" << std::endl;
		}
	}
	return myDouble;
}

void WCSimGeometryHelper::Usage()
{
}

bool WCSimGeometryHelper::MakeGeometry()
{
	std::cout << "Would you like to make a new geometry?" << std::endl;
	return AskYesNo();
}

bool WCSimGeometryHelper::AssumeLRsymmetry()
{
	std::cout << "Would you like to assume left-right symmetry in wall geometry?" << std::endl;
	return AskYesNo();
}

bool WCSimGeometryHelper::AssumeTBsymmetry()
{
	std::cout << "Would you like to mirror top and bottom caps ?" << std::endl;
	return AskYesNo();
}

void WCSimGeometryHelper::SetCapZones(bool isTop, rapidxml::xml_node<> *parentNode)
{
	std::string region;
	if (isTop)
	{
		region = WCSimGeometryEnums::DetectorRegion_t::AsString(WCSimGeometryEnums::DetectorRegion_t::kTop);
	}
	else
	{
		region = WCSimGeometryEnums::DetectorRegion_t::AsString(WCSimGeometryEnums::DetectorRegion_t::kBottom);
	}

	std::cout << "Now specify the PMT layout for the " << region << " region of the detector" << std::endl;
	std::vector<std::pair<double, double>> boundaries = GetZoneBoundaries();

	for (unsigned int i = 0; i < boundaries.size(); ++i)
	{
		rapidxml::xml_node<> *regionNode = fDoc.allocate_node(rapidxml::node_element, "region");
		std::cout << "Now let's describe the layout of zone " << i << std::endl;

		if (fCoverageType == WCSimGeometryEnums::PhotodetectorLimit_t::kTotalNumber)
		{
			rapidxml::xml_node<> *coverageNode = fDoc.allocate_node(rapidxml::node_element, "coverage", "1");
			regionNode->append_node(coverageNode);
		}
		else if (fCoverageType == WCSimGeometryEnums::PhotodetectorLimit_t::kZonalCoverage)
		{
			const char *coverage = fDoc.allocate_string(GetCoverage().c_str());
			rapidxml::xml_node<> *coverageNode = fDoc.allocate_node(rapidxml::node_element, "coverage", coverage);
			regionNode->append_node(coverageNode);
		}
		else if (fCoverageType == WCSimGeometryEnums::PhotodetectorLimit_t::kPercentCoverage)
		{
			std::stringstream ss;
			ss << fPercentCoverage;
			const char *coverage = fDoc.allocate_string(ss.str().c_str());
			rapidxml::xml_node<> *coverageNode = fDoc.allocate_node(rapidxml::node_element, "coverage", coverage);
			regionNode->append_node(coverageNode);
		}

		std::stringstream ssRegion;
		ssRegion << region;
		const char *regionStr = fDoc.allocate_string(ssRegion.str().c_str());
		rapidxml::xml_node<> *locationNode = fDoc.allocate_node(rapidxml::node_element, "location", regionStr);
		regionNode->append_node(locationNode);

		std::stringstream ss;
		ss << i;
		const char *zoneStr = fDoc.allocate_string(ss.str().c_str());
		rapidxml::xml_node<> *zoneNode = fDoc.allocate_node(rapidxml::node_element, "zone", zoneStr);

		std::stringstream ssStart;
		ssStart << boundaries.at(i).first;
		const char *startStr = fDoc.allocate_string(ssStart.str().c_str());
		rapidxml::xml_node<> *startNode = fDoc.allocate_node(rapidxml::node_element, "startAngle", startStr);

		std::stringstream ssEnd;
		ssEnd << boundaries.at(i).second;
		const char *endStr = fDoc.allocate_string(ssEnd.str().c_str());
		rapidxml::xml_node<> *endNode = fDoc.allocate_node(rapidxml::node_element, "endAngle", endStr);

		regionNode->append_node(zoneNode);
		regionNode->append_node(startNode);
		regionNode->append_node(endNode);

		if (fCoverageType == WCSimGeometryEnums::PhotodetectorLimit_t::kTotalNumber)
		{
			std::vector<std::string> uniquePMTs;
			rapidxml::xml_node<> *unitCellNode = GetUnitCellNode(uniquePMTs);
			regionNode->append_node(unitCellNode);
			for (unsigned int i = 0; i < uniquePMTs.size(); ++i)
			{
				rapidxml::xml_node<> *pmtLimitNode = GetPMTLimitNode(uniquePMTs.at(i));
				regionNode->append_node(pmtLimitNode);
			}
		}
		else
		{
			rapidxml::xml_node<> *unitCellNode = GetUnitCellNode();
			regionNode->append_node(unitCellNode);
		}
		parentNode->append_node(regionNode);
	}
	//Yibin's hack: mirror bottomCap to the topCap
	if (bTBsymmetry)
	{
		const char *bottomStr = fDoc.allocate_string(
			WCSimGeometryEnums::DetectorRegion_t::AsString(WCSimGeometryEnums::DetectorRegion_t::kBottom).c_str());
		const char *topStr = fDoc.allocate_string(
			WCSimGeometryEnums::DetectorRegion_t::AsString(WCSimGeometryEnums::DetectorRegion_t::kTop).c_str());
		rapidxml::xml_node<> *regionNode = parentNode->first_node("region");
		while (regionNode)
		{
			std::string loc = regionNode->first_node("location")->value();
			if (loc.compare(topStr) != 0)
			{
				regionNode = regionNode->next_sibling();
			}
			else
			{
				break;
			}
		}
		while (regionNode)
		{
			std::string loc = regionNode->first_node("location")->value();
			if (loc.compare(topStr) == 0)
			{
				rapidxml::xml_node<> *newNode = fDoc.clone_node(regionNode);
				newNode->first_node("location")->value(bottomStr);
				std::cout << "haha kBottom node:" << *newNode;
				parentNode->append_node(newNode);
				regionNode = regionNode->next_sibling();
			}
			else
			{
				break;
			}
		}
	}

	return;
}

void WCSimGeometryHelper::SetBottomZones(rapidxml::xml_node<> *parentNode)
{
	SetCapZones(false, parentNode);
}

void WCSimGeometryHelper::SetTopZones(rapidxml::xml_node<> *parentNode)
{
	SetCapZones(true, parentNode);
}

void WCSimGeometryHelper::SetWallZones(rapidxml::xml_node<> *parentNode)
{
	std::string region = WCSimGeometryEnums::DetectorRegion_t::AsString(WCSimGeometryEnums::DetectorRegion_t::kWall);

	fSetWallZones.clear();
	fSetWallZones.resize(fNSides, 0);

	// While there are still unset zones
	while (std::find(fSetWallZones.begin(), fSetWallZones.end(), 0) != fSetWallZones.end())
	{
		std::cout << "Now specify the PMT layout for the " << region << " region of the detector" << std::endl;

		std::cout << "To save time you can provide an identical layout for multiple zones at once" << std::endl;
		std::cout << "Note: zone 0 is the furthest downstream wall relative to the beam" << std::endl;
		std::cout << "Unset zones are: ";
		std::vector<int> zonesToSet;
		for (unsigned int i = 0; i < fSetWallZones.size(); ++i)
		{
			if (fSetWallZones.at(i) == 0)
			{
				std::cout << i << " ";
			}
		}
		std::cout << std::endl;
		std::cout << "Which zones would you like to set? " << std::endl;

		int zonesChosen = 0;

		while (true) // I don't like this any more than you do
		{
			// Break if that's enough zones for now
			int zoneToSet = AskInteger((zonesChosen == 0) - 1, fNSides - 1);
			if (zoneToSet == -1)
			{
				break;
			}

			// Check we haven't set it already
			if (fSetWallZones.at(zoneToSet) == 0)
			{

				// And that we haven't already decided to set it this time around
				if (std::find(zonesToSet.begin(), zonesToSet.end(), zoneToSet) != zonesToSet.end())
				{
					std::cout << "You've already chosen zone " << zoneToSet << std::endl;
				}
				else
				{
					zonesToSet.push_back(zoneToSet);
					zonesChosen++;
					//Yibin hack. LR symmetry.
					if (bLRsymmetry && zoneToSet != 0 && 2 * zoneToSet != fNSides)
					{
						zonesToSet.push_back(fNSides - zoneToSet);
						zonesChosen++;
					}
				}
			}
			else
			{
				std::cout << "You've already set zone " << zoneToSet << std::endl;
			}

			// Break if there aren't any unset zones that we haven't decided to set
			bool doContinue = false;
			for (unsigned int iZone = 0; iZone < fSetWallZones.size(); ++iZone)
			{
				if (fSetWallZones.at(iZone) == 0)
				{
					if (std::find(zonesToSet.begin(), zonesToSet.end(), (int)iZone) == zonesToSet.end())
					{
						doContinue = true;
					}
				}
			}
			if (!doContinue)
			{
				break;
			}
			std::cout << "Set another?  Enter -1 for no, or the desired zone number" << std::endl;
		}

		// Put the zones in numerical order
		std::sort(zonesToSet.begin(), zonesToSet.end());

		rapidxml::xml_node<> *regionNode = fDoc.allocate_node(rapidxml::node_element, "region");
		std::cout << "Now let's describe the layout of zone" << ((zonesToSet.size() > 1) ? "s: " : ": ");
		for (unsigned int jZone = 0; jZone < zonesToSet.size(); ++jZone)
		{
			if (jZone == zonesToSet.size() - 1 && jZone > 0)
			{
				std::cout << "and ";
			}
			std::cout << zonesToSet.at(jZone) << " ";
		}
		std::cout << std::endl;

		if (fCoverageType == WCSimGeometryEnums::PhotodetectorLimit_t::kTotalNumber)
		{
			rapidxml::xml_node<> *coverageNode = fDoc.allocate_node(rapidxml::node_element, "coverage", "1");
			regionNode->append_node(coverageNode);
		}
		else if (fCoverageType == WCSimGeometryEnums::PhotodetectorLimit_t::kZonalCoverage)
		{
			const char *coverage = fDoc.allocate_string(GetCoverage().c_str());
			rapidxml::xml_node<> *coverageNode = fDoc.allocate_node(rapidxml::node_element, "coverage", coverage);
			regionNode->append_node(coverageNode);
		}
		else if (fCoverageType == WCSimGeometryEnums::PhotodetectorLimit_t::kPercentCoverage)
		{
			std::stringstream ss;
			ss << fPercentCoverage;
			const char *coverage = fDoc.allocate_string(ss.str().c_str());
			rapidxml::xml_node<> *coverageNode = fDoc.allocate_node(rapidxml::node_element, "coverage", coverage);
			regionNode->append_node(coverageNode);
		}

		std::stringstream ssRegion;
		ssRegion << region;
		const char *regionStr = fDoc.allocate_string(ssRegion.str().c_str());
		rapidxml::xml_node<> *locationNode = fDoc.allocate_node(rapidxml::node_element, "location", regionStr);
		regionNode->append_node(locationNode);

		for (unsigned int kZone = 0; kZone < zonesToSet.size(); ++kZone)
		{
			std::stringstream ss;
			ss << zonesToSet.at(kZone);
			const char *zoneStr = fDoc.allocate_string(ss.str().c_str());
			rapidxml::xml_node<> *zoneNode = fDoc.allocate_node(rapidxml::node_element, "zone", zoneStr);
			regionNode->append_node(zoneNode);
		}

		if (fCoverageType == WCSimGeometryEnums::PhotodetectorLimit_t::kTotalNumber)
		{
			std::vector<std::string> uniquePMTs;
			rapidxml::xml_node<> *unitCellNode = GetUnitCellNode(uniquePMTs);
			regionNode->append_node(unitCellNode);
			for (unsigned int i = 0; i < uniquePMTs.size(); ++i)
			{
				rapidxml::xml_node<> *pmtLimitNode = GetPMTLimitNode(uniquePMTs.at(i));
				regionNode->append_node(pmtLimitNode);
			}
		}
		else
		{
			rapidxml::xml_node<> *unitCellNode = GetUnitCellNode();
			regionNode->append_node(unitCellNode);
		}
		parentNode->append_node(regionNode);

		for (unsigned int lZone = 0; lZone < zonesToSet.size(); ++lZone)
		{
			fSetWallZones.at(zonesToSet.at(lZone)) = 1;
		}
	}
}

void WCSimGeometryHelper::SaveGeometry()
{
	std::ofstream file;
	std::cout << "Would you like to save this geometry to an xml file?" << std::endl;
	bool saveIt = AskYesNo();
	bool overwrite = false;
	if (saveIt)
	{
		bool goodFile = false;
		TString filename("");
		do
		{
			std::cout << "Enter name of file where the geometry should be saved (without the .xml):" << std::endl;
			filename = TString(AskString().c_str());
			filename.ReplaceAll(".xml", "");
			filename.Append(".xml");
			std::ifstream infile(filename.Data());
			if (!infile.good())
			{
				goodFile = true;
			}
			else
			{
				std::cout << "File exists.  Write anyway? " << std::endl;
				goodFile = AskYesNo();
				if (goodFile)
				{
					std::cout << "Would you like to overwrite or append to it?" << std::endl;
					std::vector<std::string> options;
					options.push_back(std::string("Overwrite it"));
					options.push_back(std::string("Append to it"));
					std::string choice = AskOptionString(options);
					if (choice == "Overwrite it")
					{
						overwrite = true;
					}
					else
					{
						overwrite = false;
					}
				}
			}
		} while (!goodFile);

		std::cout << "Saving geometry to " << filename << std::endl;

		if (overwrite)
		{
			file.open(filename.Data());
		}
		else
		{
			file.open(filename.Data(), std::ios::app);
		}

		if (file.is_open())
		{
			file << fDoc;
			file.close();
		}
	}
}

std::string WCSimGeometryHelper::GetName()
{
	std::cout << "Enter the name of your geometry" << std::endl;
	std::string name = AskString();
	return name;
}

std::string WCSimGeometryHelper::GetInnerRadius()
{
	std::cout << "Enter the inner radius (in metres) of your detector (centre to middle of a wall)" << std::endl;
	double radius = AskDouble();
	std::stringstream ss;
	ss << radius;
	return ss.str().c_str();
}

std::string WCSimGeometryHelper::GetInnerHeight()
{
	std::cout << "Enter the inner height (in metres) of your detector" << std::endl;
	double height = AskDouble();
	std::stringstream ss;
	ss << height;
	return ss.str().c_str();
}

std::string WCSimGeometryHelper::GetNSides()
{
	std::cout << "Enter the number of sides for your detector" << std::endl;
	int sides = AskInteger(3, 100);
	fNSides = sides;
	std::stringstream ss;
	ss << sides;
	return ss.str();
}

std::string WCSimGeometryHelper::GetCoverageType()
{
	std::cout << "How should the PMT coverage be specified?" << std::endl;
	std::vector<std::string> options = WCSimGeometryEnums::PhotodetectorLimit_t::GetAllTypeNames();
	std::string coverage = AskOptionString(options);
	fCoverageType = WCSimGeometryEnums::PhotodetectorLimit_t(coverage);
	return coverage;
}

std::string WCSimGeometryHelper::GetCoverage()
{
	std::cout << "Enter your desired coverage as a decimal" << std::endl;
	double coverageFrac = AskDouble(0, 1);
	if (fCoverageType == WCSimGeometryEnums::PhotodetectorLimit_t::kPercentCoverage)
	{
		fPercentCoverage = coverageFrac;
	}
	std::stringstream ss;
	ss << coverageFrac;
	return ss.str();
}

std::string WCSimGeometryHelper::GetVetoSize()
{
	std::cout << "How big should the veto volume be (in metres)?" << std::endl;
	double vetoSize = AskDouble(1, 100);
	std::stringstream ss;
	ss << vetoSize;
	return ss.str();
}

std::string WCSimGeometryHelper::AskString()
{
	std::ostringstream optionText;
	std::string choice;
	optionText << "\tEnter a string: ";

	bool gotOption = false;
	while (!gotOption)
	{
		std::cout << optionText.str() << "\t";
		std::string input;
		getline(std::cin, input);
		std::cout << std::endl;

		// This code converts from string to number safely.
		std::stringstream myStream(input);

		if (myStream >> choice)
		{
			gotOption = true;
		}
		else if (!gotOption)
		{
			std::cout << "\tInvalid entry, please try again" << std::endl;
		}
	}
	return choice;
}

bool WCSimGeometryHelper::ValidateBoundaries(std::vector<double> boundaries)
{
	bool isGood = true;

	double diffs = 0.0;
	for (unsigned int i = 0; i < boundaries.size() - 1; ++i)
	{
		double diff = boundaries.at((i + 1) % boundaries.size()) - boundaries.at(i);
		if (diff < 0)
		{
			std::cout << "Can't follow " << boundaries.at(i - 1) << " with " << boundaries.at(i)
					  << " - angles have to go in increasing order" << std::endl;
			isGood = false;
		}
		diffs += diff;
	}
	if (diffs == 2 * M_PI)
	{
		std::cout << "Angles add up to 360 degrees" << std::endl;
	}
	else if (359.0 * (2 * M_PI) / 360.0 < diffs && 361.0 / 360.0 * (2 * M_PI) > diffs)
	{
		std::cout << "Total angle for all zones is between 359 and 361 degrees" << std::endl
				  << "I'm going to allow this" << std::endl;
	}
	else
	{
		std::cout << "Total angle for all zones is more than 1 degree away from 360" << std::endl;
		isGood = false;
	}

	if (!isGood)
	{
		std::cout << "A problem was found with your zone angle definitions" << std::endl;
	}
	return isGood;
}

std::vector<std::pair<double, double>> WCSimGeometryHelper::GetZoneBoundaries()
{
	std::cout << "How many zones would you like to divide the region into?" << std::endl;
	int numZones = AskInteger(1, fNSides);
	std::cout << "Dividing the top into " << numZones << " zones" << std::endl;

	std::vector<std::pair<double, double>> allPairs;
	if (numZones == 1)
	{
		allPairs.push_back(std::make_pair(0.0, 2 * M_PI));
	}
	else
	{
		std::cout << "How would you like to specify the angles?" << std::endl;
		std::cout << "Note: 0 degrees/radians is the downstream end of the detector, relative to the beam" << std::endl;
		std::vector<std::string> options;
		options.push_back("degrees");
		options.push_back("radians");
		double conversionFactor = 1.0;
		if (AskOptionString(options) == "degrees")
		{
			conversionFactor = M_PI / 180.0;
		}
		std::vector<double> boundaries;
		do
		{
			boundaries.clear();
			for (int i = 0; i < numZones; ++i)
			{
				std::cout << "Enter the start of zone " << i << std::endl;
				boundaries.push_back(AskDouble() * conversionFactor);
			}
			boundaries.push_back(boundaries.at(0) + 360 * conversionFactor);
		} while (!ValidateBoundaries(boundaries));

		for (unsigned int i = 0; i < boundaries.size() - 1; ++i)
		{
			allPairs.push_back(std::make_pair(boundaries.at(i), boundaries.at((i + 1) % boundaries.size())));
		}
	}
	return allPairs;
}

rapidxml::xml_node<> *WCSimGeometryHelper::GetUnitCellNode(std::vector<std::string> &uniquePMTs)
{
	rapidxml::xml_node<> *cellNode = fDoc.allocate_node(rapidxml::node_element, "cellPMTDef");
	std::cout << "Now let's specify the unit cell for this zone:" << std::endl;
	std::cout << "How many PMTs would you like to put in this cell?" << std::endl;
	int numPMTs = AskPositiveInteger();

	uniquePMTs.clear();

	for (int i = 1; i <= numPMTs; ++i)
	{
		rapidxml::xml_node<> *pmtNode = fDoc.allocate_node(rapidxml::node_element, "PMT");

		std::cout << "Enter the name of PMT number " << i << std::endl;
		const char *name = fDoc.allocate_string(AskString().c_str());
		rapidxml::xml_node<> *nameNode = fDoc.allocate_node(rapidxml::node_element, "name", name);

		if (std::find(uniquePMTs.begin(), uniquePMTs.end(), std::string(name)) == uniquePMTs.end())
		{
			uniquePMTs.push_back(name);
		}

		std::cout << "Enter the PMT x-coordinate within the cell" << std::endl;
		const char *pmtX = fDoc.allocate_string(AskDoubleStr(0, 1.0).c_str());
		rapidxml::xml_node<> *xNode = fDoc.allocate_node(rapidxml::node_element, "posX", pmtX);

		std::cout << "Enter the PMT y-coordinate within the cell" << std::endl;
		const char *pmtY = fDoc.allocate_string(AskDoubleStr(0, 1.0).c_str());
		rapidxml::xml_node<> *yNode = fDoc.allocate_node(rapidxml::node_element, "posY", pmtY);

		std::cout << "In what direction would you like this PMT to face?" << std::endl;
		std::vector<std::string> options = WCSimGeometryEnums::PMTDirection_t::GetAllTypeNames();
		const char *faceType = fDoc.allocate_string(AskOptionString(options).c_str());
		rapidxml::xml_node<> *faceNode = fDoc.allocate_node(rapidxml::node_element, "face");
		rapidxml::xml_node<> *faceTypeNode = fDoc.allocate_node(rapidxml::node_element, "type", faceType);

		pmtNode->append_node(nameNode);
		pmtNode->append_node(xNode);
		pmtNode->append_node(yNode);
		faceNode->append_node(faceTypeNode);
		pmtNode->append_node(faceNode);
		cellNode->append_node(pmtNode);
	}

	return cellNode;
}

rapidxml::xml_node<> *WCSimGeometryHelper::GetUnitCellNode()
{
	std::vector<std::string> throwAway;
	return GetUnitCellNode(throwAway);
}

rapidxml::xml_node<> *WCSimGeometryHelper::GetPMTLimitNode(const std::string &name)
{
	std::cout << "How many PMTs of type " << name << " are allowed in this zone?" << std::endl;
	int limit = AskPositiveInteger();
	std::stringstream ss;
	ss << limit;
	const char *limitStr = fDoc.allocate_string(ss.str().c_str());
	rapidxml::xml_node<> *pmtLimitNode = fDoc.allocate_node(rapidxml::node_element, "pmtLimit");
	rapidxml::xml_node<> *nameNode = fDoc.allocate_node(rapidxml::node_element, "name", name.c_str());
	rapidxml::xml_node<> *limitNode = fDoc.allocate_node(rapidxml::node_element, "limit", limitStr);

	pmtLimitNode->append_node(nameNode);
	pmtLimitNode->append_node(limitNode);

	return pmtLimitNode;
}

std::string WCSimGeometryHelper::AskIntegerStr(int min, int max)
{
	int myInt = AskInteger(min, max);
	std::stringstream ss;
	ss << myInt;
	std::string myString = ss.str();
	return myString;
}

std::string WCSimGeometryHelper::AskPositiveIntegerStr(unsigned int min, unsigned int max)
{
	int myInt = AskPositiveInteger(min, max);
	std::stringstream ss;
	ss << myInt;
	std::string myString = ss.str();
	return myString;
}

std::string WCSimGeometryHelper::AskDoubleStr(double min, double max)
{
	double myDouble = AskDouble(min, max);
	std::stringstream ss;
	ss << myDouble;
	std::string myString = ss.str();
	return myString;
}

std::string WCSimGeometryHelper::AskOptionString(std::vector<std::string> options)
{
	std::ostringstream optionText;
	optionText << "\tEnter:" << std::endl;
	int choice = options.size() + 1;

	for (unsigned int i = 0; i < options.size(); ++i)
	{
		optionText << i << " for " << options.at(i) << std::endl;
	}

	bool gotOption = false;
	while (!gotOption && options.size() != 0)
	{
		std::cout << optionText.str() << "\n";
		std::string input;
		getline(std::cin, input);
		std::cout << std::endl;

		// This code converts from string to number safely.
		std::stringstream myStream(input);
		if (myStream >> choice)
		{
			if (choice >= 0 && choice < (int)options.size())
			{
				gotOption = true;
			}
		}
		else if (!gotOption)
		{
			std::cout << "\tInvalid entry, please try again" << std::endl;
		}
	}
	std::string toReturn = options.at(choice);
	return toReturn;
}
