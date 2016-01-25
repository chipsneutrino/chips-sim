#include "WCSimPrimaryGeneratorAction.hh"
#include "WCSimDetectorConstruction.hh"
#include "WCSimPrimaryGeneratorMessenger.hh"
#include "WCSimTruthSummary.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "Randomize.hh"
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>

#include "G4Navigator.hh"
#include "G4TransportationManager.hh"

using std::vector;
using std::string;
using std::fstream;

vector<string> tokenize(string separators, string input);

inline vector<string> readInLine(fstream& inFile, int lineSize, char* inBuf)
{
  // Read in line break it up into tokens
  inFile.getline(inBuf, lineSize);
  return tokenize(" $", inBuf);
}

inline float atof(const string& s)
{
  return std::atof(s.c_str());
}
inline int atoi(const string& s)
{
  return std::atoi(s.c_str());
}

WCSimPrimaryGeneratorAction::WCSimPrimaryGeneratorAction(
    WCSimDetectorConstruction* myDC) :
  myDetector(myDC)
{
  //T. Akiri: Initialize GPS to allow for the laser use
  MyGPS = new G4GeneralParticleSource();

  //---Set defaults. Do once at beginning of session.

  G4int n_particle = 1;
  particleGun = new G4ParticleGun(n_particle);
  particleGun->SetParticleEnergy(1.0 * GeV);
  particleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.0));

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  particleGun->SetParticleDefinition(particleTable->FindParticle(particleName =
        "mu+"));

  particleGun->SetParticlePosition(G4ThreeVector(0. * m, 0. * m, 0. * m));

  messenger = new WCSimPrimaryGeneratorMessenger(this);
  useMulineEvt = true;
  useNormalEvt = false;
}

WCSimPrimaryGeneratorAction::~WCSimPrimaryGeneratorAction()
{
  inputFile.close();
  delete particleGun;
  delete MyGPS;   //T. Akiri: Delete the GPS variable
  delete messenger;
}

void WCSimPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{

  // Reset the truth information
  fTruthSummary.ResetValues();

  // Temporary kludge to turn on/off vector text format
  G4bool useNuanceTextFormat = true;

  // Do for every event
  if (useMulineEvt)
  {

    if (!inputFile.is_open())
    {
      G4cout << "Set a vector file using the command /mygen/vecfile name"
        << G4endl;
      return;
    }

    if (useNuanceTextFormat)
    {
      const int lineSize=100;
      char inBuf[lineSize];
      vector<string> token(1);

      token = readInLine(inputFile, lineSize, inBuf);

      if (token.size() == 0)
      {
        G4cout << "end of nuance vector file!" << G4endl;
        if(LoadNextVectorFile())
        {
          G4cout << "Loading next vector file" << G4endl;
          token = readInLine(inputFile, lineSize, inBuf);
        }
      }

      if( token.size() != 0)
      {
        if (token[0] != "begin")
        {
          G4cout << "unexpected line begins with " << token[0] << G4endl;
        }
        else   // normal parsing begins here
        {
          // Read the nuance line (ignore value now)
          token = readInLine(inputFile, lineSize, inBuf);
          // The nuance line contains the interaction mode. Bag it and tag it.
          fTruthSummary.SetInteractionMode(atoi(token[1]));

          // Read the Vertex line
          token = readInLine(inputFile, lineSize, inBuf);
          G4ThreeVector nuVtx = G4ThreeVector(atof(token[1])*cm,atof(token[2])*cm,atof(token[3])*cm);
          if(fUseXAxisForBeam)
          {
            nuVtx = G4ThreeVector(atof(token[3])*cm,atof(token[2])*cm,atof(token[1])*cm);
          }
          if(fUseRandomVertex)
          {
            nuVtx = GenerateRandomVertex();
          }
          fTruthSummary.SetVertex(nuVtx.x(),nuVtx.y(),nuVtx.z());
          double nuVtxT = GetBeamSpillEventTime();
          fTruthSummary.SetVertexT(nuVtxT);

          // true : Generate vertex in Rock , false : Generate vertex in WC tank
          SetGenerateVertexInRock(false);

          // Next we read the incoming neutrino and target
          // First, the neutrino line
          token=readInLine(inputFile, lineSize, inBuf);
          fTruthSummary.SetBeamPDG(atoi(token[1]));
          fTruthSummary.SetBeamEnergy(atof(token[2])*MeV);
          fTruthSummary.SetBeamDir(atof(token[3]),atof(token[4]),atof(token[5]));
          if(fUseXAxisForBeam)
          {
            fTruthSummary.SetBeamDir(atof(token[5]),atof(token[4]),atof(token[3]));
          }

          // Now read the target line

          token=readInLine(inputFile, lineSize, inBuf);
          fTruthSummary.SetTargetPDG(atoi(token[1]));
          fTruthSummary.SetTargetEnergy(atof(token[2])*MeV);
          fTruthSummary.SetTargetDir(atof(token[3]),atof(token[4]),atof(token[5]));
          if(fUseXAxisForBeam)
          {
            fTruthSummary.SetTargetDir(atof(token[5]),atof(token[4]),atof(token[3]));
          }

          // Now read the outgoing particles
          // These we will simulate.
          while ( token=readInLine(inputFile, lineSize, inBuf),
              token[0] == "track" )
          {
            // We are only interested in the particles
            // that leave the nucleus, tagged by "0"

            // G4cout << "Token[6] = " << token[6] << std::endl;
            if ( token[6] == "0")
            {
              // Leigh Hack for Coh events with the nucleus in the final state
              if(token[1] == "8016") token[1] = "1000080160";
              if(token[1] == "1001") token[1] = "1000010010";

              this->FireParticleGunFromTrackLine(anEvent, nuVtx, nuVtxT, token, fUseXAxisForBeam, false);
            }
          }
        }
      }
    }
    else
    {    // old muline format
      double nuEnergy, energy;
      double xPos, yPos, zPos;
      double xDir, yDir, zDir; 
      inputFile >> nuEnergy >> energy >> xPos >> yPos >> zPos 
        >> xDir >> yDir >> zDir;

      G4double random_z = ((myDetector->GetWaterTubePosition())
          - .5*(myDetector->GetWaterTubeLength()) 
          + 1.*m + 15.0*m*G4UniformRand())/m;
      zPos = random_z;
      G4ThreeVector vtx = G4ThreeVector(xPos, yPos, zPos);
      G4ThreeVector dir = G4ThreeVector(xDir,yDir,zDir);

      particleGun->SetParticleEnergy(energy*MeV);
      particleGun->SetParticlePosition(vtx);
      particleGun->SetParticleMomentumDirection(dir);
      particleGun->GeneratePrimaryVertex(anEvent);

      // Leigh: Will likely never use this, but assume muon?
      fTruthSummary.SetBeamPDG(13);
      fTruthSummary.SetBeamEnergy(energy*MeV);
      fTruthSummary.SetVertex(vtx.x(),vtx.y(),vtx.z());
      fTruthSummary.SetVertexT(0.0);
      fTruthSummary.SetBeamDir(dir.x(),dir.y(),dir.z());
    }
  }

  else if (useNormalEvt)
  {      // manual gun operation
    particleGun->GeneratePrimaryVertex(anEvent);

    G4ThreeVector P =anEvent->GetPrimaryVertex()->GetPrimary()->GetMomentum();
    G4ThreeVector vtx=anEvent->GetPrimaryVertex()->GetPosition();
    G4double m =anEvent->GetPrimaryVertex()->GetPrimary()->GetMass();
    G4int pdg =anEvent->GetPrimaryVertex()->GetPrimary()->GetPDGcode();

    G4ThreeVector dir = P.unit();
    G4double E = std::sqrt((P.dot(P))+(m*m));

    fTruthSummary.SetVertex(vtx.x(),vtx.y(),vtx.z());
    fTruthSummary.SetVertexT(anEvent->GetPrimaryVertex()->GetT0());
    fTruthSummary.SetBeamEnergy(E);
    fTruthSummary.SetBeamPDG(pdg);
    fTruthSummary.SetBeamDir(dir.x(),dir.y(),dir.z());
  }
  else if (useLaserEvt)
  {
    //T. Akiri: Create the GPS LASER event
    MyGPS->GeneratePrimaryVertex(anEvent);

    G4ThreeVector P   =anEvent->GetPrimaryVertex()->GetPrimary()->GetMomentum();
    G4ThreeVector vtx =anEvent->GetPrimaryVertex()->GetPosition();
    G4int pdg         =anEvent->GetPrimaryVertex()->GetPrimary()->GetPDGcode();

    G4ThreeVector dir  = P.unit();
    //Particles are photons, so p=E
    G4double E         = std::sqrt((P.dot(P)));

    fTruthSummary.SetVertex(vtx.x(),vtx.y(),vtx.z());
    fTruthSummary.SetVertexT(anEvent->GetPrimaryVertex()->GetT0());
    fTruthSummary.SetBeamEnergy(E);
    fTruthSummary.SetBeamPDG(pdg);
    fTruthSummary.SetBeamDir(dir.x(),dir.y(),dir.z());
  }
  else if (useGpsEvt)
  {
    //Just like for LASER events but with massive particles
    MyGPS->GeneratePrimaryVertex(anEvent);

    G4ThreeVector P   =anEvent->GetPrimaryVertex()->GetPrimary()->GetMomentum();
    G4ThreeVector vtx =anEvent->GetPrimaryVertex()->GetPosition();
    G4double m       =anEvent->GetPrimaryVertex()->GetPrimary()->GetMass();
    G4int pdg         =anEvent->GetPrimaryVertex()->GetPrimary()->GetPDGcode();

    G4ThreeVector dir  = P.unit();
    G4double E         = std::sqrt((P.dot(P))+(m*m));

    fTruthSummary.SetVertex(vtx.x(),vtx.y(),vtx.z());
    fTruthSummary.SetVertexT(anEvent->GetPrimaryVertex()->GetT0());
    fTruthSummary.SetBeamEnergy(E);
    fTruthSummary.SetBeamPDG(pdg);
    fTruthSummary.SetBeamDir(dir.x(),dir.y(),dir.z());
  }
  else if(useOverlayEvt){
    GenerateOverlayEvents(anEvent);
  }

}

