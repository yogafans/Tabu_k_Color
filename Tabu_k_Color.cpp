// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include<math.h>
#include<fstream>
#include<string>
#include <cstdlib>
#include <ctime>
#define random(x) rand()%(x)
using namespace std;

int equal_count = 0;//相等的解计数
int **equal_list;//储存相等的解
int best_fs;//最好的fs
int N;//顶点数
int K;//颜色数
int select_vex;//移动选择的顶点
int select_color;//移动选择的颜色
int bestdelt=10000;//最有delt
int adj_count[1000];//储存每个顶点相邻顶点的数量
//int m = 1;


class k_Color {
	int n;//顶点个数
	int k;//颜色种类
	int fs;//冲突边数
	int **Adjacent_Color_Table;//邻接颜色表
	int **TabuTenure;//禁忌周期表
	int **edge;//顶点关系表
	int *x;//着色解法
	int **vex_adj;//储存每个节点的邻接节点
public:
	long iter = 0;
	k_Color(int vexc, int colorC) {//构造函数
		n = vexc;
		k = colorC;
		fs = 0;
		edge = new int*[n];
		Adjacent_Color_Table= new int*[n];
		TabuTenure = new int*[n];
		vex_adj = new int*[n];
		x = new int[n];
		equal_list = new int*[n*(k - 1)];
		for (int i = 0; i < n*(k - 1); i++) 
			equal_list[i] = new int[2];
		for (int i = 0; i < n; i++) {
			edge[i] = new int[n];
			for (int j = 0; j < n; j++)
				edge[i][j] = 0;
		}
		for (int i = 0; i < n; i++) {
			Adjacent_Color_Table[i] = new int[k];
			for (int j = 0; j < k; j++)
				Adjacent_Color_Table[i][j] = 0;
		}
		for (int i = 0; i < n; i++) {
			TabuTenure[i] = new int[k];
			for (int j = 0; j < k; j++)
				TabuTenure[i][j] = 0;
		}
	}
	void addEdge(int vex1, int vex2) {//添加边关系
		edge[vex1-1][vex2-1] = 1;
		edge[vex2 - 1][vex1 - 1] = 1;
	}
	void display() {//展示一些表
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++)
				cout << edge[i][j] << ",";
			cout << endl;
		}
		cout << "解为：" << endl;
		for (int i = 0; i < n; i++) {
			cout << x[i] << ",";
		}
		cout << "顶点关系为:"<<endl;
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < adj_count[i]; j++) {
				cout << vex_adj[i][j] << ",";
			}
			cout << endl;
		}
		cout << endl;
	}
	bool fsTrue() {//判断fs是否大于0
		if (fs > 0 )
			return true;
		else
			return false;
	}
	void release() {//释放申请的动态数组
		for (int i = 0; i < n; i++) {
			delete[] Adjacent_Color_Table[i];
			delete[] TabuTenure[i];
			delete[] edge[i];
			delete[] vex_adj[i];
		}
		for (int i = 0; i < n*(k - 1); i++)
			delete[] equal_list[i];
		delete[] x;
		delete[] Adjacent_Color_Table;
		delete[] TabuTenure;
		delete[] edge;
		delete[] vex_adj;
		delete[] equal_list;
	}
	void initial();
	void caculateFs();
	void updateACTable();
	void findmove();
	void makemove();
};

void k_Color::initial() {//随机生成解
	srand((int)time(0));
	int a;
	for (int i = 0; i <n; i++) {
		a = random(k);
		x[i] = a;
		//cout << x[i] << ",";
	}
	//cout << endl;
}

void k_Color::caculateFs() {//计算fs
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < adj_count[i]; j++) {
			if (x[i] ==x[vex_adj[i][j]])
				fs++;
		}
	}
	fs = fs / 2;
	best_fs = fs;
	cout << "fs=" << fs << endl;
}

void k_Color::updateACTable() {//生成Adjacent_Color_Table
	int *count = new int[k];
	for (int i = 0; i < n; i++) {
		int s = 0;
		int store[1000];
		for (int j = 0; j < k; j++)
			count[j] = 0;
		for (int j = 0; j < n; j++) {
			if (edge[i][j]) {
				store[s++]=j;
				for (int h = 0; h < k; h++) {
					if (x[j] == h)
						count[h]++;
				}
			}
		}
		for (int j = 0; j < k; j++)
			Adjacent_Color_Table[i][j] = count[j];
		//cout << "s=" << s << endl;
		adj_count[i] = s;
		vex_adj[i] = new int[s];
		for (int j = 0; j < s; j++) {
			vex_adj[i][j] = store[j];
		}

		/*for (int j = 0; j < s; j++) {
			cout << vex_adj[i][j] << ",";
		}
		cout << endl;*/
	}
	
	/*for (int i = 0; i < n; i++) {
		for (int j = 0; j < k; j++)
			cout << Adjacent_Color_Table[i][j] << ",";
		cout << endl;
		//cout << "ok" << endl;
	}*/
}

