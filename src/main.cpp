/*
    Tarefa      Tipo de serviço         Atividade a ser realizada
    0           Trem batedor            Saída de BH com destino a GV
    1           Trem de passageiros     Saída de BH com destino a GV
    2           Trem batedor            Saída de GV com destino a Vitória
    3           Trem de passageiros     Saída de GV com destino a Vitória
    4           Trem batedor            Saída de Vitória com destino GV
    5           Trem de passageiros     Saída de Vitória com destino GV
    6           Trem batedor            Saída de GV com destino a BH
    7           Trem de passageiros     Saída de GV com destino a BH
    8           Tarefa especial         Tarefa especial na ponta em BH
    9           Tarefa especial         Tarefa especial na ponta em Vitória
    10          Folga dia 1             Folga em GV
    11          Folga dia 2             Folga em GV
*/

/*
    0 e 1: Podem seguir para 6, 7, 2, 3 ou 10.
    2 e 3: Podem seguir para 9.
    4 e 5: Podem seguir para 6, 7, 2, 3 ou 10.
    6 e 7: Podem seguir para 8.
    8: Pode seguir para 0 ou 1.
    9: Pode seguir para 4 ou 5.
    10: Pode seguir para 11.
    11: Pode seguir para 6, 7, 2, 3 ou 11.
*/

