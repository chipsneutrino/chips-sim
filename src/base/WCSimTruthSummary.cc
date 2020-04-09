#include <iostream>
#include <vector>

#include <TVector3.h>
#include <TLorentzVector.h>

#include "WCSimTruthSummary.hh"

ClassImp(WCSimTruthSummary)

	// Standard constructor
	WCSimTruthSummary::WCSimTruthSummary() : TObject()
{
	this->ResetValues();
}

// Copy constructor
WCSimTruthSummary::WCSimTruthSummary(const WCSimTruthSummary &ts) : TObject(ts)
{
	fVertex = ts.GetVertex();
	fVertexT = ts.GetVertexT();

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

	fOverlayVertex = ts.GetOverlayVertex();
	fOverlayVertexT = ts.GetOverlayVertexT();
	fOverlayPDGs = ts.GetOverlayPDGs();
	fOverlayEnergies = ts.GetOverlayEnergies();
	fOverlayDirs = ts.GetOverlayDirs();
}

// Destructor
WCSimTruthSummary::~WCSimTruthSummary()
{
}

void WCSimTruthSummary::ResetValues()
{
	fVertex = TVector3(-999., -999., -999.);
	fVertexT = 0.0;

	fInteractionMode = WCSimTruthSummary::kNotSet;

	fBeamPDG = -999;
	fBeamEnergy = -999.;
	fBeamDir = TVector3(-999., -999., -999.);
	fTargetPDG = -999;
	fTargetEnergy = -999.;
	fTargetDir = TVector3(-999., -999., -999.);

	fPrimaryPDGs.clear();
	fPrimaryEnergies.clear();
	fPrimaryDirs.clear();

	fOverlayVertex = TVector3(-999., -999., -999.);
	fOverlayVertexT = 0.0;
	fOverlayPDGs.clear();
	fOverlayEnergies.clear();
	fOverlayDirs.clear();
}

// Get and set the vertex information
TVector3 WCSimTruthSummary::GetVertex() const
{
	return fVertex;
}

void WCSimTruthSummary::SetVertex(TVector3 vtx)
{
	fVertex = vtx;
}

void WCSimTruthSummary::SetVertex(double x, double y, double z)
{
	fVertex = TVector3(x, y, z);
}

void WCSimTruthSummary::SetVertex(double x, double y, double z, double t)
{
	fVertex = TVector3(x, y, z);
	fVertexT = t;
}

void WCSimTruthSummary::SetVertexT(double t)
{
	fVertexT = t;
}

// Get the vertex components
double WCSimTruthSummary::GetVertexX() const
{
	return fVertex.X();
}

double WCSimTruthSummary::GetVertexY() const
{
	return fVertex.Y();
}

double WCSimTruthSummary::GetVertexZ() const
{
	return fVertex.Z();
}

double WCSimTruthSummary::GetVertexT() const
{
	return fVertexT;
}

// Get and set the interaction mode using the same numbers as GENIE
int WCSimTruthSummary::GetInteractionMode() const
{
	return fInteractionMode;
}

void WCSimTruthSummary::SetInteractionMode(int mode)
{
	fInteractionMode = mode;
}

// Convenience methods querying the interaction mode
bool WCSimTruthSummary::IsCCEvent() const
{
	return TypeIsCCEvent(fInteractionMode);
}

bool WCSimTruthSummary::TypeIsCCEvent(int typeCode)
{
	if (typeCode == WCSimTruthSummary::kCCQE)
		return true;
	else if (typeCode == WCSimTruthSummary::kCCNuPtoLPPiPlus)
		return true;
	else if (typeCode == WCSimTruthSummary::kCCNuNtoLPPiZero)
		return true;
	else if (typeCode == WCSimTruthSummary::kCCNuNtoLNPiPlus)
		return true;
	else if (typeCode == WCSimTruthSummary::kCCNuBarNtoLNPiMinus)
		return true;
	else if (typeCode == WCSimTruthSummary::kCCNuBarPtoLNPiZero)
		return true;
	else if (typeCode == WCSimTruthSummary::kCCNuBarPtoLPPiMinus)
		return true;
	else if (typeCode == WCSimTruthSummary::kCCDIS)
		return true;
	else if (typeCode == WCSimTruthSummary::kCCCoh)
		return true;
	else
		return false;
}

bool WCSimTruthSummary::IsNCEvent() const
{
	return TypeIsNCEvent(fInteractionMode);
}

