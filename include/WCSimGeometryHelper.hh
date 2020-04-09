/*
 * WCSimGeometryHelper.hh
 *
 *  Created on: 3 Dec 2014
 *      Author: ajperch
 */

#ifndef WCSIMGEOMETRYHELPER_HH_
#define WCSIMGEOMETRYHELPER_HH_

#include "rapidxml-1.13/rapidxml.hpp"
#include "WCSimGeometryEnums.hh"
#include <string>
#include <vector>

class WCSimGeometryHelper
{
public:
	WCSimGeometryHelper();
	virtual ~WCSimGeometryHelper();

	/*
		 * WCSimGeometryHelper.cc
		 *
		 *  Created on: 3 Dec 2014
		 *      Author: ajperch
		 */

	void Run();

private:
	void Usage();

	bool MakeGeometry();
	bool AssumeLRsymmetry();
	bool AssumeTBsymmetry();

	std::string GetName();
	std::string GetInnerRadius();
	std::string GetInnerHeight();
	std::string GetNSides();
	std::string GetCoverageType();
	std::string GetCoverage();
	std::string GetVetoSize();

	void SetCapZones(bool isTop, rapidxml::xml_node<> *parentNode);
	void SetTopZones(rapidxml::xml_node<> *parentNode);
	std::vector<std::pair<double, double>> GetZoneBoundaries();
	void SetBottomZones(rapidxml::xml_node<> *parentNode);
	void SetWallZones(rapidxml::xml_node<> *parentNode);
	void SaveGeometry();

	bool ValidateBoundaries(std::vector<double> boundaries);

	rapidxml::xml_node<> *GetUnitCellNode(std::vector<std::string> &uniquePMTs);
	rapidxml::xml_node<> *GetPMTLimitNode(const std::string &uniquePMTs);
	rapidxml::xml_node<> *GetUnitCellNode();

	bool AskYesNo();
	int AskInteger(int min = -999, int max = -999);
	int AskPositiveInteger(unsigned int min = 0, unsigned int max = 0);
	double AskDouble(double min = -999.9, double max = -999.9);
	std::string AskString();

	std::string AskPositiveIntegerStr(unsigned int min = -999, unsigned int max = -999);
	std::string AskIntegerStr(int min = -999, int max = -999);
	std::string AskDoubleStr(double min = -999, double max = -999);

	std::string AskOptionString(std::vector<std::string> options);
	std::vector<int> fSetWallZones;

	rapidxml::xml_document<> fDoc;
	WCSimGeometryEnums::PhotodetectorLimit_t fCoverageType;
	int fNSides;
	bool bLRsymmetry, bTBsymmetry;
	double fPercentCoverage;

	std::vector<std::string *> fAllNewStrings;
};

#endif /* WCSIMGEOMETRYHELPER_HH_ */
