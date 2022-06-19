#include <iostream>
#include <string.h>//memset

using namespace std;
//dp1[i][j] 代表从A点到(i,j)的最大卡路里值
int dp1[1010][1010],dp2[1010][1010],dp3[1010][1010],dp4[1010][1010];
int n,m;
int a[1010][1010];
int main()
{
    scanf("%d%d",&n,&m);
    for(int i=1;i<=n;i++){
        for(int j=1;j<=m;j++){
            scanf("%d ",&a[i][j]);
        }
    }
    memset(dp1,0,sizeof(dp1));
    memset(dp2,0,sizeof(dp2));
    memset(dp3,0,sizeof(dp3));
    memset(dp4,0,sizeof(dp4));
    //A从出发点到照相点
     for(int i=1;i<=n;i++){
        for(int j=1;j<=m;j++){
            //要么从上面进 要么从右面进
            dp1[i][j]=a[i][j]+max(dp1[i-1][j],dp1[i][j-1]);
        }  
    }
    //A从终点到照相点 
    for(int i=n;i>=1;i--){
        for(int j=m;j>=1;j--){
            //要么从下面出 要么从右面出
            dp2[i][j]=a[i][j]+max(dp2[i+1][j],dp2[i][j+1]);
        }
    }
    //B从出发点到照相点 
    for(int i=n;i>=1;i--){
        for(int j=1;j<=m;j++){
            //要么从下面进 要么从左面进
            dp3[i][j]=a[i][j]+max(dp3[i+1][j],dp3[i][j-1]);
        }
    }
    //B从终点到照相点 
    for(int i=1;i<=n;i++){
        for(int j=m;j>=1;j--){
            //要么从上面出 要么从右面出
            dp4[i][j]=a[i][j]+max(dp4[i-1][j],dp4[i][j+1]);
        }
    }
    //x记录总卡路里数 
    int x=0;
    //枚举照相点(i,j)
    for(int i=2;i<n;i++){
        for(int j=2;j<m;j++){
            //设照相点为O，设O的左上右下为1,2,3,4
            /**设大矩形左上角为A 顺时针B C D
             * @brief 最终解为max（A到1最大值+3到C最大值+D到4最大值+2到B的最大值，
             * A到2最大值+4到C最大 值+D到1最大值+3到B最大值）。
             */
            //dp1[i][j-1]记录A到照相点左侧的卡路里值
            //dp2[i][j+1]记录照相点右侧到C的卡路里值
            x = max(max(x,dp1[i][j-1]+dp2[i][j+1]+dp3[i+1][j]+dp4[i-1][j]),max(x,dp1[i-1][j]+dp2[i+1][j]+dp3[i][j-1]+dp4[i][j+1]));
        }
    }
    printf("%d\n",x);
    return 0;
}