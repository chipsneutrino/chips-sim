/*
 * WCSimEmissionProfileMaker.hh
 *
 *  Created on: 5 May 2015
 *      Author: ajperch
 */

#ifndef WCSIMEMISSIONPROFILEMAKER_HH_
#define WCSIMEMISSIONPROFILEMAKER_HH_

#include "G4String.hh"
#include "TFile.h"
#include "TVector3.h"

class G4TrajectoryContainer;
class WCSimTruthSummary;
class TH1F;
class TH2F;
class TTree;

class WCSimEmissionProfileMaker {
	public:
		WCSimEmissionProfileMaker(G4String file = "");
		virtual ~WCSimEmissionProfileMaker();

		static WCSimEmissionProfileMaker* Instance(G4String str);
		static WCSimEmissionProfileMaker* Instance();

		static void Open(const char* filename);
		static void Close();
		static void Fill(G4TrajectoryContainer * trajCont, WCSimTruthSummary * truthSumm);

		static void FileName(const char* filename);

		void SetFileName(const char* filename) {
			fSaveFileName = filename;
		}
		void OpenFile(const char* filename);
		void CloseFile();

	private:
		void FillEvent(G4TrajectoryContainer * trajCont, WCSimTruthSummary * truthSumm);

		void SmoothRho();
		void SmoothG();

		void NormaliseHistograms();
		void NormaliseRho();
		void NormaliseG();

		template<class T>
		T AverageVector(const std::vector<T> &vec);

		void SetThetaBins();
		void SetSBins();
		void MakeHistograms();

		TFile * fSaveFile;
		std::string fSaveFileName;

		int fPDG;
		int fNumEvents;
		int fNumPhotons;
		double fEnergy;

		// Raw histograms of s and cos theta
		TH1F * fS;
		TH2F * fSCosThetaFine;
		TH2F * fSCosThetaCoarse;
		TH2F * fSSecTheta;
		TH2F * fSCosThetaCheckBinning;

		// The emission profiles: take the raw histograms and normalise them
		TH1F * fRho;
		TH2F * fGFine;
		TH2F * fGCoarse;

		TH1F * fWavelengths;

		int fNBinsS;  /// Number of s bins for emission profile
		float fSMin;    /// Minimum value of s
		float fSMax;    /// Maximum value of s
		int fNBinsTh; /// Number of cosTheta bins for emission profile
		float fThMin;   /// Minimum value of cosTheta
		float fThMax;   /// Maximum value of cosTheta

		float fThetaCoarseMin;
		float fThetaCoarseMax;
		int fNBinsThetaCoarse;

		float fThetaFineMin;
		float fThetaFineMax;
		int fNBinsThetaFine;

		TTree * fPhotonTree;
		std::vector<TVector3> fPhotonVertices;
		std::vector<TVector3> fPhotonDirections;

};

#endif /* WCSIMEMISSIONPROFILEMAKER_HH_ */
