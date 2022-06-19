/*
 * mainfunction.h
 *
 *  Created on: 2021年3月1日
 *      Author: 徐子川
 */
#ifndef METRO_MAINFUNCTION_H
#define METRO_MAINFUNCTION_H

#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <set>

#include "graph.h"
#include "compute.h"

//文件名
#define INPUT "InputData.txt"
#define DATA "MetroData.txt"
ofstream ofs;
//计算时间时是否考虑拥挤度
bool considerCrowd;
//是否自己配置拥挤度
bool ChangeCrowd;
//是否选择避开特定的拥挤度线路
bool avoid;
//设定的要避开的拥挤度下限
double setedCrowd;

using namespace std;


//清空栈
void clearStack(stack<int> &s){
    s = stack<int>();
}


//将线路信息、站点信息的数据文件读入系统，并进行构图
void read(Graph *g){
    for (int l = 0; l < MAX_NUM; ++l) {
        for (int i = 0; i < MAX_NUM; ++i) {
            matrix[l][i] = INT16_MAX;
        }
    }
    for (int m = 0; m < MAX_NUM; ++m) {
        for (int i = 0; i < MAX_NUM; ++i) {
            timeMatrix[m][i] = INT16_MAX;
        }
    }
    //集合s用来判断是否导入过某一站
    set<string> s;
    //初始化总站数为0
    total = 0;
    ifstream in;
    //打开文件
    in.open(DATA);
    for(int i = 0; i < LINEACCOUNT; i++){
        //接收数据的变量
        int id,people,account;
        double length;
        //插入用
        int pre,index;
        //读取信息
        in >> id >> people >> length >> account;
        stations[i] = account;
        double distance = length / (account - 1);
        distances[i] = distance;
        for(int j = 0; j < account; j++){
            index = Number(i, j);
            int number;
            string name;
            in >> number >> name;
            nameMap.insert(pair<int,string>(index,name));
            //之前没有导入该站
            if(!s.count(name))
            {
                s.insert(name);
                total++;
            }
            g->totalVertex++;
            g->NodeList[index].name = name;
            g->NodeList[index].atLine = IndexToLine[i];
            //如果j不为0 则已经导入了至少两个站 此时pre已经被赋值
            if(j > 0){
                insert(g,pre,index,distance,IndexToLine[i]);
            }
            pre = index;
        }
    }
    //关闭文件
    in.close();
    for(int i = 0; i < LINEACCOUNT; i++){
        for(int j = 1; j < stations[i] - 2;j++){
            timeMatrix[Number(i,j)][Number(i,j+1)] = STOP + DRIVE;
        }
        for(int j = stations[i] - 2; j > 0; j--){
            timeMatrix[Number(i,j)][Number(i,j-1)] = STOP + DRIVE;
        }
    }
    //初始化nameMap和numberV
    //判断map中的元素是否被添加进vector中
    bool flags[MAX_NUM];
    for(bool & flag : flags){
        flag = false;
    }
    for(auto i = nameMap.begin();i != nameMap.end(); i++){
        vector<int> v;
        if(!flags[i->first]){
            v.push_back(i->first);
            flags[i->first] = true;
        }
        for(auto & j : nameMap){
            if(i->second == j.second && !flags[j.first]){
                v.push_back(j.first);
                flags[j.first] = true;
            }
        }
        if(!v.empty()){
            numberV.push_back(v);
        }
    }

    for(auto & i : numberV){
        if(i.size() > 1){
            for(auto j = i.begin(); j != i.end(); j++){
                //初始化站是否可以换乘
                g->NodeList[*j].canChange = true;
                //将两个不同index的同名站连接起来 距离设为0
                for(auto k = j; k != i.end(); k++){
                    if(k != j){
                        matrix[*k][*j] = 0;
                        matrix[*j][*k] = 0;
                        timeMatrix[*k][*j] = WALK;
                        timeMatrix[*j][*k] = WALK;
                        insertArcNode(g, *k, *j, 0, g->NodeList[*j].atLine);
                        insertArcNode(g, *j, *k, 0, g->NodeList[*k].atLine);
                        g->totalArc++;
                    }
                }
            }
        }
    }
    //初始化换乘邻接矩阵
    for(int i = 0; i < MAX_NUM; i++){
        for (int j = 0; j < MAX_NUM; ++j) {
            if(g->NodeList[i].name == g->NodeList[j].name){
                changeMatrix[i][j] = 20;
            } else{
                changeMatrix[i][j] = timeMatrix[i][j];

            }
        }
    }
    //初始化考虑拥挤度的时间矩阵为时间邻接矩阵
    for (int i = 0; i < MAX_NUM; ++i) {
        for (int j = 0; j < MAX_NUM; ++j) {
            timeMatrixCrowd[i][j] = timeMatrix[i][j];
        }
    }
    //初始化//每一号线的不同时间的拥挤度
    /*上下班类型：1 号线，3 号线；
    7:30-9:00，拥挤度 80%；16:30-18:30，拥挤度 75%；其余时间拥挤度 40%；
    */
    crowds[0][0] = 0.8;
    crowds[0][1] = 0.75;
    crowds[0][2] = 0.4;
    crowds[2][0] = 0.8;
    crowds[2][1] = 0.75;
    crowds[2][2] = 0.4;
    /*城际交通类型：2 号线，4 号线；
    全天拥挤度 50%；
    */
    for (int i = 0; i < 3; ++i) {
        crowds[1][i] = 0.5;
        crowds[3][i] = 0.5;
    }
    /*
    购物类型：8 号线；
    9:30-15:00，拥挤度 65%；其余时间拥挤度 20%
     */
    crowds[6][0] = 0.65;
    crowds[6][1] = 0.2;
    crowds[6][2] = 0.2;
    /*娱乐类型：6 号线，7 号线；
    19:00-22:00，拥挤度 65%；其余时间拥挤度 15%；
    */
    crowds[4][0] = 0.65;
    crowds[4][1] = 0.15;
    crowds[4][2] = 0.15;
    crowds[5][0] = 0.65;
    crowds[5][1] = 0.15;
    crowds[5][2] = 0.15;
    clearStack(sDis);
    clearStack(sTime);
    clearStack(sTimeCrowd);
    clearStack(sChange);
}


