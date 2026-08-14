#include "Construcao.hpp"
#include <cstdlib>
#include <algorithm>
#include <iostream>

Construcao::Construcao() {}

bool Construcao::compararMaquinistas(const RegistroInicioFerias& a, const RegistroInicioFerias& b)
{
    // Se a não tem férias e b tem, a vem primeiro
    if (a.diasInicio.empty() && !b.diasInicio.empty()) return true;
    // Se b não tem e a tem, b vem primeiro
    if (!a.diasInicio.empty() && b.diasInicio.empty()) return false;
    
    // Se ambos estão vazios, mantém a ordem pelo ID
    if (a.diasInicio.empty() && b.diasInicio.empty()) return a.idMaquinista < b.idMaquinista;

    // Se ambos têm inicio de ferias, pega o dia de início mais cedo de cada um
    int minA = *min_element(a.diasInicio.begin(), a.diasInicio.end());
    int minB = *min_element(b.diasInicio.begin(), b.diasInicio.end());

    if (minA != minB) {
        return minA < minB; // Menor dia vem primeiro
    }
    
    return a.idMaquinista < b.idMaquinista; 
}

void Construcao::definirTarefasIniciais(int id, vector<vector<int>>& escala, vector<vector<int>>& tarefasAlocadas)
{
    int tarefaInicial = escala[id][0];

    switch (tarefaInicial)
    {
        case 10: escala[id].insert(escala[id].end(), {11}); break;
        case 0:  escala[id].insert(escala[id].end(), {2, 9, 4, 10, 11}); break;
        case 1:  escala[id].insert(escala[id].end(), {3, 9, 5, 10, 11}); break;
        case 4:  escala[id].insert(escala[id].end(), {6, 8, 0, 10, 11}); break;
        case 5:  escala[id].insert(escala[id].end(), {7, 8, 1, 10, 11}); break;
        case 2:  escala[id].insert(escala[id].end(), {9, 4, 6, 8, 0, 10, 11}); break;
        case 3:  escala[id].insert(escala[id].end(), {9, 5, 7, 8, 1, 10, 11}); break;
        case 6:  escala[id].insert(escala[id].end(), {8, 0, 2, 9, 4, 10, 11}); break;
        case 7:  escala[id].insert(escala[id].end(), {8, 1, 3, 9, 5, 10, 11}); break;

        case 8:  
            if(tarefasAlocadas[0][1] == 0)
            {
                escala[id].insert(escala[id].end(), {0, 10, 11});
                tarefasAlocadas[0][1]++;
            }
            else escala[id].insert(escala[id].end(), {1, 10, 11});
            break;

        case 9:  
            if(tarefasAlocadas[4][1] == 0)
            {
                escala[id].insert(escala[id].end(), {4, 10, 11});
                tarefasAlocadas[4][1]++;
            }
            else escala[id].insert(escala[id].end(), {5, 10, 11});
            break;

        default: break;
    }
}

// Deixa previamente alocado as ferias dos maquinistas
// Essa construção não adiciona 2 dias a mais de ferias porem pode ser que alguns maquinistas não consigam tirar as férias alocadas
// int Construcao::alocarFeriasMquinistas(int k, 
//                                         int numMaquinistas, 
//                                         vector<vector<int>> preferencias, 
//                                         vector<vector<int>>& alocacaoFeriasMaquinistas,
//                                         vector<int> diaFeriasMaquinista,
//                                         vector<vector<int>> tarefasAlocadas)
// {
//     for(int i = 0; i < k; i++)
//     {
//         for(int j = 0; j < numMaquinistas; j++)
//         {
//             if(diaFeriasMaquinista[j] == 0) continue;

//             // Verifica se o maquinista quer tirar ferias nesse dia, se não existe 2 maquinistas de ferias
//             // Verifica também se o maquinista consegue tirar todas férias alocadas antes do horizonte de eventos terminar
//             if(preferencias[j][i] == -10 && tarefasAlocadas[12][i] < 2 && diaFeriasMaquinista[j]+i <= k)
//             {
//                 // Caso ele comece as férias ja aloca a mesma completamente antes de seguir com os maquinistas
//                 for(int cont = i; cont < k; cont++)
//                 {
//                     if(preferencias[j][cont] != -10) break;

//                     tarefasAlocadas[12][cont]++;
//                     preferencias[j][cont] = 5;
//                     diaFeriasMaquinista[j]--;
//                     alocacaoFeriasMaquinistas[j][cont]++;
//                 }
//             }
//         }
//     }

