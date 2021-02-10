#include <iostream>
#include <Windows.h>
#include <conio.h>
#include "TetrisHandler.h"

using namespace std;

const string BLOCK = "■";
const string BLOCK_OUTER = "▩";
const string EMPTY = "  ";
const int rowBegin = 5;
const int rowEnd = 25;
const int colBegin = 9;
const int colEnd = 22;

const int rowStd = 4;
const int colStd = 16;


CTetrisHandler::CTetrisHandler()
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO Cursor;
    Cursor.bVisible = 0;
    Cursor.dwSize = 1;
    SetConsoleCursorInfo(consoleHandle, &Cursor);

    for ( auto& rowFix : Fixed )
        for ( auto& colFix : rowFix )
            colFix = false;

    InputThread = thread(&CTetrisHandler::InputDir, this);
    InputThread.detach();
}

CTetrisHandler::~CTetrisHandler()
{
}


void CTetrisHandler::run()
{
    initBlocks();
    drawWall();
    createToy();

    while ( true )
    {
        if ( !downToy() )
        {
            addFix();
            RemoveFix();
            createToy();
        }
            
        Sleep(300);
    }

    while ( true )
    {
        int nIuput = _getch();
        cout << "문자" << (char)nIuput << ", 아스키코드: " << nIuput << endl;
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

            if ( row == rowEnd && col >= colBegin && col <= colEnd )
            {
                colD = BLOCK_OUTER;
                print(row, col);
            }

            if ( col == colBegin || col == colEnd )
            {
                colD = BLOCK_OUTER;
                print(row, col);
            }
        }
    }
}

void CTetrisHandler::createToy()
{
    srand((unsigned int)time(NULL));


    const int random = rand();
    const int num = random % EN_SHAPE_NUM;
    m_shape = static_cast<en_shape>(num);

    array<pair<int, int>,4> arr;
    switch ( num )
    {
    case EN_SQUARE:
        arr[0] = make_pair(rowStd, colStd);
        arr[1] = make_pair(rowStd, colStd+1);
        arr[2] = make_pair(rowStd-1, colStd);
        arr[3] = make_pair(rowStd-1, colStd+1);
        break;
    case EN_LINE:
        arr[0] = make_pair(rowStd, colStd);
        arr[1] = make_pair(rowStd, colStd-1);
        arr[2] = make_pair(rowStd, colStd+1);
        arr[3] = make_pair(rowStd, colStd+2);
        break;
    case EN_MOUNTAIN:
        arr[0] = make_pair(rowStd, colStd);
        arr[1] = make_pair(rowStd-1, colStd);
        arr[2] = make_pair(rowStd, colStd+1);
        arr[3] = make_pair(rowStd, colStd-1);
        break;
    case EN_BODY:
        arr[0] = make_pair(rowStd, colStd);
        arr[1] = make_pair(rowStd-1, colStd);
        arr[2] = make_pair(rowStd, colStd+1);
        arr[3] = make_pair(rowStd, colStd+2);
        break;
    case EN_BODY_R:
        arr[0] = make_pair(rowStd, colStd);
        arr[1] = make_pair(rowStd-1, colStd);
        arr[2] = make_pair(rowStd, colStd-1);
        arr[3] = make_pair(rowStd, colStd-2);
        break;
    case EN_Z:
        arr[0] = make_pair(rowStd, colStd);
        arr[1] = make_pair(rowStd-1, colStd);
        arr[2] = make_pair(rowStd, colStd+1);
        arr[3] = make_pair(rowStd+1, colStd+1);
        break;
    case EN_Z_R:
        arr[0] = make_pair(rowStd, colStd);
        arr[1] = make_pair(rowStd-1, colStd);
        arr[2] = make_pair(rowStd, colStd-1);
        arr[3] = make_pair(rowStd+1, colStd-1);
        break;
    default:
        break;
    }

    m_stdPoint = make_pair(rowStd, colStd);
    Blocks[arr[0].first][arr[0].second] = BLOCK;
    Blocks[arr[1].first][arr[1].second] = BLOCK;
    Blocks[arr[2].first][arr[2].second] = BLOCK;
    Blocks[arr[3].first][arr[3].second] = BLOCK;

    print(arr[0].first, arr[0].second);
    print(arr[1].first, arr[1].second);
    print(arr[2].first, arr[2].second);
    print(arr[3].first, arr[3].second);
}

