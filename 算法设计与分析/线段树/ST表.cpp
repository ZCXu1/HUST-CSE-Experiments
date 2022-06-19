/**
给定一个长度为 N 的数列，和 M 次询问，求出每一次询问的区间内数字的最大值。
输入格式
第一行包含两个整数 N,M，分别表示数列的长度和询问的个数。
第二行包含 N 个整数（记为 a_i），依次表示数列的第 i 项。
接下来 MM 行，每行包含两个整数 l_i,r_i，表示查询的区间为 [l_i,r_i]
ST表的功能很简单
它是解决RMQ问题(区间最值问题)的一种强有力的工具
它可以做到O(nlogn)预处理，O(1)查询最值
*/
#include<cstdio>
#include<cmath>
#include<algorithm>
typedef long long ll;
using namespace std;
const ll MAXN=1e6+10;
inline ll read()
{
    char c=getchar();ll x=0,f=1;
    while(c<'0'||c>'9'){if(c=='-')f=-1;c=getchar();}
    while(c>='0'&&c<='9'){x=x*10+c-'0';c=getchar();}
    return x*f;
}
ll Max[MAXN][21];
ll Query(ll l,ll r)
{
    ll k=log2(r-l+1); 
    return max(Max[l][k],Max[r-(1<<k)+1][k]);//把拆出来的区间分别取最值 
}
int main()
{
    ll N=read(),M=read();
    for(ll i=1;i<=N;i++) Max[i][0]=read();
    for(ll j=1;j<=21;j++)
        for(ll i=1;i+(1<<j)-1<=N;i++)//注意这里要控制边界 
            Max[i][j]=max(Max[i][j-1],Max[i+(1<<(j-1))][j-1]);
    for(ll i=1;i<=M;i++)
    {
        ll l=read(),r=read();
        printf("%d\n",Query(l,r));
    }
    return 0;
}