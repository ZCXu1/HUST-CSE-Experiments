#include <stdio.h>

int main(){
    int n;
    scanf("%d",&n);
    for(int i = 0;i < n;i++){
        int a;
        scanf("%d",&a);
        printf("%lld\n",2 * a * a - a + 1);
    }
    return 0;
}