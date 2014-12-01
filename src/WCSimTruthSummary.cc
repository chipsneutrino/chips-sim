#include <iostream>
#include <vector>

#include <TVector3.h>
#include <TLorentzVector.h>

#include "WCSimTruthSummary.hh"

ClassImp(WCSimTruthSummary)

// Standard constructor
WCSimTruthSummary::WCSimTruthSummary() : TObject() {
  this->ResetValues();
}

// Copy constructor
WCSimTruthSummary::WCSimTruthSummary(const WCSimTruthSummary &ts) : TObject(ts) {
  fVertex = ts.GetVertex();

  fInteractionMode = ts.GetInteractionMode();

  fBeamPDG = ts.GetBeamPDG();
  fBeamEnergy = ts.GetBeamEnergy();
  fBeamDir = ts.GetBeamDir();

  fTargetPDG = ts.GetTargetPDG();
  fTargetEnergy = ts.GetTargetEnergy();
  fTargetDir = ts.GetTargetDir();

  fPrimaryPDGs = ts.GetPrimaryPDGs();
  fPrimaryEnergies = ts.GetPrimaryEnergies();
  fPrimaryDirs = ts.GetPrimaryDirs();
}

// Destructor
WCSimTruthSummary::~WCSimTruthSummary(){

}

void WCSimTruthSummary::ResetValues(){
  fVertex = TVector3(-999.,-999.,-999.);

  fInteractionMode = WCSimTruthSummary::kNotSet;

  fBeamPDG = -999;
  fBeamEnergy = -999.;
  fBeamDir = TVector3(-999.,-999.,-999.);
  fTargetPDG = -999;
  fTargetEnergy = -999.;
  fTargetDir = TVector3(-999.,-999.,-999.);

  fPrimaryPDGs.clear();
  fPrimaryEnergies.clear();
  fPrimaryDirs.clear();
}

// Get and set the vertex information
TVector3 WCSimTruthSummary::GetVertex() const{
  return fVertex;
}

void WCSimTruthSummary::SetVertex(TVector3 vtx){
  fVertex = vtx;
}

void WCSimTruthSummary::SetVertex(double x, double y, double z){
  fVertex = TVector3(x,y,z);
}

// Get the vertex components
double WCSimTruthSummary::GetVertexX() const{
  return fVertex.X();
}

double WCSimTruthSummary::GetVertexY() const{
  return fVertex.Y();
}

double WCSimTruthSummary::GetVertexZ() const{
  return fVertex.Z();
}

// Get and set the interaction mode using the same numbers as GENIE
int WCSimTruthSummary::GetInteractionMode() const{
  return fInteractionMode;
}

void WCSimTruthSummary::SetInteractionMode(int mode){
  fInteractionMode = mode;
}

// Convenience methods querying the interaction mode
bool WCSimTruthSummary::IsCCEvent() const{
  if(fInteractionMode == WCSimTruthSummary::kCCQE) return true;
  else if(fInteractionMode == WCSimTruthSummary::kCCNuPtoLPPiPlus) return true;
  else if(fInteractionMode == WCSimTruthSummary::kCCNuNtoLPPiZero) return true;
  else if(fInteractionMode == WCSimTruthSummary::kCCNuNtoLNPiPlus) return true;
  else if(fInteractionMode == WCSimTruthSummary::kCCNuBarNtoLNPiMinus) return true;
  else if(fInteractionMode == WCSimTruthSummary::kCCNuBarPtoLNPiZero) return true;
  else if(fInteractionMode == WCSimTruthSummary::kCCNuBarPtoLPPiMinus) return true;
  else if(fInteractionMode == WCSimTruthSummary::kCCDIS) return true;
  else if(fInteractionMode == WCSimTruthSummary::kCCCoh) return true;
  else return false;
}