//判断输入线路是否存在
bool existLine(int line){
    return line == 1 || line == 2 || line == 3 || line == 4 || line == 6 || line == 7 || line == 8;
}


//判断输入站点是否存在
bool existStation(Graph g,string name){
    for(auto & i : numberV){
        if(g.NodeList[i[0]].name == name){
            return true;
        }
    }
    return false;
}


//把指定线路的站点依次显示出来
void print(Graph g){
    int line;
    while(true) {
        cout << "请输入要查询的线路：";
        cin >> line;
        ofs<<line<<endl;
        if(existLine(line)){
            break;
        }else{
            cout<<"输入线路有误！请重新输入。"<<endl;
        }
    }
    for (int i = 1; i <= stations[LineToIndex(line)]; ++i) {
        cout << setw(2) <<i<< "." << g.NodeList[Number(LineToIndex(line), i - 1)].name<<"\t";
        if (i % 5 == 0) {
            cout << endl;
        }
    }
    cout<<endl;
}


//返回line号线站名name的序号
int getStationNumber(Graph g,int line,const string& name){
    for(auto & i : nameMap){
        if(i.second == name){
            if(g.NodeList[i.first].atLine == line){
                return i.first;
            }
        }
    }
    return -1;
}


/***
 * 指定线路、起始站点
 * 并从该站点开始逐步找出下一站点
 * 或在换乘站点进行换线
 * 实现对地铁路线站点的依次浏览
 * */
