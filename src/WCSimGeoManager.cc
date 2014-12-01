#include <fstream>
#include <sstream>

// Use the rapidXML parser
#include <rapidxml-1.13/rapidxml.hpp>
#include <rapidxml-1.13/rapidxml_utils.hpp>

#include "G4ThreeVector.hh"

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

		int i=0;
		for(rapidxml::xml_node<> *childNode = curNode->first_node("region"); childNode; childNode = childNode->next_sibling("region") )
		{
			geo.ResetCurrent();
			std::cout << "Filling region number " << ++i << std::endl;
			this->FillRegion(geo, childNode);
		}

		for(rapidxml::xml_node<> *childNode = curNode->first_node("PMTLimit"); childNode; childNode = childNode->next_sibling("PMTLimit"))
		{
			this->FillPMTLimit(geo, childNode);
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
		geo.SetOuterRadius(tempVal*m);
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
  else if(name == "overallCoverage"){
    double tempVal;
    ss >> tempVal;
    geo.SetOverallCoverage(tempVal);
  }
  else if(name == "limitPMTNumbers"){
  	std::string tempVal;
  	ss>> tempVal;
  	if(MeansYes(tempval)) { geo.SetLimitPMTNumbers(true); }
  	else if(MeansNo(tempVal)) { geo.SetLimitPMTNumbers(false); }
  }
	else{
		std::cerr << "WCSimGeoManager::FillGeoAttribute: Unexpected parameter " << attr->name() << ", " << attr->value() << std::endl;
	}

}

void WCSimGeoManager::FillRegion(WCSimGeoConfig& geo,
		rapidxml::xml_node<>* node)
{

	for(rapidxml::xml_node<> *childNode = node->first_node("location"); childNode; childNode = childNode->next_sibling("location") )
	{
		std::string nodeVal = childNode->value();
		std::cout << "Adding location " << nodeVal << std::endl;
		geo.AddCurrentRegion(nodeVal);
	}

	for(rapidxml::xml_node<> *childNode = node->first_node("zone"); childNode; childNode = childNode->next_sibling("zone") )
	{
			std::stringstream ss;
			ss << childNode->value();
			int zoneNum;
			ss >> zoneNum;

			std::cout << "Adding location " << zoneNum << std::endl;
			geo.AddCurrentZone(zoneNum);
	}

	for(rapidxml::xml_node<> *childNode = node->first_node("cellPMTDef"); childNode; childNode = childNode->next_sibling("cellPMTDef") ){
		FillCell(geo, childNode);
	}

	for(rapidxml::xml_node<> *childNode = node->first_node("pmtLimit"); childNode; childNode = childNode->next_sibling("pmtLimit"))
	{
		FillPMTLimit(geo, childNode);
	}

	for(rapidxml::xml_node<> *childNode = node->first_node("coverage") ; childNode; childNode = childNode->next_sibling("coverage"))
	{
		std::stringstream ss;
		ss << childNode->value();
		double coverage;
		ss >> coverage;
		geo.SetZoneCoverage(coverage);
	}

}

void WCSimGeoManager::FillCell(WCSimGeoConfig& geo, rapidxml::xml_node<>* node)
{
	rapidxml::xml_node<> *nameNode = node->first_node("name");
	rapidxml::xml_node<> *xPosNode = node->first_node("posX");
	rapidxml::xml_node<> *yPosNode = node->first_node("posY");
	rapidxml::xml_node<> *faceNode = node->first_node("face");

	assert(nameNode != NULL && xPosNode != NULL && yPosNode != NULL && faceNode != NULL);

	if(nameNode)
	{
		std::string nodeVal = nameNode->value();
		geo.AddCellPMTName(nodeVal);
	}

	if(xPosNode)
	{
		std::stringstream ss;
		ss << xPosNode->value();
		double posX = 0.0;
		ss >> posX;
		geo.AddCellPMTX(posX*m);
	}

	if( yPosNode )
	{
		std::stringstream ss;
		ss << yPosNode->value();
		ss << yPosNode->value();
		double posY = 0.0;
		ss >> posY;
		geo.AddCellPMTY(posY*m);
	}

	if( faceNode )
	{
		rapidxml::xml_node<> *typeNode = faceNode->first_node("type");
		rapidxml::xml_node<> *thetaNode = faceNode->first_node("theta");
		rapidxml::xml_node<> *phiNode = faceNode->first_node("phi");
		assert( (typeNode != NULL) );
		assert( (thetaNode != NULL && phiNode != NULL) || geo.CanBuildWithoutAngles(std::string(typeNode->value())) );

		if( typeNode )
		{
			std::string faceType = typeNode->value();
			geo.AddCellPMTFaceType(faceType);

			double faceTh = -999.9;
			double facePhi = -999.9;
			if( thetaNode != NULL && phiNode != NULL )
			{
				{
					std::stringstream ss;
					ss << thetaNode->value();
					ss >> faceTh;

					ss.clear();
					ss.str( std::string() );
					ss << phiNode->value();
					ss >> facePhi;
				}
			}
			geo.AddCellPMTFaceTheta(faceTh);
			geo.AddCellPMTFacePhi(facePhi);
		}
	}
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
    std::cerr << "Available geometries are:" << std::endl;
    for( unsigned int iVec = 0; iVec < fGeoVector.size(); ++iVec)
    {
      std::cerr << fGeoVector.at(iVec).GetGeoName() << std::endl;
    }
	}

	return curGeo;		
}

bool WCSimGeoManager::GeometryExists( std::string name ) const {
  bool foundIt = false;
	for(unsigned int g = 0; g < fGeoVector.size(); ++g){
		if(name == fGeoVector[g].GetGeoName()){
			foundIt = true;
			break;
		}
	}
  return foundIt;
}

bool WCSimGeoManager::MeansYes(std::string str)
{
	return ((str == "YES") || (str == "Y") || (str == "yes") || (str == "y") || (str == "TRUE") || (str == "true"));
}

bool WCSimGeoManager::MeansNo(std::string str)
{
	return ((str == "NO") || (str == "N") || (str == "no") || (str == "n") || (str == "FALSE") || (str == "false"));
}

void WCSimGeoManager::FillPMTLimit(WCSimGeoConfig& geo,
		rapidxml::xml_node<>* node)
{
	rapidxml::xml_node<> *nameNode = node->first_node("name");
	rapidxml::xml_node<> *limitNode = node->first_node("zoneLimit");
	assert(nameNode != NULL && limitNode != NULL);

	if(nameNode && limitNode)
	{
		std::string pmtName = nameNode->value();
		std::stringstream ss;
		ss << limitNode->value();
		int limit = 0;
		ss >> limit;
		assert(limit > 0);
		geo.SetPMTLimit(pmtName, limit);
	}
}
