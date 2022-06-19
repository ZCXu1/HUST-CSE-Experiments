#include<cmath>
#include<cstdio>
#include<algorithm>
using namespace std;
const int N=10000005,M=10000005;

int n,m,Ans,A[M],B[M];
bool flag,colored[N<<2];

int read()
{
    int now=0;char c=getchar();
    while(c<'0'||c>'9')c=getchar();
    while(c>='0'&&c<='9')now=(now<<3)+(now<<1)+c-'0',c=getchar();
    return now;
}

void PushUp(int rt)
{
    colored[rt]= colored[rt<<1]&&colored[rt<<1|1];
}

/*void Build(int l,int r,int rt)
{
    if(l==r)
      return;
    int m=(l+r)>>1;
    Build(l,m,rt<<1);
    Build(m+1,r,rt<<1|1);
    PushUp(rt);
}*/

void Modify(int l,int r,int rt,int L,int R)
{
    if(colored[rt]) return;
    if(L<=l && r<=R)
    {
        flag=1;colored[rt]=1;
        return;
    }
    int m=(l+r)>>1;
    if(L<=m) Modify(l,m,rt<<1,L,R);
    if(m<R) Modify(m+1,r,rt<<1|1,L,R);
    PushUp(rt);
}

int main()
{

    n=read();m=read();
//    Build(1,n,1);
    for(int i=1;i<=m;i++)
      A[i]=read(),B[i]=read();
    for(int i=m;i>=1;i--)
    {
        flag=0;
        Modify(1,n,1,A[i],B[i]);
        if(flag) ++Ans;
    }
    printf("%d",Ans);
  
    return 0;
}