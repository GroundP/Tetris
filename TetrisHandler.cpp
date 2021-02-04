#include <iostream>
#include <Windows.h>
#include "TetrisHandler.h"

using namespace std;

const string BLOCK = "бс";
const string BLOCK_OUTER = "в╠";
const string EMPTY = "  ";
const int rowBegin = 5;
const int rowEnd = 25;
const int colBegin = 9;
const int colEnd = 22;

const int rowStd = 4;
const int colStd = 16;

CTetrisHandler::CTetrisHandler()
{
}

CTetrisHandler::~CTetrisHandler()
{
}


void CTetrisHandler::run()
{
    initBlocks();
    drawWall();
    createToy(EN_SQUARE);
    while ( true )
    {
        showBlocks();
        const auto& downRes = downToy(EN_SQUARE);
        if ( !downRes )
        {
            addFix();
            createToy(EN_SQUARE);
        }

            
        Sleep(300);
    }
}

void CTetrisHandler::initBlocks()
{
    for ( auto& sero : Blocks )
    {
        for ( auto& garo : sero )
        {
            garo = EMPTY;
        }
    }
}

void CTetrisHandler::drawWall()
{
    const int rowSize = static_cast<int>(Blocks.size());
    for ( int row = 0; row < rowSize; ++row )
    {
        if ( row < rowBegin || row > rowEnd )
            continue;

        auto& rowD = Blocks[row];
        const int colSize = static_cast<int>(rowD.size());
        for ( int col = 0; col < colSize; ++col )
        {
            auto& colD = rowD[col];

            if ( row == rowEnd && col >= colBegin && col <= colEnd)
                colD = BLOCK_OUTER;

            if ( col == colBegin || col == colEnd )
                colD = BLOCK_OUTER;
        }
    }
}

void CTetrisHandler::createToy(int shape)
{
    switch ( shape )
    {
    case EN_SQUARE:
        Blocks[rowStd][colStd] = BLOCK;
        Blocks[rowStd][colStd-1] = BLOCK;
        Blocks[rowStd+1][colStd] = BLOCK;
        Blocks[rowStd+1][colStd-1] = BLOCK;
        break;
    default:
        break;
    }
}

bool CTetrisHandler::downToy(int shape)
{
    const int rowSize = static_cast<int>(Blocks.size());
    for ( int row = rowEnd; row > 0; --row )
    {
        const int colSize = static_cast<int>(Blocks[row].size());
        for ( int col = colBegin; col < colEnd; ++col )
        {
            //if ( /*Blocks[row+1][col] == BLOCK || */Blocks[row+1][col] == BLOCK_OUTER )
            //    break;

            if ( !isFixed(row, col) && Blocks[row][col] == BLOCK )
            {
                if ( row == rowEnd -1 || isFixed(row+1, col) )
                    return false;

                Blocks[row][col] = EMPTY;
                Blocks[row+1][col] = BLOCK;
            }
        }
    }

    return true;
}

void CTetrisHandler::addFix()
{
    const int rowSize = static_cast<int>(Blocks.size());
    for ( int row = rowEnd; row > 0; --row )
    {
        const int colSize = static_cast<int>(Blocks[row].size());
        for ( int col = colBegin; col < colEnd; ++col )
        {
            if ( Blocks[row][col] == BLOCK )
                Fixed.push_back(make_pair(row, col));
        }
    }
}

bool CTetrisHandler::isFixed(int row, int col)
{
    return std::count(Fixed.begin(), Fixed.end(), make_pair(row, col)) > 0;
}

void CTetrisHandler::showBlocks()
{
    system("cls");
    for ( const auto& a : Blocks )
    {
        for ( const auto& b : a )
        {
            printf("%s", b.c_str());
        }

        cout << endl;
    }
}