//     // Caso algum maquinista não tenha tirado todas as férias alocadas resolve o problema
//     for(int i = 0; i < numMaquinistas; i++)
//     {
//         if(diaFeriasMaquinista[i] == 0) continue;
        
//         // Salva o inicio e o fim do dia que vai alocar as ferias
//         int inicio, fim, cont = 0;

//         for(int j = 0; j < k; j++)
//         {
//             if(tarefasAlocadas[12][j] < 2)
//             {
//                 if (cont == 0) inicio = j;
//                 cont++;
//                 if(cont == diaFeriasMaquinista[i])
//                 {
//                     fim = j;
//                     break;
//                 }
//             }
//             else cont = 0;
//         }

//         // Aloca as férias
//         for(int j = inicio; j <= fim; j++)
//         {
//             tarefasAlocadas[12][j]++;
//             alocacaoFeriasMaquinistas[i][j]++;
//             diaFeriasMaquinista[i] = 0;
//         }
//     }
    
//     return 0;
// }

// Essa alocação adiciona 2 dias a mais de ferias oque pode causar alguns problemas porém funciona na maioria dos casos
int Construcao::alocarFeriasMquinistas(int k, 
                                        int numMaquinistas,
                                        int tentativas,
                                        vector<vector<int>> preferencias, 
                                        vector<vector<int>>& alocacaoFeriasMaquinistas,
                                        vector<int> diaFeriasMaquinista,
                                        vector<vector<int>> tarefasAlocadas,
                                        vector<int>& maquinistasDispensados,
                                        vector<vector<int>>& blocosFeriasMaquinista,
                                        vector<vector<int>>& escala)
{
    int aux = 2; // Variavel para adicionar 2 dias de ferias extras para o maquinista
    // Conta quantas preferencias de ferias foram perdidas para cada maquinista
    // Caso ele não possa alocar as ferias no dia desejado vai aumentando o contador até ser diferente de 0,
    // se ele não tirou as férias e é diferente de 0 o contador zera
    vector<int> preferenciasPerdidas(numMaquinistas);
    vector<vector<int>> blocosFeriasTirados(numMaquinistas); // Salva os blocos de ferias que foram alocados para cada maquinista

    int parada = k;
    if(tentativas!=0) parada = (k-k*0.1*tentativas);

    for(int i = 0; i < parada; i++)
    {
        for(int j = 0; j < numMaquinistas; j++)
        {
            if(diaFeriasMaquinista[j] == 0) continue;
            if(find(maquinistasDispensados.begin(), maquinistasDispensados.end(), j) != maquinistasDispensados.end()) continue;

            int inicio = 0, fim = 0;

            // Verifica se o maquinista quer tirar ferias nesse dia, se não existe 2 maquinistas de ferias
            // Verifica também se o maquinista consegue tirar todas férias alocadas antes do horizonte de eventos terminar
            if(preferencias[j][i] == -10)
            {
                // Caso o maquinista queira tirar ferias no primeiro dia da escala mas não está de folga no primeiro dia da escala adiciona +3 dias
                if(tarefasAlocadas[12][i] < 2 && alocacaoFeriasMaquinistas[j][i] == 0)
                {
                    // Conta quantos dias sobrando o maquinista ainda quer tirar ferias depois do dia i
                    int preferenciasFaltantes = 0;
                    aux = 2;

                    // Se o maquinista tiver marcado um número de ferias maior ou do tamanho da escala menos 2 ja aloca as ferias
                    // Ou caso ele tenha ferias no inicio da escala mas ele ja começou de folga aloca as ferias
                    // Caso contrario realiza a logica abaixo adicionando mais 2 dias de ferias para o maquinista
                    if(diaFeriasMaquinista[j] < k-2 || ((i == 0 || i == 1) && (escala[j][0] == 10 || escala[j][0] == 11)))
                    {
                        if(i == 0 || i == 1)
                        {
                            preferenciasPerdidas[j]++;
                            continue;
                        }
                        else
                        {
                            for(int x = i; x < k; x++)
                            {
                                if(preferencias[j][x] == -10)
                                {
                                    preferenciasFaltantes++;
                                }
                                else break;
                            }

                            if(preferenciasFaltantes+preferenciasPerdidas[j]+i >= k) continue;

                            if(alocacaoFeriasMaquinistas[j][i-2] == 0 || i-2 < 0)
                            {
                                if(tarefasAlocadas[12][i-1] < 2 && i-1 >= 3)// Tenta adicionar 2 dias de ferias antes do marcado
                                {
                                    aux--;
                                    inicio--;

                                    // Apos adicionar o primeiro dia de ferias extra tenta adicionar o segundo dia de ferias extra
                                    if(tarefasAlocadas[12][i-2] < 2 && i-2 >= 4)
                                    {
                                        aux--;
                                        inicio--;
                                    }
                                }
                                if(aux > 0)
                                {
                                    // Adiciona os dias extras de ferias ao fim do periodo de ferias alocado
                                    if(tarefasAlocadas[12][preferenciasPerdidas[j]+preferenciasFaltantes+i+1] < 2 && preferenciasPerdidas[j]+i+1+preferenciasFaltantes < k)
                                    {
                                        aux--;
                                        fim++;

                                        if(tarefasAlocadas[12][preferenciasPerdidas[j]+preferenciasFaltantes+i+2] < 2 && aux > 0 && preferenciasPerdidas[j]+i+2+preferenciasFaltantes < k)
                                        {
                                            aux--;
                                            fim++;
                                        }
                                    }
                                }
                                if(aux != 0) 
                                {
                                    preferenciasPerdidas[j]++;
                                    continue;
                                }
                            }
                            else if(alocacaoFeriasMaquinistas[j][i-1] == 0 && i-1 >= 0) inicio--;
                        }
                    }
                    
                    int contFerias = 0;
                    // Caso ele comece as férias ja aloca a mesma completamente antes de seguir com os maquinistas
                    // Já adiciona os dois dias extras de ferias se necessario para evitar problemas
                    for(int cont = i+inicio; cont < k; cont++)
                    {
                        if(preferencias[j][cont] != -10 && cont >=i)
                        {
                            for(int x = cont; x < cont+fim+preferenciasPerdidas[j]; x++)
                            {
                                tarefasAlocadas[12][x]++;
                                alocacaoFeriasMaquinistas[j][x]++;
                            }
                            break;
                        }

                        preferencias[j][cont] = 5;
                        if(cont>=i)
                        {
                            contFerias++;
                            diaFeriasMaquinista[j]--;
                        }
                        
                        if(alocacaoFeriasMaquinistas[j][cont]+1 == 1)
                        {
                            alocacaoFeriasMaquinistas[j][cont]++;
                            tarefasAlocadas[12][cont]++;
                        } 
                    }
                    // if(j == 1) cout << diaFeriasMaquinista[j]-preferenciasPerdidas[j] << endl;
                    diaFeriasMaquinista[j] -= preferenciasPerdidas[j];
                    blocosFeriasTirados[j].push_back(contFerias+preferenciasPerdidas[j]);
                    preferenciasPerdidas[j] = 0;
                }
                else
                {
                    preferenciasPerdidas[j]++;
                }
            }
            else
            {
                preferenciasPerdidas[j] = 0;
            }
        }
    }

    // Caso o maquinista ainda não tenha tirado todas ferias alocadas e tenha bloco de 30 dias de ferias alocado
    for(int i = 0; i < numMaquinistas; i++)
    {
        if(find(maquinistasDispensados.begin(), maquinistasDispensados.end(), i) != maquinistasDispensados.end()) continue;
        if(diaFeriasMaquinista[i] <= 0) continue;

        vector<int> blocosFeriasFaltantes30Dias;
        for(int elemento : blocosFeriasMaquinista[i]) 
        {
            if(find(blocosFeriasTirados[i].begin(), blocosFeriasTirados[i].end(), elemento) == blocosFeriasTirados[i].end() && elemento == 30) 
            {
                blocosFeriasFaltantes30Dias.push_back(elemento);
            }
        }

        if(blocosFeriasFaltantes30Dias.empty()) continue;

        int inicio, fim = -1, cont = 0;

        for(int j = 3; j < k; j++)
        {
            if(tarefasAlocadas[12][j] < 2 && alocacaoFeriasMaquinistas[i][j] == 0)
            {
                if (cont == 0) inicio = j;
                cont++;
                if(cont == 32)
                {
                    fim = j;
                    break;
                }
            }
            else cont = 0;
        }

        // caso tenha sido possível alocar as férias para o maquinista, aloca as férias
        if(fim != -1)
        {
            // Aloca as férias
            for(int j = inicio; j <= fim; j++)
            {
                tarefasAlocadas[12][j]++;
                alocacaoFeriasMaquinistas[i][j]++;
            }
            diaFeriasMaquinista[i] -= 30;
            blocosFeriasTirados[i].push_back(30);
        }
        else return -1;
        // else{
        //     cont = 0;
        //     // Caso não tenha sido possivel alocar as ferias, realiza a mesma logica porem sem adicionar 2 dias de ferias a mais para o maquinista
        //     for(int j = 2; j < k; j++)
        //     {
        //         if(tarefasAlocadas[12][j] < 2 && alocacaoFeriasMaquinistas[i][j] == 0)
        //         {
        //             if (cont == 0) inicio = j;
        //             cont++;
        //             if(cont == diaFeriasMaquinista[i])
        //             {
        //                 fim = j;
        //                 break;
        //             }
        //         }
        //         else cont = 0;
        //     }

        //     // caso tenha sido possível alocar as férias para o maquinista, aloca as férias
        //     if(fim != -1)
        //     {
        //         // Aloca as férias
        //         for(int j = inicio; j <= fim; j++)
        //         {
        //             tarefasAlocadas[12][j]++;
        //             alocacaoFeriasMaquinistas[i][j]++;
        //         }
        //         diaFeriasMaquinista[i] -= 30;
        //         blocosFeriasTirados[i].push_back(30);
        //     }
        //     else break;
        // }
    }

    // Caso algum maquinista não tenha tirado todas as férias alocadas resolve o problema
    for(int i = 0; i < numMaquinistas; i++)
    {
        if(find(maquinistasDispensados.begin(), maquinistasDispensados.end(), i) != maquinistasDispensados.end()) continue;
        if(diaFeriasMaquinista[i] <= 0) continue;

        // if(i == 7) cout << diaFeriasMaquinista[i] << endl << endl;
        
        // Salva o inicio e o fim do dia que vai alocar as ferias
        int inicio, fim = -1, cont = 0;

        for(int j = 3; j < k; j++)
        {
            if(tarefasAlocadas[12][j] < 2 && alocacaoFeriasMaquinistas[i][j] == 0)
            {
                if (cont == 0) inicio = j;
                cont++;
                if(cont == diaFeriasMaquinista[i]+2)
                {
                    fim = j;
                    break;
                }
            }
            else cont = 0;
        }

        // caso tenha sido possível alocar as férias para o maquinista, aloca as férias
        if(fim != -1)
        {
            // Aloca as férias
            for(int j = inicio; j <= fim; j++)
            {
                tarefasAlocadas[12][j]++;
                alocacaoFeriasMaquinistas[i][j]++;
            }
            diaFeriasMaquinista[i] = 0;
        }
        else cont = 0;
        // else{
        //     cont = 0;
        //     // Caso não tenha sido possivel alocar as ferias, realiza a mesma logica porem sem adicionar 2 dias de ferias a mais para o maquinista
        //     for(int j = 2; j < k; j++)
        //     {
        //         if(tarefasAlocadas[12][j] < 2 && alocacaoFeriasMaquinistas[i][j] == 0)
        //         {
        //             if (cont == 0) inicio = j;
        //             cont++;
        //             if(cont == diaFeriasMaquinista[i])
        //             {
        //                 fim = j;
        //                 break;
        //             }
        //         }
        //         else cont = 0;
        //     }

        //     // caso tenha sido possível alocar as férias para o maquinista, aloca as férias
        //     if(fim != -1)
        //     {
        //         // Aloca as férias
        //         for(int j = inicio; j <= fim; j++)
        //         {
        //             tarefasAlocadas[12][j]++;
        //             alocacaoFeriasMaquinistas[i][j]++;
        //         }
        //         diaFeriasMaquinista[i] = 0;
        //     }
        // }

        if(diaFeriasMaquinista[i] <= 0) continue;

        vector<int> blocosFeriasFaltantes;
        for(int elemento : blocosFeriasMaquinista[i]) 
        {
            if(find(blocosFeriasTirados[i].begin(), blocosFeriasTirados[i].end(), elemento) == blocosFeriasTirados[i].end()) 
            {
                blocosFeriasFaltantes.push_back(elemento);
            }
        }
        sort(blocosFeriasFaltantes.rbegin(), blocosFeriasFaltantes.rend());

        inicio = 2;
        for(int x = 0; x < (int)blocosFeriasFaltantes.size(); x++)
        {
            for(int j = inicio; j < k; j++)
            {
                if(tarefasAlocadas[12][j] < 2 && alocacaoFeriasMaquinistas[i][j] == 0)
                {
                    if (cont == 0) inicio = j;
                    cont++;
                    if(cont == blocosFeriasFaltantes[x]+2 || (cont == blocosFeriasFaltantes[x] && alocacaoFeriasMaquinistas[i][inicio-blocosFeriasFaltantes[x]-1] == 1 && inicio-blocosFeriasFaltantes[x]-1 >= 0))
                    {
                        fim = j;
                        break;
                    }
                }
                else cont = 0;
            }

            // caso tenha sido possível alocar as férias para o maquinista, aloca as férias
            if(fim != -1)
            {
                // Aloca as férias
                for(int j = inicio; j <= fim; j++)
                {
                    tarefasAlocadas[12][j]++;
                    alocacaoFeriasMaquinistas[i][j]++;
                }
                inicio = fim+1;
                diaFeriasMaquinista[i] -= blocosFeriasFaltantes[x];
                blocosFeriasTirados[i].push_back(blocosFeriasFaltantes[x]);
            }
            else return -1;
            // else{
            //     cont = 0;
            //     inicio = 2;
            //     // Caso não tenha sido possivel alocar as ferias, realiza a mesma logica porem sem adicionar 2 dias de ferias a mais para o maquinista
            //     for(int j = inicio; j < k; j++)
            //     {
            //         if(tarefasAlocadas[12][j] < 2 && alocacaoFeriasMaquinistas[i][j] == 0)
            //         {
            //             if (cont == 0) inicio = j;
            //             cont++;
            //             if(cont == blocosFeriasFaltantes[x])
            //             {
            //                 fim = j;
            //                 break;
            //             }
            //         }
            //         else cont = 0;
            //     }

            //     // caso tenha sido possível alocar as férias para o maquinista, aloca as férias
            //     if(fim != -1)
            //     {
            //         // Aloca as férias
            //         for(int j = inicio; j <= fim; j++)
            //         {
            //             tarefasAlocadas[12][j]++;
            //             alocacaoFeriasMaquinistas[i][j]++;
            //         }
            //         inicio = fim+1;
            //         diaFeriasMaquinista[i] -= blocosFeriasFaltantes[x];
            //         blocosFeriasTirados[i].push_back(blocosFeriasFaltantes[x]);
            //     }
            //     else inicio = 2;
            // }
        }
    }

    // // ---------------------------------------------------------
    // for (const auto& maquinista : tarefasAlocadas[12]) {
    //     cout << maquinista << " ";
    // }
    // cout << endl << endl;
    // // ---------------------------------------------------------

    for(int i = 0; i < numMaquinistas; i++)
    {
        if(diaFeriasMaquinista[i] > 0) return -1;
    }

    // // ---------------------------------------------------------
    // for(int j = 0; j < numMaquinistas; j++)
    // {
    //     cout << endl << endl << "Maquinista " << j << ": " << endl;
    //     for(int i = 0; i < k; i++)
    //     {
    //         cout << alocacaoFeriasMaquinistas[j][i] << " ";
    //     }
    // }
    // cout << endl << endl;
    // // ---------------------------------------------------------

    return 1;
}

