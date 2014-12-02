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
		double pmtPhi = -1*TMath::ATan2(pmtY,pmtX);
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
  	canvas->Modified();
  	canvas->Update();

  	fTopPad->cd();
  	fTopHist->Draw("colz");
  	canvas->Modified();
  	canvas->Update();

  	fBottomPad->cd();
  	fBottomHist->Draw("colz");
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

  // The primary particles list
  if(fTruthSummary->IsNeutrinoEvent()){
    fTruthTextPrimaries->AddText("List of Primary Particles");
    for(unsigned int n = 0; n < fTruthSummary->GetNPrimaries(); ++n){
      dir = fTruthSummary->GetPrimaryDir(n);
      tmpS.str("");
      tmpS << "Particle: " << fTruthSummary->GetPrimaryPDG(n);
      tmpS << " with energy " << fTruthSummary->GetPrimaryEnergy(n);
      tmpS << " MeV and direction (" << dir.X() << "," << dir.Y() << "," << dir.Z() << ")";
      fTruthTextPrimaries->AddText(tmpS.str().c_str());
    } 
  }
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
			double tempPhi = -1*TMath::ATan2(pmt.GetPosition(1),pmt.GetPosition(0));
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


