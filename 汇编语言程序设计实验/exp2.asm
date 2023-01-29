.386
DATA SEGMENT USE16
N EQU 1000
NLENG EQU 10;一个学生名字的长度
SDATA EQU 14;一个学生数据的长度
BUF DB 'ZhangSan',0,0;学生姓名：张三
    DB 100,85,80,?;张三的成绩，平均成绩未计算
    DB 'LiSi',6 DUP(0)
    DB 80,100,70,?
    DB N-3 DUP('TempValue',0,80,90,95,?)
    DB 'XuZiChuan',0;本人名字
    DB 85,90,95,?;本人的成绩

IN_NAME DB 10;最多欲接收字符个数
        DB ?;实际接收字符个数
        DB 10 DUP(0);第三字节开始存放输入的字符串

TEMPSTR DB 10 DUP(0)
NOTICE DB 'Please input the students name:' ,0DH, 0AH ; 0DH 代表回车, 0AH 代表换行
       DB '(enter q to quit, enter a to calculate the avarage grade)$'
GRANOT DB 'The grade of the student is:$'
CONTINUE DB 'Please enter any key to continue...',0DH,0AH,'$'
NOTEXIST DB 'Sorry, the student does not exist.' ,0DH,0AH,'$'
CALSUCES DB 'Calculate all the avarage grades successfully!',0DH,0AH,'$'
ENTRE DB 0DH,0AH,'$'
POIN DW 0
DATA ENDS  

STACK SEGMENT USE16 STACK
DB 200 DUP(0)                                                               
STACK ENDS    ;定义堆栈段

CODE SEGMENT USE16

ASSUME DS:DATA,SS:STACK,CS:CODE

START: MOV AX,DATA
       MOV DS,AX
       MOV ES,AX

HINT: LEA DX,NOTICE;将NOTICE的偏移地址放入DX
      MOV AH,09H; 9号DOS功能调用 输出字符串
      INT 21H ;弹出字符串提示输入
      LEA DX,ENTRE;回车换行
      MOV AH,09H
      INT 21H

      LEA DX,IN_NAME;将IN_NAME的偏移地址放入DX，
      MOV AH,0AH;10号DOS功能调用
      INT 21H;读入学生姓名
      MOV CX,N;读入学生个数
      MOV BX,OFFSET IN_NAME;将IN_NAME的偏移地址存入BX
      ADD BX,2;BX=BX+2 跳过两个字符串长度,从名字开始

      CMP IN_NAME+2,0DH;如果只输入回车
      JE HINT;返回开始输入
      CMP IN_NAME+2,'q';如果输入q
      JE QUIT;则退出
      CMP IN_NAME+2,'a';如果输入a
      JE GRALL;则计算平均成绩

      LEA DX,ENTRE;回车换行
      MOV AH,09H
      INT 21H
      INC CX;学生数加1
LOOPA: DEC CX
       JE NOMAT ;如果查找了N个人的名字也没查找到 输出不存在该学生
       MOV BP,N
       SUB BP,CX
       IMUL BP,SDATA;找到名字的首地址，即14*(N-CX)
       MOV DI,NLENG;DI中存入名字长度，每比一个就减一
       MOV SI,0;已查找的字符数
LOOPB: MOV DH,DS:[BP+SI];内存中名字的字符
       MOV DL,DS:[BX+SI];输入的名字的字符
       CMP DH,0;若名字后面有'0',比到0就说明比完了,开始计算平均成绩
       JE GRADE;
       CMP DH,DL;若有一个字符不符说明不是该学生
       JNE LOOPA;进入LOOPA对比下一个学生
       INC SI;若该字符一样,已查找的字符数加一
       DEC DI;DI控制进入GRADE还是LOOPB
       JE GRADE;若比完了,开始计算平均成绩
       JNE LOOPB;若没比完,再进入LOOPB进行下一个字符的比较
GRADE: MOV BP,N
       SUB BP,CX
       IMUL BP,SDATA;找到该同学名字的首地址，即14*(N-CX)
       ADD BP,NLENG;BP+=NLENG,跳过名字,取得成绩的首地址
       MOV POIN,BP;将成绩的偏移地址存入POIN
       MOV DX,0
       MOV AX,0;清零AX,DX
       MOV AL,DS:[BP];将语文成绩存入AL
       SAL AX,1;AX左移一位,实现乘2
       MOV DL,DS:[BP+1];将数学成绩存入DL
       ADD AX,DX;语文成绩乘2加数学成绩乘1存入AX
       MOV DL,DS:[BP+2];将英语成绩存入DL
       SAR DL,1;DL右移一位,实现除以2
       ADD AX,DX;语文成绩乘2加数学成绩加英语成绩除以2乘1存入AX
       SAL AX,1;AX左移一位,实现乘2
       MOV DL,7;再除以7
       IDIV DL;整体是除以3.5
       MOV DS:[BP+3],AL;将计算结果存入平均成绩位
       LEA DX,GRANOT;输出学生的平均成绩等级
       MOV AH,09H
       INT 21H
       CMP AL,90
       JGE GRADE_A
       CMP AL,80
       JGE GRADE_B
       CMP AL,70
       JGE GRADE_C
       CMP AL,60
       JGE GRADE_D
       JL GRADE_F

NOMAT: LEA DX,NOTEXIST;输出不存在该学生
       MOV AH,09H
       INT 21H
       JMP CONTI   

CONTI: LEA DX,ENTRE ;回车换行
       MOV AH,09H
       INT 21H
       LEA DX,CONTINUE;输出继续字符串
       MOV AH,09H
       INT 21H   

NEXT:  MOV AH,01H
       INT 21H ;字符输入,继续下一个操作
       JMP HINT    

GRADE_A:MOV DL,41H 
       MOV AH,2H
       INT 21H
       LEA DX,ENTRE
       MOV AH,9
       INT 21H
       JMP CONTI

GRADE_B:MOV DL,42H
       MOV AH,2H
       INT 21H
       LEA DX,ENTRE
       MOV AH,9
       INT 21H
       JMP CONTI
       
GRADE_C:MOV DL,43H
       MOV AH,2H
       INT 21H
       LEA DX,ENTRE
       INT 21H
       JMP CONTI

GRADE_D:MOV DL,44H
       MOV AH,2H
       INT 21H
       LEA DX,ENTRE
       MOV AH,9
       INT 21H
       JMP CONTI

GRADE_F:MOV DL,46H
       MOV AH,2H
       INT 21H
       LEA DX,ENTRE
       MOV AH,9
       INT 21H
       JMP CONTI
GRALL: MOV CX,N
       MOV BP,N
GRALP: SUB BP, CX;每计算完一个,CX就-1
       IMUL BP, SDATA
       ADD BP, NLENG;使得BP是当前学生的名字的首地址
       MOV DX,0
       MOV AX,0
       MOV AL,DS:[BP]; 开始计算平均成绩,方法同GRADE段
       SAL AX,1
       MOV DL, DS:[BP +1]
       ADD AX, DX
       MOV DL, DS:[BP +2]
       SAR DL, 1
       ADD AX, DX;
       SAL AX, 1
       MOV DL, 7
       IDIV DL
       MOV DS:[BP+3], AL
       MOV BP,N;每计算完将BP重置为N来定位下一个学生名字的首地址
       DEC CX;每计算完一个学生 CX就-1
       JNE GRALP;只要还有学生 就进入GRALP继续算该学生的平均成绩
       LEA DX, CALSUCES 
       MOV AH, 9;显示计算完毕
       INT 21H
       JE CONTI
QUIT: MOV AH,4CH
      INT 21H
CODE ENDS
END START
