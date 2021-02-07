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
    void initBlocks(); // 블록 초기화
    void drawWall(); // 벽 만들기
    void createToy(); // 첫 블록 만들기
    void addFix(); // 이미 내려진 블록 FIx
    bool InputDir(); // 방향키 입력받기
    bool MoveToy(enDir dir); // 방향키에 따른 이동

    bool downToy(); // 블록 내리기
    bool leftToy(); // 블록 우로 이동
    bool rightToy(); // 블록 좌로 이동
    bool rotateToy(); // 블록 회전


    void showBlocks();

    bool isFixed(int row, int col);
    bool print(int row, int col);

    std::array<std::array<std::string, totalSize>, totalSize> Blocks;
    std::map<en_shape, std::pair<int, int>> Standard;
    std::vector<std::pair<int, int>> Fixed;
    en_shape m_shape; // 현재 블록의 모양

    std::mutex mx;
    std::thread InputThread;
    std::condition_variable cond;

    std::pair<int, int> m_stdPoint; // 현재 블록의 기준점
};


