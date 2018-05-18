// 10130 SuperSale

#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <set>
using namespace std;

/*
EXPLICACION:
    Ya que la consigna era "No realizar un knapsack por persona" y dado que la cantidad máxima de peso que podría llevar una persona es > 0 y <= 30 se decidio,
        por cada uno de los casos, calcular knapsack de los productos (almacenados en un vector de pares de peso, valor) 30 veces, desde peso = 1 hasta 30.
    Permitiendo calcular todas las posbiles soluciones.
    Como se tiene una matriz de soluciones donde se iran almacenando a medida se vayan calculando, nunca se calculara dos veces knapsack para un mismo i, weight.
    Permitiendonos acceder en tiempo constante al resutaldo.
    Una vez calculado todos los posibles valores, se lee cada una de las entradas correspondientes al peso que soporta la iesima persona
        y se accede en tiempo constante a su respectivo valor dentro de la matrix. Entonces, sumando cada una de estos accesos, el programa obtiene el resultado final, imprimiendolo por pantalla.

 Time: 0.030
*/

vector<pair<int,int>> products;
vector<vector<int>> solutions;
vector<int> family;

int T, N, P, W, G, MW;


int resolve(int i, int weight) {
    if(weight < 0 ) {return -1;}

    if(i == products.size()) {return 0;}

    auto currentProduct = products[i];

    if(solutions[i][weight] == -2) {
        int right = 0;
        int left = 0;

        int newCw = weight - currentProduct.second;
        if(newCw >= 0) {
            right = currentProduct.first + resolve(i +1, newCw);
        }

        left = resolve(i +1, weight);

        solutions[i][weight] = max(right, left);
    }

    return solutions[i][weight];
}

int main() {
    cin >> T;
    int total = 0;
    int maxW = 0;

    while(T != 0) {
        cin >> N;

        solutions = vector<vector<int>>(N, vector<int>(31, -2));

        while(N != 0) {
            cin >> P;
            cin >> W;
            products.push_back(std::make_pair(P, W));
            N--;
        }
        cin >> G;



        while(G != 0){
            cin >> MW;
            family.push_back(MW);
            maxW = max(maxW, MW);
            G--;
        }

        for(int i = 1; i <= maxW; i++){
            resolve(0, i);
        }
        for(int e = 0; e < family.size(); e++){
            total += solutions[0][family[e]];
        }

        cout << std::to_string(total) << endl;
        products.clear();
        family.clear();
        solutions.clear();
        total = 0;
        maxW = 0;
        T--;
    }
    return 0;
}

