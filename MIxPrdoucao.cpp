// Problema de Mix de Producao — CPLEX via Concert Technology (C++)
// Mesma marcenaria do exemplo em Python:
//   Mesa:    lucro R$ 70, usa 4h de carpintaria e 2h de acabamento
//   Cadeira: lucro R$ 50, usa 3h de carpintaria e 1h de acabamento
//   Disponibilidade: 240h de carpintaria, 100h de acabamento
//
// Compilar (linha unica):
//   g++ -std=c++17 mix_producao.cpp -o mix_producao \
//     -I/opt/ibm/ILOG/CPLEX_Studio222/cplex/include \
//     -I/opt/ibm/ILOG/CPLEX_Studio222/concert/include \
//     -L/opt/ibm/ILOG/CPLEX_Studio222/cplex/lib/x86-64_linux/static_pic \
//     -L/opt/ibm/ILOG/CPLEX_Studio222/concert/lib/x86-64_linux/static_pic \
//     -lilocplex -lconcert -lcplex -lm -lpthread -ldl -DIL_STD

#include <ilcplex/ilocplex.h>
#include <iostream>

ILOSTLBEGIN  // macro do Concert para compatibilidade com std::

int main() {
    IloEnv env;  // ambiente: gerencia toda a memoria do Concert
    try {
        IloModel model(env, "mix_producao");

        // Variaveis de decisao: continuas, de 0 a +infinito
        IloNumVar x1(env, 0.0, IloInfinity, ILOFLOAT, "mesas");
        IloNumVar x2(env, 0.0, IloInfinity, ILOFLOAT, "cadeiras");

        // Funcao objetivo: max 70*x1 + 50*x2
        model.add(IloMaximize(env, 70 * x1 + 50 * x2));

        // Restricoes
        IloRange r_carp = (4 * x1 + 3 * x2 <= 240);
        r_carp.setName("carpintaria");
        IloRange r_acab = (2 * x1 + 1 * x2 <= 100);
        r_acab.setName("acabamento");
        model.add(r_carp);
        model.add(r_acab);

        // Resolver
        IloCplex cplex(model);
        // cplex.setOut(env.getNullStream());  // descomente p/ silenciar o log

        if (cplex.solve()) {
            cout << "\n===== Solucao otima =====" << endl;
            cout << "Status        : " << cplex.getStatus() << endl;
            cout << "Mesas    (x1) = " << cplex.getValue(x1) << endl;
            cout << "Cadeiras (x2) = " << cplex.getValue(x2) << endl;
            cout << "Lucro maximo  = R$ " << cplex.getObjValue() << endl;

            cout << "\n===== Precos-sombra =====" << endl;
            cout << "Carpintaria: R$ " << cplex.getDual(r_carp)
                 << " por hora extra" << endl;
            cout << "Acabamento : R$ " << cplex.getDual(r_acab)
                 << " por hora extra" << endl;

            cout << "\n===== Folgas =====" << endl;
            cout << "Carpintaria: " << cplex.getSlack(r_carp) << " h" << endl;
            cout << "Acabamento : " << cplex.getSlack(r_acab) << " h" << endl;
        } else {
            cout << "Nenhuma solucao encontrada. Status: "
                 << cplex.getStatus() << endl;
        }
    } catch (IloException& e) {
        cerr << "Erro do Concert/CPLEX: " << e << endl;
    }
    env.end();  // libera toda a memoria do ambiente
    return 0;
}