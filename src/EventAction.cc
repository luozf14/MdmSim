#include "EventAction.hh"
#include "RunAction.hh"
#include "Analysis.hh"
#include "SiDetectorHit.hh"
#include "PpacHit.hh"

#include "G4SDManager.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "globals.hh"
#include "Randomize.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

namespace MdmSim
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
        fHCID_Ppac1 = sdManager->GetCollectionID("Ppac1HitsCollection");
        fHCID_Ppac2 = sdManager->GetCollectionID("Ppac2HitsCollection");
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
            G4Exception("\n---> EventAction::EndOfEventAction()",
                        "MdmSimCode01", JustWarning, msg);
            return;
        }

        //
        // Delta E
        SiDetectorHitsCollection *hcSiDetectorDeltaE = (SiDetectorHitsCollection *)hce->GetHC(fHCID_SiDetectorDeltaE);
        G4int nofHitsSiDetectorDeltaE = hcSiDetectorDeltaE->GetSize();
        G4bool siDeltaEHitAccepted;
        std::vector<G4int> siDeltaEHitTrackId;
        std::vector<G4double> siDeltaEHitEDep;
        std::vector<G4double> siDeltaEHitTime;
        std::vector<G4double> siDeltaEHitEDepExp;
        std::vector<G4double> siDeltaEHitTimeExp;
        if (nofHitsSiDetectorDeltaE == 0)
        {
            // G4ExceptionDescription msg;
            // msg << "No hits in Si detector Delta-E of this event found.\n";
            // G4Exception("\n---> EventAction::EndOfEventAction()",
            //             "MdmSimCode01", JustWarning, msg);
            siDeltaEHitAccepted = false;
            siDeltaEHitTrackId.push_back(-99);
            siDeltaEHitEDep.push_back(-99.);
            siDeltaEHitTime.push_back(-99.);
            siDeltaEHitEDepExp.push_back(-99.);
            siDeltaEHitTimeExp.push_back(-99.);
        }
        else
        {
            siDeltaEHitAccepted = true;
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
            for (int i = 0; i < siDeltaEHitTrackId.size(); i++)
            {
                siDeltaEHitEDepExp.push_back(G4RandGauss::shoot(siDeltaEHitEDep[i], fSiDetectorEnergyResolution * siDeltaEHitEDep[i] / 2.355));
                siDeltaEHitTimeExp.push_back(siDeltaEHitTime[i] + G4RandFlat::shoot(-fTdcResolution, fTdcResolution));
            }
        }
        analysis->SetSiDeltaEHitAccepted(siDeltaEHitAccepted);
        analysis->SetSiDeltaEHitTrackId(siDeltaEHitTrackId);
        analysis->SetSiDeltaEHitEDep(siDeltaEHitEDep);
        analysis->SetSiDeltaEHitTime(siDeltaEHitTime);
        analysis->SetSiDeltaEHitEDepExp(siDeltaEHitEDepExp);
        analysis->SetSiDeltaEHitTimeExp(siDeltaEHitTimeExp);

        //
        // E
        SiDetectorHitsCollection *hcSiDetectorE = (SiDetectorHitsCollection *)hce->GetHC(fHCID_SiDetectorE);
        G4int nofHitsSiDetectorE = hcSiDetectorE->GetSize();
        G4bool siEHitAccepted;
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
        std::vector<G4double> siEHitEDepExp;
        std::vector<G4double> siEHitTimeExp;
        std::vector<G4int> siEHitHorizontalNo;
        std::vector<G4int> siEHitVerticalNo;
        if (nofHitsSiDetectorE == 0)
        {
            // G4ExceptionDescription msg;
            // msg << "No hits in Si detector E of this event found.\n";
            // G4Exception("\n---> EventAction::EndOfEventAction()",
            //             "MdmSimCode01", JustWarning, msg);
            siEHitAccepted = false;
            siEHitTrackId.push_back(-99);
            siEHitEDep.push_back(-99.);
            siEHitTime.push_back(-99.);
            siEHitGlobalPosX.push_back(-99.);
            siEHitGlobalPosY.push_back(-99.);
            siEHitGlobalPosZ.push_back(-99.);
            siEHitLocalPosX.push_back(-99.);
            siEHitLocalPosY.push_back(-99.);
            siEHitLocalPosZ.push_back(-99.);
            siEHitGlobalMomentumX.push_back(-99.);
            siEHitGlobalMomentumY.push_back(-99.);
            siEHitGlobalMomentumZ.push_back(-99.);
            siEHitLocalMomentumX.push_back(-99.);
            siEHitLocalMomentumY.push_back(-99.);
            siEHitLocalMomentumZ.push_back(-99.);
            siEHitEDepExp.push_back(-99.);
            siEHitTimeExp.push_back(-99.);
            siEHitHorizontalNo.push_back(-99);
            siEHitVerticalNo.push_back(-99);
        }
        else
        {
            siEHitAccepted = true;
            siEHitTrackId.push_back((*hcSiDetectorE)[0]->GetTrackID());
            G4double siEeDep = 0.;
            G4double siETime = 0.;
            G4ThreeVector siEGlobalPos(0., 0., 0.);
            G4ThreeVector siELocalPos(0., 0., 0.);
            G4ThreeVector siEGlobalMomentum(0., 0., 0.);
            G4ThreeVector siELocalMomentum(0., 0., 0.);
            G4int itTimes = 0;
            for (int i = 0; i < nofHitsSiDetectorE; i++)
            {
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
            for (int i = 0; i < siEHitTrackId.size(); i++)
            {
                siEHitEDepExp.push_back(G4RandGauss::shoot(siEHitEDep[i], fSiDetectorEnergyResolution * siEHitEDep[i] / 2.355));
                siEHitTimeExp.push_back(siEHitTime[i] + G4RandFlat::shoot(-fTdcResolution, fTdcResolution));
                G4int frontStripNo = std::floor((siEHitLocalPosX[i] + 2.5) / (5. / 16.));
                G4int backStripNo = std::floor((siEHitLocalPosY[i] + 2.5) / (5. / 16.));
                siEHitHorizontalNo.push_back(backStripNo);
                siEHitVerticalNo.push_back(frontStripNo);
            }
        }
        analysis->SetSiEHitAccepted(siEHitAccepted);
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
        analysis->SetDaqTrigger(siEHitAccepted);
        analysis->SetSiEHitEDepExp(siEHitEDepExp);
        analysis->SetSiEHitTimeExp(siEHitTimeExp);
        analysis->SetSiEHitHorizontalNo(siEHitHorizontalNo);
        analysis->SetSiEHitVerticalNo(siEHitVerticalNo);

        //
        // PPAC 1
        PpacHitsCollection *hcPpac1 = (PpacHitsCollection *)hce->GetHC(fHCID_Ppac1);
        G4int nofHitsPpac1 = hcPpac1->GetSize();
        G4bool ppac1HitAccepted;
        std::vector<G4int> ppac1HitTrackId;
        std::vector<G4double> ppac1HitEDep;
        std::vector<G4double> ppac1HitTime;
        std::vector<G4double> ppac1HitGlobalPosX;
        std::vector<G4double> ppac1HitGlobalPosY;
        std::vector<G4double> ppac1HitGlobalPosZ;
        std::vector<G4double> ppac1HitLocalPosX;
        std::vector<G4double> ppac1HitLocalPosY;
        std::vector<G4double> ppac1HitLocalPosZ;
        std::vector<G4double> ppac1HitGlobalMomentumX;
        std::vector<G4double> ppac1HitGlobalMomentumY;
        std::vector<G4double> ppac1HitGlobalMomentumZ;
        std::vector<G4double> ppac1HitLocalMomentumX;
        std::vector<G4double> ppac1HitLocalMomentumY;
        std::vector<G4double> ppac1HitLocalMomentumZ;
        if (nofHitsPpac1 == 0)
        {
            // G4ExceptionDescription msg;
            // msg << "No hits in PPAC1 of this event found.\n";
            // G4Exception("\n---> EventAction::EndOfEventAction()",
            //             "MdmSimCode01", JustWarning, msg);
            ppac1HitAccepted = false;
            ppac1HitTrackId.push_back(-99);
            ppac1HitEDep.push_back(-99.);
            ppac1HitTime.push_back(-99.);
            ppac1HitGlobalPosX.push_back(-99.);
            ppac1HitGlobalPosY.push_back(-99.);
            ppac1HitGlobalPosZ.push_back(-99.);
            ppac1HitLocalPosX.push_back(-99.);
            ppac1HitLocalPosY.push_back(-99.);
            ppac1HitLocalPosZ.push_back(-99.);
            ppac1HitGlobalMomentumX.push_back(-99.);
            ppac1HitGlobalMomentumY.push_back(-99.);
            ppac1HitGlobalMomentumZ.push_back(-99.);
            ppac1HitLocalMomentumX.push_back(-99.);
            ppac1HitLocalMomentumY.push_back(-99.);
            ppac1HitLocalMomentumZ.push_back(-99.);
        }
        else
        {
            ppac1HitAccepted = true;
            ppac1HitTrackId.push_back((*hcPpac1)[0]->GetTrackID());
            G4double ppac1eDep = 0.;
            G4double ppac1Time = 0.;
            G4ThreeVector ppac1GlobalPos(0., 0., 0.);
            G4ThreeVector ppac1LocalPos(0., 0., 0.);
            G4ThreeVector ppac1GlobalMomentum(0., 0., 0.);
            G4ThreeVector ppac1LocalMomentum(0., 0., 0.);
            G4int itTimes = 0;
            for (int i = 0; i < nofHitsPpac1; i++)
            {
                if ((*hcPpac1)[i]->GetTrackID() == ppac1HitTrackId.back())
                {
                    ppac1eDep += (*hcPpac1)[i]->GetEnergyDeposit();
                    ppac1Time += (*hcPpac1)[i]->GetTime();
                    ppac1GlobalPos += (*hcPpac1)[i]->GetGlobalPosition();
                    ppac1LocalPos += (*hcPpac1)[i]->GetLocalPosition();
                    ppac1GlobalMomentum += (*hcPpac1)[i]->GetGlobalMomentum();
                    ppac1LocalMomentum += (*hcPpac1)[i]->GetLocalMomentum();
                    itTimes += 1;
                }
                else
                {
                    ppac1HitTrackId.push_back((*hcPpac1)[i]->GetTrackID());
                    ppac1HitEDep.push_back(ppac1eDep);
                    ppac1HitTime.push_back(ppac1Time / (double)itTimes);
                    ppac1HitGlobalPosX.push_back(ppac1GlobalPos.x() / (double)itTimes);
                    ppac1HitGlobalPosY.push_back(ppac1GlobalPos.y() / (double)itTimes);
                    ppac1HitGlobalPosZ.push_back(ppac1GlobalPos.z() / (double)itTimes);
                    ppac1HitLocalPosX.push_back(ppac1LocalPos.x() / (double)itTimes);
                    ppac1HitLocalPosY.push_back(ppac1LocalPos.y() / (double)itTimes);
                    ppac1HitLocalPosZ.push_back(ppac1LocalPos.z() / (double)itTimes);
                    ppac1HitGlobalMomentumX.push_back(ppac1GlobalMomentum.x() / (double)itTimes);
                    ppac1HitGlobalMomentumY.push_back(ppac1GlobalMomentum.y() / (double)itTimes);
                    ppac1HitGlobalMomentumZ.push_back(ppac1GlobalMomentum.z() / (double)itTimes);
                    ppac1HitLocalMomentumX.push_back(ppac1LocalMomentum.x() / (double)itTimes);
                    ppac1HitLocalMomentumY.push_back(ppac1LocalMomentum.y() / (double)itTimes);
                    ppac1HitLocalMomentumZ.push_back(ppac1LocalMomentum.z() / (double)itTimes);
                    ppac1eDep = (*hcPpac1)[i]->GetEnergyDeposit();
                    ppac1Time = (*hcPpac1)[i]->GetTime();
                    ppac1GlobalPos = (*hcPpac1)[i]->GetGlobalPosition();
                    ppac1LocalPos = (*hcPpac1)[i]->GetLocalPosition();
                    ppac1GlobalMomentum = (*hcPpac1)[i]->GetGlobalMomentum();
                    ppac1LocalMomentum = (*hcPpac1)[i]->GetLocalMomentum();
                    itTimes = 1;
                }
            }
            ppac1HitEDep.push_back(ppac1eDep);
            ppac1HitTime.push_back(ppac1Time / (double)itTimes);
            ppac1HitGlobalPosX.push_back(ppac1GlobalPos.x() / (double)itTimes);
            ppac1HitGlobalPosY.push_back(ppac1GlobalPos.y() / (double)itTimes);
            ppac1HitGlobalPosZ.push_back(ppac1GlobalPos.z() / (double)itTimes);
            ppac1HitLocalPosX.push_back(ppac1LocalPos.x() / (double)itTimes);
            ppac1HitLocalPosY.push_back(ppac1LocalPos.y() / (double)itTimes);
            ppac1HitLocalPosZ.push_back(ppac1LocalPos.z() / (double)itTimes);
            ppac1HitGlobalMomentumX.push_back(ppac1GlobalMomentum.x() / (double)itTimes);
            ppac1HitGlobalMomentumY.push_back(ppac1GlobalMomentum.y() / (double)itTimes);
            ppac1HitGlobalMomentumZ.push_back(ppac1GlobalMomentum.z() / (double)itTimes);
            ppac1HitLocalMomentumX.push_back(ppac1LocalMomentum.x() / (double)itTimes);
            ppac1HitLocalMomentumY.push_back(ppac1LocalMomentum.y() / (double)itTimes);
            ppac1HitLocalMomentumZ.push_back(ppac1LocalMomentum.z() / (double)itTimes);
            G4cout << "ppac1HitGlobalPosX=" << G4BestUnit(ppac1HitGlobalPosX[0], "Length") << G4endl;
            G4cout << "ppac1HitGlobalPosY=" << G4BestUnit(ppac1HitGlobalPosY[0], "Length") << G4endl;
            G4cout << "ppac1HitGlobalPosZ=" << G4BestUnit(ppac1HitGlobalPosZ[0], "Length") << G4endl;
            G4cout << "ppac1HitLocalPosX=" << G4BestUnit(ppac1HitLocalPosX[0], "Length") << G4endl;
            G4cout << "ppac1HitLocalPosY=" << G4BestUnit(ppac1HitLocalPosY[0], "Length") << G4endl;
            G4cout << "ppac1HitLocalPosZ=" << G4BestUnit(ppac1HitLocalPosZ[0], "Length") << G4endl;
        }
        analysis->SetPpac1Accepted(ppac1HitAccepted);
        analysis->SetPpac1HitTrackId(ppac1HitTrackId);
        analysis->SetPpac1HitEDep(ppac1HitEDep);
        analysis->SetPpac1HitTime(ppac1HitTime);
        analysis->SetPpac1HitGlobalPosX(ppac1HitGlobalPosX);
        analysis->SetPpac1HitGlobalPosY(ppac1HitGlobalPosY);
        analysis->SetPpac1HitGlobalPosZ(ppac1HitGlobalPosZ);
        analysis->SetPpac1HitLocalPosX(ppac1HitLocalPosX);
        analysis->SetPpac1HitLocalPosY(ppac1HitLocalPosY);
        analysis->SetPpac1HitLocalPosZ(ppac1HitLocalPosZ);
        analysis->SetPpac1HitGlobalMomentumX(ppac1HitGlobalMomentumX);
        analysis->SetPpac1HitGlobalMomentumY(ppac1HitGlobalMomentumY);
        analysis->SetPpac1HitGlobalMomentumZ(ppac1HitGlobalMomentumZ);
        analysis->SetPpac1HitLocalMomentumX(ppac1HitLocalMomentumX);
        analysis->SetPpac1HitLocalMomentumY(ppac1HitLocalMomentumY);
        analysis->SetPpac1HitLocalMomentumZ(ppac1HitLocalMomentumZ);

        //
        // PPAC 2
        PpacHitsCollection *hcPpac2 = (PpacHitsCollection *)hce->GetHC(fHCID_Ppac2);
        G4int nofHitsPpac2 = hcPpac2->GetSize();
        G4bool ppac2HitAccepted;
        std::vector<G4int> ppac2HitTrackId;
        std::vector<G4double> ppac2HitEDep;
        std::vector<G4double> ppac2HitTime;
        std::vector<G4double> ppac2HitGlobalPosX;
        std::vector<G4double> ppac2HitGlobalPosY;
        std::vector<G4double> ppac2HitGlobalPosZ;
        std::vector<G4double> ppac2HitLocalPosX;
        std::vector<G4double> ppac2HitLocalPosY;
        std::vector<G4double> ppac2HitLocalPosZ;
        std::vector<G4double> ppac2HitGlobalMomentumX;
        std::vector<G4double> ppac2HitGlobalMomentumY;
        std::vector<G4double> ppac2HitGlobalMomentumZ;
        std::vector<G4double> ppac2HitLocalMomentumX;
        std::vector<G4double> ppac2HitLocalMomentumY;
        std::vector<G4double> ppac2HitLocalMomentumZ;
        if (nofHitsPpac2 == 0)
        {
            // G4ExceptionDescription msg;
            // msg << "No hits in PPAC2 of this event found.\n";
            // G4Exception("\n---> EventAction::EndOfEventAction()",
            //             "MdmSimCode01", JustWarning, msg);
            ppac2HitAccepted = false;
            ppac2HitTrackId.push_back(-99);
            ppac2HitEDep.push_back(-99.);
            ppac2HitTime.push_back(-99.);
            ppac2HitGlobalPosX.push_back(-99.);
            ppac2HitGlobalPosY.push_back(-99.);
            ppac2HitGlobalPosZ.push_back(-99.);
            ppac2HitLocalPosX.push_back(-99.);
            ppac2HitLocalPosY.push_back(-99.);
            ppac2HitLocalPosZ.push_back(-99.);
            ppac2HitGlobalMomentumX.push_back(-99.);
            ppac2HitGlobalMomentumY.push_back(-99.);
            ppac2HitGlobalMomentumZ.push_back(-99.);
            ppac2HitLocalMomentumX.push_back(-99.);
            ppac2HitLocalMomentumY.push_back(-99.);
            ppac2HitLocalMomentumZ.push_back(-99.);
        }
        else
        {
            ppac2HitAccepted = true;
            ppac2HitTrackId.push_back((*hcPpac2)[0]->GetTrackID());
            G4double ppac2eDep = 0.;
            G4double ppac2Time = 0.;
            G4ThreeVector ppac2GlobalPos(0., 0., 0.);
            G4ThreeVector ppac2LocalPos(0., 0., 0.);
            G4ThreeVector ppac2GlobalMomentum(0., 0., 0.);
            G4ThreeVector ppac2LocalMomentum(0., 0., 0.);
            G4int itTimes = 0;
            for (int i = 0; i < nofHitsPpac2; i++)
            {
                if ((*hcPpac2)[i]->GetTrackID() == ppac2HitTrackId.back())
                {
                    ppac2eDep += (*hcPpac2)[i]->GetEnergyDeposit();
                    ppac2Time += (*hcPpac2)[i]->GetTime();
                    ppac2GlobalPos += (*hcPpac2)[i]->GetGlobalPosition();
                    ppac2LocalPos += (*hcPpac2)[i]->GetLocalPosition();
                    ppac2GlobalMomentum += (*hcPpac2)[i]->GetGlobalMomentum();
                    ppac2LocalMomentum += (*hcPpac2)[i]->GetLocalMomentum();
                    itTimes += 1;
                }
                else
                {
                    ppac2HitTrackId.push_back((*hcPpac2)[i]->GetTrackID());
                    ppac2HitEDep.push_back(ppac2eDep);
                    ppac2HitTime.push_back(ppac2Time / (double)itTimes);
                    ppac2HitGlobalPosX.push_back(ppac2GlobalPos.x() / (double)itTimes);
                    ppac2HitGlobalPosY.push_back(ppac2GlobalPos.y() / (double)itTimes);
                    ppac2HitGlobalPosZ.push_back(ppac2GlobalPos.z() / (double)itTimes);
                    ppac2HitLocalPosX.push_back(ppac2LocalPos.x() / (double)itTimes);
                    ppac2HitLocalPosY.push_back(ppac2LocalPos.y() / (double)itTimes);
                    ppac2HitLocalPosZ.push_back(ppac2LocalPos.z() / (double)itTimes);
                    ppac2HitGlobalMomentumX.push_back(ppac2GlobalMomentum.x() / (double)itTimes);
                    ppac2HitGlobalMomentumY.push_back(ppac2GlobalMomentum.y() / (double)itTimes);
                    ppac2HitGlobalMomentumZ.push_back(ppac2GlobalMomentum.z() / (double)itTimes);
                    ppac2HitLocalMomentumX.push_back(ppac2LocalMomentum.x() / (double)itTimes);
                    ppac2HitLocalMomentumY.push_back(ppac2LocalMomentum.y() / (double)itTimes);
                    ppac2HitLocalMomentumZ.push_back(ppac2LocalMomentum.z() / (double)itTimes);
                    ppac2eDep = (*hcPpac2)[i]->GetEnergyDeposit();
                    ppac2Time = (*hcPpac2)[i]->GetTime();
                    ppac2GlobalPos = (*hcPpac2)[i]->GetGlobalPosition();
                    ppac2LocalPos = (*hcPpac2)[i]->GetLocalPosition();
                    ppac2GlobalMomentum = (*hcPpac2)[i]->GetGlobalMomentum();
                    ppac2LocalMomentum = (*hcPpac2)[i]->GetLocalMomentum();
                    itTimes = 1;
                }
            }
            ppac2HitEDep.push_back(ppac2eDep);
            ppac2HitTime.push_back(ppac2Time / (double)itTimes);
            ppac2HitGlobalPosX.push_back(ppac2GlobalPos.x() / (double)itTimes);
            ppac2HitGlobalPosY.push_back(ppac2GlobalPos.y() / (double)itTimes);
            ppac2HitGlobalPosZ.push_back(ppac2GlobalPos.z() / (double)itTimes);
            ppac2HitLocalPosX.push_back(ppac2LocalPos.x() / (double)itTimes);
            ppac2HitLocalPosY.push_back(ppac2LocalPos.y() / (double)itTimes);
            ppac2HitLocalPosZ.push_back(ppac2LocalPos.z() / (double)itTimes);
            ppac2HitGlobalMomentumX.push_back(ppac2GlobalMomentum.x() / (double)itTimes);
            ppac2HitGlobalMomentumY.push_back(ppac2GlobalMomentum.y() / (double)itTimes);
            ppac2HitGlobalMomentumZ.push_back(ppac2GlobalMomentum.z() / (double)itTimes);
            ppac2HitLocalMomentumX.push_back(ppac2LocalMomentum.x() / (double)itTimes);
            ppac2HitLocalMomentumY.push_back(ppac2LocalMomentum.y() / (double)itTimes);
            ppac2HitLocalMomentumZ.push_back(ppac2LocalMomentum.z() / (double)itTimes);
            // G4cout << "ppac2HitGlobalPosX=" << G4BestUnit(ppac2HitGlobalPosX[0], "Length") << G4endl;
            // G4cout << "ppac2HitGlobalPosY=" << G4BestUnit(ppac2HitGlobalPosY[0], "Length") << G4endl;
            // G4cout << "ppac2HitGlobalPosZ=" << G4BestUnit(ppac2HitGlobalPosZ[0], "Length") << G4endl;
            // G4cout << "ppac2HitLocalPosX=" << G4BestUnit(ppac2HitLocalPosX[0], "Length") << G4endl;
            // G4cout << "ppac2HitLocalPosY=" << G4BestUnit(ppac2HitLocalPosY[0], "Length") << G4endl;
            // G4cout << "ppac2HitLocalPosZ=" << G4BestUnit(ppac2HitLocalPosZ[0], "Length") << G4endl;
        }
        analysis->SetPpac2Accepted(ppac2HitAccepted);
        analysis->SetPpac2HitTrackId(ppac2HitTrackId);
        analysis->SetPpac2HitEDep(ppac2HitEDep);
        analysis->SetPpac2HitTime(ppac2HitTime);
        analysis->SetPpac2HitGlobalPosX(ppac2HitGlobalPosX);
        analysis->SetPpac2HitGlobalPosY(ppac2HitGlobalPosY);
        analysis->SetPpac2HitGlobalPosZ(ppac2HitGlobalPosZ);
        analysis->SetPpac2HitLocalPosX(ppac2HitLocalPosX);
        analysis->SetPpac2HitLocalPosY(ppac2HitLocalPosY);
        analysis->SetPpac2HitLocalPosZ(ppac2HitLocalPosZ);
        analysis->SetPpac2HitGlobalMomentumX(ppac2HitGlobalMomentumX);
        analysis->SetPpac2HitGlobalMomentumY(ppac2HitGlobalMomentumY);
        analysis->SetPpac2HitGlobalMomentumZ(ppac2HitGlobalMomentumZ);
        analysis->SetPpac2HitLocalMomentumX(ppac2HitLocalMomentumX);
        analysis->SetPpac2HitLocalMomentumY(ppac2HitLocalMomentumY);
        analysis->SetPpac2HitLocalMomentumZ(ppac2HitLocalMomentumZ);

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
                printf("Set: Si detector energy resolution = %.2f%\n", fSiDetectorEnergyResolution * 100.);
            }
            else if (it.first == "TdcResolutionInNs")
            {
                fTdcResolution = it.second * ns;
                G4cout << "Set: TDC resolution = " << G4BestUnit(fTdcResolution, "Time") << G4endl;
            }
        }
    }
}
