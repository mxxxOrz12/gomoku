#include "gomoku.h"
#include <iostream>
#include <iomanip>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <algorithm> 
#include <limits>   

using namespace std;
using namespace std::chrono;

const int DIRECTIONS[4][2] = {
    {0, 1}, {1, 0}, {1, 1}, {1, -1}
};



Board::Board(bool useForbiddenRule) : 
    board(BOARD_SIZE, vector<int>(BOARD_SIZE, EMPTY)), 
    currentPlayer(BLACK),
    useForbiddenRule(useForbiddenRule) {
    reset();
}

void Board::reset() {
    for (auto& row : board) fill(row.begin(), row.end(), EMPTY);
    moveHistory.clear();
    currentPlayer = BLACK;
    lastMoveTime = steady_clock::now();
}
int Board::getPiece(int x, int y) const { if(isInBoard(x,y)) return board[x][y]; return -1; }
int Board::getCurrentPlayer() const { return currentPlayer; }
Point Board::getLastMove() const { return moveHistory.empty() ? Point(-1,-1) : moveHistory.back(); }
bool Board::isInBoard(int x, int y) const { return x>=0 && x<BOARD_SIZE && y>=0 && y<BOARD_SIZE; }
bool Board::isEmpty(int x, int y) const { return isInBoard(x,y) && board[x][y] == EMPTY; }


bool Board::makeMove(int x, int y) {
    if (!isEmpty(x, y)) return false;
    if (currentPlayer == BLACK && useForbiddenRule) {
        vector<ForbiddenType> forbiddenTypes = checkForbiddenMoves(x, y);
        if (!forbiddenTypes.empty()) {
            cout << ">>> 禁手警告！" << endl;
            this_thread::sleep_for(seconds(1));
            return false;
        }
    }
    board[x][y] = currentPlayer;
    moveHistory.push_back(Point(x, y));
    lastMoveTime = steady_clock::now();
    currentPlayer = (currentPlayer == BLACK) ? WHITE : BLACK;
    return true;
}

bool Board::checkWin(int x, int y) const {
    int player = board[x][y];
    if (player == EMPTY) return false;
    for (auto& dir : DIRECTIONS) {
        int count = 1;
        for (int i = 1; i <= 4; i++) {
            int nx = x + dir[0] * i, ny = y + dir[1] * i;
            if (isInBoard(nx, ny) && board[nx][ny] == player) count++; else break;
        }
        for (int i = 1; i <= 4; i++) {
            int nx = x - dir[0] * i, ny = y - dir[1] * i;
            if (isInBoard(nx, ny) && board[nx][ny] == player) count++; else break;
        }
        if (count >= 5) return true;
    }
    return false;
}

// 检查是否五连
bool Board::checkFiveInRow(const vector<vector<int>>& tempBoard, int x, int y) const {
    for (auto& dir : DIRECTIONS) {
        int count = 1;
        for (int i = 1; i <= 4; i++) {
            if (isInBoard(x + dir[0]*i, y + dir[1]*i) && tempBoard[x + dir[0]*i][y + dir[1]*i] == BLACK) count++; else break;
        }
        for (int i = 1; i <= 4; i++) {
            if (isInBoard(x - dir[0]*i, y - dir[1]*i) && tempBoard[x - dir[0]*i][y - dir[1]*i] == BLACK) count++; else break;
        }
        if (count == 5) return true;
    }
    return false;
}

