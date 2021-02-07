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
        //showBlocks();
        const auto& downRes = downToy();
        if ( !downRes )
        {
            addFix();
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
                break;
            }
        }
        else
            return false;
        

        MoveToy(dir);
    }

    return true;
}

bool CTetrisHandler::MoveToy(enDir dir)
{
    switch ( dir )
    {
    case EN_LEFT: leftToy(); break;
    case EN_RIGHT: rightToy(); break;
    case EN_UP: rotateToy(); break;
    case EN_DOWN: downToy(); break;
    case EN_NOTHING: break;
    default:
        return false;
    }

    
    return true;
}

bool CTetrisHandler::downToy()
{
    m_stdPoint.first++;
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

                print(row, col);
                print(row+1, col);
            }
        }
    }

    return true;
}

bool CTetrisHandler::leftToy()
{
    m_stdPoint.second--;
    const int rowSize = static_cast<int>(Blocks.size());
    for ( int row = rowEnd; row > 0; --row )
    {
        const int colSize = static_cast<int>(Blocks[row].size());
        for ( int col = colBegin; col < colEnd; ++col )
        {
            if ( !isFixed(row, col) && Blocks[row][col] == BLOCK )
            {
                if ( col == colBegin +1 || isFixed(row, col-1))
                    return false;

                Blocks[row][col-1] = BLOCK;
                Blocks[row][col] = EMPTY;

                print(row, col-1);
                print(row, col);
            }
        }
    }

    return true;
}

bool CTetrisHandler::rightToy()
{
    m_stdPoint.second++;
    const int rowSize = static_cast<int>(Blocks.size());
    for ( int row = rowEnd; row > 0; --row )
    {
        const int colSize = static_cast<int>(Blocks[row].size());
        for ( int col = colEnd; col > colBegin; --col )
        {
            if ( !isFixed(row, col) && Blocks[row][col] == BLOCK )
            {
                if ( col == colEnd -1 || isFixed(row, col+1))
                    return false;

                Blocks[row][col+1] = BLOCK;
                Blocks[row][col] = EMPTY;

                print(row, col+1);
                print(row, col);
            }
        }
    }

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
        if ( Blocks[curRow][curCol + 1] == BLOCK ) // 가로인 경우
        {
            Blocks[curRow][curCol + 1] = EMPTY;
            Blocks[curRow][curCol + 2] = EMPTY;
            Blocks[curRow][curCol - 1] = EMPTY;

            Blocks[curRow-2][curCol] = BLOCK;
            Blocks[curRow-1][curCol] = BLOCK;
            Blocks[curRow+1][curCol] = BLOCK;
        }
        else // 세로인 경우
        {
            Blocks[curRow][curCol + 1] = BLOCK;
            Blocks[curRow][curCol + 2] = BLOCK;
            Blocks[curRow][curCol - 1] = BLOCK;

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
        else if ( Blocks[curRow - 1][curCol] == EMPTY ) // 아래를 향하는 경우
        {
            Blocks[curRow][curCol+1] = EMPTY;
            Blocks[curRow-1][curCol] = BLOCK;
            print(curRow, curCol+1);
            print(curRow-1, curCol);
        }
        else if ( Blocks[curRow][curCol + 1] == EMPTY ) // 왼쪽을 향하는 경우
        {
            Blocks[curRow+1][curCol] = EMPTY;
            Blocks[curRow][curCol+1] = BLOCK;
            print(curRow+1, curCol);
            print(curRow, curCol+1);
        }
        else if ( Blocks[curRow][curCol-1] == EMPTY ) // 오른쪽을 향하는 경우
        {
            Blocks[curRow-1][curCol] = EMPTY;
            Blocks[curRow][curCol-1] = BLOCK;
            print(curRow-1, curCol);
            print(curRow, curCol-1);
        }
        break;
    case EN_BODY:
        break;
    case EN_BODY_R:
        break;
    case EN_Z:
        if ( Blocks[curRow - 1][curCol] == BLOCK )
        {
            Blocks[curRow - 1][curCol] = EMPTY;
            Blocks[curRow + 1][curCol+1] = EMPTY;
            Blocks[curRow + 1][curCol] = BLOCK;
            Blocks[curRow + 1][curCol-1] = BLOCK;
            print(curRow-1, curCol);
            print(curRow+1, curCol+1);
            print(curRow+1, curCol);
            print(curRow+1, curCol-1);
        }
        else
        {
            Blocks[curRow + 1][curCol] = EMPTY;
            Blocks[curRow + 1][curCol-1] = EMPTY;
            Blocks[curRow + 1][curCol+1] = BLOCK;
            Blocks[curRow - 1][curCol] = BLOCK;
            print(curRow+1, curCol);
            print(curRow+1, curCol-1);
            print(curRow+1, curCol+1);
            print(curRow-1, curCol);

        }
        break;
    case EN_Z_R:
        break;
    default:
        break;
    }

    return true;
}

bool CTetrisHandler::isFixed(int row, int col)
{
    return std::count(Fixed.begin(), Fixed.end(), make_pair(row, col)) > 0;
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