void WCSimPrimaryGeneratorAction::GenerateOverlayEvents(G4Event *evt){

  // For overlay events we need to read from two different vector files.
  const int lineSize=100;
  char inBuf[lineSize];
  std::vector<std::string> token(1);
  std::vector<std::string> overToken(1);

  //  G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();

  // Read the next line of the next event first.
  token = readInLine(inputFile, lineSize, inBuf);
  if (token.size() == 0)
  {
    G4cout << "end of nuance vector file!" << G4endl;
    if(LoadNextVectorFile())
    {
      G4cout << "Loading next vector file" << G4endl;
      token = readInLine(inputFile, lineSize, inBuf);
    }
  }

  // Now read the next line of the next overlay event.
  overToken = readInLine(fOverlayFile,lineSize,inBuf);
  if(overToken.size() == 0){
    if(LoadNextOverlayFile()){
      overToken = readInLine(fOverlayFile,lineSize,inBuf);
    }
  }

  // Check to see if we can read an event from each file.
  if(overToken.size() == 0 || token.size() == 0){
    std::cout << "Problem with overlay stuff: " << token.size() << ", " << overToken.size() << std::endl;
    return;
  }

  std::cout << "Trying to read an overlay event" << std::endl;

  // Now actually read the standard event
  // The line we currently has is the "begin" line, so move on.
  token = readInLine(inputFile, lineSize, inBuf);
  // The nuance line contains the interaction mode. Bag it and tag it.
  fTruthSummary.SetInteractionMode(atoi(token[1]));

  // Read the vertex line.
  token = readInLine(inputFile, lineSize, inBuf);
  G4ThreeVector nuVtx; 
  if(fUseRandomVertex){
    nuVtx = GenerateRandomVertex();
  }
  else{
    if(fUseXAxisForBeam){
      nuVtx = G4ThreeVector(atof(token[3])*cm,atof(token[2])*cm,atof(token[1])*cm);
    }
    else{
      nuVtx = G4ThreeVector(atof(token[1])*cm,atof(token[2])*cm,atof(token[3])*cm);
    }
  }
  fTruthSummary.SetVertex(nuVtx.x(),nuVtx.y(),nuVtx.z());

  // Make a vertex time in the beam window.
  double nuVtxT = this->GetBeamSpillEventTime();
  fTruthSummary.SetVertexT(nuVtxT);

  // Next we read the incoming neutrino and target.
  // For all particles we swap x and z since GENIE assumes the beam is in the z-direction.
  // First, the neutrino line
  token=readInLine(inputFile, lineSize, inBuf);
  fTruthSummary.SetBeamPDG(atoi(token[1]));
  fTruthSummary.SetBeamEnergy(atof(token[2])*MeV);
  if(fUseXAxisForBeam){
    fTruthSummary.SetBeamDir(atof(token[5]),atof(token[4]),atof(token[3]));
  }
  else{
    fTruthSummary.SetBeamDir(atof(token[3]),atof(token[4]),atof(token[5]));
  }

  // Now read the target line
  token=readInLine(inputFile, lineSize, inBuf);
  fTruthSummary.SetTargetPDG(atoi(token[1]));
  fTruthSummary.SetTargetEnergy(atof(token[2])*MeV);
  if(fUseXAxisForBeam){
    fTruthSummary.SetTargetDir(atof(token[5]),atof(token[4]),atof(token[3]));
  }
  else{
    fTruthSummary.SetTargetDir(atof(token[5]),atof(token[4]),atof(token[3]));
  }

  // Now read the outgoing particles
  while ( token=readInLine(inputFile, lineSize, inBuf),
      token[0] == "track" )
  {
    // We are only interested in the particles that leave the nucleus, tagged by "0"
    if ( token[6] == "0" && atof(token[5]) > -999)
    {
      // Leigh Hack for Coh events with the nucleus in the final state
      if(token[1] == "8016") token[1] = "1000080160";
      if(token[1] == "1001") token[1] = "1000010010";
      // Interface with the particle gun to generate the event
      FireParticleGunFromTrackLine(evt, nuVtx, nuVtxT, token, fUseXAxisForBeam, false);
    }
  }

  // We have made the standard event, now to deal with the cosmic event.
  // These cosmics have dummy entries for the neutrino and target, so 
  // we want to ignore everything other than the muon.

  // Currently on the "begin" line for the overlay, so read next.
  overToken = readInLine(fOverlayFile,lineSize,inBuf); // Interaction code, ignore.
  overToken = readInLine(fOverlayFile,lineSize,inBuf); // Vertex - need this!
  G4ThreeVector cosmicVtx = G4ThreeVector(atof(overToken[1])*cm,atof(overToken[2])*cm,atof(overToken[3])*cm);
  fTruthSummary.SetOverlayVertex(cosmicVtx.x(),cosmicVtx.y(),cosmicVtx.z());
  // We also want to define the cosmic to be at a different time.
  // For now, assume flat distribution +/- 100ns from the event.
  double cosmicTime = nuVtxT; // We will change this in FireParticleGun
  fTruthSummary.SetOverlayVertexT(cosmicTime);
  // Just read in the final state particle such that they have the last token equal to 0.
  while ( overToken=readInLine(fOverlayFile, lineSize, inBuf),
      overToken[0] == "track" ){
    if ( (overToken[6] == "0") && atof(overToken[5]) > -999){
      // Leigh Hack for Coh events with the nucleus in the final state
      if(overToken[1] == "8016") overToken[1] = "1000080160";
      if(overToken[1] == "1001") overToken[1] = "1000010010";
  
      // No need for XZ swaps with the cosmics overlays.
      FireParticleGunFromTrackLine(evt, cosmicVtx, cosmicTime, overToken, false, true);
    }  
  }  

}