void CTetrisHandler::addFix()
{
    const int rowSize = static_cast<int>(Blocks.size());
    for ( int row = rowEnd-1; row > 0; --row )
    {
        const int colSize = static_cast<int>(Blocks[row].size());
        for ( int col = colBegin+1; col < colEnd; ++col )
        {
            if ( Blocks[row][col] == BLOCK )
                Fixed[row][col] = true;
        }
    }
}

bool CTetrisHandler::InputDir()
{
    while ( true )
    {
        int nIuput = _getch();
        enDir dir = EN_NOTHING;

        if ( nIuput == 224 ) // 방향키 입력
        {
            nIuput = _getch();
            switch ( nIuput )
            {
            case 75:
                dir = EN_LEFT; break;
            case 77:
                dir = EN_RIGHT; break;
            case 72:
                dir = EN_UP; break;
            case 80:
                dir = EN_DOWN; break;
            default:
                continue;
            }
        }
        else if ( nIuput == VK_SPACE )
        {
            straight();
        }
        else
            continue;
        

        MoveToy(dir);
    }

    return true;
}

bool CTetrisHandler::MoveToy(enDir dir)
{
    switch ( dir )
    {
    case EN_LEFT: 
        if ( leftToy() )
        {
            m_stdPoint.second--;
        }
        break;
    case EN_RIGHT: 
        if ( rightToy() )
        {
            m_stdPoint.second++;
        }
        break;
    case EN_UP: 
        rotateToy(); 
        break;
    case EN_DOWN: 
        downToy(); break;
    case EN_NOTHING: break;
    default:
        return false;
    }

    
    return true;
}

bool CTetrisHandler::downToy()
{
    lock_guard<mutex> lock(mx);

    m_stdPoint.first++;

    pair<int, int> lowest;
    getOuterPoint(EN_DOWN, lowest);
    if ( lowest.first + 1 == rowEnd )
        return false;

    vector<pair<int, int>> aEmptys;
    vector<pair<int, int>> aBlocks;

    const int rowSize = static_cast<int>(Blocks.size());
    for ( int row = rowEnd-1; row > 0; --row )
    {
        const int colSize = static_cast<int>(Blocks[row].size());
        for ( int col = colBegin+1; col < colEnd; ++col )
        {
            //if ( /*Blocks[row+1][col] == BLOCK || */Blocks[row+1][col] == BLOCK_OUTER )
            //    break;

            if ( !isFixed(row, col) && Blocks[row][col] == BLOCK )
            {
                if ( row == rowEnd -1 || isFixed(row+1, col) )
                    return false;

                aEmptys.push_back(make_pair(row, col));
                aBlocks.push_back(make_pair(row+1, col));
            }
        }
    }

    for ( const auto a : aEmptys )
    {
        Blocks[a.first][a.second] = EMPTY;
        print(a.first, a.second);
    }

    for ( const auto a : aBlocks )
    {
        Blocks[a.first][a.second] = BLOCK;
        print(a.first, a.second);
    }
    
    return true;
}

bool CTetrisHandler::leftToy()
{
    lock_guard<mutex> lock(mx);

    pair<int, int> outer;
    getOuterPoint(EN_LEFT, outer);
    if ( outer.second - 1 == colBegin )
        return false;

    vector<pair<int, int>> aEmptys;
    vector<pair<int, int>> aBlocks;

    const int rowSize = static_cast<int>(Blocks.size());
    for ( int row = rowEnd-1; row > 0; --row )
    {
        const int colSize = static_cast<int>(Blocks[row].size());
        for ( int col = colBegin+1; col < colEnd; ++col )
        {
            if ( !isFixed(row, col) && Blocks[row][col] == BLOCK )
            {
                if ( col == colBegin +1 || isFixed(row, col-1))
                    return false;

                aBlocks.push_back(make_pair(row, col-1));
                aEmptys.push_back(make_pair(row, col));
            }
        }
    }

    for ( const auto a : aEmptys )
    {
        Blocks[a.first][a.second] = EMPTY;
        print(a.first, a.second);
    }

    for ( const auto a : aBlocks )
    {
        Blocks[a.first][a.second] = BLOCK;
        print(a.first, a.second);
    }

    return true;
}

