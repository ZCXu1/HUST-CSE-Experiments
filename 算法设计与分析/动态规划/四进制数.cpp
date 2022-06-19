#include <iostream>
using namespace std;

#define maxn 1005
#define MOD 999999997
//https://blog.csdn.net/qq_45228537/article/details/100941253
long long dp[maxn][6];
using namespace std;
int main()
{
    int n;
    cin>>n;

    for(int i=1;i<=n;i++)
    {
        int j=i-1;
        dp[i][0]=1;
        dp[i][1]=(dp[j][0]+dp[j][1]*2)%MOD;
        dp[i][2]=(dp[j][0]+dp[j][2])%MOD;
        dp[i][3]=(dp[j][1]+dp[j][3]*2)%MOD;
        dp[i][4]=(dp[j][1]+dp[j][2]+dp[j][4]*2)%MOD;
        dp[i][5]=(dp[j][3]+dp[j][4]+dp[j][5]*2)%MOD;
    }

    cout<<dp[n][5];
    return 0;
}