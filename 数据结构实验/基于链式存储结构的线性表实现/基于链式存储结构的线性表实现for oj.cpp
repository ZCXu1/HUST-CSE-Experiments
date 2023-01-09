/*
Date:2020-11-1
*/

#include<iostream>
#include<cstdlib>

using namespace std;

#define TRUE 1    //预定义
#define FALSE 0
#define OK 1
#define ERROR 0
#define INFEASTABLE -1
#define OVERFLOW -2

#define LIST_INIT_SIZE 100
#define LISTINCREMENT 10

typedef int Status;//状态
typedef char ElemType;//元素类型

typedef struct LNode {
    ElemType data;//元素类型
    float coeff;//系数
    int index;//指数
    struct LNode* next;//指针域
}LNode, * LinkList;//LinkList=LNode*

LinkList list[11];//定义一个线性表数组便于管理多个线性表,多项式10用于减法函数
LinkList* pL;//二级指针指向链表的头指针
int number=0;//当前链表序号


Status InitaList(LinkList& L) {//初始化链表
    L = new LNode[sizeof(LNode)];//开辟空间
    if (!L)   exit(OVERFLOW);
    L->next = NULL;//置空
    return OK;
}

Status DestroyList(LinkList& L) {//销毁链表
    LNode* p;
    while (L) {
        p = L;
        L = L->next;
        delete[]p;//一个一个释放节点
    }
    L = NULL;//头指针置空
    return OK;
}

Status ClearList(LinkList L) {//清空表
    LNode* p, * q;
    p = L->next;
    L->next = NULL;//置空
    while (p) {//利用p和q释放节点
        q = p->next;
        delete[]p;
        p = q;
    }
    return OK;
}

Status ListEmpty(LinkList L) {//判断链表是否为空
    if (L->next)
        return FALSE;
    else
        return TRUE;
}

Status ListLength(LinkList L) {//获得链表的长度(除头结点)
    LNode* p;
    int i = 0;
    p = L->next;
    while (p) {
        i++;
        p = p->next;
    }
    return i;
}

Status GetElem(LinkList L, int i, ElemType &e) {//获得i位置的元素 用e返回
    LNode* p;
    p = L->next;
    int j = 1;
    while (p && j < i) {//p从第一个节点开始找 找到第i个
        p = p->next;
        j++;
    }
    if (!p || j > i)
        return ERROR;
    e = p->data;//此时p指向第i个结点
    return OK;
}

Status  LocateElem(LinkList L, ElemType e) {//获得元素e的位置
    LNode* p;
    int i = 1;
    p = L->next;
    while (p && p->data != e) {//p从第一个开始找 找到数据为元素e
        p = p->next;
        i++;
    }
    if (p)
        return i;//返回e的位置
    else
        return ERROR;
}

Status PriorElem(LinkList L, ElemType cur_e, ElemType& pre_e) {//获得元素cur_e的前驱
    LNode* p, * q;
    p = L;
    q = p->next;
    while (q->next && q->data != cur_e) {//利用p和q寻找,一直找到最后一个或cur_e
        p = q;
        q = p->next;
    }
    if (q && p != L) {
        pre_e = p->data;//p是q的前驱 返回p的数据到pre_e
        return OK;
    }
    else
        return ERROR;
}

Status NextElem(LinkList L, ElemType cur_e, ElemType& next_e) {//获得元素cur_e的后继
    LNode* p, * q;
    p = L->next;
    q = p->next;
    while (q&& p->data != cur_e) {//利用p和q找到p为cur_e或q空为止
        p = q;
        q = p->next;
    }
    if (q) {//q是p的后继 若p为最后一个节点 q会空
        next_e = q->data;//返回p的后继q的数据到next_e
        return OK;
    }
    else
        return ERROR;
}

Status ListInsert(LinkList L, int i, ElemType e) {//在第i个位置插入元素e
    LNode* p, * q;
    int j = 0;
    p = L;
    while (p && j < i - 1) {//结束循环时j=i-1 由0到i-1 使p指向第i-1个结点
        p = p->next;
        j++;
    }
    if (!p || j > i - 1) return ERROR;
    q = new LNode[sizeof(LNode)];//开辟新节点 连接在p后面
    if (!q)  return OVERFLOW;
    q->data = e;
    q->next = p->next;
    p->next = q;

    return OK;
}