bool CTetrisHandler::rightToy()
{
    lock_guard<mutex> lock(mx);

    pair<int, int> outer;
    getOuterPoint(EN_RIGHT, outer);
    if ( outer.second + 1 == colEnd )
        return false;

    vector<pair<int, int>> aEmptys;
    vector<pair<int, int>> aBlocks;

    const int rowSize = static_cast<int>(Blocks.size());
    for ( int row = rowEnd-1; row > 0; --row )
    {
        const int colSize = static_cast<int>(Blocks[row].size());
        for ( int col = colEnd-1; col > colBegin; --col )
        {
            if ( !isFixed(row, col) && Blocks[row][col] == BLOCK )
            {
                if ( col == colEnd -1 || isFixed(row, col+1))
                    return false;

                aBlocks.push_back(make_pair(row, col+1));
                aEmptys.push_back(make_pair(row, col));
            }
        }
    }

    for ( const auto a : aEmptys )
    {
        Blocks[a.first][a.second] = EMPTY;
        print(a.first, a.second);
    }

    for ( const auto a : aBlocks )
    {
        Blocks[a.first][a.second] = BLOCK;
        print(a.first, a.second);
    }

    return true;
}

bool CTetrisHandler::straight()
{
    while ( true )
    {
        if ( !downToy() )
        {
            break;
        }
    }

    return true;

    /*m_stdPoint.first++;

    pair<int, int> lowestPoint;
    getOuterPoint(EN_DOWN, lowestPoint);
    if ( lowestPoint.first + 1 == rowEnd )
        return false;

    vector<pair<int, int>> aEmptys;
    vector<pair<int, int>> aBlocks;

    const int rowSize = static_cast<int>(Blocks.size());
    for ( int row = rowEnd-1; row > 0; --row )
    {
        const int colSize = static_cast<int>(Blocks[row].size());
        for ( int col = colBegin+1; col < colEnd; ++col )
        {
            if ( !isFixed(row, col) && Blocks[row][col] == BLOCK )
            {
                if ( row == rowEnd -1 || isFixed(row+1, col) )
                    return false;

                aEmptys.push_back(make_pair(row, col));
                aBlocks.push_back(make_pair(row+1, col));
            }

            if ( t )

        }
    }

    for ( const auto a : aEmptys )
    {
        Blocks[a.first][a.second] = EMPTY;
        print(a.first, a.second);
    }

    for ( const auto a : aBlocks )
    {
        Blocks[a.first][a.second] = BLOCK;
        print(a.first, a.second);
    }*/


    return true;
}


