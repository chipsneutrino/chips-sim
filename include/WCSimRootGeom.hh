#ifndef WCSim_RootGeom
#define WCSim_RootGeom

//////////////////////////////////////////////////////////////////////////
//                                                                      
// WCSim_RootGeom                                                      
//                                                                      
// This class contains information needed to be passed to reconstruction
//     routines.  It's just simple right now-- only the bare-bones  
//     WC info
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TClonesArray.h"

class TDirectory;

//////////////////////////////////////////////////////////////////////////

class WCSimRootPMT: public TObject {

	private:
		Int_t fTubeNo;
		Int_t fCylLoc;  // endcap1, wall, endcap2
		Float_t fOrientation[3];
		Float_t fPosition[3];
		Float_t fRadius; // In mm
		Float_t fMaxRadius; // in mm
		TString fPMTName;

	public:
		WCSimRootPMT();
		WCSimRootPMT(Int_t tubeNo, Int_t cylLoc, Float_t orientation[3], Float_t position[3], Float_t rad,
				Float_t maxRad, TString pmtName);
		virtual ~WCSimRootPMT();

		void SetTubeNo(Int_t i) {
			fTubeNo = i;
		}
		void SetCylLoc(Int_t i) {
			fCylLoc = i;
		}
		void SetOrientation(Int_t i, Float_t f) {
			fOrientation[i] = ((i < 3) ? f : 0);
		}
		void SetPosition(Int_t i, Float_t f) {
			fPosition[i] = ((i < 3) ? f : 0);
		}
		void SetRadius(Float_t rad) {
			fRadius = rad;
		}
		void SetMaxRadius(Float_t maxRad) {
			fMaxRadius = maxRad;
		}

		void SetPMTName(TString name) {
			fPMTName = name;
		}
		Int_t GetTubeNo() const {
			return fTubeNo;
		}
		Int_t GetCylLoc() const {
			return fCylLoc;
		}
		Float_t GetOrientation(Int_t i = 0) {
			return (i < 3) ? fOrientation[i] : 0;
		}
		Float_t GetPosition(Int_t i = 0) {
			return (i < 3) ? fPosition[i] : 0;
		}
		Float_t GetRadius() {
			return fRadius;
		}
		Float_t GetMaxRadius() {
			return fMaxRadius;
		}
		TString GetPMTName() {
			return fPMTName;
		}

		ClassDef(WCSimRootPMT,1)
		//WCSimPMT structure
};

//////////////////////////////////////////////////////////////////////////

class WCSimRootGeom: public TObject {

	private:

		static const Int_t maxNumPMT = 40000;
		Float_t fWCCylRadius;  // Radius of WC tank
		Float_t fWCCylLength;  // Length of WC tank

		Float_t fMailBox_x; //mailbox x
		Float_t fMailBox_y; //mailbox y
		Float_t fMailBox_z; //mailbox z
		Int_t fgeo_type;  // mailbox or cylinder?

		Float_t fWCPMTRadius; // Radius of PMT
		Int_t fWCNumPMT;   // Number of PMTs
		Int_t fWCNumVetoPMT; // Number of veto PMTs
		Float_t fWCOffset[3]; // Offset of barrel center in global coords
		Int_t fOrientation; //Orientation o detector, 0 is 2km horizontal, 1 is Upright

		// Could make a TClonesArray of PMTs but let's keep it simple
		//   since the arrays just won't be that large
		//WCSimRootPMT          fPMTArray[maxNumPMT];  // Array of PMTs
		TClonesArray *fPMTArray;

	public:

		WCSimRootGeom();
		virtual ~WCSimRootGeom();

		// Sets and gets

		void SetWCCylRadius(Float_t f) {
			fWCCylRadius = f;
		}
		void SetWCCylLength(Float_t f) {
			fWCCylLength = f;
		}

		void SetMailBox_x(Float_t f) {
			fMailBox_x = f;
		}
		void SetMailBox_y(Float_t f) {
			fMailBox_y = f;
		}
		void SetMailBox_z(Float_t f) {
			fMailBox_z = f;
		}

		void SetGeo_Type(Int_t f) {
			fgeo_type = f;
		}

		void SetWCNumPMT(Int_t i) {
			fWCNumPMT = i;
		}
		void SetWCNumVetoPMT(Int_t i) {
			fWCNumVetoPMT = i;
		}
		void SetWCPMTRadius(Float_t f) {
			fWCPMTRadius = f;
		}
		void SetWCOffset(Float_t x, Float_t y, Float_t z) {
			fWCOffset[0] = x;
			fWCOffset[1] = y;
			fWCOffset[2] = z;
		}
		void SetPMT(Int_t i, Int_t tubeno, Int_t cyl_loc, Float_t rot[3], Float_t pos[3], Float_t rad, Float_t maxRad,
				TString pmtName, bool expand = true);
		void SetOrientation(Int_t o) {
			fOrientation = o;
		}

		Float_t GetWCCylRadius() const {
			return fWCCylRadius;
		}
		Float_t GetWCCylLength() const {
			return fWCCylLength;
		}

		Float_t GetMailBox_x() const {
			return fMailBox_x;
		}
		Float_t GetMailBox_y() const {
			return fMailBox_y;
		}
		Float_t GetMailBox_z() const {
			return fMailBox_z;
		}
		Int_t GetGeo_Type() const {
			return fgeo_type;
		}

		Int_t GetWCNumPMT() const {
			return fWCNumPMT;
		}
		Int_t GetWCNumVetoPMT() const {
			return fWCNumVetoPMT;
		}
		Int_t GetWCNumInnerPMT() const {
			return fWCNumPMT - fWCNumVetoPMT;
		}
		Float_t GetWCPMTRadius() const {
			return fWCPMTRadius;
		}
		Float_t GetWCOffset(Int_t i) const {
			return (i < 3) ? fWCOffset[i] : 0.;
		}
		Int_t GetOrientation() {
			return fOrientation;
		}

		// WCSimRootPMT GetPMT(Int_t i){return *(new WCSimRootPMT());}
		//quick hack to get the pmt with correct tubeid /11.09.13
		WCSimRootPMT GetPMTFromTubeID(Int_t tubeid) {
			return *(WCSimRootPMT*) (*fPMTArray)[tubeid - 1];
		}
		WCSimRootPMT GetPMT(Int_t i) {
			return this->GetPMTFromArray(i);
		}
		WCSimRootPMT GetPMTFromArray(Int_t index) {
			return *(WCSimRootPMT*) (*fPMTArray)[index];
		}

		ClassDef(WCSimRootGeom,1)
		//WCSimRootEvent structure
};

#endif
