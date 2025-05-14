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
#include <string>
#include <regex>

#include "TSystem.h"
#include "TInterpreter.h"
#include <TFile.h>
#include <TTree.h>
#include <TMath.h>
#include <ROOT/RVec.hxx>

#include "globals.hh"
#include "eventlyso.hh"
#include "waveformmppc.hh"
#include "configure.hh"

using namespace std;
using namespace ROOT;



const char* GenerateOutputFilename(const std::string& barFilename)
{
    // Regular expression per estrarre XXXXX e Y (opzionale)
    std::regex pattern(R"(\/BarID_(\d+)(?:_t(\d+))?\.root$)");
    std::smatch matches;

    // Cerca il pattern
    if (std::regex_search(barFilename, matches, pattern)) {
        std::string id = matches[1];  // XXXXX
        std::string suffix;

        // Controlla se esiste il parametro Y e lo aggiunge al suffisso se presente
        if (matches.size() > 2 && matches[2].matched) {
            suffix = "_t" + matches[2].str();  // "_tY"
        }

        // Costruisce il filename finale
        static std::string outputFilename = "RootFiles/AnalyzerID_" + id + suffix + ".root";
        return outputFilename.c_str();
    } else {
        std::cerr << "Filename format not recognized." << std::endl;
        return "";
    }
}




int main(int argc, char** argv)
{
    if(argc < 3)
    {
        cerr << "Usage: " << argv[0] << " <barFilename> <configFilename>" << endl;
        return 1;
    }

    const char *barFilename = argv[1];
    const char *configFilename = argv[2];
    const char *outputFilename = GenerateOutputFilename(barFilename);

    Int_t fEvent;
    vector<RVecF> *fTime_F = 0;
    vector<RVecF> *fTime_B = 0;
    vector<RVecF> *fFront = 0; 
    vector<RVecF> *fBack = 0; 
    
    unique_ptr<TFile> barFile(TFile::Open(barFilename, "READ"));
    TTree *lyso_wfs_times = barFile->Get<TTree>("lyso_wfs_times");
    TTree *lyso_wfs = barFile->Get<TTree>("lyso_wfs");

    lyso_wfs->SetBranchAddress("Event", &fEvent);
    lyso_wfs_times->SetBranchAddress("Time_F", &fTime_F);
    lyso_wfs_times->SetBranchAddress("Time_B", &fTime_B);
    lyso_wfs->SetBranchAddress("Front", &fFront);
    lyso_wfs->SetBranchAddress("Back", &fBack);

    lyso_wfs_times->GetEntry(0);
    vector<RVecD> fTime_F_data(fTime_F->begin(), fTime_F->end());
    vector<RVecD> fTime_B_data(fTime_B->begin(), fTime_B->end());

    Int_t nEntries = lyso_wfs->GetEntries();
    
    cout << "AnalyzerWT>> Entries = " << nEntries << endl;

    unique_ptr<EventLYSO> eventlyso = nullptr;
    EventLYSO* eventlyso_ptr = nullptr;
    
    unique_ptr<TFile> outFile(TFile::Open(outputFilename, "RECREATE") );
    auto lyso_est = make_unique<TTree>("lyso_est", "TTree of lyso estimators");

    lyso_est->Branch("EventEstimators", &eventlyso_ptr);
    
    ConfigAnalyzer::GetInstance()->LoadConfig(configFilename);
    for(Int_t k = 0; k < nEntries; k++)
    {
        lyso_wfs->GetEntry(k);

        vector<RVecD> fFront_data(fFront->begin(), fFront->end());
        vector<RVecD> fBack_data(fBack->begin(), fBack->end());

        eventlyso = make_unique<EventLYSO>(fEvent, fTime_F_data, fTime_B_data, fFront_data, fBack_data);
        eventlyso->CalculateEstimatorsForEveryMPPC();
        eventlyso->MeasureDetectorCharge();
        eventlyso->MeasureDetectorTime();
        eventlyso->MeasureDetectorPosition();

        eventlyso_ptr = eventlyso.get();
        lyso_est->Fill();

        if(nEntries < 10 || k % (nEntries / 10) == 0)
            cout << "\rAnalyzerWT>> Processed " << k + 1 << " events" << flush;
    }
    cout << endl;

    outFile->cd();
    outFile->WriteObject(lyso_est.get(), "lyso_est");

    delete lyso_wfs_times;
    delete lyso_wfs;
    delete fTime_F;
    delete fTime_B;
    delete fFront;
    delete fBack;

    // Finally
    return 0;
}
