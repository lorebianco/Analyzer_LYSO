#include "waveformmppc.hh"

using namespace std;
using namespace ROOT;


WaveformMPPC::WaveformMPPC(Int_t chid, RVecD times, RVecD volts)
{
    Ch = chid;
    fWave = WaveDRS(times, volts);
    
    MeasureBaseline();
    fWave.SetBaseline(Baseline);
}



WaveformMPPC::WaveformMPPC(WaveDRS wave)
{
    fWave = wave;
}



void WaveformMPPC::MeasureCharge(Int_t binStart, Int_t binStop)
{
    // Convention is [binStart, binStop]
    binStop++;
    auto w = Take(fWave.samples, binStop);
    w = Take(w, binStart - binStop);
    auto t = Take(fWave.times, binStop);
    t = Take(t, binStart - binStop);

    Charge = 0;
    for(auto i = 0; i < (binStop - binStart - 1); i++)
    {
        Charge += (2*Baseline - (w[i] + w[i+1]))*(t[i+1]-t[i])*0.5;
    }
}



void WaveformMPPC::MeasureAmplitude(Int_t binStart, Int_t binStop)
{
    // Convention is [binStart, binStop]
    binStop++;
    auto w = Take(fWave.samples, binStop);
    w = Take(w, binStart - binStop);

    Amplitude = Max(Baseline - w);
    
    // Set trigger boolean
    Trigger = Amplitude > -ConfigAnalyzer::GetInstance()->trgLevel;
}



void WaveformMPPC::MeasureTimeCF(Float_t frac, Int_t leFrac)
{
    MeasureAmplitude();
    Double_t thr = Baseline - Amplitude*frac;
    
    if(!Trigger)
    {
        TimeCF15 = -1; TimeCF25 = -1; TimeCF50 = -1;
        return;
    }

    Int_t trgCell = CrossingPoint(Baseline + ConfigAnalyzer::GetInstance()->trgLevel, false, ZERO_TIME_BIN, 1023);
    Int_t binOfTimeSup, binOfTimeInf;

    if(thr < (Baseline + ConfigAnalyzer::GetInstance()->trgLevel))
    {
        binOfTimeSup = CrossingPoint(thr, false, trgCell, 1023);
        binOfTimeInf = CrossingPoint(thr, true, binOfTimeSup, ZERO_TIME_BIN);
    }
    else
    {
        binOfTimeInf = CrossingPoint(thr, true, trgCell, ZERO_TIME_BIN);
        binOfTimeSup = CrossingPoint(thr, false, binOfTimeInf, 1023);
    }

    pair<Double_t, Double_t> infSample = make_pair(fWave.times[binOfTimeInf], fWave.samples[binOfTimeInf]);
    pair<Double_t, Double_t> supSample = make_pair(fWave.times[binOfTimeSup], fWave.samples[binOfTimeSup]);

    // Linear interpolation
    Double_t fTimeCF = ((thr - infSample.second)/(supSample.second - infSample.second))*(supSample.first - infSample.first) + infSample.first;

    if(fTimeCF < 0.0)
    {
        cerr << "Le PROBLEM for frac = " << leFrac << "Threshold at " << thr << " Estimation at = " << fTimeCF << endl;
    }

    switch(leFrac)
    {
        case 15:
        default:
            TimeCF15 = fTimeCF;
            break;
        case 25:
            TimeCF25 = fTimeCF;
            break;
        case 50:
            TimeCF50 = fTimeCF;
            break;
    }
}



void WaveformMPPC::MeasureBaseline(Int_t binStart, Int_t binStop)
{
    if(binStart >= binStop)
        cerr << "Limits not valid!" << endl;

    // Convention is [binStart, binStop]
    binStop++;
    auto w = Take(fWave.samples, binStop);
    w = Take(w, binStart - binStop);

    Baseline = Mean(w);
    SigmaNoise = StdDev(w);
}



Int_t WaveformMPPC::CrossingPoint(Double_t value, Bool_t isGreaterOrLesser, Int_t binStart, Int_t binEnd)
{
    auto& data = fWave.samples;

    // Lambda function for comparison
    auto compare = [value, isGreaterOrLesser](Double_t sample)
    {
        return isGreaterOrLesser ? sample >= value : sample <= value;
    };

    // Check for search direction based on binStart and binEnd
    if(binStart <= binEnd)
    {
        // Forward search
        for (Int_t i = binStart; i <= binEnd; ++i)
        {
            if (compare(data[i]))
            {
                return i; // Return the bin index where the condition is first met
            }
        }
    }
    else
    {
        // Reverse search
        for (Int_t i = binStart; i >= binEnd; --i)
        {
            if (compare(data[i]))
            {
                return i; // Return the bin index where the condition is first met
            }
        }
    }

    // No crossing point found
    cerr << "ERROR! CROSSING POINT NOT FOUND for value: " << value << ", starting from bin: " << binStart << " to bin: " << binEnd << endl;
    return -1;
}