void nextOrChange(Graph g){
    int startL;
    while(true){
        cout<<"请输入起始线路：";
        cin>>startL;
        ofs<<startL<<endl;
        if(existLine(startL)){
            break;
        }else{
            cout<<"输入线路有误！请重新输入。"<<endl;
        }
    }
    string startStationName;
    while(true){
        cout<<"请输入起始站点：";
        cin>>startStationName;
        ofs<<startStationName<<endl;
        if(existStation(g, startStationName)){
            break;
        }else{
            cout<<"输入站点有误！请重新输入。"<<endl;
        }
    }
    int index = getStationNumber(g, startL, startStationName);
    while(true){
        //判断是否可以换乘
        char in;
        if(g.NodeList[index].canChange){
            while(true){
                cout<<"当前站可以换乘，请选择是否换乘（是：y/Y,否：n/N）：";
                cin>>in;
                ofs<<in<<endl;
                if(in != 'y' && in != 'n' && in != 'Y' && in != 'N'){
                    cout<<"输入选项有误！请重新输入"<<endl;
                }
                else{
                    break;
                }
            }
            if(in == 'y' || in == 'Y'){
                int change = 0;
                while(true){
                    bool transfer = false;
                    cout<<"可换乘的线有："<<endl;
                    //遍历numberV找到index在的一维数组
                    auto find = numberV.begin();
                    bool flag = false;
                    for(auto i = numberV.begin(); i != numberV.end(); i++){
                        if(i->size() > 1){
                            for(auto j = i->begin(); j != i->end(); j++){
                                if(*j == index){
                                    flag = true;
                                    find = i;
                                    break;
                                }
                            }
                        }
                        if(flag){
                            break;
                        }
                    }
                    for(auto i = find->begin(); i != find->end(); i++){
                        if(*i != index){
                            cout<<g.NodeList[*i].atLine<<"号线"<<endl;
                        }
                    }
                    cout<<"请选择换乘至几号线："<<endl;
                    cin>>change;
                    ofs<<change<<endl;
                    for(auto i = find->begin(); i != find->end(); i++){
                        if(*i != index){
                            if(g.NodeList[*i].atLine == change){
                                index = *i;
                                startL = change;
                                transfer = true;
                                break;
                            }
                        }
                    }
                    if(transfer){
                        cout<<"您已换乘至"<<g.NodeList[index].atLine<<"号线"<<endl;
                        break;
                    }
                    cout<<"输入线路有误！请重新输入。"<<endl;
                }
                //change不为0说明换线成功 但是换线后仍然要判断是否换乘 保证系统合理性
                if(change != 0){
                    continue;
                }
            }
        }
        while(true){
            cout<<"是否查询下一站点（是：y/Y,否：n/N）：";
            cin>>in;
            ofs<<in<<endl;
            if(in != 'y' && in != 'n' && in != 'Y' && in != 'N'){
                cout<<"输入选项有误！请重新输入"<<endl;
            }
            else{
                break;
            }
        }
        if(in == 'N' || in == 'n'){
            break;
        }
        else{
            while(true){
                ArcNode* p = g.NodeList[index].firstArc;
                cout<<"下一站是："<<endl;
                while(p){
                    if(p->distance != 0){
                        cout<<g.NodeList[p->adjVex].name<<endl;
                    }
                    p = p->next;
                }
                cout<<"请输入你要查询的下一站：";
                string next;
                cin>>next;
                ofs<<next<<endl;
                int to = getStationNumber(g, startL, next);
                if(to == -1){
                    cout<<"输入站名有误！请重新输入。"<<endl;
                }else{
                    index = to;
                    cout<<"已变更为"<<g.NodeList[index].name<<endl;
                    break;
                }
            }
        }
    }
}


/*
 * 构建一套模拟武汉地铁的主要转乘线路询问系统，
 * 输入两个不同的起始站点和目的站点，设置当前时间
 * 给出 1-3 个转乘线路的建议
 * 并给出需要花费的时间和票价
 * 以及计算并显示线路中各段路线的拥挤程度
 * */
//设置当前时间
void setTime(){
    while(true){
        cout<<"请输入当前时(0~23)：";
        cin>>hour;
        ofs<<hour<<endl;
        if(hour < 0 || hour > 23){
            cout<<"输入错误！请重新输入。"<<endl;
        }else{
            break;
        }
    }
    while(true){
        cout<<"请输入当前分(0~59)：";
        cin>>minute;
        ofs<<minute<<endl;
        if(minute < 0 || minute > 59){
            cout<<"输入错误！请重新输入。"<<endl;
        }else{
            break;
        }
    }
    hAndMin = 60 * hour + minute;
}


//匹配当前时刻的前后多少分的几号线的进站和出站时刻


