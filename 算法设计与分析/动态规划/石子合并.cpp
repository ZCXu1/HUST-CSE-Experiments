#include<iostream>  
#include<cstdio>  
#include<cmath>  
using namespace std;   
int n,maxl,f[4005][4005],num[4005];  
int s[4005];  
inline int d(int i,int j){return s[j]-s[i-1];}  
//O(n^3)的转移方程：f[i][j] = max(f[i][k]+f[k+1][j]+d[i][j];
int main()  
{   
    scanf("%d",&n);  
    for(int i=1;i<=n+n;i++)
    {  
        /**
     * 输入只到n 后面后续处理n~2n的数
     */    
        scanf("%d",&num[i]);  
        num[i+n]=num[i];  
        s[i]=s[i-1]+num[i];  
    }  
    // 枚举区间长度
    // 区间长度从1到n-1枚举 不能枚举i从1到2n j从1到2n 要按长度从1到n-1
    // 因为如果按前者枚举的话 有的前面的可能没dp到
    // 比如dp[1][3]要用到dp[2][3] 但是dp[2][3]还没有计算出 所以要按i和j之前的长度枚举
    for(int p=1;p<n;p++)  
    {  
        for(int i=1,j=i+p;(j<n+n) && (i<n+n);i++,j=i+p)  
        {  
            //超时
            // for(int k=i;k<j;k++)  
            // {  
            //     f[i][j] = max(f[i][j], f[i][k]+f[k+1][j]+d(i,j));   
            // }  
            f[i][j] = max(f[i+1][j],f[i][j-1])+d(i,j);
        }  
    }  
    for(int i=1;i<=n;i++)  
    {  
        maxl=max(maxl,f[i][i+n-1]);  
    }  
    printf("%d\n",maxl);  
    return 0;  
}