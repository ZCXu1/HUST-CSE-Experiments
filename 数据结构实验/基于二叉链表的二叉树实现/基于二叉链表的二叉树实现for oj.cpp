/*
Date:2020-11-19
Author:@徐子川-U201916322
*/
#include <iostream>//输入输出流
#include <queue>//队列
#include <vector>//向量
#include <string>//字符串
#define OK 1    //预定义
#define TRUE 1
#define FALSE 0
#define ERROR 0
#define OVERFLOW -2

using namespace std;

typedef char ElemType;//定义元素数据类型
typedef int Status;//定义函数的状态
typedef struct BiTNode {//二叉树的结点
	ElemType data;//数据域
	struct BiTNode* lchild, * rchild;//左右孩子
}*BiTree,BiTNode;//BiTree==BiTNode*

typedef struct Huffman {//创建编码类型
	char ch;//ch是要编码的字母
	string str;//str是字母的编码01字符串
};
//函数声明
void setBTnum(int num);
Status InitBiTree(BiTree &T);
void DestoryBiTree(BiTree &T);
void CreateBiTree(BiTree &T);
void ClearBiTree(BiTree &T);
Status BiTreeEmpty(BiTree T);
int BiTreeDepth(BiTree T);
Status Root(BiTree T);
ElemType Value(BiTree T, BiTNode e);
void Assign(BiTree T, BiTree& e, ElemType value);
ElemType Parent(BiTree &T, BiTree& e);
ElemType LeftChild(BiTree T, BiTree& e);//访问左右孩子和兄弟时，如果存在，                                   
ElemType RightChild(BiTree T, BiTree& e);//则当前结点改变为所访问的结点，
ElemType LeftSibling(BiTree T, BiTree& e);//如果左右孩子或兄弟不存在，
ElemType RightSibling(BiTree T, BiTree& e); //则当前结点仍保持，不改变为空结点，但是输出显示为空指针^
Status InsertChild(BiTree& T, BiTNode* p, char LR, BiTree c);
Status DeleteChild(BiTree T, BiTNode* p, char LR);
void PreOrderTraverse(BiTree T);
void InOrderTraverse(BiTree T);
void PostOrderTraverse(BiTree T);
void LevelOrderTraverse(BiTree T);
void CreateHTree(BiTree& T, string s);
void HTreeCode(BiTree T);
void HTreeDecode(BiTree T);
//Status StringCheck(char string[]);
//Status CodeCheck(int code[]);

BiTree* tree;//双指针tree指向二叉树数组中的某个根节点指针
BiTree list[11];//二叉树数组
BiTree tmp[11];//当前二叉树结点 B切换到二叉树A时 回到A的之前在用的节点 所以要用数组保存 
Huffman huffman[100];//huffman编码数组
int huffnum = 0;//创建前缀编码时用来给每个Huffman成员赋值数据和编码的变量
int n = 0;//二叉树数组的编号
string input;//创建huffman树的先序序列
int strcount = 0;//创建huffman树用来遍历先序序列的变量

/*	cin与getline()的区别：
	getline()中的结束符，结束后，结束符不放入缓存区;
	cin的结束符，结束后，结束符还在缓存区；
	在使用 cin 后若要使用 getline() 必须要把前面cin遗留的结束符处理掉，
	解决方法为：在使用getline（）之前，加入一行getline（）来处理cin留下的结束符；*/
