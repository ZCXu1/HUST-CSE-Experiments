#include <cstdio>
#include <algorithm>

using namespace std;

const int N=2000002;
long long m,n,a[N],ans,now,x;
long long read()
{	char x=getchar();
	while(x<48||x>57) x=getchar();
	long long ans=0;
	while(x>=48&&x<=57)
	{	ans=(ans<<1)+(ans<<3)+(x^48);
		x=getchar();
	}
	return ans;
}

int main()
{
	n=read();m=read();
	for(int i=1;i<=n;i++)
	a[i]=read();
	sort(a+1,a+n+1);
	a[n+1]=m+1;
	if(a[1]!=1){	
		printf("No answer!!!\n");return 0;
	}
	x=1;
	for(int i=1;i<=n;i++)
	{	if(a[i+1]<=now+1&&i<n) 
			continue;
		x=(a[i+1]-1-now)/a[i]+((a[i+1]-1-now)%a[i]>0);
		ans+=x;
		now+=x*a[i];
		if(now>=m) 
			break;
	}
	printf("%lld\n",ans);
	return 0;
}