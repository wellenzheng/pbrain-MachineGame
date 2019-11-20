#include "search.h"
#include <algorithm>

//��ʼ��(���̵�)
void brain_init()
{
	pipeOut("DEBUG brain_init");
	if (width < 5 || width > MAX_BOARD || height < 5 || height > MAX_BOARD)
	{
		pipeOut("ERROR size of the board");
		return;
	}

	//��ʼ������
	boardInit();
	evaluationInit(width, height);
	initHistoryTable(width, height);
	pipeOut("OK");
}

//��ʼ
void brain_restart()
{
	pipeOut("DEBUG brain_restart");
	boardInit();
	evaluationInit(width, height);
	initHistoryTable(width, height);
	pipeOut("OK");
}

int isFree(int x, int y)
{
	return x >= 0 && y >= 0 && x < width && y < height && Square(x, y)->z == EMPTY_MOVE;
}
//AI ����
void brain_my(int x, int y)
{
	pipeOut("DEBUG brain_my [%d,%d]", x, y);
	if (isFree(x, y))
	{
		SetChessOnBoard(x, y, TURN_MOVE);
	}
	else
	{
		pipeOut("ERROR my move [%d,%d]", x, y);
	}
}

//��������
void brain_opponents(int x, int y)
{
	pipeOut("DEBUG brain_opponents [%d , %d]", x, y);
	if (isFree(x, y))
	{
		SetChessOnBoard(x, y, OPPONENT_MOVE);
	}
	else
	{
		pipeOut("ERROR opponents's move [%d,%d]", x, y);
	}
}

//������
void brain_block(int x, int y)
{
	if (isFree(x, y))
	{
		SetChessOnBoard(x, y, OUTSIDE_MOVE);
	}
	else
	{
		pipeOut("ERROR winning move [%d,%d]", x, y);
	}
}

//undo
int brain_takeback(int x, int y)
{
	pipeOut("DEBUG brain_takeback [%d , %d]", x, y);
	if (!isFree(x, y))
	{
		SetChessOnBoard(x, y, EMPTY_MOVE);
		return 0;
	}
	return 2;
}

#define MATCH_SPARE 7 //how much is time spared for the rest of game

//�㷨ֹͣʱ��
DWORD stopTime()
{
	return start_time + std::min(info_timeout_turn, info_time_left / MATCH_SPARE) - 30;
}

//��������
void brain_turn()
{
	pipeOut("DEBUG brain_turn");

	//ʹ��negamax+��������
	IterDeeping();
	//ʹ�ü��������̶����
	//SimpleSearch();
}

void brain_end()
{
	freeResource();
}

//����δʵ��
#ifdef DEBUG_EVAL
#include <windows.h>

void brain_eval(int x, int y)
{
	HDC dc;
	HWND wnd;
	RECT rc;
	char c;
	wnd = GetForegroundWindow();
	dc = GetDC(wnd);
	GetClientRect(wnd, &rc);
	c = (char)(board[x][y] + '0');
	TextOut(dc, rc.right - 15, 3, &c, 1);
	ReleaseDC(wnd, dc);
}

#endif

//������������+NegaMax/alphaBeta����
void IterDeeping()
{
	//��������ʱ��
	int nMinTimer = GetTickCount() + (unsigned int)((std::min(info_timeout_turn, info_time_left / MATCH_SPARE) - 30) * 0.618f);
	reSetHistoryTable(width, height);

	Mov resultMov;
	//������������
	for (int depth = 1; depth < 10; (depth < 4 ? depth *= 2 : depth += 1))
	{
		bestMove.val = -10000;
		pipeOut("DEBUG MAX depth:%d,", depth);

		int score;
		//������ֵ���� or // alpha-beta����
		// score = NegaMax(depth, 0, depth);
		score = alphabeta(depth, -9999, 9999, 0, depth);
		pipeOut("DEBUG Depth value:%d", score);
		pipeOut("DEBUG BestMove:[%d,%d],%d", bestMove.x, bestMove.y, bestMove.val);
		pipeOut("DEBUG Time use:%d,time left:%d", GetTickCount() - start_time, info_timeout_turn - (GetTickCount() - start_time));

		// ��ǿ����ֹ˼����ֹͣ����
		if (terminate)
			break;

		// ��ʱ���Ѿ��ﵽ�涨ʱ���һ�룬������һ���ʱ����ܲ�����ֹͣ������
		if (GetTickCount() > nMinTimer)
			break;

		// �ڹ涨������ڣ�����ɱ�壬ֹͣ˼����
		if (score == 9999)
			break;
	}
	do_mymove(bestMove.x, bestMove.y);
}

