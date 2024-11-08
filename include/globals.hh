#ifndef GLOBALS_HH
#define GLOBALS_HH

#include <vector>
#include <utility>
#include <ROOT/RVec.hxx>

constexpr Int_t CHANNELS = 115; /**< @brief Number of channels of the detectors */
constexpr Int_t SAMPLINGS = 1024; /**< @brief Number of samplings for one waveform */

// Coordinates of MPPCs (Si layers)
extern const ROOT::RVecD detX; //!
extern const ROOT::RVecD detY; //!

constexpr Double_t xSideDet = 7.35; // mm 
constexpr Double_t ySideDet = 6.85; // mm 

#endif // GLOBALS_HH
