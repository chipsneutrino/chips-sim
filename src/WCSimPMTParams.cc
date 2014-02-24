#include <iostream>

#include "WCSimPMTParams.hh"

const unsigned int WCSimPMTParams::fNBins = 20;

WCSimPMTParams::WCSimPMTParams(){

//	fRadius = 0;
//	fExposeHeight = 0;
	fCurrPMTType = -1;
	this->InitialisePMTTypes();
	this->InitialisePMTGeom();
	this->InitialiseEfficiencies();

}

WCSimPMTParams::WCSimPMTParams(std::string type){

//	fRadius = 0;
//	fExposeHeight = 0;
	fCurrPMTType = -1;
	this->InitialisePMTTypes();
	this->InitialisePMTGeom();
	this->InitialiseEfficiencies();
	this->SetPMTType(type);

}

WCSimPMTParams::~WCSimPMTParams(){

}

void WCSimPMTParams::SetPMTType(std::string type){

	fPMTType = type;

	// Is the PMT a recognised type?
	bool found = false;
	for(unsigned int i = 0; i < fPMTTypeList.size(); ++i){
		if(fPMTType == fPMTTypeList[i]){
			found = true;
			fCurrPMTType = i;
			break;
		} 
	}
	if(!found){
		std::cout << "WARNING: PMT type not recognised." << std::endl;
		fPMTType = "";
		fCurrPMTType = -1;
	}
}

double WCSimPMTParams::GetRadius(){
	if(fCurrPMTType != -1 && fCurrPMTType <= fRadii.size()){
		return fRadii[fCurrPMTType];
	}
	else{
		std::cerr << "ERROR - WCSimPMTParams::GetRadius: PMT Type not set. Returning 0." << std::endl;
		return 0.0;
	}
}

double WCSimPMTParams::GetExposeHeight(){
	if(fCurrPMTType != -1 && fCurrPMTType <= fExposeHeights.size()){
		return fExposeHeights[fCurrPMTType];
	}
	else{
		std::cerr << "ERROR - WCSimPMTParams::GetExposeHeight: PMT Type not set. Returning 0." << std::endl;
		return 0.0;
	}

}

double WCSimPMTParams::GetGlassThickness(){
	if(fCurrPMTType != -1 && fCurrPMTType <= fGlassThickness.size()){
		return fGlassThickness[fCurrPMTType];
	}
	else{
		std::cerr << "ERROR - WCSimPMTParams::GetGlassThickness: PMT Type not set. Returning 0." << std::endl;
		return 0.0;
	}

}

void WCSimPMTParams::GetEfficiency(G4float *eff, const unsigned int nBins){

	// Since GEANT likes arrays and we are internally using vectors, need
	// a quick loop to sort things out.
	if(nBins == fNBins){
		if(fCurrPMTType != -1){
			for(unsigned int i = 0; i < fNBins; ++i){
				eff[i] = fEfficiencies[fCurrPMTType][i];
			}
		}
		else std::cout << "WARNING: PMT type not set" << std::endl;
	}
	else std::cout << "WARNING: Array size not " << fNBins << std::endl;
}

double WCSimPMTParams::GetMaxEfficiency(){
	if(fCurrPMTType != -1 && fCurrPMTType <= fMaxQEs.size()){
		return fMaxQEs[fCurrPMTType];
	}
	else{
		std::cerr << "ERROR - WCSimPMTParams::GetMaxEfficiency: PMT Type not set. Returning 0." << std::endl;
		return 0.0;
	}
}

// Return PMT type as a string
std::string WCSimPMTParams::GetPMTType(){
	if(fCurrPMTType != -1 && fCurrPMTType <= fPMTTypeList.size()){
		return fPMTTypeList[fCurrPMTType];	
	}
	else{
		std::cerr << "ERROR - WCSimPMTParams::GetPMTType: PMT Type not set. Returning empty string" << std::endl;
		return "";
	}
}

// Initialise the PMT radii and exposed heights
void WCSimPMTParams::InitialisePMTGeom(){

	// Conversion between inches and metres
	double i2m = 0.0254;

	double rad = 0.0;

	// 20 inch PMT from SuperK
	rad = 10.0 * i2m * m;
	fRadii.push_back(rad);
	fExposeHeights.push_back(0.18*m);
	fGlassThickness.push_back(0.4*cm);

	// 10 inch PMT
	rad = 5.0 * i2m * m;
	fRadii.push_back(rad);
	fExposeHeights.push_back(rad-0.01*m);
	fGlassThickness.push_back(0.55*cm);

	// 10 inch HQE PMT
	rad = 5.0 * i2m * m;
	fRadii.push_back(rad);
	fExposeHeights.push_back(rad-0.01*m);
	fGlassThickness.push_back(0.55*cm);

	// 12 inch HQE PMT
	rad = 6.0 * i2m * m;
	fRadii.push_back(rad);
	fExposeHeights.push_back(0.118*m);
	fGlassThickness.push_back(0.55*cm);

}

// Initialise the efficiencies. Add one for each type of PMT.
void WCSimPMTParams::InitialiseEfficiencies(){

	// Stored in this order: 20 inch, 10 inch, 10 inch HQE
	std::vector<double> effVec0, effVec1, effVec2;

	// 20 inch PMTs
	double tempEff0[fNBins] = { 0.00, .0139, .0854, .169, .203, .206, .211, .202, .188, .167, 
      .140,  .116, .0806, .0432, .0265, .0146, .00756, .00508, .00158, 0.00};
  fMaxQEs.push_back(0.211);
  
	// 10 inch PMTs
  double tempEff1[fNBins] = { 0.00, .0375, .13, .195, .22, .23, .24, .24, .225, .205,
      .18, .16, .14, .085, .065, .05, .035, .02, .005, 0.0};
  fMaxQEs.push_back(0.24);
  
	// 10 inch HQE PMTs
  double tempEff2[fNBins]  = { 0.00, .0502, .2017, .2933, .3306, .3396, .3320, .3168, .2915, .2655, 
      .2268,  .1971, .1641, .1102, .0727, .0499, .0323, .0178, .0061, 0.00};
	fMaxQEs.push_back(0.3396);

	for(unsigned int i = 0; i < fNBins; ++i){
		effVec0.push_back(tempEff0[i]);
		effVec1.push_back(tempEff1[i]);
		effVec2.push_back(tempEff2[i]);
	}

	// Push them back into the efficiencies vector
	fEfficiencies.push_back(effVec0);
	fEfficiencies.push_back(effVec1);
	fEfficiencies.push_back(effVec2);
	fEfficiencies.push_back(effVec2); // Push this back twice since the 12 inch tube uses the same as the 10 inch one...
}

// The list of currently recognised PMTs
void WCSimPMTParams::InitialisePMTTypes(){
	fPMTTypeList.push_back("20inch");
	fPMTTypeList.push_back("10inch");
	fPMTTypeList.push_back("10inchHQE");
	fPMTTypeList.push_back("12inchHQE");
}
