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
//#include <TRootEmbeddedCanvas.h>
//#include <RQ_OBJECT.h>
#include "WCSimEvDisplay.hh"
#include "WCSimRootGeom.hh"
#include "WCSimRootEvent.hh"

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
//	can->Divide(1,2);
//	can->cd(2)->Divide(2,1);

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
/*
void WCSimEvDisplay::FillPlotsFromPhotonEvent(){

	// First thing we need to do is clear the plots
	this->ClearPlots();

	int eventID;
	float vtxX, vtxY, vtxZ;
	float endX, endY, endZ;
	float endDirX, endDirY, endDirZ;
	float endTime;

	bool slanted = false;

	// Firstly we need to attach some variables to the TChain.
	fChain->SetBranchAddress("eventID",&eventID);
	fChain->SetBranchAddress("vtxX",&vtxX);
	fChain->SetBranchAddress("vtxY",&vtxY);
	fChain->SetBranchAddress("vtxZ",&vtxZ);
	fChain->SetBranchAddress("endX",&endX);
	fChain->SetBranchAddress("endY",&endY);
	fChain->SetBranchAddress("endZ",&endZ);
	fChain->SetBranchAddress("endTime",&endTime);
	fChain->SetBranchAddress("enddirX",&endDirX);
	fChain->SetBranchAddress("enddirY",&endDirY);
	fChain->SetBranchAddress("enddirZ",&endDirZ);

	for(int i = 0; i < fChain->GetEntries(); ++i){

		fChain->GetEntry(i);

		// Should be ordered by event. If less than the event we want, continue.
		if(eventID < fCurrentEvent) continue;
		// If bigger, may as well break out of the loop to save wasting time.
		if(eventID > fCurrentEvent) break; 

		// Did the photon start in the detector?
		if(!this->PhotonStartedInDet(vtxX,vtxY,vtxZ)) continue;

		// Make sure the photon was detected within a reasonable time
		if(endTime > 1000) continue;

		// If it is too steep, we can't see it with slanted PMTs.
		if(slanted && endDirZ < -(1.0 / sqrt(2))) continue; 

//		Float_t radius = sqrt(endX*endX + endY*endY);
		Float_t phi = -1*TMath::ATan2(endY,endX);

//		std::cout << endX << ", " << endY << " || " << radius << ", " << theta << std::endl;
		if(endZ > 1740){
			// Only want photons coming upwards
			if(endDirZ < 0) continue; // Just incase we missed any dodgy photons
			fTopHist->Fill(endX,endY);
		}
		else if(endZ < -1740){
		// Only want photons going downwards
			if(slanted) continue; // The bottom is pointless if we are being realistic
			if(endDirZ > 0) continue; // Just incase we missed any dodgy photons
			fBottomHist->Fill(endX,endY);
		}
		else{
			fBarrelHist->Fill(phi,endZ);
		}
	}

	this->UpdateCanvases();

}
*/

// Switch the z-axis scale to show charge.
void WCSimEvDisplay::SetViewCharge(){
//	if(fFileType == 0){
		if(fViewType != 0){
			fViewType = 0;
			std::cout << "Setting colour axis to charge" << std::endl;
			this->FillPlotsFromWCSimEvent();
		}
		else{
			std::cout << "Already viewing charge." << std::endl;
		}
//	}
//	else{
//		std::cout << "This toggle only applies for WCSim files." << std::endl;
//	}
}

// Switch the z-axis scale to show time.
void WCSimEvDisplay::SetViewTime(){
//	if(fFileType == 0){
		if(fViewType != 1){
			fViewType = 1;
			std::cout << "Setting colour axis to time" << std::endl;
			this->FillPlotsFromWCSimEvent();
		}
		else{
			std::cout << "Already viewing time." << std::endl;
		}
//	}
//	else{
//		std::cout << "This toggle only applies for WCSim files." << std::endl;
//	}
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
		TList *list = can->GetListOfPrimitives();
		list->Remove(fChargePad);
		list->Remove(fTimePad); 
		// Now resize the othe pads
    fBarrelPad->SetPad(0.0,0.5,1.0,1.0);
    fTopPad->SetPad(0.0,0.0,0.5,0.5);
    fBottomPad->SetPad(0.5,0.0,1.0,0.5);
	}	
	this->UpdateCanvases();
}

void WCSimEvDisplay::UpdateCanvases(){

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


