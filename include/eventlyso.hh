#ifndef EVENTLYSO_HH
#define EVENTLYSO_HH

#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cstring>

#include <TMath.h>
#include <ROOT/RVec.hxx>

#include "globals.hh"
#include "waveformmppc.hh"


class EventLYSO
{
public:
    EventLYSO() = default;
    EventLYSO(Int_t evtID, std::vector<ROOT::RVecD> times_F, std::vector<ROOT::RVecD> times_B, std::vector<ROOT::RVecD> volts_F, std::vector<ROOT::RVecD> volts_B);
    ~EventLYSO() = default;

    void CalculateEstimatorsForEveryMPPC();

    // Global analysis
    inline Int_t FindFrontChOfMaxCharge() const { return ROOT::VecOps::ArgMax(fCharges_F); };
    inline Int_t FindBackChOfMaxCharge() const { return ROOT::VecOps::ArgMax(fCharges_B); };
    inline Int_t FindFrontChOfMaxAmplitude() const { return ROOT::VecOps::ArgMax(fAmplitudes_F); };
    inline Int_t FindBackChOfMaxAmplitude() const { return ROOT::VecOps::ArgMax(fAmplitudes_B); };

    inline Double_t GetFrontMeanX() const { return Sum(detX*fCharges_F)/Sum(fCharges_F); }; 
    inline Double_t GetFrontMeanY() const { return Sum(detY*fCharges_F)/Sum(fCharges_F); }; 
    inline Double_t GetBackMeanX() const { return Sum(detX*fCharges_B)/Sum(fCharges_B); }; 
    inline Double_t GetBackMeanY() const { return Sum(detY*fCharges_B)/Sum(fCharges_B); };
    inline Double_t GetFrontMaxX() const { return detX[FindFrontChOfMaxAmplitude()]; }; 
    inline Double_t GetFrontMaxY() const { return detY[FindFrontChOfMaxAmplitude()]; }; 
    inline Double_t GetBackMaxX() const { return detX[FindBackChOfMaxAmplitude()]; }; 
    inline Double_t GetBackMaxY() const { return detY[FindBackChOfMaxAmplitude()]; };

    Double_t GetMeanRadius(const char* face = "F", const char* optR0 = "CHMAX", Int_t nOfChannels = CHANNELS);

    // Global Estimation Methods
        // Energy
    void MeasureDetectorCharge(ROOT::RVecI channelsFront = ROOT::VecOps::Range(CHANNELS), ROOT::RVecI channelsBack = ROOT::VecOps::Range(CHANNELS));
        // Time

        // Position

private:
    // Auxiliary methods
    void FillEstimatorsVectors();
    
    ROOT::RVecI FindFirstNeighbors(Int_t meanCh, Int_t nNeighbors = 8);
    ROOT::RVecI FindFirstNeighbors(Double_t meanX, Double_t meanY, Int_t nNeighbors = 9);
    
    WaveformMPPC SumWaveforms(ROOT::RVecI channelsFront = ROOT::VecOps::Range(CHANNELS), ROOT::RVecI channelsBack = ROOT::VecOps::Range(CHANNELS));
    WaveformMPPC SumWaveforms(const char* face, ROOT::RVecI channels = ROOT::VecOps::Range(CHANNELS));


    // Members
    Int_t Event;
    std::vector<WaveformMPPC> Front;
    std::vector<WaveformMPPC> Back;
    
    // Vectors of Front and Back estimators
        // Useful for global estimation
    ROOT::RVecD fCharges_F; //!
    ROOT::RVecD fAmplitudes_F; //!
    ROOT::RVecD fTimeCFs_F; //!
    ROOT::RVecD fCharges_B; //!
    ROOT::RVecD fAmplitudes_B; //!
    ROOT::RVecD fTimeCFs_B; //!


    // Global estimators
    Double_t Charge_F;
    Double_t Charge_B;
    Double_t Charge_Tot;
    
    // Double_t FrontMaxCharge;
    // Int_t FrontChMaxCharge;
    // Double_t BackMaxCharge;
    // Int_t BackChMaxCharge;
    
};



#endif // EVENTLYSO_HH