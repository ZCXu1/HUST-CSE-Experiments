#include<iostream>
#include<list>
#include<map>
#include<vector>
using namespace std;
#define pageTotalSize 2000
#define pageSize 10
#define pageFrameNum 3
#define seqLength 100
int page[pageTotalSize], pageFrame[pageFrameNum][pageSize];
int orderedSeq[seqLength] = { 100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
	110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
	120, 121, 122, 123, 124, 125, 126, 127, 128, 129,
	130, 131, 132, 133, 134, 135, 136, 137, 138, 139,
	140, 141, 142, 143, 144, 145, 146, 147, 148, 149,
	150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
	160, 161, 162, 163, 164, 165, 166, 167, 168, 169,
	170, 171, 172, 173, 174, 175, 176, 177, 178, 179,
	180, 181, 182, 183, 184, 185, 186, 187, 188, 189,
	190, 191, 192, 193, 194, 195, 196, 197, 198, 199 };
int skipSeq[seqLength] = { 682, 683, 684, 685, 686, 687, 688, 689, 690, 691,
	1625, 1626, 1627, 1628, 1629, 1630, 1631, 1632, 1633,1634,
	443, 444, 445, 446, 447, 448, 449, 450, 451, 452,
	739, 740, 741, 742, 743, 744, 745, 746, 747, 748,
	218, 219, 220, 221, 222, 223, 224, 225, 226, 227,
	964, 965, 966, 967, 968, 969, 970, 971, 972, 973,
	1223, 1224, 1225, 1226, 1227, 1228, 1229, 1230, 1231, 1232,
	495, 496, 497, 498, 499, 500, 501, 502, 503, 504,
	1135, 1136, 1137, 1138, 1139, 1140, 1141, 1142, 1143, 1144,
	1883, 1884, 1885, 1886, 1887, 1888, 1889, 1890, 1891, 1892 };
int loopSeq[seqLength] = { 218, 219, 220, 221, 222, 223, 224, 225, 226, 227,
	218,219, 220, 221, 222, 223, 224, 225, 226, 227,
	218, 219, 220, 221, 222, 223, 224, 225, 226, 227,
	218, 219, 220, 221, 222, 223, 224, 225, 226, 227,
	218, 219, 220, 221, 222, 223, 224, 225, 226, 227,
	443, 444, 445, 446, 447, 448, 449, 450, 451, 452,
	443, 444, 445, 446, 447, 448, 449, 450, 451, 452,
	443, 444, 445, 446, 447, 448, 449, 450, 451, 452,
	443, 444, 445, 446, 447, 448, 449, 450, 451, 452,
	443, 444, 445, 446, 447, 448, 449, 450, 451, 452, };
int randomSeq[seqLength] = { 1487, 1841, 1767, 1057, 1943, 646, 380, 1886, 1187, 123,
	318, 93, 1373, 1604, 760, 714, 163, 147, 1277, 1744,
	1236, 1149, 231, 869, 1818, 382, 237, 833, 1128, 605,
	1389, 1608, 262, 127, 1316, 1082, 283, 329, 1555, 1722,
	1224, 1786, 1200, 731, 1427, 169, 1666, 764, 68, 491,
	1879, 1004, 856, 1262, 1321, 1741, 932, 1323, 1763, 1923,
	514, 109, 1987, 1877, 306, 1106, 1580, 1930, 513, 1358,
	648, 1778, 1513, 1176, 1696, 42, 1709, 322, 818, 178,
	1524, 1649, 1725, 992, 1667, 825, 568, 1759, 1143, 1461,
	405, 10, 1394, 401, 1832, 1792, 1619, 865, 755, 689 };
