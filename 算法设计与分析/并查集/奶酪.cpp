#include<iostream>
#include<cmath>
#define rep(i,l,r) for(ll i=(l);i<=(r);++i)
#define rpe(i,r,l) for(ll i=(r);i>=(l);--i)
typedef long long ll;
using namespace std;
inline ll max(ll x,ll y){return x>y?x:y;}
inline ll read(){
    ll x=0,f=1;char ch=' ';
    while(ch<'0' || ch>'9'){if(ch=='-')f=-1;ch=getchar();}
    while(ch>='0' && ch<='9'){x=x*10+(ch^48);ch=getchar();}
    return f==1?x:-x;
}
#define maxn 1200
// 连通分量个数
ll cnt;
// 存储一棵树
ll parent[maxn]={0};
// 记录树的“重量”
ll weight[maxn]={0};
void init(ll n) {
    cnt = n;
    for (ll i = 1; i <= n; i++) {
        parent[i] = i;
        weight[i] = 1;
    }
}
ll find(ll x) {
    while (parent[x] != x) {
        // 进行路径压缩
        parent[x] = parent[parent[x]];
        x = parent[x];
    }
    return x;
}

void u(ll p, ll q) {
    ll rootP = find(p);
    ll rootQ = find(q);
    if (rootP == rootQ)
        return;
        
    // 小树接到大树下面，较平衡
    if (weight[rootP] > weight[rootQ]) {
        parent[rootQ] = rootP;
        weight[rootP] += weight[rootQ];
    } else {
        parent[rootP] = rootQ;
        weight[rootQ] += weight[rootP];
    }
    cnt--;
}

bool connected(int p, int q) {
    int rootP = find(p);
    int rootQ = find(q);
    return rootP == rootQ;
}

ll pos[maxn][3]={0};
ll n=0,h=0,r=0;

bool under(ll z){
    if(z - r <= 0){
        return true;
    }else{
        return false;
    }
}
bool up(ll z){
    if(z + r >= h){
        return true;
    }else{
        return false;
    }
}

bool flag(ll x1,ll y1,ll z1,ll x2,ll y2,ll z2){
    if(sqrt(pow(x1-x2,2)+pow(y1-y2,2)+pow(z1-z2,2)) <= r + r){
        return true;
    }else{
        return false;
    }
}

int main(){
    ll t = read();
    ll x,y,z;
    rep(i,1,t){
        n = read();
        h = read();
        r = read();
        init(n+2);
        //n+1是下表面 n+2是上表面
        rep(j,1,n){
            x = read();
            y = read();
            z = read();
            pos[j][0] = x;
            pos[j][1] = y;
            pos[j][2] = z;
            if(up(z)){
                u(n+2,j);
            }
            if(under(z)){
                u(n+1,j);
            }
            rep(k,1,j-1){
                if(flag(x,y,z,pos[k][0],pos[k][1],pos[k][2])){
                    u(k,j);
                }
            }
        } 

        if(connected(n+1,n+2)){
            printf("Yes\n");
        }else{
            printf("No\n");
        }   
    }
    return 0;
}
