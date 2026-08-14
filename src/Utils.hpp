#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>
#include <map>

using namespace std;

class Utils
{
    public:
        Utils();

        // inviabilidadesEscala conta o número de inviabilidades na escala a cada 7 dias
        // w1 a w5 são os pesos da função objetivo
        // k é o horizonte de planejamento
        // satisfacao representa a soma das preferências dos maquinistas (é somado um valor negativo a cada folga tirada no dia de preferencia)
        double calcula_fo(int inviabilidadesEscala, int dist, int numMaquinistas, int satisfacao, int satisfacao_max,
                            int numMaquinistasDevendoFerias, int maquinistasUtilizados, int tarefasSemMaquinista, float w1, float w2, float w3, float w4, float w5, float w6);

        int calcularInviabilidadesEscala(vector<vector<int>>& escala, int numMaquinistas, int k); // Caso o maquinista tenha mais de 6 dias trabalhados em um período de 7 dias, conta como uma inviabilidade
        int calcularSomatorioDistancia(vector<vector<int>>& matDist, vector<vector<int>>& escala, int numMaquinistas, int k); // Verifica o somatório das distâncias percorridas por todos os maquinistas na escala (soma 10 para cada transição invalida)
        int calcularSatisfacao(vector<vector<int>>& preferencias, vector<vector<int>>& escala, int numMaquinistas, int k); // Calcula a satisfação total dos maquinistas com base na escala gerada
        int calcularFeriasNaoAtendidas(vector<vector<int>>& escala, int k, int numMaquinistas, vector<vector<int>>& blocosFeriasMaquinista); // Conta o número de maquinistas que não tiraram o número de ferias alocadas
        int calcularMaquinistasUtilizados(vector<vector<int>>& escala, int numMaquinistas, int k); // Calcula os maquinistas que foram utilizados (trabalhou no minimo 1 dia)
        int calcularTarefasSemMaquinista(vector<vector<int>>& escala, int numMaquinistas, int k);

        void salvarResultado(string nomeArquivo, const vector<vector<int>>& escala, int numMaquinistas, int k); // Salva a escala gerada em um arquivo

        // Função responsável por alocar as férias dos maquinistas
        static int alocarFeriasMquinistas(int k, 
                                            int numMaquinistas,
                                            int porcentagemDestruicao, 
                                            vector<vector<int>> preferencias, 
                                            vector<vector<int>>& alocacaoFeriasMquinistas,
                                            vector<int> diaFeriasMaquinista,
                                            vector<vector<int>> tarefasAlocadas,
                                            vector<int>& maquinistasDispensados,
                                            vector<vector<int>> &blocosFeriasMaquinista,
                                            vector<vector<int>>& escala);

        // Função que vai destruindo 10% da solução e tentando reconstruir para obter melhora
        bool reconstroiParcialmenteSolucao(int k, 
                                            int numTarefas, 
                                            int numMaquinistas,
                                            int porcentagemDestruicao,
                                            map<int, vector<int>>& transicoesDeTarefas,
                                            vector<vector<int>>& escala,
                                            vector<vector<int>>& preferencias,
                                            vector<int>& diaFeriasMaquinista,
                                            vector<vector<int>>& blocosFeriasMaquinista,
                                            vector<int>& maquinistasDispensados);
};

#endif