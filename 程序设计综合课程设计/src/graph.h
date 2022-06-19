/*
 * graph.h
 *
 *  Created on: 2021年3月1日
 *      Author: 徐子川
 */

/*------------预定义-----------*/
#ifndef GRAPH_H_
#define GRAPH_H_
//站的最大数量
#define MAX_NUM 280
//一共7条线路
#define LINEACCOUNT 7
//每条线的站的最大数量
#define MAXSTATIONATLINE 40
//六点开始发车
#define START 6*60
//二十三点结束运营
#define END 23*60
//两站间行车时间两分钟
#define DRIVE 2
//停车时间一分钟
#define STOP 1
//每五分钟发一辆车
#define LAUNCH 5
//换乘时间
#define WALK 3.5

/*------------头文件-----------*/
#include <cstdlib>
#include <vector>
#include <map>
#include <stack>
using namespace std;

/*-----------数据结构-----------*/
// 边表
typedef struct ArcNode
{
    // 邻接点在数组中的位置下标
    int adjVex;
    // 指向下一个邻接点的指针
    struct ArcNode* next;
    //该边属于哪条线
    int line;
    //两站间平均长度
    double distance;

}ArcNode;


//顶点表
typedef struct VerNode
{
    //站名
    string name;
    //所在线路
    int atLine;
    //是否可以换乘
    bool canChange;
    // 顶点的第一条弧
    struct ArcNode* firstArc;
}VerNode;


//图
typedef struct Graph
{
    // 顶点数组
    struct VerNode NodeList[MAX_NUM];
    // 总顶点数
    int totalVertex;
    // 总边数
    int totalArc;
}Graph;


//将line号线换为操作下标
int LineToIndex(int id){
    switch(id){
        case 1:
            return 0;
        case 2:
            return 1;
        case 3:
            return 2;
        case 4:
            return 3;
        case 6:
            return 4;
        case 7:
            return 5;
        case 8:
            return 6;
        default:
            return -1;
    }
}
//时
double hour;
//分
double minute;
//时加分全部转化为分
double hAndMin;
//每一号线的不同时间的拥挤度
double crowds[7][3];
//去除重复的总站数
int total;
//储存站的ID的数组
int IndexToLine[LINEACCOUNT] = {1,2,3,4,6,7,8};
//储存每一号线的两站平均间距
double distances[LINEACCOUNT];
//储存每一号线的站数量
int stations[LINEACCOUNT];
//每一点到起始点的最短距离(dijkstra用)
double d[MAX_NUM];
//每一点到起始点的最短时间(dijkstraTime用)
double t[MAX_NUM];
//每一点到起始点的最短时间(dijkstraTimeCrowd用)
double tCrowd[MAX_NUM];
//每一点到起始点的最短距离(dijkstraChange用)
double dc[MAX_NUM];
//根据站的序号找到名字 读取文件用
map<int,string> nameMap;
//储存站的序号的二维数组 同名的站序号放在一个一维数组里 读取文件用
vector<vector<int>> numberV;
//图的距离邻接矩阵
double matrix[MAX_NUM][MAX_NUM];
//图的时间邻接矩阵
double timeMatrix[MAX_NUM][MAX_NUM];
//图的考虑拥挤度的时间邻接矩阵
double timeMatrixCrowd[MAX_NUM][MAX_NUM];
//图的换乘邻接矩阵（时间邻接矩阵换乘站3.5变为20）
double changeMatrix[MAX_NUM][MAX_NUM];
/*------第一行为不考虑拥挤度的数据结构 第二行为考虑拥挤度的数据结构------*/
//dijkstra用的下标数组
int pre[MAX_NUM];
//dijkstraTime用的下标数组
int preT[MAX_NUM];
int preTCrowd[MAX_NUM];
//dijkstraChange用的下标数组
int preC[MAX_NUM];
//最短路径票价
int priceDis;
//最短时间票价
int priceTime;
int priceTimeCrowd;
//最少换乘次数票价
int priceChange;
//储存最短长度路径的栈
stack<int> sDis;
//储存最短时间路径的栈
stack<int> sTime;
stack<int> sTimeCrowd;
//储存最少换线次数路径的栈
stack<int> sChange;

/*-----------功能函数-----------*/
//为line号线count位站设置序号
int Number(int lineIndex,int count){
    return MAXSTATIONATLINE * lineIndex + count;
}


//图初始化
void initGraph(Graph *g){
    g->totalArc = 0;
    g->totalVertex = 0;
    for(int i = 0; i < MAX_NUM; i++){
        g->NodeList[i].canChange = false;
        g->NodeList[i].firstArc = nullptr;
        g->NodeList[i].name = "";
        g->NodeList[i].count = 1;
        for(int j = 0; j < LINEACCOUNT; j++){
            g->NodeList[i].atLine = 0;
        }
    }
}


// Number为pre的邻接边表后插入节点index
void insertArcNode(Graph *g, int pre, int index, double distance,int line) {
    if (g && pre < MAX_NUM && index < MAX_NUM) {
        ArcNode* p,* q;
        //动态分配内存
        p = (ArcNode*)malloc(sizeof(ArcNode));
        //动态分配内存失败
        if (!p) {
            return;
        }
        p->adjVex = index;
        p->line = line;
        p->distance = distance;
        p->next = nullptr;
        //开始插入节点
        q = g->NodeList[pre].firstArc;
        //如果首节点存在,找到结尾，然后插入
        if (q) {
            while (q->next) {
                q = q->next;
            }
            q->next = p;
        }
            //如果首节点不存在，在首节点位置插入
        else {
            g->NodeList[pre].firstArc = p;
        }
        g->NodeList[pre].count++;

    }
}


// 图的插入函数，如果两个节点邻接，将彼此插入对方的邻接边表
void insert(Graph *g, int index1, int index2, double distance,int line) {
    if (g && index1 < MAX_NUM && index2 < MAX_NUM) {
        matrix[index1][index2] = distance;
        matrix[index2][index1] = distance;
        timeMatrix[index1][index2] = DRIVE;
        timeMatrix[index2][index1] = DRIVE;
        insertArcNode(g, index1, index2, distance, line);
        insertArcNode(g, index2, index1, distance, line);
        g->totalArc++;
    }
}

#endif /* GRAPH_H_ */
