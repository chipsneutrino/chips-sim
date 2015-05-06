#include "WCSimEmissionProfileMaker.hh"
#include "WCSimEventAction.hh"
#include "WCSimPhotonNtuple.hh"
#include "WCSimTrajectory.hh"
#include "WCSimRunAction.hh"
#include "WCSimPrimaryGeneratorAction.hh"
#include "WCSimWCHit.hh"
#include "WCSimWCDigi.hh"
#include "WCSimWCDigitizer.hh"
#include "WCSimDetectorConstruction.hh"
#include "WCSimPMTConfig.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4EventManager.hh"
#include "G4UImanager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4VVisManager.hh"
#include "G4ios.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4TransportationManager.hh" 
#include "G4Navigator.hh" 
#include "G4SDManager.hh"
#include "G4DigiManager.hh"
#include "G4UnitsTable.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"

#include <set>
#include <iomanip>
#include <string>
#include <vector>

#include "TTree.h"
#include "TFile.h"
#include "WCSimRootEvent.hh"
#include "TStopwatch.h"

#define _SAVE_RAW_HITS

WCSimEventAction::WCSimEventAction(WCSimRunAction* myRun, 
    WCSimDetectorConstruction* myDetector, 
    WCSimPrimaryGeneratorAction* myGenerator)
:runAction(myRun), generatorAction(myGenerator), 
  detectorConstructor(myDetector)
{
  G4DigiManager* DMman = G4DigiManager::GetDMpointer();

  WCSimWCDigitizer* WCDM = new WCSimWCDigitizer( "WCReadout", detectorConstructor);
  DMman->AddNewModule(WCDM);
}

WCSimEventAction::~WCSimEventAction(){}

void WCSimEventAction::BeginOfEventAction(const G4Event*){}

void WCSimEventAction::EndOfEventAction(const G4Event* evt)
{

  // ----------------------------------------------------------------------
  //  Get Trajectory Container
  // ----------------------------------------------------------------------

  G4TrajectoryContainer* trajectoryContainer = evt->GetTrajectoryContainer();

  G4int n_trajectories = 0;
  if (trajectoryContainer) n_trajectories = trajectoryContainer->entries();
  std::cout << "There were " << n_trajectories << " trajectories" << std::endl;

  // ----------------------------------------------------------------------
  //  Get Event Information
  // ----------------------------------------------------------------------

  G4int         event_id = evt->GetEventID();

  // ----------------------------------------------------------------------
  //  Get WC Hit Collection
  // ----------------------------------------------------------------------

  G4SDManager* SDman = G4SDManager::GetSDMpointer();

  // Get Hit collection of this event
  G4HCofThisEvent* HCE         = evt->GetHCofThisEvent();
  WCSimWCHitsCollection* WCHC = 0;

  if (HCE)
  { 
    G4String name =   "glassFaceWCPMT";
    G4int collectionID = SDman->GetCollectionID(name);
    WCHC = (WCSimWCHitsCollection*)HCE->GetHC(collectionID);
  }

  // ----------------------------------------------------------------------
  //  Get Digitized Hit Collection
  // ----------------------------------------------------------------------

  // Get a pointer to the Digitizing Module Manager
  G4DigiManager* DMman = G4DigiManager::GetDMpointer();

  // Get a pointer to the WC Digitizer module
  WCSimWCDigitizer* WCDM = (WCSimWCDigitizer*)DMman->FindDigitizerModule("WCReadout");

  // new MFechner, aug 2006
  // need to clear up the old info inside Digitizer
  WCDM->ReInitialize();

  // Figure out what size PMTs we are using in the WC detector.
  WCSimPMTConfig pmt = detectorConstructor->GetPMTVector()[0];
  G4float PMTSize = pmt.GetRadius();
  WCDM->SetPMTSize(PMTSize);

  // Digitize the hits
  WCDM->Digitize();\
  // Get the digitized collection for the WC
  G4int WCDCID = DMman->GetDigiCollectionID("WCDigitizedCollection");
  WCSimWCDigitsCollection * WCDC = (WCSimWCDigitsCollection*) DMman->GetDigiCollection(WCDCID);

  // Fill photon ntuple
  if( GetRunAction()->GetSavePhotonNtuple()  || GetRunAction()->GetSaveEmissionProfile() )
  {
      if( GetRunAction()->GetSaveEmissionProfile())
      {
    	  WCSimTruthSummary summ = generatorAction->GetTruthSummary();
    	  WCSimEmissionProfileMaker::Instance()->Fill( evt->GetTrajectoryContainer(), &summ );
      }

    for ( G4int i=0; i < n_trajectories; i++ ) {
      WCSimTrajectory* trj = (WCSimTrajectory*)((*(evt->GetTrajectoryContainer()))[i]);

      // get properties of trajectory
      G4int trackID = trj->GetTrackID();
      G4int parentID = trj->GetParentID();
      G4int processID = trj->GetProcessID();
      G4int pdgCode = trj->GetPDGEncoding();
      G4double energy = trj->GetEnergy();
      G4double lambda = trj->GetWavelength();
      G4bool opticalPhoton = trj->IsOpticalPhoton();
      G4bool scatteredPhoton = trj->IsScatteredPhoton();
      G4double vtxX = trj->GetVtxX();
      G4double vtxY = trj->GetVtxY();
      G4double vtxZ = trj->GetVtxZ();
      G4double vtxTime = trj->GetVtxTime();
      G4double vtxdirX = trj->GetVtxDirX();
      G4double vtxdirY = trj->GetVtxDirY();
      G4double vtxdirZ = trj->GetVtxDirZ();    
      G4double endX = trj->GetEndX();
      G4double endY = trj->GetEndY();
      G4double endZ = trj->GetEndZ();
      G4double endTime = trj->GetEndTime();

      if(GetRunAction()->GetSavePhotonNtuple())
      {
    	  WCSimPhotonNtuple::Fill( event_id,
    			  pdgCode, trackID, parentID, processID,
    			  energy, lambda,
    			  opticalPhoton, scatteredPhoton,
    			  vtxX, vtxY, vtxZ, vtxTime,
    			  endX, endY, endZ, endTime,
    			  vtxdirX, vtxdirY, vtxdirZ );
      }


    }
  }

  G4cout << " Filling Root Event " << G4endl;

  if(GetRunAction()->GetSaveRootFile())
  {
	  FillRootEvent(event_id,
			  trajectoryContainer,
			  WCHC,
			  WCDC);
  }

}

