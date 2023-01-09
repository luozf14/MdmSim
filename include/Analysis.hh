#ifndef MdmSimAnalysis_h
#define MdmSimAnalysis_h 1

#include "globals.hh"

class TFile;
class TTree;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
namespace MdmSim
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

        void SetSlitHitAccepted(G4bool acc) { fSlitHitAccepted = acc; };
        void SetSlitHitTransmitted(G4bool tra) { fSlitHitTransmitted = tra; };
        void SetSlitHitTrackId(std::vector<G4int> &v) { fSlitHitTrackId = v; };
        void SetSlitHitLocalPosX(std::vector<G4double> &v) { fSlitHitLocalPosX = v; };
        void SetSlitHitLocalPosY(std::vector<G4double> &v) { fSlitHitLocalPosY = v; };
        void SetSlitHitLocalPosZ(std::vector<G4double> &v) { fSlitHitLocalPosZ = v; };
        void SetMdmTracePositionX(std::vector<G4double> &v) { fMdmTracePositionX = v; };
        void SetMdmTracePositionY(std::vector<G4double> &v) { fMdmTracePositionY = v; };
        void SetMdmTraceAngleX(std::vector<G4double> &v) { fMdmTraceAngleX = v; };
        void SetMdmTraceAngleY(std::vector<G4double> &v) { fMdmTraceAngleY = v; };

        void SetSiDeltaEHitAccepted(G4bool acc) { fSiDeltaEHitAccepted = acc; };
        void SetSiDeltaEHitTrackId(std::vector<G4int> &v) { fSiDeltaEHitTrackId = v; };
        void SetSiDeltaEHitEDep(std::vector<G4double> &v) { fSiDeltaEHitEDep = v; };
        void SetSiDeltaEHitTime(std::vector<G4double> &v) { fSiDeltaEHitTime = v; };

        void SetSiEHitAccepted(G4bool acc) { fSiEHitAccepted = acc; };
        void SetSiEHitTrackId(std::vector<G4int> &v) { fSiEHitTrackId = v; };
        void SetSiEHitEDep(std::vector<G4double> &v) { fSiEHitEDep = v; };
        void SetSiEHitTime(std::vector<G4double> &v) { fSiEHitTime = v; };
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

        void SetPpac1Accepted(G4bool acc) { fPpac1Accepted = acc; };
        void SetPpac1HitGlobalPosX(std::vector<G4double> &v) { fPpac1HitGlobalPosX = v; };
        void SetPpac1HitGlobalPosY(std::vector<G4double> &v) { fPpac1HitGlobalPosY = v; };
        void SetPpac1HitGlobalPosZ(std::vector<G4double> &v) { fPpac1HitGlobalPosZ = v; };
        void SetPpac1HitLocalPosX(std::vector<G4double> &v) { fPpac1HitLocalPosX = v; };
        void SetPpac1HitLocalPosY(std::vector<G4double> &v) { fPpac1HitLocalPosY = v; };
        void SetPpac1HitLocalPosZ(std::vector<G4double> &v) { fPpac1HitLocalPosZ = v; };
        void SetPpac1HitGlobalMomentumX(std::vector<G4double> &v) { fPpac1HitGlobalMomentumX = v; };
        void SetPpac1HitGlobalMomentumY(std::vector<G4double> &v) { fPpac1HitGlobalMomentumY = v; };
        void SetPpac1HitGlobalMomentumZ(std::vector<G4double> &v) { fPpac1HitGlobalMomentumZ = v; };
        void SetPpac1HitLocalMomentumX(std::vector<G4double> &v) { fPpac1HitLocalMomentumX = v; };
        void SetPpac1HitLocalMomentumY(std::vector<G4double> &v) { fPpac1HitLocalMomentumY = v; };
        void SetPpac1HitLocalMomentumZ(std::vector<G4double> &v) { fPpac1HitLocalMomentumZ = v; };
        void SetPpac1HitTrackId(std::vector<G4int> &v) { fPpac1HitTrackId = v; };
        void SetPpac1HitEDep(std::vector<G4double> &v) { fPpac1HitEDep = v; };
        void SetPpac1HitTime(std::vector<G4double> &v) { fPpac1HitTime = v; };

        void SetPpac2Accepted(G4bool acc) { fPpac2Accepted = acc; };
        void SetPpac2HitGlobalPosX(std::vector<G4double> &v) { fPpac2HitGlobalPosX = v; };
        void SetPpac2HitGlobalPosY(std::vector<G4double> &v) { fPpac2HitGlobalPosY = v; };
        void SetPpac2HitGlobalPosZ(std::vector<G4double> &v) { fPpac2HitGlobalPosZ = v; };
        void SetPpac2HitLocalPosX(std::vector<G4double> &v) { fPpac2HitLocalPosX = v; };
        void SetPpac2HitLocalPosY(std::vector<G4double> &v) { fPpac2HitLocalPosY = v; };
        void SetPpac2HitLocalPosZ(std::vector<G4double> &v) { fPpac2HitLocalPosZ = v; };
        void SetPpac2HitGlobalMomentumX(std::vector<G4double> &v) { fPpac2HitGlobalMomentumX = v; };
        void SetPpac2HitGlobalMomentumY(std::vector<G4double> &v) { fPpac2HitGlobalMomentumY = v; };
        void SetPpac2HitGlobalMomentumZ(std::vector<G4double> &v) { fPpac2HitGlobalMomentumZ = v; };
        void SetPpac2HitLocalMomentumX(std::vector<G4double> &v) { fPpac2HitLocalMomentumX = v; };
        void SetPpac2HitLocalMomentumY(std::vector<G4double> &v) { fPpac2HitLocalMomentumY = v; };
        void SetPpac2HitLocalMomentumZ(std::vector<G4double> &v) { fPpac2HitLocalMomentumZ = v; };
        void SetPpac2HitTrackId(std::vector<G4int> &v) { fPpac2HitTrackId = v; };
        void SetPpac2HitEDep(std::vector<G4double> &v) { fPpac2HitEDep = v; };
        void SetPpac2HitTime(std::vector<G4double> &v) { fPpac2HitTime = v; };

        void SetDaqTrigger(G4bool tri) { fDaqTrigger = tri; };
        void SetSiDeltaEHitEDepExp(std::vector<G4double> &v) { fSiDeltaEHitEDepExp = v; };
        void SetSiDeltaEHitTimeExp(std::vector<G4double> &v) { fSiDeltaEHitTimeExp = v; };
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
        
        G4bool fSlitHitAccepted;
        G4bool fSlitHitTransmitted;
        std::vector<G4int> fSlitHitTrackId;
        std::vector<G4double> fSlitHitLocalPosX;
        std::vector<G4double> fSlitHitLocalPosY;
        std::vector<G4double> fSlitHitLocalPosZ;
        std::vector<G4double> fMdmTracePositionX;
        std::vector<G4double> fMdmTracePositionY;
        std::vector<G4double> fMdmTraceAngleX;
        std::vector<G4double> fMdmTraceAngleY;

        G4bool fSiDeltaEHitAccepted;
        std::vector<G4int> fSiDeltaEHitTrackId;
        std::vector<G4double> fSiDeltaEHitEDep;
        std::vector<G4double> fSiDeltaEHitTime;

        G4bool fSiEHitAccepted;
        std::vector<G4int> fSiEHitTrackId;
        std::vector<G4double> fSiEHitEDep;
        std::vector<G4double> fSiEHitTime;
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

        // G4bool fSlitAccepted;
        // std::vector<G4double> fSlitHitKineticEnergy;
        // std::vector<G4double> fSlitHitLocalPosX;
        // std::vector<G4double> fSlitHitLocalPosY;
        // std::vector<G4double> fSlitHitLocalPosZ;
        // std::vector<G4double> fSlitHitLocalMomentumX;
        // std::vector<G4double> fSlitHitLocalMomentumY;
        // std::vector<G4double> fSlitHitLocalMomentumZ;

        G4bool fPpac1Accepted;
        std::vector<G4double> fPpac1HitGlobalPosX;
        std::vector<G4double> fPpac1HitGlobalPosY;
        std::vector<G4double> fPpac1HitGlobalPosZ;
        std::vector<G4double> fPpac1HitLocalPosX;
        std::vector<G4double> fPpac1HitLocalPosY;
        std::vector<G4double> fPpac1HitLocalPosZ;
        std::vector<G4double> fPpac1HitGlobalMomentumX;
        std::vector<G4double> fPpac1HitGlobalMomentumY;
        std::vector<G4double> fPpac1HitGlobalMomentumZ;
        std::vector<G4double> fPpac1HitLocalMomentumX;
        std::vector<G4double> fPpac1HitLocalMomentumY;
        std::vector<G4double> fPpac1HitLocalMomentumZ;
        std::vector<G4int> fPpac1HitTrackId;
        std::vector<G4double> fPpac1HitEDep;
        std::vector<G4double> fPpac1HitTime;

        G4bool fPpac2Accepted;
        std::vector<G4double> fPpac2HitGlobalPosX;
        std::vector<G4double> fPpac2HitGlobalPosY;
        std::vector<G4double> fPpac2HitGlobalPosZ;
        std::vector<G4double> fPpac2HitLocalPosX;
        std::vector<G4double> fPpac2HitLocalPosY;
        std::vector<G4double> fPpac2HitLocalPosZ;
        std::vector<G4double> fPpac2HitGlobalMomentumX;
        std::vector<G4double> fPpac2HitGlobalMomentumY;
        std::vector<G4double> fPpac2HitGlobalMomentumZ;
        std::vector<G4double> fPpac2HitLocalMomentumX;
        std::vector<G4double> fPpac2HitLocalMomentumY;
        std::vector<G4double> fPpac2HitLocalMomentumZ;
        std::vector<G4int> fPpac2HitTrackId;
        std::vector<G4double> fPpac2HitEDep;
        std::vector<G4double> fPpac2HitTime;

        G4bool fDaqTrigger;
        std::vector<G4double> fSiDeltaEHitEDepExp;
        std::vector<G4double> fSiDeltaEHitTimeExp;
        std::vector<G4int> fSiEHitHorizontalNo;
        std::vector<G4int> fSiEHitVerticalNo;
        std::vector<G4double> fSiEHitEDepExp;
        std::vector<G4double> fSiEHitTimeExp;
    };

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
}
#endif