Status ListDelete(LinkList L, int i, ElemType &e) {//删除第i个元素 用e返回
    LNode* p, * q;
    int j = 0;
    p = L;
    while (p && j < i - 1) {//找到第i-1个结点
        p = p->next;
        ++j;
    }
    if (!(p->next) || j > i - 1)
        return ERROR;
    q = p->next;//p是第i-1个结点 则q是第i个
    e = q->data;//用e返回
    p->next = q->next;//删除q指向的结点
    delete[]q;//释放
    return e;
}

Status ListTraverse(LinkList L) {//遍历链表
    LNode* p;
    if (!L)  return INFEASTABLE;
    p = L->next;
    if (!p)  return ERROR;
    while (p) {
        cout<<p->data;
        p = p->next;//输出每个元素
    }
    cout << endl;
    return OK;
}

Status setListnum(int num) {//设置当前链表工作序号
    
    if ((num > 0) &&(num < 11)) {
        pL = &(list[num - 1]);//pL是全局变量
        return OK;
    }
    else
        return ERROR;
}

LNode* Last(LinkList L) {//返回多项式链表最后一个节点
    LNode* q;
    q = L;
    while (q->next) {
        q = q->next;
    }
    return q;
}

Status CheckIndex(float x, int y) {//检查是否有可以合并的项
    LNode* p, * q;//p用来找有无相同的项 q用来判断若有相同的项 相加后系数是否为0
    q = *pL;
    p = (*pL)->next;
    while (p) {
        if (p->index == y) break;//如果当前项的指数是y 跳出循环
        q = p;
        p = p->next;
    }
    if (!p) return INFEASTABLE;//如果没有相同指数的项 返回没有
    else {
        p->coeff += x;//找到相同指数 则系数相加
        if (p->coeff == 0) {//若相加后系数为0 则删除节点
            q->next = p->next;
            delete[]p;
            return ERROR;
        }
        else return OK;
    }
}

Status PolySort(LinkList L) {//指数递减顺序排序多项式 冒泡排序思想 一轮比较找出一个最小的放在最后的位置
    int len = ListLength(L);//len为多项式长度
    int i, j;
    LNode* p, * q, * pre;
    for (i = 1; i <len; i++) {//第i轮比较
        pre = L;//每轮比较都要从头开始  pre是p的前驱 方便交换用
        p = pre->next;
        q = p->next;
        for (j = 0; j < len-i; j++) {//第i轮第j+1次比较
            if (q) {
                if (p->index < q->index) {
                    pre->next = q;
                    p->next = q->next;
                    q->next = p;//交换p节点与q节点
                    p = pre->next;
                    q = p->next;//交换p指针与q指针位置 以便进行下一次比较
                }
                pre = pre->next;//每比较完一次指针后移
                p = p->next;
                q = q->next;
            }
        }
    }
    return OK;
}

Status CreateList(int listnum) {//序号listnum 建立一个多项式
    LNode* p, * q;
    int i = 1, n = 0;//i表示第i项 n表示一共有n项
    cin >> n;
    q = *pL;
    float x = 1;
    int y;
    while (x && i <= n) {
        cin >> x >> y; //x是第i项的系数，y是指数
        if (!x) {
            i++;
            continue;//如果系数是0，直接跳到下一个循环输入下一项
        }
        Status flag = CheckIndex(x, y);//检查原多项式是否有可以合并的项 有就合并
        if (flag == INFEASTABLE) {
            //如果没有可以合并的 就建立新的节点
            p = new LNode[sizeof(LNode)];
            p->coeff = x;//系数
            p->index = y;//指数
            p->next = NULL;//下一个节点为空
            q = Last(*pL);//令q指向最后一个节点 方便插入
            q->next = p;//把p接到链表尾
        }
        i++ ;
    }
    if (ListLength(*pL) == 0) {//没有项 该多项式为0
        p = new LNode[sizeof(LNode)];
        p->coeff = 0;
        p->index = 0;
        p->next = NULL;
        q->next = p;//q是当前链表头结点，下一项是0，再下一项空，整个多项式为0
    }
    else PolySort(*pL);//多项式不为0，按递减顺序排序
    return OK;
}

