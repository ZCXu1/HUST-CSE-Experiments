#include<iostream>
#include<algorithm>
#include<cstdio>
//定义dp[i][j][k](1<=i<=n,0<=j<=2,0<=k<=1)
//表示前i棵树所能达到的最大观赏价值和
//其中,j表示哪一种树(j==0,表示树高为10;j==1,表示树高为20,等等)
//k==0表示第i棵树比左右两棵树都低,否则相反;

//很容易得出以下思路：
	// dp[i][0][0]=max(dp[i-1][1][1],dp[i-1][2][1])+a[i];
    // dp[i][1][0]=dp[i-1][2][1]+b[i];
    // dp[i][1][1]=dp[i-1][0][0]+b[i];
    // dp[i][2][1]=max(dp[i-1][1][0],dp[i-1][0][0])+c[i];
//DP完成后,枚举最后一棵树的状态(它的高度,比左右高还是低)并取max
// 第i个位置的树与第i+1个位置的树相邻，特别地，第1个位置的树与第n个位置的树相邻。
// 因此，我们可以先枚举第一棵树的高度(0,1,2)，每一次都取max,但是枚举范围不同。
using namespace std;

int dp[100010][3][2];
int n;
int a[100010][3];
int main()
{
    cin>>n;
    for(int i=1;i<=n;i++)
    {
        cin>>a[i][0];
        cin>>a[i][1];
        cin>>a[i][2];
    }
    int ans=0;
    //枚举第一棵树的高度
    for(int j=0;j<3;j++)
    {
        for(int i=0;i<3;i++)
            for(int k=0;k<2;k++)
                //初始化前1棵树的dp
                dp[1][i][k]=0;
        //固定第一棵树种某一个高度
        dp[1][j][0]=a[1][j];
        dp[1][j][1]=a[1][j];

        for(int i=2;i<=n;i++)
        {
            dp[i][0][0]=max(dp[i-1][1][1],dp[i-1][2][1])+a[i][0];
            dp[i][1][0]=dp[i-1][2][1]+a[i][1];
            dp[i][1][1]=dp[i-1][0][0]+a[i][1];
            dp[i][2][1]=max(dp[i-1][1][0],dp[i-1][0][0])+a[i][2];
        }
        //最后一棵树高度小于第一棵树的情况
        for(int i=0;i<j;i++)
            ans=max(ans,dp[n][i][0]);
        //最后一棵树高度大于第一棵树的情况
        for(int i=2;i>j;i--)
            ans=max(ans,dp[n][i][1]);
    }
    printf("%d\n",ans);
    return 0;
}