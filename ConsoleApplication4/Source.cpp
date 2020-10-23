#include <fstream>
#include <iostream>

using namespace std;
// #ǽ�� _�ռ� .Ŀ�ĵ� o���� p���
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
		mArray(nullptr), //���ɽ�ָ�븳ֵΪ0��ϰ��
		mSize0(size0),
		mSize1(size1) {
		mArray = new int[size0 * size1];
	}
	~IntArray2D() {
		delete[] mArray;
		mArray = nullptr; //���ɽ�����Ҫ��ָ�븳ֵΪ0��ϰ��
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

//��ȡ�����ַ�������ת����Object����
void initialize(Object* state, int w, int h, const char* stageData);
//�滭����
void draw(const Object* state, int w, int h);
//���º���
void update(Object* state, char input, int w, int h);
//�����Ϸ�Ƿ�ͨ��
bool checkClear(const Object* state, int w, int h);
//���ļ��ж�ȡ��ͼ��Ϣ
char* readFileImage(const char* fileName);

//���ļ��ж�ȡ��ͼ��Ϣ
char* readFileImage(const char* fileName) {
	ifstream inputFile(fileName, ifstream::binary);
	inputFile.seekg(0, ifstream::end);//�ƶ���ĩβ
	//λ��=�ļ���С
	int fileSize = static_cast<int>( inputFile.tellg() );
	cout << "fileSize " << fileSize<<endl;
	inputFile.seekg(0, ifstream::beg);//���ص���ʼλ��
	char* fileImage = new char[fileSize]; //�����㹻�Ŀռ�
	inputFile.read(fileImage, fileSize); //��ȡ�ļ�
	//fileImage[fileSize] = '\0';

	cout.write(fileImage, fileSize);
	return fileImage;

}

//��ȡ�����ַ�������ת����Object����
void initialize(Object* state, int width, int height, const char* stageData) {
	const char* d = stageData; //��ȡ��ͼ��Ϣָ��
	int x = 0, y = 0;
	while (*d != '\0') {//��ǰ�ַ���ΪNULLʱ
		Object t;
		switch (*d) {
		case '#': t = OBJ_WALL; break;
		case ' ': t = OBJ_SPACE; break;
		case 'o': t = OBJ_BLOCK; break;
		case 'O': t = OBJ_BLOCK_ON_GOAL; break;
		case '.': t = OBJ_GOAL; break;
		case 'p': t = OBJ_MAN; break;
		case 'P': t = OBJ_MAN_ON_GOAL; break;
		case '\n': //����һ��
			x = 0;//x���ص������
			++y; //y������һ��
			t = OBJ_UNKNOWN; // ���з�û�����ݣ�������д��
			break;
		default: t = OBJ_UNKNOWN; break;
		}

		//�������δ֪�ַ�������
		if (t != OBJ_UNKNOWN) {
			//cout << x << " " << y << endl;
			state[y * width + x] = t;//д�볡������
			++x;
		}
		++d; // ��ȡ��һ����ͼ��Ϣ
	}
}

//�滭����
void draw(const Object* state, int width, int height) {
	//Object ����˳�� ����
	const char font[] = { ' ', '#', '.', 'o', 'O', 'p', 'P' };
	for (int y = 0; y < gStageHeight; ++y) {
		for (int x = 0; x < gStageWidth;++x) {
			Object o = state[y*gStageWidth +x];
			cout << font[o];
		}
		cout << endl;
	}
	
}

//���º���
void update(Object* s, char input, int w, int h) {
	int dx = 0, dy = 0;
	switch (input) {
	case 'w': dy = -1; break;
	case 'a': dx = -1; break;
	case 's': dy = 1; break;
	case 'd': dx = 1; break;
	}
	//�������λ��
	int i = -1;
	for (i = 0;i < w * h;++i) {
		if (s[i] == OBJ_MAN || s[i] == OBJ_MAN_ON_GOAL) {
			break;
		}
	}

	int x = i % w;
	int y = i / w;

	//�ƶ�������
	int tx = x + dx;
	int ty = y + dy;
	if (tx < 0 || ty < 0 || tx >= w || ty >= h) {
		return; // ���������ͼ�߽����ƶ�
	}

	int tp = ty * w + tx;//�ƶ�������
	if (s[tp] == OBJ_SPACE || s[tp] == OBJ_GOAL) {
		s[tp] = (s[tp] == OBJ_GOAL) ? OBJ_MAN_ON_GOAL : OBJ_MAN;
		s[i] = (s[i] == OBJ_GOAL) ? OBJ_GOAL : OBJ_SPACE;
	}
	else if (s[tp] == OBJ_BLOCK || s[tp] == OBJ_BLOCK_ON_GOAL) {//���������
		//���÷����ϵĵڶ��������λ���Ƿ�������Χ��
		int tx2 = tx + dx;
		int ty2 = ty + dy;
		int tp2 = ty2 * w + tx2;
		if (tx2<0 || ty2<0 || tx2>=w|| ty2>=h) {
			return;//�޷��ƶ�
		}
		if (s[tp2] == OBJ_SPACE || s[tp2] == OBJ_GOAL) {
			s[tp2] = (s[tp2] == OBJ_GOAL) ? OBJ_BLOCK_ON_GOAL : OBJ_BLOCK;
			s[tp] = (s[tp] == OBJ_GOAL) ? OBJ_MAN_ON_GOAL : OBJ_MAN;
			s[i] = (s[i] == OBJ_GOAL) ? OBJ_GOAL : OBJ_SPACE;
		}
		else {}//ǽ�ڣ������κδ���
	}
}

//�����Ϸ�Ƿ�ͨ��
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
	//char* stagedata = readFileImage("stageData.txt"); //ע��char* stagedata����Ϊ��ͼ��Ϣ��
	//cout << stagedata << endl; //char * stage��β��û��\0��ֹ�����ʰ�cout���ʱ������ȷ��β

	const IntArray2D array(5, 8);

	cout << "Test" << endl;
	cout << endl;


	//������ʼObject����,�������µ���Ϸ��Ϣ
	Object* state = new Object[gStageHeight * gStageWidth];
	//��ʼ������
	initialize(state, gStageWidth, gStageHeight, gStageData);

	//��ѭ��
	while (true) {
		//���Ȼ���
		draw( state, gStageWidth, gStageHeight);
		//ͨ�ؼ��
		if (checkClear(state, gStageWidth, gStageHeight)) {
			break; // ͨ�ؼ��
		}
		//��ȡ����
		cout << "w a s d. ?" << endl;//����˵��
		char input;
		cin >> input;
		//����
		update(state, input, gStageWidth, gStageHeight);
	}
	//ʤ����ʾ��Ϣ
	cout << "Congratulation's! You win." << endl;
	//ͨ��
	delete[] state; // ע���������ͷ�new���������飬��Ӧ��ʹ��delete[]������delete
	state = 0;
	return 0;
}