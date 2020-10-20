// ConsoleApplication4.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
char map[5][10] = {
"########" ,
"# .. p #" ,
"# oo   #" ,
"#      #" ,
"########" 
};
int playerx = 1;
int playery = 5;

int bx[] = {2,2};
int by[] = {2,3};

int targetx[2] = { 1, 1 };
int targety[2] = { 2, 3 };

char curInput;

using namespace std;
void getInput() {
    cin >> curInput;
}
//检查是否位移后碰墙
bool checkIsNextToWall(int x, int y, int px, int py) {
    if (px + x == 0) return true; // 如果检测到第0行
    if (px + x == 4) return true; // 如果检测到第4行
    if (py + y == 0) return true;
    if (py + y == 7) return true;
    return false;
}
//检查移位方向上是否是箱子,如果是的话返回箱子的编号,否则返回-1
int checkIsNextToBox(int x, int y, int px, int py) {
    for (int i = 0; i < sizeof(bx)/sizeof(bx[0]); ++i) {
        if (px + x == bx[i] && py + y == by[i]) {
            return i;
        }
    }
    return -1;
}
//尝试更新状态
void tryMove(int x, int y) {
    bool isWall = checkIsNextToWall(x, y, playerx, playery);
    if (isWall) return;
    int boxNum = checkIsNextToBox(x, y, playerx, playery);
    if (boxNum >= 0) {//如果玩家下一个是箱子
        //检查位移方向上箱子是否紧挨着另一个箱子
        int boxToBoxNum = checkIsNextToBox(x, y, bx[boxNum], by[boxNum]);
        //检查位移方向上箱子是否紧挨着墙
        bool isBoxToWall = checkIsNextToWall(x, y, bx[boxNum], by[boxNum]);
        if (boxToBoxNum == -1 && !isBoxToWall) {//可以推
            //update box state
            bx[boxNum] += x;
            by[boxNum] += y;
            //update player state
            playerx += x;
            playery += y;
        }
    }
    else { // 如果玩家下一个为空地
        playerx += x;
        playery += y;
    }
}
//检查是否是target,如果是的话返回target的编号,否则返回-1
int isOnTarget(int cx, int cy) {
    for (int i = 0; i < sizeof(bx)/sizeof(bx[0]); ++i) {
        if (cx == targetx[i] && cy == targety[i]) {
            return i;
        }
    }
    return -1;
}
//检查游戏是否结束
bool checkIsOver() {
    int sz = sizeof(targetx) / sizeof(targetx[0]);
    int sum = 0;
    for (int i = 0; i < sz; ++i) {
        if (map[targetx[i]][targety[i]] == 'O') {
            ++sum;
        }
    }
    if (sum == sz) return true;
    return false;
}
void updateGame() {
    switch (curInput) {
    case 'w':{
        tryMove(-1, 0);
        break;
    }
    case 'a':{
        tryMove(0, -1);
        break;
    }
    case 's':{
        tryMove(1, 0);
        break;
    }
    case 'd': {
        tryMove(0, 1);
        break;
    }
    default:{
    }
    }       
}

void draw() {
    //清空除墙之外的信息
    for (int i = 1; i < 5-1; ++i) {
        for (int j = 1; j < 8 - 1; ++j) {
            map[i][j] = ' ';
        }
    }
    //更新target信息
    for (int i = 0; i < sizeof(targetx)/sizeof(targetx[0]); ++i) {
        map[targetx[i]][targety[i]] = '.';
    }
    //更新玩家信息
    map[playerx][playery] = 'p';
    if (isOnTarget(playerx, playery) >= 0) {
        map[playerx][playery] = 'P';
    }
    //更新箱子信息
    for (int i = 0; i < sizeof(bx)/sizeof(bx[0]); ++i) {
        map[bx[i]][by[i]] = 'o';
        if (isOnTarget(bx[i], by[i]) >= 0) {
            map[bx[i]][by[i]] = 'O';
        }
    }
    //打印地图
    for (int i = 0; i < 5; ++i) {
        cout << map[i]<<endl;
    }
    cout << endl;
}
int main_old()
{
    draw();
    while (true) {
        getInput();
        updateGame();
        draw();
        if (checkIsOver()) {
            cout << "You win" << endl;
            break;
        }
    }
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
