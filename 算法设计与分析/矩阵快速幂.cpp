#include <iostream>
#include <cstring>

#define Max_rank 3
#define mod 1000000007
struct Matrix {
    long long a[Max_rank][Max_rank];

    Matrix() {
        memset(a, 0, sizeof(a));
    }

    void init(){
        a[1][1] = a[1][2] = a[2][1] = 1;
        a[2][2] = 0;
    }

    Matrix operator*(const Matrix b) {
        Matrix res;
        for (int i = 1; i <= 2; i++)
            for (int j = 1; j <= 2; j++)
                for (int u = 1; u <= 2; u++)
                    res.a[i][j] = (res.a[i][j] + a[i][u]*b.a[u][j])%mod;
        return res;
    }
};

long long q_pow(long long n){
    Matrix ans,base;
    ans.init();
    base.init();
    while(n > 0){
        if(n&1) ans =ans *base;
        base = base *base;
        n >>= 1;
    }
    return ans.a[1][1];
}
int main() {
    long long n;
    while(std::cin >> n){
        std::cout << q_pow(n-2) << std::endl;
    }
    return 0;
}