#include <iostream>
#include <cmath>
using namespace std;
int a[35] = {0};
int dp[30005] = {0};
int main(){
    int V,n;
    cin>>V>>n;
    for(int i = 1;i <= n;i++){
        cin>>a[i];
    }
    for(int i = 1;i <= n;i++){
        for(int j = V;j >= a[i];j--){
            dp[j] = max(dp[j],dp[j-a[i]]+a[i]);
        }
    }
    cout<<V - dp[V]<<endl;
    return 0;
}