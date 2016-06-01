#ifndef WCSIMEVDISPLAY_HH
#define WCSIMEVDISPLAY_HH

#include <iostream>
#include <string>

#include <TGClient.h>
#include <TRootEmbeddedCanvas.h>
#include <TGFrame.h>

#include <TVector3.h>

class TChain;
class TH1;
class TH1D;
class TH2D;
class TGWindow;
class TRootEmbeddedCanvas;
class TPad;
class TGHorizontalFrame;
class TGNumberEntry;
class TPaveText;
class TDatabasePDG;
class TPolyMarker;
class TLine;
class TLegend;
class TClonesArray;
class TGraph;
class WCSimTruthSummary;
class TText;

class WCSimEvDispPi0 {

public:
  WCSimEvDispPi0(){};
  ~WCSimEvDispPi0(){};

  void SetPi0Information(double en, TVector3 vtx, TVector3 dir) {fPi0Energy = en; fPi0Vtx = vtx; fPi0Dir = dir;};
  void SetPhotonInformation(int id, double en, TVector3 dir){
    if(id == 1){
      fPhotonEnergy1 = en; fPhotonDir1 = dir; 
    }
    else if(id == 2){
      fPhotonEnergy2 = en; fPhotonDir2 = dir; 
    }
    else std::cerr << "Photon id must be either 1 or 2" << std::endl;
  }; 
  
  double GetPi0Energy() const {return fPi0Energy;};
  TVector3 GetPi0Vertex() const {return fPi0Vtx;};
  TVector3 GetPi0Direction() const {return fPi0Dir;};

  double GetPhotonEnergy(int id) const {
    if(id == 1) return fPhotonEnergy1;
    else if(id == 2) return fPhotonEnergy2;
    else{
      std::cerr << "Photon id must be either 1 or 2" << std::endl;
      return 0.;
    }
  };

  TVector3 GetPhotonDirection(int id) const {
    if(id == 1) return fPhotonDir1;
    else if(id == 2) return fPhotonDir2;
    else{
      std::cerr << "Photon id must be either 1 or 2" << std::endl;
      return TVector3(0,0,0);
    }
  };

  void Print() const{
    std::cout << "== WCSimEvDispPi0 ==" << std::endl;
    std::cout << " - Energy    = " << fPi0Energy << std::endl;
    std::cout << " - Vertex    = " << fPi0Vtx.X() << "," << fPi0Vtx.Y() << "," << fPi0Vtx.Z() << std::endl;
    std::cout << " - Direction = " << fPi0Dir.X() << "," << fPi0Dir.Y() << "," << fPi0Dir.Z() << std::endl;
    std::cout << " = Decay Photon: " << std::endl;
    std::cout << "  - Energy    = " << fPhotonEnergy1 << std::endl;
    std::cout << "  - Direction = " << fPhotonDir1.X() << "," << fPhotonDir1.Y() << "," << fPhotonDir1.Z() << std::endl;
    std::cout << " = Decay Photon: " << std::endl;
    std::cout << "  - Energy    = " << fPhotonEnergy2 << std::endl;
    std::cout << "  - Direction = " << fPhotonDir2.X() << "," << fPhotonDir2.Y() << "," << fPhotonDir2.Z() << std::endl;
  }

private:
  // Pi0 information
  double fPi0Energy;
  TVector3 fPi0Vtx;
  TVector3 fPi0Dir;
  // Decay photon information
  double fPhotonEnergy1;    
  double fPhotonEnergy2;    
  TVector3 fPhotonDir1;
  TVector3 fPhotonDir2;
};

class WCSimEvDisplay : public TGMainFrame {
protected:

