// Tuning parameters

#include "WCSimTuningParameters.hh"
#include "WCSimTuningMessenger.hh"
#include <cassert>

static WCSimTuningParameters *fgWCSimTuningParameters = NULL;

WCSimTuningParameters *WCSimTuningParameters::Instance()
{
	if (fgWCSimTuningParameters == NULL)
	{
		fgWCSimTuningParameters = new WCSimTuningParameters();
	}
	if (fgWCSimTuningParameters == NULL)
	{
		assert(fgWCSimTuningParameters != NULL);
	}

	return fgWCSimTuningParameters;
}

WCSimTuningParameters::WCSimTuningParameters()
{

	TuningMessenger = new WCSimTuningMessenger();
	// Default values
	rayff = 0.625;
	bsrff = 1.0;
	abwff = 1.0;
	rgcff = 0.0;
	mieff = 0.0;

	//jl145 - For Top Veto
	tvspacing = 100.0;
	topveto = false;
}

WCSimTuningParameters::~WCSimTuningParameters()
{
	delete TuningMessenger;
	TuningMessenger = 0;
}
