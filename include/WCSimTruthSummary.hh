#ifndef WCSimTruthSummary_h
#define WCSimTruthSummary_h

#include <vector>

// Root
#include <TObject.h>
#include <TVector3.h>

class WCSimTruthSummary : public TObject{

public:

  // Define an enumeration and typedef for the interaction type
  enum intCode
  {
    // Mirrors the values used in GENIE's GHepUtils.cxx
    kOther = 0, // This usually refers to complex resonant events with strange or higher mass quarks
    // First QE
    kCCQE = 1,
    kNCQE = 2,
    // Resonance states
    // Nu = neutrino, P = proton, N = neutron, L = charged lepton (charge implied)
    kCCNuPtoLPPiPlus = 3,
    kCCNuNtoLPPiZero = 4,
    kCCNuNtoLNPiPlus = 5,
    kNCNuPtoNuPPiZero = 6,
    kNCNuPtoNuNPiPlus = 7,
    kNCNuNtoNuNPiZero = 8,
    kNCNuNtoNuPPiMinus = 9,
    // Now for the corresponding anti-neutrino resonant states
    kCCNuBarNtoLNPiMinus = 10,
    kCCNuBarPtoLNPiZero = 11,
    kCCNuBarPtoLPPiMinus = 12,
    kNCNuBarPtoNuBarPPiZero = 13,
    kNCNuBarPtoNuBarNPiPlus = 14,
    kNCNuBarNtoNuBarNPiZero = 15,
    kNCNuBarNtoNuBarPPiMinus = 16,
    // Others
    kCCDIS = 91,
    kNCDIS = 92,
    kNCCoh = 96, // No idea why NC is first here, but that's what it says.
    kCCCoh = 97, 
    kElastic = 98, // Nu_e electron elastic scattering
    kInverseMuDecay = 99,
    // Dummy value for using particle guns
    kNotSet = 999
  };

  // Constructors and Deconstructor
  WCSimTruthSummary();
  WCSimTruthSummary(const WCSimTruthSummary &ts);
  ~WCSimTruthSummary();
  
  // Reset the parameters to their dummy values
  void ResetValues();

  // Get and set the vertex information
  TVector3 GetVertex() const;
  void SetVertex(TVector3 vtx);
  void SetVertex(double x, double y, double z);

  double GetVertexX() const;
  double GetVertexY() const;
  double GetVertexZ() const;

  // Interaction type
  int GetInteractionMode() const;
  void SetInteractionMode(int mode);

  // A few convenience methods for querying the interaction type
  bool IsCCEvent() const;
  bool IsNCEvent() const;
  bool IsQEEvent() const;
  bool IsResEvent() const;
  bool IsDISEvent() const;
  bool IsCohEvent() const;
  bool IsNueElectronElasticEvent() const;
  bool IsInverseMuonDecayEvent() const;

  // Beam PDG code
  int GetBeamPDG() const;
  void SetBeamPDG(int pdg);

  // Beam energy
  double GetBeamEnergy() const;
  void SetBeamEnergy(double en);

  // Beam direction
  TVector3 GetBeamDir() const;
  void SetBeamDir(TVector3 dir);
  void SetBeamDir(double dx, double dy, double dz);

  // Target PDG code
  int GetTargetPDG() const;
  void SetTargetPDG(int pdg);

  // Target energy
  double GetTargetEnergy() const;
  void SetTargetEnergy(double en);

  // Target direction
  TVector3 GetTargetDir() const;
  void SetTargetDir(TVector3 dir);
  void SetTargetDir(double dx, double dy, double dz);

  // Primary particle functions
  void AddPrimary(int pdg, double en, TVector3 dir);
  void AddPrimary(int pdg, double en, double dx, double dy, double dz);
  int GetPrimaryPDG(unsigned int p) const; // Index starts at 0
  double GetPrimaryEnergy(unsigned int p) const; // Index starts at 0
  TVector3 GetPrimaryDir(unsigned int p) const; // Index starts at 0
  std::vector<int> GetPrimaryPDGs() const;
  std::vector<double> GetPrimaryEnergies() const; 
  std::vector<TVector3> GetPrimaryDirs() const;
  unsigned int GetNPrimaries() const;
  

  // Just to see what type of event we had
  bool IsParticleGunEvent() const;
  bool IsNeutrinoEvent() const;

private:

  // Vertex position
  TVector3 fVertex;

  // Neutrino interaction type. In the case of a beam gun, this is will have a dummy value
  int fInteractionMode;

  // Beam information. This is usually the neutrino beam, but could also be a particle gun
  int fBeamPDG;
  double fBeamEnergy;
  TVector3 fBeamDir;

  // Target information. In the case of a particle gun, these will have dummy values
  int fTargetPDG;
  double fTargetEnergy;
  TVector3 fTargetDir;

  // Also store information about the primary particles (only those escaping the nucleus)
  std::vector<int> fPrimaryPDGs;
  std::vector<double> fPrimaryEnergies;
  std::vector<TVector3> fPrimaryDirs;

  ClassDef(WCSimTruthSummary,1);

};

#endif
