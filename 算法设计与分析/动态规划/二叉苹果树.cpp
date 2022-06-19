#include <iostream>
#include <cstdio>
using namespace std;
struct node
{
    int t; //这条边通向的节点
    int apple; //第i条树枝的苹果数
    int next; //第i条边的下一条边
};
node e[2*101];
int dp[101][101];
int head[101],n,q,tot=0;
void add(int x,int y,int z)  //邻接表存数
{
    e[++tot].t=y;
    e[tot].apple=z;
    e[tot].next=head[x];
    head[x]=tot;
}
void dfs(int f,int fa,int apple) //递归遍历这棵树
{
    int son[101]={0},cnt=0; //son[1]表示f的左儿子在第几条边,son[2]表示f的右儿子在第几条边
    bool flag=false;
    for(int xun=head[f];xun;xun=e[xun].next)
    {
        if(e[xun].t!=fa)
        {
        	flag=true;
            son[++cnt]=xun;
            dfs(e[xun].t,f,e[xun].apple);
        }
    }
    if(!flag) //如果没有儿子，说明它是叶子结点，直接回溯
    {
        return;
    }
    for(int i=1;i<=q;i++) //DP部分
    {
        for(int j=0;j<=i;j++)
        {
        	int t1=0;
        	if(j-1>=0) t1+=e[son[1]].apple;  //j-1>=0表示分配给了左儿子与i节点的一条相连的树枝
        	if(i-j-1>=0) t1+=e[son[2]].apple;//i-j-1>=0表示分配给了右儿子与i节点的一条相连的树枝
        	if(j!=0)
         		dp[f][i]=max(dp[f][i],dp[e[son[1]].t][j-1]+t1+dp[e[son[2]].t][i-j-1]);  //j!=0，表示两个儿子都分配了
         	else //j==0，表示只分配给了右儿子树枝
         		dp[f][i]=max(dp[f][i],dp[e[son[2]].t][i-j-1]+t1);
        }
    }
}
int main()
{
    scanf("%d %d",&n,&q);
    for(int i=1;i<=n-1;i++)
    {
        int x,y,z;
        scanf("%d %d %d",&x,&y,&z);
        add(x,y,z);
        add(y,x,z);
    }
    dfs(1,0,0);
    printf("%d",dp[1][q]); //因为最终我们要求的苹果数是以1为根节点的子树中保留q根树枝的最大苹果数，所以最终的结果等于dp[1][q]
    return 0;
}