void matchTime(Graph g){
    setTime();
    int line;
    int range;
    int index;
    string name;
    while(true){
        cout<<"请输入要查询的线路：";
        cin>>line;
        ofs<<line<<endl;
        cout<<"请输入要查询的站点：";
        cin>>name;
        ofs<<name<<endl;
        index = getStationNumber(g, line, name);
        if(index != -1){
            break;
        }else{
            cout<<"输入站点或线路有误！请重新输入。"<<endl;
        }
    }
    while(true){
        cout<<"请输入要查询的时间范围（整数，单位：分钟）：";
        cin>>range;
        ofs<<range<<endl;
        if(range < 0 || (hAndMin + range >= END) || (hAndMin - range <= START)){
            cout<<"输入时间范围有误！请重新输入。"<<endl;
        }else{
            break;
        }
    }
    //查询的站位于该线路的第几个站
    int num = 0;
    //将查询线路换为可操作的下标
    int indexL = LineToIndex(line);
    for(int i = 0; i < stations[indexL]; i++){
        if(Number(indexL,i) == index){
            num = i;
            break;
        }
    }
    //从序号为0的一方
    cout<<endl;
    cout<<"────────────────────────列车时刻表────────────────────────"<<endl;
    cout<<"★★★开往"<<g.NodeList[Number(indexL, stations[indexL] - 1)].name<<"方向★★★"<<endl;
    //计算与序号0的站之间站数 来计算行程时间
    int t1 = (num - 0 + 1 - 1) *DRIVE + (num - 0 + 1 - 2) * STOP;
    vector<int> time;
    for(int i = 0;t1 + i * LAUNCH < END - START ; i++){
        time.push_back(START + t1 + i * LAUNCH);
    }
    cout<<"列车离开时间为："<<endl;
    for(int & i : time){
        if(i >= hAndMin - range && i <= hAndMin){
                cout<<setw(2) << ((i + 1) / 60) << "时"<<setw(2) << ((i + 1) % 60) << "分" << "   ";
        }
    }
    cout<<endl;
    cout<<"列车到达时间为："<<endl;
    for(int & i : time){
        if(i <= hAndMin + range && i > hAndMin){
            cout<<setw(2)<<(i/60)<<"时"<<setw(2)<<(i%60)<<"分"<<"   ";
        }
    }
    cout<<endl;
    cout<<"★★★开往"<<g.NodeList[Number(indexL, 0)].name<<"方向★★★"<<endl;
    int t2 = (stations[indexL] - 1 - num + 1 - 1) * DRIVE + (stations[indexL] - 1 - num + 1 - 2) * STOP;
    vector<int> time2;
    for(int i = 0; t2 + i * LAUNCH < END - START; i++){
        time2.push_back(START + t2 + i * LAUNCH);
    }
    cout<<"列车离开时间为："<<endl;
    for(int & i : time2){
        if(i >= hAndMin - range && i <= hAndMin){
            cout<<setw(2)<<((i+1)/60)<<"时"<<setw(2)<<((i+1)%60)<<"分"<<"   ";
        }
    }
    cout<<endl;
    cout<<"列车到达时间为："<<endl;
    for(int & i : time2){
        if(i <= hAndMin + range && i > hAndMin){
            cout<<setw(2)<<(i/60)<<"时"<<setw(2)<<(i%60)<<"分"<<"   ";
        }
    }
    cout<<endl;
    cout<<"───────────────────────────────────────────────────────────"<<endl;
}


//自顶向下打印栈储存的路径
void printStack(Graph g,stack<int> s){
    while (!s.empty()){
        if(s.size()>1){
            cout<<g.NodeList[s.top()].name<<"("<<g.NodeList[s.top()].atLine<<"号线)->";
        }else{
            cout<<g.NodeList[s.top()].name<<"("<<g.NodeList[s.top()].atLine<<"号线)"<<endl;
        }
        s.pop();
    }
}


//求最短长度的路径
void dijkstra(Graph g,int start,int end){
    //顶点最大个数
    int n = MAX_NUM;
    // 判断是否已存入该点到S集合中 为真则表示该点已经确立了距离起始点的最小值
    bool s[n];
    for(int i = 0; i < n;i++){
        //初始化d数组为两点之间的距离
        d[i] = matrix[start][i];
        // 初始都未用过该点
        s[i] = false;
        //将与初始点连接的站点的pre设为初始点 若不连接 设为-1
        if(d[i] == INT16_MAX){
            pre[i] = -1;
        }else{
            pre[i] = start;
        }
    }
    d[start] = 0;
    s[start] = true;
    //除了初始点剩余n-1个点 循环n-1次
    for (int i = 0; i < n - 1 ; ++i) {
        //通过minD寻找与每一次循环与初始点距离最近的点 即min{d[i]}
        int minD = INT16_MAX;
        // 找出当前未使用的点j的dist[j]最小值
        int u = start;
        for(int j = 0; j < n; j++){
            if(!s[j] && d[j] < minD){
                //第i+1次循环 寻找第i+1个与初始点距离最近的点
                // u保存当前邻接点中距离最小的点的号下标
                u = j;
                minD = d[j];
            }
        }
        // 设为真
        s[u] = true;
        //对于从u出发的所有边(u,y)且没有确立距离起始点最小值，更新d[y] = min{d[y], d[u]+w(u,y)}
        for(int j = 0; j < n; j++){
            if(!s[j] && matrix[u][j] < INT16_MAX){
                //在通过新加入的u点路径找到离v0点更短的路径
                if(d[u] + matrix[u][j] < d[j]){
                    //更新dist
                    d[j] = d[u] + matrix[u][j];
                    //记录前驱顶点
                    pre[j] = u;
                }
            }
        }
    }
    int a = end;
    while(a != start){
        sDis.push(a);
        a = pre[a];
    }
    sDis.push(start);
    priceDis = price(d[end]);
}


