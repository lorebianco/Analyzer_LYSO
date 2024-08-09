#ifndef WAVEDRS_HH
#define WAVEDRS_HH

#include <iostream>

#include <TMath.h>
#include <ROOT/RVec.hxx>
#include <TCanvas.h>
#include <TGraph.h>

#include "globals.hh"


struct WaveDRS
{
    ROOT::RVecD times;
    ROOT::RVecD samples;

    // Costruttori
    WaveDRS() = default;
    WaveDRS(ROOT::RVecD t, ROOT::RVecD s) : times(t), samples(s) {}

    // Operatore di assegnazione
    WaveDRS& operator=(const WaveDRS& other)
    {
        if(this != &other)
        {
            times = other.times;
            samples = other.samples;
        }
        return *this;
    }

    // Operatore di somma
    WaveDRS operator+(const WaveDRS& other) const
    {
        if(times.size() != other.times.size())
        {
            throw std::invalid_argument("The time vectors must have the same size");
        }

        WaveDRS result = *this;
        result.times = other.times;
        result.samples += other.samples;
        
        return result;
    }

    // Operatore di somma composto (+=)
    WaveDRS& operator+=(const WaveDRS& other)
    {
        if(times.size() != other.times.size())
        {
            throw std::invalid_argument("The time vectors must have the same size");
        }

        times = other.times;
        samples += other.samples;

        return *this;
    }

    // Stampa per debugging
    void PrintWave() const
    {
        std::cout << "Times: ";
        for(const auto& t : times)
        {
            std::cout << t << " ";
        }
        std::cout << std::endl;

        std::cout << "Samples: ";
        for(const auto& s : samples)
        {
            std::cout << s << " ";
        }
        std::cout << std::endl;
    }

    // Draw per debugging
    void DrawWave()
    {
        auto c1 = new TCanvas("Debug canvas");
        auto graph = new TGraph(SAMPLINGS, times.data(), samples.data());
        graph->SetMarkerStyle(20);
        graph->Draw("APL");
        c1->Print("c1Debug.png");
    
        delete graph;
        delete c1;
    }
};

#endif // WAVEDRS_HH