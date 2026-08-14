#include "Utils.hpp"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>

Utils::Utils() {}

// Função Objetivo

// w1 . (dist/(k . 10 . 20) + 1) ---------- Verifica se o maquinista realizou uma transição inválida, caso valor seja diferente de 0 a solulçao é inválida

// w2. n_maquinistas/20 ---------- Verifica quantos maquinistas foram utilizados na escala, quanto menor melhor

// w3 . (inviabilidadesEscala) / ((k-7) . 20) ---------- ?

// w4 . ((dia_ferias*custo_ferias)/(total_dia_ferias_maquinistas*10) + 1) ---------- ?

// w5 . ((total_dia_ferias_maquinistas - dias folga)/total_dia_ferias_maquinistas) +1) ---------- ?

// Para se ter um resultado válido, a função fo deve ser menor ou igual a 2
double Utils::calcula_fo(int inviabilidadesEscala, int dist, int numMaquinistas, int satisfacao, int satisfacao_max, 
                            int numMaquinistasDevendoFerias, int maquinistasUtilizados, int tarefasSemMaquinista, float w1, float w2, float w3, float w4, float w5, float w6)
{
    // cout << numMaquinistasDevendoFerias << endl;
    return w1 * (dist * 3) +
           w2*0 * ((float)maquinistasUtilizados / (float)numMaquinistas) +
           w3 * (inviabilidadesEscala * 3) +
           w4 * (((float)satisfacao + (float)satisfacao_max) / (float)(satisfacao_max*2)) +
           w5 * (numMaquinistasDevendoFerias * 3) +
           w6 * (tarefasSemMaquinista * 3);
}

// Caso o maquinista tenha mais de 6 dias trabalhados em um período de 7 dias, conta como uma inviabilidade
int Utils::calcularInviabilidadesEscala(vector<vector<int>>& escala, int numMaquinistas, int k)
{
    int inviabilidadesEscala = 0;

    for (int i = 0; i < numMaquinistas; i++)
    {
        int diasTrabalhados = 0;

        for (int j = 0; j < k; j++)
        {
            if (escala[i][j] != 10 && escala[i][j] != 11)
            {
                diasTrabalhados++;
            }
            else 
            {
                // Zera a contagem de dias trabalhados ao encontrar um dia de folga
                diasTrabalhados = 0;
            }

            // Se trabalhou mais de 6 dias no período, conta uma inviabilidade
            if (diasTrabalhados > 6)
            {
                inviabilidadesEscala++;
            }
        }
    }
    
    return inviabilidadesEscala;
}

// Verifica o somatório das distâncias percorridas por todos os maquinistas na escala para verificar se não teve transições inválidas de tarefas
int Utils::calcularSomatorioDistancia(vector<vector<int>>& matDist, vector<vector<int>>& escala, int numMaquinistas, int k)
{
    int somatorioDist = 0;

    for (int i = 0; i < numMaquinistas; i++)
    {
        for (int j = 0; j < k - 1; j++)
        {
            int tarefaAtual = escala[i][j];
            int proxTarefa = escala[i][j + 1];

            if (matDist[tarefaAtual][proxTarefa] != 0)
            {
                somatorioDist++;
            }
        }
    }

    return somatorioDist;
}

// Realiza uma contagem da satisfação dos maquinistas com a escala gerada
// Caso ele esteja trabalhando em um dia que ele prefere folgar, soma 10 ao nível de satisfação
// Agora caso ele esteja de folga em um dia que ele prefere folgar, subtrai 10 do nível de satisfação
int Utils::calcularSatisfacao(vector<vector<int>>& preferencias, vector<vector<int>>& escala, int numMaquinistas, int k)
{
    int satisfacao = 0;

    for (int i = 0; i < numMaquinistas; i++)
    {
        for (int j = 0; j < k; j++)
        {
            if (preferencias[i][j] == -10 && escala[i][j] != 10 && escala[i][j] != 11)
            {
                satisfacao += 10;
            }
            else if (preferencias[i][j] == -10 && (escala[i][j] == 10 || escala[i][j] == 11))
            {
                satisfacao += -10;
            }
        }
    }

    return satisfacao;
}

