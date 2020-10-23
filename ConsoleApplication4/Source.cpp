#include <fstream>
#include <iostream>

using namespace std;
// #墙壁 _空间 .目的地 o箱子 p玩家
const char gStageData[] = "\
########\n\
# .. p #\n\
# oo   #\n\
#      #\n\
########";
const int gStageHeight = 5;
const int gStageWidth = 8;

class IntArray2D {
	int* mArray; //private defaultly 2D 
	const int mSize0;
	const int mSize1;
public:
	IntArray2D(int size0, int size1) :
		mArray(nullptr), //养成将指针赋值为0的习惯
		mSize0(size0),
		mSize1(size1) {
		mArray = new int[size0 * size1];
	}
	~IntArray2D() {
		delete[] mArray;
		mArray = nullptr; //养成将不需要的指针赋值为0的习惯
	}
	int& operator() (int index0, int index1) { //array(2,1) = 5;
		return mArray[ index1* mSize0 + index0];
	}

	const int& operator() (int index0, int index1) const { // const IntArray2D array(5, 8);
		return mArray[index1 * mSize0 + index0];
	}
};

enum Object {
	OBJ_SPACE,
	OBJ_WALL,
	OBJ_GOAL,
	OBJ_BLOCK,
	OBJ_BLOCK_ON_GOAL,
	OBJ_MAN,
	OBJ_MAN_ON_GOAL,

	OBJ_UNKNOWN,
};

//读取场景字符串将其转换成Object数组
void initialize(Object* state, int w, int h, const char* stageData);
//绘画函数
void draw(const Object* state, int w, int h);
//更新函数
void update(Object* state, char input, int w, int h);
//检测游戏是否通关
bool checkClear(const Object* state, int w, int h);
//从文件中读取地图信息
char* readFileImage(const char* fileName);

//从文件中读取地图信息
char* readFileImage(const char* fileName) {
	ifstream inputFile(fileName, ifstream::binary);
	inputFile.seekg(0, ifstream::end);//移动到末尾
	//位置=文件大小
	int fileSize = static_cast<int>( inputFile.tellg() );
	cout << "fileSize " << fileSize<<endl;
	inputFile.seekg(0, ifstream::beg);//返回到起始位置
	char* fileImage = new char[fileSize]; //分配足够的空间
	inputFile.read(fileImage, fileSize); //读取文件
	//fileImage[fileSize] = '\0';

	cout.write(fileImage, fileSize);
	return fileImage;

}

//读取场景字符串将其转换成Object数组
void initialize(Object* state, int width, int height, const char* stageData) {
	const char* d = stageData; //读取地图信息指针
	int x = 0, y = 0;
	while (*d != '\0') {//当前字符不为NULL时
		Object t;
		switch (*d) {
		case '#': t = OBJ_WALL; break;
		case ' ': t = OBJ_SPACE; break;
		case 'o': t = OBJ_BLOCK; break;
		case 'O': t = OBJ_BLOCK_ON_GOAL; break;
		case '.': t = OBJ_GOAL; break;
		case 'p': t = OBJ_MAN; break;
		case 'P': t = OBJ_MAN_ON_GOAL; break;
		case '\n': //到下一行
			x = 0;//x返回到最左边
			++y; //y进到下一行
			t = OBJ_UNKNOWN; // 换行符没有数据，不进行写入
			break;
		default: t = OBJ_UNKNOWN; break;
		}

		//如果遇到未知字符则无视
		if (t != OBJ_UNKNOWN) {
			//cout << x << " " << y << endl;
			state[y * width + x] = t;//写入场景数据
			++x;
		}
		++d; // 读取下一个地图信息
	}
}

//绘画函数
void draw(const Object* state, int width, int height) {
	//Object 类型顺序 按照
	const char font[] = { ' ', '#', '.', 'o', 'O', 'p', 'P' };
	for (int y = 0; y < gStageHeight; ++y) {
		for (int x = 0; x < gStageWidth;++x) {
			Object o = state[y*gStageWidth +x];
			cout << font[o];
		}
		cout << endl;
	}
	
}

//更新函数
void update(Object* s, char input, int w, int h) {
	int dx = 0, dy = 0;
	switch (input) {
	case 'w': dy = -1; break;
	case 'a': dx = -1; break;
	case 's': dy = 1; break;
	case 'd': dx = 1; break;
	}
	//查找玩家位置
	int i = -1;
	for (i = 0;i < w * h;++i) {
		if (s[i] == OBJ_MAN || s[i] == OBJ_MAN_ON_GOAL) {
			break;
		}
	}

	int x = i % w;
	int y = i / w;

	//移动后坐标
	int tx = x + dx;
	int ty = y + dy;
	if (tx < 0 || ty < 0 || tx >= w || ty >= h) {
		return; // 如果超出地图边界则不移动
	}

	int tp = ty * w + tx;//移动后坐标
	if (s[tp] == OBJ_SPACE || s[tp] == OBJ_GOAL) {
		s[tp] = (s[tp] == OBJ_GOAL) ? OBJ_MAN_ON_GOAL : OBJ_MAN;
		s[i] = (s[i] == OBJ_GOAL) ? OBJ_GOAL : OBJ_SPACE;
	}
	else if (s[tp] == OBJ_BLOCK || s[tp] == OBJ_BLOCK_ON_GOAL) {//如果是箱子
		//检测该方向上的第二个网格的位置是否在允许范围内
		int tx2 = tx + dx;
		int ty2 = ty + dy;
		int tp2 = ty2 * w + tx2;
		if (tx2<0 || ty2<0 || tx2>=w|| ty2>=h) {
			return;//无法移动
		}
		if (s[tp2] == OBJ_SPACE || s[tp2] == OBJ_GOAL) {
			s[tp2] = (s[tp2] == OBJ_GOAL) ? OBJ_BLOCK_ON_GOAL : OBJ_BLOCK;
			s[tp] = (s[tp] == OBJ_GOAL) ? OBJ_MAN_ON_GOAL : OBJ_MAN;
			s[i] = (s[i] == OBJ_GOAL) ? OBJ_GOAL : OBJ_SPACE;
		}
		else {}//墙壁，不做任何处理
	}
}

//检测游戏是否通关
bool checkClear(const Object* state, int w, int h) {
	for (int i = 0; i < w * h; ++i) {
		if (state[i] == OBJ_BLOCK) {
			return false;
		}
	}
	return true;
}

int main() {
	cout << "new myBox" << endl;
	//char* stagedata = readFileImage("stageData.txt"); //注意char* stagedata数组为地图信息，
	//cout << stagedata << endl; //char * stage结尾并没有\0休止符，故按cout输出时不能正确结尾

	const IntArray2D array(5, 8);

	cout << "Test" << endl;
	cout << endl;


	//创建初始Object数组,即被更新的游戏信息
	Object* state = new Object[gStageHeight * gStageWidth];
	//初始化场景
	initialize(state, gStageWidth, gStageHeight, gStageData);

	//主循环
	while (true) {
		//首先绘制
		draw( state, gStageWidth, gStageHeight);
		//通关检测
		if (checkClear(state, gStageWidth, gStageHeight)) {
			break; // 通关检测
		}
		//获取输入
		cout << "w a s d. ?" << endl;//操作说明
		char input;
		cin >> input;
		//更新
		update(state, input, gStageWidth, gStageHeight);
	}
	//胜利提示信息
	cout << "Congratulation's! You win." << endl;
	//通关
	delete[] state; // 注意这里是释放new创建的数组，而应当使用delete[]而不是delete
	state = 0;
	return 0;
}