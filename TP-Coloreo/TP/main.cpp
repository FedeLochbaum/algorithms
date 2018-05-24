#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

typedef pair<int,int> pii;
typedef vector<int> vi;
typedef vector<vi> mi;
typedef vector< pii > vii;
typedef vector<vii> graph;

int N, T, M, current, i, j, cost;
vi f; // indica el costo de usar la frecuencia i
mi cost_conflict_matrix; // costo de colisiones de frecuencias para el par de antenas i, k

struct Solution {
    vi y; // 0 o 1 si la frecuencia i esta usada
    mi x; // 0 o 1 en k, i si la frecuencia k es usada por la antena i
    mi z; // 0 o 1 si para la antena i, j se asigna la misma frecuecia
    Solution();
    int functional();
    int loss_for_collision();
    int cost_of_frequencies();
    void show_solution();
};

Solution::Solution() {
    // constructor
}

int Solution::functional() {
    return cost_of_frequencies() + loss_for_collision();
}

int Solution::loss_for_collision(){
    return 0;
}

int Solution::cost_of_frequencies() {
    return 0;
}

void Solution::show_solution() {

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
    cin >> N; // cantidad de antenas
    cin >> T; // cantidad de frecuencias
    cin >> M; // cantidad de conflictos

    f = vi(T);
    cost_conflict_matrix = vector<vi>(N, vector<int>(N));


    for(auto i = 0; i < T; i++) {
        cin >> current;
        f.push_back(current);
    }

    //ordenar costo de frecuencia de menor a mayor

    for(auto e = 0; e < M; e++) {
        cin >> i;
        cin >> j;
        cin >> cost;

        i--; // Uso indices desde 0 hasta N-1
        j--;

        cost_conflict_matrix[i][j] = cost;
        cost_conflict_matrix[j][i] = cost;
    }

    grasp().show_solution();

    return 0;
}