//打印最短长度的路径
void printDis(Graph g,int start,int end){
    int a = end;
    int changeTime = 0;
    double pathTime = 0;
    while(a != start){
        pathTime += timeMatrix[pre[a]][a];
        if(g.NodeList[a].name == g.NodeList[pre[a]].name){
            changeTime++;
        }
        a = pre[a];
    }
    if(hAndMin + pathTime < 23 * 60){
        cout << "From: " << g.NodeList[start].name << " To: " << g.NodeList[end].name << endl << "(总路程共计"
             << setiosflags(ios::fixed) << setprecision(2) << d[end] << "公里,总时间共计" << pathTime << "分钟,票价共计" << priceDis
             << "元,换乘次数共计" << changeTime << "次)" << endl;
        printStack(g, sDis);
        cout << "整个乘车期间的拥挤程度为：" << pathCrowd(g, sDis, hAndMin) * 100 << "%" << endl;
    }else{
        cout << "由于时间问题，地铁停运无法到达目的地！" << endl;
    }
}


//求最短时间的路径
void dijkstraTime(Graph g,int start,int end){
    int n = MAX_NUM;
    bool s[n];
    for(int i = 0; i < n;i++){
        t[i] = timeMatrix[start][i];
        s[i] = false;
        if(t[i] == INT16_MAX){
            preT[i] = -1;
        }else{
            preT[i] = start;
        }
    }
    t[start] = 0;
    s[start] = true;
    for (int i = 0; i < n - 1 ; ++i) {
        double minT = INT16_MAX;
        int u = start;
        for(int j = 0; j < n; j++){
            if(!s[j] && t[j] < minT){
                u = j;
                minT = t[j];
            }
        }
        s[u] = true;
        for(int j = 0; j < n; j++){
            if(!s[j] && timeMatrix[u][j] < INT16_MAX){
                if(t[u] + timeMatrix[u][j] < t[j]){
                    t[j] = t[u] + timeMatrix[u][j];
                    preT[j] = u;
                }
            }
        }
    }
    int a = end;
    double pathDis = 0;
    while(a != start){
        sTime.push(a);
        pathDis += matrix[preT[a]][a];
        a = preT[a];
    }
    sTime.push(start);
    priceTime = price(pathDis);
}


//打印最短时间的路径
void printTime(Graph g,int start,int end){
    int a = end;
    double pathDis = 0;
    int changeTime = 0;
    while(a != start){
        pathDis += matrix[preT[a]][a];
        if(g.NodeList[a].name == g.NodeList[preT[a]].name){
            changeTime++;
        }
        a = preT[a];
    }
    cout<<"From: "<<g.NodeList[start].name<<" To: "<<g.NodeList[end].name
    <<endl<<"(总路程共计"<<setiosflags(ios::fixed)<<setprecision(2)<<pathDis
    <<"公里,总时间共计"<<t[end]<<"分钟,票价共计"<<priceTime<<"元,换乘次数共计"
    <<changeTime<<"次)"<<endl;
    printStack(g,sTime);
    cout << "整个乘车期间的拥挤程度为：" << pathCrowd(g, sTime, hAndMin) * 100 << "%" << endl;
}


//求换乘次数最少的路径
void dijkstraChange(Graph g,int start,int end){
    int n = MAX_NUM;
    bool s[n];
    for(int i = 0; i < n;i++){
        dc[i] = changeMatrix[start][i];
        s[i] = false;
        if(dc[i] == INT16_MAX){
            preC[i] = -1;
        }else{
            preC[i] = start;
        }
    }
    dc[start] = 0;
    s[start] = true;

    for (int i = 0; i < n - 1 ; ++i) {
        double minDC = INT16_MAX;
        int u = start;
        for(int j = 0; j < n; j++){
            if(!s[j] && dc[j] < minDC){
                u = j;
                minDC = dc[j];
            }
        }
        s[u] = true;
        for(int j = 0; j < n; j++){
            if(!s[j] && changeMatrix[u][j] < INT16_MAX){
                if(dc[u] + changeMatrix[u][j] < dc[j]){
                    dc[j] = dc[u] + changeMatrix[u][j];
                    preC[j] = u;
                }
            }
        }
    }
    int a = end;
    double pathDis = 0;
    while(a != start){
        sChange.push(a);
        pathDis += matrix[preC[a]][a];
        a = preC[a];
    }
    sChange.push(start);
    priceChange = price(pathDis);
}


