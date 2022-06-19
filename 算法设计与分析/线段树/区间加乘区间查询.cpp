/**已知一个数列，你需要进行下面三种操作：
将某区间每一个数乘上 x
将某区间每一个数加上 x
求出某区间每一个数的和
输入格式
第一行包含三个整数 n,m,p分别表示该数列数字的个数、操作的总个数和模数。
第二行包含 n 个用空格分隔的整数，其中第 i 个数字表示数列第 i 项的初始值。
接下来 m 行每行包含若干个整数，表示一个操作，具体如下：
操作 1： 格式：1 x y k 含义：将区间 [x,y] 内每个数乘上 k
操作 2： 格式：2 x y k 含义：将区间 [x,y] 内每个数加上 k
操作 3： 格式：3 x y 含义：输出区间 [x,y] 内每个数的和对 p 取模所得的结果(要求为正)
 */
#include <iostream>
#include <cstdio>
using namespace std;
typedef long long ll;
//题目中给的p
ll p;
//暂存数列的数组
ll a[100007];
//线段树结构体，v表示此时的答案，mul表示乘法意义上的lazytag，add是加法意义上的
struct node{
    ll v, mul, add;
}st[400007];
//buildtree
void bt(ll root, ll l, ll r){
//初始化lazytag
    st[root].mul=1;
    st[root].add=0;
    if(l==r){
        st[root].v=a[l];
    }
    else{
        ll m=(l+r)/2;
        bt(root*2, l, m);
        bt(root*2+1, m+1, r);
        st[root].v=st[root*2].v+st[root*2+1].v;
    }
    st[root].v=(st[root].v+p)%p;
    return ;
}
//核心代码，维护lazytag
void pushdown(ll root, ll l, ll r){
    ll m=(l+r)/2;
//根据我们规定的优先度，儿子的值=此刻儿子的值*爸爸的乘法lazytag+儿子的区间长度*爸爸的加法lazytag
    st[root*2].v=(st[root*2].v*st[root].mul+st[root].add*(m-l+1)+p)%p;
    st[root*2+1].v=(st[root*2+1].v*st[root].mul+st[root].add*(r-m)+p)%p;
//很好维护的lazytag
    st[root*2].mul=(st[root*2].mul*st[root].mul+p)%p;
    st[root*2+1].mul=(st[root*2+1].mul*st[root].mul+p)%p;
    st[root*2].add=(st[root*2].add*st[root].mul+st[root].add+p)%p;
    st[root*2+1].add=(st[root*2+1].add*st[root].mul+st[root].add+p)%p;
//把父节点的值初始化
    st[root].mul=1;
    st[root].add=0;
    return ;
}
//update1，乘法，stdl此刻区间的左边，stdr此刻区间的右边，l给出的左边，r给出的右边
void ud1(ll root, ll stdl, ll stdr, ll l, ll r, ll k){
//假如本区间和给出的区间没有交集
    if(r<stdl || stdr<l){
        return ;
    }
//假如给出的区间包含本区间
    if(l<=stdl && stdr<=r){
        st[root].v=(st[root].v*k+p)%p;
        st[root].mul=(st[root].mul*k+p)%p;
        st[root].add=(st[root].add*k+p)%p;
        return ;
    }
//假如给出的区间和本区间有交集，但是也有不交叉的部分
//先传递lazytag
    pushdown(root, stdl, stdr);
    ll m=(stdl+stdr)/2;
    ud1(root*2, stdl, m, l, r, k);
    ud1(root*2+1, m+1, stdr, l, r, k);
    st[root].v=(st[root*2].v+st[root*2+1].v+p)%p;
    return ;
}
//update2，加法，和乘法同理
void ud2(ll root, ll stdl, ll stdr, ll l, ll r, ll k){
    if(r<stdl || stdr<l){
        return ;
    }
    if(l<=stdl && stdr<=r){
        st[root].add=(st[root].add+k+p)%p;
        st[root].v=(st[root].v+k*(stdr-stdl+1)+p)%p;
        return ;
    }
    pushdown(root, stdl, stdr);
    ll m=(stdl+stdr)/2;
    ud2(root*2, stdl, m, l, r, k);
    ud2(root*2+1, m+1, stdr, l, r, k);
    st[root].v=(st[root*2].v+st[root*2+1].v+p)%p;
    return ;
}
//访问，和update一样
ll query(ll root, ll stdl, ll stdr, ll l, ll r){
    if(r<stdl || stdr<l){
        return 0;
    }
    if(l<=stdl && stdr<=r){
        return st[root].v;
    }
    pushdown(root, stdl, stdr);
    ll m=(stdl+stdr)/2;
    return (query(root*2, stdl, m, l, r)+query(root*2+1, m+1, stdr, l, r)+p)%p;
}
int main(){
    ll n, m;
    scanf("%lld%lld%lld", &n, &m, &p);
    for(ll i=1; i<=n; i++){
        scanf("%lld", &a[i]);
    }
    bt(1, 1, n);
    while(m--){
        ll chk;
        scanf("%lld", &chk);
        ll x, y;
        ll k;
        if(chk==1){
            scanf("%lld%lld%lld", &x, &y, &k);
            ud1(1, 1, n, x, y, k);
        }
        else if(chk==2){
            scanf("%lld%lld%lld", &x, &y, &k);
            ud2(1, 1, n, x, y, k);
        }
        else{
            scanf("%lld%lld", &x, &y);
            printf("%lld\n", query(1, 1, n, x, y));
        }
    }
    return 0;
}