bool WCSimTruthSummary::TypeIsNCEvent(int typeCode)
{
	if (typeCode == WCSimTruthSummary::kNCQE)
		return true;
	else if (typeCode == WCSimTruthSummary::kNCNuPtoNuPPiZero)
		return true;
	else if (typeCode == WCSimTruthSummary::kNCNuPtoNuNPiPlus)
		return true;
	else if (typeCode == WCSimTruthSummary::kNCNuNtoNuNPiZero)
		return true;
	else if (typeCode == WCSimTruthSummary::kNCNuNtoNuPPiMinus)
		return true;
	else if (typeCode == WCSimTruthSummary::kNCNuBarPtoNuBarPPiZero)
		return true;
	else if (typeCode == WCSimTruthSummary::kNCNuBarPtoNuBarNPiPlus)
		return true;
	else if (typeCode == WCSimTruthSummary::kNCNuBarNtoNuBarNPiZero)
		return true;
	else if (typeCode == WCSimTruthSummary::kNCNuBarNtoNuBarPPiMinus)
		return true;
	else if (typeCode == WCSimTruthSummary::kNCDIS)
		return true;
	else if (typeCode == WCSimTruthSummary::kNCCoh)
		return true;
	else
		return false;
}

bool WCSimTruthSummary::IsQEEvent() const
{
	return TypeIsQEEvent(fInteractionMode);
}

bool WCSimTruthSummary::TypeIsQEEvent(int typeCode)
{
	if (typeCode == WCSimTruthSummary::kCCQE)
		return true;
	else if (typeCode == WCSimTruthSummary::kNCQE)
		return true;
	else
		return false;
}

bool WCSimTruthSummary::IsResEvent() const
{
	return TypeIsResEvent(fInteractionMode);
}

bool WCSimTruthSummary::TypeIsResEvent(int typeCode)
{
	if (typeCode == WCSimTruthSummary::kCCNuPtoLPPiPlus)
		return true;
	else if (typeCode == WCSimTruthSummary::kCCNuNtoLPPiZero)
		return true;
	else if (typeCode == WCSimTruthSummary::kCCNuNtoLNPiPlus)
		return true;
	else if (typeCode == WCSimTruthSummary::kCCNuBarNtoLNPiMinus)
		return true;
	else if (typeCode == WCSimTruthSummary::kCCNuBarPtoLNPiZero)
		return true;
	else if (typeCode == WCSimTruthSummary::kCCNuBarPtoLPPiMinus)
		return true;
	else if (typeCode == WCSimTruthSummary::kNCNuPtoNuPPiZero)
		return true;
	else if (typeCode == WCSimTruthSummary::kNCNuPtoNuNPiPlus)
		return true;
	else if (typeCode == WCSimTruthSummary::kNCNuNtoNuNPiZero)
		return true;
	else if (typeCode == WCSimTruthSummary::kNCNuNtoNuPPiMinus)
		return true;
	else if (typeCode == WCSimTruthSummary::kNCNuBarPtoNuBarPPiZero)
		return true;
	else if (typeCode == WCSimTruthSummary::kNCNuBarPtoNuBarNPiPlus)
		return true;
	else if (typeCode == WCSimTruthSummary::kNCNuBarNtoNuBarNPiZero)
		return true;
	else if (typeCode == WCSimTruthSummary::kNCNuBarNtoNuBarPPiMinus)
		return true;
	else
		return false;
}

bool WCSimTruthSummary::IsDISEvent() const
{
	return TypeIsDISEvent(fInteractionMode);
}

bool WCSimTruthSummary::TypeIsDISEvent(int typeCode)
{
	if (typeCode == WCSimTruthSummary::kCCDIS)
		return true;
	else if (typeCode == WCSimTruthSummary::kNCDIS)
		return true;
	else
		return false;
}

bool WCSimTruthSummary::IsCohEvent() const
{
	return TypeIsCohEvent(fInteractionMode);
}

bool WCSimTruthSummary::TypeIsCohEvent(int typeCode)
{
	if (typeCode == WCSimTruthSummary::kCCCoh)
		return true;
	else if (typeCode == WCSimTruthSummary::kNCCoh)
		return true;
	else
		return false;
}

bool WCSimTruthSummary::IsNueElectronElasticEvent() const
{
	return TypeIsNueElectronElasticEvent(fInteractionMode);
}

bool WCSimTruthSummary::TypeIsNueElectronElasticEvent(int typeCode)
{
	if (typeCode == WCSimTruthSummary::kElastic)
		return true;
	else
		return false;
}

bool WCSimTruthSummary::IsInverseMuonDecayEvent() const
{
	return TypeIsInverseMuonDecayEvent(fInteractionMode);
}

bool WCSimTruthSummary::TypeIsInverseMuonDecayEvent(int typeCode)
{
	if (typeCode == WCSimTruthSummary::kInverseMuDecay)
		return true;
	else
		return false;
}

