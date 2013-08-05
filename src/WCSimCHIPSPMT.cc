// C++ headers
#include <iostream>
// GEANT headers
#include "globals.hh"
// Root headers
#include "TRandom.h"
// WCSim headers
#include "WCSimCHIPSPMT.hh"


WCSimCHIPSPMT::WCSimCHIPSPMT(){
	// Total gain of the PMT
	fTotalGain = 1e7;

	// Relative gains of the dynodes relative to the first one.
	fDynodeGainMod.push_back(1.000);
	fDynodeGainMod.push_back(0.236);
	fDynodeGainMod.push_back(0.293);
	fDynodeGainMod.push_back(0.194);
	fDynodeGainMod.push_back(0.097);
	fDynodeGainMod.push_back(0.059);
	fDynodeGainMod.push_back(0.070);
	fDynodeGainMod.push_back(0.088);
	fDynodeGainMod.push_back(0.130);
	fDynodeGainMod.push_back(0.177);

	// Get the number of dynode stages
	fDynodeStages = fDynodeGainMod.size();

	// Calculate the absolute gain of the first dynode from total gain G
	// and the relative gains r_i
	// Total gain G = g^10 * (r_i*r_2...r_n)
	double multisum = 1;
	for(int i = 1; i < fDynodeStages; ++i){
			multisum *= fDynodeGainMod[i];
	}
	// Now divide the total gain by this number, and take the tenth root
	// Remember roughly that gain is proportional to V^0.7, so need to raise 
  // multisum to the power 0.7.
	fDynodeGain = pow(fTotalGain/pow(multisum,0.7),0.1);

	fNonLinAlpha = GetNonLinFuncAlpha(fTotalGain/1e6);
	fNonLinBeta = GetNonLinFuncBeta(fTotalGain/1e6);

//	std::cout << fNonLinAlpha << ", " << fNonLinBeta << std::endl;

	fDamping = 1.;

	fProbCathodeSkip = 0.0;
	fProbDynode1Skip = 0.0;
}

WCSimCHIPSPMT::WCSimCHIPSPMT(const WCSimCHIPSPMT& rhs){
	fDynodeStages = rhs.GetDynodeStages();
	fTotalGain = rhs.GetTotalGain();
	fDamping = 0.; // So we get the right gain, update later.
	fDynodeGain = rhs.GetDynodeGain(0);
	for(int i = 0; i < 10; ++i){
		fDynodeGainMod[i] = rhs.GetDynodeGainMod(i);
	}
	fNonLinAlpha = rhs.GetNonLinFuncAlpha(fTotalGain/1e6);
	fNonLinBeta = rhs.GetNonLinFuncBeta(fTotalGain/1e6);
	
	fDamping = rhs.GetDamping();
}

WCSimCHIPSPMT::~WCSimCHIPSPMT(){

}