/*
	0 a 7 → aparecem exatamente uma vez cada.
	8, 9 → aparecem duas vezes cada, 
	10, 11 → aparece várias vezes
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <algorithm>

#include "Utils.hpp"
#include "Arquivos.hpp"
#include "Construcao.hpp"

using namespace std;

const int k = 360; // Horizonte de planejamento (número de dias)
const int numTarefas = 12;
const int numMaquinistas = 20;
const string nomeArquivoVac = "vac_360_0";

// Matriz de distância entre tarefas
// Distância 0 indica que a transição é possível
// Distância 10 indica que a transição não é possível
vector<vector<int>> matDist = {
    {10, 10, 0, 0, 10, 10, 0, 0, 10, 10, 0, 10}, 
    {10, 10, 0, 0, 10, 10, 0, 0, 10, 10, 0, 10}, 
    {10, 10, 10, 10, 10, 10, 10, 10, 10, 0, 10, 10}, 
    {10, 10, 10, 10, 10, 10, 10, 10, 10, 0, 10, 10}, 
    {10, 10, 0, 0, 10, 10, 0, 0, 10, 10, 0, 10}, 
    {10, 10, 0, 0, 10, 10, 0, 0, 10, 10, 0, 10}, 
    {10, 10, 10, 10, 10, 10, 10, 10, 0, 10, 10, 10}, 
    {10, 10, 10, 10, 10, 10, 10, 10, 0, 10, 10, 10}, 
    {0, 0, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10}, 
    {10, 10, 10, 10, 0, 0, 10, 10, 10, 10, 10, 10}, 
    {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 0}, 
    {10, 10, 0, 0, 10, 10, 0, 0, 10, 10, 10, 0}
};

// Transições possíveis de cada tarefa
map<int, vector<int>> transicoesDeTarefas = {
    {0, {6, 7, 2, 3, 10}},
    {1, {6, 7, 2, 3, 10}},
    {2, {9}},
    {3, {9}},
    {4, {6, 7, 2, 3, 10}},
    {5, {6, 7, 2, 3, 10}},
    {6, {8}},
    {7, {8}},
    {8, {0, 1}},
    {9, {4, 5}},
    {10, {11}},
    {11, {6, 7, 2, 3, 11}}
};

// Transições possíveis de cada tarefa com uma lógica para manter um padrão de alocação de tarefas
map<int, vector<int>> auxiliarDeTransicoesDeTarefas = {
    {0, {6}},
    {1, {7}},
    {2, {9}},
    {3, {9}},
    {4, {2}},
    {5, {3}},
    {6, {8}},
    {7, {8}},
    {8, {0, 1}},
    {9, {4, 5}},
    {10, {11}},
    {11, {6, 7, 2, 3, 11}}
};

int main()
{
    srand(time(0));

    // Instâncias de classes
    Utils utils;
    Arquivos arq;
    Construcao c;

    double fo, // Valor da função objetivo
            fo_atual = 0; // Salva o valor da função objetivo da solução atual

    bool ver; // Auxiliar que verificar se a reconstrução foi válida

    // Variáveis para cálculo da função objetivo
    int inviabilidadesEscala = 0, // Conta o número de inviabilidades na escala a cada 7 dias. <= 6 := 0, caso contrário := 1
        totalDiaFeriasMaquinistas = 0, // Conta o total de dias de férias dos maquinistas (quantos numeros -10 tem no arquivo vac)
        numMaquinistasDevendoFerias = 0, // Conta o número de maquinistas que não tiraram o número de ferias alocadas
        satisfacaoMax = 0, // Salva qual o o pior valor de satisfação possível
        satisfacao = 0, // Conta o nível de satisfação dos maquinistas com a escala gerada (quanto maior pior)
        somatorioDist = 0, // Somatório das distâncias percorrida por todos maquinistas
        maquinistasUtilizados = 0, // Conta o número de maquinistas que foram utilizados (trabalhou no minimo 1 dia)
        tarefasSemMaquinista = 0, // Verifica se algum dia do horizonte de planejamento ficou sem maquinista alocado para a tarefa
        cont = 1; // Variável auxiliar na recostrução parcial da solução (vai destruindo 10% da solução a cada tentativa de melhora)
        
    vector<vector<int>> escala(numMaquinistas); // Escala dos maquinistas (maquinista e tarefa no dia)
    vector<int> diaFeriasMaquinista(numMaquinistas); // Conta os dias de férias alocados para cada maquinista
    vector<vector<int>> preferencias(numMaquinistas, vector<int>(k)); // Preferências dos maquinistas para cada dia -10 ferias, -1 indiferente, 5 quer trabalhar
    // Armazena os blocos de férias para cada maquinista (Exemplo: maquinista tem ferias alocadas como 15 dias e depois 15 dias)
    // Nesse exemplo não pode acontecer do mesmo tirar 10+10+10 mas ele pode tirar 30 dias diretos
    vector<vector<int>> blocosFeriasMaquinista(numMaquinistas); 
    vector<int> maquinistasDispensados; // Maquinistas a serem dispensados

    // Le o arquivo start
    if (!arq.lerArquivoStart("start0", escala))
    {
        return 1;
    }

    // Le o arquivo vac
    if (!arq.lerArquivoVac(nomeArquivoVac, preferencias, diaFeriasMaquinista, totalDiaFeriasMaquinistas, blocosFeriasMaquinista))
    {
        return 1;
    }

    satisfacaoMax = totalDiaFeriasMaquinistas * 10; // Cada dia de férias alocado que o maquinista prefere folgar soma 10 ao nível máximo de satisfação
    
    // Chama a função para gerar uma escala de forma aleatória
    // c.gerarEscalaAleatoria(k, numTarefas, numMaquinistas, transicoesDeTarefas, escala);

    c.gerarEscalaValida(k, numTarefas, numMaquinistas, auxiliarDeTransicoesDeTarefas, escala, preferencias, 
                        diaFeriasMaquinista, blocosFeriasMaquinista, maquinistasDispensados);

    inviabilidadesEscala = utils.calcularInviabilidadesEscala(escala, numMaquinistas, k);
    somatorioDist = utils.calcularSomatorioDistancia(matDist, escala, numMaquinistas, k);
    satisfacao = utils.calcularSatisfacao(preferencias, escala, numMaquinistas, k);
    numMaquinistasDevendoFerias = utils.calcularFeriasNaoAtendidas(escala, k, numMaquinistas, blocosFeriasMaquinista);
    maquinistasUtilizados = utils.calcularMaquinistasUtilizados(escala, numMaquinistas, k);
    tarefasSemMaquinista = utils.calcularTarefasSemMaquinista(escala, numMaquinistas, k);

    fo = utils.calcula_fo(inviabilidadesEscala, somatorioDist, numMaquinistas, satisfacao, satisfacaoMax,
                            numMaquinistasDevendoFerias, maquinistasUtilizados, tarefasSemMaquinista, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

    cout << "Função Objetivo: " << fixed << setprecision(4) << fo << endl << endl;
    utils.salvarResultado("resultado.txt", escala, numMaquinistas, k);

    while(cont <= 10)
    {
        for(int i = 0; i < 10; i++)
        {
            // zera a escala de todos maquinistas para o dia 0
            for(int m = 0; m < numMaquinistas; m++)
            {
                if(!escala[m].empty()) 
                {
                    escala[m].resize(1);
                }
            }

            // Chama a função para gerar uma escala de forma aleatória
            ver = utils.reconstroiParcialmenteSolucao(k, numTarefas, numMaquinistas, cont, auxiliarDeTransicoesDeTarefas, escala, preferencias, 
                                diaFeriasMaquinista, blocosFeriasMaquinista, maquinistasDispensados);

            if(ver)
            {
                inviabilidadesEscala = utils.calcularInviabilidadesEscala(escala, numMaquinistas, k);
                somatorioDist = utils.calcularSomatorioDistancia(matDist, escala, numMaquinistas, k);
                satisfacao = utils.calcularSatisfacao(preferencias, escala, numMaquinistas, k);
                numMaquinistasDevendoFerias = utils.calcularFeriasNaoAtendidas(escala, k, numMaquinistas, blocosFeriasMaquinista);
                maquinistasUtilizados = utils.calcularMaquinistasUtilizados(escala, numMaquinistas, k);
                tarefasSemMaquinista = utils.calcularTarefasSemMaquinista(escala, numMaquinistas, k);

                fo_atual = utils.calcula_fo(inviabilidadesEscala, somatorioDist, numMaquinistas, satisfacao, satisfacaoMax,
                                        numMaquinistasDevendoFerias, maquinistasUtilizados, tarefasSemMaquinista, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);


                // cout << fo_atual << endl;
                if(fo_atual < fo)
                {     
                    fo = fo_atual;
                    cout << "Função Objetivo: " << fixed << setprecision(4) << fo << endl << endl;
                    utils.salvarResultado("resultado.txt", escala, numMaquinistas, k);
                }
            }
        }

        cont++;
    }

    return 0;
}
