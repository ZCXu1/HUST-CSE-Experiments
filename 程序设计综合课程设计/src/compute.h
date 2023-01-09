/*
 * compute.h
 *
 *  Created on: 2021年3月4日
 */
#ifndef METRO_COMPUTE_H
#define METRO_COMPUTE_H

#include <cmath>


/*票价：按里程分段计价
    4 公里以内（含 4 公里）2 元；
    4 - 12 公里（含 12 公里），1 元 / 4 公里；
    12 - 24 公里（含 24 公里），1 元 / 6 公里；
    24 - 40 公里（含 40 公里），1 元 / 8 公里；
    40 - 50 公里（含 50 公里），1 元 / 10 公里;*/
int price(double distance)
{

    int cost = 0;
    if (distance < 0)
    {
        cost = 0;
    }
    else if (distance <= 4)
    {
        cost = 2;
    }
    else if (distance <= 12)
    {
        cost = 2;
        distance -= 4;
        if (fmod(distance, 4) == 0)
        {
            cost += (int)distance / 4;
        }
        else
        {
            cost += (int)distance / 4 + 1;
        }
    }
    else if (distance <= 24)
    {
        cost = 4;
        distance -= 12;
        if (fmod(distance, 6) == 0)
        {
            cost += (int)distance / 6;
        }
        else
        {
            cost += (int)distance / 6 + 1;
        }
    }
    else if (distance <= 40)
    {
        cost = 6;
        distance -= 24;
        if (fmod(distance, 8) == 0)
        {
            cost += (int)distance / 8;
        }
        else
        {
            cost += (int)distance / 8 + 1;
        }
    }
    else if (distance <= 50)
    {
        cost = 8;
        distance -= 40;
        if (fmod(distance, 10) == 0)
        {
            cost += (int)distance / 10;
        }
        else
        {
            cost += (int)distance / 10 + 1;
        }
    }
    else
    {
        cost = 9;
        distance -= 50;
        if (fmod(distance, 20) == 0)
        {
            cost += (int)distance / 20;
        }
        else
        {
            cost += (int)distance / 20 + 1;
        }
    }
    return cost;
}


//计算id为index的站在time时间的拥挤度
double crowd(Graph g,double time,int index){
    if (g.NodeList[index].atLine == 2){
        return crowds[1][2];
    }
    else if(g.NodeList[index].atLine ==4){
        return crowds[3][2];
    }
    else if(g.NodeList[index].atLine == 6){
        if(time >= 19 * 60 && time <= 22 * 60){
            return crowds[4][0];
        }
        else{
            return crowds[4][2];
        }
    }else if(g.NodeList[index].atLine == 7){
        if(time >= 19 * 60 && time <= 22 * 60){
            return crowds[5][0];
        }
        else{
            return crowds[5][2];
        }
    }
    else if (g.NodeList[index].atLine == 8){
        if (time >= 9 * 30 && time <= 15 * 60){
            return crowds[6][0];
        }
        else{
            return crowds[6][2];
        }
    }
    else if (g.NodeList[index].atLine == 1){
        if(time >= 7 * 60 + 30 && time <= 9 * 60){
            return crowds[0][0];
        }
        else if(time >= 16 * 60 + 30 && time <= 18 * 60 + 30){
            return crowds[0][1];
        }
        else{
            return crowds[0][2];
        }
    }
    else if(g.NodeList[index].atLine == 3){
        if(time >= 7 * 60 + 30 && time <= 9 * 60){
            return crowds[2][0];
        }
        else if(time >= 16 * 60 + 30 && time <= 18 * 60 + 30){
            return crowds[2][1];
        }
        else{
            return crowds[2][2];
        }
    }
    return 0;
}


//综合各站点间地铁车厢人员拥挤情况，得到整个乘车期间的拥挤程度；
//路径储存于栈s中 栈顶为起点
double pathCrowd(Graph g,stack<int> s,double time){
    double totalCrowd = 0;
    int n = s.size();
    //使用map记录换乘站的上一个id
    map<string,int> name;
    //不考虑目的站的拥挤度
    //因为乘车时的拥挤度认为是上车时的拥挤度
    while (s.size() > 1){
        int a = s.top();
        //如果a站不是换乘站 或者a站是换乘站但是在此站下车进行换乘 拥挤度要换到本站的另一条线算 但是先在此处算上 下面else再减
        if(name.count(g.NodeList[a].name) == 0){
            totalCrowd += crowd(g,time,a);
            name.insert(pair<string,int>(g.NodeList[a].name,a));
        }
        //a站是换乘站的另一条线 在本线上车 把上一个a站的拥挤度减去 加上此时线路a站的拥挤度 再n-- 注意此时的time已经变了 要减掉换线时间
        else{
            totalCrowd -= crowd(g,time - WALK,name.find(g.NodeList[a].name)->second);
            n--;
            totalCrowd += crowd(g,time,a);
            name.insert(pair<string,int>(g.NodeList[a].name,a));
        }
        s.pop();
        int b = s.top();
        time += timeMatrix[a][b];
    }
    return totalCrowd / (n - 1);
}


/*
 * 一条地铁线路的拥挤程度在某段时间内认为是相同的，20%以下为宽松，
 * 可以找到座位，50%为一般拥挤程度，75%为拥挤，不能超过 100%；
 * 约定宽松情况下，乘车时间系数为 0.6，拥挤情况下时间系数为 1.5，
 * 一般拥挤程度为1.2，宽松以上但不到一般拥挤为0.8
 */
//根据拥挤度返回时间系数
double timeRatio(double crowd,bool avoid,double setCrowd){
    if(!avoid) {
        if (crowd < 0.2) {
            return 0.6;
        } else if (crowd < 0.5) {
            return 0.8;
        } else if (crowd < 0.75) {
            return 1.2;
        } else if (crowd <= 1) {
            return 1.5;
        } else {
            return INT16_MAX;
        }
    }else{
        if(crowd >= setCrowd){
            return INT16_MAX;
        }else{
            if (crowd < 0.2) {
                return 0.6;
            } else if (crowd < 0.5) {
                return 0.8;
            } else if (crowd < 0.75) {
                return 1.2;
            } else {
                return 1.5;
            }
        }
    }
}


#endif //METRO_COMPUTE_H
