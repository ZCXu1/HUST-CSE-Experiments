#include<iostream>
using namespace std;
//一遍递推算出最长上升子序列，再一遍递推算出最长下降子序列，再一遍历维护max求出答案。
int n,a[105],f[2][105],ans;
int main(){
    scanf("%d",&n);
    for(int i=1;i<=n;i++) 
		scanf("%d",&a[i]);

    a[0]=0;

    for(int i=1;i<=n;i++)//从1到n求LIS
    	for(int j=0;j<i;j++) 
			if(a[i]>a[j]) 
				f[0][i]=max(f[0][i],f[0][j]+1);
				
    a[n+1]=0;

    for(int i=n;i;i--)//从n到1求LIS
    	for(int j=n+1;j>i;j--) 
			if(a[i]>a[j]) 
				f[1][i]=max(f[1][i],f[1][j]+1);

    for(int i=1;i<=n;i++) 
		ans=max(f[0][i]+f[1][i]-1,ans);//枚举Ti，从1到Ti的LIS+从TK到Ti的LIS-1(Ti被加了两次) 

    printf("%d\n",n-ans);
    return 0;
}