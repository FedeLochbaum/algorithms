#include <vector>
#include <iostream>
#include <algorithm>
#include <tuple>

using namespace std;

int N, MWSup, W;
vector<pair<int,int>> elems;
vector<vector<int>> solutions;

int maxBoxes(int pos, int weight) {

    if(weight < 0) {return -10;}

    if(pos == elems.size()) {return 0;}

    if(solutions[pos][weight] == -2) {
        auto current = elems[pos];

        int newW = min(weight - current.first, current.second);

        int right = 1 + maxBoxes(pos+1, newW);

        int left = maxBoxes(pos+1, weight);

        solutions[pos][weight] = max(right, left);
    }

    return solutions[pos][weight];
}


int main() {
    cin >> N;

    while(N != 0){
        while(N > 0) {
            cin >> W;
            cin >> MWSup;
            elems.push_back(std::make_pair(W, MWSup));
            N--;
        }

        solutions = vector<vector<int>>(elems.size(), vector<int>(6001, -2));
        cout << std::to_string(maxBoxes(0, 3000)) << endl;
        cin >> N;
        elems.clear();
        solutions.clear();
    }

    return 0;
}
