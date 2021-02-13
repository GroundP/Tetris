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

void setCursorPoint(int row, int col)
{
    COORD pos;
    pos.X = 2*col;
    pos.Y = row;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
};

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
    while ( true )
    {
        runUnit();
        printGameOver();
        while ( true )
        {
            if ( _kbhit() )
            {
                int key = _getch();
                if ( key == VK_SPACE )
                {
                    break;
                }
            }
            int a = 0;
        }

        system("cls");
        isGameOver = false;
    }
}

void CTetrisHandler::runUnit()
{
    init();
    initBlocks();
    drawWall();
    printTODO(rowBegin+10, colEnd+3);
    m_Shape = static_cast<en_shape>(getRandomBlock());
    printBestScore();
    printScore();
    printLevel(false);
    createToy(true);

    int levelPoint = 0;
    int speed = 400;
    while ( true )
    {
        printDeadLine();

        if ( !downToy() || bStraightFlag )
        {
            addFix();
            RemoveFix();
            createToy();
            drawNextBlock();
            bStraightFlag = false;

            score += (level + 4);
            printScore();

            levelPoint++;
            if ( levelPoint % 15 == 0 )
            {
                level++;
                if ( speed >= 150 )
                    speed -= 50;

                printLevel();
            }

            if ( checkGameOver() )
            {
                saveScore();
                break;
            }
        }

        Sleep(speed);
    }
}

void CTetrisHandler::init()
{
    bStraightFlag = false;
    score = 0;
    level = 1;
    bestScore = 0; 
    isNewRecord = false;
    isGameOver = false;

    FILE *file = fopen("score.txt", "rt");
    if ( file == 0 )
    {
        bestScore = 0;
    }
    else
    {
        fscanf(file, "%d", &bestScore);
        fclose(file);
    }

    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO Cursor;
    Cursor.bVisible = 0;
    Cursor.dwSize = 1;
    SetConsoleCursorInfo(consoleHandle, &Cursor);

    for ( auto& rowFix : Fixed )
        for ( auto& colFix : rowFix )
            colFix = false;
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
    lock_guard<mutex> lock(mx);

    // Main wall
    for ( int row = rowBegin; row <= rowEnd; ++row )
    {
        auto& rowD = Blocks[row];
        const int colSize = static_cast<int>(rowD.size());
        for ( int col = colBegin; col <= colEnd; ++col )
        {
            auto& colD = rowD[col];

            if ( row == rowEnd )
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

    // Next Block Wall
    const auto b = "─";
    const auto c = "¡";
    setCursorPoint(rowBegin, colEnd+2);   printf(" ─────────── ");
    setCursorPoint(rowBegin+1, colEnd+2); printf("%s", c);
    setCursorPoint(rowBegin+2, colEnd+2); printf("%s", c);
    setCursorPoint(rowBegin+3, colEnd+2); printf("%s", c);
    setCursorPoint(rowBegin+4, colEnd+2); printf("%s", c);
    setCursorPoint(rowBegin+5, colEnd+2); printf(" ─────────── ");
    setCursorPoint(rowBegin+1, colEnd+8); printf("%s", c);
    setCursorPoint(rowBegin+2, colEnd+8); printf("%s", c);
    setCursorPoint(rowBegin+3, colEnd+8); printf("%s", c);
    setCursorPoint(rowBegin+4, colEnd+8); printf("%s", c);
}

void CTetrisHandler::createToy(bool bFirst)
{
    lock_guard<mutex> lock(mx);

    if ( !bFirst )
        m_Shape = m_NextShape;

    array<pair<int, int>,4> arr;
    switch ( m_Shape )
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
    lock_guard<mutex> lock(mx);

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
        if ( !isGameOver && _kbhit() )
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
            else if ( nIuput == VK_ESCAPE )
            {
                system("cls");
                exit(0);
            }
            else
                continue;


            MoveToy(dir);
        }
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
            bStraightFlag = true;
            break;
        }
    }

    return true;
}