G4int WCSimEventAction::WCSimEventFindStartingVolume(G4ThreeVector vtx)
{
  // Get volume of starting point (see GEANT4 FAQ)

  G4int vtxvol = -1;

  G4Navigator* tmpNavigator = 
    G4TransportationManager::GetTransportationManager()->
    GetNavigatorForTracking();

  G4VPhysicalVolume* tmpVolume = tmpNavigator->LocateGlobalPointAndSetup(vtx);
  G4String       vtxVolumeName = tmpVolume->GetName();



  if ( vtxVolumeName == "outerTube" ||
      vtxVolumeName == "innerTube" ||
      vtxVolumeName == "rearEndCap"|| 
      vtxVolumeName == "frontEndCap" ){
    vtxvol = 10;
  }
  else if ( vtxVolumeName.contains("WC") && !vtxVolumeName.contains("FV") )
  {
    // aah original line  ->if (vtxVolumeName.contains("WCBarrel"))
    if ((vtxVolumeName.contains("WCBarrel"))|| (vtxVolumeName.contains("Tank"))){	//aah I added "Tank" as MB equivalent of Barrel
      vtxvol = 10;
    }
    else if (vtxVolumeName == "WCBox") {vtxvol = -2;}
    else if (vtxVolumeName.contains("PMT") ||
        vtxVolumeName.contains("Cap") ||
        vtxVolumeName.contains("Cell")){
      vtxvol = 11;
    }
    else if (vtxVolumeName.contains("OD")) {vtxvol = 12;}
    else
    {
      G4cout << vtxVolumeName << " unkown vtxVolumeName " << G4endl;
      vtxvol = -3;
    }
  }
  else if ( vtxVolumeName == "expHall" ) {vtxvol = 0;}
  else if ( vtxVolumeName == "catcher" ) {vtxvol = 40;}


  return vtxvol;
}

