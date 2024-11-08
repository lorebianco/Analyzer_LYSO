#ifndef CONFIGANALYZER_HH
#define CONFIGANALYZER_HH

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <regex>

#include <TMath.h>

class ConfigAnalyzer
{
  public:
    // Singleton
    static ConfigAnalyzer* GetInstance()
    {
        static ConfigAnalyzer instance;
        return &instance;
    }

    // Configurable parameters
    Int_t lowBase;
    Int_t upBase;
    Int_t lowInt;
    Int_t upInt;
    Float_t timeCF;
    Float_t timeCF_Sum;

    // Load configuration from a file
    void LoadConfig(const char* filename);
    void PrintDebug() const;

  private:
    ConfigAnalyzer() = default; // Private constructor for singleton pattern
    ConfigAnalyzer(const ConfigAnalyzer&) = delete;
    ConfigAnalyzer& operator=(const ConfigAnalyzer&) = delete;
};


#endif // CONFIGANALYZER_HH