//打印换乘次数最少的路径
void printChange(Graph g,int start,int end){
    int a = end;
    double pathDis = 0;
    stack<int> st;
    //换乘次数
    int changeTime = 0;
    while(a != start){
        st.push(a);
        pathDis += matrix[preC[a]][a];
        if(g.NodeList[a].name == g.NodeList[preC[a]].name){
            changeTime++;
        }
        a = preC[a];
    }
    if(hAndMin +dc[end] - changeTime * (20 - WALK) < 23 * 60) {
        cout << "From: " << g.NodeList[start].name << " To: " << g.NodeList[end].name << endl << "(总路程共计"
             << setiosflags(ios::fixed) << setprecision(2) << pathDis << "公里,总时间共计"
             << dc[end] - changeTime * (20 - WALK) << "分钟,票价共计" << priceChange << "元,换乘次数共计" << changeTime << "次)"
             << endl;
        printStack(g, sChange);
        cout << "整个乘车期间的拥挤程度为：" << pathCrowd(g, sChange, hAndMin) * 100 << "%" << endl;
    }else{
        cout << "由于时间问题，地铁停运无法到达目的地！" << endl;
    }
}


//根据当前时间hAndMin更新考虑拥挤度的时间邻接矩阵
void update(Graph g){
    for (int i = 0; i < MAX_NUM; ++i) {
        for (int j = 0; j < MAX_NUM; ++j) {
            timeMatrixCrowd[i][j] = timeMatrix[i][j];
        }
    }
    for (int i = 0; i < MAX_NUM; ++i) {
        for (int j = 0; j < MAX_NUM; ++j) {
            if(timeMatrixCrowd[i][j] != INT16_MAX){
                //换乘的时候不需要考虑拥挤度
                if(g.NodeList[i].name != g.NodeList[j].name){
                    timeMatrixCrowd[i][j] *= timeRatio(crowd(g,hAndMin,i),avoid,setedCrowd);
                }
            }
        }
    }
}


//计算路径s的加权时间
double pathWeightedTime(Graph g,stack<int> s){
    double weightedTime = 0;
    update(g);
    while (s.size() > 1){
        int a = s.top();
        s.pop();
        int b = s.top();
        weightedTime += timeMatrixCrowd[a][b];
        hAndMin += timeMatrix[a][b];
        update(g);
    }
    return weightedTime;
}


//求最短加权时间的路径
void dijkstraTimeCrowd(Graph g,int start,int end){
    int n = MAX_NUM;
    bool s[n];
    for (int i = 0; i < n; ++i) {
        tCrowd[i] = timeMatrixCrowd[start][i];
        s[i] = false;
        if(tCrowd[i] == INT16_MAX){
            preTCrowd[i] = -1;
        }else{
            preTCrowd[i] = start;
        }
    }
    tCrowd[start] = 0;
    s[start] = true;
    for (int i = 0; i < n - 1; ++i) {
        double min = INT16_MAX;
        int u = start;
        for (int j = 0; j < n; ++j) {
            if(!s[j] && tCrowd[j] < min){
                u = j;
                min = tCrowd[j];
            }
        }
        s[u] = true;
        //更新timeMatrixCrowd矩阵
        hAndMin += timeMatrix[preTCrowd[u]][u];
        update(g);
        for (int k = 0; k < n; ++k) {
            if(!s[k] && timeMatrixCrowd[u][k] != INT16_MAX){
                if(tCrowd[u] + timeMatrixCrowd[u][k] < tCrowd[k]){
                    tCrowd[k] = tCrowd[u] + timeMatrixCrowd[u][k];
                    preTCrowd[k] = u;
                }
            }
        }
    }
    int a = end;
    double pathDis = 0;
    while(a != start){
        sTimeCrowd.push(a);
        pathDis += matrix[preTCrowd[a]][a];
        a = preTCrowd[a];
    }
    sTimeCrowd.push(start);
    priceTimeCrowd = price(pathDis);
}


