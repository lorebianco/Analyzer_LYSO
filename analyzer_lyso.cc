//****************************************************************************//
//                         Lorenzo Bianco 2/08/2024                           //
//                                                                            //
//              My 'Analyzer' for the LYSO calorimeter prototype              //
//                                                                            //
//****************************************************************************//

#include <iostream>
#include <vector>
#include <chrono>
#include <memory>
 
#include "TSystem.h"
#include "TInterpreter.h"
#include <TFile.h>
#include <TTree.h>
#include <TMath.h>
#include <ROOT/RVec.hxx>

#include "globals.hh"
#include "eventlyso.hh"
#include "waveformmppc.hh"


using namespace std;
using namespace ROOT;



int main(int argc, char** argv)
{
    const char *barFilename = argv[1];

    Int_t fEvent;
    RVecD *fTime = 0;
    vector<RVecD> *fFront = 0; 
    vector<RVecD> *fBack = 0; 
    
    unique_ptr<TFile> barFile(TFile::Open(barFilename, "READ"));
    TTree *lyso_wfs = barFile->Get<TTree>("lyso_wfs");

    lyso_wfs->SetBranchAddress("Event", &fEvent);
    lyso_wfs->SetBranchAddress("Time", &fTime);
    lyso_wfs->SetBranchAddress("Front", &fFront);
    lyso_wfs->SetBranchAddress("Back", &fBack);

    Int_t nEntries = lyso_wfs->GetEntries();


    unique_ptr<EventLYSO> eventlyso = nullptr;
    EventLYSO* eventlyso_ptr = nullptr;
    
    unique_ptr<TFile> outFile(TFile::Open("prova.root", "RECREATE") );
    auto lyso_est = make_unique<TTree>("lyso_est", "TTree of lyso estimators");

    lyso_est->Branch("EventEstimators", &eventlyso_ptr);
    

    for(Int_t k = 0; k < nEntries; k++)
    {
        lyso_wfs->GetEntry(k);
        
        RVecD fTime_data(fTime->begin(), fTime->end());
        vector<RVecD> fFront_data(fFront->begin(), fFront->end());
        vector<RVecD> fBack_data(fBack->begin(), fBack->end());

        cout << "Event " << fEvent << endl;

        eventlyso = make_unique<EventLYSO>(fEvent, fTime_data, fFront_data, fBack_data);
        eventlyso->CalculateEstimatorsForEveryMPPC();
        eventlyso->MeasureDetectorCharge();

        //cout << eventlyso->FindFirstNeighbors(0.0, 0.0, 9) << endl;
        //cout << eventlyso->GetBackMeanY() << endl;
        //WaveformMPPC test = eventlyso->SumWaveforms();
        //test.GetWave().DrawWave();


        eventlyso_ptr = eventlyso.get();
        lyso_est->Fill();
    }

    outFile->cd();
    outFile->WriteObject(lyso_est.get(), "lyso_est");

    delete lyso_wfs;
    delete fTime, fFront, fBack;

    // Finally
    return 0;
}



        // for(Int_t j = 0; j < CHANNELS; j++)
        // {
        //     RVecD fFront_data((*fFront)[j].begin(), (*fFront)[j].end());
        //     RVecD fBack_data((*fBack)[j].begin(), (*fBack)[j].end());

        //    auto wMPPC = make_unique<WaveformMPPC>(fTime_data, fFront_data);
        //    wMPPC->MeasureCharge();

            // cout << "Event:" <<fEvent << endl;
            // cout << "Channel FFront-" << j << endl;
            // cout << fTime_data[0] << endl;
            // cout << fFront_data[0] << endl;
        //}