Status ShowList(int listnum) {//显示多项式 注意系数为0，1，-1和指数为0和1的情况 分开讨论
    LNode* p;
    p = list[listnum - 1];
    if (!p || !(p->next)) return ERROR;
    p = p->next;//先考虑第一项
    if (p->coeff > 0) {
        if (p->coeff == 1) {//如果系数是1 可以忽略
            if (p->index == 1) cout << "x";//如果指数是1 可以忽略
            else if (p->index == 0) cout << "1";//如果指数是0 输出1
            else cout << "x^" << p->index;
        }
        else {
            if (p->index == 1) cout << p->coeff << "x";//如果指数是1 可以忽略
            else if (p->index == 0) cout << p->coeff;//如果指数是0 直接输出系数
            else cout << p->coeff << "x^" << p->index;
        }
    }
    else if (p->coeff == 0) cout << "0";//整个多项式为0 输出0
    else {
        if (p->coeff == -1) {//负数情况同上
            if (p->index == 1) cout << "-x";
            else if (p->index == 0) cout << "-1";
            else cout << "-x^" << p->index;
        }
        else {
            if (p->index == 1) cout << p->coeff << "x";
            else if (p->index == 0) cout << p->coeff;
            else cout << p->coeff << "x^" << p->index;
        }
    }
    p = p->next;
    while (p) {//与第一项不同
        if (p->coeff > 0) {//如果系数正 需要补符号"+" 其余同上
            if (p->coeff == 1) {
                if (p->index == 1) cout << "+x";
                else if (p->index == 0) cout << "+1";
                else cout << "+x^" << p->index;
            }
            else {
                if (p->index == 1) cout << "+" << p->coeff << "x";
                else if (p->index == 0) cout << "+" << p->coeff;
                else cout << "+" << p->coeff << "x^" << p->index;
            }
        }
        else {
            if (p->coeff == -1) {
                if (p->index == 1) cout << "-x";
                else if (p->index == 0) cout << "-1";
                else cout << "-x^" << p->index;
            }
            else {
                if (p->index == 1) cout << p->coeff << "x";
                else if (p->index == 0) cout << p->coeff;
                else cout << p->coeff << "x^" << p->index;
            }
        }
        p = p->next;
    }
    cout << endl;
    return OK;
}

Status AddList(int a, int b, int c) {//a和b多项式相加 结果储存在c里
    LNode* pa, * pb, * pc, * qa, * qb, * qc;
    ClearList(list[c - 1]);//先清空c
    qa = list[a - 1];
    pa = qa->next;
    qb = list[b - 1];
    pb = qb->next;
    qc = list[c - 1];
    while (pa && pb) {//a和b都没有遍历完
        pc = new LNode[sizeof(LNode)];
        if (pa->index > pb->index) {//从指数大的项开始加
            pc->coeff = pa->coeff;
            pc->index = pa->index;
            qc->next = pc;
            qc = qc->next;
            pc->next = NULL;
            pa = pa->next;
        }
        else if (pa->index < pb->index) {//从指数大的项开始加
            pc->coeff = pb->coeff;
            pc->index = pb->index;
            qc->next = pc;
            qc = qc->next;
            pc->next = NULL;
            pb = pb->next;
        }
        else {
            pc->coeff = pa->coeff + pb->coeff;
            if (pc->coeff == 0) delete[]pc;//如果相加为0 释放该节点
            else {
                pc->index = pa->index;
                qc->next = pc;
                qc = qc->next;
                pc->next = NULL;
            }
            pa = pa->next;
            pb = pb->next;
        }
    }
    while (pa) {//多项式b先遍历结束
        pc = new LNode[sizeof(LNode)];//开辟新节点 和a此时结点相同 接在链表c后面
        pc->coeff = pa->coeff;
        pc->index = pa->index;
        qc->next = pc;
        qc = qc->next;
        pc->next = NULL;
        pa = pa->next;
    }
    while (pb) {//a先遍历结束
        pc = new LNode[sizeof(LNode)];//开辟新节点 和b此时结点相同 接在链表c后面
        pc->coeff = pb->coeff;
        pc->index = pb->index;
        qc->next = pc;
        qc = qc->next;
        pc->next = NULL;
        pb = pb->next;
    }
    if (!(list[c - 1]->next)) {//如果操作后c不存在项
        pc = new LNode[sizeof(LNode)];
        pc->coeff = 0;
        pc->index = 0;
        pc->next = NULL;
        list[c - 1]->next = pc;
    }
if(ListLength(list[c-1])!=1){//如果链表c长度不是1
    qc = list[c - 1];
    pc = qc->next;
    while (pc) {//若多项式中有0 去除该节点
        if (pc->coeff == 0) {
            qc->next = pc->next;
            delete[]pc;
            break;
        }
        else {
            qc = pc;
            pc = pc->next;
        }
    }
    }
    return OK;
}

