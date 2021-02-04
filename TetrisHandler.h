#pragma once
#include <array>
#include <string>
#include <map>
#include <vector>

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

class CTetrisHandler
{
public:
    CTetrisHandler();
    ~CTetrisHandler();

    void run();

private:
    void initBlocks();
    void drawWall();
    void createToy(int shape);
    bool downToy(int shape);
    void addFix();

    void showBlocks();

    bool isFixed(int row, int col);

    std::array<std::array<std::string, totalSize>, totalSize> Blocks;
    std::map<en_shape, std::pair<int, int>> Standard;
    std::vector<std::pair<int, int>> Fixed;
};