bool CTetrisHandler::rotateToy()
{
    const int curRow = m_stdPoint.first;
    const int curCol = m_stdPoint.second;
    switch ( m_shape )
    {
    case EN_SQUARE:
        break;
    case EN_LINE:
        if ( Blocks[curRow][curCol+1] == BLOCK ) // 가로인 경우
        {
            Blocks[curRow][curCol+1] = EMPTY;
            Blocks[curRow][curCol+2] = EMPTY;
            Blocks[curRow][curCol-1] = EMPTY;

            Blocks[curRow-2][curCol] = BLOCK;
            Blocks[curRow-1][curCol] = BLOCK;
            Blocks[curRow+1][curCol] = BLOCK;
        }
        else // 세로인 경우
        {
            if ( curCol + 2 >= colEnd )
                return false;

            if ( curCol - 1 <= colBegin )
                return false;

            Blocks[curRow][curCol+1] = BLOCK;
            Blocks[curRow][curCol+2] = BLOCK;
            Blocks[curRow][curCol-1] = BLOCK;

            Blocks[curRow-2][curCol] = EMPTY;
            Blocks[curRow-1][curCol] = EMPTY;
            Blocks[curRow+1][curCol] = EMPTY;
        }

        print(curRow, curCol+1);
        print(curRow, curCol+2);
        print(curRow, curCol-1);
        print(curRow-2, curCol);
        print(curRow-1, curCol);
        print(curRow+1, curCol);
        break;
    case EN_MOUNTAIN:
        if ( Blocks[curRow+1][curCol] == EMPTY ) // 위를 향하는 경우
        {
            Blocks[curRow][curCol-1] = EMPTY;
            Blocks[curRow+1][curCol] = BLOCK;
            print(curRow, curCol-1);
            print(curRow+1, curCol);
        }
        else if ( Blocks[curRow-1][curCol] == EMPTY ) // 아래를 향하는 경우
        {
            Blocks[curRow][curCol+1] = EMPTY;
            Blocks[curRow-1][curCol] = BLOCK;
            print(curRow, curCol+1);
            print(curRow-1, curCol);
        }
        else if ( Blocks[curRow][curCol+1] == EMPTY ) // 왼쪽을 향하는 경우
        {
            if ( curCol + 1 >= colEnd )
                return false;

            Blocks[curRow+1][curCol] = EMPTY;
            Blocks[curRow][curCol+1] = BLOCK;
            print(curRow+1, curCol);
            print(curRow, curCol+1);
        }
        else if ( Blocks[curRow][curCol-1] == EMPTY ) // 오른쪽을 향하는 경우
        {
            if ( curCol - 1 <= colBegin )
                return false;

            Blocks[curRow-1][curCol] = EMPTY;
            Blocks[curRow][curCol-1] = BLOCK;
            print(curRow-1, curCol);
            print(curRow, curCol-1);
        }
        break;
    case EN_BODY:
        if ( Blocks[curRow][curCol+1] == BLOCK && Blocks[curRow][curCol+2] == BLOCK ) // 오른쪽으로 긴 모양
        {
            Blocks[curRow][curCol+2] = EMPTY;
            Blocks[curRow-1][curCol] = EMPTY;
            Blocks[curRow+1][curCol] = BLOCK;
            Blocks[curRow+2][curCol] = BLOCK;
            print(curRow, curCol+2);
            print(curRow-1, curCol);
            print(curRow+1, curCol);
            print(curRow+2, curCol);
        }
        else if ( Blocks[curRow+1][curCol] == BLOCK && Blocks[curRow+2][curCol] == BLOCK ) // 아래로 긴 모양
        {
            if ( curCol - 2 <= colBegin )
                return false;

            Blocks[curRow+2][curCol] = EMPTY;
            Blocks[curRow][curCol+1] = EMPTY;
            Blocks[curRow][curCol-1] = BLOCK;
            Blocks[curRow][curCol-2] = BLOCK;
            print(curRow+2, curCol);
            print(curRow, curCol+1);
            print(curRow, curCol-1);
            print(curRow, curCol-2);
        }
        else if ( Blocks[curRow][curCol-1] == BLOCK && Blocks[curRow][curCol-2] == BLOCK ) // 왼쪽으로 긴 모양
        {
            Blocks[curRow][curCol-2] = EMPTY;
            Blocks[curRow+1][curCol] = EMPTY;
            Blocks[curRow-1][curCol] = BLOCK;
            Blocks[curRow-2][curCol] = BLOCK;
            print(curRow, curCol-2);
            print(curRow+1, curCol);
            print(curRow-1, curCol);
            print(curRow-2, curCol);
        }
        else if ( Blocks[curRow-1][curCol] == BLOCK && Blocks[curRow-2][curCol] == BLOCK ) // 위로 긴 모양
        {
            if ( curCol + 2 >= colEnd )
                return false;

            Blocks[curRow-2][curCol] = EMPTY;
            Blocks[curRow][curCol-1] = EMPTY;
            Blocks[curRow][curCol+1] = BLOCK;
            Blocks[curRow][curCol+2] = BLOCK;
            print(curRow-2, curCol);
            print(curRow, curCol-1);
            print(curRow, curCol+1);
            print(curRow, curCol+2);
        }
        break;
    case EN_BODY_R:
        if ( Blocks[curRow][curCol-1] == BLOCK && Blocks[curRow][curCol-2] == BLOCK ) // 왼쪽으로 긴 모양
        {
            if ( curCol + 1 >= colEnd )
                return false;

            Blocks[curRow][curCol-1] = EMPTY;
            Blocks[curRow][curCol-2] = EMPTY;
            Blocks[curRow-2][curCol] = BLOCK;
            Blocks[curRow][curCol+1] = BLOCK;
            print(curRow, curCol-1);
            print(curRow, curCol-2);
            print(curRow-2, curCol);
            print(curRow, curCol+1);
        }
        else if ( Blocks[curRow-1][curCol] == BLOCK && Blocks[curRow-2][curCol] == BLOCK ) // 위로 긴 모양
        {
            if ( curCol + 2 >= colEnd )
                return false;

            Blocks[curRow-2][curCol] = EMPTY;
            Blocks[curRow-1][curCol] = EMPTY;
            Blocks[curRow+1][curCol] = BLOCK;
            Blocks[curRow][curCol+2] = BLOCK;
            print(curRow-2, curCol);
            print(curRow-1, curCol);
            print(curRow+1, curCol);
            print(curRow, curCol+2);
        }
        else if ( Blocks[curRow][curCol+1] == BLOCK && Blocks[curRow][curCol+2] == BLOCK ) // 오른쪽으로 긴 모양
        {
            if ( curCol - 1 <= colBegin )
                return false;

            Blocks[curRow][curCol+2] = EMPTY;
            Blocks[curRow][curCol+1] = EMPTY;
            Blocks[curRow][curCol-1] = BLOCK;
            Blocks[curRow+2][curCol] = BLOCK;
            print(curRow, curCol+2);
            print(curRow, curCol+1);
            print(curRow, curCol-1);
            print(curRow+2, curCol);
        }
        else if ( Blocks[curRow+1][curCol] == BLOCK && Blocks[curRow+2][curCol] == BLOCK ) // 아래로 긴 모양
        {
            if ( curCol - 2 <= colBegin )
                return false;

            Blocks[curRow+2][curCol] = EMPTY;
            Blocks[curRow+1][curCol] = EMPTY;
            Blocks[curRow][curCol-2] = BLOCK;
            Blocks[curRow-1][curCol] = BLOCK;
            print(curRow+2, curCol);
            print(curRow+1, curCol);
            print(curRow, curCol-2);
            print(curRow-1, curCol);
        }
        break;
    case EN_Z:
        if ( Blocks[curRow-1][curCol] == BLOCK )
        {
            if ( curCol - 1 <= colBegin )
                return false;

            Blocks[curRow-1][curCol] = EMPTY;
            Blocks[curRow+1][curCol+1] = EMPTY;
            Blocks[curRow+1][curCol] = BLOCK;
            Blocks[curRow+1][curCol-1] = BLOCK;
            print(curRow-1, curCol);
            print(curRow+1, curCol+1);
            print(curRow+1, curCol);
            print(curRow+1, curCol-1);
        }
        else
        {
            if ( curCol + 1 >= colEnd )
                return false;

            Blocks[curRow+1][curCol] = EMPTY;
            Blocks[curRow+1][curCol-1] = EMPTY;
            Blocks[curRow+1][curCol+1] = BLOCK;
            Blocks[curRow-1][curCol] = BLOCK;
            print(curRow+1, curCol);
            print(curRow+1, curCol-1);
            print(curRow+1, curCol+1);
            print(curRow-1, curCol);

        }
        break;
    case EN_Z_R:
        if ( Blocks[curRow-1][curCol] == BLOCK )
        {
            if ( curCol + 1 >= colEnd )
                return false;

            Blocks[curRow-1][curCol] = EMPTY;
            Blocks[curRow+1][curCol-1] = EMPTY;
            Blocks[curRow+1][curCol] = BLOCK;
            Blocks[curRow+1][curCol+1] = BLOCK;
            print(curRow-1, curCol);
            print(curRow+1, curCol-1);
            print(curRow+1, curCol);
            print(curRow+1, curCol+1);
        }
        else
        {
            if ( curCol - 1 <= colBegin )
                return false;

            Blocks[curRow+1][curCol] = EMPTY;
            Blocks[curRow+1][curCol+1] = EMPTY;
            Blocks[curRow-1][curCol] = BLOCK;
            Blocks[curRow+1][curCol-1] = BLOCK;
            print(curRow+1, curCol);
            print(curRow+1, curCol+1);
            print(curRow-1, curCol);
            print(curRow+1, curCol-1);
        }
        break;
    default:
        break;
    }

    return true;
}

