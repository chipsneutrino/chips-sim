#include <cstring>
#include <string>
#include <sstream>
//#include <TGClient.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TRandom.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TApplication.h>
#include <TMath.h>
#include <TH2D.h>
#include <TList.h>
#include <TChain.h>
#include <TGFileDialog.h>
#include <TGMenu.h>
#include <TGNumberEntry.h>
#include <TGLabel.h>
#include <TVector3.h>
#include <TPaveText.h>
#include <TDatabasePDG.h>
#include <TParticlePDG.h>
#include <TPolyMarker.h>
#include <TLine.h>
#include <TLegend.h>
#include <TClonesArray.h>
#include <TGraph.h>
#include <TColor.h>
//#include <TRootEmbeddedCanvas.h>
//#include <RQ_OBJECT.h>
#include "WCSimEvDisplay.hh"
#include "WCSimRootGeom.hh"
#include "WCSimRootEvent.hh"
#include "WCSimTruthSummary.hh"

ClassImp(WCSimEvDisplay)

WCSimEvDisplay::WCSimEvDisplay() : TGMainFrame(){
	// Do nothing
}

WCSimEvDisplay::WCSimEvDisplay(const TGWindow *p,UInt_t w,UInt_t h) : TGMainFrame(p,w,h) {

  // Initialise some histogram pointers
  fBarrelHist = 0x0;
  fTopHist = 0x0;
  fBottomHist = 0x0;
  fChargeHist = 0x0;
  fTimeHist = 0x0;

	// Initialise the TChain pointers
	fChain = 0x0; 
  fGeomTree = 0x0;

  // Initialise the truth object
  fTruthSummary = 0x0;
  fTruthLegend = 0x0;
  fWhichPads = 0; // Default is reco
  fTruthTextMain = new TPaveText(0.05,0.45,0.95,0.90,"NDC");
  fTruthTextPrimaries = new TPaveText(0.05,0.1,0.95,0.40,"NDC");
  fDatabasePDG = 0x0;

  // Initialise the TGNumberEntry
  fEventInput = 0x0;
  fPEInput = 0x0;
  fChargeCut = 0;

  // Create the TGraph vectors with default TGraphs
  this->MakeGraphColours();
  gStyle->SetPalette(10,fColours);
  for(unsigned int g = 0; g < 10; ++g){
    fTopGraphs.push_back(new TGraph());
    fBarrelGraphs.push_back(new TGraph());
    fBottomGraphs.push_back(new TGraph());
    // Initialise the graphs
    this->InitialiseGraph(fTopGraphs[g],g);
    this->InitialiseGraph(fBarrelGraphs[g],g);
    this->InitialiseGraph(fBottomGraphs[g],g);
  }

	// Set up some plot style
	this->SetStyle();

	std::cout << "Starting WCSim Event Display" << std::endl;

	// Create the menu bar that lives at the top of the window
	TGMenuBar *mainMenu = new TGMenuBar(this,400,20,kHorizontalFrame);
//	mainMenu->AddPopup("&File",fMenuFile,new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
	this->AddFrame(mainMenu,new TGLayoutHints(kLHintsTop|kLHintsLeft|kLHintsExpandX,0,0,0,2));

  // Create canvas widget
  fHitMapCanvas = new TRootEmbeddedCanvas("barrelCanvas",this,400,200);
	TCanvas *can = fHitMapCanvas->GetCanvas();
	can->cd();
	fBarrelPad = new TPad("fBarrelPad","",0.0,0.6,1.0,1.0);
	fTopPad = new TPad("fTopPad","",0.0,0.2,0.5,0.6);
	fBottomPad = new TPad("fBottomPad","",0.5,0.2,1.0,0.6);
	fChargePad = new TPad("fChargePad","",0.0,0.0,0.5,0.2);
	fTimePad = new TPad("fTimePad","",0.5,0.0,1.0,0.2);
	fBarrelPad->Draw();
	fTopPad->Draw();
	fBottomPad->Draw();
	fChargePad->Draw();
	fTimePad->Draw();
  // Create the truth information pad, but don't draw it
  fTruthPad = new TPad("fTruthPad","",0.0,0.0,1.0,1.0);
  fTruthOverlayPad = new TPad("fTruthOverlayPad","",0.0,0.0,1.0,0.2);

	this->AddFrame(fHitMapCanvas, new TGLayoutHints(kLHintsExpandX| kLHintsExpandY,10,10,10,1));

	// These build from top to bottom, so add them in the correct order.
	this->CreateSubButtonBar();
	this->CreateMainButtonBar();

  // Set a name to the main frame
  this->SetWindowName("CHIPS Event Display");
  // Map all subwindows of main frame
  this->MapSubwindows();
  // Set the window size
  this->Resize(w,h);
  // Map main frame
  this->MapWindow();

	// Create temporary dummy plots then update them later
  this->MakeDefaultPlots();
	this->UpdateCanvases();

	// Set the current event to the default value of 0.
	fCurrentEvent = 0;
	fFileType = -1;
	this->HideFrame(hWCSimButtons);
	fViewType = 0; // Look at charge by default

	// By default show the 1D plots
	fShow1DHists = 1;
}

