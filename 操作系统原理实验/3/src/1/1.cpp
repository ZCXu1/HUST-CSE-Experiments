#include<bits/stdc++.h>
using namespace std;
#define N 10240
int MyArray[N][2*N];

int main() {
    for(int i = 0;i < N;i++)
        for(int j = 0;j < 2*N;j++) {
            MyArray[i][j] = 0;
        }
    return 0;
}