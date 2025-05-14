#ifndef EVENTLYSO_HH
#define EVENTLYSO_HH

#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cstring>
#include <utility>

#include <TMath.h>
#include <ROOT/RVec.hxx>

#include "globals.hh"
#include "waveformmppc.hh"
#include "configure.hh"


class EventLYSO
{
public:
    EventLYSO() = default;
    EventLYSO(Int_t evtID, std::vector<ROOT::RVecD> times_F, std::vector<ROOT::RVecD> times_B, std::vector<ROOT::RVecD> volts_F, std::vector<ROOT::RVecD> volts_B);
    ~EventLYSO() = default;

    void CalculateEstimatorsForEveryMPPC();

    // Global analysis (left public for debugging)
    inline Int_t FindFrontChOfMaxCharge() const { return ROOT::VecOps::ArgMax(fCharges_F); };
    inline Int_t FindBackChOfMaxCharge() const { return ROOT::VecOps::ArgMax(fCharges_B); };
    inline Int_t FindFrontChOfMaxAmplitude() const { return ROOT::VecOps::ArgMax(fAmplitudes_F); };
    inline Int_t FindBackChOfMaxAmplitude() const { return ROOT::VecOps::ArgMax(fAmplitudes_B); };

    Double_t GetCentroidX(const char* face = "F", Int_t nCircles = 0);
    Double_t GetCentroidY(const char* face = "F", Int_t nCircles = 0);
    std::pair<Double_t, Double_t> GetCentroidStdDev(const char* face = "F", Int_t nCircles = 0);

    // Global Estimation Methods
        // Energy
    void MeasureDetectorCharge(ROOT::RVecI channelsFront = ROOT::VecOps::Range(CHANNELS), ROOT::RVecI channelsBack = ROOT::VecOps::Range(CHANNELS));
        // Time
    void MeasureDetectorTime(Int_t nCircles = ConfigAnalyzer::GetInstance()->nCircles_Time);
        // Position
    void MeasureDetectorPosition(Int_t nCircles = ConfigAnalyzer::GetInstance()->nCircles_Position);

private:
    // Auxiliary methods
    void FillEstimatorsVectors();

    ROOT::RVecI FindFirstNeighbors(Int_t meanCh, Int_t nCircles = 0);
    WaveformMPPC SumWaveforms(ROOT::RVecI channelsFront = ROOT::VecOps::Range(CHANNELS), ROOT::RVecI channelsBack = ROOT::VecOps::Range(CHANNELS));
    WaveformMPPC SumWaveforms(const char* face, ROOT::RVecI channels = ROOT::VecOps::Range(CHANNELS));


    // Members
    Int_t EventAZ;
        // Single estimators
    std::vector<WaveformMPPC> Front;
    std::vector<WaveformMPPC> Back;
        // Global estimators
    Double_t Charge_F;
    Double_t Charge_B;
    Double_t Charge_Tot;
    Double_t Time15_F[5];
    Double_t Time15_B[5];
    Double_t Time25_F[5];
    Double_t Time25_B[5];
    Double_t Time50_F[5];
    Double_t Time50_B[5];
    Double_t Centroid_F[4]; // x, y, sigmax, sigmay
    Double_t Centroid_B[4]; // x, y, sigmax, sigmay

    
    // Vectors of Front and Back estimators
        // Useful for global estimation
    ROOT::RVecD        fCharges_F; //!
    ROOT::RVecD        fAmplitudes_F; //!
    ROOT::RVecD        fTimeCFs15_F; //!
    ROOT::RVecD        fTimeCFs25_F; //!
    ROOT::RVecD        fTimeCFs50_F; //!
    ROOT::RVec<Bool_t> fTrigger_F; //!
    ROOT::RVecD        fCharges_B; //!
    ROOT::RVecD        fAmplitudes_B; //!
    ROOT::RVecD        fTimeCFs15_B; //!
    ROOT::RVecD        fTimeCFs25_B; //!
    ROOT::RVecD        fTimeCFs50_B; //!
    ROOT::RVec<Bool_t> fTrigger_B; //!
};



#endif // EVENTLYSO_HH