void Construcao::gerarEscalaAleatoria(int k, 
                                      int numTarefas, 
                                      int numMaquinistas,
                                      map<int, vector<int>>& transicoesDeTarefas,
                                      vector<vector<int>>& escala)
{
    int tarefaAnt10 = 0; // Conta a quantidade de maquinistas que estavam na tarefa 10 no dia anterior
        
    vector<int> diasTrabalhados(numMaquinistas); // Conta os dias trabalhados consecutivos de cada maquinista

    for(int i = 0; i < numMaquinistas; i++)
    {
        // Inicializa a contagem de dias trabalhados
        if(escala[i][0] != 10 && escala[i][0] != 11)
        {
            diasTrabalhados[i] = 1;
        }
        else
        {
            diasTrabalhados[i] = 0;

            if(escala[i][0] == 10) tarefaAnt10++; // Conta quantos maquinistas estao na primeira folga no dia 0
        }
    }

    // Geração aleatória seguindo regras
    for (int i = 1; i < k; i++)
    {
        int tarefaAtual10 = 0; // Numero de maquinistas alocados na tarefa 10 no dia atual

        // Vetor para contar quantos maquinistas estão realizando a tarefa
        // Tarefas 10 e 11 foram consideradas como 1 na contagem
        vector<int> tarefas(numTarefas-1);
        tarefas[10] = tarefaAnt10;

        for (int j = 0; j < numMaquinistas; j++)
        {
            bool transicao = false;
            int tarefaAtual = escala[j][i-1]; // Tarefa atual do maquinista j
            int numTransicoes = transicoesDeTarefas[tarefaAtual].size();
            int proxTarefa;

            while(!transicao)
            {
                proxTarefa = transicoesDeTarefas[tarefaAtual][rand() % numTransicoes];
                if(proxTarefa >= 10)
                {
                    // Caso tivesse 8 maquinistas alocados na tarefa 10 (folga 1) no dia anterior sorteia novamente
                    // Caso contrario o número de maquinistas de folga seja menor que o número total de maquinistas menos as tarefas obrigatorias adiciona na escal
                    // Caso contrario sorteia outra tarefa
                    if(proxTarefa == 10 && tarefaAnt10+tarefaAtual10 < 8 && tarefas[10] < numMaquinistas-12)
                    {
                        escala[j].push_back(proxTarefa);// Maquinista j no dia i recebe a proxima tarefa
                        tarefas[10]++;
                        tarefaAtual10++;
                        diasTrabalhados[j] = 0; // Zera os dias trabalhados por estar de folga
                        transicao = true;
                    }
                    else if(proxTarefa == 11)
                    {
                        escala[j].push_back(proxTarefa);
                        transicao = true;
                    }
                    else
                    {
                        continue;
                    }
                }
                else if(proxTarefa == 0 || proxTarefa == 1 || proxTarefa == 2 || proxTarefa == 3 || 
                        proxTarefa == 4 || proxTarefa == 5 || proxTarefa == 6 || proxTarefa == 7)
                {
                    if(tarefas[proxTarefa] == 0)
                    {
                        escala[j].push_back(proxTarefa);
                        tarefas[proxTarefa]++;
                        diasTrabalhados[j]++;
                        transicao = true;
                    }
                }
                else
                {
                    escala[j].push_back(proxTarefa);
                    tarefas[proxTarefa]++;
                    diasTrabalhados[j]++;
                    transicao = true;
                }
            }
            // cout << "Dia: " << i << "Maquinista: " << j << "       Proxima tarefa: " << proxTarefa << endl;

            // Se a escala de dias trabalhados for maior que 6, diminui o contador de dias trabalhados para validar o próximo periodo
            if(diasTrabalhados[j] > 6)
            {
                diasTrabalhados[j]--;
            }
        }
        tarefaAnt10 = tarefaAtual10;
    }
}

