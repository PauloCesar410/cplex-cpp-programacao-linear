# Programação Linear com CPLEX em C++ (Concert Technology)

Modelos de PL resolvidos com IBM ILOG CPLEX via API C++.

- `MIxPrdoucao.cpp` — mix de produção (mesas × cadeiras)

## Compilação (Linux)

Ajuste `CPLEXDIR` para o caminho da sua instalação:

```bash
CPLEXDIR=/opt/ibm/ILOG/CPLEX_Studio2211
g++ -std=c++17 MIxPrdoucao.cpp -o mix_producao \
  -I$CPLEXDIR/cplex/include -I$CPLEXDIR/concert/include \
  -L$CPLEXDIR/cplex/lib/x86-64_linux/static_pic \
  -L$CPLEXDIR/concert/lib/x86-64_linux/static_pic \
  -lilocplex -lconcert -lcplex -lm -lpthread -ldl \
  -DIL_STD
```

## Execução

```bash
./mix_producao
```