Status MinusList(int a, int b, int c) {//多项式a和b相减，存在c里
    LNode* p, * q, * pb;
    setListnum(10);//10暂存加了负号的多项式b
    InitaList(*pL);
    q = *pL;
    pb = list[b - 1]->next;
    while (pb) {//将b添加负号，其余不变
        p = new LNode[sizeof(LNode)];
        p->coeff = -(pb->coeff);
        p->index = pb->index;
        p->next = NULL;
        q->next = p;
        q = q->next;
        pb = pb->next;
    }
    AddList(a, 10, c);//减法化为加法
    DestroyList(*pL);//销毁10
    setListnum(c);
    return OK;
}

int main() {
    pL = NULL;//二级指针置空
    int op = 1;//op是输入的序号
    int i = 0, a, b, c = 0;//i是输入的变量，abc是多项式序号
    int num = 0;//链表序号
    ElemType e = 0, cur_e = 0, pre_e = 0, next_e = 0;//元素，当前元素，前驱，后继

    while (op) {
        cin >> op; 
        switch (op) {
        case 1:
            InitaList(*pL);
            break;
        case 2:
            DestroyList(*pL);
            break;
        case 3:
            ClearList(*pL);
            break;
        case 4:
            if (ListEmpty(*pL) == TRUE)
                cout << "1\n";//空表输出1
            else if (ListEmpty(*pL) == FALSE)
                cout << "0\n";//非空表输出0
            break;
        case 5:
            cout << ListLength(*pL) << endl;
            break;
        case 6:
            cin >> i;
            if (GetElem(*pL, i, e) == OK) cout<<e<<endl;
            else  cout << "NoElem" << endl;                    //不存在则打印NoElem
            break;
        case 7:
            cin >> e;
            if (LocateElem(*pL, e) != ERROR)
                cout << LocateElem(*pL, e) << endl;
            else
                cout << "NoElem\n";
            break;
        case 8:
            cur_e = e;//一直往前找前驱 要更改cur_e
            if (PriorElem(*pL, cur_e, pre_e) == OK)
            {
                e = pre_e;//一直往前找前驱 要更改cur_e
                cout << pre_e << endl;
            }
            else
                cout << "NoElem\n";
            break;
        case 9:
            cur_e = e;//一直往后找后继 要更改cur_e
            if (NextElem(*pL, cur_e, next_e) == OK)
            {
                e = next_e;//一直往后找后继 要更改cur_e
                cout << next_e << endl;
            }
            else
                cout << "NoElem\n";
            break;
        case 10:
            cin >> i;
            cin >> e;
            ListInsert(*pL, i, e);
            break;
        case 11:
            cin >> i;
            ListDelete(*pL, i, e);
            cout << e << endl;
            break;
        case 12:
            ListTraverse(*pL);
            break;
        case 19:
            cin >> number;
            setListnum(number);
            break;
        case 21:
            CreateList(number);
            break;
        case 22:
            cin >> num;
            ShowList(num);
            break;
        case 23:
            cin >> a >> b >> c;
            AddList(a, b, c);

            break;
        case 24:
            cin >> a >> b >> c;
            MinusList(a, b, c);
            break;
        case 0:
            break;
        }//end of switch
    }//end of while
    return 0;
}