bool WCSimTruthSummary::IsNCEvent() const{
  if(fInteractionMode == WCSimTruthSummary::kNCQE) return true;
  else if(fInteractionMode == WCSimTruthSummary::kNCNuPtoNuPPiZero) return true;
  else if(fInteractionMode == WCSimTruthSummary::kNCNuPtoNuNPiPlus) return true;
  else if(fInteractionMode == WCSimTruthSummary::kNCNuNtoNuNPiZero) return true;
  else if(fInteractionMode == WCSimTruthSummary::kNCNuNtoNuPPiMinus) return true;
  else if(fInteractionMode == WCSimTruthSummary::kNCNuBarPtoNuBarPPiZero) return true;
  else if(fInteractionMode == WCSimTruthSummary::kNCNuBarPtoNuBarNPiPlus) return true;
  else if(fInteractionMode == WCSimTruthSummary::kNCNuBarNtoNuBarNPiZero) return true;
  else if(fInteractionMode == WCSimTruthSummary::kNCNuBarNtoNuBarPPiMinus) return true;
  else if(fInteractionMode == WCSimTruthSummary::kNCDIS) return true;
  else if(fInteractionMode == WCSimTruthSummary::kNCCoh) return true;
  else return false;
}

bool WCSimTruthSummary::IsQEEvent() const{
  if(fInteractionMode == WCSimTruthSummary::kCCQE) return true;
  else if(fInteractionMode == WCSimTruthSummary::kNCQE) return true;
  else return false;
}

bool WCSimTruthSummary::IsResEvent() const{
  if(fInteractionMode == WCSimTruthSummary::kCCNuPtoLPPiPlus) return true;
  else if(fInteractionMode == WCSimTruthSummary::kCCNuNtoLPPiZero) return true;
  else if(fInteractionMode == WCSimTruthSummary::kCCNuNtoLNPiPlus) return true;
  else if(fInteractionMode == WCSimTruthSummary::kCCNuBarNtoLNPiMinus) return true;
  else if(fInteractionMode == WCSimTruthSummary::kCCNuBarPtoLNPiZero) return true;
  else if(fInteractionMode == WCSimTruthSummary::kCCNuBarPtoLPPiMinus) return true;
  else if(fInteractionMode == WCSimTruthSummary::kNCNuPtoNuPPiZero) return true;
  else if(fInteractionMode == WCSimTruthSummary::kNCNuPtoNuNPiPlus) return true;
  else if(fInteractionMode == WCSimTruthSummary::kNCNuNtoNuNPiZero) return true;
  else if(fInteractionMode == WCSimTruthSummary::kNCNuNtoNuPPiMinus) return true;
  else if(fInteractionMode == WCSimTruthSummary::kNCNuBarPtoNuBarPPiZero) return true;
  else if(fInteractionMode == WCSimTruthSummary::kNCNuBarPtoNuBarNPiPlus) return true;
  else if(fInteractionMode == WCSimTruthSummary::kNCNuBarNtoNuBarNPiZero) return true;
  else if(fInteractionMode == WCSimTruthSummary::kNCNuBarNtoNuBarPPiMinus) return true;
  else return false;
}

bool WCSimTruthSummary::IsDISEvent() const{
  if(fInteractionMode == WCSimTruthSummary::kCCDIS) return true;
  else if(fInteractionMode == WCSimTruthSummary::kNCDIS) return true;
  else return false;
}

bool WCSimTruthSummary::IsCohEvent() const{
  if(fInteractionMode == WCSimTruthSummary::kCCCoh) return true;
  else if(fInteractionMode == WCSimTruthSummary::kNCCoh) return true;
  else return false;
}

bool WCSimTruthSummary::IsNueElectronElasticEvent() const{
  if(fInteractionMode == WCSimTruthSummary::kElastic) return true;
  else return false; 
}

bool WCSimTruthSummary::IsInverseMuonDecayEvent() const{
  if(fInteractionMode == WCSimTruthSummary::kInverseMuDecay) return true;
  else return false; 
}

//  Get and set the beam information
int WCSimTruthSummary::GetBeamPDG() const{
  return fBeamPDG;
}

void WCSimTruthSummary::SetBeamPDG(int pdg){
  fBeamPDG = pdg;
}

