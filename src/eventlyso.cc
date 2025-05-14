#include "eventlyso.hh"

using namespace std;
using namespace ROOT;
using namespace ROOT::VecOps;


EventLYSO::EventLYSO(Int_t evtID, vector<RVecD> times_F, vector<RVecD> times_B, vector<RVecD> volts_F, vector<RVecD> volts_B)
    : EventAZ(evtID), Front(CHANNELS), Back(CHANNELS), fCharges_F(CHANNELS), fCharges_B(CHANNELS), fAmplitudes_F(CHANNELS), fAmplitudes_B(CHANNELS), fTimeCFs15_F(CHANNELS), fTimeCFs15_B(CHANNELS), fTimeCFs25_F(CHANNELS), fTimeCFs25_B(CHANNELS), fTimeCFs50_F(CHANNELS), fTimeCFs50_B(CHANNELS),
    fTrigger_F(CHANNELS), fTrigger_B(CHANNELS)
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
        Front[i].MeasureTimeCF(0.15, 15);
        Front[i].MeasureTimeCF(0.25, 25);
        Front[i].MeasureTimeCF(0.50, 50);

        Back[i].MeasureCharge();
        Back[i].MeasureAmplitude();
        Back[i].MeasureTimeCF(0.15, 15);
        Back[i].MeasureTimeCF(0.25, 25);
        Back[i].MeasureTimeCF(0.50, 50);
    }

    FillEstimatorsVectors();
}



void EventLYSO::FillEstimatorsVectors()
{
    for(Int_t i = 0; i < CHANNELS; i++)
    {
        fCharges_F[i] = Front[i].GetCharge();
        fAmplitudes_F[i] = Front[i].GetAmplitude();
        fTimeCFs15_F[i] = Front[i].GetTimeCF15();
        fTimeCFs25_F[i] = Front[i].GetTimeCF25();
        fTimeCFs50_F[i] = Front[i].GetTimeCF50();
        fTrigger_F[i] = Front[i].GetTrigger();
        
        fCharges_B[i] = Back[i].GetCharge();
        fAmplitudes_B[i] = Back[i].GetAmplitude();
        fTimeCFs15_B[i] = Back[i].GetTimeCF15();
        fTimeCFs25_B[i] = Back[i].GetTimeCF25();
        fTimeCFs50_B[i] = Back[i].GetTimeCF50();
        fTrigger_B[i] = Back[i].GetTrigger();
    }
}



RVecI EventLYSO::FindFirstNeighbors(Int_t meanCh, Int_t nCircles)
{
    // Note! It returns meanCh ITSELF plus the neighbors
    if(nCircles < 0)
    {
        nCircles = 0;
        cerr << "Invalid nCircles, minimum is 0! Fixed to default value = 0" << endl;
    }
    Double_t epsilon = 0.01;
    
    Double_t xMax = nCircles*xSideDet + epsilon;
    Double_t yMax = nCircles*ySideDet + epsilon;

    RVecI channels = Range(115);
    RVecD distX = abs(detX-detX[meanCh]);
    RVecD distY = abs(detY-detY[meanCh]);
    
    return Take(channels, Nonzero(distX < xMax && distY < yMax));
}



