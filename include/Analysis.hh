#ifndef TexPPACSimAnalysis_h
#define TexPPACSimAnalysis_h 1

#include "globals.hh"

class TFile;
class TTree;
class TVector3;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
namespace TexPPACSim
{

    class Analysis
    {
    public:
        Analysis();
        ~Analysis();
        static Analysis *Instance();

        void Book(std::string processNumber);
        void Save();

        void FillTreeAccurate();
        void FillTreeExperiment();

        void SetSiEHitGlobalPos(std::vector<TVector3 *> &v) { fSiEHitGlobalPos = v; };
        void SetSiEHitLocalPos(std::vector<TVector3 *> &v) { fSiEHitLocalPos = v; };
        void SetSiEHitGlobalMomentum(std::vector<TVector3 *> &v) { fSiEHitGlobalMomentum = v; };
        void SetSiEHitLocalMomentum(std::vector<TVector3 *> &v) { fSiEHitLocalMomentum = v; };
        void SetSiEHitTrackId(std::vector<G4int> &v) { fSiEHitTrackId = v; };
        void SetSiEHitEDep(std::vector<G4double> &v) { fSiEHitEDep = v; };
        void SetSiEHitTime(std::vector<G4double> &v) { fSiEHitTime = v; };

        void SetDaqTrigger(G4bool tri) { fDaqTrigger = tri; };
        void SetSiEHitHorizontalNo(std::vector<G4int> &v) { fSiEHitHorizontalNo = v; };
        void SetSiEHitVerticalNo(std::vector<G4int> &v) { fSiEHitVerticalNo = v; };
        void SetSiEHitEDepExp(std::vector<G4double> &v) { fSiEHitEDepExp = v; };
        void SetSiEHitTimeExp(std::vector<G4double> &v) { fSiEHitTimeExp = v; };

    private:
        static Analysis *fInstance;
        G4bool fFactoryOn;
        TFile *fFile = nullptr;
        TTree *fTreeAccurate = nullptr;
        TTree *fTreeExperiment = nullptr;

        std::vector<TVector3 *> fSiEHitGlobalPos;
        std::vector<TVector3 *> fSiEHitLocalPos;
        std::vector<TVector3 *> fSiEHitGlobalMomentum;
        std::vector<TVector3 *> fSiEHitLocalMomentum;
        std::vector<G4int> fSiEHitTrackId;
        std::vector<G4double> fSiEHitEDep;
        std::vector<G4double> fSiEHitTime;

        G4bool fDaqTrigger;
        std::vector<G4int> fSiEHitHorizontalNo;
        std::vector<G4int> fSiEHitVerticalNo;
        std::vector<G4double> fSiEHitEDepExp;
        std::vector<G4double> fSiEHitTimeExp;
    };

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
}
#endif