bool CTetrisHandler::rotateToy()
{
    if ( bStraightFlag )
        return false;

    lock_guard<mutex> lock(mx);

    const int curRow = m_stdPoint.first;
    const int curCol = m_stdPoint.second;
    switch ( m_Shape )
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

int CTetrisHandler::getRandomBlock()
{
    srand((unsigned int)time(NULL));

    const int random = rand();
    return random % EN_SHAPE_NUM;
}

void CTetrisHandler::drawNextBlock()
{
    lock_guard<mutex> lock(mx);

    int rowPoint = rowBegin + 3; 
    int colPoint = colEnd + 5; 

    for ( int col = -2; col <= 2; ++col )
    {
        Blocks[rowPoint-1][colPoint+col] = EMPTY;
        Blocks[rowPoint][colPoint+col] = EMPTY;
        Blocks[rowPoint+1][colPoint+col] = EMPTY;

        print(rowPoint-1, colPoint+col);
        print(rowPoint, colPoint+col);
        print(rowPoint+1, colPoint+col);
    }

    array<pair<int, int>,4> arr;
    m_NextShape = static_cast<en_shape>(getRandomBlock());
    switch ( m_NextShape )
    {
    case EN_SQUARE:
        arr[0] = make_pair(rowPoint, colPoint);
        arr[1] = make_pair(rowPoint, colPoint+1);
        arr[2] = make_pair(rowPoint-1, colPoint);
        arr[3] = make_pair(rowPoint-1, colPoint+1);
        break;
    case EN_LINE:
        arr[0] = make_pair(rowPoint, colPoint);
        arr[1] = make_pair(rowPoint, colPoint-1);
        arr[2] = make_pair(rowPoint, colPoint+1);
        arr[3] = make_pair(rowPoint, colPoint+2);
        break;
    case EN_MOUNTAIN:
        arr[0] = make_pair(rowPoint, colPoint);
        arr[1] = make_pair(rowPoint-1, colPoint);
        arr[2] = make_pair(rowPoint, colPoint+1);
        arr[3] = make_pair(rowPoint, colPoint-1);
        break;
    case EN_BODY:
        colPoint--;
        arr[0] = make_pair(rowPoint, colPoint);
        arr[1] = make_pair(rowPoint-1, colPoint);
        arr[2] = make_pair(rowPoint, colPoint+1);
        arr[3] = make_pair(rowPoint, colPoint+2);
        break;
    case EN_BODY_R:
        colPoint++;
        arr[0] = make_pair(rowPoint, colPoint);
        arr[1] = make_pair(rowPoint-1, colPoint);
        arr[2] = make_pair(rowPoint, colPoint-1);
        arr[3] = make_pair(rowPoint, colPoint-2);
        break;
    case EN_Z:
        arr[0] = make_pair(rowPoint, colPoint);
        arr[1] = make_pair(rowPoint-1, colPoint);
        arr[2] = make_pair(rowPoint, colPoint+1);
        arr[3] = make_pair(rowPoint+1, colPoint+1);
        break;
    case EN_Z_R:
        colPoint++;
        arr[0] = make_pair(rowPoint, colPoint);
        arr[1] = make_pair(rowPoint-1, colPoint);
        arr[2] = make_pair(rowPoint, colPoint-1);
        arr[3] = make_pair(rowPoint+1, colPoint-1);
        break;
    default:
        break;
    }
 
    Blocks[arr[0].first][arr[0].second] = BLOCK;
    Blocks[arr[1].first][arr[1].second] = BLOCK;
    Blocks[arr[2].first][arr[2].second] = BLOCK;
    Blocks[arr[3].first][arr[3].second] = BLOCK;

    print(arr[0].first, arr[0].second);
    print(arr[1].first, arr[1].second);
    print(arr[2].first, arr[2].second);
    print(arr[3].first, arr[3].second);
}

bool CTetrisHandler::RemoveFix()
{
    lock_guard<mutex> lock(mx);

    int combo = 0;
    const int rowSize = static_cast<int>(Blocks.size());
    for ( int row = rowEnd-1; row > rowBegin; )
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
        else
            --row;
    }

    if ( combo >= 2 )
    {
        for ( int i = 1; i < combo; ++i )
        {
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
        }
    }

    if ( combo > 0 )
    {
        switch ( combo )
        {
        case 1 : score += 100; break;
        case 2 : score += 250; break;
        case 3 : score += 400; break;
        case 4 : score += 550; break;
        default:
            break;
        }

        printScore();

        for ( int row = rowEnd-1; row > rowBegin; --row )
            for ( int col = colBegin+1; col < colEnd; ++col )
                print(row,col);
    }


    return true;
}

