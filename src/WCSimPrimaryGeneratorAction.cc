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

vector<string> tokenize( string separators, string input );

inline vector<string> readInLine(fstream& inFile, int lineSize, char* inBuf)
{
  // Read in line break it up into tokens
  inFile.getline(inBuf,lineSize);
  return tokenize(" $", inBuf);
}

inline float atof( const string& s ) {return std::atof( s.c_str() );}
inline int   atoi( const string& s ) {return std::atoi( s.c_str() );}

WCSimPrimaryGeneratorAction::WCSimPrimaryGeneratorAction(
    WCSimDetectorConstruction* myDC)
:myDetector(myDC)
{
  //T. Akiri: Initialize GPS to allow for the laser use 
  MyGPS = new G4GeneralParticleSource();

  // Initialize to zero
//  mode = 0;
//  vtxvol = 0;
//  vtx = G4ThreeVector(0.,0.,0.);
//  nuEnergy = 0.;
  _counterRock=0; // counter for generated in Rock
  _counterCublic=0; // counter generated

  //---Set defaults. Do once at beginning of session.

  G4int n_particle = 1;
  particleGun = new G4ParticleGun(n_particle);
  particleGun->SetParticleEnergy(1.0*GeV);
  particleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.0));

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  particleGun->
    SetParticleDefinition(particleTable->FindParticle(particleName="mu+"));

  particleGun->
    SetParticlePosition(G4ThreeVector(0.*m,0.*m,0.*m));

  messenger = new WCSimPrimaryGeneratorMessenger(this);
  useMulineEvt = true;
  useNormalEvt = false;
}

WCSimPrimaryGeneratorAction::~WCSimPrimaryGeneratorAction()
{
  if (IsGeneratingVertexInRock()){
    G4cout << "Fraction of Rock volume is : " << G4endl;
    G4cout << " Random number generated in Rock / in Cublic = " 
      << _counterRock << "/" << _counterCublic 
      << " = " << _counterRock/(G4double)_counterCublic << G4endl;
  }
  inputFile.close();
  delete particleGun;
  delete MyGPS;   //T. Akiri: Delete the GPS variable
  delete messenger;
}

void WCSimPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{

  // Reset the truth information
  fTruthSummary.ResetValues();

  // We will need a particle table
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

  // Temporary kludge to turn on/off vector text format 

  G4bool useNuanceTextFormat = true;
  G4bool useXAxisForBeam = true;    // AJP 30/May/13

  // Do for every event

  if (useMulineEvt)
  { 

    if ( !inputFile.is_open() )
    {
      G4cout << "Set a vector file using the command /mygen/vecfile name"
        << G4endl;
      return;
    }

    if (useNuanceTextFormat)
    {
      const int lineSize=100;
      char      inBuf[lineSize];
      vector<string> token(1);

      token = readInLine(inputFile, lineSize, inBuf);

      if (token.size() == 0) 
      {
        G4cout << "end of nuance vector file!" << G4endl;
      }
      else if (token[0] != "begin")
      {
        G4cout << "unexpected line begins with " << token[0] << G4endl;
      }
      else   // normal parsing begins here
      {
        // Read the nuance line 
        token = readInLine(inputFile, lineSize, inBuf);
        // The nuance line contains the interaction mode. Bag it and tag it.
        fTruthSummary.SetInteractionMode(atoi(token[1]));

        // Read the Vertex line
        token = readInLine(inputFile, lineSize, inBuf);
        G4ThreeVector nuVtx = G4ThreeVector(atof(token[1])*cm,
            atof(token[2])*cm,
            atof(token[3])*cm);
        // Set the vertex
        fTruthSummary.SetVertex(atof(token[1])*cm,atof(token[2])*cm,atof(token[3])*cm);

        if(useXAxisForBeam)
        {
          nuVtx = G4ThreeVector(atof(token[3])*cm,
              atof(token[2])*cm,
              atof(token[1])*cm);
          fTruthSummary.SetVertex(atof(token[3])*cm,atof(token[2])*cm,atof(token[1])*cm);
        }

        std::cerr << " About to do the random vertex " << std::endl;
        G4bool useRandomVertex = true;
        if(useRandomVertex)
        {
          std::cerr << " About to do the random vertex " << std::endl;
          // Genie events are generates at (0,0,0) - we want to place them randomly in a 
          // box or cylinder
          if( myDetector->GetIsMailbox() )
          {
            G4double xMax = myDetector->GetWCCylInfo(0);
            G4double yMax = myDetector->GetWCCylInfo(1);
            G4double zMax = myDetector->GetWCCylInfo(2);
            double vtxX = (G4UniformRand() - 0.5) * xMax * cm;
            double vtxY = (G4UniformRand() - 0.5) * yMax * cm;
            double vtxZ = (G4UniformRand() - 0.5) * zMax * cm;
            nuVtx = G4ThreeVector(vtxX, vtxY, vtxZ);
            fTruthSummary.SetVertex(vtxX,vtxY,vtxZ);
          }
          else
          {
            double rRand,thetaRand,zRand;
            rRand = sqrt(G4UniformRand()) * 0.5 * myDetector->GetWCCylInfo(0) * cm;
            zRand = (G4UniformRand() - 0.5) * myDetector->GetWCCylInfo(2) * cm;
            thetaRand = (G4UniformRand()) * 2.0 * M_PI;
            nuVtx = G4ThreeVector(rRand*cos(thetaRand),rRand*sin(thetaRand),zRand); 
            fTruthSummary.SetVertex(rRand*cos(thetaRand),rRand*sin(thetaRand),zRand); 
          }
        }  

        // true : Generate vertex in Rock , false : Generate vertex in WC tank
        SetGenerateVertexInRock(false);

        // Next we read the incoming neutrino and target

        // First, the neutrino line
        token=readInLine(inputFile, lineSize, inBuf);
        fTruthSummary.SetBeamPDG(atoi(token[1]));
        fTruthSummary.SetBeamEnergy(atof(token[2])*MeV);
        fTruthSummary.SetBeamDir(atof(token[3]),atof(token[4]),atof(token[5]));
        if(useXAxisForBeam)
        {
          fTruthSummary.SetBeamDir(atof(token[5]),atof(token[4]),atof(token[3]));
        }

        // Now read the target line

        token=readInLine(inputFile, lineSize, inBuf);
        fTruthSummary.SetTargetPDG(atoi(token[1]));
        fTruthSummary.SetTargetEnergy(atof(token[2])*MeV);
        fTruthSummary.SetTargetDir(atof(token[3]),atof(token[4]),atof(token[5]));
        if(useXAxisForBeam)
        {
          fTruthSummary.SetTargetDir(atof(token[5]),atof(token[4]),atof(token[3]));
        }

        /* Leigh: This next line doesn't exist, so no idea why it is here. 
           There is no counter in the nuance-style files to count which event this is.
        // Read the info line, basically a dummy
        token=readInLine(inputFile, lineSize, inBuf);
        G4cout << "Vector File Record Number " << token[2] << G4endl;
        vecRecNumber = atoi(token[2]);

        // Now read the outgoing particles
        // These we will simulate.
         */

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
            G4int pdgid = atoi(token[1]);
            G4double energy = atof(token[2])*MeV;
            G4ThreeVector dir = G4ThreeVector(atof(token[3]),
                atof(token[4]),
                atof(token[5]));
            if(useXAxisForBeam)
            {
              dir = G4ThreeVector(atof(token[5]),
                  atof(token[4]),
                  atof(token[3]));
            }

            particleGun->SetParticleDefinition(particleTable->FindParticle(pdgid));
            G4double mass = particleGun->GetParticleDefinition()->GetPDGMass();

            G4double ekin = energy - mass;

            particleGun->SetParticleEnergy(ekin);
            //G4cout << "Particle: " << pdgid << " KE: " << ekin << G4endl;
            particleGun->SetParticlePosition(nuVtx);
            particleGun->SetParticleMomentumDirection(dir);
            particleGun->GeneratePrimaryVertex(anEvent);

            // Add the primary to the truth summary object
            fTruthSummary.AddPrimary(pdgid,energy,TVector3(dir.x(),dir.y(),dir.z()));
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
      fTruthSummary.SetBeamDir(dir.x(),dir.y(),dir.z());
    }
  }

  else if (useNormalEvt)
  {      // manual gun operation
    particleGun->GeneratePrimaryVertex(anEvent);

    G4ThreeVector P  =anEvent->GetPrimaryVertex()->GetPrimary()->GetMomentum();
    G4ThreeVector vtx=anEvent->GetPrimaryVertex()->GetPosition();
    G4double m       =anEvent->GetPrimaryVertex()->GetPrimary()->GetMass();
    G4int pdg        =anEvent->GetPrimaryVertex()->GetPrimary()->GetPDGcode();

    G4ThreeVector dir  = P.unit();
    G4double E         = std::sqrt((P.dot(P))+(m*m));

    fTruthSummary.SetVertex(vtx.x(),vtx.y(),vtx.z());
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
    fTruthSummary.SetBeamEnergy(E);
    fTruthSummary.SetBeamPDG(pdg);
    fTruthSummary.SetBeamDir(dir.x(),dir.y(),dir.z());
  }

}

// Returns a vector with the tokens
vector<string> tokenize( string separators, string input ) 
{
  int startToken = 0, endToken; // Pointers to the token pos
  vector<string> tokens;  // Vector to keep the tokens

  if( separators.size() > 0 && input.size() > 0 ) 
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

