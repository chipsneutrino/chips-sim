#pragma once

/////////////////////////////////////////////////////////////////
//
// Save the Pmt info
//
/////////////////////////////////////////////////////////////////

#include <string>
#include "TObject.h"

class TDirectory;

class WCSimPmtInfo : public TObject
{
private:
	Int_t cylocation;
	Double_t trans_x;
	Double_t trans_y;
	Double_t trans_z;
	Double_t orien_x;
	Double_t orien_y;
	Double_t orien_z;
	Int_t tube;
	std::string pmtName;

public:
	WCSimPmtInfo();

	WCSimPmtInfo(Int_t i, Double_t t1, Double_t t2, Double_t t3, Double_t o1, Double_t o2, Double_t o3,
				 Int_t tubeid, std::string name);

	virtual ~WCSimPmtInfo();

	Int_t Get_cylocation()
	{
		return cylocation;
	}
	Double_t Get_transx()
	{
		return trans_x;
	}
	Double_t Get_transy()
	{
		return trans_y;
	}
	Double_t Get_transz()
	{
		return trans_z;
	}
	Double_t Get_orienx()
	{
		return orien_x;
	}
	Double_t Get_orieny()
	{
		return orien_y;
	}
	Double_t Get_orienz()
	{
		return orien_z;
	}
	Int_t Get_tubeid()
	{
		return tube;
	}
	std::string Get_name()
	{
		return pmtName;
	};

	ClassDef(WCSimPmtInfo, 1);
};
