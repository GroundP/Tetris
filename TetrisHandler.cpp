#include <iostream>
#include "TetrisHandler.h"

using namespace std;

const string BLOCK = "бс";
const string BLOCK_OUTER = "в╠";
const int Horizontal = 22;
const int Vertical = 13;
const int LeftMargin = 15;
const int TopMargin = 4;

enum en_shape
{
    EN_SQUARE,
    EN_LINE,
    EN_MOUNTAIN,
    EN_N,
    EN_N_R,
    EN_Z,
    EN_Z_R,
    EN_SHAPE_NUM,
};

CTetrisHandler::CTetrisHandler()
{
}

CTetrisHandler::~CTetrisHandler()
{
}


void CTetrisHandler::run()
{
    initBlocks();
    showBlocks();
}

void CTetrisHandler::initBlocks()
{
    for ( auto& a : Blocks )
    {
        for ( auto& b : a )
        {
            b = BLOCK_OUTER;
        }
    }
}

void CTetrisHandler::showBlocks()
{
    for ( const auto& a : Blocks )
    {
        for ( const auto& b : a )
        {
            printf("%s", b.c_str());
        }

        cout << endl;
    }
}

void AdjustLeftMargin()
{
    for ( int nIdx = 0; nIdx < LeftMargin; ++nIdx )
    {
        cout << "  ";
    }
}

void CreateOuterLine()
{
    // OUTER BLOCK
    for ( int hIdx = 0; hIdx < Horizontal; ++hIdx )
    {
        //AdjustLeftMargin();

        if ( hIdx == Horizontal - 1 )
        {
            for ( int vIdx = 0; vIdx < Vertical; ++vIdx )
            {
                cout << BLOCK_OUTER;
            }   
        }
        else
        {
            for ( int vIdx = 0; vIdx < Vertical; ++vIdx )
            {
                if ( vIdx == 0 || vIdx == Vertical - 1 )
                    cout << BLOCK_OUTER;
                else
                    cout << "  ";
            }   
        }

        cout << endl;
    }
}

void CreateTetriminos()
{

}