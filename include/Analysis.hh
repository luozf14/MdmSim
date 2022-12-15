#ifndef TexPPACSimAnalysis_h
#define TexPPACSimAnalysis_h 1

#include "globals.hh"

class TFile;
class TTree;

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

        void SetSiEHitGlobalPosX(std::vector<G4double> &v) { fSiEHitGlobalPosX = v; };
        void SetSiEHitGlobalPosY(std::vector<G4double> &v) { fSiEHitGlobalPosY = v; };
        void SetSiEHitGlobalPosZ(std::vector<G4double> &v) { fSiEHitGlobalPosZ = v; };
        void SetSiEHitLocalPosX(std::vector<G4double> &v) { fSiEHitLocalPosX = v; };
        void SetSiEHitLocalPosY(std::vector<G4double> &v) { fSiEHitLocalPosY = v; };
        void SetSiEHitLocalPosZ(std::vector<G4double> &v) { fSiEHitLocalPosZ = v; };
        void SetSiEHitGlobalMomentumX(std::vector<G4double> &v) { fSiEHitGlobalMomentumX = v; };
        void SetSiEHitGlobalMomentumY(std::vector<G4double> &v) { fSiEHitGlobalMomentumY = v; };
        void SetSiEHitGlobalMomentumZ(std::vector<G4double> &v) { fSiEHitGlobalMomentumZ = v; };
        void SetSiEHitLocalMomentumX(std::vector<G4double> &v) { fSiEHitLocalMomentumX = v; };
        void SetSiEHitLocalMomentumY(std::vector<G4double> &v) { fSiEHitLocalMomentumY = v; };
        void SetSiEHitLocalMomentumZ(std::vector<G4double> &v) { fSiEHitLocalMomentumZ = v; };
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

        std::vector<G4double> fSiEHitGlobalPosX;
        std::vector<G4double> fSiEHitGlobalPosY;
        std::vector<G4double> fSiEHitGlobalPosZ;
        std::vector<G4double> fSiEHitLocalPosX;
        std::vector<G4double> fSiEHitLocalPosY;
        std::vector<G4double> fSiEHitLocalPosZ;
        std::vector<G4double> fSiEHitGlobalMomentumX;
        std::vector<G4double> fSiEHitGlobalMomentumY;
        std::vector<G4double> fSiEHitGlobalMomentumZ;
        std::vector<G4double> fSiEHitLocalMomentumX;
        std::vector<G4double> fSiEHitLocalMomentumY;
        std::vector<G4double> fSiEHitLocalMomentumZ;
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