	// Canvas to show the hit PMTs
  TRootEmbeddedCanvas *fHitMapCanvas;
	// This Canvas will contain 5 Pads
	TPad *fBarrelPad;
	TPad *fTopPad;
	TPad *fBottomPad;
	TPad *fTimePad;
	TPad *fChargePad;
  // Add a truth information pad
  TPad *fTruthPad;
  // Truth overlay pad containing the legend
  TPad *fTruthOverlayPad;
  TLegend *fTruthLegend;
  // Which pad do we want to see?
  // 0 = Reco
  // 1 = Truth
  // 2 = Reco with Truth Overlay
  unsigned int fWhichPads;
  TDatabasePDG* fDatabasePDG;

	// The three 2D histograms that show the hits
	TH2D *fBarrelHist;
	TH2D *fTopHist;
	TH2D *fBottomHist;
	// Single 1D histogram to show either charge or time
	TH1D *fChargeHist;
	TH1D *fTimeHist;

  // Some histogram text titles
  TText *fBarrelTitle;
  TText *fTopTitle;
  TText *fBottomTitle;

  // As the PMTs are not uniform, use TGraphs to display the points
  // Store a vector of 10 graphs for each of the 3 regions, with each
  // graph storing a range of charges.
  std::vector<TGraph*> fTopGraphs;
  std::vector<TGraph*> fBarrelGraphs;
  std::vector<TGraph*> fBottomGraphs;
  std::vector<double> fChargeBins; // Lower edges
  std::vector<double> fTimeBins; // Lower edges
  double fQMin;
  double fQMax;
  double fTMin;
  double fTMax;
  std::vector<Int_t> fColours;
  void InitialiseGraph(TGraph* g, int i);
  void CalculateChargeAndTimeBins();
  unsigned int GetChargeBin(double charge) const;
  unsigned int GetTimeBin(double time) const;
  void MakeGraphColours();
  void ResetGraphs();
  void DrawHitGraphs(std::vector<TGraph*> vec);

  // The truth display is all contained within TPaveText objects
  TPaveText *fTruthTextMain;
  TPaveText *fTruthTextPrimaries;
  // Also have a truth text display for overlay events
  TPaveText *fTruthTextOverlay;
  // Function to change the sizes of the TPaveTexts
  void ResizeTruthPaveTexts(bool isOverlay);

	// When using WCSim files, store the geometry information
	double fWCRadius;
	double fWCLength;
	double fPMTRadius;
	int fPMTBarrel;
	int fPMTTop;
	int fPMTBottom;
	int fPMTVeto;

	// The WCSim only buttons frame need to be a member variable
	// so that we can hide it.
	TGHorizontalFrame *hWCSimButtons;

	// Current event counter
	int fCurrentEvent;
	// Minimum value for event counter, from file.
	int fMinEvent;
	// Maximum value for event counter, from file.
	int fMaxEvent;
  // Entry field to set the event
  TGNumberEntry *fEventInput;

	// Do we have a WCSim file loaded?
	// -1 = not set
	// 0 = WCSim file
	int fFileType;

	// Do we want to look at charge or time?
	// 0 = charge
	// 1 = time
	int fViewType;

  // Do we want to display veto hits?
  bool fViewVeto;

  // Switch to log scale for charge plots
  bool fLogZCharge;

	// The current file to look at
	TChain *fChain;
	TChain *fGeomTree;

  // A pointer to the truth summary object of the current event
  WCSimTruthSummary *fTruthSummary;

  // Vectors of TPolyMarkers to store the truth rings for each detector region
  std::vector<TPolyMarker*> fTruthMarkersTop;
  std::vector<TPolyMarker*> fTruthMarkersBarrel;
  std::vector<TPolyMarker*> fTruthMarkersBottom;
  std::vector<TLine*> fTruthLines; // One line per truth ring to fill the legend

  // Charge cut and corresponding entry box
  TGNumberEntry *fPEInput;
  double fChargeCut;

	// Flag to decide whether we show the 1D plots
	bool fShow1DHists;
	// Function to update the pads as a result.
	virtual void ResizePads();

  // Default construct some plots
  void MakeDefaultPlots();
  void FormatTitles(TText *t);