int Utils::calcularFeriasNaoAtendidas(vector<vector<int>>& escala, int k, int numMaquinistas, vector<vector<int>>& blocosFeriasMaquinista)
{
    // Variavel que conta quantos maquinistas não tiveram todos os seus blocos de férias atendidos
    int numMaquinistaFerias = 0, cont = 0;

    for(int i = 0; i < numMaquinistas; i++)
    {
        if(blocosFeriasMaquinista[i].empty()) continue;

        cont = 0;
        vector<int> blocosFeriasRealizados; // Salva os blocos de férias realizados para o maquinista i
        
        for(int j = 0; j < k; j++)
        {
            if(escala[i][j] == 10 || escala[i][j] == 11)
            {
                cont++;
            }
            else 
            {
                // Quando a sequencia de férias acaba salva o bloco realizado se a sequencia tiver sido maior que 2
                if(cont > 2) blocosFeriasRealizados.push_back(cont);
                cont = 0;
            }
        }
        // Se acabar o horizonte de planejamento salva o ultimo bloco realizado caso ele seja maior que 2
        if(cont > 2) blocosFeriasRealizados.push_back(cont);

        sort(blocosFeriasMaquinista[i].rbegin(), blocosFeriasMaquinista[i].rend());
        sort(blocosFeriasRealizados.rbegin(), blocosFeriasRealizados.rend());

        // // ---------------------------------------------------------
        // if(i == 7)
        // {for(int x = 0; x < (int)blocosFeriasRealizados.size(); x++)
        // {
        //     cout << blocosFeriasRealizados[x] << " ";
        // }
        // cout << endl<<endl;}
        // // ---------------------------------------------------------

        vector<int> aux = blocosFeriasMaquinista[i];

        // Tenta colocar os blocos de ferias do maquinista dentro dos blocos de ferias realizadas
        for(int j = 0; j < (int)blocosFeriasRealizados.size(); j++) 
        {
            for(int k = 0; k < (int)aux.size(); k++) 
            {
                if(aux[k] > 0 && blocosFeriasRealizados[j] >= aux[k]) 
                {
                    blocosFeriasRealizados[j] -= aux[k];
                    aux[k] = 0;
                }
            }
        }

        // Verifica se o maquinista não tirou todas as ferias necessárias
        for(int j = 0; j < (int)aux.size(); j++) 
        {
            if(aux[j] > 0) 
            {
                // cout << endl << i << endl << aux[j] << endl;
                numMaquinistaFerias++;
                break;
            }
        }
    }

    return numMaquinistaFerias;
}

int Utils::calcularMaquinistasUtilizados(vector<vector<int>>& escala, int numMaquinistas, int k)
{
    int numMaquinistasUtilizados = 0;

    for (int i = 0; i < numMaquinistas; i++)
    {
        for (int j = 0; j < k; j++)
        {
            if (escala[i][j] != 10 && escala[i][j] != 11)
            {
                numMaquinistasUtilizados++;
                break;
            }
        }
    }

    return numMaquinistasUtilizados;
}

// Verifica se existe dias que tiveram tarefas sem maquinistas alocados
int Utils::calcularTarefasSemMaquinista(vector<vector<int>>& escala, int numMaquinistas, int k)
{
    for (int j = 0; j < k; j++)
    {
        vector<int> contagemTarefas(10, 0); 
        for (int i = 0; i < numMaquinistas; i++)
        {
            int tarefaAtual = escala[i][j];

            if (tarefaAtual >= 0 && tarefaAtual <= 9)
            {
                contagemTarefas[tarefaAtual]++;
            }
        }

        // Caso alguma tarefa necessária não tenha nenhum maquinista alocado, a solução é inválida
        for (int t = 0; t <= 7; t++)
        {
            if (contagemTarefas[t] == 0) 
            {
                cout << j << endl;
                return 1;
            }
        }
        // for (int t = 8; t <= 9; t++)
        // {
        //     if (contagemTarefas[t] < 2) 
        //     {
        //         // cout << j << endl;
        //         return 1;
        //     }
        // }
    }

    return 0;
}

// Gera um arquivo com o resultado da escala gerada
void Utils::salvarResultado(string nomeArquivo, const vector<vector<int>>& escala, int numMaquinistas, int k)
{
    // Gera um arquivo com o resultado e printa o mesmo
    ofstream arquivoSaida(nomeArquivo);
    
    if (!arquivoSaida.is_open())
    {
        cerr << "Erro ao criar arquivo: " << nomeArquivo << "\n";
        return;
    }

    for (int i = 0; i < numMaquinistas; i++)
    {
        arquivoSaida << "Maquinista " << i << ": ";
        // cout << "Maquinista " << i << ": ";

        // CORRIGIR: 0 a k
        for (int j = 0; j < k; j++)
        {
            // if(j == 58)
            // {
            //     arquivoSaida << "((" << escala[i][j] << "))" << " -> ";
            //     // cout << "((" << escala[i][j] << "))" << " -> ";
            // }
            // else
            // {
                arquivoSaida << escala[i][j] << " -> ";
                // cout << escala[i][j] << " -> ";
            // }
        }

        arquivoSaida << "\n";
        // cout << endl;
    }

    arquivoSaida.close();
}