//  Get and set the beam information
int WCSimTruthSummary::GetBeamPDG() const
{
	return fBeamPDG;
}

void WCSimTruthSummary::SetBeamPDG(int pdg)
{
	fBeamPDG = pdg;
}

bool WCSimTruthSummary::IsNuEEvent() const
{
	if (fBeamPDG == 12)
		return true;
	else
		return false;
}

bool WCSimTruthSummary::IsNuMuEvent() const
{
	if (fBeamPDG == 14)
		return true;
	else
		return false;
}

bool WCSimTruthSummary::IsNuTauEvent() const
{
	if (fBeamPDG == 16)
		return true;
	else
		return false;
}

bool WCSimTruthSummary::IsNuEBarEvent() const
{
	if (fBeamPDG == -12)
		return true;
	else
		return false;
}

bool WCSimTruthSummary::IsNuMuBarEvent() const
{
	if (fBeamPDG == -14)
		return true;
	else
		return false;
}

bool WCSimTruthSummary::IsNuTauBarEvent() const
{
	if (fBeamPDG == -16)
		return true;
	else
		return false;
}

double WCSimTruthSummary::GetBeamEnergy() const
{
	return fBeamEnergy;
}

void WCSimTruthSummary::SetBeamEnergy(double en)
{
	fBeamEnergy = en;
}

TVector3 WCSimTruthSummary::GetBeamDir() const
{
	return fBeamDir;
}

void WCSimTruthSummary::SetBeamDir(TVector3 dir)
{
	fBeamDir = dir;
}

void WCSimTruthSummary::SetBeamDir(double dx, double dy, double dz)
{
	fBeamDir = TVector3(dx, dy, dz);
}

// Get and set the target information
int WCSimTruthSummary::GetTargetPDG() const
{
	return fTargetPDG;
}

void WCSimTruthSummary::SetTargetPDG(int pdg)
{
	fTargetPDG = pdg;
}

double WCSimTruthSummary::GetTargetEnergy() const
{
	return fTargetEnergy;
}

void WCSimTruthSummary::SetTargetEnergy(double en)
{
	fTargetEnergy = en;
}

TVector3 WCSimTruthSummary::GetTargetDir() const
{
	return fTargetDir;
}

void WCSimTruthSummary::SetTargetDir(TVector3 dir)
{
	fTargetDir = dir;
}

void WCSimTruthSummary::SetTargetDir(double dx, double dy, double dz)
{
	fTargetDir = TVector3(dx, dy, dz);
}

// Primary particle functions
void WCSimTruthSummary::AddPrimary(int pdg, double en, TVector3 dir)
{
	fPrimaryPDGs.push_back(pdg);
	fPrimaryEnergies.push_back(en);
	fPrimaryDirs.push_back(dir);
}

void WCSimTruthSummary::AddPrimary(int pdg, double en, double dx, double dy, double dz)
{
	fPrimaryPDGs.push_back(pdg);
	fPrimaryEnergies.push_back(en);
	TVector3 tmpDir(dx, dy, dz);
	fPrimaryDirs.push_back(tmpDir);
}

int WCSimTruthSummary::GetPrimaryPDG(unsigned int p) const
{
	if (p < this->GetNPrimaries())
	{
		return fPrimaryPDGs[p];
	}
	else
	{
		std::cerr << "== Request for primary particle " << p << " of [0..." << this->GetNPrimaries() - 1 << "]"
				  << std::endl;
		return -999;
	}
}

double WCSimTruthSummary::GetPrimaryEnergy(unsigned int p) const
{
	if (p < this->GetNPrimaries())
	{
		return fPrimaryEnergies[p];
	}
	else
	{
		std::cerr << "== Request for primary particle " << p << " of [0..." << this->GetNPrimaries() - 1 << "]"
				  << std::endl;
		return -999.;
	}
}

TVector3 WCSimTruthSummary::GetPrimaryDir(unsigned int p) const
{
	if (p < this->GetNPrimaries())
	{
		return fPrimaryDirs[p];
	}
	else
	{
		std::cerr << "== Request for primary particle " << p << " of [0..." << this->GetNPrimaries() - 1 << "]"
				  << std::endl;
		return TVector3(-999., -999., -999.);
	}
}

std::vector<int> WCSimTruthSummary::GetPrimaryPDGs() const
{
	return fPrimaryPDGs;
}

std::vector<double> WCSimTruthSummary::GetPrimaryEnergies() const
{
	return fPrimaryEnergies;
}

std::vector<TVector3> WCSimTruthSummary::GetPrimaryDirs() const
{
	return fPrimaryDirs;
}

unsigned int WCSimTruthSummary::GetNPrimaries() const
{
	return fPrimaryPDGs.size();
}