bool CTetrisHandler::getOuterPoint(enDir dir, pair<int, int>& point)
{
    pair<int, int> LeftPoint = {0, colEnd};
    pair<int, int> RightPoint = {0, colBegin};
    pair<int, int> DownPoint = {rowBegin, 0};
    const int rowSize = static_cast<int>(Blocks.size());
    for ( int row = rowEnd-1; row > 0; --row )
    {
        const int colSize = static_cast<int>(Blocks[row].size());
        for ( int col = colEnd-1; col > colBegin; --col )
        {
            if ( !isFixed(row, col) && Blocks[row][col] == BLOCK )
            {
                if ( col < LeftPoint.second )
                    LeftPoint = make_pair(row, col);

                if ( col > RightPoint.second )
                    RightPoint = make_pair(row, col);

                if ( row > DownPoint.first )
                    DownPoint = make_pair(row, col);
            }
        }
    }

    switch ( dir )
    {
    case EN_LEFT:   point = LeftPoint; return true;
    case EN_RIGHT:  point = RightPoint; return true;
    case EN_DOWN:   point = DownPoint; return true;
    case EN_NOTHING:
    case EN_UP:
        return false;
    default:
        return false;
    }
}

bool CTetrisHandler::RemoveFix()
{
    int combo = 0;
    const int rowSize = static_cast<int>(Blocks.size());
    for ( int row = rowEnd-1; row > rowBegin; --row )
    {
        bool bFull = true;
        const int colSize = static_cast<int>(Blocks[row].size());
        for ( int col = colBegin+1; col < colEnd; ++col )
        {
            if ( Fixed[row][col] == false )
            {
                bFull = false;
                break;
            }
        }

        if ( bFull )
        {
            combo++;

            for ( int rowIdx = row; rowIdx > rowBegin; --rowIdx )
            {
                for ( int col = colBegin + 1; col < colEnd; ++col )
                {
                    Fixed[rowIdx][col] = Fixed[rowIdx-1][col];
                    Blocks[rowIdx][col] = Blocks[rowIdx-1][col];
                }
            }
        }
    }

    if ( combo > 0 )
    {
        for ( int row = rowEnd-1; row > rowBegin; --row )
            for ( int col = colBegin+1; col < colEnd; ++col )
                print(row,col);
    }

   /* while ( true )
    {
        bool bFull = true;
        for ( int col = colBegin+1; col < colEnd; ++col )
        {
            if ( Fixed[rowEnd-1][col] == false )
            {
                bFull = false;
                break;
            }
        }

        if ( bFull )
        {
            combo++;

            for ( int row = rowEnd-1; row > 0; --row )
            {
                for ( int col = colBegin+1; col < colEnd; ++col )
                {
                    Fixed[row][col] = Fixed[row-1][col];
                    Blocks[row][col] = Blocks[row-1][col];
                }
            }
        }
        else
            break;
    }

    if ( combo > 0 )
    {
        for ( int row = rowEnd-1; row > 0; --row )
            for ( int col = colBegin+1; col < colEnd; ++col )
                    print(row,col);
    }*/
    

    return true;
}

bool CTetrisHandler::isFixed(int row, int col)
{
    return Fixed[row][col];
}

bool CTetrisHandler::print(int row, int col)
{
    COORD pos;
    pos.X = 2*col;
    pos.Y = row;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);

    cout << Blocks[row][col];
    return true;
}