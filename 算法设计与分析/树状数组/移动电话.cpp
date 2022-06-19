
#include <stdio.h>
#include <string.h>
 
#define rep(i,l,r) for(int i=(l);i<=(r);++i)
#define rpe(i,r,l) for(int i=(r);i>=(l);--i)
 
typedef long long ll;
 
inline ll max(ll x,ll y){return x>y?x:y;}
 
inline ll read(){
    ll x=0,f=1;char ch=' ';
    while(ch<'0' || ch>'9'){if(ch=='-')f=-1;ch=getchar();}
    while(ch>='0' && ch<='9'){x=x*10+(ch^48);ch=getchar();}
    return f==1?x:-x;
}

#define MAX_N 5000

int tree[MAX_N][MAX_N], n;

// 单点修改 & 区间查询
void add(int x, int y, int z){ //将点(x, y)加上z
    int memo_y = y;
    while(x <= n){
        y = memo_y;
        while(y <= n)
            tree[x][y] += z, y += y & -y;
        x += x & -x;
    }
}
ll ask(int x, int y){//求左上角为(1,1)右下角为(x,y) 的矩阵和
    int res = 0, memo_y = y;
    while(x){
        y = memo_y;
        while(y)
            res += tree[x][y], y -= y & -y;
        x -= x & -x;
    }
    return res;
}

int main(){
    ll x,y,a,l,b,r,t,opt;
    bool flag = false;
    while (1)
    {
        opt = read();
        switch (opt)
        {
        case 0:{
            n = read();
            break;
        }
        case 1:{
            x = read();
            x++;
            y = read();
            y++;
            a = read();
            add(x,y,a);
            break;
        }
        case 2:{
            l = read();l++;
            b = read();b++;
            r = read();r++;
            t = read();t++;
            printf("%lld\n",ask(r,t)-ask(l-1,t)-ask(r,b-1)+ask(l-1,b-1));
            break;
        }  
        default:
            flag = true;
            break;
        }
        if(flag){
            break;
        }
    }
    
}