//打印最短加权时间路径
void printTimeCrowd(Graph g,int start,int end){
    int a = end;
    double pathDis = 0;
    double pathTime = 0;
    int changeTime = 0;
    while(a != start) {
        pathDis += matrix[preTCrowd[a]][a];
        pathTime += timeMatrix[preTCrowd[a]][a];
        if (g.NodeList[preTCrowd[a]].name == g.NodeList[a].name) {
            changeTime++;
        }
        a = preTCrowd[a];
    }
    
    cout<<"From: "<<g.NodeList[start].name<<" To: "<<g.NodeList[end].name<<endl<<"(考虑拥挤度的最短加权时间："<<setiosflags(ios::fixed)<<setprecision(2)<<tCrowd[end]<<"分钟,实际时间共计"<<pathTime<<"分钟,总路程共计"<<pathDis<<"公里,票价共计"<<priceTimeCrowd<<"元,换乘次数共计"<<changeTime<<"次)"<<endl;
    printStack(g,sTimeCrowd);
}


//导航询问系统
void navigation(Graph g){
    setTime();
    double startTime = hAndMin;
    int startStation;
    int endStation;
    int startLine;
    int endLine;
    while(true){
        cout<<"请输入起始线路：";
        cin>>startLine;
        ofs<<startLine<<endl;
        if(!existLine(startLine)){
            cout<<"输入线路有误！请重新输入。"<<endl;
            continue;
        }else{
            break;
        }
    }
    while(true){
        cout<<"请输入起始站序号：";
        cin>>startStation;
        ofs<<startStation<<endl;
        if(startStation < 1 || startStation > stations[LineToIndex(startLine)]){
            cout<<"输入起始站序号有误！请重新输入。"<<endl;
            continue;
        }else{
            break;
        }
    }
    while(true){
        cout<<"请输入终点线路：";
        cin>>endLine;
        ofs<<endLine<<endl;
        if(!existLine(endLine)){
            cout<<"输入线路有误！请重新输入。"<<endl;
            continue;
        }else{
            break;
        }
    }
    while(true){
        cout<<"请输入终点站序号：";
        cin>>endStation;
        ofs<<endStation<<endl;
        if(endStation < 1 || endStation > stations[LineToIndex(endLine)]){
            cout<<"输入终点站序号有误！请重新输入。"<<endl;
            continue;
        }else{
            break;
        }
    }
    int startIndex = Number(LineToIndex(startLine), startStation - 1);
    int endIndex = Number(LineToIndex(endLine), endStation - 1);

    char in;
    while(true){
        cout<<"请问是否将拥挤度作为因素进行时间综合评估？(是:Y/y,否:N/n)"<<endl;
        cin>>in;
        ofs<<in<<endl;
        if(in != 'y' && in != 'n' && in != 'Y' && in != 'N'){
            cout<<"输入选项有误！请重新输入"<<endl;
        }
        else{
            break;
        }
    }
    d[startIndex] = 0;
    dc[startIndex] = 0;
    t[startIndex] = 0;
    tCrowd[startIndex] = 0;
    for (int j = 0; j < MAX_NUM && j != startIndex; ++j) {
        d[j] = INT16_MAX;
        dc[j] = INT16_MAX;
        t[j] = INT16_MAX;
        tCrowd[j] = INT16_MAX;
    }
    considerCrowd = (in == 'y' || in == 'Y');
    if(!considerCrowd) {
        //求最短时间路径
        dijkstraTime(g, startIndex, endIndex);
        //求最短长度路径
        dijkstra(g, startIndex, endIndex);
        //求最少换线次数路径
        dijkstraChange(g, startIndex, endIndex);
        if (hAndMin + t[endIndex] > 23 * 60 || hAndMin < 6 * 60) {
            cout << "由于时间问题，地铁停运无法到达目的地！" << endl;
        } else {
            cout << "------------------------------------------------------------------------------------" << endl;
            cout<<"第一条推荐路线："<<endl;
            printDis(g, startIndex, endIndex);
            cout << "------------------------------------------------------------------------------------" << endl;
            cout<<"第二条推荐路线："<<endl;
            if(priceTime < priceChange){
                printTime(g, startIndex, endIndex);
                cout << "------------------------------------------------------------------------------------" << endl;
                cout<<"第三条推荐路线："<<endl;
                printChange(g, startIndex, endIndex);
            }else{
                printChange(g, startIndex, endIndex);
                cout << "------------------------------------------------------------------------------------" << endl;
                cout<<"第三条推荐路线："<<endl;
                printTime(g, startIndex, endIndex);
            }
            cout << "------------------------------------------------------------------------------------" << endl;
        }
    }else{
        char in2;
        while(true){
            cout<<"请问是否自己配置当前时间拥挤度？(是:Y/y,否:N/n)"<<endl;
            cin>>in2;
            ofs<<in2<<endl;
            if(in2 != 'y' && in2 != 'n' && in2 != 'Y' && in2 != 'N'){
                cout<<"输入选项有误！请重新输入"<<endl;
            }
            else{
                break;
            }
        }
        ChangeCrowd = (in2 == 'y' || in2 == 'Y');
        if(ChangeCrowd) {
            while (true){
                int line;
                char in3;
                cout<<"请输入配置拥挤度的线路：";
                cin>>line;
                ofs<<line<<endl;
                if(!existLine(line)){
                    cout<<"输入线路有误！请重新输入。"<<endl;
                    continue;
                }else{
                    double in4;
                    while(true){
                        cout<<"请输入要配置的拥挤度值（0~1）：";
                        cin>>in4;
                        ofs<<in4<<endl;
                        if(in4 < 0 || in4 > 1){
                            cout<<"输入拥挤度有误！请重新输入。"<<endl;
                            continue;
                        }else{
                            break;
                        }
                    }
                    for (int i = 0; i < 3; ++i) {
                        crowds[LineToIndex(line)][i] = in4;
                    }
                    while (true){
                        cout<<"是否继续配置拥挤度？(是:Y/y,否:N/n)";
                        cin>>in3;
                        ofs<<in3<<endl;
                        if(in3 != 'y' && in3 != 'n' && in3 != 'Y' && in3 != 'N'){
                            cout<<"输入选项有误！请重新输入"<<endl;
                        }
                        else{
                            break;
                        }
                    }
                    if(in3 == 'N' || in3 == 'n'){
                        break;
                    }else{
                        continue;
                    }
                }
            }
        }
        char in5;
        while(true){
            cout<<"是否选择避开特定拥挤度线路？(是:Y/y,否:N/n)";
            cin>>in5;
            ofs<<in5<<endl;
            if(in5 != 'y' && in5 != 'n' && in5 != 'Y' && in5 != 'N'){
                cout<<"输入选项有误！请重新输入。"<<endl;
            }else{
                break;
            }
        }
        avoid = (in5 == 'y' || in5 =='Y');
        if(avoid){
            while (true){
                cout<<"请输入设置的拥挤度下限(0~1)：";
                cin>>setedCrowd;
                ofs<<setedCrowd<<endl;
                if(setedCrowd > 1 || setedCrowd < 0){
                    cout<<"输入数据有误！请重新输入。"<<endl;
                }else{
                    break;
                }
            }

        }
        tCrowd[startIndex] = 0;
        for (int i = 0; i < MAX_NUM && i != startIndex; ++i) {
            tCrowd[i] = INT16_MAX;
        }
        //更新初始时间考虑拥挤度的时间邻接矩阵
        //更新矩阵会更改hAndMin 需要设置一个startTime将其改回来
        startTime = hAndMin;
        update(g);
        dijkstraTimeCrowd(g, startIndex, endIndex);
        hAndMin = startTime;
        dijkstra(g, startIndex, endIndex);
        if (startTime + tCrowd[endIndex] > 23 * 60 || startTime < 6 * 60) {
            cout << "由于时间问题，地铁停运无法到达目的地！" << endl;
        } else {
            cout << "------------------------------------------------------------------------------------" << endl;
            cout << "第一条推荐路线:" << endl;
            printTimeCrowd(g, startIndex, endIndex);
            hAndMin = startTime;
            cout << "整个乘车期间的拥挤程度为：" << pathCrowd(g, sTimeCrowd, hAndMin) * 100 << "%" << endl;
            hAndMin = startTime;
            cout << "------------------------------------------------------------------------------------" << endl;
            if(pathWeightedTime(g,sDis) < INT16_MAX){
                hAndMin = startTime;
                cout << "第二条推荐路线：" << endl;
                printDis(g, startIndex, endIndex);
                hAndMin = startTime;
                cout << "------------------------------------------------------------------------------------" << endl;
            }else{
                hAndMin = startTime;
            }

        }
    }
}
#endif //METRO_MAINFUNCTION_H
