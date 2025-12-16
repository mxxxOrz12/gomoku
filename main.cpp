#include "gomoku.h"
#include <iostream>
#include <thread>
#include <limits> // 用于 numeric_limits

using namespace std;

int main() {
    // 尝试设置控制台编码，防止乱码（仅部分环境有效）
    // system("chcp 65001"); 
    
    while (true) {
        // 简易清屏
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif

        cout << "========== 五子棋游戏菜单 ==========" << endl;
        cout << "1. 人人对战 (无禁手)" << endl;
        cout << "2. 人人对战 (带禁手)" << endl;
        cout << "3. 人机对战 (电脑执白)" << endl;
        cout << "4. 退出游戏" << endl;
        cout << "===================================" << endl;
        cout << "请选择: ";
        
        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        
        if (choice == 1) {
            GomokuGame game(0, false);
            game.start();
        } else if (choice == 2) {
            GomokuGame game(0, true);
            game.start();
        } else if (choice == 3) {
            GomokuGame game(1, true);
            game.start();
        } else if (choice == 4) {
            cout << "再见！" << endl;
            break;
        } else {
            cout << "无效选择！" << endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    return 0;
}