// Do we have a primary pi-zero?
bool WCSimTruthSummary::IsPrimaryPiZero() const
{
	for (unsigned int d = 0; d < fPrimaryPDGs.size(); ++d)
	{
		if (fPrimaryPDGs[d] == 111)
			return true;
	}
	return false;
}

// Check all of the primaries and return the energies of all pi0
std::vector<double> WCSimTruthSummary::GetPiZeroEnergies() const
{
	std::vector<double> pi0Ens;
	for (unsigned int d = 0; d < fPrimaryPDGs.size(); ++d)
	{
		if (fPrimaryPDGs[d] == 111)
		{
			pi0Ens.push_back(fPrimaryEnergies[d]);
		}
	}
	return pi0Ens;
}

// What sort of event do we have?
bool WCSimTruthSummary::IsParticleGunEvent() const
{
	if (fTargetPDG == -999 && fTargetEnergy == -999.)
		return true;
	else
		return false;
}

bool WCSimTruthSummary::IsNeutrinoEvent() const
{
	if (fTargetPDG != -999 && fInteractionMode != 999)
	{
		if (abs(fBeamPDG) == 12 || abs(fBeamPDG) == 14 || abs(fBeamPDG) == 16)
			return true;
		else
			return false;
	}
	else
		return false;
}

// ---------------------------------------------
// Overlay information, if it exists.
// ---------------------------------------------

bool WCSimTruthSummary::IsOverlayEvent() const
{
	if (this->GetNOverlays() > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// Get and set the overlay vertex information
TVector3 WCSimTruthSummary::GetOverlayVertex() const
{
	return fOverlayVertex;
}

void WCSimTruthSummary::SetOverlayVertex(TVector3 vtx)
{
	fOverlayVertex = vtx;
}

void WCSimTruthSummary::SetOverlayVertex(double x, double y, double z)
{
	fOverlayVertex = TVector3(x, y, z);
}

void WCSimTruthSummary::SetOverlayVertex(double x, double y, double z, double t)
{
	fOverlayVertex = TVector3(x, y, z);
	fOverlayVertexT = t;
}

void WCSimTruthSummary::SetOverlayVertexT(double t)
{
	fOverlayVertexT = t;
}

// Get the overlay vertex components
double WCSimTruthSummary::GetOverlayVertexX() const
{
	return fOverlayVertex.X();
}

double WCSimTruthSummary::GetOverlayVertexY() const
{
	return fOverlayVertex.Y();
}

double WCSimTruthSummary::GetOverlayVertexZ() const
{
	return fOverlayVertex.Z();
}

double WCSimTruthSummary::GetOverlayVertexT() const
{
	return fOverlayVertexT;
}

void WCSimTruthSummary::AddOverlayTrack(int pdg, double en, TVector3 dir)
{
	fOverlayPDGs.push_back(pdg);
	fOverlayEnergies.push_back(en);
	fOverlayDirs.push_back(dir);
}

void WCSimTruthSummary::AddOverlayTrack(int pdg, double en, double dx, double dy, double dz)
{
	this->AddOverlayTrack(pdg, en, TVector3(dx, dy, dz));
}

int WCSimTruthSummary::GetOverlayPDG(unsigned int p) const
{
	if (p < this->GetNOverlays())
	{
		return fOverlayPDGs[p];
	}
	else
	{
		std::cerr << "== Request for overlay particle " << p << " of [0..." << this->GetNOverlays() - 1 << "]"
				  << std::endl;
		return -999;
	}
}

double WCSimTruthSummary::GetOverlayEnergy(unsigned int p) const
{
	if (p < this->GetNOverlays())
	{
		return fOverlayEnergies[p];
	}
	else
	{
		std::cerr << "== Request for overlay particle " << p << " of [0..." << this->GetNOverlays() - 1 << "]"
				  << std::endl;
		return -999.;
	}
}

TVector3 WCSimTruthSummary::GetOverlayDir(unsigned int p) const
{
	if (p < this->GetNOverlays())
	{
		return fOverlayDirs[p];
	}
	else
	{
		std::cerr << "== Request for overlay particle " << p << " of [0..." << this->GetNOverlays() - 1 << "]"
				  << std::endl;
		return TVector3(-999., -999., -999.);
	}
}

std::vector<int> WCSimTruthSummary::GetOverlayPDGs() const
{
	return fOverlayPDGs;
}

std::vector<double> WCSimTruthSummary::GetOverlayEnergies() const
{
	return fOverlayEnergies;
}

std::vector<TVector3> WCSimTruthSummary::GetOverlayDirs() const
{
	return fOverlayDirs;
}

unsigned int WCSimTruthSummary::GetNOverlays() const
{
	return fOverlayPDGs.size();
}
