#include "eventlyso.hh"

using namespace std;
using namespace ROOT;


EventLYSO::EventLYSO(Int_t evtID, vector<RVecD> times_F, vector<RVecD> times_B, vector<RVecD> volts_F, vector<RVecD> volts_B)
    : Event(evtID), Front(CHANNELS), Back(CHANNELS), fCharges_F(CHANNELS), fCharges_B(CHANNELS), fAmplitudes_F(CHANNELS), fAmplitudes_B(CHANNELS), fTimeCFs_F(CHANNELS), fTimeCFs_B(CHANNELS)
{
    for(Int_t i = 0; i < CHANNELS; i++)
    {
        Front[i] = WaveformMPPC(i, times_F[i], volts_F[i]);
        Back[i] = WaveformMPPC(i, times_B[i], volts_B[i]);
    }
}



void EventLYSO::CalculateEstimatorsForEveryMPPC()
{
    for(Int_t i = 0; i < CHANNELS; i++)
    {
        Front[i].MeasureCharge();
        Front[i].MeasureAmplitude();
        Front[i].MeasureTimeCF();

        Back[i].MeasureCharge();
        Back[i].MeasureAmplitude();
        Back[i].MeasureTimeCF();
    }

    FillEstimatorsVectors();
}



void EventLYSO::FillEstimatorsVectors()
{
    for(Int_t i = 0; i < CHANNELS; i++)
    {
        fCharges_F[i] = Front[i].GetCharge();
        fAmplitudes_F[i] = Front[i].GetAmplitude();
        fTimeCFs_F[i] = Front[i].GetTimeCF();
        
        fCharges_B[i] = Back[i].GetCharge();
        fAmplitudes_B[i] = Back[i].GetAmplitude();
        fTimeCFs_B[i] = Back[i].GetTimeCF();
    }
}



RVecI EventLYSO::FindFirstNeighbors(Int_t meanCh, Int_t nNeighbors)
{
    // Note! It returns meanCh ITSELF plus nNeighbors

    if(nNeighbors < 1)
    {
        nNeighbors = 1;
        cerr << "Invalid nNeighbors, minimum is 1! Fixed to default value = 1" << endl;
    }
    nNeighbors++;
    
    RVecI channels = ROOT::VecOps::Range(CHANNELS);
    RVecD distances = sqrt(pow((detX-detX[meanCh]),2)+pow((detY-detY[meanCh]),2));
    RVecI sortCh = StableArgsort(distances);
    channels = Take(channels, sortCh);

    return Take(channels, nNeighbors);
}



RVecI EventLYSO::FindFirstNeighbors(Double_t meanX, Double_t meanY, Int_t nNeighbors)
{    
    if(nNeighbors < 1)
    {
        nNeighbors = 1;
        cerr << "Invalid nNeighbors, minimum is 1! Fixed to default value = 1" << endl;
    }
    
    RVecI channels = ROOT::VecOps::Range(CHANNELS);
    RVecD distances = sqrt(pow((detX-meanX),2)+pow((detY-meanY),2));
    RVecI sortCh = StableArgsort(distances);
    channels = Take(channels, sortCh);

    return Take(channels, nNeighbors);
}



Double_t EventLYSO::GetMeanRadius(const char* face, const char* optR0, Int_t nOfChannels)
{
    Double_t fX0, fY0, bX0, bY0;
    RVecI channelsFront, channelsBack;

    if(strcmp(optR0, "CHMAX") == 0 || strcmp(optR0, "chmax") == 0)
    {
        fX0 = GetFrontMaxX();
        fY0 = GetFrontMaxY();
        bX0 = GetBackMaxX();
        bY0 = GetBackMaxY();

        channelsFront = FindFirstNeighbors(FindFrontChOfMaxAmplitude(),nOfChannels-1);
        channelsBack = FindFirstNeighbors(FindBackChOfMaxAmplitude(), nOfChannels-1);
    }
    else if(strcmp(optR0, "MEAN") == 0 || strcmp(optR0, "mean") == 0)
    {
        fX0 = GetFrontMeanX();
        fY0 = GetFrontMeanY();
        bX0 = GetBackMeanX();
        bY0 = GetBackMeanY();
    
        channelsFront = FindFirstNeighbors(GetFrontMeanX(), GetFrontMeanY(), nOfChannels);
        channelsBack = FindFirstNeighbors(GetBackMeanX(), GetBackMeanY(), nOfChannels);
    }
    else
    {
        cerr << "Not valid optR0 input! Valids are: CHMAX (position of channel of maximum), MEAN (mean X and Y).\n Set default value: CHMAX" << endl;
        return GetMeanRadius(face, "CHMAX", nOfChannels);
    }

    if(strcmp(face, "F") == 0 || strcmp(face, "f") == 0)
    {
        auto detXvec = Take(detX, channelsFront);
        auto detYvec = Take(detY, channelsFront);
        auto charges_Fvec = Take(fCharges_F, channelsFront);

        return Sum(sqrt(pow((detXvec-fX0),2)+pow((detYvec-fY0),2))*charges_Fvec)/Sum(charges_Fvec); 
    }
    else if(strcmp(face, "B") == 0 || strcmp(face, "b") == 0)
    {
        auto detXvec = Take(detX, channelsBack);
        auto detYvec = Take(detY, channelsBack);
        auto charges_Bvec = Take(fCharges_B, channelsBack);

        return Sum(sqrt(pow((detXvec-bX0),2)+pow((detYvec-bY0),2))*charges_Bvec)/Sum(charges_Bvec);
    }
    else
    {
        cerr << "Not valid face input! Set default value: Front Face" << endl;
        return GetMeanRadius("F", optR0, nOfChannels);
    }
}



WaveformMPPC EventLYSO::SumWaveforms(const char* face, RVecI channels)
{
    RVecD tt(SAMPLINGS), vv(SAMPLINGS);
    WaveDRS outWave(tt,vv);

    if(strcmp(face, "F") == 0 || strcmp(face, "f") == 0)
    {
        for(auto ch : channels)
        {
            outWave += Front[ch].GetWave();
        }        
    }
    else if(strcmp(face, "B") == 0 || strcmp(face, "b") == 0)
    {
        for(auto ch : channels)
        {
            outWave += Back[ch].GetWave();
        }
    }
    else
    {
        cerr << "Not valid face input! Set default value: Front Face" << endl;
        return SumWaveforms("F", channels);
    }

    return WaveformMPPC(outWave);
}



WaveformMPPC EventLYSO::SumWaveforms(RVecI channelsFront, RVecI channelsBack)
{
    RVecD tt(SAMPLINGS), vv(SAMPLINGS);
    WaveDRS outWave(tt,vv);

    for(auto ch : channelsFront)
    {
        outWave += Front[ch].GetWave();
    }
    for(auto ch : channelsBack)
    {
        outWave += Back[ch].GetWave();
    }

    return WaveformMPPC(outWave);
}



void EventLYSO::MeasureDetectorCharge(RVecI channelsFront, RVecI channelsBack)
{
    RVecI chargeVec_F = Take(fCharges_F, channelsFront);
    RVecI chargeVec_B = Take(fCharges_B, channelsBack);
    
    Charge_F = Sum(chargeVec_F);
    Charge_B = Sum(chargeVec_B);
    Charge_Tot = Charge_F + Charge_B;
}