void WCSimEvDisplay::CreateMainButtonBar(){
  // Create a horizontal frame widget with buttons
  TGHorizontalFrame *hframe = new TGHorizontalFrame(this,200,40);
  TGTextButton *open = new TGTextButton(hframe,"&Open");
  open->Connect("Clicked()","WCSimEvDisplay",this,"OpenFile()");
  hframe->AddFrame(open, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
  TGTextButton *prev = new TGTextButton(hframe,"&Prev");
  prev->Connect("Clicked()","WCSimEvDisplay",this,"PrevEvent()");
  hframe->AddFrame(prev, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
  TGTextButton *next = new TGTextButton(hframe,"&Next");
  next->Connect("Clicked()","WCSimEvDisplay",this,"NextEvent()");
  hframe->AddFrame(next, new TGLayoutHints(kLHintsCenterX,5,5,3,4));

  // Numeric entry field for choosing events
  fEventInput = new TGNumberEntry(hframe,0,9,999,TGNumberFormat::kNESInteger,
                               TGNumberFormat::kNEANonNegative);
  fEventInput->Connect("ValueSet(Long_t)","WCSimEvDisplay",this,"SetEvent()");
  (fEventInput->GetNumberEntry())->Connect("ReturnPressed()","WCSimEvDisplay",this,"SetEvent()");
  // Make a label to go along side it
  TGLabel *eventLabel = new TGLabel(hframe,"Event:");
	hframe->AddFrame(eventLabel, new TGLayoutHints(kLHintsCenterX&&kLHintsCenterY,5,5,3,4));
	hframe->AddFrame(fEventInput, new TGLayoutHints(kLHintsCenterX,5,5,3,4));  

  TGTextButton *save = new TGTextButton(hframe,"&Save");
  save->Connect("Clicked()","WCSimEvDisplay",this,"SaveEvent()");
  hframe->AddFrame(save, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
  TGTextButton *exit = new TGTextButton(hframe,"&Exit","gApplication->Terminate(0)");
  hframe->AddFrame(exit, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
  this->AddFrame(hframe, new TGLayoutHints(kLHintsCenterX,2,2,2,2));
}

void WCSimEvDisplay::CreateSubButtonBar(){
	// Create a horizontal frame to store buttons specific to WCSim files
	hWCSimButtons = new TGHorizontalFrame(this,200,40);
  // Show Charge on z axis
	TGTextButton *togCharge = new TGTextButton(hWCSimButtons,"&Charge");
	togCharge->Connect("Clicked()","WCSimEvDisplay",this,"SetViewCharge()");
	hWCSimButtons->AddFrame(togCharge, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
  // Show time on z axis
	TGTextButton *togTime = new TGTextButton(hWCSimButtons,"&Time");
	togTime->Connect("Clicked()","WCSimEvDisplay",this,"SetViewTime()");
	hWCSimButtons->AddFrame(togTime, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
  // Toggle the 1D plots
	TGTextButton *togHists = new TGTextButton(hWCSimButtons,"Toggle 1D Plots");
	togHists->Connect("Clicked()","WCSimEvDisplay",this,"Toggle1DHists()");
	hWCSimButtons->AddFrame(togHists, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
  // Show the standard reco view
  TGTextButton *showReco = new TGTextButton(hWCSimButtons,"&Reco");
  showReco->Connect("Clicked()","WCSimEvDisplay",this,"ShowReco()");
  hWCSimButtons->AddFrame(showReco, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
  // Show the truth overlay view
  TGTextButton *showTruthOverlay = new TGTextButton(hWCSimButtons,"&Truth Overlay");
  showTruthOverlay->Connect("Clicked()","WCSimEvDisplay",this,"ShowTruthOverlay()");
  hWCSimButtons->AddFrame(showTruthOverlay, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
  // Show the truth summary information
  TGTextButton *showTruth = new TGTextButton(hWCSimButtons,"&Truth");
  showTruth->Connect("Clicked()","WCSimEvDisplay",this,"ShowTruth()");
  hWCSimButtons->AddFrame(showTruth, new TGLayoutHints(kLHintsCenterX,5,5,3,4));

  // Numeric entry field for the PE threshold
  fPEInput = new TGNumberEntry(hWCSimButtons,0,9,999,TGNumberFormat::kNESRealOne,
                               TGNumberFormat::kNEANonNegative);
  // TGNumberEntry has two ways to set numbers, so two connects
  fPEInput->Connect("ValueSet(Long_t)","WCSimEvDisplay",this,"SetChargeCut()");
  (fPEInput->GetNumberEntry())->Connect("ReturnPressed()","WCSimEvDisplay",this,"SetChargeCut()");
  // Make a label to go along side it
  TGLabel *peLabel = new TGLabel(hWCSimButtons,"Charge Cut:");
	hWCSimButtons->AddFrame(peLabel, new TGLayoutHints(kLHintsCenterX&&kLHintsCenterY,5,5,3,4));
	hWCSimButtons->AddFrame(fPEInput, new TGLayoutHints(kLHintsCenterX,5,5,3,4));

  // Add the TGHorizontalFrame to the main layout
	this->AddFrame(hWCSimButtons,new TGLayoutHints(kLHintsCenterX,2,2,2,2));
}

void WCSimEvDisplay::FillPlotsFromWCSimEvent(){
	// First things first, clear the histograms.
	this->ClearPlots();

	// Need to load the events.	
	WCSimRootEvent *wcSimEvt = new WCSimRootEvent();
 	fChain->SetBranchAddress("wcsimrootevent",&wcSimEvt);
  // Force deletion to prevent memory leak 
  fChain->GetBranch("wcsimrootevent")->SetAutoDelete(kTRUE);

	// Get the geometry
	WCSimRootGeom *geo = new WCSimRootGeom();
	fGeomTree->SetBranchAddress("wcsimrootgeom",&geo);
	fGeomTree->GetEntry(0);

	// Get the current event;
	fChain->GetEntry(fCurrentEvent);
	if(wcSimEvt==0x0) std::cout << "Null pointer :( " << std::endl;
	WCSimRootTrigger* wcSimTrigger = wcSimEvt->GetTrigger(0);

  // Get the truth information
  if(fTruthSummary != 0x0){
    delete fTruthSummary;
    fTruthSummary = 0x0;
  }
  fTruthSummary = new WCSimTruthSummary(wcSimEvt->GetTruthSummary());
  this->ClearPi0Vector();

  // Quick check for pi zeroes and their decay photons
  if(fTruthSummary->IsPrimaryPiZero()){
    std::vector<double> pi0EnVec = fTruthSummary->GetPiZeroEnergies();
    for(unsigned int p = 0; p < pi0EnVec.size(); ++p){
      this->SearchForPi0Photons(pi0EnVec[p],wcSimTrigger->GetTracks());
    }
  }

  // Update the truth view
  this->UpdateTruthPave();

  int nDigiHits = wcSimTrigger->GetNcherenkovdigihits();
	std::cout << "Number of PMTs hit: " << nDigiHits << std::endl;

  // Need to loop through the hits once to find the charge and time ranges
  fQMin = 1e10;
  fQMax = -1e10;
  fTMin = 1e10;
  fTMax = -1e10;
  for(int i = 0; i < nDigiHits; ++i){
		TObject *element = (wcSimTrigger->GetCherenkovDigiHits())->At(i);
		WCSimRootCherenkovDigiHit *hit = dynamic_cast<WCSimRootCherenkovDigiHit*>(element);
    double q = hit->GetQ();
    double t = hit->GetT();
    if(q < fQMin) fQMin = q;
    if(q > fQMax) fQMax = q;
    if(t < fTMin) fTMin = t;
    if(t > fTMax) fTMax = t;
  }
  // For now, always want charge to start at 0.
  fQMin = 0;
  this->CalculateChargeAndTimeBins();
  this->ResetGraphs();

  // Now loop through again and fill things
	for (int i=0;i<nDigiHits;i++)
	{
		// Loop through elements in the TClonesArray of WCSimRootCherenkovDigHits
		TObject *element = (wcSimTrigger->GetCherenkovDigiHits())->At(i);

		WCSimRootCherenkovDigiHit *wcSimDigiHit = dynamic_cast<WCSimRootCherenkovDigiHit*>(element);

		WCSimRootPMT pmt = geo->GetPMT(wcSimDigiHit->GetTubeId());
		double pmtX = pmt.GetPosition(0);
		double pmtY = pmt.GetPosition(1);
		double pmtZ = pmt.GetPosition(2);
		double pmtPhi = TMath::ATan2(pmtY,pmtX);
		double pmtQ = wcSimDigiHit->GetQ();
		double pmtT = wcSimDigiHit->GetT();
		// Set the z-axis to be charge or time.
		double colourAxis = pmtQ;
		if(fViewType == 1) colourAxis = pmtT;

    // Make sure we pass the charge cut
    if(pmtQ > fChargeCut){
    unsigned int bin;
    if(fViewType == 0) bin = this->GetChargeBin(pmtQ);
    else bin = this->GetTimeBin(pmtT);

    // Set the underflow bin of the histograms to make sure the colour axis shows
    fTopHist->SetBinContent(0,1);
    fBarrelHist->SetBinContent(0,1);
    fBottomHist->SetBinContent(0,1);
    
	  	// Top cap
  		if(pmt.GetCylLoc() == 0){
//  			fTopHist->Fill(pmtY,pmtX,colourAxis);
          fTopGraphs[bin]->SetPoint(fTopGraphs[bin]->GetN(),pmtY,pmtX);
  		}
  		// Bottom cap
  		else if(pmt.GetCylLoc() == 2){
//  			fBottomHist->Fill(pmtY,pmtX,colourAxis);
          fBottomGraphs[bin]->SetPoint(fBottomGraphs[bin]->GetN(),pmtY,pmtX);
      
  		}
  		// Barrel
  		else{
//  			fBarrelHist->Fill(pmtPhi,pmtZ,colourAxis);
          fBarrelGraphs[bin]->SetPoint(fBarrelGraphs[bin]->GetN(),pmtPhi,pmtZ);
  		}

		  // Now fill the 1D histograms
		  fChargeHist->Fill(pmtQ);	 
		  fTimeHist->Fill(pmtT);	 
    }


	} // End of loop over Cherenkov digihits

	delete geo;
	geo = 0x0;

  // Update the pads
  this->UpdateRecoPads();
  this->UpdateTruthPad();
  this->UpdateTruthOverlayPad();
  // Now draw whichever pad we need
	this->UpdateCanvases();
}

void WCSimEvDisplay::InitialiseGraph(TGraph* g, int i){

  g->SetMarkerColor(fColours[i]);
  g->SetMarkerStyle(7);
  g->SetEditable(0);

}

// Series of functions to take care of the TGraphs
void WCSimEvDisplay::CalculateChargeAndTimeBins(){
  // Firstly, clear the existing vectors
  fChargeBins.clear();
  fTimeBins.clear();
  
  double deltaQ = (fQMax - fQMin) / 10.;
  double deltaT = (fTMax - fTMin) / 10.;

  for(int i = 0; i < 10; ++i){
    fChargeBins.push_back(fQMin+i*deltaQ);
    fTimeBins.push_back(fTMin+i*deltaT);
  }
}

unsigned int WCSimEvDisplay::GetChargeBin(double charge) const{
  unsigned int bin = 9;
  for(unsigned int i = 1; i < fChargeBins.size(); ++i){
    if(charge < fChargeBins[i]){
      bin = i - 1;
      break;
    }
  }
  return bin;
}

unsigned int WCSimEvDisplay::GetTimeBin(double time) const{
  unsigned int bin = 9;
  for(unsigned int i = 0; i < fTimeBins.size(); ++i){
    if(time < fTimeBins[i]){
      bin = i - 1;
      break;
    }
  }
  return bin;
}

void WCSimEvDisplay::MakeGraphColours(){

  // Make a palette
  fColours[0] = TColor::GetColor("#330000");
  fColours[1] = TColor::GetColor("#660000");
  fColours[2] = TColor::GetColor("#990000");
  fColours[3] = TColor::GetColor("#CC0000");
  fColours[4] = TColor::GetColor("#FF0000");
  fColours[5] = TColor::GetColor("#FF3300");
  fColours[6] = TColor::GetColor("#FF6600");
  fColours[7] = TColor::GetColor("#FF8800");
  fColours[8] = TColor::GetColor("#FFAA00");
  fColours[9] = TColor::GetColor("#FFCC00");
  
}

void WCSimEvDisplay::ResetGraphs(){

  TList *listTop = fTopPad->GetListOfPrimitives();
  TList *listBarrel = fBarrelPad->GetListOfPrimitives();
  TList *listBottom = fBottomPad->GetListOfPrimitives();
  
  for(unsigned int i = 0; i < fTopGraphs.size(); ++i){
    // Firstly, remove the graphs from the pads
    if(listTop->FindObject(fTopGraphs[i])){
      listTop->Remove(fTopGraphs[i]);
    }
    if(listBarrel->FindObject(fBarrelGraphs[i])){
      listBarrel->Remove(fBarrelGraphs[i]);
    }
    if(listBottom->FindObject(fBottomGraphs[i])){
      listBottom->Remove(fBottomGraphs[i]);
    }

    // Now reset the plots
    fTopGraphs[i]->Set(0);
    fBottomGraphs[i]->Set(0);
    fBarrelGraphs[i]->Set(0);
  }
}

void WCSimEvDisplay::DrawHitGraphs(std::vector<TGraph*> vec){
  for(unsigned int i = 0; i < vec.size(); ++i){
    if(vec[i]->GetN() > 0){
      vec[i]->Draw("P");
    }
  }
}

// Switch the z-axis scale to show charge.
void WCSimEvDisplay::SetViewCharge(){
	if(fViewType != 0){
		fViewType = 0;
		std::cout << "Setting colour axis to charge" << std::endl;
		this->FillPlotsFromWCSimEvent();
	}
	else{
		std::cout << "Already viewing charge." << std::endl;
	}
}

// Switch the z-axis scale to show time.
void WCSimEvDisplay::SetViewTime(){
  if(fViewType != 1){
		fViewType = 1;
		std::cout << "Setting colour axis to time" << std::endl;
		this->FillPlotsFromWCSimEvent();
	}
	else{
		std::cout << "Already viewing time." << std::endl;
	}
}

// Update the charge cut
void WCSimEvDisplay::SetChargeCut(){

  // Update the charge cut
  fChargeCut = fPEInput->GetNumberEntry()->GetNumber();
  // Redraw the event
  this->FillPlotsFromWCSimEvent();

}

// Show or hide the 1D plots.
void WCSimEvDisplay::Toggle1DHists(){
	// Are the plots currently visible?
	if(fShow1DHists == 1){
		// Need to hide the plots.
		fShow1DHists = 0;
		std::cout << "Hiding 1D plots" << std::endl;
	}
	else{
		// Need to show the plots.
		fShow1DHists = 1;
		std::cout << "Showing 1D plots" << std::endl;
	}
	this->ResizePads();
}
void WCSimEvDisplay::ClearPlots(){

	fBarrelHist->Reset();
	fTopHist->Reset();
	fBottomHist->Reset();
	fChargeHist->Reset();
	fTimeHist->Reset();

}

void WCSimEvDisplay::MakePlotsPretty(TH1* h){
	h->GetXaxis()->SetNdivisions(507);
	h->GetYaxis()->SetNdivisions(507);
}

// Function mostly to account for the offset in the time origin. In
// WCSim this offset is around 950ns.
void WCSimEvDisplay::GetMinColourAxis(TH2D* h){
	h->SetMinimum(0);
	double min = 1e6;
	// Find the minimum value that isn't zero
	for(int x = 1; x <= h->GetNbinsX(); ++x){
		for(int y = 1; y <= h->GetNbinsY(); ++y){
			double temp = h->GetBinContent(x,y);
			if(temp == 0.0) continue;
			if(temp < min){
				min = temp;
			}
		}
	}
	// Only bother to change the z minumum if we aren't close to 0 anyway.
	if(min > 100){
		h->SetMinimum(0.95*min);
	}
}

void WCSimEvDisplay::SetPlotZAxes(){

  double min = fQMin;
  double max = fQMax;

  if(fViewType == 1){
    min = fTMin;
    max = fTMax;
  }

	// Make sure the histogram max / min are correct
	fBarrelHist->SetMaximum(max);
	fBarrelHist->SetMinimum(min);
	fTopHist->SetMaximum(max);
	fTopHist->SetMinimum(min);
	fBottomHist->SetMaximum(max);
	fBottomHist->SetMinimum(min);

}

// Resize the pads when hiding / showing the 1D plots
void WCSimEvDisplay::ResizePads(){

  // Get list of objects attached to the main canvas
  TList *list = fHitMapCanvas->GetCanvas()->GetListOfPrimitives();

  // UpdateCanvases draw the pads we want, so remove them all here
  if(list->FindObject(fTruthPad)) list->Remove(fTruthPad);
 	if(list->FindObject(fTruthOverlayPad)) list->Remove(fTruthOverlayPad);   
  if(list->FindObject(fBarrelPad)) list->Remove(fBarrelPad);
  if(list->FindObject(fTopPad)) list->Remove(fTopPad);
  if(list->FindObject(fBottomPad)) list->Remove(fBottomPad);
  if(list->FindObject(fChargePad)) list->Remove(fChargePad);
  if(list->FindObject(fTimePad)) list->Remove(fTimePad); 

  // If we want to show truth
  if(fWhichPads == 1){
    fTruthPad->SetPad(0.0,0.0,1.0,1.0);
  }
  // Or else show the reco
  else if (fWhichPads == 0){
  	// Resize the reco pads
  	if(fShow1DHists){
  		fBarrelPad->SetPad(0.0,0.6,1.0,1.0);
  		fTopPad->SetPad(0.0,0.2,0.5,0.6);
  		fBottomPad->SetPad(0.5,0.2,1.0,0.6);
  		fChargePad->SetPad(0.0,0.0,0.5,0.2);
  		fTimePad->SetPad(0.5,0.0,1.0,0.2);
  	}
  	else{
  		// Resize the reco pads
      fBarrelPad->SetPad(0.0,0.5,1.0,1.0);
      fTopPad->SetPad(0.0,0.0,0.5,0.5);
      fBottomPad->SetPad(0.5,0.0,1.0,0.5);
  	}
  }
  // Else show the truth overlays
  else{
    // Make sure to leave space for the truth overlay pad at the bottom
    fBarrelPad->SetPad(0.0,0.6,1.0,1.0);
  	fTopPad->SetPad(0.0,0.2,0.5,0.6);
  	fBottomPad->SetPad(0.5,0.2,1.0,0.6);
    fTruthOverlayPad->SetPad(0.0,0.0,1.0,0.2); 
  }

	this->UpdateCanvases();
}

void WCSimEvDisplay::UpdateCanvases(){

  TCanvas *canvas = fHitMapCanvas->GetCanvas();
  canvas->cd();

  // Remove the truth overlays
  this->HideTruthOverlays();
  
  if(fWhichPads == 0){
    // Now draw the pads
    fBarrelPad->Draw();
    fTopPad->Draw();
    fBottomPad->Draw();
  	if(fShow1DHists){
  		fChargePad->Draw();
      fTimePad->Draw();
  	}
  }
  else if (fWhichPads == 1){
    fTruthPad->Draw();
  }
  else{
    this->DrawTruthOverlays();
    canvas->cd(); // Need to cd back here since the above changes directory
    fBarrelPad->Draw();
    fTopPad->Draw();
    fBottomPad->Draw();
    fTruthOverlayPad->Draw();  
  }
 	canvas->Modified();
 	canvas->Update();
}

// Draw the reco plots to the reco pads
void WCSimEvDisplay::UpdateRecoPads(){

	this->SetPlotZAxes();
  // Set the styles how we want them
  this->MakePlotsPretty(fBarrelHist);
  this->MakePlotsPretty(fTopHist);
  this->MakePlotsPretty(fBottomHist);
  this->MakePlotsPretty(fChargeHist);
  this->MakePlotsPretty(fTimeHist);

  // Take the plots one by one and draw them.
  fBarrelPad->cd();
  fBarrelHist->Draw("colz");
  this->DrawHitGraphs(fBarrelGraphs);
  fBarrelPad->Modified();
  fBarrelPad->Update();

  fTopPad->cd();
  fTopHist->Draw("colz");
  this->DrawHitGraphs(fTopGraphs);
  fTopPad->Modified();
  fTopPad->Update();

  fBottomPad->cd();
  fBottomHist->Draw("colz");
  this->DrawHitGraphs(fBottomGraphs);
  fBottomPad->Modified();
  fBottomPad->Update();

  fChargePad->cd();
  fChargeHist->Draw();
  fChargePad->Modified();
  fChargePad->Update();

  fTimePad->cd();
  fTimeHist->Draw();
  fTimePad->Modified();
  fTimePad->Update();

  fHitMapCanvas->GetCanvas()->cd();
}

// Draw the truth information to the truth pad
void WCSimEvDisplay::UpdateTruthPad(){
  fTruthPad->cd();
  fTruthTextMain->Draw();
  fTruthTextPrimaries->Draw();
  
  fHitMapCanvas->GetCanvas()->cd();
}

// Draw the truth overlay information
void WCSimEvDisplay::UpdateTruthOverlayPad(){

  fTruthOverlayPad->cd();
  // Draw the TLegend
  if(fTruthLegend){
    fTruthLegend->Draw();
  }
  else{
    std::cout << "No truth rings found" << std::endl;
  }

  fHitMapCanvas->GetCanvas()->cd();
}

// Actually draw the Truth Overlays
void WCSimEvDisplay::DrawTruthOverlays(){

  	// Take the plots one by one and draw them.
    fBarrelPad->cd();
  	fBarrelHist->Draw("colz");
    this->DrawHitGraphs(fBarrelGraphs);
    // Draw the truth rings
    for(unsigned int r = 0; r < fTruthMarkersBarrel.size(); ++r){
      fTruthMarkersBarrel[r]->Draw("C");      
    }
    fBarrelPad->Modified();
    fBarrelPad->Update();

  	fTopPad->cd();
  	fTopHist->Draw("colz");
    this->DrawHitGraphs(fTopGraphs);
    // Draw the truth rings
    for(unsigned int r = 0; r < fTruthMarkersTop.size(); ++r){
      fTruthMarkersTop[r]->Draw("C");
    }
    fTopPad->Modified();
    fTopPad->Update();

  	fBottomPad->cd();
  	fBottomHist->Draw("colz");
    this->DrawHitGraphs(fBottomGraphs);
    // Draw the truth rings
    for(unsigned int r = 0; r < fTruthMarkersBottom.size(); ++r){
      fTruthMarkersBottom[r]->Draw("C");
    }
    fBottomPad->Modified();
    fBottomPad->Update();

}

void WCSimEvDisplay::HideTruthOverlays(){
  TList *list = fBarrelPad->GetListOfPrimitives();
  for(unsigned int r = 0; r < fTruthMarkersBarrel.size(); ++r){
    if(list->FindObject(fTruthMarkersBarrel[r])){
      list->Remove(fTruthMarkersBarrel[r]);
    }
  }

  list = fTopPad->GetListOfPrimitives();
  for(unsigned int r = 0; r < fTruthMarkersTop.size(); ++r){
    if(list->FindObject(fTruthMarkersTop[r])){
      list->Remove(fTruthMarkersTop[r]);
    }
  }
  
  list = fBottomPad->GetListOfPrimitives();
  for(unsigned int r = 0; r < fTruthMarkersBottom.size(); ++r){
    if(list->FindObject(fTruthMarkersBottom[r])){
      list->Remove(fTruthMarkersBottom[r]);
    }
  }
}

void WCSimEvDisplay::NextEvent() {
	if(fChain->GetEntries() > 0){
		if(fCurrentEvent < fMaxEvent){
			++fCurrentEvent;
			std::cout << "Moving to event " << fCurrentEvent << std::endl;
			this->FillPlotsFromWCSimEvent();
      fEventInput->GetNumberEntry()->SetNumber(fCurrentEvent);
		}
		else{
			std::cout << "Already at the final event" << std::endl;
		}
	}
	else{
		std::cout << "Can't change event without a file loaded!" << std::endl;
	}
}

void WCSimEvDisplay::PrevEvent(){

	if(fChain->GetEntries() > 0){
		if(fCurrentEvent > fMinEvent){
			--fCurrentEvent;
			std::cout << "Moving to event " << fCurrentEvent << std::endl;
			this->FillPlotsFromWCSimEvent();
      fEventInput->GetNumberEntry()->SetNumber(fCurrentEvent);
		}
		else{
			std::cout << "Already at the first event" << std::endl;
		}
	}
	else{
		std::cout << "Can't change event without a file loaded!" << std::endl;
	}
}

void WCSimEvDisplay::SetEvent(){
	if(fChain->GetEntries() > 0){
    int newEvt = (int)fEventInput->GetNumberEntry()->GetNumber();
    if(newEvt == fCurrentEvent){
      std::cout << "Already displaying event " << fCurrentEvent << std::endl;
    }
    else if(newEvt >= fMinEvent && newEvt <= fMaxEvent){
      fCurrentEvent = newEvt;
      this->FillPlotsFromWCSimEvent();
    }
    else{
      std::cout << "Event number " << newEvt << " is out of range" << std::endl;
    }
	}
	else{
		std::cout << "Can't change event without a file loaded!" << std::endl;
	}
}

void WCSimEvDisplay::SetInputFile(std::string name){
	this->OpenFile(name);
}

void WCSimEvDisplay::OpenFile(){
	this->OpenFile("");
}

void WCSimEvDisplay::OpenFile(std::string name){

	TGFileInfo fileInfo;
	const char *filetypes[] = {"ROOT files", "*.root", 0, 0};
	fileInfo.fFileTypes = filetypes;
	fileInfo.fIniDir = StrDup(".");

	// Open the browser if no file is supplied.
	if(name == ""){
		new TGFileDialog(gClient->GetDefaultRoot(), this, kFDOpen, &fileInfo);
		if(fileInfo.fFilename){
			name = fileInfo.fFilename;
		}
		std::cout << "'" << name << "' selected." << std::endl;
	}
	
	if(name != ""){
		// Quick test to see if the file contains what we need
		TFile tempFile(name.c_str(),"READ");
    if(tempFile.Get("wcsimT")){
			this->OpenWCSimFile(name);
		}
		else{
			std::cout << "Selected file is not a WCSim file." << std::endl;
		}
		tempFile.Close();
	}
}

void WCSimEvDisplay::OpenWCSimFile(std::string name){

	fFileType = 0; // We have a WCSim file
	// Check what we should be showing.
	this->ResizePads();
  // Show the WCSim buttons if they are not visible.
  if(!this->IsVisible(hWCSimButtons)){
    this->ShowFrame(hWCSimButtons);
  }

	// Sort the main chain first
	if(fChain != 0x0){
	  delete fChain;
	}
	fChain = new TChain("wcsimT");
	fChain->Reset();
	fChain->Add(name.c_str());
	// Now the geometry
	if(fGeomTree != 0x0){
	  delete fGeomTree;
	}
	fGeomTree = new TChain("wcsimGeoT");
	fGeomTree->Reset();
	fGeomTree->Add(name.c_str());
	this->ResizePlotsFromGeometry();
	// Each entry is an event, so use this to set the limits.
	fMinEvent = 0;
	fCurrentEvent = 0;
	fMaxEvent = fChain->GetEntries() - 1;
	this->FillPlotsFromWCSimEvent();

}

void WCSimEvDisplay::SaveEvent(){
	if(fFileType!=-1){
		TGFileInfo fileInfo;
		const char *filetypes[] = {"All Files", "*", 0, 0};
		fileInfo.fFileTypes = filetypes;
		fileInfo.fIniDir = StrDup(".");
		new TGFileDialog(gClient->GetDefaultRoot(), this, kFDSave, &fileInfo);
		if( fileInfo.fFilename ){
			// Save the plot
			TCanvas *can = fHitMapCanvas->GetCanvas();
			can->Print(fileInfo.fFilename);
		}
	}
	else{
		std::cout << "Need to open a file to save a plot." << std::endl;
	}
}

void WCSimEvDisplay::CloseWindow(){
	std::cout << "Closing Event Display" << std::endl;
	gApplication->Terminate(0);
}

void WCSimEvDisplay::SetStyle(){

	gStyle->SetOptStat(0000);

	// Make sure all backgrounds are white
	gStyle->SetFillColor(10);
	gStyle->SetFrameFillColor(10);
	gStyle->SetCanvasColor(10);
	gStyle->SetPadColor(10);
	gStyle->SetTitleFillColor(10);
	gStyle->SetStatColor(10);

	// Use nice fonts
	const int kMinosFont = 42;
//	const int kMinosFont = 40;

	gStyle->SetStatFont(kMinosFont);
	gStyle->SetLabelFont(kMinosFont,"xyz");
	gStyle->SetTitleFont(kMinosFont,"xyz");
	gStyle->SetTitleFont(kMinosFont,"");
	gStyle->SetTextFont(kMinosFont);

//	gROOT->SetStyle("eds");
	gROOT->ForceStyle();

}

void WCSimEvDisplay::ShowTruth(){

  if(fTruthSummary != 0x0){

    if(fWhichPads != 1){

      fWhichPads = 1;
  
      std::cout << "== Event Truth Information ==" << std::endl;
  
      if(fTruthSummary->IsNeutrinoEvent()){
        std::string intType = this->ConvertTrueEventType();
        TVector3 vtx = fTruthSummary->GetVertex();
        TVector3 dir = fTruthSummary->GetBeamDir();      
        // The interaction
        std::cout << "= Interaction = " << std::endl;
        std::cout << " - Type: " << intType << std::endl;
        std::cout << " - Neutrino flavour  : " << fTruthSummary->GetBeamPDG() << std::endl;
        std::cout << " - Neutrino energy   : " << fTruthSummary->GetBeamEnergy() << std::endl;
        std::cout << " - Neutrino vertex   : (" << vtx.X() << "," << vtx.Y() << "," << vtx.Z() << ")" << std::endl;
        std::cout << " - Neutrino direction: (" << dir.X() << "," << dir.Y() << "," << dir.Z() << ")" << std::endl;
        std::cout << " - Target nucleus    : " << fTruthSummary->GetTargetPDG() << std::endl;
        // Primaries
        std::cout << "= Primary Particles = " << std::endl;
        for(unsigned int n = 0; n < fTruthSummary->GetNPrimaries(); ++n){
          TVector3 dir = fTruthSummary->GetPrimaryDir(n);
          std::cout << " - Particle: " << fTruthSummary->GetPrimaryPDG(n);
          std::cout << " with energy " << fTruthSummary->GetPrimaryEnergy(n);
          std::cout << " MeV and direction (" << dir.X() << "," << dir.Y() << "," << dir.Z() << ")" << std::endl;
        }     
      }
      else{
        // Must have some sort of particle gun event
        TVector3 vtx = fTruthSummary->GetVertex();
        TVector3 dir = fTruthSummary->GetBeamDir();
  
        std::cout << "= Particle Gun Information =" << std::endl;
        std::cout << " - Particle : " << fTruthSummary->GetBeamPDG() << std::endl;
        std::cout << " - Energy   : " << fTruthSummary->GetBeamEnergy() << std::endl;
        std::cout << " - Vertex   : (" << vtx.X() << "," << vtx.Y() << "," << vtx.Z() << ")" << std::endl;
        std::cout << " - Direction: (" << dir.X() << "," << dir.Y() << "," << dir.Z() << ")" << std::endl;
      }
      this->ResizePads();
    }
    else{
      std::cerr << "Already displaying the truth information" << std::endl;
    }
  }
  else{
    std::cerr << "No truth information found, so can't display it" << std::endl;
  }
}

void WCSimEvDisplay::ShowReco(){
  if(fWhichPads != 0){
    fWhichPads = 0;
    this->ResizePads();
  }
  else{
    std::cerr << "Already displaying reco view." << std::endl;
  }
}

void WCSimEvDisplay::ShowTruthOverlay(){
  if(fWhichPads != 2){
    fWhichPads = 2;
    this->ResizePads();
  }
  else{
    std::cerr << "Already displaying truth overlays" << std::endl;
  }
}

void WCSimEvDisplay::UpdateTruthPave(){
  fTruthTextMain->Clear();
  fTruthTextMain->SetFillColor(kWhite);
  fTruthTextMain->SetBorderSize(0);

  fTruthTextPrimaries->Clear();
  fTruthTextPrimaries->SetFillColor(kWhite);
  fTruthTextPrimaries->SetBorderSize(0);

  // Clear truth ring vectors
  this->ClearTruthMarkerVectors();

  // Stream to parse things into strings
  std::stringstream tmpS;
  TVector3 vtx = fTruthSummary->GetVertex();
  tmpS << vtx.X() << "," << vtx.Y() << "," << vtx.Z();
  fTruthTextMain->AddText(("Vertex at ("+tmpS.str()+") mm").c_str());
  if(fTruthSummary->IsNeutrinoEvent()){
    // Neutrino information
    tmpS.str("");
    tmpS << this->ConvertTrueEventType();
    fTruthTextMain->AddText(("Interaction Type = "+tmpS.str()).c_str());
    fTruthTextMain->AddText("");
    fTruthTextMain->AddText("Neutrino Information");
  }
  else{
    tmpS.str("");
    fTruthTextMain->AddText("Beam Information");
  }
  tmpS.str("");
  tmpS << this->GetParticleName(fTruthSummary->GetBeamPDG());
  fTruthTextMain->AddText(tmpS.str().c_str());
  tmpS.str("");
  tmpS << fTruthSummary->GetBeamEnergy();
  fTruthTextMain->AddText(("Energy = "+tmpS.str()+" MeV").c_str());
  TVector3 dir = fTruthSummary->GetBeamDir();
  tmpS.str("");
  tmpS << dir.X() << "," << dir.Y() << "," << dir.Z();
  fTruthTextMain->AddText(("Direction = ("+tmpS.str()+")").c_str());
  if(fTruthSummary->IsNeutrinoEvent()){
    // Target information
    fTruthTextMain->AddText("");
    fTruthTextMain->AddText("Target Information");
    tmpS.str("");
    tmpS << fTruthSummary->GetTargetPDG();
    fTruthTextMain->AddText(("PDG Code = "+tmpS.str()).c_str());
  }

  int nTruthRings = 0;
  // Create the TLegend for the truth overlays
  if(fTruthLegend != 0x0){
    // Remove it from the pad
    if(fTruthOverlayPad->GetListOfPrimitives()->FindObject(fTruthLegend)){
      fTruthOverlayPad->GetListOfPrimitives()->Remove(fTruthLegend);
    }
    delete fTruthLegend;
    fTruthLegend = 0x0;
  }
  fTruthLegend = new TLegend(0.2,0.2,0.8,0.8);
  fTruthLegend->SetFillColor(kWhite);
  fTruthLegend->SetBorderSize(0);
  
  // The primary particles list
  if(fTruthSummary->IsNeutrinoEvent()){
    fTruthTextPrimaries->AddText("List of Primary Particles (*** above Cherenkov threshold)");
    for(unsigned int n = 0; n < fTruthSummary->GetNPrimaries(); ++n){
      int pdg = fTruthSummary->GetPrimaryPDG(n);
      double energy = fTruthSummary->GetPrimaryEnergy(n);
      std::string mod = "";
      if(this->IsAboveCherenkovThreshold(pdg,energy)){
        mod = "***";
        // Add a true ring to the display
        ++nTruthRings; 
        int ringColour = this->GetTruthRingColour(nTruthRings);
        this->DrawTruthRing(n,ringColour);
      }
      dir = fTruthSummary->GetPrimaryDir(n);
      tmpS.str("");
      tmpS << mod << " ";
//      tmpS << "Particle: " << pdg;
      tmpS << this->GetParticleName(pdg);
      tmpS << " with energy " << energy;
      tmpS << " MeV and direction (" << dir.X() << "," << dir.Y() << "," << dir.Z() << ")";
      tmpS << " " << mod;
      fTruthTextPrimaries->AddText(tmpS.str().c_str());
    } 
  }
  else{
    // Draw the truth ring for the particle gun
    int pdg = fTruthSummary->GetBeamPDG();
    double energy = fTruthSummary->GetBeamEnergy();
    if(this->IsAboveCherenkovThreshold(pdg,energy)){
      this->DrawTruthRing(9999,this->GetTruthRingColour(1)); // Dummy value to flag it isn't a primary particle
    }
  }
}

bool WCSimEvDisplay::IsAboveCherenkovThreshold(int pdg, double energy){

  if(fDatabasePDG == 0x0){
    fDatabasePDG = new TDatabasePDG();
  }

  TParticlePDG* particle = fDatabasePDG->GetParticle(pdg);
  if(!particle) return false;

  double charge = particle->Charge(); // Charge in |e| / 3
  double mass = particle->Mass() * 1000.; // Mass in MeV
  double threshold = 1e6;

  // See if the particle is charged
  if(charge != 0.0){
    
    // Cherenkov threshold happens at cos_theta = 1/(n*beta) = 1
    // -> p/E = 1/n
    // -> E = sqrt (m*m / (1-1/(n*n)) ) where n = 1.33 for water
    double refracWater = 1.33;
    threshold = sqrt((mass*mass)/(1-(1/(refracWater*refracWater))));

  }
  else{
    // Photons cause electromagnetic showers that create Cherenkov light...
    // Set limit for a few pair productions?
    if(pdg == 22){
      threshold = 20 * 0.511;
    }
    // Don't set a threshold for pi-zeroes. We can set one on the photons later
    // if it is deemed neccessary
    if(pdg == 111){
      threshold = mass;
    }
  }

  if(energy > threshold){
    return true;
  }
  
  return false;
  
}

std::string WCSimEvDisplay::ConvertTrueEventType() const{

  std::string type = "";

  if(fTruthSummary != 0x0){
    // QE events
    if(fTruthSummary->IsCCEvent() && fTruthSummary->IsQEEvent()) type = "CC QE";
    if(fTruthSummary->IsNCEvent() && fTruthSummary->IsQEEvent()) type = "NC QE";
    // Resonant events
    if(fTruthSummary->IsCCEvent() && fTruthSummary->IsResEvent()) type = "CC Res";
    if(fTruthSummary->IsNCEvent() && fTruthSummary->IsResEvent()) type = "NC Res";
    // DIS events
    if(fTruthSummary->IsCCEvent() && fTruthSummary->IsDISEvent()) type = "CC DIS";
    if(fTruthSummary->IsNCEvent() && fTruthSummary->IsDISEvent()) type = "NC DIS";
    // Coherent events
    if(fTruthSummary->IsCCEvent() && fTruthSummary->IsCohEvent()) type = "CC Coherent";
    if(fTruthSummary->IsNCEvent() && fTruthSummary->IsCohEvent()) type = "NC Coherent";
    // Others
    if(fTruthSummary->IsNueElectronElasticEvent()) type = "Nue Elastic";
    if(fTruthSummary->IsInverseMuonDecayEvent()) type = "Inverse Muon Decay";
    // If the interaction mode is 0, we likely have have a complex resonant event.
    // Look at the primaries and look for a lepton to decide CC or NC
    if(fTruthSummary->GetInteractionMode() == WCSimTruthSummary::kOther){

      bool isCC = false;

      for(unsigned int n = 0; n < fTruthSummary->GetNPrimaries(); ++n){
        // Neutrinos
        if(fTruthSummary->IsNuEEvent() && fTruthSummary->GetPrimaryPDG(n) == 11) isCC = true;
        if(fTruthSummary->IsNuMuEvent() && fTruthSummary->GetPrimaryPDG(n) == 13) isCC = true;
        if(fTruthSummary->IsNuTauEvent() && fTruthSummary->GetPrimaryPDG(n) == 15) isCC = true;
        // Antineutrinos
        if(fTruthSummary->IsNuEBarEvent() && fTruthSummary->GetPrimaryPDG(n) == -11) isCC = true;
        if(fTruthSummary->IsNuMuBarEvent() && fTruthSummary->GetPrimaryPDG(n) == -13) isCC = true;
        if(fTruthSummary->IsNuTauBarEvent() && fTruthSummary->GetPrimaryPDG(n) == -15) isCC = true;
        // Stop if we find the correct lepton
        if(isCC) break;
      }
      
      if(isCC) type = "CC Res";
      else type = "NC Res";

    }

  }  

  return type;
}

std::string WCSimEvDisplay::GetParticleName(int pdgCode){

  std::string pName = "";

  // Get the database if we don't already have one
  if(fDatabasePDG == 0x0){
    fDatabasePDG = new TDatabasePDG();
  }
  
  if(pdgCode > 9999){
    pName = "nucleus";
  }
  else{
    pName = fDatabasePDG->GetParticle(pdgCode)->GetName();
  }

  return pName;

}

WCSimEvDisplay::~WCSimEvDisplay() {
  // Clean up used widgets: frames, buttons, layouthints
  this->Cleanup();

	if(fBarrelHist) delete fBarrelHist;
	if(fTopHist) delete fTopHist;
	if(fBottomHist) delete fBottomHist;
	if(fChargeHist) delete fChargeHist;
	if(fTimeHist) delete fTimeHist;
}

void WCSimEvDisplay::ResizePlotsFromGeometry(){

	WCSimRootGeom *geo = new WCSimRootGeom();
	fGeomTree->SetBranchAddress("wcsimrootgeom",&geo);
	// Only a single entry
	fGeomTree->GetEntry(0);

	fWCRadius = geo->GetWCCylRadius();
	fWCLength = geo->GetWCCylLength();
	fPMTRadius = geo->GetWCPMTRadius();
	std::cout << "== Geometry information: " << std::endl;
	std::cout << "- Radius: " << fWCRadius << std::endl;
	std::cout << "- Height: " << fWCLength << std::endl;
	std::cout << "- NumPMT: " << geo->GetWCNumPMT() << std::endl;	

	// Count number of PMTs in each section.
	fPMTTop = 0;
	fPMTBarrel = 0;
	fPMTBottom = 0;
	// Also use this loop to find unique positions to count:
	// Number of PMTs in barrel height, number round the barrel
	// Number across the top and bottom.
	std::vector<double> xPos;
	std::vector<double> yPos;
	std::vector<double> zPos;
	std::vector<double> phiPos;
	for(int p = 0; p < geo->GetWCNumPMT(); ++p){
		WCSimRootPMT pmt = geo->GetPMT(p);
		if(pmt.GetCylLoc() == 0){
			++fPMTTop;
			if(std::find(xPos.begin(),xPos.end(),pmt.GetPosition(0)) == xPos.end()){
				xPos.push_back(pmt.GetPosition(0));
			}
			if(std::find(yPos.begin(),yPos.end(),pmt.GetPosition(1)) == yPos.end()){
				yPos.push_back(pmt.GetPosition(1));
			}
		}
		else if(pmt.GetCylLoc() == 1){
			++fPMTBarrel;
			double tempZ = pmt.GetPosition(2);
			if(fabs(tempZ) < 1e-10){ 
				tempZ = 0.0;
			}
			if(std::find(zPos.begin(),zPos.end(),tempZ) == zPos.end()){
				zPos.push_back(tempZ);
			}
			double tempPhi = TMath::ATan2(pmt.GetPosition(1),pmt.GetPosition(0));
			if(std::find(phiPos.begin(),phiPos.end(),tempPhi) == phiPos.end()){
				phiPos.push_back(tempPhi);
//				std::cout << "UNIQUE PHIPOS: " << tempPhi << std::endl;
			}
		}
		else{
			++fPMTBottom;
		}
	}

	std::cout << "\t- Barrel : " << fPMTBarrel << std::endl;
	std::cout << "\t- Top    : " << fPMTTop << std::endl;
	std::cout << "\t- Bottom : " << fPMTBottom << std::endl;

	std::cout << xPos.size() << ", " << yPos.size() << ", " << zPos.size() << ", " << phiPos.size() << std::endl;

	// How many bins do we need?
	// For x and y, round up sqrt of number of Top PMTs
	int nBinsX = (int)xPos.size();
	int nBinsY = (int)yPos.size();
	int nBinsZ = (int)zPos.size(); 
	int nBinsPhi = (int)phiPos.size();

  // Now with phi, sort the vector then make a variably binned array from it

	double phiMin = TMath::Pi()* -1 + (TMath::Pi()/(double)phiPos.size());
	double phiMax = TMath::Pi() + (TMath::Pi()/(double)phiPos.size());

	double xMin = -geo->GetWCCylRadius();
	double xMax = geo->GetWCCylRadius();
	double yMin = -geo->GetWCCylRadius();
	double yMax = geo->GetWCCylRadius();
	double zMin = -0.5*geo->GetWCCylLength();
	double zMax = 0.5*geo->GetWCCylLength();

	if(fBarrelHist){
		delete fBarrelHist;
	}
	fBarrelHist = new TH2D("barrelHist","Barrel;#phi = atan(y/x);z/cm",nBinsPhi,phiMin,phiMax,nBinsZ,zMin,zMax);
	fBarrelHist->SetDirectory(0);
	if(fTopHist){
		delete fTopHist;
	}
	fTopHist = new TH2D("topHist","Top Cap;y/cm;x/cm",nBinsX,xMin,xMax,nBinsY,yMin,yMax);
	fTopHist->SetDirectory(0);
	if(fBottomHist){
		delete fBottomHist;
	}
	fBottomHist = new TH2D("BottomHist","Bottom Cap;y/cm;x/cm",nBinsX,xMin,xMax,nBinsY,yMin,yMax);
	fBottomHist->SetDirectory(0);
	if(fChargeHist){
		delete fChargeHist;
	}
	fChargeHist = new TH1D("chargeHist","Charge;Charge / PE",100,0,25);	
	fChargeHist->SetDirectory(0);
	if(fTimeHist){
		delete fTimeHist;
	}
	fTimeHist = new TH1D("timeHist","Time;Time / ns",100,800,1200);	
	fTimeHist->SetDirectory(0);
	// Clean up.
	delete geo;
	geo = 0x0;
}

void WCSimEvDisplay::MakeDefaultPlots(){
	fBarrelHist = new TH2D("barrelHist","Barrel;#phi = atan(y/x);z/cm",1,0,1,1,0,1);
	fTopHist = new TH2D("topHist","Top Cap;y/cm;x/cm",1,0,1,1,0,1);
	fBottomHist = new TH2D("BottomHist","Bottom Cap;y/cm;x/cm",1,0,1,1,0,1);
	fChargeHist = new TH1D("chargeHist","Charge;Charge / PE",1,0,1);	
	fTimeHist = new TH1D("timeHist","Time;Time / ns",1,0,1);	
}

// Draw the truth ring corresponding to primary particle number particleNo
void WCSimEvDisplay::DrawTruthRing(unsigned int particleNo, int colour){

  // Does the truth information exist
  if(fTruthSummary==0x0) return; 

  // Get the track vertex and direction
  TVector3 trkVtx = fTruthSummary->GetVertex();
  trkVtx = trkVtx * 0.1; // Convert to cm
  TVector3 trkDir;
  if(particleNo==9999){
    trkDir = fTruthSummary->GetBeamDir();
  }
  else{
    trkDir = fTruthSummary->GetPrimaryDir(particleNo);
  }
  // Get the projection of the track vertex onto the wall
  TVector3 trkVtxProj; // Point where the track would hit the wall, filled by ProjectToWall
  unsigned int detRegion; // Region of the detector, filled by ProjectToWall 
  this->ProjectToWall(trkVtx,trkDir,trkVtxProj, detRegion);

  // Now that we have the projected point on the wall, iterate around the Cherenkov cone and
  // find where all the points of the cone intercept the wall
  unsigned int nMarkers = 360; // Number of points that will appear on the final plots
  double dPhi = 360 / (double)nMarkers; // Angle step around the direction vector

  // Get the particle mass
  if(fDatabasePDG == 0x0){
    fDatabasePDG = new TDatabasePDG();
  }
  int pdgCode;
  double en;
  if(particleNo==9999){
    pdgCode = fTruthSummary->GetBeamPDG();
    en = fTruthSummary->GetBeamEnergy();
  }
  else{
    pdgCode = fTruthSummary->GetPrimaryPDG(particleNo);
    en = fTruthSummary->GetPrimaryEnergy(particleNo);
  }
  double mass = 1000*fDatabasePDG->GetParticle(pdgCode)->Mass();
  double beta = sqrt(en*en - mass*mass) / en; // beta = p / E
  double refrac = 1.33; // Refractive index of water
  double thetaC = (180.0 / TMath::Pi()) * TMath::ACos(1./(refrac*beta)); // Cherenkov angle


  // Also need 6 vectors to store the 2D-coordinates for the 3 regions
  std::vector<double> topPos1; // For top, this is the y coord
  std::vector<double> topPos2; // For top, this is the x coord
  std::vector<double> barrelPos1; // This is the phi coord
  std::vector<double> barrelPos2; // This is the z coord
  std::vector<double> bottomPos1; // This is the y coord
  std::vector<double> bottomPos2; // This is the x coord

  // Special case for pi0s
  // Need to make sure the Cherenkov angle is correct, and draw rings for both photons
  TVector3 trkDir2;
  TVector3 trkVtxProj2;
  if(pdgCode == 111){
    thetaC = (180.0 / TMath::Pi()) * TMath::ACos(1./(refrac)); // Beta = 1 for photons
    // Find the pi0 we want
    WCSimEvDispPi0* thisPi0 = fPi0s[GetPi0(en)];
    // Get the projection of the first photon onto the wall
    // Photon has the same vtx as pi0, so no need to update that.
    trkDir = thisPi0->GetPhotonDirection(1);
    this->ProjectToWall(trkVtx,trkDir,trkVtxProj, detRegion);
    // Now for photon two
    trkDir2 = thisPi0->GetPhotonDirection(2);
    this->ProjectToWall(trkVtx,trkDir2,trkVtxProj2,detRegion);
  }

  for(unsigned int n = 0; n < nMarkers; ++n){

    double phi = n * dPhi;

    // Find a point on the Cherenkov cone and its direction
    TVector3 circPos;
    TVector3 circDir;
    this->FindCircle(trkVtxProj,trkVtx,thetaC,phi,circPos,circDir);     

    // Now we have the point on the circle, project this onto the wall
    TVector3 finalPos;
    this->ProjectToWall(circPos,circDir,finalPos,detRegion);

    // Special case for pi0s... add another ring.
    TVector3 circPos2, circDir2, finalPos2;
    unsigned int detRegion2;
    if(pdgCode == 111){
      this->FindCircle(trkVtxProj2,trkVtx,thetaC,phi,circPos2,circDir2);
      this->ProjectToWall(circPos2,circDir2,finalPos2,detRegion2);
    }

    if(detRegion == 0){
      topPos1.push_back(finalPos.Y());
      topPos2.push_back(finalPos.X());
    }    
    else if(detRegion == 1){
      barrelPos1.push_back(TMath::ATan2(finalPos.Y(),finalPos.X()));
      barrelPos2.push_back(finalPos.Z());
    }
    else{
      bottomPos1.push_back(finalPos.Y());
      bottomPos2.push_back(finalPos.X());
    }
    // Special case for the pi-zero's second photon
    if(pdgCode == 111){
      if(detRegion2 == 0){
        topPos1.push_back(finalPos2.Y());
        topPos2.push_back(finalPos2.X());
      }    
      else if(detRegion2 == 1){
        barrelPos1.push_back(TMath::ATan2(finalPos2.Y(),finalPos2.X()));
        barrelPos2.push_back(finalPos2.Z());
      }
      else{
        bottomPos1.push_back(finalPos2.Y());
        bottomPos2.push_back(finalPos2.X());
      }
    }
  }

  // Now we have the vectors of coordinates that we need for each region. Now to make the TPolyMarkers
  // and add an entry to the legend
  std::stringstream legendText;
  legendText << this->GetParticleName(pdgCode) << " with total energy = " << en << " MeV";
  TLine* line = new TLine();
  line->SetLineColor(colour);
  fTruthLines.push_back(line);
  fTruthLegend->AddEntry(fTruthLines[fTruthLines.size()-1],legendText.str().c_str(),"l");
  if(topPos1.size() != 0){
    this->MakePolyMarker(topPos1,topPos2,fTruthMarkersTop,colour);
  }
  if(barrelPos1.size() != 0){
    this->MakePolyMarker(barrelPos1,barrelPos2,fTruthMarkersBarrel,colour);
  }
  if(bottomPos1.size() != 0){
    this->MakePolyMarker(bottomPos1,bottomPos2,fTruthMarkersBottom,colour);
  }
  
}

// Project the position and direction onto the detector wall, returning proj vector
// Adapted from the method in WCSimAnalysis' WCSimGeometry
void WCSimEvDisplay::ProjectToWall(TVector3 vtx, TVector3 dir, TVector3& proj, unsigned int& region){

  // Defaults
  proj = TVector3(-99999.9,-99999.9,-99999.9);
  region = 999;

  // Get the geometry information
  Double_t r = fWCRadius;
  Double_t L = fWCLength;

  Bool_t foundProjectionXY = 0;
  Bool_t foundProjectionZ = 0;

  Double_t t1 = 0.0;
  Double_t x1 = 0.0;
  Double_t y1 = 0.0;
  Double_t z1 = 0.0;
  Int_t region1 = -1;

  Double_t t2 = 0.0;  
  Double_t x2 = 0.0;
  Double_t y2 = 0.0;
  Double_t z2 = 0.0;
  Int_t region2 = -1;

  Double_t rSq = r*r;
  Double_t r0r0 = vtx.X()*vtx.X() + vtx.Y()*vtx.Y();
  Double_t r0p = vtx.X()*dir.X() + vtx.Y()*dir.Y();
  Double_t pSq = dir.X()*dir.X() + dir.Y()*dir.Y();
  
  // calculate intersection in XY
  if( pSq>0.0 ){
    if( r0p*r0p - pSq*(r0r0-rSq)>0.0 ){
      t1 = ( -r0p - sqrt(r0p*r0p-pSq*(r0r0-rSq)) ) / pSq;
      t2 = ( -r0p + sqrt(r0p*r0p-pSq*(r0r0-rSq)) ) / pSq;
      foundProjectionXY = 1;
    }
  }
  // propagation along z-axis
  else if( r0r0<=rSq ){

    if( dir.Z()>0 ){
      t1 = -L/2.0 - vtx.Z();
      t2 = +L/2.0 - vtx.Z();
    }
    else{
      t1 = -L/2.0 + vtx.Z();
      t2 = +L/2.0 + vtx.Z();
    }
    foundProjectionXY = 1;
  }
  
  // found intersection in XY
  if( foundProjectionXY ){

    z1 = vtx.Z() + t1*dir.Z();
    z2 = vtx.Z() + t2*dir.Z();

    if( ( z1>=-L/2.0 && z2<=+L/2.0 )
     || ( z2>=-L/2.0 && z1<=+L/2.0 ) ){
      foundProjectionZ = 1;
    }
  }

  // found intersection in Z
  if( foundProjectionZ ){

    // first intersection
    if( z1>-L/2.0 && z1<+L/2.0 ){
      region1 = 1;
    }
    if( z1>=+L/2.0 ){
      region1 = 0;
      if( z1>+L/2.0 ){
        z1 = +L/2.0; 
        t1 = (+L/2.0-vtx.Z())/dir.Z();
      }
    }
    if( z1<=-L/2.0 ){
      region1 = 2;
      if( z1<-L/2.0 ){
        z1 = -L/2.0; 
        t1 = (-L/2.0-vtx.Z())/dir.Z();
      }
    }

    x1 = vtx.X() + t1*dir.X();
    y1 = vtx.Y() + t1*dir.Y();

    // second intersection
    if( z2>-L/2.0 && z2<+L/2.0 ){
      region2 = 1;
    }
    if( z2>=+L/2.0 ){
      region2 = 0;
      if( z2>+L/2.0 ){
        z2 = +L/2.0; 
        t2 = (+L/2.0-vtx.Z())/dir.Z();
      }
    }
    if( z2<=-L/2.0 ){
      region2 = 2;
      if( z2<-L/2.0 ){
        z2 = -L/2.0; 
        t2 = (-L/2.0-vtx.Z())/dir.Z();
      }
    }

    x2 = vtx.X() + t2*dir.X();
    y2 = vtx.Y() + t2*dir.Y();

    // If we always want the far one... always want region 2?
    proj = TVector3(x2,y2,z2);
    region = region2;
  }
 
}

// Find the point on the circle with position circPos and direction circDir given the vector projected
// onto the wall and vertex position, and the Cherenkov angle and rotation angle (both in degrees)
// Adapted from WCSimAnalysis' WCSimGeometry class
void WCSimEvDisplay::FindCircle(TVector3 proj, TVector3 vtx, double thetaC, double phi, TVector3& circPos, TVector3& circDir){

  // Default values
  circPos = proj;
  circDir = TVector3(0.,0.,0.);

  // Inputs
  Double_t angle = (TMath::Pi()/180.0)*thetaC; // radians
  Double_t omega = (TMath::Pi()/180.0)*phi; // radians

  // Gives the vector from the vertex to the projected position
  TVector3 v2p = proj - vtx;

  // Unit vector from vertex to projected position
  TVector3 unit = v2p.Unit();

  // Rotate away from the projected positon by the cone angle, then around by omega, which runs up 
  // to 2pi to draw a cone of given angle around the projected position
  TVector3 initDir = unit;
  TVector3 orthDir = unit.Orthogonal();
  unit.Rotate(angle,orthDir);
  unit.Rotate(omega,initDir);
 
  // This is making a cone around the pmt hit starting at the vertex
  // whose cone angle is "angle" 
  // We increment omega to step round the edge of the cone
  // For each step we return the (x,y,z) coordinate of the point on the cone
  // and the direction
  circPos = vtx + v2p.Mag() * unit;
  circDir = unit;

  return;
}

// Create the TPolyMarker from two vectors reprsenting the "x" and "y" coordinates
void WCSimEvDisplay::MakePolyMarker(std::vector<double> coord1, std::vector<double> coord2, std::vector<TPolyMarker*>& poly, int colour){
  // Convert vectors into arrays
  double *x = new double[coord1.size()];
  double *y = new double[coord1.size()];

  for(unsigned int e = 0; e < coord1.size(); ++e){
    x[e] = coord1[e];
    y[e] = coord2[e];
  }

  TPolyMarker *newPoly = new TPolyMarker(coord1.size(),x,y);
  newPoly->SetMarkerColor(colour);
  newPoly->SetMarkerStyle(7);
  newPoly->SetMarkerSize(1.0);
  poly.push_back(newPoly);

  delete [] x;
  delete [] y;
  x = 0x0;
  y = 0x0;
}

void WCSimEvDisplay::ClearTruthMarkerVectors(){
  this->DeleteAndClearElements(fTruthMarkersTop);
  this->DeleteAndClearElements(fTruthMarkersBarrel);
  this->DeleteAndClearElements(fTruthMarkersBottom);
  // Also delete the vector of lines
  for(unsigned int l = 0; l < fTruthLines.size(); ++l){
    delete (TLine*)fTruthLines[l];
    fTruthLines[l] = 0x0;
  }
  fTruthLines.clear();
}

void WCSimEvDisplay::DeleteAndClearElements(std::vector<TPolyMarker*>& vec){
  if(vec.size()!=0){
    for(unsigned int v = 0; v < vec.size(); ++v){
      delete (TPolyMarker*)vec[v];
      vec[v] = 0x0;
    }
  }
  vec.clear();
}

int WCSimEvDisplay::GetTruthRingColour(int ring) const{
  if(ring == 1) return kMagenta;
  if(ring == 2) return kGreen;
  if(ring == 3) return kRed;
  if(ring == 4) return kOrange;
  return kBlack;
}

// Using the energy as the pi0 id, look for its photons
void WCSimEvDisplay::SearchForPi0Photons(double energy, TClonesArray* trajCont){
  // Iterate through the TClonesArray looking for what we want...
  bool gotPi0 = false;
  int gotPhotons = 0;
  int pi0ID = -1; // This is the geant4 track ID for the parent pi0

  // Temporarily store the information
  double pi0Energy = energy;
  TVector3 pi0Vtx;
  TVector3 pi0Dir;
  double photonEn1, photonEn2;
  TVector3 photonDir1, photonDir2;  

  TVector3 mainVtx = fTruthSummary->GetVertex();
  for(int e = 0; e < trajCont->GetEntries(); ++e){
    // Get the track
    WCSimRootTrack* trk = (WCSimRootTrack*)(*trajCont)[e];
    
    if(!gotPi0){
      // Is it a pizero?
      if(trk->GetIpnu() != 111) continue;
      if(trk->GetE() != energy) continue;

      // This is our pi0
      gotPi0 = true;
      pi0ID = trk->GetId();

      pi0Vtx = TVector3(trk->GetStart(0),trk->GetStart(1),trk->GetStart(2));
      pi0Dir = TVector3(trk->GetDir(0),trk->GetDir(1),trk->GetDir(2));

    }
    else{
      // Presumably the photons are after the pi0? Let's look for them, then
      if(trk->GetIpnu() != 22) continue; // Is it a photon?
      if(trk->GetParenttype() != 111) continue; // Was the parent a pi0?
      if(trk->GetParentId() != pi0ID) continue; // Pi0 stop and photon start at the same point?
      // This is a photon that we are looking for!
      if(gotPhotons == 0){
        photonEn1 = trk->GetE();
        photonDir1 = TVector3(trk->GetDir(0),trk->GetDir(1),trk->GetDir(2));
      }
      else{
        photonEn2 = trk->GetE();
        photonDir2 = TVector3(trk->GetDir(0),trk->GetDir(1),trk->GetDir(2));
      }
      ++gotPhotons;
    }
  }
  if(gotPi0 && gotPhotons==2){
    WCSimEvDispPi0 *newPi0 = new WCSimEvDispPi0();
    newPi0->SetPi0Information(pi0Energy,pi0Vtx,pi0Dir);
    newPi0->SetPhotonInformation(1,photonEn1,photonDir1);
    newPi0->SetPhotonInformation(2,photonEn2,photonDir2);
    fPi0s.push_back(newPi0);
  }
}

void WCSimEvDisplay::ClearPi0Vector(){
  for(unsigned int i = 0; i < fPi0s.size(); ++i){
    delete (WCSimEvDispPi0*)fPi0s[i];
    fPi0s[i] = 0x0;
  }
  fPi0s.clear();
}

unsigned int WCSimEvDisplay::GetPi0(double en) const{
  for(unsigned int p = 0; p < fPi0s.size(); ++p){
    if(fPi0s[p]->GetPi0Energy() == en){
      return p;
    } 
  }
  return 9999;
}


