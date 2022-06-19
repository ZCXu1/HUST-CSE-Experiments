/**
题目描述
如题，给出一个网络图，以及其源点和汇点，求出其网络最大流。
输入格式
第一行包含四个正整数 n,m,s,tn,m,s,t，分别表示点的个数、有向边的个数、源点序号、汇点序号。
接下来 m 行每行包含三个正整数 u_i,v_i,w_i，表示第 i 条有向边从 u_i出发，到达 v_i，边权为 w_i
输出格式
一行，包含一个正整数，即为该网络的最大流。
*/
// DINIC算法

#include <bits/stdc++.h>
using namespace std;
typedef long long LL;

const int N = 10010, E = 200010;

int n, m, s, t; LL ans = 0;
LL cnt = 1, first[N], nxt[E], to[E], val[E];
inline void addE(int u, int v, LL w) {
	to[++cnt] = v;
	val[cnt] = w;
	nxt[cnt] = first[u];
	first[u] = cnt;
}
int dep[N], q[N], l, r;
bool bfs() {//顺着残量网络，方法是 bfs；这是个bool型函数，返回是否搜到了汇点 
	memset(dep, 0, (n + 1) * sizeof(int));//记得开局先初始化 

	q[l = r = 1] = s;
	dep[s] = 1;
	while(l <= r) {
		int u = q[l++];
		for(int p = first[u]; p; p = nxt[p]) {
			int v = to[p];
			if(val[p] and !dep[v]) {//按照有残量的边搜过去 
				dep[v] = dep[u] + 1;
				q[++r] = v;
			}
		}
	}
	return dep[t];//dep[t] != 0，就是搜到了汇点 
}
LL dfs(int u, LL in/*u收到的支持（不一定能真正用掉）*/) {
//注意，return 的是真正输出的流量
	if(u == t)
		return in;//到达汇点是第一个有效return 
	LL out = 0;
	for(int p = first[u]; p and in; p = nxt[p]) {
		int v = to[p];
		if(val[p] and dep[v] == dep[u] + 1) {//仅允许流向下一层 
			LL res = dfs(v, min(val[p], in)/*受一路上最小流量限制*/);
			//res是v真正输出到汇点的流量
			val[p] -= res;
			val[p ^ 1] += res;
			in -= res;
			out += res;
		}
	}
	if(out == 0)//我与终点（顺着残量网络）不连通 
		dep[u] = 0;//上一层的点请别再信任我，别试着给我流量
	return out;
}
int main() {
	scanf("%d %d %d %d", &n, &m, &s, &t);
	for(int i = 1; i <= m; ++i) {
		int u, v; LL w;
		scanf("%d %d %lld", &u, &v, &w);
		addE(u, v, w);
		addE(v, u, 0);
	}
	while(bfs()) 
		ans += dfs(s, 1e18);
	printf("%lld\n", ans);
	return 0;
}