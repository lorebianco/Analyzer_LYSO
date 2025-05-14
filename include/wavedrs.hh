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
    Double_t baseline = 0.450;

    // Costruttori
    WaveDRS() = default;
    WaveDRS(ROOT::RVecD t, ROOT::RVecD s) : times(t), samples(s) {}

    // Set baseline operator
    inline void SetBaseline(Double_t newbase) { baseline = newbase; };

    // Operatore di assegnazione
    WaveDRS& operator=(const WaveDRS& other)
    {
        if(this != &other)
        {
            times = other.times;
            samples = other.samples;
            baseline = other.baseline;
        }
        return *this;
    }

    // Funzione di interpolazione lineare
    static Double_t LinearInterpolate(Double_t x0, Double_t y0, Double_t x1, Double_t y1, Double_t x)
    {
        return y0 + (y1 - y0) * (x - x0) / (x1 - x0);
    }

    WaveDRS operator+(const WaveDRS& other) const
    {
        if(times.size() != other.times.size())
        {
            throw std::invalid_argument("The time vectors must have the same size");
        }

        WaveDRS result;
        Double_t baseline_media = (baseline + other.baseline) / 2.0;

        for(size_t i = 0; i < times.size(); i++)
        {
            Double_t time1 = times[i];
            Double_t time2 = other.times[i];
        
            if(TMath::Abs(time1 - time2) < 1e-6)  // Tempi quasi uguali
            {
                result.times.push_back(time1);
                // Somma le ampiezze rispetto alla propria baseline e aggiungi la baseline media
                Double_t summed_sample = (samples[i] - baseline) + (other.samples[i] - other.baseline) + baseline_media;
                result.samples.push_back(summed_sample);
            }
            else
            {
                Double_t meanTime = (time1 + time2) / 2;
                Double_t interpSample1, interpSample2;

                if(i == 0 || i == times.size() - 1)  // Bordi
                {
                    interpSample1 = samples[i];
                    interpSample2 = other.samples[i];
                }
                else
                {
                    interpSample1 = LinearInterpolate(times[i-1], samples[i-1], times[i+1], samples[i+1], meanTime);
                    interpSample2 = LinearInterpolate(other.times[i-1], other.samples[i-1], other.times[i+1], other.samples[i+1], meanTime);
                }

                Double_t summed_sample = (interpSample1 - baseline) + (interpSample2 - other.baseline) + baseline_media;
                result.times.push_back(meanTime);
                result.samples.push_back(summed_sample);
            }
        }

        result.baseline = baseline_media;  // Setta la baseline media come baseline del risultato
        return result;
    }   

    // Operatore di somma composto (+=)
    WaveDRS& operator+=(const WaveDRS& other)
    {
        if(times.size() != other.times.size())
        {
            throw std::invalid_argument("The time vectors must have the same size");
        }

        Double_t baseline_media = (baseline + other.baseline) / 2.0;

        for(size_t i = 0; i < times.size(); i++)
        {
            Double_t time1 = times[i];
            Double_t time2 = other.times[i];

            if(TMath::Abs(time1 - time2) < 1e-6)  // Tempi quasi uguali
            {
                // Somma le ampiezze rispetto alla propria baseline e aggiungi la baseline media
                samples[i] = (samples[i] - baseline) + (other.samples[i] - other.baseline) + baseline_media;
            }
            else  // Tempi diversi, usa interpolazione o approssimazione ai bordi
            {
                Double_t meanTime = (time1 + time2) / 2;
                Double_t interpSample1, interpSample2;

                if (i == 0 || i == times.size() - 1)  // Gestione dei bordi
                {
                    interpSample1 = samples[i];
                    interpSample2 = other.samples[i];
                }
                else  // Interpolazione normale
                {
                    interpSample1 = LinearInterpolate(times[i-1], samples[i-1], times[i+1], samples[i+1], meanTime);
                    interpSample2 = LinearInterpolate(other.times[i-1], other.samples[i-1], other.times[i+1], other.samples[i+1], meanTime);
                }

                // Somma le ampiezze interpolate rispetto alle rispettive baseline e aggiungi la baseline media
                samples[i] = (interpSample1 - baseline) + (interpSample2 - other.baseline) + baseline_media;
                times[i] = meanTime;  // Aggiorna il tempo medio
            }
        }

        baseline = baseline_media;  // Aggiorna la baseline con la baseline media
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