void k_Color::findmove() {//寻找最优移动
	bestdelt = 10000;
	int delt;
	for (int i = 0; i < n; i++) {
		if (Adjacent_Color_Table[i][x[i]] > 0) {
			for (int j = 0; j < k; j++) {
				if (j != x[i]) {
					delt = Adjacent_Color_Table[i][j] - Adjacent_Color_Table[i][x[i]];
					//cout << "delt="<<delt << endl;
					if (delt <= bestdelt && (iter > TabuTenure[i][j] || (fs + delt) < best_fs)) {
						if (delt < bestdelt) {
							//cout << "ok3" << endl;
							equal_count = 0;
							bestdelt = delt;
						}
						equal_list[equal_count][0] = i;
						equal_list[equal_count][1] = j;
						equal_count++;
					}
				}
			}
		}
		//else
			//cout << "222" << "i="<<i<<endl;
	}
	srand((int)time(0));
	int t;
	t = random(equal_count);
	/*if (best_fs<10) {
		cout << equal_count << endl;
		cout << "best_fs=" << best_fs << endl;
	}*/
	//cout << "bestdelt1=" << bestdelt << endl;
	select_vex = equal_list[t][0];
	select_color= equal_list[t][1];
}

void k_Color::makemove() {//移动操作
	//cout << "bestdelt=" << bestdelt << endl;
	fs = fs + bestdelt;
	//cout << "fs=" << fs << endl;
	int old_color = x[select_vex];
	x[select_vex] = select_color;
	if (fs < best_fs)
		best_fs = fs;
	srand((int)time(0));
	int t;
	t = random(10);
	TabuTenure[select_vex][old_color] = fs + iter + t;
	for (int i = 0; i < n; i++) {
		if (edge[select_vex][i]) {
			Adjacent_Color_Table[i][old_color]--;
			Adjacent_Color_Table[i][select_color]++;
		}
	}
}

void getN(char *filename) {//获得顶点数目
	ifstream fin;
	fin.open(filename, ios::in);
	char buff;
	int i = 0;
	string s;
	int vexcount = 0;
	while (!fin.eof()) {
		getline(fin, s);
		buff = s[0];
		int j = 0;
		i++;
		if (buff == 'p') {
			string vexc, edgec;
			j += 7;
			buff = s[j];
			while (buff != ' ') {
				vexc += buff;
				buff = s[++j];
			}
			buff = s[++j];
			while (buff != 0) {
				edgec += buff;
				buff = s[++j];
			}
			cout << "edge:" << vexc << "," << edgec << endl;
			N = atoi(vexc.c_str());
			break;
		}
		else
			continue;
	}
	fin.close();
}

void getfile(char *filename,k_Color &a) {//读取边关系
	ifstream fin;
	fin.open(filename,ios::in);
	if (!fin.is_open()) {
		cout << "Can not find target  file." << endl;
		system("pause");
	}
	char buff;
	int i = 0;
	string s;
	while (!fin.eof()) {
		getline(fin, s);
		//cout << s << endl;
		buff = s[0];
		//cout << buff;
		int j = 0;
		i++;
		if (buff == 'c')
		{
			continue;
		}
		if (buff == 'e') {
			string vex1,vex2;
			j += 2;
			buff = s[j];
			while(buff!=' '){
				vex1 += buff;
				buff = s[++j];
			}
			buff = s[++j];
			while (buff != 0) {
				vex2 += buff;
				buff = s[++j];
			}
			//cout << vex1 << "," << vex2 << endl;
			int vex3, vex4;
			vex3 = atoi(vex1.c_str());
			vex4 = atoi(vex2.c_str());
			a.addEdge(vex3, vex4);
		}
		if (buff == 'p') {
			continue;
		}
	}
	fin.close();
}


int main()
{
	int caculate_count = 4;
	while (caculate_count) {
		K = 72;
		char filename[] = "data\\DSJC250.9.col";
		getN(filename);
		k_Color a(N, K);
		getfile(filename, a);
		cout << "边：" << endl;
		a.initial();
		//cout << "邻接颜色表:" << endl;
		a.updateACTable();
		a.caculateFs();
		//a.display();
		clock_t start, end;
		start = clock();
		cout << "ok1" << endl;
		while (a.fsTrue()) {
			//cout << "ok2" << endl;
			a.findmove();
			a.makemove();
			a.iter++;
		}
		end = clock();
		cout << "iter=" << a.iter << "\t time=" << (end - start) << endl;
		//a.caculateFs();
		a.release();
		caculate_count--;
	}
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
