#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <set>
using namespace std;

/*
EXPLICACION:
    En principio, por cada uno de los casos, el programa lee los siguientes N números y los guarda en un vector local llamado elems.
    A su vez, chequea que sea posible calcular el numero T deseado, es decir, revisando si la suma de los N números es mayor o igual a T.
    Si es posible calcular la solución se llama a resolve, sino devuelve "NONE".

    Resolve es una procedimiento recursivo que imprime por pantalla el resultado.
    Si se recorrió cada numero de elems, no se consiguió tomar ninguno y además, no se encontró previamente una solución, imprime NONE.
    Si el valor V (estado actual del T previo) es igual a 0 quiere decir que ah encontrado una posible solución, por lo tanto la imprime y lo guarda en un vector de soluciones.
    En otro caso, si la lista posee elementos, se obtiene el elemento actual, se realiza el chequeo del primer valor agregado (ya que el estado parcial siempre el string resultado).
        además, revisa si vale la pena optar por esa rama, es decir, si la resta del V y el valor actual no es negativa, ya que si lo fuera no seria una rama valida.
    Si es una rama valida se llama recursivamente a resolve actualizando todo el estado ("lo tomo"). Finalmente, también se llama a resolve sin haber actualizo el estado ("no lo tomo").

  Time: 0.000
*/


std::set<std::string> solutions;
int T, N, num, S;
bool print = false;

void resolve(int V, std::vector<int> elems,  string partial){
    if (elems.empty() && partial == "" && !print){
        cout << "NONE" << endl;
    }

    if (V == 0) {
        std::set<std::string>::iterator it = solutions.find(partial);
        if(it == solutions.end()){
            solutions.insert(partial);
            cout << partial << endl;
            print = true;
        }
        return;
    }

    if(elems.size() > 0){
        string newPartial;
        int current = elems.back();
        elems.pop_back();

        if(partial.empty()) {
            newPartial = std::to_string(current);
        } else newPartial = partial + "+" + std::to_string(current);

        if(V - current >= 0)
            resolve(V - current, elems, newPartial);

        resolve(V, elems, partial);
    }
}

int main() {
    cin >> T;
    cin >> N;
    vector<int> elems;
    string partial = "";

    while(N != 0) {
        S = 0;
        for(int i = N; i > 0; i--){
            cin >> num;
            S += num;
            elems.push_back(num);
        }
        std::reverse(elems.begin(), elems.end());
        cout << "Sums of " << T << ":" << endl;

        if(S >= T)
            resolve(T, elems, partial);
        else { cout << "NONE" << endl; }

        cin >> T;
        cin >> N;
        elems.clear();
        solutions.clear();
        print = false;
    }

    return 0;
}
