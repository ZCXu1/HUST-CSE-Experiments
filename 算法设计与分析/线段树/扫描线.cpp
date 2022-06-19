#include<bits/stdc++.h> 
using namespace std;
#define ls (rt << 1)//左儿子
#define rs (rt << 1 | 1)//右儿子
#define int long long
int n, x_, x__, y_, y__, x, y, rk[2097152], val[2097152], cnt, maxn = 1 << 31;

//注意：用万能库的同学千万不要以x1,x2,y1,y2作为变量名!!!!!!
//maxn是为了保证不会去理左右端点 

struct Segment_tree
{
	int l, r;//区间左右端点
	int cnt, len;//如上所述
}t[2097152];

struct node
{
	int x, yh, yl, flag;
}e[2097152];//记录每一条竖线

void pushup(int rt)
{
	if ((t[rt].l == maxn && t[rt].r == maxn)) return;
	if (t[rt].cnt) t[rt].len = val[t[rt].r + 1] - val[t[rt].l];
	else t[rt].len = t[ls].len + t[rs].len;
}//向上更新节点

void build(int rt, int l, int r)
{
	t[rt].l = l, t[rt].r = r;
	if (l == r) return;
	int mid = (t[rt].l + t[rt].r) >> 1;
	build(ls, l, mid);
	build(rs, mid + 1, r);
}

void add(int rt, int l, int r, int v)
{
	if (l <= t[rt].l && t[rt].r <= r)
	{
		t[rt].cnt += v;
		pushup(rt);
		return;
	}
	int mid = (t[rt].l + t[rt].r) >> 1;
	if (l <= mid) add(ls, l, r, v);
	if (mid < r) add(rs, l, r, v);
	pushup(rt);
}//基本操作，不再赘述

bool cmp(node a, node b)
{
	if (a.x != b.x) return a.x < b.x;
	return a.flag > b.flag;
}

signed main()
{
	cin >> n;
	int ans = 0;
	for (int i = 1; i <= n; i++)
	{
		scanf("%lld%lld%lld%lld", &x_, &y_, &x__, &y__);
		e[(i << 1) - 1].x = x_, e[i << 1].x = x__;
		e[(i << 1) - 1].yh = e[i << 1].yh = y__;
		e[(i << 1) - 1].yl = e[i << 1].yl = y_;
		e[(i << 1) - 1].flag = 1, e[i << 1].flag = -1;//flag表示该边是矩形的左边界或右边界
		rk[++cnt] = y_; 
		rk[++cnt] = y__;//存入一个数组，准备离散化
	}
	sort(rk + 1, rk + (n << 1) + 1);//排序
	cnt = unique(rk + 1, rk + (n << 1) + 1) - rk - 1;//去重
	for (int i = 1; i <= 2 * n; i++)
	{
		int pos1 = lower_bound(rk + 1, rk + cnt + 1, e[i].yh) - rk;
		int pos2 = lower_bound(rk + 1, rk + cnt + 1, e[i].yl) - rk;//在数组中二分查找位置
		val[pos1] = e[i].yh;
		val[pos2] = e[i].yl;//即为上文的raw数组
		e[i].yh = pos1; maxn = max(maxn, pos1);
		e[i].yl = pos2;
	}
	sort(e + 1, e + 2 * n + 1, cmp);//按照x坐标
	build(1, 1, n << 1);
	for (int i = 1; i < n << 1; i++)
	{
		add(1, e[i].yl, e[i].yh - 1, e[i].flag);//区间加flag
		ans += t[1].len * (e[i + 1].x - e[i].x);//根节点的len值*与下一条线段的距离=这一块内的矩形面积
	}
	cout << ans << endl;//得到最终答案,AC!
	return 0;
}