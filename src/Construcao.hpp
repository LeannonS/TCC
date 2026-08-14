#ifndef CONSTRUCAO_HPP
#define CONSTRUCAO_HPP

#include <vector>
#include <map>

using namespace std;

class Construcao {
public:
    Construcao();

    // Essa struct vai anotar todas as preferencias de inicio de ferias dos maquinistas
    struct RegistroInicioFerias {
        int idMaquinista;
        vector<int> diasInicio;
    };

    // Função para ordenação de um vector que define a lógica de adicionar maquinistas na escala e salvar o inicio das ferias
    static bool compararMaquinistas(const RegistroInicioFerias& a, const RegistroInicioFerias& b);
    // Define as primeiras tarefas do maquinista antes de começar a usar uma sequencia pre determinada
    static void definirTarefasIniciais(int idMaquinista, vector<vector<int>>& escala, vector<vector<int>>& tarefasAlocadas);
    // Função responsável por alocar as férias dos maquinistas
    static int alocarFeriasMquinistas(int k, 
                                        int numMaquinistas,
                                        int tentativas, 
                                        vector<vector<int>> preferencias, 
                                        vector<vector<int>>& alocacaoFeriasMquinistas,
                                        vector<int> diaFeriasMaquinista,
                                        vector<vector<int>> tarefasAlocadas,
                                        vector<int>& maquinistasDispensados,
                                        vector<vector<int>> &blocosFeriasMaquinista,
                                        vector<vector<int>>& escala);

    void gerarEscalaAleatoria(int k, 
                               int numTarefas, 
                               int numMaquinistas,
                               map<int, vector<int>>& transicoesDeTarefas,
                               vector<vector<int>>& escala);

    void gerarEscalaValida(int k, 
                            int numTarefas, 
                            int numMaquinistas,
                            map<int, vector<int>>& transicoesDeTarefas,
                            vector<vector<int>>& escala,
                            vector<vector<int>>& preferencias,
                            vector<int>& diaFeriasMaquinista,
                            vector<vector<int>>& blocosFeriasMaquinista,
                            vector<int>& maquinistasDispensados);
};

#endif