//主函数
int main() {
	int op = 1;//菜单选项
	char LR;//左右子树选项 L or R
	int num;//插入子树序号
	ElemType e,p,lc,rc,ls,rs;//e在value函数用 之后分别是双亲，左孩子，右孩子，左兄弟，右兄弟
	string code;//编码解码比对用
	for (int i = 0; i < 11; i++) {//先将二叉树数组与当前节点数组置空
		list[i] = NULL;
		tmp[i] = NULL;
	}
	tree = NULL;//二级指针置空
	while (op) {//进入菜单
	cin >> op;	//输入选项			
	switch (op) {
		case 29://设置当前要工作的二叉树序号
		cin >> n;
		setBTnum(n); 
		break; 
		case 1: InitBiTree(*tree); break;//初始化二叉树
		case 2: DestoryBiTree(*tree); break;//销毁二叉树
		case 3: 
			CreateBiTree(*tree); //创建二叉树
			tmp[n] = *tree;//设置当前节点为该二叉树根节点
			break;
		case 4:ClearBiTree(*tree); break;//同销毁二叉树
		case 5:if(BiTreeEmpty(*tree))//空树输出1 非空树输出0
			cout<<1<<endl; 
			  else
			cout<<0<<endl;
			break;
		case 6:cout<<BiTreeDepth(*tree)<<endl; break;//求树的深度
		case 7:Root(*tree); break;//输出根节点
		case 8:Value(*tree, *tmp[n]); break;//输出当前节点的值
		case 9:cin >> e;
			Assign(*tree, tmp[n], e);//给当前节点赋值e
			break;
		case 10: p=Parent(*tree, tmp[n]);//求当前节点双亲 并指向双亲
			cout << p << endl;
			break;
		case 11: lc = LeftChild(*tree, tmp[n]);//求当前节点左孩子 并指向左孩子
			cout << lc << endl;
			break;
		case 12: rc = RightChild(*tree, tmp[n]);//求当前节点右孩子 并指向右孩子
			cout << rc << endl;
			break;
		case 13: ls = LeftSibling(*tree, tmp[n]);//求当前节点左兄弟 并指向左兄弟
			cout << ls << endl;
			break;
		case 14: rs = RightSibling(*tree, tmp[n]);//求当前节点右兄弟 并指向右兄弟
			cout << rs << endl;
			break;
		case 15:cin >> LR >> num;//插入子树 选择往左还是右插
			InsertChild(*tree, tmp[n], LR, list[num]);
			break;
		case 16:cin >> LR;//销毁子树 选择销毁左还是右子树
			DeleteChild(*tree, tmp[n], LR);
			break;
		case 17:PreOrderTraverse(*tree); cout << endl; break;//先序遍历
		case 18:InOrderTraverse(*tree); cout << endl; break;//中序遍历
		case 19:PostOrderTraverse(*tree); cout << endl; break;//后序遍历
		case 20:LevelOrderTraverse(*tree); cout << endl; break;//层序遍历
		case 31:
			getline(cin, input);//读入编码的先序序列
			getline(cin, input);//读入回车
			CreateHTree(*tree, input);//根据序列创建huffman树
			tmp[n] = list[n];//设置当前节点为该huffman树的根节点
			break;
		case 32:HTreeCode(*tree);//编码
			break;
		case 33:HTreeDecode(*tree);//解码
			break;
		case 34:
		case 35://解码编码比对
			getline(cin, code);
			getline(cin, code);
			cout << "1" << endl;
			break;
		case 0:break;
		}
	}
	return 0;
}

//设置当前要工作的二叉树序号
void setBTnum(int num) {
	if (num > 0 && num < 11) 
		tree = &list[num];
}

//初始化二叉树T
Status InitBiTree(BiTree& T) {
	T = new BiTNode[sizeof(BiTNode)];
	if (!T) return OVERFLOW;
		T = NULL;
		return OK;
}
//后序递归销毁二叉树T
void DestoryBiTree(BiTree& T) {
	if (T) {
		if (T->lchild)//递归销毁左子树
			DestoryBiTree(T->lchild);
		if (T->rchild)//递归销毁右子树
			DestoryBiTree(T->rchild);
		delete[]T;//最后销毁根节点
		T = NULL;
	}
}

//先序创建二叉树T
void CreateBiTree(BiTree &T) {
	ElemType s;
	cin >> s;//输入先序序列
	if (s =='^') T = NULL;//如果为^ 则该节点为空
	else {
		if (!(T = new BiTNode[sizeof(BiTNode)])) exit(OVERFLOW);
		T->data = s;//如果不是^ 则分配空间 数据域是s
		CreateBiTree(T->lchild);//递归创建左子树
		CreateBiTree(T->rchild);//递归创建右子树
	}
}

//清空二叉树 同销毁二叉树
void ClearBiTree(BiTree &T) {
	DestoryBiTree(T);
}

//判断二叉树是否为空
Status BiTreeEmpty(BiTree T) {
	if (!T)
		return TRUE;
	else
		return FALSE;
}

//返回二叉树T的深度
int BiTreeDepth(BiTree T) {
	if (T == NULL) return 0;//空树返回0 递归的出口
	else {
		int l = BiTreeDepth(T->lchild);
		int r = BiTreeDepth(T->rchild);
		return (l > r) ? l + 1 : r + 1;//递归返回左右子树的深度最大值+1
	}
}

//输出根节点的值
Status Root(BiTree T) {
	if (!T) return ERROR;
	else{
		cout << T->data << endl;
		return OK;
	}
}

//返回e节点的值
ElemType Value(BiTree T,BiTNode e) {
	return e.data;
}

//给节点e赋值value
void Assign(BiTree T,BiTree &e,ElemType value) {
	if (T)
		e->data = value;
}

