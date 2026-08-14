#include "Arquivos.hpp"

#include <iostream>
#include <fstream>

Arquivos::Arquivos() {}

// Le o arquivo start
bool Arquivos::lerArquivoStart(const string& nomeArquivo, vector<vector<int>>& escala)
{
    ifstream arquivo(nomeArquivo);

    if (!arquivo.is_open())
    {
        cerr << "Erro ao abrir: " << nomeArquivo << "\n";
        return false;
    }

    int maquinista, tarefaAtual;

    // O arquivo start esta em ordem crescente, logo as posições do vetor correspondem aos maquinistas
    while (arquivo >> maquinista >> tarefaAtual)
    {
        escala[maquinista].push_back(tarefaAtual); // Maquinista recebe a tarefa inicial do arquivo start0
    }
    arquivo.close();

    return true;
}

// Le o arquivo vac
bool Arquivos::lerArquivoVac(const string& nomeArquivo, vector<vector<int>>& preferencias, 
                              vector<int>& diaFeriasMaquinista, int& totalDiaFeriasMaquinistas, vector<vector<int>>& blocosFeriasMaquinista)
{
    ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open())
    {
        cerr << "Erro ao abrir: " << nomeArquivo << "\n";
        return false;
    }

    int maquinista, dia, val;
    vector<int> cont(blocosFeriasMaquinista.size(), 0);
    totalDiaFeriasMaquinistas = 0;

    while (arquivo >> maquinista >> dia >> val)
    {
        preferencias[maquinista][dia] = val;

        if (val == -10)
        {
            cont[maquinista]++;
            diaFeriasMaquinista[maquinista]++;
            totalDiaFeriasMaquinistas++;
        }
        else
        {
            if (cont[maquinista] > 0)
            {
                blocosFeriasMaquinista[maquinista].push_back(cont[maquinista]);
                cont[maquinista] = 0;
            }
        }
    }
    arquivo.close();

    for (size_t i = 0; i < cont.size(); i++) 
    {
        if (cont[i] > 0) 
        {
            blocosFeriasMaquinista[i].push_back(cont[i]);
        }
    }

    return true;
}