vector<ForbiddenType> Board::checkForbiddenMoves(int x, int y) const {
    vector<ForbiddenType> forbiddenTypes;
    vector<vector<int>> tempBoard = board;
    tempBoard[x][y] = BLACK;
    for (auto& dir : DIRECTIONS) {
        int count = 1;
        for (int i = 1; i <= 5; i++) {
            if (isInBoard(x + dir[0]*i, y + dir[1]*i) && tempBoard[x + dir[0]*i][y + dir[1]*i] == BLACK) count++; else break;
        }
        for (int i = 1; i <= 5; i++) {
            if (isInBoard(x - dir[0]*i, y - dir[1]*i) && tempBoard[x - dir[0]*i][y - dir[1]*i] == BLACK) count++; else break;
        }
        if (count >= 6) { forbiddenTypes.push_back(LONG_LINE); return forbiddenTypes; }
    }
    if (checkFiveInRow(tempBoard, x, y)) return forbiddenTypes;
    int threeCount = 0, fourCount = 0;
    for (int i = 0; i < 4; i++) {
        if (checkActiveThree(tempBoard, x, y, DIRECTIONS[i][0], DIRECTIONS[i][1])) threeCount++;
        if (checkActiveFour(tempBoard, x, y, DIRECTIONS[i][0], DIRECTIONS[i][1])) fourCount++;
    }
    if (threeCount >= 2) forbiddenTypes.push_back(THREE_THREE);
    if (fourCount >= 2) forbiddenTypes.push_back(FOUR_FOUR);
    return forbiddenTypes;
}

bool Board::checkActiveThree(const vector<vector<int>>& tempBoard, int x, int y, int dx, int dy) const {
    int count = 1;
    int i = 1; while (isInBoard(x + dx*i, y + dy*i) && tempBoard[x + dx*i][y + dy*i] == BLACK) { count++; i++; }
    bool openStart = isInBoard(x + dx*i, y + dy*i) && tempBoard[x + dx*i][y + dy*i] == EMPTY;
    int j = 1; while (isInBoard(x - dx*j, y - dy*j) && tempBoard[x - dx*j][y - dy*j] == BLACK) { count++; j++; }
    bool openEnd = isInBoard(x - dx*j, y - dy*j) && tempBoard[x - dx*j][y - dy*j] == EMPTY;
    return (count == 3 && openStart && openEnd);
}

bool Board::checkActiveFour(const vector<vector<int>>& tempBoard, int x, int y, int dx, int dy) const {
    int count = 1;
    int i = 1; while (isInBoard(x + dx*i, y + dy*i) && tempBoard[x + dx*i][y + dy*i] == BLACK) { count++; i++; }
    int j = 1; while (isInBoard(x - dx*j, y - dy*j) && tempBoard[x - dx*j][y - dy*j] == BLACK) { count++; j++; }
    return (count == 4);
}


void Board::setPiece(int x, int y, int player) {
    if (isInBoard(x, y)) board[x][y] = player;
}

void Board::removePiece(int x, int y) {
    if (isInBoard(x, y)) board[x][y] = EMPTY;
}

int Board::getPieceCount() const {
    return moveHistory.size();
}

// 检查某个点周围距离distance内是否有棋子（用于剪枝，不搜索孤立点）
bool Board::hasNeighbor(int x, int y, int distance) const {
    int startX = max(0, x - distance);
    int endX = min(BOARD_SIZE - 1, x + distance);
    int startY = max(0, y - distance);
    int endY = min(BOARD_SIZE - 1, y + distance);

    for (int i = startX; i <= endX; i++) {
        for (int j = startY; j <= endY; j++) {
            if (board[i][j] != EMPTY) return true;
        }
    }
    return false;
}

int Board::getLineScore(int count, int openEnds, int currentTurn) const {
    if (count >= 5) return 100000;
    if (count == 4) {
        if (openEnds == 2) return 10000; 
        if (openEnds == 1) return 1000;  
    }
    if (count == 3) {
        if (openEnds == 2) return 1000; 
        if (openEnds == 1) return 100;   
    }
    if (count == 2) {
        if (openEnds == 2) return 100;   
        if (openEnds == 1) return 10;
    }
    return 0;
}

