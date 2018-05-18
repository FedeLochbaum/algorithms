#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <set>
using namespace std;

int C, P, Q, current_p;
vector<int> binary;

void setBinary() {
    current_p = P;
    binary.push_back(1);
    for(int i = 1; i < (P+Q)-1; i++) {
        if(binary.at(i -1) == 1) {
            binary.push_back(0);
        } else if(binary.at(i -1) == 0 || (binary.at(i -1) == 1 && binary.at(i -2) == 0)){
            binary.push_back(1);
        }
    }
}

int toNumber(){
    int number = 0;
    for(int i = 0; i < binary.size(); i++){
        if(binary.at(i) == 1) {
            number += pow(2,i);
        }
    }
    return number;
}


int main() {
    cin >> C;

    while(C != 0) {
        cin >> P;
        cin >> Q;


        setBinary();

        for(i = 0; i < binary.size(); i++) {
            cout << i << "";
        }

        cout << toNumber() << endl;
        binary.clear();
        C--;
    }

    return 0;
}
