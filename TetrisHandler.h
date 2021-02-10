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
    void initBlocks(); // ��� �ʱ�ȭ
    void drawWall(); // �� �����
    void createToy(); // ù ��� �����

    void addFix(); // �̹� ������ ��� FIx
    bool InputDir(); // ����Ű �Է¹ޱ�
    bool MoveToy(enDir dir); // ����Ű�� ���� �̵�

    bool downToy(); // ��� ������
    bool leftToy(); // ��� ��� �̵�
    bool rightToy(); // ��� �·� �̵�
    bool straight(); // ��� �ѹ濡 ������ ������
    bool rotateToy(); // ��� ȸ��
    bool getOuterPoint(enDir dir, std::pair<int, int>& point); // ��� ������/��/�� �̵��� ���� ������ ����Ʈ ��ȯ

    bool RemoveFix();

    bool isFixed(int row, int col);
    bool print(int row, int col);

    std::array<std::array<std::string, totalSize>, totalSize> Blocks;
    std::map<en_shape, std::pair<int, int>> Standard;
    //std::vector<std::pair<int, int>> Fixed;
    std::array<std::array<bool, totalSize>, totalSize> Fixed;
    en_shape m_shape; // ���� ����� ���

    std::mutex mx;
    std::thread InputThread;
    std::condition_variable cond;

    std::pair<int, int> m_stdPoint; // ���� ����� ������
};