WaveformMPPC EventLYSO::SumWaveforms(const char* face, RVecI channels)
{
    RVecD tt = Range(0, SAMPLINGS);
    RVecD vv(SAMPLINGS, 0.);
    WaveDRS outWave(tt, vv);

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
    RVecD tt = Range(0, SAMPLINGS);
    RVecD vv(SAMPLINGS, 0.);
    WaveDRS outWave(tt, vv);

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



Double_t EventLYSO::GetCentroidX(const char* face, Int_t nCircles)
{
    // Some useful indices
    Int_t chAmpMaxFront = FindFrontChOfMaxAmplitude();
    Int_t chAmpMaxBack = FindBackChOfMaxAmplitude();
    
    auto channelsFront = FindFirstNeighbors(chAmpMaxFront, nCircles);
    auto channelsBack = FindFirstNeighbors(chAmpMaxBack, nCircles);

    if(strcmp(face, "F") == 0 || strcmp(face, "f") == 0)
    {
        auto detXvec = Take(detX, channelsFront);
        auto charges_Fvec = Take(fCharges_F, channelsFront);

        return Sum(detXvec*charges_Fvec)/Sum(charges_Fvec);
    }
    else if(strcmp(face, "B") == 0 || strcmp(face, "b") == 0)
    {
        auto detXvec = Take(detX, channelsBack);
        auto charges_Bvec = Take(fCharges_B, channelsBack);

        return Sum(detXvec*charges_Bvec)/Sum(charges_Bvec);
    }
    else
    {
        cerr << "Not valid face input! Set default value: Front Face" << endl;
        return GetCentroidX("F", nCircles);
    }
}



Double_t EventLYSO::GetCentroidY(const char* face, Int_t nCircles)
{
    // Some useful indices
    Int_t chAmpMaxFront = FindFrontChOfMaxAmplitude();
    Int_t chAmpMaxBack = FindBackChOfMaxAmplitude();
    
    auto channelsFront = FindFirstNeighbors(chAmpMaxFront, nCircles);
    auto channelsBack = FindFirstNeighbors(chAmpMaxBack, nCircles);

    if(strcmp(face, "F") == 0 || strcmp(face, "f") == 0)
    {
        auto detYvec = Take(detY, channelsFront);
        auto charges_Fvec = Take(fCharges_F, channelsFront);

        return Sum(detYvec*charges_Fvec)/Sum(charges_Fvec);
    }
    else if(strcmp(face, "B") == 0 || strcmp(face, "b") == 0)
    {
        auto detYvec = Take(detY, channelsBack);
        auto charges_Bvec = Take(fCharges_B, channelsBack);

        return Sum(detYvec*charges_Bvec)/Sum(charges_Bvec);
    }
    else
    {
        cerr << "Not valid face input! Set default value: Front Face" << endl;
        return GetCentroidY("F", nCircles);
    }
}



pair<Double_t, Double_t> EventLYSO::GetCentroidStdDev(const char* face, Int_t nCircles)
{
    Double_t X0, Y0;
    RVecI channelsFront, channelsBack;

    X0 = GetCentroidX(face, nCircles);
    Y0 = GetCentroidY(face, nCircles);
    Int_t chAmpMaxFront = FindFrontChOfMaxAmplitude();
    Int_t chAmpMaxBack = FindBackChOfMaxAmplitude();
    
    channelsFront = FindFirstNeighbors(chAmpMaxFront, nCircles);
    channelsBack = FindFirstNeighbors(chAmpMaxBack, nCircles);

    if(strcmp(face, "F") == 0 || strcmp(face, "f") == 0)
    {
        auto detXvec = Take(detX, channelsFront);
        auto detYvec = Take(detY, channelsFront);
        auto charges_Fvec = Take(fCharges_F, channelsFront);

        Double_t sigmaX = TMath::Sqrt(Sum(pow(detXvec - X0, 2)*charges_Fvec) / Sum(charges_Fvec));
        Double_t sigmaY = TMath::Sqrt(Sum(pow(detYvec - Y0, 2)*charges_Fvec) / Sum(charges_Fvec));
        return {sigmaX, sigmaY};
    }
    else if(strcmp(face, "B") == 0 || strcmp(face, "b") == 0)
    {
        auto detXvec = Take(detX, channelsBack);
        auto detYvec = Take(detY, channelsBack);
        auto charges_Bvec = Take(fCharges_B, channelsBack);

        Double_t sigmaX = TMath::Sqrt(Sum(pow(detXvec - X0, 2)*charges_Bvec) / Sum(charges_Bvec));
        Double_t sigmaY = TMath::Sqrt(Sum(pow(detYvec - Y0, 2)*charges_Bvec) / Sum(charges_Bvec));
        return {sigmaX, sigmaY};
    }
    else
    {
        cerr << "Not valid face input! Set default value: Front Face" << endl;
        return GetCentroidStdDev("F", nCircles);
    }
}



void EventLYSO::MeasureDetectorCharge(RVecI channelsFront, RVecI channelsBack)
{
    RVecI chargeVec_F = Take(fCharges_F, channelsFront);
    RVecI chargeVec_B = Take(fCharges_B, channelsBack);
    
    Charge_F = Sum(chargeVec_F);
    Charge_B = Sum(chargeVec_B);
    Charge_Tot = Charge_F + Charge_B;
}



void EventLYSO::MeasureDetectorTime(Int_t nCircles)
{
    // Some useful indices and waveforms
    Int_t chAmpMaxFront = FindFrontChOfMaxAmplitude();
    Int_t chAmpMaxBack = FindBackChOfMaxAmplitude();
    
    auto neighborsMaxFront = FindFirstNeighbors(chAmpMaxFront, nCircles);
    auto neighborsMaxBack = FindFirstNeighbors(chAmpMaxBack, nCircles);
    
    RVecI trgIndices_F = Nonzero(fTrigger_F);
    RVecI trgIndices_B = Nonzero(fTrigger_B);

    auto intersection_F = Intersect(neighborsMaxFront, trgIndices_F);
    auto intersection_B = Intersect(neighborsMaxBack, trgIndices_B);

    WaveformMPPC sumWaveFront = SumWaveforms("F", neighborsMaxFront);
    WaveformMPPC sumWaveBack = SumWaveforms("B", neighborsMaxBack);
    

    // Single waves: First -> Entry 0
    Time15_F[0] = Min(Take(fTimeCFs15_F, trgIndices_F));
    Time15_B[0] = Min(Take(fTimeCFs15_B, trgIndices_B));

    // Single waves: Higher -> Entry 1
    Time15_F[1] = fTimeCFs15_F[chAmpMaxFront];
    Time15_B[1] = fTimeCFs15_B[chAmpMaxBack];
    
    // Single waves around higher: Average -> Entry 2
    Time15_F[2] = Mean(Take(fTimeCFs15_F, intersection_F));
    Time15_B[2] = Mean(Take(fTimeCFs15_B, intersection_B));

    // Single waves around higher: Weighted average -> Entry 3
    Double_t wmFront15 = Sum(Take(fTimeCFs15_F*fAmplitudes_F, intersection_F)) / Sum(Take(fAmplitudes_F, intersection_F));
    Double_t wmBack15 = Sum(Take(fTimeCFs15_B*fAmplitudes_B, intersection_B)) / Sum(Take(fAmplitudes_B, intersection_B));
    Time15_F[3] = wmFront15;
    Time15_B[3] = wmBack15;

    // Sum of waves around higher -> Entry 4
    sumWaveFront.MeasureTimeCF(0.15, 15);
    sumWaveBack.MeasureTimeCF(0.15, 15);
    Time15_F[4] = sumWaveFront.GetTimeCF15();
    Time15_B[4] = sumWaveBack.GetTimeCF15();


    // Single waves: First -> Entry 0
    Time25_F[0] = Min(Take(fTimeCFs25_F, trgIndices_F));
    Time25_B[0] = Min(Take(fTimeCFs25_B, trgIndices_B));

    // Single waves: Higher -> Entry 1
    Time25_F[1] = fTimeCFs25_F[chAmpMaxFront];
    Time25_B[1] = fTimeCFs25_B[chAmpMaxBack];
    
    // Single waves around higher: Average -> Entry 2
    Time25_F[2] = Mean(Take(fTimeCFs25_F, intersection_F));
    Time25_B[2] = Mean(Take(fTimeCFs25_B, intersection_B));

    // Single waves around higher: Weighted average -> Entry 3
    Double_t wmFront25 = Sum(Take(fTimeCFs25_F*fAmplitudes_F, intersection_F)) / Sum(Take(fAmplitudes_F, intersection_F));
    Double_t wmBack25 = Sum(Take(fTimeCFs25_B*fAmplitudes_B, intersection_B)) / Sum(Take(fAmplitudes_B, intersection_B));
    Time25_F[3] = wmFront25;
    Time25_B[3] = wmBack25;

    // Sum of waves around higher -> Entry 4
    sumWaveFront.MeasureTimeCF(0.25, 25);
    sumWaveBack.MeasureTimeCF(0.25, 25);
    Time25_F[4] = sumWaveFront.GetTimeCF25();
    Time25_B[4] = sumWaveBack.GetTimeCF25();


    // Single waves: First -> Entry 0
    Time50_F[0] = Min(Take(fTimeCFs50_F, trgIndices_F));
    Time50_B[0] = Min(Take(fTimeCFs50_B, trgIndices_B));

    // Single waves: Higher -> Entry 1
    Time50_F[1] = fTimeCFs50_F[chAmpMaxFront];
    Time50_B[1] = fTimeCFs50_B[chAmpMaxBack];
    
    // Single waves around higher: Average -> Entry 2
    Time50_F[2] = Mean(Take(fTimeCFs50_F, intersection_F));
    Time50_B[2] = Mean(Take(fTimeCFs50_B, intersection_B));

    // Single waves around higher: Weighted average -> Entry 3
    Double_t wmFront50 = Sum(Take(fTimeCFs50_F*fAmplitudes_F, intersection_F)) / Sum(Take(fAmplitudes_F, intersection_F));
    Double_t wmBack50 = Sum(Take(fTimeCFs50_B*fAmplitudes_B, intersection_B)) / Sum(Take(fAmplitudes_B, intersection_B));
    Time50_F[3] = wmFront50;
    Time50_B[3] = wmBack50;

    // Sum of waves around higher -> Entry 4
    sumWaveFront.MeasureTimeCF(0.50, 50);
    sumWaveBack.MeasureTimeCF(0.50, 50);
    Time50_F[4] = sumWaveFront.GetTimeCF50();
    Time50_B[4] = sumWaveBack.GetTimeCF50();
}



void EventLYSO::MeasureDetectorPosition(Int_t nCircles)
{
    // xCentroid
    Centroid_F[0] = GetCentroidX("F", nCircles);
    Centroid_B[0] = GetCentroidX("B", nCircles);

    // y Centroid
    Centroid_F[1] = GetCentroidY("F", nCircles);
    Centroid_B[1] = GetCentroidY("B", nCircles);

    // sigma Centroid
    auto stdF = GetCentroidStdDev("F", nCircles);
    auto stdB = GetCentroidStdDev("B", nCircles);
    
    Centroid_F[2] = stdF.first;
    Centroid_B[2] = stdB.first;
    
    Centroid_F[3] = stdF.second;
    Centroid_B[3] = stdB.second;
}