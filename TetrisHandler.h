#pragma once
#include <array>
#include <string>

class CTetrisHandler
{
public:
    CTetrisHandler();
    ~CTetrisHandler();

    void run();

private:
    void initBlocks();
    void showBlocks();

    std::array<std::array<std::string, 30>, 30> Blocks;
    int totalSize = 30;
};