G4int WCSimEventAction::WCSimEventFindStoppingVolume(G4String stopVolumeName)
{
  G4int stopvol = -1;

  if ( stopVolumeName.contains("WC") && !stopVolumeName.contains("FV") )
  {
    //aah Original line->    if (stopVolumeName.contains("WCBarrel"))
    if ((stopVolumeName.contains("WCBarrel"))|| (stopVolumeName.contains("Tank")))	// aah I added "Tank" as MB equivalent of Barrel
      stopvol = 10;
    else if (stopVolumeName == "WCBox")
      stopvol = -2;
    else if (stopVolumeName.contains("PMT") ||
        stopVolumeName.contains("Cap") ||
        stopVolumeName.contains("Cell"))
      stopvol = 11;
    else if (stopVolumeName.contains("OD"))
      stopvol = 12;
    else
    {
      G4cout << stopVolumeName << " unkown stopVolumeName " << G4endl;
      stopvol = -3;
    }
  }

  else if ( stopVolumeName.contains("FV") )
  {
    if (stopVolumeName == "WCFVBarrel" ||
        stopVolumeName == "WCFVAnnulus" ||
        stopVolumeName == "WCFVRing" )
      stopvol = 10;
    else if (stopVolumeName.contains("FVPMT"))
      stopvol = 13;
    else
    {
      G4cout << stopVolumeName << " unkown stopVolumeName " << G4endl;
      stopvol = -3;
    }
  }
  else if ( stopVolumeName == "expHall" )
    stopvol = 0;
  else if ( stopVolumeName == "catcher" )
    stopvol = 40;


  return stopvol;
}

