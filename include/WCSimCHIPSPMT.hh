#include <iostream>
#include "TRandom3.h"

class WCSimCHIPSPMT
{

public:
	// Constructors / Destructor
	WCSimCHIPSPMT();
	WCSimCHIPSPMT(const WCSimCHIPSPMT &rhs);
	~WCSimCHIPSPMT();

	// Calculate the charge from pe photoelectrons on the cathode
	// with time spread end - start.
	double CalculateCharge(int pe, double start, double end);

	// Getter functions
	double GetTotalGain() const
	{
		return fTotalGain;
	}
	double GetDynodeGain(int dynode) const;
	int GetDynodeStages() const
	{
		return fDynodeStages;
	}
	double GetDynodeGainMod(int dynode) const
	{
		return fDynodeGainMod[dynode];
	}
	double GetDamping() const
	{
		return fDamping;
	}
	double GetProbCathodeSkip() const
	{
		return fProbCathodeSkip;
	}
	double GetProbDynode1Skip() const
	{
		return fProbDynode1Skip;
	}

private:
	// Calculate the damping term due to non linearity
	double CalculateDamping(double peRate);

	// Calculate the parameters used in the non linearity function;
	double GetNonLinFuncAlpha() const;
	double GetNonLinFuncBeta() const;

	double GetChargeRandom(double pe);

	double fTotalGain;
	double fDamping;
	double fDynodeGain;
	std::vector<double> fDynodeGainMod;
	int fDynodeStages;

	TRandom3 fRand;

	double fNonLinAlpha;
	double fNonLinBeta;

	// These are the probabilities for sub-dominant processes:
	// The probability the photon passes through the photo-cathode and
	// liberates an electron on dynode 1.
	double fProbCathodeSkip;
	// The probability the photo-electron on the cathode passes
	// straight to dynode 2.
	double fProbDynode1Skip;
};
