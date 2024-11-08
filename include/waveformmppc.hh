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
#include "configure.hh"

class WaveformMPPC
{
  public:
    WaveformMPPC() = default;
    WaveformMPPC(Int_t chid, ROOT::RVecD times, ROOT::RVecD volts);
    WaveformMPPC(WaveDRS wave);
    ~WaveformMPPC() = default;
    
    // Methods for estimation
    void MeasureCharge(Int_t binStart = ConfigAnalyzer::GetInstance()->lowInt, Int_t binStop = ConfigAnalyzer::GetInstance()->upInt);
    void MeasureAmplitude(Int_t binStart = ConfigAnalyzer::GetInstance()->lowInt, Int_t binStop = ConfigAnalyzer::GetInstance()->upInt);
    void MeasureTimeCF(Float_t frac = ConfigAnalyzer::GetInstance()->timeCF);
    void MeasureBaseline(Int_t binStart = ConfigAnalyzer::GetInstance()->lowBase, Int_t binStop = ConfigAnalyzer::GetInstance()->upBase);

    // Getters
    inline WaveDRS GetWave() const { return fWave; }

    inline Double_t GetCharge() const { return Charge; }
    inline Double_t GetAmplitude() const { return Amplitude; }
    inline Double_t GetTimeCF() const { return TimeCF; }

  private:
    // Auxiliary methods
    Double_t CrossingPoint(Double_t value, Bool_t isGreaterOrLesser, Int_t binStart, Int_t binEnd);
    

    WaveDRS fWave; //!
    Int_t Ch; // Channel of MPPC

    // Estimators
    Double_t Charge,
             Amplitude,
             TimeCF;

    // Others
    Double_t Baseline,
             SigmaNoise;
};


#endif // WAVEFORMMPPC_HH