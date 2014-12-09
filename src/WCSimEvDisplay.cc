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
  fShowTruth = false; 
  fTruthTextMain = new TPaveText(0.05,0.45,0.95,0.90,"NDC");
  fTruthTextPrimaries = new TPaveText(0.05,0.1,0.95,0.40,"NDC");
  fDatabasePDG = 0x0;

  // Initialise the TGNumberEntry
  fPEInput = 0x0;
  fChargeCut = 0;

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

	this->AddFrame(fHitMapCanvas, new TGLayoutHints(kLHintsExpandX| kLHintsExpandY,10,10,10,1));

	// These build from top to bottom, so add them in the correct order.
	this->CreateSubButtonBar();
	this->CreateMainButtonBar();

	// Initialise the histograms and display them.
//	this->ResizePlotsFromNtuple();

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
	TGTextButton *togCharge = new TGTextButton(hWCSimButtons,"&Charge");
	togCharge->Connect("Clicked()","WCSimEvDisplay",this,"SetViewCharge()");
	hWCSimButtons->AddFrame(togCharge, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
	TGTextButton *togTime = new TGTextButton(hWCSimButtons,"&Time");
	togTime->Connect("Clicked()","WCSimEvDisplay",this,"SetViewTime()");
	hWCSimButtons->AddFrame(togTime, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
	TGTextButton *togHists = new TGTextButton(hWCSimButtons,"Toggle 1D Plots");
	togHists->Connect("Clicked()","WCSimEvDisplay",this,"Toggle1DHists()");
	hWCSimButtons->AddFrame(togHists, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
  TGTextButton *showReco = new TGTextButton(hWCSimButtons,"&Reco");
  showReco->Connect("Clicked()","WCSimEvDisplay",this,"ShowReco()");
  hWCSimButtons->AddFrame(showReco, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
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
  // Update the truth view
  this->UpdateTruthPave();

  int nDigiHits = wcSimTrigger->GetNcherenkovdigihits();
	std::cout << "Number of PMTs hit: " << nDigiHits << std::endl;

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

	  	// Top cap
  		if(pmt.GetCylLoc() == 0){
  			fTopHist->Fill(pmtY,pmtX,colourAxis);
  		}
  		// Bottom cap
  		else if(pmt.GetCylLoc() == 2){
  			fBottomHist->Fill(pmtY,pmtX,colourAxis);
  		}
  		// Barrel
  		else{
  			fBarrelHist->Fill(pmtPhi,pmtZ,colourAxis);
  		}

		  // Now fill the 1D histograms
		  fChargeHist->Fill(pmtQ);	 
		  fTimeHist->Fill(pmtT);	 
    }


	} // End of loop over Cherenkov digihits

	delete geo;
	geo = 0x0;

	this->UpdateCanvases();
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

void WCSimEvDisplay::MatchPlotZAxes(){

	// Make sure the histogram max / min are correct
	fBarrelHist->SetMaximum(fBarrelHist->GetBinContent(fBarrelHist->GetMaximumBin()));
	fBarrelHist->SetMinimum(fBarrelHist->GetBinContent(fBarrelHist->GetMinimumBin()));
	fTopHist->SetMaximum(fTopHist->GetBinContent(fTopHist->GetMaximumBin()));
	fTopHist->SetMinimum(fTopHist->GetBinContent(fTopHist->GetMinimumBin()));
	fBottomHist->SetMaximum(fBottomHist->GetBinContent(fBottomHist->GetMaximumBin()));
	fBottomHist->SetMinimum(fBottomHist->GetBinContent(fBottomHist->GetMinimumBin()));

	// Sort out the minimum z-values for the colour plots
	this->GetMinColourAxis(fBarrelHist);
	this->GetMinColourAxis(fTopHist);
	this->GetMinColourAxis(fBottomHist);

	// Initialise the min and max z-values from the barrel plot
	double min = fBarrelHist->GetMinimum();
	double max = fBarrelHist->GetMaximum();

	// Check if the top or bottom chaneg these min and max values
	if(fTopHist->GetMinimum() < min){min = fTopHist->GetMinimum();}
	if(fBottomHist->GetMinimum() < min){min = fBottomHist->GetMinimum();}
	if(fTopHist->GetMaximum() > max){max = fTopHist->GetMaximum();}
	if(fBottomHist->GetMaximum() > max){max = fBottomHist->GetMaximum();}

	// Set the minumum and maximum for all plots
	fBarrelHist->SetMinimum(min);
	fBarrelHist->SetMaximum(max);
	fTopHist->SetMinimum(min);
	fTopHist->SetMaximum(max);
	fBottomHist->SetMinimum(min);
	fBottomHist->SetMaximum(max);
}

// Resize the pads when hiding / showing the 1D plots
void WCSimEvDisplay::ResizePads(){

	TCanvas *can = fHitMapCanvas->GetCanvas();
	can->cd();
  TList *list = can->GetListOfPrimitives();

  // If we want to show truth
  if(fShowTruth){
    list->Remove(fBarrelPad);
    list->Remove(fTopPad);
    list->Remove(fBottomPad);
  	if(fShow1DHists){
  		list->Remove(fChargePad);
  		list->Remove(fTimePad); 
  	}
    fTruthPad->SetPad(0.0,0.0,1.0,1.0);
    fTruthPad->Draw();
  }
  // Or else show the reco
  else{
    // Remove the truth information pad
 		list->Remove(fTruthPad);   
  	// Are the 1D plots visible?
  	if(fShow1DHists){
  		fBarrelPad->SetPad(0.0,0.6,1.0,1.0);
  		fTopPad->SetPad(0.0,0.2,0.5,0.6);
  		fBottomPad->SetPad(0.5,0.2,1.0,0.6);
  		fChargePad->SetPad(0.0,0.0,0.5,0.2);
  		fTimePad->SetPad(0.5,0.0,1.0,0.2);
  		// Only need to draw the 1D plots since they are
  		// the only ones that are hidden.
  		fChargePad->Draw();
  		fTimePad->Draw();
  	}
  	else{
  		// Firstly, remove the 1D pads from the TList
  		list->Remove(fChargePad);
  		list->Remove(fTimePad); 
  		// Now resize the othe pads
      fBarrelPad->SetPad(0.0,0.5,1.0,1.0);
      fTopPad->SetPad(0.0,0.0,0.5,0.5);
      fBottomPad->SetPad(0.5,0.0,1.0,0.5);
  	}
    fBarrelPad->Draw();
    fTopPad->Draw();
    fBottomPad->Draw();
  }

//  can->Modified();
//  can->Update();
	this->UpdateCanvases();
}

void WCSimEvDisplay::UpdateCanvases(){

  if(!fShowTruth){
	  this->MatchPlotZAxes();

  	this->MakePlotsPretty(fBarrelHist);
  	this->MakePlotsPretty(fTopHist);
  	this->MakePlotsPretty(fBottomHist);

  	// Take the plots one by one and draw them.
  	TCanvas *canvas = fHitMapCanvas->GetCanvas();
  	canvas->cd(1);
  	fBarrelPad->cd();
  	fBarrelHist->Draw("colz");
    // Draw the truth rings if needed
    for(unsigned int r = 0; r < fTruthMarkersBarrel.size(); ++r){
      std::cout << "Drawing Barrel Truth " << r << std::endl;
      if(!fTruthMarkersBarrel[r]){
        std::cout << "Why the hell am I a null pointer?" << std::endl;
      }
      else fTruthMarkersBarrel[r]->Draw("C");
      
    }
  	canvas->Modified();
  	canvas->Update();

  	fTopPad->cd();
  	fTopHist->Draw("colz");
    // Draw the truth rings if needed
    for(unsigned int r = 0; r < fTruthMarkersTop.size(); ++r){
      std::cout << "Drawing Top Truth " << r << std::endl;
      fTruthMarkersTop[r]->Draw("C");
    }
  	canvas->Modified();
  	canvas->Update();

  	fBottomPad->cd();
  	fBottomHist->Draw("colz");
    // Draw the truth rings if needed
    for(unsigned int r = 0; r < fTruthMarkersBottom.size(); ++r){
      std::cout << "Drawing Bottom Truth " << r << std::endl;
      fTruthMarkersBottom[r]->Draw("C");
    }
  	canvas->Modified();
  	canvas->Update();

  	if(fShow1DHists){
  		fChargePad->cd();
  		this->MakePlotsPretty(fChargeHist);
  		fChargeHist->Draw();

  		fTimePad->cd();
  		this->MakePlotsPretty(fTimeHist);
  		fTimeHist->Draw();
  	}
  }
  else{
  	TCanvas *canvas = fHitMapCanvas->GetCanvas();
    fTruthPad->cd();
    fTruthPad->Draw();
    fTruthTextMain->Draw();
    fTruthTextPrimaries->Draw();
  	canvas->Modified();
  	canvas->Update();
  }
}

void WCSimEvDisplay::NextEvent() {
	if(fChain->GetEntries() > 0){
		if(fCurrentEvent < fMaxEvent){
			++fCurrentEvent;
			std::cout << "Moving to event " << fCurrentEvent << std::endl;
			this->FillPlotsFromWCSimEvent();
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
		}
		else{
			std::cout << "Already at the first event" << std::endl;
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

    if(fShowTruth == false){

      fShowTruth = true;
  
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
  if(fShowTruth == true){
    fShowTruth = false;
    this->ResizePads();
  }
  else{
    std::cerr << "Already displaying reco view." << std::endl;
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
  std::cout << "Vertex = " << vtx.X() << ", " << vtx.Y() << ", " << vtx.Z() << std::endl;
  std::cout << "Radius = " << fWCRadius << ", " << "Height = " << fWCLength << std::endl;
  tmpS << vtx.X() << "," << vtx.Y() << "," << vtx.Z();
  fTruthTextMain->AddText(("Vertex at ("+tmpS.str()+") cm").c_str());
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
  tmpS << fTruthSummary->GetBeamPDG();
  fTruthTextMain->AddText(("PDG Code = "+tmpS.str()).c_str());
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
        this->DrawTruthRing(n,this->GetTruthRingColour(nTruthRings));
      }
      dir = fTruthSummary->GetPrimaryDir(n);
      tmpS.str("");
      tmpS << mod << " ";
      tmpS << "Particle: " << pdg;
      tmpS << " with energy " << energy;
      tmpS << " MeV and direction (" << dir.X() << "," << dir.Y() << "," << dir.Z() << ")";
      tmpS << " " << mod;
      fTruthTextPrimaries->AddText(tmpS.str().c_str());
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
  TVector3 trkDir = fTruthSummary->GetPrimaryDir(particleNo);

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
  double mass = 1000*fDatabasePDG->GetParticle(fTruthSummary->GetPrimaryPDG(particleNo))->Mass();
  double en = fTruthSummary->GetPrimaryEnergy(particleNo);
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

  for(unsigned int n = 0; n < nMarkers; ++n){

    double phi = n * dPhi;

    // Find a point on the Cherenkov cone and its direction
    TVector3 circPos;
    TVector3 circDir;
    this->FindCircle(trkVtxProj,trkVtx,thetaC,phi,circPos,circDir);     

    // Now we have the point on the circle, project this onto the wall
    TVector3 finalPos;
    this->ProjectToWall(circPos,circDir,finalPos,detRegion);

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
  }

  // Now we have the vectors of coordinates that we need for each region. Now to make the TPolyMarkers
  std::cout << "= Making markers for particle " << particleNo << std::endl;
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

//  Double_t xNear = -99999.9;
//  Double_t yNear = -99999.9;
//  Double_t zNear = -99999.9;
//  Int_t regionNear = WCSimGeometry::kUnknown;

//  Double_t xFar = -99999.9;
//  Double_t yFar = -99999.9;
//  Double_t zFar = -99999.9;
//  Int_t regionFar = WCSimGeometry::kUnknown;

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
//  Double_t r0r0 = x0*x0 + y0*y0;
  Double_t r0r0 = vtx.X()*vtx.X() + vtx.Y()*vtx.Y();
//  Double_t r0p = x0*px + y0*py;
  Double_t r0p = vtx.X()*dir.X() + vtx.Y()*dir.Y();
//  Double_t pSq = px*px+py*py;
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

    // near/far projection
//    if( t1>=0 ){
//      xNear = x1;
//      yNear = y1;
//      zNear = z1;
//      regionNear = region1;

//      xFar = x2;
//      yFar = y2;
//      zFar = z2;
//      regionFar = region2;
//    }
//    else if( t2>0 ){
//      xNear = x2;
//      yNear = y2;
//      zNear = z2;
//      regionNear = region2;

//      xFar = x2;
//      yFar = y2;
//      zFar = z2;
//      regionFar = region2;
//    }
    // If we always want the far one... always want region 2?
    proj = TVector3(x2,y2,z2);
    region = region2;
  }
 
//  }
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
 
  // Outputs
//  rx = x0 + ds*myDir.x(); 
//  ry = y0 + ds*myDir.y();   
//  rz = z0 + ds*myDir.z();   

  // This is making a cone around the pmt hit starting at the vertex
  // whose cone angle is "angle" 
  // We increment omega to step round the edge of the cone
  // For each step we return the (x,y,z) coordinate of the point on the cone
  // and the direction
  circPos = vtx + v2p.Mag() * unit;
  circDir = unit;

//  nx = myDir.x();   
//  ny = myDir.y();       // circDir is the unit vector from the vertex to the rim of the cone
//  nz = myDir.z();

//  r = ds*sin(angle);      // the radius from the PMT hit to the edge of the cone

  return;
}

// Create the TPolyMarker from two vectors reprsenting the "x" and "y" coordinates
void WCSimEvDisplay::MakePolyMarker(std::vector<double> coord1, std::vector<double> coord2, std::vector<TPolyMarker*>& poly, int colour){
  // Convert vectors into arrays
  double *x = new double[coord1.size()];
  double *y = new double[coord1.size()];

  std::cout << "New ring info: " << std::endl;
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

