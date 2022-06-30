//main.cpp
#include<stdio.h>
#include<stdlib.h>
#define NUM 1000
#define N 10

char name[NUM][10], in_name[N];
short score[NUM][4];//一个short两个字节

extern "C" int calculate(short* s);
extern "C" int continueOrQuit(char* s1);
extern "C" int match(char* s1, char* s2);

int cpy(char* s1, const char s2[]) {
    int j = 0, i = 0;;
    for (j = 0; j < N; j++) {//对s1进行初始化
        *(s1 + j) = 0;
    }
    while (s2[i]) {
        *(s1 + i) = s2[i];//将s2字符串复制到s1
        i++;
    }
    return 0;
}

int main() {
    int flag = 0;
    int  num = 0, c = 0, i = 0, j = 0;

    cpy(name[0], "zhangsan");//初始化0-999号学生
    score[0][0] = 100;
    score[0][1] = 85;
    score[0][2] = 80;
    cpy(name[1], "lisi");
    score[1][0] = 80;
    score[1][1] = 100;
    score[1][2] = 70;
    cpy(name[2], "xzc");
    score[2][0] = 80;
    score[2][1] = 85;
    score[2][2] = 100;

    for (i = 3; i < NUM; i++) {
        cpy(name[i], "TempValue");
        score[i][0] = 80;
        score[i][1] = 90;
        score[i][2] = 95;
    }

    for (i = 0; i < NUM; i++) {//计算1000个人的平均成绩
        calculate(score[i]);//score[i]是第i个学生的语文成绩地址
    }

    while (1) {
        flag = 0, num = 0;
        for (j = 0; j < N; j++) {
            in_name[j] = 0;//初始化in_name字符串
        }
        printf("Please input the student's name:\n");
        printf("(Enter q to quit and enter a to calculate the average grade)\n");
        i = 0;
        c = getchar();
        while (c != '\n' && i < N - 1) {
            in_name[i] = c;
            i++;
            c = getchar();//通过getchar获得名字
        }
        int status = continueOrQuit(in_name);
        if (status == 1) {
            continue;//跳出本次循环 重新提示输入
        }
        if (status == 2) {
            printf("Exit successfully!");
            break;//跳出循环

        }
        if (status == 3) {
            for (i = 0; i < NUM; i++) {
                calculate(score[i]);
            }
            printf("Calculate all the average grades successfully!\n");
            flag = 2;
        }
            for (j = 0; j < 1000; j++) {
                if (match(in_name, name[j]) == 1) {
                    num = j;
                    flag = 1;
                    break;//获得名字的位置 跳出for循环
                }
            }

        if (flag == 0) {
            printf("%s does not exist!\n", in_name);
        }
        else if (flag == 1) {
            int t = score[num][3];
            printf("The grade of %s is %c !\n", in_name, t > 90 ? 'A' : t > 80 ? 'B' : t > 70 ? 'C' : t > 60 ? 'D' : 'F');
        }
    }
    return 0;
}   
      
