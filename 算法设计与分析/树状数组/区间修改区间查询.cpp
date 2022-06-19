//区间修改区间查询
/**
给定数列 a[1],a[2],…,a[n]，你需要依次进行 q 个操作，操作有两类：
1 l r x：给定 l,r,x，对于所有 i∈[l,r]，将 a[i] 加上 x（换言之，将 a[l],a[l+1],…,a[r] 分别加上 x）；
2 l r：给定 l,r，求 ∑ri=la[i] 的值（换言之，求 a[l]+a[l+1]+⋯+a[r] 的值）。
 * 
 */
#include<stdio.h>
#define maxn 1100000
typedef long long ll;
ll a[maxn],n;
ll c1[maxn];
ll c2[maxn];
int lowbit(ll x)
{
    return x&(-x);
}
ll up_tree(ll i,ll x)
{
    ll l=0,t=i;
   for(l=i;l<=n;l=l+lowbit(l))
   {
       c1[l]=c1[l]+x;
       c2[l]=c2[l]+x*(t-1);
   }
}
ll sum(ll x)
{
    ll i=x,ans=0;
    while(i>0)
    {
        ans=ans+x*c1[i]-c2[i];
        i=i-lowbit(i);
    }
    return ans;
}
int main()
{
    ll q,i;
    scanf("%lld %lld",&n,&q);
    for(i=1;i<=n;i++)
    {
        scanf("%lld",&a[i]);
        up_tree(i,a[i]-a[i-1]);
    }
    while(q--)
    {
        ll l,r,x;
        char s[2];
        scanf("%s",s);
        if(s[0]=='1')
        {
            scanf("%lld %lld %lld",&l,&r,&x);
            up_tree(l,x);
            up_tree(r+1,-x);
        }
        else
        {
            scanf("%lld %lld",&l,&r);
            printf("%lld\n",sum(r)-sum(l-1));
        }
    }
}