//������ֵ����+��ʷ������
int NegaMax(int depth, int player, int MaxDepth)
{
	if (depth <= 0) //Ԥ�в��ϵ�αҶ�ӽ�㣬��������������ֵ
	{
		return evaluate(player);
	}

	int bestMoveIndex = -1;
	int bestVal = -10000;
	int moveListLen = 0;
	Mov *moveList = GenerateMoves(moveListLen, player);

	if (moveListLen == 0)
	{
		pipeOut("gen movelist is empty");
		delete[] moveList;
		moveList = NULL;
		return evaluate(player);
	}
	for (int i = 0; i < moveListLen; i++)
	{
		moveList[i].val = getHistoryScore(moveList[i], player);
	}

	moveList = MergeSort(moveList, moveListLen); //��ʷ��������alpha-betaʱ����

	for (int i = 0; i < moveListLen; i++)
	{
		if (terminate || GetTickCount() >= stopTime())
		{
			pipeOut("DEBUG It's time to terminate");
			break;
		}

		MakeMove(moveList[i], player);

		if (isGameOver(moveList[i])) //���ӽ��Ϊʤ���ѷ�״̬��������Ҷ�ӽ�㣩������player������һ���߷�����ʤ
		{
			if (depth == MaxDepth)
			{
				bestMove = moveList[i];
			}
			UnmakeMove(moveList[i]);
			delete[] moveList;
			moveList = NULL;
			return 9999;
		}

		moveList[i].val = -NegaMax(depth - 1, 1 - player, MaxDepth);

		UnmakeMove(moveList[i]);

		if (bestVal < moveList[i].val)
		{

			bestVal = moveList[i].val;
			bestMoveIndex = i;
			if (depth == MaxDepth)
			{
				bestMove = moveList[i];
			}
		}
	}

	if (bestMoveIndex != -1)
	{
		enterHistoryScore(moveList[bestMoveIndex], depth, player);
	}

	if (depth == MaxDepth)
	{
		bestMove = moveList[bestMoveIndex];
	}
	delete[] moveList;
	moveList = NULL;

	return bestVal;
}

//�̶����+NegaMax����
void SimpleSearch()
{
	Mov resultMov;
	int depth = 4;

	bestMove.val = -10000;
	int score;
	//������ֵ����
	score = NegaMax00(depth, 0, depth);
	pipeOut("DEBUG Depth value:%d", score);
	pipeOut("DEBUG BestMove:[%d,%d],%d", bestMove.x, bestMove.y, bestMove.val);
	do_mymove(bestMove.x, bestMove.y);
}

//������ֵ����
int NegaMax00(int depth, int player, int MaxDepth)
{
	if (depth <= 0) //αҶ�ӽ��
	{
		return evaluate(player);
	}

	int bestMoveIndex = -1;
	int bestVal = -10000;
	int moveListLen = 0;
	Mov *moveList = GenerateMoves(moveListLen, player);

	if (moveListLen == 0)
	{
		pipeOut("gen movelist is empty");
		delete[] moveList;
		moveList = NULL;
		return evaluate(player);
	}

	for (int i = 0; i < moveListLen; i++)
	{
		if (terminate || GetTickCount() >= stopTime())
		{
			pipeOut("DEBUG It's time to terminate");
			break;
		}

		MakeMove(moveList[i], player);

		if (isGameOver(moveList[i])) //���ӽ��Ϊʤ���ѷ�״̬��������Ҷ�ӽ��
		{
			if (depth == MaxDepth)
			{
				bestMove = moveList[i];
			}
			UnmakeMove(moveList[i]);
			delete[] moveList;
			moveList = NULL;
			return 9999;
		}

		moveList[i].val = -NegaMax(depth - 1, 1 - player, MaxDepth);

		UnmakeMove(moveList[i]);

		if (bestVal < moveList[i].val)
		{

			bestVal = moveList[i].val;
			bestMoveIndex = i;
			if (depth == MaxDepth)
			{
				bestMove = moveList[i];
			}
		}
	}

	if (depth == MaxDepth)
	{
		bestMove = moveList[bestMoveIndex];
	}
	delete[] moveList;
	moveList = NULL;

	return bestVal;
}