int Utils::alocarFeriasMquinistas(int k, 
                                    int numMaquinistas,
                                    int porcentagemDestruicao,
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
    if(porcentagemDestruicao!=0) parada = (k-k*0.1*porcentagemDestruicao);

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

    // Salva os maquinistas que ainda precisam tirar ferias
    vector<int> maquinistasFaltantes;
    for (int i = 0; i < (int)diaFeriasMaquinista.size(); i++) 
    {
        if (diaFeriasMaquinista[i] > 0) maquinistasFaltantes.push_back(i);
    }
    aux = 2;

    // Logica para cada vez que a função ser chamado o algoritmo tenta mudar a ordem de preferentencia dos maquinistas
    static random_device rd; 
    static default_random_engine rng(rd());
    std::shuffle(maquinistasFaltantes.begin(), maquinistasFaltantes.end(), rng);

    int auxInicio = (k-k*0.1*porcentagemDestruicao);

    for(auto j: maquinistasFaltantes)
    {
        for(int i = auxInicio; i < k; i++)
        {
            if(diaFeriasMaquinista[j] == 0) break;

            int inicio = 0, fim = 0;

            // Verifica se o maquinista quer tirar ferias nesse dia, se não existe 2 maquinistas de ferias
            // Verifica também se o maquinista consegue tirar todas férias alocadas antes do horizonte de eventos terminar
            if(preferencias[j][i] == -10)
            {
                if(tarefasAlocadas[12][i] < 2 && alocacaoFeriasMaquinistas[j][i] == 0)
                {
                    // Conta quantos dias sobrando o maquinista ainda quer tirar ferias depois do dia i
                    int preferenciasFaltantes = 0;
                    aux = 2;

                    // Se o maquinistar tiver marcado um número de ferias maior ou do tamanho da escala menos 2 ja aloca as ferias
                    // Caso contrario realiza a logica abaixo adicionando mais 2 dias de ferias para o maquinista
                    if(diaFeriasMaquinista[j] < k-2)
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

                        // Verificar se em todo o periodo não existe 2 maquinistas de ferias, caso não exista adiciona os dias extras de ferias ao fim do periodo de ferias alocado
                        for(int x = i; x <= i+preferenciasPerdidas[j]+preferenciasFaltantes; x++)
                        {
                            if(tarefasAlocadas[12][x]+1 > 2)
                            {
                                aux = 22; // número ilustrativo apenas para mostrar erro
                                break;
                            }
                        }
                        if(aux == 22)
                        {
                            preferenciasPerdidas[j]++;
                            continue;
                        }

                        if(alocacaoFeriasMaquinistas[j][i-2] == 0 || i-2 < 0)
                        {
                            // Caso o maquinista queira tirar ferias no primeiro dia da escala, aloca +3 dias de ferias para o maquinista
                            if(i == 0)
                            {
                                fim = 3;
                                aux = 0;
                            }
                            else if(tarefasAlocadas[12][i-1] < 2 && i-1 >= 0)// Tenta adicionar 2 dias de ferias antes do marcado
                            {
                                aux--;
                                inicio--;

                                // Apos adicionar o primeiro dia de ferias extra tenta adicionar o segundo dia de ferias extra
                                if(tarefasAlocadas[12][i-2] < 2 && i-2 >= 0)
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
    }

    // Caso algum maquinista não tenha tirado todas as férias alocadas resolve o problema
    for(int i = 0; i < numMaquinistas; i++)
    {
        if(find(maquinistasDispensados.begin(), maquinistasDispensados.end(), i) != maquinistasDispensados.end()) continue;
        if(diaFeriasMaquinista[i] <= 0) continue;
        
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
        }
    }

    // // ---------------------------------------------------------
    // for (const auto& maquinista : tarefasAlocadas[12]) {
    //     cout << maquinista << " ";
    // }
    // cout << endl<<endl;
    // // ---------------------------------------------------------

    for(int i = 0; i < numMaquinistas; i++)
    {
        if(diaFeriasMaquinista[i] > 0) return -1;
    }

    // ---------------------------------------------------------
    // for(int j = 0; j < numMaquinistas; j++)
    // {
    //     cout << endl << endl << "Maquinista " << j << ": " << endl;
    //     for(int i = 45; i < 60; i++)
    //     {
    //         cout << alocacaoFeriasMaquinistas[j][i] << " ";
    //     }
    // }
    // cout << endl << endl;
    // ---------------------------------------------------------

    return 1;
}

bool Utils::reconstroiParcialmenteSolucao(int k, 
                            int numTarefas, 
                            int numMaquinistas,
                            int porcentagemDestruicao,
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

    // Aloca a tarefa 11 em toda a escala para os maquinistas dispensados
    for(int i = 0; i < (int)maquinistasDispensados.size(); i++)
    {
        // cout << "Maquinista dispensado: " << maquinistasDispensados[i] << endl;
        diaFeriasMaquinista[maquinistasDispensados[i]] = 0; // Zera os dias de ferias para os maquinistas dispensados
        for(int j = 1; j < k; j++)
        {
            escala[maquinistasDispensados[i]].push_back(11);
        }
    }

    int ver = 0; // Variavel para verificar se a alocação gerada esta valida

    ver = Utils::alocarFeriasMquinistas(k, numMaquinistas, porcentagemDestruicao, preferencias, alocacaoFeriasMaquinistas, diaFeriasMaquinista, tarefasAlocadas, maquinistasDispensados, blocosFeriasMaquinista, escala);

    if(ver != 1)
    {
        return false;
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
                
                // Caso o maquinista tenha entrado para a tarefa 10 antes de trabalhar 6 dias seguidos passa os dias de trabalho dele para o proximo maquinista
                if (!salvarDiasTrabalhados.empty())
                {
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

    return true;
}