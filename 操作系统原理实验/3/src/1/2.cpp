#include<bits/stdc++.h>
using namespace std;
#define N 10240
int MyArray[2*N][N];
int main() {
    for(int i = 0;i < N;i++)
        for(int j = 0;j < 2*N;j++) {
            MyArray[j][i] = 0;
        }      
    return 0;
}