//alphaBeta����
int alphabeta(int depth, int alpha, int beta, int player, int MaxDepth)
{
	if (depth <= 0)
	{
		return evaluate(player); //游戏结束，返回估值
	}

	int bestMoveIndex = -1;
	int bestVal = -10000;
	int moveListLen = 0;
	Mov *moveList = GenerateMoves(moveListLen, player);

	if (moveListLen == 0)
	{
		pipeOut((char *)"gen movelist is empty");
		delete[] moveList;
		moveList = NULL;
		return evaluate(player);
	}

	for (int i = 0; i < moveListLen; i++)
	{
		moveList[i].val = getHistoryScore(moveList[i], player);
	}

	moveList = MergeSort(moveList, moveListLen);

	for (int i = 0; i < moveListLen; i++) //对每一个可能的走法
	{
		if (terminate || GetTickCount() >= stopTime())
		{
			pipeOut((char *)"DEBUG It's time to terminate");
			break;
		}

		MakeMove(moveList[i], player); //产生子节点，递归搜索子节点

		if (isGameOver(moveList[i]))
		{
			if (depth == MaxDepth)
			{
				bestMove = moveList[i];
			}
			UnmakeMove(moveList[i]);
			delete[] moveList;
			moveList = NULL;
			return 9999;
		}

		moveList[i].val = -alphabeta(depth - 1, -beta, -alpha, 1 - player, MaxDepth);

		UnmakeMove(moveList[i]); //撤销搜索过的子节点

		if (moveList[i].val > alpha)
		{
			alpha = bestVal = moveList[i].val; //保存最大值
			bestMoveIndex = i;
			if (depth == MaxDepth)
			{
				bestMove = moveList[i];
			}
		}
		if (alpha >= beta)
		{
			break; //beta剪枝
		}
	}

	if (bestMoveIndex != -1)
	{
		enterHistoryScore(moveList[bestMoveIndex], depth, player);
	}
	if (depth == MaxDepth)
	{
		bestMove = moveList[bestMoveIndex];
	}
	delete[] moveList;
	moveList = NULL;

	return alpha;
}

//������ǰ���̵Ŀ����岽��ֻ��ȡ���������������������ڵĿ���λ�����ҿ����Ƿ����
Mov *GenerateMoves(int &moveLen, int player)
{
	int range = 2;
	int count = 0;
	Psquare p = boardb;

	Mov *result = new Mov[width * height];

	//��ʼ����־λ�������ظ�����������
	int *makeSign = new int[width * height];
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			makeSign[i * width + j] = 0;
		}
	}

	//��ʼ��ȡ���е���λ
	while (p != boardk)
	{
		if (p->z == TURN_MOVE || p->z == OPPONENT_MOVE)
		{
			for (int i = (p->x - range < 0 ? 0 : p->x - range); i <= (p->x + range > width ? width : p->x + range); i++)
			{
				for (int j = (p->y - range < 0 ? 0 : p->y - range); j <= (p->y + range > height ? height : p->y + range); j++)
				{

					if (Square(i, j)->z == EMPTY_MOVE && makeSign[i * width + j] == 0)
					{
						makeSign[i * width + j] = 1;
						if (isBan(Square(i, j), player)) //�����жϣ�������������жϸ�ѡ���Ƿ���Ҫ�����жϣ��ڼ�����ز���
						{
							continue;
						}
						result[count].x = i;
						result[count].y = j;
						result[count].val = 0;
						count++;
					}
				}
			}
		}
		p = (Psquare)(((char *)p) + sizeof(Tsquare));
	}

	delete[] makeSign;
	//����Ϊ�գ��������ȡһ��λ��
	if (count == 0) //����Ϊ�ջ�����������
	{
		if (Square(0, 0)->z == EMPTY_MOVE)
		{ //����Ϊ�գ������������ѡ��һ��λ��
			srand(GetTickCount());
			int rndX = rand() % width;
			int rndY = rand() % height;
			for (int i = (rndX - range < 0 ? 0 : rndX - range); i <= (rndX + range > width ? width : rndX + range); i++)
			{
				for (int j = (rndY - range < 0 ? 0 : rndY - range); j <= (rndY + range > height ? height : rndY + range); j++)
				{
					makeSign[i * width + j] = 1;
					result[count].x = i;
					result[count].y = j;
					result[count].val = 0;
					count++;
				}
			}
		}
	}

	moveLen = count;
	return result;
}

//
Mov mov_rd(Mov *moveList, int count)
{
	srand(GetTickCount());
	int num = rand() % count;
	//pipeOut("DEBUG rnd :%d",num);
	return moveList[num];
}

//�ж��Ƿ��ǽ��ֹؼ���
bool isBan(Psquare p0, int player)
{
	//������
	if (info_fb_check && player == firstPlayer)
	{
		ChessAnalyzeData checkData[4];
		p0->z = player + 1;
		int fb_type = ForbiddenCheck(checkData, p0);
		p0->z = EMPTY_MOVE;

		if (fb_type != NO_FORBIDDEN)
		{
			return true;
		}
	}
	return false;
}

//�ж�ִ��move������Ƿ����
bool isGameOver(Mov move)
{
	Psquare p0 = Square(move.x, move.y);

	for (int i = 0; i < 4; i++)
	{
		int s = diroff[i];
		Psquare pPrv = p0;
		Psquare pNxt = p0;
		int count = -1;
		do
		{
			prvP(pPrv, 1);
			count++;
		} while (pPrv->z == p0->z);
		do
		{
			nxtP(pNxt, 1);
			count++;
		} while (pNxt->z == p0->z);

		if (count == 5)
			return true;
		else if (count > 5 && !info_exact5)
			return true;
	}
	return false;
}

void freeResource()
{
	delete[] PosValue;
	PosValue = NULL;

	delete[] m_nRecord;
	m_nRecord = NULL;
};