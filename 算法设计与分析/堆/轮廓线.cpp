#include<cstdio>
#include<queue>
#include<algorithm>
using namespace std;
#define N 1000005
struct building{
	int h,r;//h为高度，r为右端点
	building(int h_,int r_){h=h_;r=r_;}
	bool operator <(const building& i)const{return h<i.h;}//重载运算符，以高度h来建大根堆。
};

struct sp{
	int v,w;//v指向原来位置，w为x坐标
	bool p;//p=0则是左端点，p=1是右端点
	sp(int v_=0,int w_=0,bool p_=0){v=v_;w=w_;p=p_;}
}b[N*2];


int n,a[N][3];
inline bool comp(const sp&x,const sp&y){return x.w<y.w;}

priority_queue<building>q;

int main(){
	int l,h,r;
	while (scanf("%d%d%d",&l,&h,&r)!=EOF){
		a[++n][0]=h;
		a[n][1]=l;
		a[n][2]=r;
		b[n*2-1]=sp(n,l,0);
		b[n*2]=sp(n,r,1);//把左右端点都加个b数组
	}
	sort(b+1,b+2*n+1,comp);//将b数组按x坐标排序
	int last=0;
    //遍历b数组 也即遍历所有的线
	for (int i=1;i<=2*n;i++){
        //说明如果堆顶的building的右坐标小于当前遍历的线的坐标
        //说明堆顶的building已经被遍历过去了 不应当存在在堆里了
        //这一段if是核心 如何判断堆顶的building已经成为了过去 该删掉了
		if (q.size()){
			r=(q.top()).r;
			while (q.size()&&b[i].w>=r){
				q.pop();
				if (q.empty()) break;
				r=(q.top()).r;
			}
		}
        //是左端点就把左端点代表的building入堆
		if (!b[i].p) 
            q.push(building(a[b[i].v][0],a[b[i].v][2]));

        //两条线坐标重合
		if (i<2*n&&b[i].w==b[i+1].w) 
            continue;

		if (q.size()) 
            h=(q.top()).h; 
        else 
            //堆空高度为0
            h=0;

        //高度变化则输出
		if (h!=last){
			printf("%d %d ",b[i].w,h);
			last=h;
		}
	}
	return 0;
}