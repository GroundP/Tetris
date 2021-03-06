#pragma once
#include <array>
#include <string>
#include <map>
#include <vector>
#include <thread>
#include <condition_variable>
#include <mutex>

const int totalSize = 30;

enum en_shape : int
{
    EN_SQUARE,
    EN_LINE,
    EN_MOUNTAIN,
    EN_BODY,
    EN_BODY_R,
    EN_Z,
    EN_Z_R,
    EN_SHAPE_NUM,
};

enum enDir
{
    EN_LEFT,
    EN_RIGHT,
    EN_UP,
    EN_DOWN,
    EN_NOTHING,
};


class CTetrisHandler
{
public:
    CTetrisHandler();
    ~CTetrisHandler();

    void run();

private:
    void runUnit();
    void init(); // 설정 초기화
    void initBlocks(); // 블록 초기화
    void drawWall(); // 벽 만들기
    void createToy(bool bFirst = false); // 첫 블록 만들기

    void addFix(); // 이미 내려진 블록 FIx
    bool InputDir(); // 방향키 입력받기
    bool MoveToy(enDir dir); // 방향키에 따른 이동

    bool downToy(); // 블록 내리기
    bool leftToy(); // 블록 우로 이동
    bool rightToy(); // 블록 좌로 이동
    bool straight(); // 블록 한방에 끝까지 내리기
    bool rotateToy(); // 블록 회전
    bool getOuterPoint(enDir dir, std::pair<int, int>& point); // 블록 내리기/좌/우 이동시 가장 마지막 포인트 반환

    int getRandomBlock();
    void drawNextBlock();

    bool RemoveFix();
    bool checkGameOver();

    bool isFixed(int row, int col);
    bool print(int row, int col);
    void printTODO(int row, int col);
    void printBestScore();
    void printScore();
    void printLevel(bool bLevelUp = true);
    void printDeadLine();
    void printGameOver();
    void saveScore();

    std::array<std::array<std::string, totalSize>, totalSize> Blocks;
    std::map<en_shape, std::pair<int, int>> Standard;
    //std::vector<std::pair<int, int>> Fixed;
    std::array<std::array<bool, totalSize>, totalSize> Fixed;
    en_shape m_Shape; // 현재 블록
    en_shape m_NextShape; // 다음 블록

    std::mutex mx;
    std::thread InputThread;
    std::condition_variable cond;

    bool bStraightFlag;
    std::pair<int, int> m_stdPoint; // 현재 블록의 기준점

    int score;
    int level;
    int bestScore;

    bool isNewRecord;
    bool isGameOver;
};


