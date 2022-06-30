#include <iostream>
#include<cstdlib>
#include<string.h>

#define N 1000
#define NLENG 10
#define SDATA 14

using namespace std;

char notice[] = "Please input the students name:\n(enter q to quit, enter a to calculate the avarage grade)\n";
char granot[] = "The grade of the student is:\n";
char conti[] = "Please enter any key to continue...\n";
char notexist[] = "Sorry, the student does not exist.\n";
char calcuses[] = "Calculate all the avarage grades successfully!\n";
char inName[NLENG];

struct Student {//学生结构体，有姓名，语文成绩，数学成绩，英语成绩，平均成绩
	char name[NLENG+1];
	int grade[3];
	float avg;
};
struct Student students[N];

int getLen(char a[]) {
	int i = 0;
	int count = 0;
	while (a[i] != '\0') {
		count++;
		i++;
	}
	return count;
}

void getName() {
	cout << notice;
	cin >> inName;
}

int main() {

	strcpy(students[0].name,"ZhangSan00");
	students[0].grade[0] = 100;
	students[0].grade[1] = 85;
	students[0].grade[2] = 80;
	strcpy(students[1].name, "LiSi000000");
	students[1].grade[0] = 80;
	students[1].grade[1] = 100;
	students[1].grade[2] = 70;
	strcpy(students[0].name, "XuZiChuan0");
	students[2].grade[0] = 85;
	students[2].grade[1] = 90;
	students[2].grade[2] = 95;



	getName();
	if ((getLen(inName)==1)&&(*inName == 'q')) {//输入q就退出
		exit(0);
	}
	else if ((getLen(inName) == 1)&&(*inName == 'a')){
		cout << "2";
	}
	else if ((getLen(inName) == 1) && (*inName == '\n')) {//输入回车 还没做好
	}
	__asm {
		mov ax,students
		mov ds,ax
		mov es,ax
		mov cx,N
	}
		
	return 0;
}
