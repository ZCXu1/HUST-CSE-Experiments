#include <stdio.h>
#include <string.h>

#define loop(i,l,r) for(int i=(l);i<(r);++i)

typedef long long ll;

inline ll max(ll x,ll y){return x>y?x:y;}

inline ll read(){
    ll x=0,f=1;char ch=' ';
    while(ch<'0' || ch>'9'){if(ch=='-')f=-1;ch=getchar();}
    while(ch>='0' && ch<='9'){x=x*10+(ch^48);ch=getchar();}
    return f==1?x:-x;
}

// dp[i][j][0/1] 表示前i个中高度为j作为第一个的种数，(0表矮 1表高) j<=i
// basecase:
// dp[1][1][0] = 1
// dp[1][1][1] = 1
// dp[i][j][0] = sum(dp[i-1][k][1]) k>j 
// dp[i][j][1] = sum(dp[i-1][k][0]) k<j

ll dp[25][25][2] = {0};

int main(){
    int n;
    scanf("%d",&n);

    dp[1][1][0] = dp[1][1][1] = 1;

    loop(i, 2, 21){
        loop(j, 1, i+1){
            loop(k, 1, j){
                dp[i][j][1] += dp[i-1][k][0];
            }
            loop(k, j, i+1){
                dp[i][j][0] += dp[i-1][k][1];
            }
        }
    }

    loop(i, 1, n+1){
        int num, p;
        num = read(), p = read();
        ll sum = 0;

        if(p == 1) {
            printf("%d 1\n", i);
            continue;
        }

        loop(i, 1, p+1){
            sum += dp[p][i][0] + dp[p][i][1];
        }

        printf("%d %lld\n", i, sum);
    }


    return 0;
}