// Set up the particle gun for all tracks in vector files (including overlays).
void WCSimPrimaryGeneratorAction::FireParticleGunFromTrackLine(G4Event *evt, G4ThreeVector &vtx, double& vtxTime, std::vector<std::string> &tokens, bool swapXZ, bool isOverlay){

  // Double check the first token is actually a track.
  if(tokens[0] != "track") return;

  G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();

  G4int pdgid = atoi(tokens[1]);
  G4double energy = atof(tokens[2])*MeV;
  G4ThreeVector dir = G4ThreeVector(atof(tokens[3]),atof(tokens[4]),atof(tokens[5]));

  if(swapXZ){
    // Swap the x and z coordinates if we need to.
    dir = G4ThreeVector(dir.z(),dir.y(),dir.x());
  }

  // For overlay events, check the muon actually enters the detector. If not, don't bother tracking it.
  // If so, update the vertex to be the point at which the muon enters the detector.
  if(isOverlay){
    double timeOffset = 0;
    bool inDet = this->UpdateOverlayVertexAndEnergy(vtx,timeOffset,dir,energy);
    if(!inDet) return;
    fTruthSummary.SetOverlayVertex(vtx.x(),vtx.y(),vtx.z());
    vtxTime = vtxTime - timeOffset + (G4UniformRand()-0.5)*200*ns;
    fTruthSummary.SetOverlayVertexT(vtxTime);
  }

  // Get the particle mass and hence kinetic energy  
  particleGun->SetParticleDefinition(particleTable->FindParticle(pdgid));
  G4double mass = particleGun->GetParticleDefinition()->GetPDGMass();
  G4double ekin = energy - mass;

  particleGun->SetParticleEnergy(ekin);
  particleGun->SetParticlePosition(vtx);
  particleGun->SetParticleTime(vtxTime);
  particleGun->SetParticleMomentumDirection(dir);
  particleGun->GeneratePrimaryVertex(evt);

  // Now for the truth summary update
  if(!isOverlay){
    fTruthSummary.AddPrimary(pdgid,energy,TVector3(dir.x(),dir.y(),dir.z()));
  }
  else{
    fTruthSummary.AddOverlayTrack(pdgid,energy,TVector3(dir.x(),dir.y(),dir.z()));
  }

}

