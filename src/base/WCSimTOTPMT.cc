// C++ headers
#include <iostream>
// GEANT headers
#include "globals.hh"
// Root headers
#include "TRandom3.h"
#include "TMath.h"
#include <string>
// WCSim headers
#include "WCSimTOTPMT.hh"

WCSimTOTPMT::WCSimTOTPMT()
{
	fRand = TRandom3(0);

	fUpperBoundMadison = 19.462568;
	fLambdaMadison = 0.37098;
	fMultiplierMadison = 7.09985;

	fUpperBoundNikhef = 19.462568;
	fLambdaNikhef = 0.37098;
	fMultiplierNikhef = 7.09985;
}

WCSimTOTPMT::~WCSimTOTPMT()
{
	//Empty
}

double WCSimTOTPMT::CalculateCharge(double totalPe, std::string PMTName)
{

	//std::cout << PMTName.compare("88mm") << std::endl;

	double sigma, mean, tot, peSmeared;
	if (PMTName.compare("88mm") == 0 || PMTName.compare("88mm_LC_v2") == 0)
	{
		sigma = TMath::Sqrt(totalPe);
		mean = fUpperBoundNikhef - (fMultiplierNikhef * TMath::Exp(-1. * fLambdaNikhef * totalPe));
		tot = fRand.Gaus(mean, sigma);
		peSmeared = (-1) * TMath::Log(((-1) * tot + fUpperBoundNikhef) / fMultiplierNikhef) / fLambdaNikhef;
	}
	else if (PMTName.compare("R6091") == 0 || PMTName.compare("R6091_LC_v1") == 0)
	{
		sigma = TMath::Sqrt(totalPe);
		mean = fUpperBoundMadison - (fMultiplierMadison * TMath::Exp(-1. * fLambdaMadison * totalPe));
		tot = fRand.Gaus(mean, sigma);
		peSmeared = (-1) * TMath::Log(((-1) * tot + fUpperBoundMadison) / fMultiplierMadison) / fLambdaMadison;
	}
	else
	{
		std::cout << "PMT is not currently implemented for the TOT, setting zero." << std::endl;
		peSmeared = 0.0;
	}

	//std::cout << "totalPe -> " << totalPe << ", Mean -> " << mean << ", sigma -> " << sigma << ", tot -> " << tot << ", peSmeared -> " << peSmeared << std::endl;
	return peSmeared;
}
