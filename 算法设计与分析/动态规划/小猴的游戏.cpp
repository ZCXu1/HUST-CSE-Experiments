#include<iostream>
#include <cmath>
using namespace std;

typedef long long ll;
inline ll max(ll x,ll y){return x>y?x:y;}
inline ll min(ll x,ll y){return x<y?x:y;}
inline ll read(){
    ll x=0,f=1;char ch=' ';
    while(ch<'0' || ch>'9'){if(ch=='-')f=-1;ch=getchar();}
    while(ch>='0' && ch<='9'){x=x*10+(ch^48);ch=getchar();}
    return f==1?x:-x;
}
ll  s[100005]={0};
int dp[100005]={0};
ll  A_max[100005]={0};
ll  A_min[100005]={0};
int main()
{
    int n,m;
    n = read();m = read();
    for( int i=0;i<n;i++)
    {
       s[i] = read();
    }
    dp[1]=1;
    int maxs=0;
    for(  int i=2;i<=n;i++)
    {
        dp[i]=1;
        for(int j=1;j<i; j++)
        {   
            if((s[j-1]-s[i-1]) >=m||(s[j-1]-s[i-1]) <= -m ) 
            dp[i]=max(dp[i],dp[j]+1);

        }
        maxs=max(maxs,dp[i]);
    }
    cout<<maxs;
}