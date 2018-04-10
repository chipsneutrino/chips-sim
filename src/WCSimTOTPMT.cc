// C++ headers
#include <iostream>
// GEANT headers
#include "globals.hh"
// Root headers
#include "TRandom3.h"
#include "TMath.h"
// WCSim headers
#include "WCSimTOTPMT.hh"

WCSimTOTPMT::WCSimTOTPMT(){
	fRand = TRandom3(0);

	fUpperBoundMadison = 19.462568;
	fLambdaMadison = 0.37098;
	fMultiplierMadison = 7.09985;

	fUpperBoundNikhef = 19.462568;
	fLambdaNikhef = 0.37098;
	fMultiplierNikhef = 7.09985;
}

WCSimTOTPMT::~WCSimTOTPMT(){
	//Empty
}

double WCSimTOTPMT::CalculateCharge(int totalPe, std::string PMTName){

    double sigma, mean, tot, peSmeared;
    if(PMTName == "88mm" || PMTName == "88mm_LC_v2"){
        sigma = TMath::Sqrt(totalPe);
        mean = fUpperBoundNikhef - (fMultiplierNikhef * TMath::Exp(-1. * fLambdaNikhef * totalPe));
        tot = fRand.Gaus(mean,sigma);
    	peSmeared = (-1)*TMath::Log(((-1)*tot + fUpperBoundNikhef)/fMultiplierNikhef)/fLambdaNikhef;
    }
    if(PMTName == "R6091" || PMTName == "R6091_LC_v1"){
        sigma = TMath::Sqrt(totalPe);
        mean = fUpperBoundMadison - (fMultiplierMadison * TMath::Exp(-1. * fLambdaMadison * totalPe));
        tot = fRand.Gaus(mean,sigma);
    	peSmeared = (-1)*TMath::Log(((-1)*tot + fUpperBoundMadison)/fMultiplierMadison)/fLambdaMadison;
    }
    else{
    	std::cout << "WTF!!!" << std::endl;
    	peSmeared = 0.0;
    }

	std::cout << "totalPe -> " << totalPe << ", Mean -> " << mean << ", sigma -> " << sigma << ", tot -> " << tot << ", peSmeared -> " << peSmeared << std::endl;
	return peSmeared;
}
