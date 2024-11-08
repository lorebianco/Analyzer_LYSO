#include "waveformmppc.hh"

using namespace std;
using namespace ROOT;


WaveformMPPC::WaveformMPPC(Int_t chid, RVecD times, RVecD volts)
{
    Ch = chid;
    fWave = WaveDRS(times, volts);
    MeasureBaseline();
}



WaveformMPPC::WaveformMPPC(WaveDRS wave)
{
    fWave = wave;
    MeasureBaseline();
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

    Amplitude = Max(Baseline - fWave.samples);
}



void WaveformMPPC::MeasureTimeCF(Float_t frac)
{
    MeasureAmplitude();
    Double_t thr = Baseline - Amplitude*frac;
    
    Int_t binOfTimeSup = CrossingPoint(thr, false, 0, 1023);
    Int_t binOfTimeInf = CrossingPoint(thr, true, binOfTimeSup, 0);

    pair<Double_t, Double_t> infSample = make_pair(fWave.times[binOfTimeInf], fWave.samples[binOfTimeInf]);
    pair<Double_t, Double_t> supSample = make_pair(fWave.times[binOfTimeSup], fWave.samples[binOfTimeSup]);

    // Linear interpolation
    TimeCF = ((thr - infSample.second)/(supSample.second - infSample.second))*(supSample.first - infSample.first) + infSample.first;
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



Double_t WaveformMPPC::CrossingPoint(Double_t value, Bool_t isGreaterOrLesser, Int_t binStart, Int_t binEnd)
{
    auto& data = fWave.samples;

    // Lambda function for comparison
    auto compare = [value, isGreaterOrLesser](Double_t sample)
    {
        return isGreaterOrLesser ? sample >= value : sample <= value;
    };

    if(binStart <= binEnd)
    {
        // Forward search
        auto it = find_if(data.begin() + binStart, data.begin() + binEnd + 1, compare);
        if(it != data.begin() + binEnd + 1)
        {
            return distance(data.begin(), it);
        }
    }
    else
    {
        // Reverse search
        auto rit = find_if(make_reverse_iterator(data.begin() + binStart + 1), make_reverse_iterator(data.begin() + binEnd), compare);
        if(rit != make_reverse_iterator(data.begin() + binEnd))
        {
            return distance(data.begin(), rit.base() - 1);
        }
    }

    cerr << "ERROR! CROSSING POINT NOT FOUND!" << endl;
    return -1; // Sample not found
}