// 全局估值函数：正分代表白棋(AI)优势，负分代表黑棋(玩家)优势
int Board::evaluateBoard() const {
    int blackScore = 0;
    int whiteScore = 0;

    // 扫描四个方向的所有线
    // 简化策略：遍历棋盘每个点，作为线的起点向4个方向扫描
    // 为了不重复计算，只统计以该点为起点的线段（例如：遇到空位或异色则停止）

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == EMPTY) continue;
            
            int player = board[i][j];
            
            for (auto& dir : DIRECTIONS) {
                // 只在正向检查，避免重复
                // 且只在该方向上的"头"部计算，避免子线段重复分
                int prevX = i - dir[0];
                int prevY = j - dir[1];
                if (isInBoard(prevX, prevY) && board[prevX][prevY] == player) continue; 
                
                int count = 1;
                int openEnds = 0;
                
                if (isInBoard(prevX, prevY) && board[prevX][prevY] == EMPTY) openEnds++;
                
                int r = 1;
                while (true) {
                    int nx = i + dir[0] * r;
                    int ny = j + dir[1] * r;
                    
                    if (!isInBoard(nx, ny)) break;
                    if (board[nx][ny] == player) {
                        count++;
                    } else {
                        if (board[nx][ny] == EMPTY) openEnds++;
                        break;
                    }
                    r++;
                }
                
                int score = getLineScore(count, openEnds, player);
                if (player == WHITE) whiteScore += score;
                else blackScore += score;
            }
        }
    }
    
    return whiteScore - blackScore; 
}

// ==================== GomokuGame AI 实现 ====================

GomokuGame::GomokuGame(int mode, bool forbidden) : 
    gameMode(mode), 
    useForbiddenRule(forbidden),
    gameOver(false),
    winner(EMPTY),
    board(forbidden) {
}

void GomokuGame::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void GomokuGame::start() {
    clearScreen();
    cout << "========== 五子棋 AI 对战 ==========" << endl;
    cout << "电脑正在使用 Minimax 算法思考..." << endl;
    cout << "黑棋(X)由您执子，白棋(O)由电脑执子" << endl;
    cout << "输入格式：行 列 (例如: 7 7)" << endl;
    cout << "按回车开始..." << endl;
    cin.ignore(); cin.get();
    printBoard();
    while (!gameOver) { playTurn(); }
    printResult();
}

void GomokuGame::printBoard() {
    clearScreen();
    cout << "    ";
    for (int i = 0; i < BOARD_SIZE; i++) cout << setw(2) << i << " ";
    cout << endl;
    for (int i = 0; i < BOARD_SIZE; i++) {
        cout << setw(2) << i << "  ";
        for (int j = 0; j < BOARD_SIZE; j++) {
            int p = board.getPiece(i, j);
            Point lm = board.getLastMove();
            string s = (p==BLACK)?"X ":(p==WHITE)?"O ":"+ ";
            if(lm.x==i && lm.y==j) cout << "[" << s[0] << "]"; else cout << " " << s;
        }
        cout << endl;
    }
}

void GomokuGame::playTurn() {
    int currentPlayer = board.getCurrentPlayer();
    if (gameMode == 1 && currentPlayer == WHITE) {
        computerMove();
    } else {
        playerMove();
    }
    
    Point lastMove = board.getLastMove();
    if (lastMove.x != -1 && board.checkWin(lastMove.x, lastMove.y)) {
        gameOver = true;
        winner = board.getPiece(lastMove.x, lastMove.y);
    } else if (!gameOver && isBoardFull()) {
        gameOver = true;
        winner = EMPTY;
    }
    if (!gameOver) printBoard();
}

void GomokuGame::playerMove() {
    int x, y;
    while (true) {
        cout << "请输入落子 (行 列): ";
        if (cin >> x >> y) {
            if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
                if (board.makeMove(x, y)) break;
                else cout << "无法落子！" << endl;
            } else cout << "越界！" << endl;
        } else {
            cin.clear(); cin.ignore(10000, '\n');
        }
    }
}


void GomokuGame::computerMove() {
    cout << "AI 正在思考中..." << endl;
    auto start = steady_clock::now();

    Point bestMove = getBestMove();
    
    if (bestMove.x == -1) return;

    board.makeMove(bestMove.x, bestMove.y);

    auto end = steady_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    cout << "AI 落子: " << bestMove.x << " " << bestMove.y 
         << " (耗时: " << duration.count() << "ms)" << endl;
    this_thread::sleep_for(milliseconds(500)); 
}

