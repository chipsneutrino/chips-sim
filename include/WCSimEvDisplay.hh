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
  Int_t fColours[10];
  void CalculateChargeAndTimeBins();
  unsigned int GetChargeBin(double charge) const;
  unsigned int GetTimeBin(double time) const;
  void MakeGraphColours();
  void ResetGraphs();
  void DrawHitGraphs(std::vector<TGraph*> vec);

  // The truth display is all contained within TPaveText objects
  TPaveText *fTruthTextMain;
  TPaveText *fTruthTextPrimaries;

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

	// Do we have a WCSim file loaded?
	// -1 = not set
	// 0 = WCSim file
	int fFileType;

	// Do we want to look at charge or time?
	// 0 = charge
	// 1 = time
	int fViewType;

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
	void ResizePads();

  // Default construct some plots
  void MakeDefaultPlots();

	// Function to draw plots from the standard WCSim files
	void FillPlotsFromWCSimEvent();
	// Use the geometry to resize the plots.
	void ResizePlotsFromGeometry();

	// Function to clear the plots 
	void ClearPlots();

	// Function to make the plots look how we want them to.
	void MakePlotsPretty(TH1* h);
	// Function to find what the colour axis minimum should be
	void GetMinColourAxis(TH2D* h);
	// Set the colour axes for the 2D plots
	void SetPlotZAxes();
	// Update the canvases after updating the plots.
	void UpdateCanvases();
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

  // Slot for updating the PE cut
  void SetChargeCut();

	// Button to hide the 1D plots since we don't always want them visible
	void Toggle1DHists();

  // Button to show truth or reco
  void ShowTruth();
  void ShowReco();
  void ShowTruthOverlay();

  // Update the truth TPaveText panel
  void UpdateTruthPave();

  // Is a particle above Cherenkov threshold?
  bool IsAboveCherenkovThreshold(int pdg, double energy);


	ClassDef(WCSimEvDisplay,0)
};