void WCSimEventAction::FillRootEvent(G4int event_id, 
    G4TrajectoryContainer* TC,
    WCSimWCHitsCollection* WCHC, 
    WCSimWCDigitsCollection* WCDC)
{
  // Fill up a Root event with stuff from the ntuple

  WCSimRootEvent* wcsimrootsuperevent = GetRunAction()->GetRootEvent();

  // Add the truth information from the WCSimPrimaryActionGenerator
  WCSimTruthSummary truthSum = generatorAction->GetTruthSummary();
  wcsimrootsuperevent->SetTruthSummary(truthSum);

  // start with the first "sub-event"
  // if the WC digitization requires it, we will add another subevent
  // for the WC.
  // all the rest goes into the first "sub-event".
  WCSimRootTrigger* wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);
  // get number of gates
  G4DigiManager* DMman = G4DigiManager::GetDMpointer();
  WCSimWCDigitizer* WCDM =
    (WCSimWCDigitizer*)DMman->FindDigitizerModule("WCReadout");
  int ngates = WCDM->NumberOfGatesInThisEvent(); 
  G4cout << "ngates =  " << ngates << "\n";
  for (int index = 0 ; index < ngates ; index++) 
  {
    if (index >=1 ) {
      wcsimrootsuperevent->AddSubEvent();
      wcsimrootevent = wcsimrootsuperevent->GetTrigger(index);
      wcsimrootevent->SetHeader(event_id,0,
          0,index+1); // date & # of subevent 
      wcsimrootevent->SetMode(truthSum.GetInteractionMode()); 
    }
  }


  // Fill the header
  // Need to add run and date
  wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);
  wcsimrootevent->SetHeader(event_id,0,0); // will be set later.

  // Fill other info for this event
  wcsimrootevent->SetMode(truthSum.GetInteractionMode());
  TVector3 tmpVec = truthSum.GetVertex();
  G4ThreeVector vtx = G4ThreeVector(tmpVec.X(),tmpVec.Y(),tmpVec.Z());
  wcsimrootevent->SetVtxvol(WCSimEventFindStartingVolume(vtx));
  for (int j=0;j<3;j++)
  {
    wcsimrootevent->SetVtx(j,vtx[j]);
  }
  // wcsimrootevent->SetJmu(jhfNtuple.jmu); // Never appears to be set...
  // wcsimrootevent->SetJp(jhfNtuple.jp); // Never appears to be set...
  // wcsimrootevent->SetNpar(jhfNtuple.npar); // Never appears to be set...
  wcsimrootevent->SetVecRecNumber(event_id);//generatorAction->GetVecRecNumber()); //jhfNtuple.vecRecNumber);

  float dir[3];
  float pdir[3];
  float stop[3];
  float start[3];

  // First track is special and contains beam information
  G4double beamEnergy = truthSum.GetBeamEnergy();
  tmpVec = truthSum.GetBeamDir();
  G4ThreeVector beamDir = G4ThreeVector(tmpVec.X(),tmpVec.Y(),tmpVec.Z());
  G4ThreeVector beamMom = beamDir*beamEnergy;

  // Fill the float arrays (Why can't we use three vectors for this?)
  for(unsigned int i = 0; i < 3; ++i){
    dir[i] = beamDir[i];
    pdir[i] = beamMom[i];
    stop[i] = vtx[i]/cm; // Apparently not meaningful?
    start[i] = 0.0; // I never saw this set anywhere.
  }

  wcsimrootevent->AddTrack(truthSum.GetBeamPDG(),
      -1, // This is the dummy flag for the neutrino
      0.0, // Assume zero mass for neutrino
      beamEnergy, // Momentum = Energy for zero mass
      beamEnergy, // Energy 
      -1, // Start volume = -1
      -1, // Stop volume = -1
      dir, // Direction
      pdir, // Mom-vector
      stop, // Stopping point
      start, // Starting point
      0, // Neutrino has no parent
      0.0, // Time doesn't appear to be set
      0, // No track id as not tracked
      0); // No parent information                        

  // Second track is special and contains target information
  G4double targetEnergy = truthSum.GetTargetEnergy();
  tmpVec = truthSum.GetTargetDir();
  G4ThreeVector targetDir = G4ThreeVector(tmpVec.X(),tmpVec.Y(),tmpVec.Z());
  G4int targetPDG = truthSum.GetTargetPDG();
  // Find out the target mass
  G4double targetMass = 0.0;
  G4double targetPMag = 0.0;
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  if (targetPDG!=0 && targetPDG!=-999) {            // protects against seg-fault
    if (targetPDG > 999){         // 16O nucleus not in pdg table
      targetMass = targetEnergy; // 16O is at rest, so E = m
    }
    else{
      targetMass = particleTable->FindParticle(targetPDG)->GetPDGMass();
    }

    if (targetEnergy > targetMass){
      //      targetpmag = sqrt(targetenergy*targetenergy - targetmass*targetenergy);
      // MF : bug fix
      targetPMag = sqrt(targetEnergy*targetEnergy - targetMass*targetMass);
    }
    else{ // protect against NaN
      targetPMag = 0.0;
    }
  }
  G4ThreeVector targetMom = targetDir*targetPMag;

  // Fill the float arrays (Why can't we use three vectors for this?)
  for(unsigned int i = 0; i < 3; ++i){
    dir[i] = targetDir[i];
    pdir[i] = targetMom[i];
    stop[i] = vtx[i]/cm; // Apparently not meaningful?
    start[i] = 0.0; // I never saw this set anywhere.
  }

  // If the target PDG code is -999, then this was a particle gun with no target, so don't store it.
  if(targetPDG!=-999){
    wcsimrootevent->AddTrack(targetPDG,
      -2, // This is the dummy flag for the target
      targetMass, // Mass
      targetPMag, // Momentum 
      targetEnergy, // Energy 
      -1, // Start volume = -1
      -1, // Stop volume = -1
      dir, // Direction
      pdir, // Mom-vector
      stop, // Stopping point
      start, // Starting point
      0, // Target has no parent
      0.0, // Time doesn't appear to be set
      0, // No track id as not tracked
      0); // No parent information
  }

  // the rest of the tracks come from WCSimTrajectory

  std::set<int> pizeroList;
  // added by M Fechner, dec 16th, 2004
  std::set<int> muonList;
  std::set<int> antimuonList;
  // same, april 7th 2005
  std::set<int> pionList;
  std::set<int> antipionList;

  // Pi0 specific variables
  Float_t pi0Vtx[3];
  Int_t   gammaID[2];
  Float_t gammaE[2];
  Float_t gammaVtx[2][3];
  Int_t   r = 0;

  G4int n_trajectories = 0;
  if (TC)
    n_trajectories = TC->entries();


  // M Fechner : removed this limit to get to the primaries...
  //if (n_trajectories>50)  // there is no need for this limit, but it has
  //n_trajectories=50;    // existed in previous versions of the code.  It also
  // makes the ROOT file smaller.  

  for (int i=0; i <n_trajectories; i++) 
  {
    WCSimTrajectory* trj = (WCSimTrajectory*)(*TC)[i];

    // If this track is a pizero remember it for later
    if ( trj->GetPDGEncoding() == 111)
      pizeroList.insert(trj->GetTrackID());
    // If it is a mu+/mu- also remember it
    if ( trj->GetPDGEncoding() == 13 ) muonList.insert(trj->GetTrackID());
    if ( trj->GetPDGEncoding() == -13 ) antimuonList.insert(trj->GetTrackID());
    if ( trj->GetPDGEncoding() == 211 ) pionList.insert(trj->GetTrackID());
    if ( trj->GetPDGEncoding() == -211 ) antipionList.insert(trj->GetTrackID());


    // Process primary tracks or the secondaries from pizero or muons...

    if ( trj->GetSaveFlag() )
    {
      // initial point of the trajectory
      G4TrajectoryPoint* aa =   (G4TrajectoryPoint*)trj->GetPoint(0) ;   
      runAction->incrementEventsGenerated();

      G4int         ipnu   = trj->GetPDGEncoding();
      G4int         id     = trj->GetTrackID();
      G4int         flag   = 0;    // will be set later
      G4double      mass   = trj->GetParticleDefinition()->GetPDGMass();
      G4ThreeVector mom    = trj->GetInitialMomentum();
      G4double      mommag = mom.mag();
      G4double      energy = sqrt(mom.mag2() + mass*mass);
      G4ThreeVector Stop   = trj->GetStoppingPoint();
      G4ThreeVector Start  = aa->GetPosition();

      G4String stopVolumeName = trj->GetStoppingVolume()->GetName();
      G4int    stopvol     = WCSimEventFindStoppingVolume(stopVolumeName);
      G4int    startvol    = WCSimEventFindStartingVolume(Start);

      G4double ttime = trj->GetGlobalTime(); 

      G4int parentType;
      G4int parentID = trj->GetParentID();

      // Right now only secondaries whose parents are pi0's are stored
      // This may change later
      // M Fechner : dec 16, 2004 --> added decay e- from muons
      if (trj->GetParentID() == 0){
        parentType = 0;
      } else if (pizeroList.count(trj->GetParentID())   ) {
        parentType = 111;
      } else if (muonList.count(trj->GetParentID())     ) {
        parentType = 13;
      } else if (antimuonList.count(trj->GetParentID()) ) {
        parentType = -13;
      } else if (antipionList.count(trj->GetParentID()) ) {
        parentType = -211;
      } else if (pionList.count(trj->GetParentID()) ) {
        parentType = 211;
      } else {  // no identified parent, but not a primary
        parentType = 999;
      }

      // G4cout << parentType << " " << ipnu << " " 
      //	     << id << " " << energy << "\n";

      // fill ntuple
      float dir[3];
      float pdir[3];
      float stop[3];
      float start[3];
      for (int l=0;l<3;l++)
      {
        dir[l]= mom[l]/mommag; // direction 
        pdir[l]=mom[l];        // momentum-vector 
        stop[l]=Stop[l]/cm; // stopping point 
        start[l]=Start[l]/cm; // starting point 
        //	G4cout<<"part 2 start["<<l<<"]: "<< start[l] <<G4endl;
      }


      // Add the track to the TClonesArray, watching out for times
      if ( ! ( (ipnu==22)&&(parentType==999))  ) {
        int choose_event=0;

        if (ngates)
        {

          if ( ttime > WCDM->GetTriggerTime(0)+950. && WCDM->GetTriggerTime(1)+950. > ttime ) choose_event=1; 
          if ( ttime > WCDM->GetTriggerTime(1)+950. && WCDM->GetTriggerTime(2)+950. > ttime ) choose_event=2; 
          if (choose_event >= ngates) choose_event = ngates-1; // do not overflow the number of events

        }

        wcsimrootevent= wcsimrootsuperevent->GetTrigger(choose_event);
        wcsimrootevent->AddTrack(ipnu, 
            flag, 
            mass, 
            mommag, 
            energy,
            startvol, 
            stopvol, 
            dir, 
            pdir, 
            stop,
            start,
            parentType,
            ttime,
            id,
            parentID); 
      }


      if (detectorConstructor->SavePi0Info() == true)
      {
        G4cout<<"Pi0 parentType: " << parentType <<G4endl;
        if (parentType == 111)
        {
          if (r>1)
            G4cout<<"WARNING: more than 2 primary gammas found"<<G4endl;
          else
          {

            for (int y=0;y<3;y++)
            {
              pi0Vtx[y] = start[y];
              gammaVtx[r][y] = stop[y];
            }

            gammaID[r] = id;
            gammaE[r] = energy;
            r++;

            //amb79
            G4cout<<"Pi0 data: " << id <<G4endl;
            wcsimrootevent->SetPi0Info(pi0Vtx, gammaID, gammaE, gammaVtx);
          }
        }
      }
    }
  }

  // Add the Cherenkov hits
  wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);

  //  wcsimrootevent->SetNumTubesHit(jhfNtuple.numTubesHit);
