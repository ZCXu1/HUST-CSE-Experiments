#include<iostream>
#include<cstdio>
#include<algorithm>
#include<cstring>
#define MAXN 105
using namespace std;
struct BigInt{
    int a[100],MOD=10000,len;
    BigInt(){
        memset(a,0,sizeof(a));len=1;
    }
    BigInt(int b){
        memset(a,0,sizeof(a));len=0;
        while(b){
            a[len++]=b%MOD;
            b/=MOD;
        }
    }
    BigInt(const BigInt &b){
        memset(a,0,sizeof(a));
        len=b.len;
        for(int i=0;i<len;i++)a[i]=b.a[i];
    }
    BigInt operator=(const BigInt &b){
        memset(a,0,sizeof(a));
        len=b.len;
        for(int i=0;i<len;i++)a[i]=b.a[i];
        return *this;
    }
    BigInt operator+(const BigInt &b){
        BigInt r;r.len=max(len,b.len);int up=0;
        for(int i=0;i<r.len;i++){
            int tmp=(i<len?a[i]:0)+(i<b.len?b.a[i]:0)+up;
            r.a[i]=tmp%MOD;up=tmp/MOD;
        }
        if(up)r.a[r.len++]=up;
        return r;
    }
    BigInt operator-(const BigInt &b){
        BigInt r;r.len=max(len,b.len);int down=0;
        bool flag=false;BigInt t1,t2;
        if(*this<b)flag=true,t1=b,t2=*this;
        else t1=*this,t2=b;
        for(int i=0;i<t1.len;i++){
            int tmp=t1.a[i]-(i<t2.len?t2.a[i]:0)-down;
            r.a[i]=(tmp%MOD+MOD)%MOD;
            if(tmp<0)down=1;else down=0;
        }
        while(!r.a[r.len-1]&&r.len>1)r.len--;
        if(flag)r.a[r.len-1]=-r.a[r.len-1];
        return r;
    }
    bool operator<(const BigInt &b){
        if(len<b.len)return true;
        else if(len>b.len)return false;
        else{
            int i=len-1;
            while(i>=0){
                if(a[i]!=b.a[i])
                    return a[i]<b.a[i];
                i--;
            }
            return false;
        }
    }
    void output(){
        printf("%d",a[len-1]);
        for(int i=len-2;i>=0;i--)
            printf("%04d",a[i]);
        printf("\n");
    }
};
int n,k;
BigInt sum,dp[MAXN][2];
BigInt f(int k){
    dp[0][0]=0;dp[0][1]=1;
    for(int i=1;i<=n;i++){
        sum=dp[i-1][0]+dp[i-1][1];
        dp[i][1]=sum;
        if(i<=k)dp[i][0]=sum;
        else if(i==k+1)dp[i][0]=sum-1;
        else dp[i][0]=sum-dp[i-k-1][1];
    }
    return dp[n][0]+dp[n][1];
}
int main(){
    while(scanf("%d%d",&n,&k)!=EOF){
        BigInt res=f(n)-f(k-1);
        res.output();
    }
    return 0;
}

