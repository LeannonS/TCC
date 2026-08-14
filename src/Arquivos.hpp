#ifndef Arquivos_HPP
#define Arquivos_HPP

#include <vector>
#include <string>

using namespace std;

class Arquivos
{
    public:
        Arquivos();

        static bool lerArquivoStart(const string& nomeArquivo, vector<vector<int>>& escala);

        static bool lerArquivoVac(const string& nomeArquivo, vector<vector<int>>& preferencias, 
                                vector<int>& diaFeriasMaquinista, int& totalDiaFeriasMaquinistas, vector<vector<int>> &blocosFeriasMaquinista);
};

#endif