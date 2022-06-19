#include<iostream>
using namespace std;
#define MAXN 1000007
#define INF 100000089
struct Splay_tree{
	int f,sub_size,cnt,value,tag;
	int son[2];
}s[MAXN];
int original[MAXN],root,wz;
inline bool which(int x){
	return x==s[s[x].f].son[1];
}
inline void update(int x){
	if(x){
	s[x].sub_size=s[x].cnt;
	if(s[x].son[0])s[x].sub_size+=s[s[x].son[0]].sub_size;
    if(s[x].son[1])s[x].sub_size+=s[s[x].son[1]].sub_size;
	}
}
inline void pushdown(int x){
    if(x&&s[x].tag){
    	s[s[x].son[1]].tag^=1;
    	s[s[x].son[0]].tag^=1;
    	swap(s[x].son[1],s[x].son[0]);
    	s[x].tag=0;
    }	
}
inline void rotate(int x){
	int fnow=s[x].f,ffnow=s[fnow].f;
	pushdown(x),pushdown(fnow);
	bool w=which(x);
	s[fnow].son[w]=s[x].son[w^1];
	s[s[fnow].son[w]].f=fnow;
	s[fnow].f=x;
	s[x].f=ffnow;
	s[x].son[w^1]=fnow;
	if(ffnow){
		s[ffnow].son[s[ffnow].son[1]==fnow]=x;
	}
	update(fnow);
}
inline void splay(int x,int goal){
	for(int qwq;(qwq=s[x].f)!=goal;rotate(x)){
		if(s[qwq].f!=goal){//这个地方特别重要，原因是需要判断的是当前的父亲有没有到目标节点，而如果把“qwq”改成“x”……就会炸 
			rotate(which(x)==which(qwq)?qwq:x);
		}
	}
	if(goal==0){
		root=x;
	}
}

int build_tree(int l, int r, int fa) {
        if(l > r) { return 0; }
        int mid = (l + r) >> 1;
        int now = ++ wz;
        s[now].f=fa;
	    s[now].son[0]=s[now].son[1]=0;
		s[now].cnt++;
    	s[now].value=original[mid];
		s[now].sub_size++;
        s[now].son[0] = build_tree(l, mid - 1, now);
        s[now].son[1] = build_tree(mid + 1, r, now);
        update(now);
        return now;
}
inline int find(int x){
	int now=root;
	while(1)
	{
	    pushdown(now);
		if(x<=s[s[now].son[0]].sub_size){
			now=s[now].son[0];
		}	
		else  {
		x-=s[s[now].son[0] ].sub_size + 1;
	    if(!x)return now;
	    now=s[now].son[1];
		}
	}
}
inline void reverse(int x,int y){
	int l=x-1,r=y+1;
	l=find(l),r=find(r);
	splay(l,0);
	splay(r,l);
	int pos=s[root].son[1];
	pos=s[pos].son[0];
	s[pos].tag^=1;
}
inline void dfs(int now){
	pushdown(now);
	if(s[now].son[0])dfs(s[now].son[0]);
	if(s[now].value!=-INF&&s[now].value!=INF){
		cout<<s[now].value<<" ";
	}
	if(s[now].son[1])dfs(s[now].son[1]);
}
int main(){
	int n,m,x,y;
	cin>>n>>m;
	original[1]=-INF,original[n+2]=INF;
	for(int i=1;i<=n;i++){
		original[i+1]=i;
	}
	root=build_tree(1,n+2,0);
	for(int i=1;i<=m;i++){
		cin>>x>>y;
		reverse(x+1,y+1);
	}
	dfs(root);
}