#include <iostream>
#include "TRandom3.h"

class WCSimSK1pePMT{

	public:
		// Constructors / Destructor
		WCSimSK1pePMT();
		~WCSimSK1pePMT();

		// Calculate the charge from pe photoelectrons on the cathode
		// with time spread end - start.
		double CalculateCharge(int pe);

	private:
		double rn1pe();
		double Threshold(double pe);

		TRandom3 fRand;
};
