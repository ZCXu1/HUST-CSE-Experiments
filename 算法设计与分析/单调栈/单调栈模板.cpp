#include <iostream>
#include <vector>
#include <stack>
using namespace std;
int n,a[3000005],f[3000005];//a是需要判断的数组（即输入的数组），f是存储答案的数组
stack<int> S;
int main() {
    scanf("%d",&n);
    for (int i = 1; i <= n; ++i) scanf("%d",&a[i]);
    
    for (int i = 1; i <= n; ++i) {
        //栈从栈底到栈顶递减
        //从前往后扫 遇到比栈顶元素大的 就是栈顶的“下一个更大的数”
        while (!S.empty() && a[S.top()] < a[i]) {
            f[S.top()] = i;
            S.pop();
        }
        S.push(i);
    }
    for (int i = 1; i <= n; ++i) printf("%d ",f[i]);
    return 0;
}