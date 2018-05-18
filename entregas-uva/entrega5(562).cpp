#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>

using namespace std;

/*
EXPLICACI�N:
    En principio, por cada uno de los casos, el programa lee las siguientes N monedas y los guarda en un vector local llamado elems.
    A su vez, va acumulando la sumatoria de todas las monedas en una variable llamada maxM.
    Se inicializa la matriz con la cantidad de monedas del caso y la m�xima diferencia que podr�a haber en la repartici�n, es decir la sumatoria de las monedas (maxM).

    Luego, se llama a minimalDiff con 0 = la posici�n del vector, 0 = el monto que tiene el lado derecho y 0 = el monto que tiene el lado izquierdo.
    MinimalDiff, por cada una de las monedas en la bolsa, revisa si ya fue calculado con el elemento actual y diferencia. Si no es as�, decide d�rsela al lado izquierdo o d�rsela al lado derecho, llamando a si misma y actualizando nuevamente el monto correspondiente.
    En otro caso, ya fue calculado y se retorna 'solutions[pos][diff]'

    Una diferencia importante que tiene esta implementaci�n a comparaci�n de los ejercicios anteriores es que la matriz es accedida mediante la diferencia entre los dos n�meros.
    Entonces, en el caso en que ya se ha recorrido todo el vector se devuelve la diferencia actual 'abs(amount_right - amount_left)'.


 Time: 0.050
*/

int N, M, money, maxM;
vector<int> elems;
vector<vector<int>> solutions;

int minimalDiff(int pos, int amount_right, int amount_left) {
    int diff = abs(amount_right - amount_left);
    if(pos == elems.size()) { return diff;}

    int current = elems[pos];
    if(solutions[pos][diff] == -2) {
        int minimalCountRight = minimalDiff(pos + 1, amount_right + current, amount_left);

        int minimalCountLeft = minimalDiff(pos + 1, amount_right, amount_left + current);

        solutions[pos][diff] = min(minimalCountRight, minimalCountLeft);
    }
    return solutions[pos][diff];
}


int main() {
    cin >> N;

    while(N != 0){
        cin >> M;
        maxM = 0;
        while(M > 0) {
            cin >> money;
            maxM += money;
            elems.push_back(money);
            M--;
        }
        solutions = vector<vector<int>>(elems.size(), vector<int>(maxM, -2));
        cout << std::to_string(minimalDiff(0, 0, 0)) << endl;
        N--;
        elems.clear();
    }

    return 0;
}
