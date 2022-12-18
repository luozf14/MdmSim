#include "EventAction.hh"
#include "RunAction.hh"
#include "Analysis.hh"
#include "SiDetectorHit.hh"

#include "G4SDManager.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "globals.hh"
#include "Randomize.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

namespace TexPPACSim
{

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    EventAction::EventAction(RunAction *runAction)
        : fRunAction(runAction)
    {
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    EventAction::~EventAction()
    {
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    void EventAction::BeginOfEventAction(const G4Event *)
    {
        G4SDManager *sdManager = G4SDManager::GetSDMpointer();
        fHCID_SiDetectorE = sdManager->GetCollectionID("SiDetectorEHitsCollection");
        fHCID_SiDetectorDeltaE = sdManager->GetCollectionID("SiDetectorDeltaEHitsCollection");
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    void EventAction::EndOfEventAction(const G4Event *aEvent)
    {
        Analysis *analysis = Analysis::Instance();

        G4HCofThisEvent *hce = aEvent->GetHCofThisEvent();
        if (!hce)
        {
            G4ExceptionDescription msg;
            msg << "No hits collection of this event found.\n";
            // G4Exception("\n---> EventAction::EndOfEventAction()",
            //             "TexPPACSimCode01", JustWarning, msg);
            return;
        }

        //
        // Delta E
        SiDetectorHitsCollection *hcSiDetectorDeltaE = (SiDetectorHitsCollection *)hce->GetHC(fHCID_SiDetectorDeltaE);
        G4int nofHitsSiDetectorDeltaE = hcSiDetectorDeltaE->GetSize();
        if (nofHitsSiDetectorDeltaE == 0)
        {
            G4ExceptionDescription msg;
            msg << "No hits in Si detector Delta-E of this event found.\n";
            // G4Exception("\n---> EventAction::EndOfEventAction()",
            //             "TexPPACSimCode01", JustWarning, msg);
            return;
        }
        std::vector<G4int> siDeltaEHitTrackId;
        std::vector<G4double> siDeltaEHitEDep;
        std::vector<G4double> siDeltaEHitTime;
        siDeltaEHitTrackId.push_back((*hcSiDetectorDeltaE)[0]->GetTrackID());
        G4double siDeltaEeDep = 0.;
        G4double siDeltaETime = 0.;
        G4int itTimes = 0;
        for (int i = 0; i < nofHitsSiDetectorDeltaE; i++)
        {
            if ((*hcSiDetectorDeltaE)[i]->GetTrackID() == siDeltaEHitTrackId.back())
            {
                siDeltaEeDep += (*hcSiDetectorDeltaE)[i]->GetEnergyDeposit();
                siDeltaETime += (*hcSiDetectorDeltaE)[i]->GetTime();
                itTimes += 1;
            }
            else
            {
                siDeltaEHitTrackId.push_back((*hcSiDetectorDeltaE)[i]->GetTrackID());
                siDeltaEHitEDep.push_back(siDeltaEeDep);
                siDeltaEHitTime.push_back(siDeltaETime / (double)itTimes);
                siDeltaEeDep = (*hcSiDetectorDeltaE)[i]->GetEnergyDeposit();
                siDeltaETime = (*hcSiDetectorDeltaE)[i]->GetTime();
                itTimes = 1;
            }
        }
        siDeltaEHitEDep.push_back(siDeltaEeDep);
        siDeltaEHitTime.push_back(siDeltaETime);

        analysis->SetSiDeltaEHitTrackId(siDeltaEHitTrackId);
        analysis->SetSiDeltaEHitEDep(siDeltaEHitEDep);
        analysis->SetSiDeltaEHitTime(siDeltaEHitTime);
        std::vector<G4double> siDeltaEHitEDepExp;
        std::vector<G4double> siDeltaEHitTimeExp;
        for (int i = 0; i < siDeltaEHitTrackId.size(); i++)
        {
            siDeltaEHitEDepExp.push_back(G4RandGauss::shoot(siDeltaEHitEDep[i], fSiDetectorEnergyResolution * siDeltaEHitEDep[i] / 2.355));
            siDeltaEHitTimeExp.push_back(siDeltaEHitTime[i] + G4RandFlat::shoot(-fTdcResolution, fTdcResolution));
        }
        analysis->SetSiDeltaEHitEDepExp(siDeltaEHitEDepExp);
        analysis->SetSiDeltaEHitTimeExp(siDeltaEHitTimeExp);

        //
        // E
        SiDetectorHitsCollection *hcSiDetectorE = (SiDetectorHitsCollection *)hce->GetHC(fHCID_SiDetectorE);
        G4int nofHitsSiDetectorE = hcSiDetectorE->GetSize();
        if (nofHitsSiDetectorE == 0)
        {
            G4ExceptionDescription msg;
            msg << "No hits in Si detector E of this event found.\n";
            // G4Exception("\n---> EventAction::EndOfEventAction()",
            //             "TexPPACSimCode01", JustWarning, msg);
            return;
        }
        std::vector<G4int> siEHitTrackId;
        std::vector<G4double> siEHitEDep;
        std::vector<G4double> siEHitTime;
        std::vector<G4double> siEHitGlobalPosX;
        std::vector<G4double> siEHitGlobalPosY;
        std::vector<G4double> siEHitGlobalPosZ;
        std::vector<G4double> siEHitLocalPosX;
        std::vector<G4double> siEHitLocalPosY;
        std::vector<G4double> siEHitLocalPosZ;
        std::vector<G4double> siEHitGlobalMomentumX;
        std::vector<G4double> siEHitGlobalMomentumY;
        std::vector<G4double> siEHitGlobalMomentumZ;
        std::vector<G4double> siEHitLocalMomentumX;
        std::vector<G4double> siEHitLocalMomentumY;
        std::vector<G4double> siEHitLocalMomentumZ;
        siEHitTrackId.push_back((*hcSiDetectorE)[0]->GetTrackID());
        G4double siEeDep = 0.;
        G4double siETime = 0.;
        G4ThreeVector siEGlobalPos(0., 0., 0.);
        G4ThreeVector siELocalPos(0., 0., 0.);
        G4ThreeVector siEGlobalMomentum(0., 0., 0.);
        G4ThreeVector siELocalMomentum(0., 0., 0.);
        itTimes = 0;
        G4bool daqTrigger = false;
        for (int i = 0; i < nofHitsSiDetectorE; i++)
        {
            daqTrigger = true;
            if ((*hcSiDetectorE)[i]->GetTrackID() == siEHitTrackId.back())
            {
                siEeDep += (*hcSiDetectorE)[i]->GetEnergyDeposit();
                siETime += (*hcSiDetectorE)[i]->GetTime();
                siEGlobalPos += (*hcSiDetectorE)[i]->GetGlobalPosition();
                siELocalPos += (*hcSiDetectorE)[i]->GetLocalPosition();
                siEGlobalMomentum += (*hcSiDetectorE)[i]->GetGlobalMomentum();
                siELocalMomentum += (*hcSiDetectorE)[i]->GetLocalMomentum();
                itTimes += 1;
            }
            else
            {
                siEHitTrackId.push_back((*hcSiDetectorE)[i]->GetTrackID());
                siEHitEDep.push_back(siEeDep);
                siEHitTime.push_back(siETime / (double)itTimes);
                siEHitGlobalPosX.push_back(siEGlobalPos.x() / (double)itTimes);
                siEHitGlobalPosY.push_back(siEGlobalPos.y() / (double)itTimes);
                siEHitGlobalPosZ.push_back(siEGlobalPos.z() / (double)itTimes);
                siEHitLocalPosX.push_back(siELocalPos.x() / (double)itTimes);
                siEHitLocalPosY.push_back(siELocalPos.y() / (double)itTimes);
                siEHitLocalPosZ.push_back(siELocalPos.z() / (double)itTimes);
                siEHitGlobalMomentumX.push_back(siEGlobalMomentum.x() / (double)itTimes);
                siEHitGlobalMomentumY.push_back(siEGlobalMomentum.y() / (double)itTimes);
                siEHitGlobalMomentumZ.push_back(siEGlobalMomentum.z() / (double)itTimes);
                siEHitLocalMomentumX.push_back(siELocalMomentum.x() / (double)itTimes);
                siEHitLocalMomentumY.push_back(siELocalMomentum.y() / (double)itTimes);
                siEHitLocalMomentumZ.push_back(siELocalMomentum.z() / (double)itTimes);
                siEeDep = (*hcSiDetectorE)[i]->GetEnergyDeposit();
                siETime = (*hcSiDetectorE)[i]->GetTime();
                siEGlobalPos = (*hcSiDetectorE)[i]->GetGlobalPosition();
                siELocalPos = (*hcSiDetectorE)[i]->GetLocalPosition();
                siEGlobalMomentum = (*hcSiDetectorE)[i]->GetGlobalMomentum();
                siELocalMomentum = (*hcSiDetectorE)[i]->GetLocalMomentum();
                itTimes = 1;
            }
        }
        siEHitEDep.push_back(siEeDep);
        siEHitTime.push_back(siETime / (double)itTimes);
        siEHitGlobalPosX.push_back(siEGlobalPos.x() / (double)itTimes);
        siEHitGlobalPosY.push_back(siEGlobalPos.y() / (double)itTimes);
        siEHitGlobalPosZ.push_back(siEGlobalPos.z() / (double)itTimes);
        siEHitLocalPosX.push_back(siELocalPos.x() / (double)itTimes);
        siEHitLocalPosY.push_back(siELocalPos.y() / (double)itTimes);
        siEHitLocalPosZ.push_back(siELocalPos.z() / (double)itTimes);
        siEHitGlobalMomentumX.push_back(siEGlobalMomentum.x() / (double)itTimes);
        siEHitGlobalMomentumY.push_back(siEGlobalMomentum.y() / (double)itTimes);
        siEHitGlobalMomentumZ.push_back(siEGlobalMomentum.z() / (double)itTimes);
        siEHitLocalMomentumX.push_back(siELocalMomentum.x() / (double)itTimes);
        siEHitLocalMomentumY.push_back(siELocalMomentum.y() / (double)itTimes);
        siEHitLocalMomentumZ.push_back(siELocalMomentum.z() / (double)itTimes);

        analysis->SetDaqTrigger(daqTrigger);
        analysis->SetSiEHitTrackId(siEHitTrackId);
        analysis->SetSiEHitEDep(siEHitEDep);
        analysis->SetSiEHitTime(siEHitTime);
        analysis->SetSiEHitGlobalPosX(siEHitGlobalPosX);
        analysis->SetSiEHitGlobalPosY(siEHitGlobalPosY);
        analysis->SetSiEHitGlobalPosZ(siEHitGlobalPosZ);
        analysis->SetSiEHitLocalPosX(siEHitLocalPosX);
        analysis->SetSiEHitLocalPosY(siEHitLocalPosY);
        analysis->SetSiEHitLocalPosZ(siEHitLocalPosZ);
        analysis->SetSiEHitGlobalMomentumX(siEHitGlobalMomentumX);
        analysis->SetSiEHitGlobalMomentumY(siEHitGlobalMomentumY);
        analysis->SetSiEHitGlobalMomentumZ(siEHitGlobalMomentumZ);
        analysis->SetSiEHitLocalMomentumX(siEHitLocalMomentumX);
        analysis->SetSiEHitLocalMomentumY(siEHitLocalMomentumY);
        analysis->SetSiEHitLocalMomentumZ(siEHitLocalMomentumZ);
        std::vector<G4double> siEHitEDepExp;
        std::vector<G4double> siEHitTimeExp;
        std::vector<G4int> siEHitHorizontalNo;
        std::vector<G4int> siEHitVerticalNo;
        for (int i = 0; i < siEHitTrackId.size(); i++)
        {
            siEHitEDepExp.push_back(G4RandGauss::shoot(siEHitEDep[i], fSiDetectorEnergyResolution * siEHitEDep[i] / 2.355));
            siEHitTimeExp.push_back(siEHitTime[i] + G4RandFlat::shoot(-fTdcResolution, fTdcResolution));
            G4int frontStripNo = std::floor((siEHitLocalPosX[i] + 2.5) / (5. / 16.));
            G4int backStripNo = std::floor((siEHitLocalPosY[i] + 2.5) / (5. / 16.));
            siEHitHorizontalNo.push_back(backStripNo);
            siEHitVerticalNo.push_back(frontStripNo);
        }
        analysis->SetSiEHitEDepExp(siEHitEDepExp);
        analysis->SetSiEHitTimeExp(siEHitTimeExp);
        analysis->SetSiEHitHorizontalNo(siEHitHorizontalNo);
        analysis->SetSiEHitVerticalNo(siEHitVerticalNo);

        //
        // Fill the trees
        analysis->FillTreeAccurate();
        analysis->FillTreeExperiment();
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
    void EventAction::ParseParams(std::map<std::string, G4double> params)
    {
        G4cout << "\n---> EventAction::ParseParams():" << G4endl;
        for (auto it : params)
        {
            if (it.first == "SiDetectorEnergyResolution")
            {
                fSiDetectorEnergyResolution = it.second;
                printf("Set: Si detector energy resolution = %.2f%\n",fSiDetectorEnergyResolution*100.);
            }
            else if (it.first == "TdcResolution")
            {
                fTdcResolution = it.second *ns;
                G4cout << "Set: TDC resolution = " << G4BestUnit(fTdcResolution, "Time") << G4endl;
            }
        }
    }
}
