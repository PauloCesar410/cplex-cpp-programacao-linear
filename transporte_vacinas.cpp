// Problema do Transporte — Distribuicao de Vacinas no RN (Concert C++)
// Mesmo problema da versao em Python (docplex), agora com arrays e lacos.
//
// Ofertas:  Mossoro = 800 doses | Natal = 1000 doses
// Demandas: Pau dos Ferros = 500 | Caico = 600 | Joao Camara = 700
// Custos (R$/dose):
//              PFerros  Caico  JCamara
//   Mossoro       4       6       9
//   Natal         8       5       3
//
// Compilar: Ctrl+Shift+B (tarefa "Compilar com CPLEX")
// Executar: Ctrl+F5 ou ./transporte_vacinas

#include <ilcplex/ilocplex.h>
#include <iostream>
#include <string>
#include <vector>

ILOSTLBEGIN

int main() {
    IloEnv env;
    try {
        // ---------- Dados ----------
        const vector<string> origens  = {"Mossoro", "Natal"};
        const vector<string> destinos = {"PauDosFerros", "Caico", "JoaoCamara"};
        const int m = origens.size();   // numero de origens
        const int n = destinos.size();  // numero de destinos

        const vector<double> oferta  = {800, 1000};
        const vector<double> demanda = {500, 600, 700};

        // Matriz de custos c[i][j]
        const vector<vector<double>> custo = {
            {4, 6, 9},   // Mossoro
            {8, 5, 3}    // Natal
        };

        // ---------- Modelo ----------
        IloModel model(env, "transporte_vacinas");

        // x[i][j] = doses enviadas da origem i ao destino j
        // IloArray de IloNumVarArray = matriz de variaveis
        IloArray<IloNumVarArray> x(env, m);
        for (int i = 0; i < m; i++) {
            x[i] = IloNumVarArray(env, n, 0.0, IloInfinity, ILOFLOAT);
            for (int j = 0; j < n; j++) {
                string nome = "x_" + origens[i] + "_" + destinos[j];
                x[i][j].setName(nome.c_str());
            }
        }

        // Objetivo: min soma c[i][j] * x[i][j]
        // IloExpr acumula a expressao termo a termo
        IloExpr custoTotal(env);
        for (int i = 0; i < m; i++)
            for (int j = 0; j < n; j++)
                custoTotal += custo[i][j] * x[i][j];
        model.add(IloMinimize(env, custoTotal));
        custoTotal.end();  // IloExpr deve ser liberada apos o uso

        // Restricoes de oferta: soma_j x[i][j] <= oferta[i]
        for (int i = 0; i < m; i++) {
            IloExpr enviado(env);
            for (int j = 0; j < n; j++)
                enviado += x[i][j];
            model.add(enviado <= oferta[i]);
            enviado.end();
        }

        // Restricoes de demanda: soma_i x[i][j] == demanda[j]
        for (int j = 0; j < n; j++) {
            IloExpr recebido(env);
            for (int i = 0; i < m; i++)
                recebido += x[i][j];
            model.add(recebido == demanda[j]);
            recebido.end();
        }

        // ---------- Resolucao ----------
        IloCplex cplex(model);

        if (cplex.solve()) {
            cout << "\n===== Plano otimo de distribuicao =====" << endl;
            for (int i = 0; i < m; i++) {
                for (int j = 0; j < n; j++) {
                    double v = cplex.getValue(x[i][j]);
                    if (v > 1e-6) {
                        cout << origens[i] << " -> " << destinos[j]
                             << ": " << v << " doses"
                             << " (custo R$ " << custo[i][j] * v << ")"
                             << endl;
                    }
                }
            }
            cout << "\nCusto total minimo: R$ "
                 << cplex.getObjValue() << endl;

            cout << "\n===== Utilizacao dos centros =====" << endl;
            for (int i = 0; i < m; i++) {
                double enviado = 0;
                for (int j = 0; j < n; j++)
                    enviado += cplex.getValue(x[i][j]);
                cout << origens[i] << ": " << enviado << " de "
                     << oferta[i] << " doses ("
                     << 100.0 * enviado / oferta[i] << "%)" << endl;
            }
        } else {
            cout << "Nenhuma solucao encontrada. Status: "
                 << cplex.getStatus() << endl;
        }
    } catch (IloException& e) {
        cerr << "Erro do Concert/CPLEX: " << e << endl;
    }
    env.end();
    return 0;
}