double WCSimPrimaryGeneratorAction::GetBeamSpillEventTime() const{

  double beamSpillDuration = 10000*ns;

  return G4UniformRand() * beamSpillDuration;
}

G4ThreeVector WCSimPrimaryGeneratorAction::GenerateRandomVertex() const{
  std::cerr << " About to do the random vertex " << std::endl;
  // Genie events are generates at (0,0,0) - we want to place them randomly in a 
  // box or cylinder
  G4ThreeVector rdmVtx;
  double border = fFidBorder * m;
  if( myDetector->GetIsMailbox() )
  {
    G4double xMax = myDetector->GetWCCylInfo(0) - 2*border;
    G4double yMax = myDetector->GetWCCylInfo(1) - 2*border;
    G4double zMax = myDetector->GetWCCylInfo(2) - 2*border;
    double vtxX = (G4UniformRand() - 0.5) * xMax * cm;
    double vtxY = (G4UniformRand() - 0.5) * yMax * cm;
    double vtxZ = (G4UniformRand() - 0.5) * zMax * cm;
    rdmVtx = G4ThreeVector(vtxX, vtxY, vtxZ);
  }
  else
  {
    double rRand,thetaRand,zRand;
    rRand = sqrt(G4UniformRand()) * 0.5 * (myDetector->GetWCCylInfo(0)-border) * cm;
    zRand = (G4UniformRand() - 0.5) * (myDetector->GetWCCylInfo(2)-2*border) * cm;
    thetaRand = (G4UniformRand()) * 2.0 * M_PI;
    rdmVtx = G4ThreeVector(rRand*cos(thetaRand),rRand*sin(thetaRand),zRand); 
  }
  return rdmVtx;
}

// For the overlay events, need to find a fake vertex just inside the detector, and adjust the energy correspondingly.
bool WCSimPrimaryGeneratorAction::UpdateOverlayVertexAndEnergy(G4ThreeVector &vtx, double &timeOffset, G4ThreeVector dir, double &energy){

  G4ThreeVector newVtx;

  bool withinDet = false;
  // Use vector algebra.
  // Firstly, find the position of the track at the top of the detector.
  double detTop = 0.5 * myDetector->GetWCCylInfo(2)*cm;
  // Check over the detector height to see if we are in the detector.
  for(int i = 0; i < 2*(int)detTop; ++i){
    double thisZ = detTop - i;
    newVtx = vtx + ((thisZ - vtx.z()) / dir.z())*dir;
    if(newVtx.r() < (0.5*myDetector->GetWCCylInfo(0)*cm)){
      withinDet = true;
      break;
    }
  }
  bool energyOK = false;
  if(withinDet){
    double dist = (newVtx-vtx).mag(); // In mm
    double muonSpeed = 2.9979e8 * m / s;
    energy -= dist * 2.0 / cm; // Assume 2.0 MeV/cm energy loss
    timeOffset = dist / muonSpeed;
    if(energy > 0){
      energyOK = true;
      vtx = newVtx;
    }
  }
  
  if(withinDet && energyOK) return true;
  else return false;
  
}

// Returns a vector with the tokens
vector<string> tokenize(string separators, string input)
{
  std::size_t startToken = 0, endToken; // Pointers to the token pos
  vector<string> tokens;  // Vector to keep the tokens

  if (separators.size() > 0 && input.size() > 0)
  {

    while( startToken < input.size() )
    {
      // Find the start of token
      startToken = input.find_first_not_of( separators, startToken );

      // If found...
      if( startToken != input.npos ) 
      {
        // Find end of token
        endToken = input.find_first_of( separators, startToken );
        if( endToken == input.npos )
          // If there was no end of token, assign it to the end of string
          endToken = input.size();

        // Extract token
        tokens.push_back( input.substr( startToken, endToken - startToken ) );

        // Update startToken
        startToken = endToken;
      }
    }
  }

  return tokens;
}