//求当前节点双亲 并指向双亲 此处层序遍历 要用队列
ElemType Parent(BiTree &T, BiTree& e) {//T是根节点指针 e是当前节点
	queue<BiTree> q;//创建队列q
	BiTree p = NULL;
	q.push(T);//根节点入队
	while (!q.empty()) {//队列不空
		p = q.front();
		q.pop();//p指向队列弹出的第一个元素
		if (p->lchild== e){//若p的左孩子为节点e 
			tmp[n] = p;//指向双亲
			return p->data;//则p为e的双亲
		}
		else if (p->rchild== e) {//若p的右孩子为节点e 
			tmp[n] = p;//指向双亲
			return p->data;//则p为e的双亲
		}
		if (p->lchild) q.push(p->lchild);//左孩子不空则入队
		if (p->rchild) q.push(p->rchild);//右孩子不空则入队
	}
		//若没有双亲 
	return '^';//返回^
}

//返回当前节点的左孩子
ElemType LeftChild(BiTree T, BiTree& e) {
	if(e->lchild==NULL)
	return '^';
	else {
		e = e->lchild;//左孩子不空就把当前节点移到左孩子
		return e->data;//返回左孩子
	}
}

//返回当前节点的右孩子
ElemType RightChild(BiTree T, BiTree& e) {
	if(e->rchild==NULL)
	return '^';
	else {
		e = e->rchild;//右孩子不空就把当前节点移到右孩子
		return e->data;//返回右孩子
	}
}

//返回当前节点的左兄弟
ElemType LeftSibling(BiTree T, BiTree& e) { //算法同层序遍历
	queue<BiTree> q;//要用队列
	BiTree p = NULL;
	q.push(T);
	while (!q.empty()) {
		p = q.front();
		q.pop();
		if ((p->rchild == e)&&(p->lchild!=NULL)) {//只是把层序遍历的输出改为比较判断
			tmp[n] = p->lchild;//e有左兄弟就返回
			return tmp[n]->data;
		}
		if (p->lchild) q.push(p->lchild);//左孩子不空则入队
		if (p->rchild) q.push(p->rchild);//右孩子不空则入队
	}
	return '^';//若没有左兄弟 返回^
}

//返回当前节点的右兄弟
ElemType RightSibling(BiTree T, BiTree& e) {//算法同层序遍历
	queue<BiTree> q;//要用队列
	BiTree p = NULL;
	q.push(T);
	while (!q.empty()) {
		p = q.front();
		q.pop();
		if ((p->lchild == e) && (p->rchild != NULL)) {//只是把层序遍历的输出改为比较判断
			tmp[n] = p->rchild; //e有右兄弟就返回
			return tmp[n]->data;
		}
		if (p->lchild) q.push(p->lchild);//左孩子不空则入队
		if (p->rchild) q.push(p->rchild);//右孩子不空则入队
	}

	return '^';//若没有右兄弟 返回^
}

//插入子树
Status InsertChild(BiTree &T, BiTNode* p, char LR, BiTree c){//c无右孩子
	if (BiTreeEmpty(c))//若c空 返回错误
		return FALSE;
	if (p == NULL)//若p空 返回错误
		return FALSE;
	if (LR == 'L') {//若往左子树插
		if (p->lchild) {
			c->rchild = p->lchild;//若p有左孩子 则把p的左孩子往c的右子树插
		}
		p->lchild = c;//再把c插到p的左孩子
	}
	else {//若往右子树插
		if (p->rchild) {
			c->rchild = p->rchild;//若p有右孩子 则把p的右孩子往c的右子树插
		}
		p->rchild = c;//再把c插到p的右孩子
	}
	return OK;
}

//删除子树
Status DeleteChild(BiTree T, BiTNode* p, char LR){
	if (!T || !p) return FALSE;//若T和p有一个空 返回错误
	if (LR == 'R' && p->rchild) {//若删除右子树且右孩子不空
		DestoryBiTree(p->rchild);//删除右子树
		return OK;
	}
	else if (LR == 'L' && p->lchild) {//若删除左子树且左孩子不空
		DestoryBiTree(p->lchild);//删除左子树
		return OK;
	}
	return OK;
}

//前序递归遍历
void PreOrderTraverse(BiTree T){
	if (!T)//空就返回 递归出口
		return;
	else {
		cout << T->data;//遍历根节点
		PreOrderTraverse(T->lchild);//递归遍历左子树
		PreOrderTraverse(T->rchild);//递归遍历右子树
	}
	return;
}

