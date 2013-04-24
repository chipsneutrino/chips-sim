void total_charge(char *filename=NULL) {
  /* A simple script to plot aspects of phototube hits 
   * This code is rather cavalier; I should be checking return values, etc.
   * First revision 6-24-10 David Webber
   * 
   * I like to run this macro as 
   * $ root -l -x 'read_PMT.C("../wcsim.root")'
   */

  gROOT->Reset();
  gSystem->Load("../libWCSimRoot.so");
  gStyle->SetOptStat(1);

  TFile *f;
  if (filename==NULL){
//    f = new TFile("../wcsim.root");
	  f= new TFile("../../mcSamples/numi_numu_NC_100kT_10pC_1000events_NC.root");

  }else{
    f = new TFile(filename);
  }
  if (!f->IsOpen()){
    cout << "Error, could not open input file: " << filename << endl;
    return -1;
  }

  TTree  *wcsimT = f->Get("wcsimT");  
  int nevt = wcsimT->GetEntries();	  // ANDY: counts # events  
	cout << nevt << endl;
  WCSimRootEvent *wcsimrootsuperevent = new WCSimRootEvent();
  wcsimT->SetBranchAddress("wcsimrootevent",&wcsimrootsuperevent);

  // Force deletion to prevent memory leak when issuing multiple
  // calls to GetEvent()
  wcsimT->GetBranch("wcsimrootevent")->SetAutoDelete(kTRUE);

  TH1D * h_TotCharge = new TH1D("h_TotCharge","Total Charge",32,0,80000);


for(int k =0; k<nevt; k++)
{
	cout << k << endl;
  wcsimT->GetEvent(k); 
  // In the default vis.mac, only one event is run.  I suspect you could loop over more events, if they existed.
  WCSimRootTrigger *wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);
  int totalQ = 0;


  int max = wcsimrootevent->GetNcherenkovdigihits();
  for (int i = 0; i<max; i++){
    WCSimRootCherenkovDigiHit *cDigiHit = wcsimrootevent->GetCherenkovDigiHits()->At(i);
    //WCSimRootChernkovDigiHit has methods GetTubeId(), GetT(), GetQ()
	totalQ += cDigiHit->GetQ();
  }

  h_TotCharge->Fill(totalQ);

} // close loop over evts

h_TotCharge->Draw();

}
