#ifndef ANALYZER_LYSO
#define ANALYZER_LYSO

#include <vector>

#include "globals.hh"

using Waveform = std::vector<Double_t>;


// Algorithms for energy estimation
namespace LYSO_Energy
{
    Double_t energyIntegral(const Waveform& wave);
    Double_t energyFit(const Waveform& wave);
};


// Algorithms for time estimation
namespace LYSO_Time
{
    Double_t timeCF(const Waveform& wave);
};


// Algorithms for position estimation
namespace LYSO_Position
{
    Double_t positionAlgorithm_1(const Waveform& wave);
};


#endif // ANALYZER_LYSO