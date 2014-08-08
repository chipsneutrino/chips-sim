#include <TGClient.h>
#include <TRootEmbeddedCanvas.h>
#include <TGFrame.h>

class TChain;
class TH1;
class TH1D;
class TH2D;
class TGWindow;
class TRootEmbeddedCanvas;
class TPad;
class TGHorizontalFrame;

class WCSimEvDisplay : public TGMainFrame {
private:
	// Canvas to show the hit PMTs
  TRootEmbeddedCanvas *fHitMapCanvas;
	// This Canvas will contain 5 Pads
	TPad *fBarrelPad;
	TPad *fTopPad;
	TPad *fBottomPad;
	TPad *fTimePad;
	TPad *fChargePad;

	// The three 2D histograms that show the hits
	TH2D *fBarrelHist;
	TH2D *fTopHist;
	TH2D *fBottomHist;
	// Single 1D histogram to show either charge or time
	TH1D *fChargeHist;
	TH1D *fTimeHist;

	// When using WCSim files, store the geometry information
	double fWCRadius;
	double fWCLength;
	double fPMTRadius;
	int fPMTBarrel;
	int fPMTTop;
	int fPMTBottom;

	// The WCSim only buttons frame need to be a member variable
	// so that we can hide it.
	TGHorizontalFrame *hWCSimButtons;

	// Current event counter
	int fCurrentEvent;
	// Minimum value for event counter, from file.
	int fMinEvent;
	// Maximum value for event counter, from file.
	int fMaxEvent;

	// Do we have a photon ntuple or WCSim file?
	// -1 = not set
	// 0 = WCSim file
	// 1 = Photon Ntuple
	int fFileType;

	// Do we want to look at charge or time?
	// 0 = charge
	// 1 = time
	int fViewType;

	// The current file to look at
	TChain *fChain;
	TChain *fGeomTree;

	// Flag to decide whether we show the 1D plots
	bool fShow1DHists;
	// Function to update the pads as a result.
	void ResizePads(bool forceHide = false);

	// Function to draw plots from the standard WCSim files
	void FillPlotsFromWCSimEvent();
	// Use the geometry to resize the plots.
	void ResizePlotsFromGeometry();
	// Resize the plots for the photon ntuple...
	void ResizePlotsFromNtuple();

	// Function to fill the plots using photon ntuples
	void FillPlotsFromPhotonEvent();
	// Check if the photon started in the detector.
	bool PhotonStartedInDet(Float_t vx, Float_t vy, Float_t vz);

	// Function to clear the plots 
	void ClearPlots();

	// Function to make the plots look how we want them to.
	void MakePlotsPretty(TH1* h);
	// Function to find what the colour axis minimum should be
	void GetMinColourAxis(TH2D* h);
	// Match the colour axes for the 2D plots
	void MatchPlotZAxes();
	// Update the canvases after updating the plots.
	void UpdateCanvases();

	// Set up the style for the plots
	void SetStyle();

	// Get the first and last photon events
	void GetMinMaxPhotonEvents();

	// The actual behind the scenes code that opens the files.	
	void OpenNtupleFile(std::string name);
	void OpenWCSimFile(std::string name);

	// Encapsulate some of the GUI generation
	void CreateMainButtonBar();
	void CreateSubButtonBar();

public:
  WCSimEvDisplay(const TGWindow *p,UInt_t w,UInt_t h);
  WCSimEvDisplay();
  ~WCSimEvDisplay();

	// Public method to set the input file. Useful for supplying the file 
	// from the command line.
	void SetInputFile(std::string name);

	// All "slot" commands, ie those that are called by buttons
	// need to be public.
	// Open the file to display events from
	void OpenFile(std::string name);
	void OpenFile();
	// Toggles to switch between events
  void NextEvent();
	void PrevEvent();

	// Save the main canvas
	void SaveEvent();

	// Overload this inherited function to kill the program on 
	// clicking the window "x:. 
	void CloseWindow();

	// Toggles for buttons to view charge or time for the WCSim files
	void SetViewCharge();
	void SetViewTime();

	// Button to hide the 1D plots since we don't always want them visible
	void Toggle1DHists();

	ClassDef(WCSimEvDisplay,0)
};

