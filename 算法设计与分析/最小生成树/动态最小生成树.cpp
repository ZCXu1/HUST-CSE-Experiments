#include <iostream>
#include <algorithm>
#include <string.h>
#include <cmath>
#include <vector>
using namespace std;
typedef long long ll;
inline char read1() {
	static const int IN_LEN = 1000000;
	static char buf[IN_LEN], *s, *t;
	return (s==t?t=(s=buf)+fread(buf,1,IN_LEN,stdin),(s==t?-1:*s++):*s++);
}

inline uint32_t read(void)
{
	char ch;
	uint32_t x = 0;
	ch = read1();
	while (ch != ' ' && ch != '\n' && ch != '\r')
	{
		x *= 16;
		if (ch >= '0' && ch <= '9')
			x += ch - '0';
		else
			x += ch - 'a' + 10;
		ch = read1();
	}
	return x;
}

#define N 40005
#define INF INT32_MAX
int n,m,k;
int d[255][255];
int s,sum;
int dis[255],v[255];


void prim(){
	s = 0;
	for(int i = 1;i<=n;i++){
		dis[i] = d[1][i];
		v[i] = 0;
	}
	v[1] = 1;
	int pos = 0;
	for(int i = 1;i<=n; i++){
		int min = INF;
		for(int j=1;j<=n;j++){
			if(!v[j]&&min>dis[j]){
				pos = j;
				min = dis[j];
			}
		}
		if(min == INF) break;
		v[pos] = 1;
		s += min ;
		for(int j=1;j<=n;j++){
			if(!v[j]&&dis[j]>d[pos][j])
				dis[j] = d[pos][j];
		}
	}
}


void dj(){    //Dijkstra求最短路径
	//初始化
	for(int i=1;i<=n;i++){    //初始化将dis[i]赋值为点1到点i的直接距离
		dis[i] = d[1][i];
		v[i] = 0;      //标记数组
	}
	dis[1] = 0;
	v[1] = 1;
	int pos = -1;
	int min = 0;
	for(int i = 1;i<=n; i++){
		min = INF;
		for(int j=1;j<=n;j++){
			if(!v[j]&&min>dis[j]){    //选出当前到点1路径最短的点
				pos = j;       //记录点位置和对应的距离
				min = dis[j];
			}
		}
		v[pos] = 1;        //标记选出的点加入点集
             for(int j=1;j<=n;j++){
           int temp = (d[pos][j]==INF?INF : (min + d[pos][j]));  //防止溢出
            if(!v[j]&&dis[j]>temp)     //对dis进行更新。判断min + d[pos][j](点j通过中继点pos到点1的距离)是否小于当前点j到点1的距离(dis[j])，若小于则更新
		dis[j] = temp;          
		}
	}
	sort(dis+1,dis+1+n);
	if(dis[n]==INF) cout <<-1 << " " <<-1<<endl;    //判断是否联通
	else {
		sum = 0;
		for(int i=1;i<=n;i++)
			sum += dis[i];        //计算路径和
		prim();        //暴力prim
		cout << s <<" " << sum <<endl;   //结果
	} 
}

int main(){
	cin >> n >> m >> k;
	for(int i=1;i<=n;i++)
		for(int j=1;j<=n;j++){
			d[i][j] = (i==j?0:INF);
		}
			
	
	for(int i=1;i<=m;i++){
		int u,v;
		scanf("%d%d",&u,&v);
		scanf("%d",&d[u][v]);
		d[v][u] = d[u][v];
	}
	for(int i=1;i<=k;i++){
		int op,x,y;
		cin >> op >> x >> y;
		if(op==1){
			 d[x][y] = INF;
			 d[y][x] = INF;
		}
		else {
			int z;
			cin >> z;
			d[x][y] = z;
			d[y][x] = z;
		}
		dj();
	}	
    return 0;
}