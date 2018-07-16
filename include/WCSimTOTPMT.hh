#include <iostream>
#include "TRandom3.h"

class WCSimTOTPMT {

	public:
		// Constructors / Destructor
		WCSimTOTPMT();
		~WCSimTOTPMT();

		// Calculate the charge from pe photoelectrons on the cathode
		double CalculateCharge(double totalPe, std::string PMTName);

	private:
		TRandom3 fRand;

		double fUpperBoundMadison;
		double fLambdaMadison;
		double fMultiplierMadison;

		double fUpperBoundNikhef;
		double fLambdaNikhef;
		double fMultiplierNikhef;

};