double WCSimCHIPSPMT::CalculateCharge(int pe, double start, double end){

	double currentPE = pe;
	double timeSpread = end - start;
	double rate = 0.0; // This is pe / ns
	if(end != start) rate = pe / timeSpread;
//	std::cout << "pe rate = " << rate << std::endl;

	// Calculate the damping term
	fDamping = CalculateDamping(rate);

	// Take care of the two special cases: 
	// 1) Photon liberates electron on dynode 1
	// 2) Electron misses the first dynode.
	double nSkipCathode = 0.;
	double nSkipDynode1 = 0.;
	for(int i = 0; i < pe; ++i){
		if(fRand.Rndm() < fProbCathodeSkip) ++nSkipCathode;
		else if(fRand.Rndm() < fProbDynode1Skip) ++nSkipDynode1;
	}
	currentPE -= nSkipCathode; // We will treat these separately.
	currentPE -= nSkipDynode1; // We will treat these separately.

	// Loop over the dynodes, calculating the pe as we go
	for(int d = 0; d < fDynodeStages; ++d){
//		std::cout << "  Current PE: " << currentPE << std::endl;
		currentPE = GetChargeRandom(currentPE*GetDynodeGain(d));
		// Small, but non-zero chance of getting a zero (~0.7% for pe = 5)
	}

	// If we have any photons that hit the first dynode, deal with those here.
	// Remember to skip the first dynode multiplication stage.
	if(nSkipCathode != 0.){
		for(int d = 1; d < fDynodeStages; ++d){
			nSkipCathode = GetChargeRandom(nSkipCathode*GetDynodeGain(d));
		}
	}	

	// Now deal with the electrons passing straight to dynode 2. Let's do this 
	// by skipping dynode 1, treating dynode 2 separately, then doing things
	// as usual from dynode 3.
	if(nSkipDynode1 != 0.){
		nSkipDynode1 = GetChargeRandom(nSkipDynode1*(GetDynodeGain(0)+GetDynodeGain(1)));
		for(int d = 2; d < fDynodeStages; ++d){
			nSkipDynode1 = GetChargeRandom(nSkipDynode1*GetDynodeGain(d));	
		}
	}

	// Add up the contributions from the 3 cases
	currentPE += nSkipCathode;
	currentPE += nSkipDynode1;

	// Now we scale the charge by the gain in order to get back to sensible units.
	currentPE /= fTotalGain;
	return currentPE;
}

double WCSimCHIPSPMT::GetChargeRandom(double pe){

	// Use TRandom to give us our random number.
	double poisRand = 0;

	// Use Poisson for low pe, and Gaussian for higher values.
	if(pe < 50.) poisRand = fRand.Poisson(pe);
	else poisRand = fRand.Gaus(pe,sqrt(pe));

	return poisRand;
}

// Dynode Gain depends on two things:
// 1) Which dynode is it?
// 2) Is the photocathode rate large enough to cause non-linearity?
double WCSimCHIPSPMT::GetDynodeGain(int dynode) const{
	
	// Used the electonrics schematic in the icecube paper to look at the individual gains for the dynodes. Decided to set them all relative to dynode one.
	double gain = fDynodeGain * pow(fDynodeGainMod[dynode],0.7) * fDamping;

	return gain;
}

double WCSimCHIPSPMT::CalculateDamping(double peRate){

	// This value is used to damp the gain due to pile up of electrons.
	double nonLinearDamp = 1.;

//	peRate = 0.; // Delete this once I fix the next part!

	if(peRate != 0.0){
		// Convert the pe rate into a current, since we have a current based
		// parametrisation from IceCube. Unfortunatly this gives us the ideal 
		// current as a function of measured current, and not the other way
		// round. Hence had to plot the function and fit it with two params.
		double initialCurrent = (50./31.)*peRate;
	
		// Calculate what the non-linearity curve predicts for the final current.
		double finalCurrent = 0.;
		double A = fNonLinAlpha;
		double B = fNonLinBeta;
		// This actually returns the ln of the finalCurrent, so remember to correct for that.
		if(initialCurrent < A) finalCurrent = initialCurrent;
		else{
			finalCurrent = A + (initialCurrent-A)/(1+B*(initialCurrent-A));
		}
	
//		std::cout << "Initial / Final Currents: " << initialCurrent << ", " << finalCurrent << std::endl;
	
		// Finally, we take the ratio between finalCurrent and initialCurrent to be the factor to reduce the gain by, and take the 10th root.
		nonLinearDamp = pow(finalCurrent / initialCurrent,0.1);
	}

	return nonLinearDamp;

}

// These calculate the 3 parameters in the non-linearity function as defined
// in the IceCube PMT paper.
double WCSimCHIPSPMT::GetNonLinFuncAlpha(double gamma) const{

	// I guess this should eventually be a function of the gain?
	double alpha = 16.68;
	return alpha;
}

double WCSimCHIPSPMT::GetNonLinFuncBeta(double gamma) const{

	double beta = 0.009571;
	return beta;
}


