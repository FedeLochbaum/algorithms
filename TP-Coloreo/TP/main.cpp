#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

typedef pair<int,int> pii;
typedef vector<int> vi;
typedef vector<vi> mi;
typedef vector< pii > vii;
typedef vector<vi> graph;

int T;  // numero de frecuecncias
int N; // numero de antenas disponibles
int M; // numero de conflictos posibbles por asignar con la misma frecuencia

vi f;  // indica el costo de usar la frecuencia i
mi cost_conflict_matrix; // costo de colisiones de frecuencias para el par de antenas i, k

struct Solution {
    vi y; // 0 o 1 si la frecuencia i esta usada
    mi x; // 0 o 1 en k, i si la frecuencia k es usada por la antena i. [Por ahora no lo uso]
    mi z; // 0 o 1 si para la antena i, j se asigna la misma frecuecia
    vi assings; // assings[i] indica la frecuencia asignada al vertice i.

    graph graph;
    Solution();
    int functional();
    int loss_for_collision();
    int cost_of_frequencies();
    void show_solution();
};

Solution::Solution() {}

int Solution::loss_for_collision(){
    int res = 0;
    for(int i = 0; i < cost_conflict_matrix.size(); i ++) {
        for(int k = 0; k < cost_conflict_matrix[i].size(); k++) {
            if(assings[i] == assings[k]) {
                res += cost_conflict_matrix[i][k];
            }
        }
    }
    return res;
}

int Solution::cost_of_frequencies() {
    int res = 0;
    for(int i = 0; i < T; i++) {
        if(y[i] == 1) {
            res += f[i];
        }
    }

    return res;
}

int Solution::functional() {
    return cost_of_frequencies() + loss_for_collision();
}

void Solution::show_solution() { // por ahora no escribe en files
    for(auto i = 0; i < N; i++) {
        cout << assings[i] << endl;
    }
}


Solution grasp() {
    Solution global_solution = greedy_solution();

    while(!stop_criteria()) {
        Solution current_solution = local_search(greedy_solution());
        if(current_solution.functional() < global_solution.functional()) {
            global_solution = current_solution;
        }
    }
    return global_solution;
}



int main() {
    int current, i, j, cost;

    cin >> N, T, M;

    f = vi(T);
    cost_conflict_matrix = vector<vi>(N, vector<int>(N));


    for(auto i = 0; i < T; i++) {
        cin >> current;
        f.push_back(current);
    }

    //ordenar costo de frecuencia de menor a mayor, pero mantener el indice de la frecuencia

    for(auto e = 0; e < M; e++) {
        cin >> i, j, cost;

        i--; // Uso indices desde 0 hasta N-1
        j--;

        cost_conflict_matrix[i][j] = cost;
        cost_conflict_matrix[j][i] = cost;
    }

    grasp().show_solution();

    return 0;
}