void FIFO(int seq[seqLength], bool showDetail) {
	int missTimes = 0, flag = 0, temp1, temp2;
	double missRate = 0;
	list<pair<int, int> > pageTable;	// 页表，first保存页号，second保存页框号
	for (int i = 0; i < seqLength; i++) {
		flag = 0;
		for (auto x : pageTable) {
			if (seq[i] >= x.first && seq[i] < x.first + 10) {	// 说明需要访问的元素在页框中
				flag = 1;
				if(showDetail)
					printf("在页框%d中取得位于%d指令:%d\n", x.second, seq[i], pageFrame[x.second][seq[i] % 10]);
				break;
			}
		}
		if (flag == 0) {	// 访问元素不再页框中
			missTimes++;
			if (pageTable.size() == 3) {	// 页表已满，需要淘汰
				temp1 = (*(pageTable.begin())).second;	// 保存删除的位置
				pageTable.erase(pageTable.begin());		// FIFO淘汰算法
			}
			else
				temp1 = pageTable.size();	// 未装满
			if (showDetail)
				printf("在页中取得位于%d指令:%d\n", seq[i], page[seq[i]]);
			temp2 = (seq[i] / 10) * 10;
			for (int j = 0; j < pageSize; j++) {
				pageFrame[temp1][j] = page[temp2 + j];	// 将页面转入页框
			}
			pageTable.push_back(make_pair(temp2, temp1));
		}
		if (showDetail){
			printf("\n--------页表状态--------\n");
			printf("页框号\t\t页面范围\n");
			for (auto x : pageTable) {
				printf("%d\t\t%d-%d\n", x.second, x.first, x.first + 10);
			}
			printf("-----------------------\n\n");
		}
	}
	missRate = ((double)missTimes / 100) * 100;
	printf("缺页次数:%d\t\t缺页率:%.1f%%\n", missTimes, missRate);
}
void LRU(int seq[seqLength], bool showDetail) {
	int missTimes = 0, flag = 0, temp1, temp2;
	double missRate = 0;
	vector<pair<int, int> > pageTable;	// 页表，first保存页号，second保存页框号
	vector<int> ru;
	for (int i = 0; i < seqLength; i++) {
		flag = 0;
		for (auto x : pageTable) {
			if (seq[i] >= x.first && seq[i] < x.first + 10) {	// 说明需要访问的元素在页框中
				flag = 1;
				for (int j = 0; j < ru.size(); j++) {
					if (j == x.second)
						ru[j] = 0;	// LRU算法，访问后置0
					else
						ru[j]++;	// 未访问加1
				}
				if (showDetail)
					printf("在页框%d中取得位于%d指令:%d\n", x.second, seq[i], pageFrame[x.second][seq[i] % 10]);
				break;
			}
		}
		if (flag == 0) {	// 访问元素不再页框中
			missTimes++;
			if (pageTable.size() == 3) {	// 页表已满，需要淘汰
				temp1 = 0;
				temp2 = ru[0];
				for (int j = 1; j < ru.size(); j++) {	// 找到最久未被访问的元素
					if (ru[j] > temp2) {
						temp1 = j;
						temp2 = ru[j];
					}
				}
			}
			else {
				temp1 = pageTable.size();
				pageTable.push_back(make_pair(0, temp1));	// 页号暂未确定
				ru.push_back(0);
			}
			if (showDetail)
				printf("在页中取得位于%d指令:%d\n", seq[i], page[seq[i]]);
			temp2 = (seq[i] / 10) * 10;
			for (int j = 0; j < pageSize; j++) {
				pageFrame[temp1][j] = page[temp2 + j];	// 将页面转入页框
			}
			pageTable[temp1].first = temp2;
		}
		if (showDetail) {
			printf("\n--------页表状态--------\n");
			printf("页框号\t\t页面范围\n");
			for (auto x : pageTable) {
				printf("%d\t\t%d-%d\n", x.second, x.first, x.first + 10);
			}
			printf("-----------------------\n\n");
		}
	}
	missRate = ((double)missTimes / 100) * 100;
	printf("缺页次数:%d\t\t缺页率:%.1f%%\n", missTimes, missRate);
}
void LFU(int seq[seqLength], bool showDetail) {
	int missTimes = 0, flag = 0, temp1, temp2;
	double missRate = 0;
	vector<pair<int, int> > pageTable;	// 页表，first保存页号，second保存页框号
	vector<int> ru;
	for (int i = 0; i < seqLength; i++) {
		flag = 0;
		for (auto x : pageTable) {
			if (seq[i] >= x.first && seq[i] < x.first + 10) {	// 说明需要访问的元素在页框中
				flag = 1;
				for (int j = 0; j < ru.size(); j++) {
					if (j == x.second) {
						ru[j]++;	// LFU算法，访问加1	未访问不变
						break;
					}
				}
				if (showDetail)
					printf("在页框%d中取得位于%d指令:%d\n", x.second, seq[i], pageFrame[x.second][seq[i] % 10]);
				break;
			}
		}
		if (flag == 0) {	// 访问元素不再页框中
			missTimes++;
			if (pageTable.size() == 3) {	// 页表已满，需要淘汰
				temp1 = 0;
				temp2 = ru[0];
				for (int j = 1; j < ru.size(); j++) {	// 找到访问次数最少的元素
					if (ru[j] < temp2) {
						temp1 = j;
						temp2 = ru[j];
					}
				}
			}
			else {
				temp1 = pageTable.size();
				pageTable.push_back(make_pair(0, temp1));	// 页号暂未确定
				ru.push_back(0);
			}
			if (showDetail)
				printf("在页中取得位于%d指令:%d\n", seq[i], page[seq[i]]);
			temp2 = (seq[i] / 10) * 10;
			for (int j = 0; j < pageSize; j++) {
				pageFrame[temp1][j] = page[temp2 + j];	// 将页面转入页框
			}
			pageTable[temp1].first = temp2;
		}
		if (showDetail) {
			printf("\n--------页表状态--------\n");
			printf("页框号\t\t页面范围\n");
			for (auto x : pageTable) {
				printf("%d\t\t%d-%d\n", x.second, x.first, x.first + 10);
			}
			printf("-----------------------\n\n");
		}
	}
	missRate = ((double)missTimes / 100) * 100;
	printf("缺页次数:%d\t\t缺页率:%.1f%%\n", missTimes, missRate);
}
int main() {
	// 将页表中的数据初始化为随机数
	for (int i = 0; i < pageTotalSize; i++) {
		page[i] = rand();
	}
	printf("----FIFO淘汰算法-----\n");
	printf("顺序结构：");
	FIFO(orderedSeq, false);
	printf("跳转结构：");
	FIFO(skipSeq, false);
	printf("循环结构：");
	FIFO(loopSeq, false);
	printf("随机结构：");
	FIFO(randomSeq, false);

	printf("----LRU淘汰算法-----\n");
	printf("顺序结构：");
	LRU(orderedSeq, false);
	printf("跳转结构：");
	LRU(skipSeq, false);
	printf("循环结构：");
	LRU(loopSeq, false);
	printf("随机结构：");
	LRU(randomSeq, false);

	printf("----LFU淘汰算法-----\n");
	printf("顺序结构：");
	LFU(orderedSeq, false);
	printf("跳转结构：");
	LFU(skipSeq, false);
	printf("循环结构：");
	LFU(loopSeq, false);
	printf("随机结构：");
	LFU(randomSeq, false);
	return 0;
}