	// Function to draw plots from the standard WCSim files
  virtual void FillPlots();
	void FillPlotsFromWCSimEvent();
	// Use the geometry to resize the plots.
	void ResizePlotsFromGeometry();

	// Function to clear the plots 
	void ClearPlots();

	// Function to make the plots look how we want them to.
	void MakePlotsPretty(TH1* h);
  // Adjust the palette axis on the barrel plot.
  void AdjustBarrelZAxis();

	// Function to find what the colour axis minimum should be
	void GetMinColourAxis(TH2D* h);
	// Set the colour axes for the 2D plots
	void SetPlotZAxes();
	// Update the canvases after updating the plots.
	virtual void UpdateCanvases();
  // Draw the reco plots to their pads, but don't show yet.
  void UpdateRecoPads();
  // Draw the truth information to its pad, but don't show yet.
  void UpdateTruthPad();
  // Draw the truth overlay rings to their pads
  void UpdateTruthOverlayPad();

	// Set up the style for the plots
	void SetStyle();

	// The actual behind the scenes code that opens the files.	
	void OpenNtupleFile(std::string name);
	void OpenWCSimFile(std::string name);

	// Encapsulate some of the GUI generation
	void CreateMainButtonBar();
	void CreateSubButtonBar();

  // Convert the true event type into a string
  std::string ConvertTrueEventType() const;
  std::string GetParticleName(int pdgCode);

  // Few functions to draw truth rings
  void DrawTruthRing(unsigned int particleNo, int colour);
  void ProjectToWall(TVector3 vtx, TVector3 dir, TVector3& proj, unsigned int& region);
  void FindCircle(TVector3 proj, TVector3 vtx, double thetaC, double phi, TVector3& circPos, TVector3& circDir);
  void MakePolyMarker(std::vector<double> coord1, std::vector<double> coord2, std::vector<TPolyMarker*>& poly, int colour);
  void ClearTruthMarkerVectors();
  void DeleteAndClearElements(std::vector<TPolyMarker*>& vec);
  int GetTruthRingColour(int ring) const;
  void DrawTruthOverlays();
  void HideTruthOverlays();

  // Treatment for pi0s
  std::vector<WCSimEvDispPi0*> fPi0s; // Store the pi-zeros with sufficient energy and their decay photon information
  void SearchForPi0Photons(double pi0En, TClonesArray* trajCont);
  void ClearPi0Vector();
  unsigned int GetPi0(double en) const;

public:
  WCSimEvDisplay(const TGWindow *p,UInt_t w,UInt_t h);
  WCSimEvDisplay();
  ~WCSimEvDisplay();

	// Public method to set the input file. Useful for supplying the file 
	// from the command line.
	virtual void SetInputFile(std::string name);

	// All "slot" commands, ie those that are called by buttons
	// need to be public.
	// Open the file to display events from
	virtual void OpenFile(std::string name);
	void OpenFile();
	// Toggles to switch between events
  void NextEvent();
	void PrevEvent();
  void SetEvent();

	// Save the main canvas
	void SaveEvent();

	// Overload this inherited function to kill the program on 
	// clicking the window "x:. 
	void CloseWindow();

	// Toggles for buttons to view charge or time for the WCSim files
	void SetViewCharge();
	void SetViewTime();

  // Slot for updating the PE cut
  void SetChargeCut();

	// Button to hide the 1D plots since we don't always want them visible
	void Toggle1DHists();

  // Toggle between log and normal z axes for charge
  void ToggleLogZ();

  // Button to show truth or reco
  void ShowTruth();
  void ShowReco();
  void ShowTruthOverlay();

  // Update the truth TPaveText panel
  void UpdateTruthPave();

  // Toggle between inner detector and veto
  void ShowVeto();

  // Is a particle above Cherenkov threshold?
  bool IsAboveCherenkovThreshold(int pdg, double energy);


	ClassDef(WCSimEvDisplay,0)
};

#endif
