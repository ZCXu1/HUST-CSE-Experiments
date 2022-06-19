
typedef long long ll;
using namespace std;
#define maxn 3000005

// 连通分量个数
ll cnt;
// 存储一棵树
ll parent[maxn]={0};
//编号为i的战舰到parent[i]的距离
ll d[maxn] = {0};
//编号为i的战舰所在的一列有多少战舰
ll s[maxn] = {0};
void init(ll n) {
    cnt = n;
    for (ll i = 1; i <= n; i++) {
        parent[i] = i;
        s[i] = 1;
    } 
}
ll find(ll x) {
    if (parent[x] != x) {
        int k = parent[x];
        //每次find的时候都更新d和s
        parent[x] = find(parent[x]);
        d[x] += d[k];
        s[x] = s[parent[x]];
    }
    return parent[x];
}

void u(ll p, ll q) {
    ll rootP = find(p);
    ll rootQ = find(q);
    if (rootP == rootQ)
        return;
    //p放到q的后面
    parent[rootP] = rootQ;

    d[rootP] = d[rootQ] + s[rootQ];
    s[rootQ] += s[rootP];
    s[rootP] = s[rootQ];
    cnt--;
}

bool connected(ll p, ll q) {
    return find(p) == find(q);
}