double WCSimTruthSummary::GetBeamEnergy() const{
  return fBeamEnergy;
}

void WCSimTruthSummary::SetBeamEnergy(double en){
  fBeamEnergy = en;
}

TVector3 WCSimTruthSummary::GetBeamDir() const{
  return fBeamDir;
}

void WCSimTruthSummary::SetBeamDir(TVector3 dir){
  fBeamDir = dir;
}

void WCSimTruthSummary::SetBeamDir(double dx, double dy, double dz){
  fBeamDir = TVector3(dx,dy,dz);
}

// Get and set the target information
int WCSimTruthSummary::GetTargetPDG() const{
  return fTargetPDG;
}

void WCSimTruthSummary::SetTargetPDG(int pdg){
  fTargetPDG = pdg;
}

double WCSimTruthSummary::GetTargetEnergy() const{
  return fTargetEnergy;
}

void WCSimTruthSummary::SetTargetEnergy(double en){
  fTargetEnergy = en;
}

TVector3 WCSimTruthSummary::GetTargetDir() const{
  return fTargetDir;
}

void WCSimTruthSummary::SetTargetDir(TVector3 dir){
  fTargetDir = dir;
}

void WCSimTruthSummary::SetTargetDir(double dx, double dy, double dz){
  fTargetDir = TVector3(dx,dy,dz);
}

// Primary particle functions
void WCSimTruthSummary::AddPrimary(int pdg, double en, TVector3 dir){
  fPrimaryPDGs.push_back(pdg);
  fPrimaryEnergies.push_back(en);
  fPrimaryDirs.push_back(dir);
}

void WCSimTruthSummary::AddPrimary(int pdg, double en, double dx, double dy, double dz){
  fPrimaryPDGs.push_back(pdg);
  fPrimaryEnergies.push_back(en);
  TVector3 tmpDir(dx,dy,dz);
  fPrimaryDirs.push_back(tmpDir);
}

int WCSimTruthSummary::GetPrimaryPDG(unsigned int p) const{
  if(p < this->GetNPrimaries()){
    return fPrimaryPDGs[p]; 
  }
  else{
    std::cerr << "== Request for primary particle " << p << " of [0..." << this->GetNPrimaries()-1 << "]" << std::endl;
    return -999;
  }
}

double WCSimTruthSummary::GetPrimaryEnergy(unsigned int p) const{
  if(p < this->GetNPrimaries()){
    return fPrimaryEnergies[p]; 
  }
  else{
    std::cerr << "== Request for primary particle " << p << " of [0..." << this->GetNPrimaries()-1 <<  "]" << std::endl;
    return -999.;
  }

}

TVector3 WCSimTruthSummary::GetPrimaryDir(unsigned int p) const{
  if(p < this->GetNPrimaries()){
    return fPrimaryDirs[p]; 
  }
  else{
    std::cerr << "== Request for primary particle " << p << " of [0..." << this->GetNPrimaries()-1 <<  "]" << std::endl;
    return TVector3(-999.,-999.,-999.);
  }

}

std::vector<int> WCSimTruthSummary::GetPrimaryPDGs() const{
  return fPrimaryPDGs;
}

std::vector<double> WCSimTruthSummary::GetPrimaryEnergies() const{
  return fPrimaryEnergies;
}

std::vector<TVector3> WCSimTruthSummary::GetPrimaryDirs() const{
  return fPrimaryDirs;
}

unsigned int WCSimTruthSummary::GetNPrimaries() const{
  return fPrimaryPDGs.size();
}

// What sort of event do we have?
bool WCSimTruthSummary::IsParticleGunEvent() const{
  if(fTargetPDG == -999 && fTargetEnergy == -999.) return true;
  else return false;
}

bool WCSimTruthSummary::IsNeutrinoEvent() const{
  if(fTargetPDG != -999 && fInteractionMode !=999){
    if(abs(fBeamPDG) == 12 || abs(fBeamPDG) == 14 || abs(fBeamPDG) == 16) return true;
    else return false;
  }
  else return false;
}