bool CTetrisHandler::checkGameOver()
{
    for ( int nIdx = colBegin + 1; nIdx < colEnd; ++nIdx )
    {
        if ( Fixed[rowBegin][nIdx] )
        {
            isGameOver = true;
            return true;
        }
    }

    return false;
}

bool CTetrisHandler::isFixed(int row, int col)
{
    return Fixed[row][col];
}

bool CTetrisHandler::print(int row, int col)
{
    setCursorPoint(row, col);
    cout << Blocks[row][col];
    return true;
}

void CTetrisHandler::printTODO(int row, int col)
{
    lock_guard<mutex> lock(mx);

    setCursorPoint(row++, col);
    setCursorPoint(row++, col);
    cout << "↑: Shift";
    setCursorPoint(row++, col);
    cout << "SPACE: Hard landing";
    setCursorPoint(row++, col);
    cout << "ESC: Quit";

    setCursorPoint(row++, col);
    setCursorPoint(row++, col);
    setCursorPoint(row++, col);
    cout << "// Developed by GroundP.";

}

void CTetrisHandler::printBestScore()
{
    lock_guard<mutex> lock(mx);

    setCursorPoint(rowBegin+7, colEnd+3);
    cout << "Best score : " << bestScore;
}

void CTetrisHandler::printScore()
{
    lock_guard<mutex> lock(mx);

    setCursorPoint(rowBegin+8, colEnd+3);
    cout << "Score : " << score;
}

void CTetrisHandler::printLevel(bool bLevelUp)
{
    lock_guard<mutex> lock(mx);

    setCursorPoint(rowBegin+9, colEnd+3);
    cout << "Level : " << level;

    if ( bLevelUp )
    {
        setCursorPoint(rowBegin+3, colBegin-5);
        cout << "Level Up!";
        setCursorPoint(rowBegin+4, colBegin-5);
        cout << "Speed Up!";

        Sleep(700);
        setCursorPoint(rowBegin+3, colBegin-5);
        cout << "         ";
        setCursorPoint(rowBegin+4, colBegin-5);
        cout << "         ";
    }

}

inline void CTetrisHandler::printDeadLine()
{
    lock_guard<mutex> lock(mx);
        
    setCursorPoint(rowBegin, colBegin+1);
    printf("- - - - - - - - - - - -");
}

void CTetrisHandler::printGameOver()
{
    lock_guard<mutex> lock(mx);

    int row = rowBegin;
    system("cls");
    setCursorPoint(row, colBegin);
    printf("* Game Over !");
    row += 2;
    setCursorPoint(row, colBegin);
    if ( isNewRecord )
    {
        printf("* Congratulation!!! You broke the best score ! (score: %d)", score);
        row += 2;
    }
    else
    {
        printf("* Your score is %d ! (best : %d)", score, bestScore);
        row += 2;
    }
    
    setCursorPoint(row, colBegin);
    printf("* Enter the 'space key' to restart.");
}

void CTetrisHandler::saveScore()
{
    if ( score > bestScore )
    { 
        isNewRecord = true;
        FILE* file = fopen("score.txt", "wt");

        if ( file == 0 )
        {
            setCursorPoint(0, 0);
            printf("FILE ERROR: System cannot write best score on \"score.txt\"");
        }
        else
        {
            fprintf(file, "%d", score);
            fclose(file);
        }
    }
}