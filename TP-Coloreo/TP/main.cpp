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
vi frequency;
mi cost_matrix;


graph grasp() {
    graph global_solution = greedy_solution();

    while(!stop_criteria()) {
        graph current_solution = local_search(greedy_solution());
        if(functional(current_solution) < functional(global_solution)) {
            global_solution = current_solution;
        }
    }
    return global_solution;
}

void show_solution(graph graph) {

}



int main() {

    cin >> N;
    cin >> T;
    cin >> M;

    frequency = vi(T);
    cost_matrix = vector<vi>(N, vector<int>(N));


    for(auto i = 0; i < T; i++) {
        cin >> current;
        frequency.push_back(current);
    }

    for(auto e = 0; e < M; e++) {
        cin >> i;
        cin >> j;
        cin >> cost;

        i--; // Uso indices desde 0 hasta N-1
        j--;

        cost_matrix[i][j] = cost;
        cost_matrix[j][i] = cost;
    }

    show_solution(grasp());

    return 0;
}