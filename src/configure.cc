#include "configure.hh"

using namespace std;


void ConfigAnalyzer::LoadConfig(const char* filename)
{
    ifstream file(filename);
    string line;

    if(!file.is_open())
    {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    while(getline(file, line))
    {
        // Rimuove gli spazi iniziali e finali
        line = line.substr(line.find_first_not_of(" \t"));

        // Ignora righe vuote e commenti
        if(line.empty() || line[0] == '#')
        {
            continue;
        }

        // Trova il carattere '='
        size_t equalPos = line.find('=');
        if(equalPos == string::npos)
        {
            continue; // Se non c'è un '=' nella riga, ignoriamo la riga
        }

        // Estrai il nome del parametro e il suo valore
        string paramName = line.substr(0, equalPos);
        string paramValue = line.substr(equalPos + 1);

        // Rimuove gli spazi bianchi iniziali e finali usando regex
        paramName = regex_replace(paramName, regex("^\\s+|\\s+$"), "");
        paramValue = regex_replace(paramValue, regex("^\\s+|\\s+$"), "");

        // Gestisci i vari parametri
        if(paramName == "lowBase")
        {
            lowBase = stoi(paramValue);
        }
        else if(paramName == "upBase")
        {
            upBase = stoi(paramValue);
        }
        else if(paramName == "lowInt")
        {
            lowInt = stoi(paramValue);
        }
        else if(paramName == "upInt")
        {
            upInt = stoi(paramValue);
        }
        else if(paramName == "timeCF")
        {
            timeCF = stof(paramValue);
        }
        else if(paramName == "timeCF_Sum")
        {
            timeCF_Sum = stof(paramValue);
        }
        else
        {
            cerr << "Unknown parameter: " << paramName << endl;
        }
    }

    file.close();
}


void ConfigAnalyzer::PrintDebug() const
{
    cout << "ConfigAnalyzer parameters:" << endl;
    cout << "lowBase: " << lowBase << endl;
    cout << "upBase: " << upBase << endl;
    cout << "lowInt: " << lowInt << endl;
    cout << "upInt: " << upInt << endl;
    cout << "timeCF: " << timeCF << endl;
    cout << "timeCF_Sum: " << timeCF_Sum << endl;
}