void Construcao::gerarEscalaValida(int k, 
                            int numTarefas, 
                            int numMaquinistas,
                            map<int, vector<int>>& transicoesDeTarefas,
                            vector<vector<int>>& escala,
                            vector<vector<int>>& preferencias,
                            vector<int>& diaFeriasMaquinista,
                            vector<vector<int>>& blocosFeriasMaquinista,
                            vector<int>& maquinistasDispensados)
{       
    vector<int> salvarDiasTrabalhados; // Variavel para salvar os dias trabalhados para passar de um maquinista para outro
    vector<int> diasTrabalhados(numMaquinistas); // Conta os dias trabalhados consecutivos de cada maquinista
    vector<vector<int>> tarefasAlocadas(numTarefas+1, vector<int>(k)); // Salvo o número de maquinistas alocados para cada tarefa, a ultima tarefa da lista é maquinistas de ferias
    vector<vector<int>> alocacaoFeriasMaquinistas(numMaquinistas, vector<int>(k)); // Deixa alocado as férias dos maquinistas
    vector<int> tarefasNecessariasVerificar = {6, 7, 2, 3}; // Tarefas que precisam ser verificadas pois podem ficar sem maquinistas alocados
    vector<int> maquinistasFolga; // Maquinistas que estão de folga no dia 0

    // Salva os maquinistas que começam a escala com folga
    for (int i = 0; i < numMaquinistas; i++) 
    {
        if (escala[i][0] == 10 || escala[i][0] == 11) 
        {
            maquinistasFolga.push_back(i);
        }
    }

    int dispensados = numMaquinistas - 18; // Num maquinistas que vao ser dispensados

    // Seleciona os maquinistas que vão ser dispensados
    if (dispensados > 0) 
    {
        for (int i = 0; i < dispensados; i++) 
        {
            int indiceSorteado = rand() % maquinistasFolga.size();
            
            // seleciona aleatoriamente um maquinista de folga para ser dispensado
            maquinistasDispensados.push_back(maquinistasFolga[indiceSorteado]);

            // cout << "Maquinista dispensado: " << maquinistasFolga[indiceSorteado] << endl;
            
            // Remove o maquinista para que nao seja sorteado novamente
            maquinistasFolga.erase(maquinistasFolga.begin() + indiceSorteado);
        }
        // maquinistasDispensados.push_back(9);
        // maquinistasDispensados.push_back(16);
    }

    // Aloca a tarefa 11 em toda a escala para os maquinistas dispensados
    for(int i = 0; i < (int)maquinistasDispensados.size(); i++)
    {
        diaFeriasMaquinista[maquinistasDispensados[i]] = 0; // Zera os dias de ferias para os maquinistas dispensados
        for(int j = 1; j < k; j++)
        {
            escala[maquinistasDispensados[i]].push_back(11);
        }
    }

    int tentativas = 0;
    int ver = 0; // Variavel para verificar se a alocação gerada esta valida
    
    while(ver != 1 && tentativas < 10)
    {
        // Reseta o vetor para o estado inicial
        for (auto& linha : alocacaoFeriasMaquinistas) fill(linha.begin(), linha.end(), 0);
        ver = Construcao::alocarFeriasMquinistas(k, numMaquinistas, tentativas, preferencias, alocacaoFeriasMaquinistas, diaFeriasMaquinista, tarefasAlocadas, maquinistasDispensados, blocosFeriasMaquinista, escala);
        tentativas++;
    }

    // // ---------------------------------------------------------
    // int a = 0;
    // for (const auto& maquinista : alocacaoFeriasMaquinistas) {
    //     cout << a << endl;
    //     for (int valor : maquinista) {
    //         cout << valor << " ";
    //     }
    //     cout << endl;
    //     a++;
    // }
    // // ---------------------------------------------------------

    // 8 e 9: Trabalharam o 5 dia seguido
    // 0, 1, 4 e 5: Trabalharam o 3 dia seguido
    // 2, 3, 6 e 7: Trabalharam o 1 dia
    for(int i = 0; i < numMaquinistas; i++)
    {
        // Inicializa a contagem de dias trabalhados para cada maquinista de acordo com a tarefa do primeiro dia
        if(escala[i][0] == 8 || escala[i][0] == 9)
        {
            diasTrabalhados[i] = 5;
        }
        else if(escala[i][0] == 0 || escala[i][0] == 1 || escala[i][0] == 4 || escala[i][0] == 5)
        {
            diasTrabalhados[i] = 3;
        }
        else if(escala[i][0] == 2 || escala[i][0] == 3 || escala[i][0] == 6 || escala[i][0] == 7)
        {
            diasTrabalhados[i] = 1;
        }
        else
        {
            diasTrabalhados[i] = 0;
        }
    }

    // Geração valida seguindo regras
    for (int i = 1; i < k; i++)
    {
        // Salva os maquinistas que estão de folga e podem voltar a trabalhar no dia seguinte
        // Os maquinistas na tarefa 11 vão ser os ultimos a serem alocados no dia
        vector<int> tarefaAtual11;

        for (int j = 0; j < numMaquinistas; j++)
        {
            // Caso o maquinista j já tenha uma tarefa alocada no dia i, passa para o próximo maquinista
            if (escala[j].size() > static_cast<size_t>(i)) continue;

            int ultimaTarefa = escala[j][i-1]; // Ultima tarefa realizada pelo maquinista j

            // Caso o maquinista j tenha ferias alocada no dia i e seja possível iniciar as férias
            if(alocacaoFeriasMaquinistas[j][i] == 1 && ultimaTarefa != 2 && ultimaTarefa != 3 && 
                ultimaTarefa != 6 && ultimaTarefa != 7 && ultimaTarefa != 8 && ultimaTarefa != 9)
            {
                int cont = i; // Váriavel para auxiliar a alocar completamente as férias do maquinista
                if(diasTrabalhados[j] != 6 && diasTrabalhados[j] != 0) salvarDiasTrabalhados.push_back(diasTrabalhados[j]);
                diasTrabalhados[j] = 0; // Zera os dias trabalhados por estar de ferias

                if(ultimaTarefa == 0 || ultimaTarefa == 1 || ultimaTarefa == 4 || ultimaTarefa == 5)
                {
                    // Caso o maquinista estivesse trabalhando inicia as férias indo para a tarefa 10
                    escala[j].push_back(10);
                    cont += 1;
                }

                // Aloca completamente as férias do maquinista
                while(cont < k && alocacaoFeriasMaquinistas[j][cont] > 0)
                {
                    cont++;
                    escala[j].push_back(11);
                }
            }
            else
            {
                // Se o maquinista tiver 6 ou mais dias trabalhados, aloca uma folga e zera os dias trabalhados
                if(diasTrabalhados[j] >= 6)
                {
                    escala[j].push_back(10);
                    diasTrabalhados[j] = 0;
                }
                else if(ultimaTarefa != 11) // Caso o maquinista não esteja de folga/ferias e apto a trabalhar no dia seguinte
                {
                    int numTransicoes = transicoesDeTarefas[ultimaTarefa].size();
                    int proxTarefa;

                    while(numTransicoes--)
                    {
                        proxTarefa = transicoesDeTarefas[ultimaTarefa][numTransicoes];

                        // Caso a ultima tarefa realizada tenha sido a tarefa 10 apenas adiciona a próxima tarefa sendo a 11 sem modificações
                        if(ultimaTarefa == 10)
                        {
                            escala[j].push_back(proxTarefa);
                        }
                        else if(proxTarefa == 8 || proxTarefa == 9)
                        {
                            // Caso a próxima tarefa seja 8 ou 9, verifica se o maquinista pode ser alocado nessa tarefa
                            if(tarefasAlocadas[proxTarefa][i] < 2)
                            {
                                escala[j].push_back(proxTarefa);
                                tarefasAlocadas[proxTarefa][i]++;
                                diasTrabalhados[j]++;
                                break;
                            }
                        }
                        else
                        {
                            // Caso a próxima tarefa seja diferente de 8, 9, 10 ou 11, verifica se o maquinista pode ser alocado nessa tarefa
                            if(tarefasAlocadas[proxTarefa][i] < 1)
                            {
                                escala[j].push_back(proxTarefa);
                                tarefasAlocadas[proxTarefa][i]++;
                                diasTrabalhados[j]++;
                                break;
                            }
                        }
                    }
                }
                else
                {
                    // Caso a ultima tarefa do maquinista foi a tarefa 11, salva o maquinista j para alocar uma tarefa valida posteriormente
                    tarefaAtual11.push_back(j);
                }
            }
        }

        for (int idTarefa : tarefasNecessariasVerificar) 
        {
            // Verifica se a tarefa idTarefa ficou sem maquinistas alocados, caso sim aloca um maquinista na mesma
            if (tarefasAlocadas[idTarefa][i] < 1 && !tarefaAtual11.empty()) 
            {
                // Escolhe o maquinista de forma aleatória dentre os maquinistas que estavam de folga no dia anterior
                int indiceSorteado = rand() % tarefaAtual11.size();
                int j = tarefaAtual11[indiceSorteado];

                escala[j].push_back(idTarefa);
                
                // if(i == 31)
                // {
                //     cout << salvarDiasTrabalhados.back() << endl;
                // }
                // Caso o maquinista tenha entrado para a tarefa 10 antes de trabalhar 6 dias seguidos passa os dias de trabalho dele para o proximo maquinista
                if (!salvarDiasTrabalhados.empty())
                {
                    // if(i == 31)
                    // {
                    //     cout << "teste" << endl;
                    // }
                    diasTrabalhados[j] = salvarDiasTrabalhados.back() + 1;
                    salvarDiasTrabalhados.pop_back(); 
                }
                else
                {
                    // Se a lista estiver vazia, ele começa a contagem do zero normalmente
                    diasTrabalhados[j]++;
                }

                tarefaAtual11.erase(tarefaAtual11.begin() + indiceSorteado);
            }
        }
        // Aloca a tarefa 11 para os maquinistas que estavam de folga no dia anterior e não foram alocados para uma tarefa necessária
        for (int idMaquinista : tarefaAtual11) 
        {
            escala[idMaquinista].push_back(11);
        }
    }

    // // ---------------------------------------------------------
    // for(int i = 0; i < k; i++)
    // {
    //     cout << escala[13][i] << " ";
    // }
    // cout << endl << endl;
    // // ---------------------------------------------------------
}