#ifndef GOMOKU_H
#define GOMOKU_H

#include <vector>
#include <chrono>

// 棋盘大小
const int BOARD_SIZE = 15;
const int MAX_TIME_PER_MOVE = 15; 

// 棋子定义
const int EMPTY = 0;
const int BLACK = 1;
const int WHITE = 2;

// 禁手类型
enum ForbiddenType {
    NONE,
    THREE_THREE,
    FOUR_FOUR,
    LONG_LINE
};

struct Point {
    int x, y;
    int score; 
    Point(int _x = -1, int _y = -1) : x(_x), y(_y), score(0) {}
};

// ==================== Board 类声明 ====================
class Board {
private:
    std::vector<std::vector<int>> board;
    std::vector<Point> moveHistory;
    int currentPlayer;
    bool useForbiddenRule;
    std::chrono::steady_clock::time_point lastMoveTime;

public:
    Board(bool useForbiddenRule);
    void reset();
    int getPiece(int x, int y) const;
    int getCurrentPlayer() const;
    Point getLastMove() const;
    bool isInBoard(int x, int y) const;
    bool isEmpty(int x, int y) const;
    bool makeMove(int x, int y);
    bool checkWin(int x, int y) const;
    std::vector<ForbiddenType> checkForbiddenMoves(int x, int y) const;


    void setPiece(int x, int y, int player); 
    void removePiece(int x, int y);     
    int evaluateBoard() const;               // 局势评估函数
    bool hasNeighbor(int x, int y, int distance) const; // 优化搜索范围
    int getPieceCount() const;               // 获取棋盘总子数

private:
    bool checkActiveThree(const std::vector<std::vector<int>>& tempBoard, int x, int y, int dx, int dy) const;
    bool checkActiveFour(const std::vector<std::vector<int>>& tempBoard, int x, int y, int dx, int dy) const;
    bool checkFiveInRow(const std::vector<std::vector<int>>& tempBoard, int x, int y) const;
    // 估分辅助
    int getLineScore(int count, int openEnds, int currentTurn) const;
};

// ==================== GomokuGame 类声明 ====================
class GomokuGame {
private:
    Board board;
    int gameMode; 
    bool useForbiddenRule;
    bool gameOver;
    int winner;

public:
    GomokuGame(int mode, bool forbidden);
    void start();

private:
    void printBoard();
    void playTurn();
    void playerMove();
    void computerMove(); 
    bool isBoardFull() const;
    void printResult();
    void clearScreen();

    Point getBestMove();
    int minimax(int depth, int alpha, int beta, bool isMaximizing);
};

#endif // GOMOKU_H