// Minimax 入口
Point GomokuGame::getBestMove() {
    // 1. 如果棋盘是空的，直接下天元 (7, 7)
    if (board.getPieceCount() == 0) return Point(7, 7);

    // 2. 搜索所有候选点
    vector<Point> candidates;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            // 只有空位且周围2格内有子才考虑（大幅剪枝）
            if (board.isEmpty(i, j) && board.hasNeighbor(i, j, 2)) {
                candidates.push_back(Point(i, j));
            }
        }
    }

    // 简单排序启发式：优先考虑中心位置
    
    Point bestMove = candidates[0];
    int bestScore = -std::numeric_limits<int>::max();
    int alpha = -std::numeric_limits<int>::max();
    int beta = std::numeric_limits<int>::max();

    int depth = 2; 

    for (const auto& move : candidates) {
        // 尝试落子
        board.setPiece(move.x, move.y, WHITE);
        
        if (board.checkWin(move.x, move.y)) {
            board.removePiece(move.x, move.y);
            return move;
        }
        int score = minimax(depth - 1, alpha, beta, false);
        
        // 撤销落子
        board.removePiece(move.x, move.y);

        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
        // Alpha 更新
        alpha = max(alpha, bestScore);
    }
    
    return bestMove;
}

// Minimax 递归函数 (Alpha-Beta 剪枝)
int GomokuGame::minimax(int depth, int alpha, int beta, bool isMaximizing) {
    // 基础情况：达到深度或游戏结束（这里只检测深度，胜负在循环中检测了）
    if (depth == 0) {
        return board.evaluateBoard();
    }

    // 再次生成候选点（由于棋盘变了，需要重新扫描，这里简化：依然扫全局）
    // 优化：其实只需要扫之前的候选点 - 占用的点 + 新增点周围的点。
    // 为了代码简单，这里暴力扫描全盘邻近点。
    vector<Point> candidates;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board.isEmpty(i, j) && board.hasNeighbor(i, j, 1)) { // 深度越深，邻居范围可缩小至1
                candidates.push_back(Point(i, j));
            }
        }
    }
    
    if (candidates.empty()) return board.evaluateBoard();

    if (isMaximizing) { 
        int maxScore = -std::numeric_limits<int>::max();
        for (const auto& move : candidates) {
            board.setPiece(move.x, move.y, WHITE);
            

            if (board.checkWin(move.x, move.y)) {
                board.removePiece(move.x, move.y);
                return 100000 + depth; 
            }
            
            int score = minimax(depth - 1, alpha, beta, false);
            board.removePiece(move.x, move.y);
            
            maxScore = max(maxScore, score);
            alpha = max(alpha, score);
            if (beta <= alpha) break; 
        }
        return maxScore;
    } else { 
        int minScore = std::numeric_limits<int>::max();
        for (const auto& move : candidates) {
            board.setPiece(move.x, move.y, BLACK);

            if (board.checkWin(move.x, move.y)) {
                board.removePiece(move.x, move.y);
                return -100000 - depth; 
            }
            
            int score = minimax(depth - 1, alpha, beta, true);
            board.removePiece(move.x, move.y);
            
            minScore = min(minScore, score);
            beta = min(beta, score);
            if (beta <= alpha) break;
        }
        return minScore;
    }
}

bool GomokuGame::isBoardFull() const {
    for(int i=0;i<BOARD_SIZE;i++) for(int j=0;j<BOARD_SIZE;j++) if(board.isEmpty(i,j)) return false;
    return true;
}

void GomokuGame::printResult() {
    printBoard();
    if(winner==BLACK) cout << "黑棋胜！" << endl;
    else if(winner==WHITE) cout << "白棋(AI)胜！" << endl;
    else cout << "平局！" << endl;
    cin.ignore(); cin.get();
}