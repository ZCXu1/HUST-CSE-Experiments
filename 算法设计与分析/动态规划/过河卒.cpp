#include<iostream>
using namespace std;
long long a,b,n,m,x[23][23],ma[23][23];
void work(long long x,long long y){
    ma[x][y]=1;
    ma[x-1][y-2]=1;
    ma[x-2][y-1]=1;
    ma[x-2][y+1]=1;
    ma[x-1][y+2]=1;
    ma[x+1][y-2]=1;
    ma[x+2][y-1]=1;
    ma[x+2][y+1]=1;
    ma[x+1][y+2]=1;
}
int main(){
    scanf("%lld %lld %lld %lld",&n,&m,&a,&b);
    a++;
    b++;
    n++;
    m++;
    work(a,b);
    x[1][1]=1;
    for(int i=1;i<=n;i++){
        for(int j=1;j<=m;j++){
        	if(i==1&&j==1)continue;
        	if(ma[i][j]==0)x[i][j]=x[i-1][j]+x[i][j-1];
        }
    }
    printf("%lld",x[n][m]);
    return 0;
}