#ifdef _SAVE_RAW_HITS
  if (WCHC) 
  {
    G4cout<<"RAW HITS"<<G4endl;
    wcsimrootevent->SetNumTubesHit(WCHC->entries());
    for (int k=0;k<WCHC->entries();k++){

      std::vector<float> truetime;
      std::vector<int>   primaryParentID;

      int tubeID  = (*WCHC)[k]->GetTubeID();
      int totalpe = (*WCHC)[k]->GetTotalPe();

      for (int l=0;l<totalpe;l++)
      {
        truetime.push_back((*WCHC)[k]->GetTime(l));
        primaryParentID.push_back((*WCHC)[k]->GetParentID(l));
      }

      wcsimrootevent->AddCherenkovHit(tubeID,
          truetime,
          primaryParentID); 
    } 
  }

#endif

  // Add the digitized hits

  if (WCDC) 
  {
    G4float sumq_tmp = 0.;

    for ( int index = 0 ; index < ngates ; index++)
    {	
      sumq_tmp = 0.0;	
      G4float gatestart;
      int countdigihits = 0;
      wcsimrootevent = wcsimrootsuperevent->GetTrigger(index);
      for (int k=0;k<WCDC->entries();k++)
      {
        if ( (*WCDC)[k]->HasHitsInGate(index)) {
          wcsimrootevent->AddCherenkovDigiHit((*WCDC)[k]->GetPe(index),
              (*WCDC)[k]->GetTime(index),
              (*WCDC)[k]->GetTubeID());  
          sumq_tmp = sumq_tmp + (*WCDC)[k]->GetPe(index);

          countdigihits++;
          wcsimrootevent->SetNumDigitizedTubes(countdigihits);
          wcsimrootevent->SetSumQ(sumq_tmp);
        }
      }

      gatestart = WCDM->GetTriggerTime(index);
      WCSimRootEventHeader*HH = wcsimrootevent->GetHeader();
      HH->SetDate(int(gatestart));
    }

    // end of loop over WC trigger gates --> back to the main sub-event
    wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);

  }


  for (int i = 0 ; i < wcsimrootsuperevent->GetNumberOfEvents(); i++) {
    wcsimrootevent = wcsimrootsuperevent->GetTrigger(i);
    G4cout << ">>>Root event "
      <<std::setw(5)<<wcsimrootevent->GetHeader()->GetEvtNum()<<"\n";
  }

#ifdef _SAVE_RAW_HITS
  //if (WCHC)
  //     G4cout <<"WC:"<<std::setw(4)<<wcsimrootevent->GetNcherenkovhits()<<"  ";
  //    if (WCFVHC)
  //G4cout <<"WCFV:"<<std::setw(4)<<wcsimrootevent->GetINcherenkovhits()<<"  ";
#endif

  //  if (WCFVDC){
  //G4cout <<"WCFV digi:"<<std::setw(4)<<wcsimrootevent->GetNcherenkovdigihits()<<"  ";
  //G4cout <<"WCFV digi sumQ:"<<std::setw(4)<<wcsimrootevent->GetSumQ()<<"  ";
  //  }

  TTree* tree = GetRunAction()->GetTree();
  tree->Fill();
  TFile* hfile = tree->GetCurrentFile();
  // MF : overwrite the trees -- otherwise we have as many copies of the tree
  // as we have events. All the intermediate copies are incomplete, only the
  // last one is useful --> huge waste of disk space.
  hfile->Write("",TObject::kOverwrite);

  // M Fechner : reinitialize the super event after the writing is over
  wcsimrootsuperevent->ReInitialize();

  }
