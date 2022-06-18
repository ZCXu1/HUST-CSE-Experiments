#include <iostream>
#include<windows.h>
#include<stdio.h>
#include <stdlib.h>
#pragma comment(lib, "ws2_32.lib")  
#define HEAP_SIZE 1024 * 8
using namespace std;

void heapOperation();

int main()
{
	heapOperation();
	return 0;
}


//堆操作(含重复释放）
void heapOperation() {

	printf("请按任意键来创建堆\n");
	getchar();
	HANDLE hHeap = HeapCreate(HEAP_NO_SERIALIZE, HEAP_SIZE * 10, HEAP_SIZE * 100);
	int* myArray = (int*)HeapAlloc(hHeap, 0, sizeof(int) * 50);

	for (int i = 0; i < 50; ++i)
	{
		myArray[i] = i + 1;
	}
	printf("成功创建!\n");
	for (int i = 0; i < 50; ++i)
	{
		if (i % 5 == 0)
			cout << endl;
		printf("%3d ", myArray[i]);
	}

	cout << endl;
	cout << endl;
	printf("请按任意键来开始第一次堆释放\n");
	getchar();
	HeapFree(hHeap, 0, myArray);
	printf("请按任意键来开始第二次堆释放\n");
	getchar();
	HeapFree(hHeap, 0, myArray);

	printf("请按任意键来销毁堆\n");
	getchar();
	HeapDestroy(hHeap);
	printf("成功销毁!\n");
	getchar();
}

