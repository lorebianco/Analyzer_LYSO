#ifndef WAVEFORMMPPC_HH
#define WAVEFORMMPPC_HH

#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>

#include <TMath.h>
#include <ROOT/RVec.hxx>

#include "globals.hh"
#include "wavedrs.hh"


class WaveformMPPC
{
public:
    WaveformMPPC() = default;
    WaveformMPPC(Int_t chid, ROOT::RVecD times, ROOT::RVecD volts);
    WaveformMPPC(WaveDRS wave);
    ~WaveformMPPC() = default;
    
    // Methods for estimation
    void MeasureCharge();
    void MeasureAmplitude();
    void MeasureTimeCF(Float_t frac = 0.25);
    // void MeasureEstimatorsWithFit();

    void MeasureBaseline(Int_t binStart = 100, Int_t binStop = 300);


    // Getters
    inline WaveDRS GetWave() const { return fWave; }

    inline Double_t GetCharge() const { return Charge; }
    inline Double_t GetAmplitude() const { return Amplitude; }
    inline Double_t GetTimeCF() const { return TimeCF; }
    // inline Double_t GetChargeFit() const { return ChargeFit; }
    //inline Double_t GetAmplitudeFit() const { return AmplitudeFit; }
    //inline Double_t GetTimeFit() const { return TimeFit; }

private:
    // Auxiliary methods
    Double_t CrossingPoint(Double_t value, Bool_t isGreaterOrLesser, Int_t binStart, Int_t binEnd);
    

    WaveDRS fWave; //!
    Int_t Ch; // Channel of MPPC

    // Estimators
    Double_t Charge,
             Amplitude,
             TimeCF;
    
    Double_t ChargeFit; //!
    Double_t AmplitudeFit; //!
    Double_t TimeFit; //!

    // Others
    Double_t Baseline,
             SigmaNoise;
};


#endif // WAVEFORMMPPC_HH