//exp2->task1 2020.10.20

#include <stdio.h>
#define NUM 1000
#define N 10

char name[NUM][10], in_name[N];
short score[NUM][4];//一个short两个字节

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

int calculate(short* s) {//汇编计算函数
    _asm
    {
        mov esi, s      //成绩首地址存入esi
        mov edi, s      //成绩首地址存入edi
        add di, 6      //跳过语文数学英语共六个字节的成绩进入平均成绩地址
        cld            //DF标志位清零 意味着地址指针+2
        lodsw          //AX-<DS:[SI],SI+=2
        add ax, ax      //AX此时为语文成绩 先乘2
        add ax, ax      //再乘2为除以7做准备
        mov bx, ax      //把计算结果存入BX
        lodsw           //AX-<DS:[SI],SI+=2
        add ax, ax      //此时AX是数学成绩 乘2 为除以7做准备
        add bx, ax      //语文成绩*4+数学成绩*2
        lodsw           //AX-<DS:[SI],SI+=2
        add ax, bx      //语文成绩*4+数学成绩*2+英语成绩*1
        mov dl, 7
        div dl          //AH=AX/7
        mov ah, 0       //余数清零 
        stosw           //ES:[DI]<-AX,DI+=2 DI是平均成绩位置 算完了存到里面
    }
    return 0;
}

// 回车1，退出2,计算平均3，其他0
int continueOrQuit(char* s1) {//传入输入的名字
    _asm {
        mov esi, s1 //名字首个字符地址传入SI
        cld //DF标志位清零
        lodsw //AX-<DS:[SI],SI+=2
        cmp ax, 0000h//AX和回车比较
        je Continue//如果是回车 跳到continue
        cmp ax, 0071h//AX和'q'比较
        je Quit//如果是'q'跳到Quit
        cmp ax, 0061h//AX和'a'比较
        je CalAve
    }
    return 0;
Continue:_asm {
}
return 1;
Quit:_asm {
}
return 2;
CalAve:_asm {
}
return 3;
}

// 匹配1，不匹配0
int match(char* s1, char* s2) {//传入两个字符串首地址
    _asm {
        mov esi, s1//s1地址传入si
        mov edi, s2//s2地址传入di
        cld//DF标志位清零
        mov cx, N//CX存入最多的字符数
        repz cmpsb//repz 每执行一次串指令,DEC CX 只要CX=0或ZF=0,重复执行结束.cmpsb DS:[SI]-ES:[DI] INC SI, INC DI
        jne False//不相等 跳到False段
    }
    return 1;
False:_asm {
}
return 0;//字符串不匹配 返回0
}

int main() {
    int flag = 0, num = 0, c = 0, i = 0, j = 0;
    cpy(name[0], "zhangsan");//初始化0-999号学生
    score[0][0] =100;
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