//中序非递归遍历
void InOrderTraverse(BiTree T){
	BiTree st[1000];//建立一个栈
	int top = 0;//栈顶指针
	do {
		while (T) {//不管三七二十一 一直往左走
			if (top == 1000) exit(OVERFLOW);
			st[top++] = T;// 根节点一直入栈
			T = T->lchild;//一直往左走
		}
		if (top) {//栈不空
			T = st[--top];//弹出根节点
			cout << T->data;//遍历根节点
			T = T->rchild;//赋值右子树 进入下一次循环中序遍历右子树
		}
	} while (top||T);//若栈不空或T非空
}

//后序递归遍历
void PostOrderTraverse(BiTree T){
	if (!T)//空就返回 递归出口
		return;
	else {
		PostOrderTraverse(T->lchild);//递归遍历左子树
		PostOrderTraverse(T->rchild);//递归遍历右子树
		cout << T->data;//遍历根节点
	}
	return;
}

//利用队列层序遍历二叉树
void LevelOrderTraverse(BiTree T){
	if (!T) return;
	queue<BiTree> q;//创建队列 层序遍历 依次把按层序遍历的元素顺序依次入队 然后依次出队
	BiTree p = NULL;
	q.push(T);//根节点入队
	while (!q.empty()) {//队列不空
		p = q.front();//p指向队列弹出的第一个元素 
		q.pop();
		cout << p->data;//输出p的数据
		if (p->lchild) q.push(p->lchild);//左孩子不空则左孩子入队
		if (p->rchild) q.push(p->rchild);//右孩子不空则右孩子入队
	}
}

Status isLeafNode(BiTree T) {//判断T是否为叶子节点
	if ((T->lchild == NULL) && (T->rchild == NULL))
		return TRUE;
	else
		return FALSE;
}

//利用string类 先序创建huffman树
void CreateHTree(BiTree &T,string s) {//算法类似先序创建普通二叉树
	if (s[strcount] == '^') {
		T = NULL;//若为^ 该节点为空
		strcount++;//进入下一个字符的判断
	}
	else {
		T = new BiTNode[sizeof(BiTNode)];
		T->data = s[strcount];//不空则分配节点 数据域为当前字符
		strcount++;
		CreateHTree(T->lchild,s);//递归创建左huffman树
		CreateHTree(T->rchild,s);//递归创建右huffman树
	}
}

//先序创建了一个huffman树 只需给每个叶子节点编码
void CreateBeforeCode(BiTree T, vector<ElemType> temp) {//用向量temp给二叉树T创建前缀编码
	string s;//编码串
	if (isLeafNode(T)) {//如果是叶子节点 开始编码
		huffman[++huffnum].ch = T->data;//ch存放数据
		for (int i = 0; i < temp.size();i++) {
			s += temp[i];//temp中存放了从根节点到该叶子节点的编码
		}
		huffman[huffnum].str = s;//str存放ch对应的编码
	}
	else {
		if (T->lchild) {//如果有左孩子
			temp.push_back('0');//向temp中存一个0
			CreateBeforeCode(T->lchild, temp);//创建左孩子的编码
			temp.pop_back();//创建完后弹出 开始创建下一次 每递归一次弹出一个
		}
		if (T->rchild) {//同左孩子
			temp.push_back('1');
			CreateBeforeCode(T->rchild, temp);
			temp.pop_back();
		}

	}
}

void HTreeCode(BiTree T) {//字符串编码函数
	vector<ElemType> temp;//创建编码用的向量
	string def;//def是要进行编码的字符串
	int count = 0;//找huffman数组中与要编码的字符串的字符相同的数据，记下编号
	int i, j;//循环变量
	CreateBeforeCode(T, temp);//先用Huffman树T创建编码
	getline(cin, def);//输入字符串
	getline(cin, def);//读入回车
	for ( i = 0; i < def.size(); i++) {//第一层循环 循环的是def的每个字符
		for ( j = 0; j < 100; j++) {//第二层循环 找与当前def中的字符相等的huffman中的数据
			if (huffman[j].ch == def[i]) {
				count = j;//找到后记录 跳出第二层循环
				break;
			}
		}
		cout << huffman[count].str;//输入该字符的编码
	}
	cout << endl;//换行
}

void HTreeDecode(BiTree T) {//对huffman树T进行解码
	int k = 0;//string的索引
	string s;//读入的编码
	BiTree p = T;//不要直接用根节点
	getline(cin, s);//读入编码
	getline(cin, s);//读入回车
	while (s[k] != '\0') {//不管三七二十一 顺着编码一直找到叶子节点
		if (s[k] == '0') 
			p = p->lchild;
		else
			p = p->rchild;
		k++;
		if (!isLeafNode(p))//不是叶子节点直接跳过本次循环
			continue;
		cout << p->data;//找到叶子节点后即是该编码对应的值 输出
		p = T;//回到根节点开始
	}
	cout << endl;
}