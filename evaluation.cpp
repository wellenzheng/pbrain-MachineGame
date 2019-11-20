#include "evaluation.h"

extern int width, height;
/*
*������س�ʼ��
*
*/
void evaluationInit(int width, int height)
{
	//�����¼
	if (m_nRecord)
		delete m_nRecord;
	m_nRecord = new int **[width];
	for (int i = 0; i < width; i++)
	{
		m_nRecord[i] = new int *[height];
		for (int j = 0; j < height; j++)
		{
			m_nRecord[i][j] = new int[4];
		}
	}
}

/*
*���ۺ���
*/

int evaluate(int player)
{
	//pipeOut("DEBUG evaluate");

	int i, j, k;
	//��ʼ�� λ�÷�����¼��
	for (i = 0; i < width; i++)
	{
		for (j = 0; j < height; j++)
		{
			m_nRecord[i][j][0] = TOBEANALYZE;
			m_nRecord[i][j][1] = TOBEANALYZE;
			m_nRecord[i][j][2] = TOBEANALYZE;
			m_nRecord[i][j][3] = TOBEANALYZE;
		}
	}
	//��ʼ�� ������ ������¼��
	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < 7; j++)
		{
			m_nRecordCount[i][j] = 0;
		}
	}

	//pipeOut("DEBUG before analyze");
	//����ͳ�ƺᡢ������б����б������
	for (i = 0; i < width; i++)
	{
		for (j = 0; j < height; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				int s = diroff[k];
				Psquare p0 = Square(i, j);
				if (m_nRecord[i][j][k] == TOBEANALYZE && p0->z != EMPTY_MOVE)
				{
					ChessAnalyzeData *data = new ChessAnalyzeData(); //��¼�������ͳ����Ϣ�Ľṹ

					AnalysisLine(p0, s, data); //ͳ��������Ϣ

					AnalysisBoardType(data, k, m_nRecord, i, j); //����ͳ�Ƶ���Ϣ�������������Ͳ�����λ�÷�����¼��
					delete[] data;
				}
			}
		}
	}

	//pipeOut("DEBUG before count");
	//ͳ�Ƹ����͵�����
	for (i = 0; i < width; i++)
	{
		for (j = 0; j < height; j++)
		{
			Psquare p0 = Square(i, j);

			if (p0->z != EMPTY_MOVE)
			{
				for (k = 0; k < 4; k++)
				{
					int nPlayTurn = p0->z - 1; //��λ�ô��������
					switch (m_nRecord[i][j][k])
					{
					case FIVE: //����
						m_nRecordCount[nPlayTurn][FIVE]++;
						break;
					case FOUR: //����
						m_nRecordCount[nPlayTurn][FOUR]++;
						break;
					case SFOUR: //����
						m_nRecordCount[nPlayTurn][SFOUR]++;
						break;
					case THREE: //����
						m_nRecordCount[nPlayTurn][THREE]++;
						break;
					case STHREE: //����
						m_nRecordCount[nPlayTurn][STHREE]++;
						break;
					case TWO: //���
						m_nRecordCount[nPlayTurn][TWO]++;
						break;
					case TWO * 2: //����������������ֻ������ڷ�������ʱΪ����һ�ӵ������
						m_nRecordCount[nPlayTurn][TWO] += 2;
						break;
					default:
						break;
					}
				}
			}
		}
	}

	//pipeOut("DEBUG before return score");
	int opponentPlayer = 1 - player; //�������

	//���������ֱ�ӷ���ʤ��������ֵ

	//五连，胜利
	if (m_nRecordCount[player][FIVE] > 0)
		return 9999; //�ɹ�
	//对手五连，败北
	if (m_nRecordCount[opponentPlayer][FIVE] > 0)
		return -9999; //ʧ��

	int Turn_Value = 0, Opponent_Turn_Value = 0;

	//我方活四，必胜
	if (m_nRecordCount[player][FOUR])
		return 9990; //��ʤ

	//我方冲四，必胜
	if (m_nRecordCount[player][SFOUR])
	{
		return 9990;
	}

	//对手活四，必输
	if (m_nRecordCount[opponentPlayer][FOUR])
		return -9990;

	//对方冲四，必防
	if (m_nRecordCount[opponentPlayer][SFOUR])
		return -9990;

	if (m_nRecordCount[opponentPlayer][SFOUR] && m_nRecordCount[opponentPlayer][TWO])
		return -9960;

	//我方活三，先攻击，由于前面已排除了四连的情况
	if (m_nRecordCount[player][THREE])
	{
		return 9980;
	}

	//对手活三，必防
	if (m_nRecordCount[opponentPlayer][THREE])
	{
		return -9940;
	}

	// if (m_nRecordCount[player][THREE] > 1 &&
	// 	m_nRecordCount[opponentPlayer][SFOUR] == 0 &&
	// 	m_nRecordCount[opponentPlayer][THREE] == 0 &&
	// 	m_nRecordCount[opponentPlayer][STHREE] == 0)
	// {
	// 	return 9940;
	// }

	

	// if (m_nRecordCount[player][SFOUR])
	// 	Turn_Value += 300;

	// if (m_nRecordCount[opponentPlayer][THREE])
	// {
	// 	Opponent_Turn_Value += m_nRecordCount[opponentPlayer][THREE] * 2000;
	// }
	// else
	// {
	// 	if (m_nRecordCount[opponentPlayer][THREE])
	// 		Opponent_Turn_Value += 200;
	// }

	// if (m_nRecordCount[player][THREE])
	// {
	// 	Turn_Value += m_nRecordCount[player][THREE] * 500;
	// }
	// else
	// {
	// 	if (m_nRecordCount[player][THREE])
	// 		Turn_Value += 100;
	// }

	if (m_nRecordCount[player][STHREE])
		Turn_Value += m_nRecordCount[player][STHREE] * 1000;
	if (m_nRecordCount[opponentPlayer][STHREE])
		Opponent_Turn_Value += m_nRecordCount[opponentPlayer][STHREE] * 500;
	if (m_nRecordCount[player][TWO])
		Turn_Value += m_nRecordCount[player][TWO] * 100;
	if (m_nRecordCount[opponentPlayer][TWO])
		Opponent_Turn_Value += m_nRecordCount[opponentPlayer][TWO] * 10;

	return Turn_Value - Opponent_Turn_Value;
}

//������ѡȡ�����е�����
void AnalysisBoardType(ChessAnalyzeData *data, int direction, int ***m_nRecord, int x, int y)
{
	//pipeOut("DEBUG analysis %d,%d,%d,%d,%d||%d,%d,%d,%d,%d",data->jumpjumpsamePre,data->jumpemptyPre,data->jumpsamePre,data->adjemptyPre,data->adjsamePre, data->adjsameNxt,data->adjemptyNxt, data->jumpsameNxt  , data->jumpemptyNxt ,data->jumpjumpsameNxt);
	int adjsame = data->adjsamePre + data->adjsameNxt - 1;
	int leftEdge, rightEdge;
	int leftRange = data->adjsamePre + data->adjemptyPre + data->jumpsamePre + data->jumpemptyPre + data->jumpjumpsamePre;
	int rightRange = data->adjsameNxt + data->adjemptyNxt + data->jumpsameNxt + data->jumpemptyNxt + data->jumpjumpsameNxt;
	//�϶����ܹ���������ֱ�ӷ���
	if (leftRange + rightRange - 1 < 5)
	{
		//�����ѷ���λ����Ϣ
		//	pipeOut("DEBUG analysisBoardType <5");
		SetAnalyzed(direction, m_nRecord, x, y, leftRange, rightRange);
		return;
	}

	if (adjsame > 5) //����
	{
		//�����ѷ���λ����Ϣ
		SetAnalyzed(direction, m_nRecord, x, y, data->adjsamePre, data->adjsameNxt);
		if (!info_exact5) //�������������������
		{
			//	pipeOut("DEBUG type:five1");
			SetBoardType(direction, m_nRecord, x, y, FIVE);
		}
	}
	else if (adjsame == 5) //����
	{
		//	pipeOut("DEBUG type:five2");
		SetAnalyzed(direction, m_nRecord, x, y, data->adjsamePre, data->adjsameNxt);
		SetBoardType(direction, m_nRecord, x, y, FIVE);
	}
	else if (adjsame == 4) //����
	{
		leftEdge = data->adjsamePre;
		rightEdge = data->adjsameNxt;
		SetAnalyzed(direction, m_nRecord, x, y, leftEdge, rightEdge);
		//?****?
		//����
		if ((data->adjemptyPre == 0 && data->adjemptyNxt >= 1) || (data->adjemptyNxt == 0 && data->adjemptyPre >= 1)) //o****_ ,  _****o
		{
			//	pipeOut("DEBUG type:SFOUR o****_ ,  _****o");
			SetBoardType(direction, m_nRecord, x, y, SFOUR);
		}
		//����
		else if (data->adjemptyNxt >= 1 && data->adjemptyPre >= 1) //_****_
		{
			//	pipeOut("DEBUG type:FOUR _****_");
			SetBoardType(direction, m_nRecord, x, y, FOUR);
		}
	}
	else if (adjsame == 3) //����
	{
		//����
		if (data->adjemptyPre == 1 && data->jumpsamePre >= 1) //*_***
		{
			//	pipeOut("DEBUG type:SFOUR *_***");
			leftEdge = data->adjsamePre + data->adjemptyPre + 1;
			rightEdge = data->adjsameNxt;
			SetAnalyzed(direction, m_nRecord, x, y, leftEdge, rightEdge);
			SetLeftBoardType(direction, m_nRecord, x, y, data->adjsamePre, SFOUR);
		}
		if (data->adjemptyNxt == 1 && data->jumpsameNxt >= 1) //***_*
		{
			//	pipeOut("DEBUG type:SFOUR ***_*");
			leftEdge = data->adjsamePre;
			rightEdge = data->adjsameNxt + data->adjemptyNxt + 1;
			SetAnalyzed(direction, m_nRecord, x, y, leftEdge, rightEdge);
			SetRightBoardType(direction, m_nRecord, x, y, data->adjsameNxt, SFOUR);
		}

		//活三
		//if ((data->adjemptyPre > 2 || data->adjemptyPre == 2 && data->jumpsamePre == 0) && (data->adjemptyNxt > 1 || data->adjemptyNxt == 1 && data->jumpsameNxt == 0))
		//___***__ , ___***_o ,o__***__ , o__***_o
		if (data->adjemptyPre >= 2 && data->adjemptyNxt >= 1)
		{ //__***_
			//	pipeOut("DEBUG type:THREE __***_");
			leftEdge = data->adjsamePre;
			rightEdge = data->adjsameNxt;
			SetAnalyzed(direction, m_nRecord, x, y, leftEdge, rightEdge);
			SetLeftBoardType(direction, m_nRecord, x, y, data->adjsamePre, THREE);
		}
		// if ((data->adjemptyNxt > 2 || data->adjemptyNxt == 2 && data->jumpsameNxt == 0) && data->adjemptyPre > 1 || data->adjemptyPre == 1 && data->jumpsamePre == 0)
		//__***___ , o_***___ , __***__o , o_***__o
		if (data->adjemptyNxt >= 2 && data->adjemptyPre >= 1)
		{ //_***__
			//pipeOut("DEBUG type:THREE _***__");
			leftEdge = data->adjsamePre;
			rightEdge = data->adjsameNxt;
			SetAnalyzed(direction, m_nRecord, x, y, leftEdge, rightEdge);
			SetRightBoardType(direction, m_nRecord, x, y, data->adjsameNxt, THREE);
		}

		//����
		if (data->adjemptyPre == 0 && data->adjemptyNxt >= 2 || data->adjemptyNxt == 0 && data->adjemptyPre >= 2 || data->adjemptyPre == 1 && data->jumpsamePre == 0 && data->adjemptyNxt == 1 && data->jumpsameNxt == 0)
		{ // __***o , o***__ , o_***_o
			//pipeOut("DEBUG type:STHREE __***o , o***__ , o_***_o");
			leftEdge = data->adjsamePre;
			rightEdge = data->adjsameNxt;
			SetAnalyzed(direction, m_nRecord, x, y, leftEdge, rightEdge);
			SetBoardType(direction, m_nRecord, x, y, STHREE);
		}
	}
	else if (adjsame == 2) //二连
	{

		bool left = false;
		bool right = false;
		//冲四 0110110
		if (data->adjemptyPre == 1 && data->jumpsamePre >= 2)
		{ //��**_**
			//	pipeOut("DEBUG type:SFOUR left**_**");
			leftEdge = data->adjsamePre + data->adjemptyPre + 2;
			rightEdge = data->adjsameNxt;
			SetAnalyzed(direction, m_nRecord, x, y, leftEdge, rightEdge);
			SetLeftBoardType(direction, m_nRecord, x, y, data->adjsamePre, SFOUR);
			left = true;
		}
		if (data->adjemptyNxt == 1 && data->jumpsameNxt >= 2)
		{ //��**_**
			//	pipeOut("DEBUG type:SFOUR right**_**");
			leftEdge = data->adjsamePre;
			rightEdge = data->adjsameNxt + data->adjemptyNxt + 2;
			SetAnalyzed(direction, m_nRecord, x, y, leftEdge, rightEdge);
			SetRightBoardType(direction, m_nRecord, x, y, data->adjsameNxt, SFOUR);
			right = true;
		}
		//活三 010110
		if (data->adjemptyPre == 1 && data->jumpsamePre == 1 && data->jumpemptyPre >= 1 && data->adjemptyNxt >= 1)
		{ //_*_**_
			//	pipeOut("DEBUG type:THREE _*_**_");
			leftEdge = data->adjsamePre + data->adjemptyPre + data->jumpsamePre;
			rightEdge = data->adjsameNxt;
			SetAnalyzed(direction, m_nRecord, x, y, leftEdge, rightEdge);
			SetLeftBoardType(direction, m_nRecord, x, y, data->adjsamePre, THREE);
			left = true;
		}
		if (data->adjemptyNxt == 1 && data->jumpsameNxt == 1 && data->jumpemptyNxt >= 1 && data->adjemptyPre >= 1)
		{ //_**_*_
			//	pipeOut("DEBUG type:THREE _**_*_");
			leftEdge = data->adjsamePre;
			rightEdge = data->adjsameNxt + data->adjemptyNxt + data->jumpsameNxt;
			SetAnalyzed(direction, m_nRecord, x, y, leftEdge, rightEdge);
			SetRightBoardType(direction, m_nRecord, x, y, data->adjsameNxt, THREE);
			right = true;
		}
		//眠三 010112
		if (data->adjemptyPre == 1 && data->jumpsamePre == 1 && data->jumpemptyPre >= 1 && data->adjemptyNxt == 0)
		{ //_*_**o
			//	pipeOut("DEBUG type:STHREE _*_**o");
			leftEdge = data->adjsamePre + data->adjemptyPre + data->jumpsamePre;
			rightEdge = data->adjsameNxt;
			SetAnalyzed(direction, m_nRecord, x, y, leftEdge, rightEdge);
			SetLeftBoardType(direction, m_nRecord, x, y, data->adjsamePre, STHREE);
			left = true;
		}
		if (data->adjemptyNxt == 1 && data->jumpsameNxt == 1 && data->jumpemptyNxt >= 1 && data->adjemptyPre == 0)
		{ //o**_*_
			//	pipeOut("DEBUG type:STHREE o**_*_");
			leftEdge = data->adjsamePre;
			rightEdge = data->adjsameNxt + data->adjemptyNxt + data->jumpsameNxt;
			SetAnalyzed(direction, m_nRecord, x, y, leftEdge, rightEdge);
			SetRightBoardType(direction, m_nRecord, x, y, data->adjsameNxt, STHREE);
			right = true;
		}
		// 011012
		// if (data->adjemptyPre == 1 && data->jumpsamePre == 1 && data->jumpemptyPre == 0 && data->adjemptyNxt >= 1)
		// {
		// 	leftEdge = data->adjsameNxt;
		// 	rightEdge = data->adjsamePre + data->jumpemptyPre + data->jumpsamePre;
		// 	SetAnalyzed(direction, m_nRecord, x, y, leftEdge, rightEdge);
		// 	SetRightBoardType(direction, m_nRecord, x, y, data->adjsamePre, STHREE);
		// }
		// if (data->adjemptyNxt == 1 && data->jumpsameNxt == 1 && data->jumpemptyNxt == 0 && data->adjemptyPre >= 1)
		// {
		// 	leftEdge = data->adjsameNxt + data->jumpemptyNxt + data->jumpsameNxt;
		// 	rightEdge = data->adjsamePre;
		// 	SetAnalyzed(direction, m_nRecord, x, y, leftEdge, rightEdge);
		// 	SetRightBoardType(direction, m_nRecord, x, y, data->adjsameNxt, STHREE);
		// }

		//活二
		if (data->adjemptyPre != 0 && data->adjemptyNxt != 0 && data->adjemptyPre + data->adjemptyNxt >= 4)
		{ //_**___ , __**__ , ___**_
			//	pipeOut("DEBUG type:TWO _**___ , __**__ , ___**_");
			if (left && data->adjemptyPre == 1)
			{
			}
			if (right && data->adjemptyNxt == 1)
			{
			}
			else
			{
				leftEdge = data->adjsamePre;
				rightEdge = data->adjsameNxt;
				SetAnalyzed(direction, m_nRecord, x, y, leftEdge, rightEdge);
				SetBoardType(direction, m_nRecord, x, y, TWO);
			}
		}
		//�߶��������жϣ���Ϊ��ֵ����
	}
	else if (adjsame == 1) //����һ��
	{
		//���ġ�������������ֱ�Ӻ��ԣ���Ϊ����ͨ�����ϵ��������������

		//���
		if (data->adjemptyPre == 1 && data->jumpsamePre == 1 && data->jumpemptyPre != 0 && data->adjemptyNxt != 0 && data->jumpemptyPre + data->adjemptyNxt >= 3)
		{ //__*_*_ , _*_*__
			//	pipeOut("DEBUG type:TWO __*_*_ , _*_*__");
			leftEdge = data->adjsamePre + data->adjemptyPre + data->jumpsamePre;
			rightEdge = data->adjsameNxt;
			SetAnalyzed(direction, m_nRecord, x, y, leftEdge, rightEdge);
			SetBoardType(direction, m_nRecord, x, y, TWO);
		}
		if (data->adjemptyNxt == 1 && data->jumpsameNxt == 1 && data->jumpemptyNxt != 0 && data->adjemptyPre != 0 && data->jumpemptyNxt + data->adjemptyPre >= 3)
		{ //__*_*_ , _*_*__
			//		pipeOut("DEBUG type:TWO __*_*_ , _*_*__");
			leftEdge = data->adjsamePre;
			rightEdge = data->adjsameNxt + data->adjemptyNxt + data->jumpsameNxt;
			SetAnalyzed(direction, m_nRecord, x, y, leftEdge, rightEdge);
			if (isRecordedTwo(direction, m_nRecord, x, y))
			{
				SetBoardType(direction, m_nRecord, x, y, TWO * 2); //����ֻ��һ�������¼������������Ͳ��������Է���
			}
			SetBoardType(direction, m_nRecord, x, y, TWO);
		}
		if (data->adjemptyPre == 2 && data->jumpsamePre == 1 && data->jumpemptyPre > 0 && data->adjemptyNxt > 0)
		{ //_*__*_
			//		pipeOut("DEBUG type:TWO _*__*_");
			leftEdge = data->adjsamePre + data->adjemptyPre + data->jumpsamePre;
			rightEdge = data->adjsameNxt;
			SetAnalyzed(direction, m_nRecord, x, y, leftEdge, rightEdge);
			SetBoardType(direction, m_nRecord, x, y, TWO);
		}
		if (data->adjemptyNxt == 2 && data->jumpsameNxt == 1 && data->jumpemptyNxt > 0 && data->adjemptyPre > 0)
		{ // _*__*_
			//		pipeOut("DEBUG type:TWO  _*__*_");
			leftEdge = data->adjsamePre;
			rightEdge = data->adjsameNxt + data->adjemptyNxt + data->jumpsameNxt;
			SetAnalyzed(direction, m_nRecord, x, y, leftEdge, rightEdge);
			if (isRecordedTwo(direction, m_nRecord, x, y))
			{
				SetBoardType(direction, m_nRecord, x, y, TWO * 2); //����ֻ��һ�������¼������������Ͳ��������Է���
			}
			SetBoardType(direction, m_nRecord, x, y, TWO);
		}
	}
}

//�����ѷ�������λ��¼
void SetAnalyzed(int direction, int ***m_nRecord, int x, int y, int leftEdge, int rightEdge)
{
	m_nRecord[x][y][direction] = ANALYZED;
	switch (direction)
	{
	case UP_DOWN:
		for (int i = 1; i < leftEdge; i++)
		{
			m_nRecord[x][y - i][UP_DOWN] = ANALYZED;
		}
		for (int i = 1; i < rightEdge; i++)
		{
			m_nRecord[x][y + i][UP_DOWN] = ANALYZED;
		}
		break;
	case LEFTUP_RIGHTDOWN:
		for (int i = 1; i < leftEdge; i++)
		{
			m_nRecord[x - i][y - i][LEFTUP_RIGHTDOWN] = ANALYZED;
		}
		for (int i = 1; i < rightEdge; i++)
		{
			m_nRecord[x + i][y + i][LEFTUP_RIGHTDOWN] = ANALYZED;
		}
		break;
	case LEFT_RIGHT:
		for (int i = 1; i < leftEdge; i++)
		{
			m_nRecord[x - i][y][LEFT_RIGHT] = ANALYZED;
		}
		for (int i = 1; i < rightEdge; i++)
		{
			m_nRecord[x + i][y][LEFT_RIGHT] = ANALYZED;
		}
		break;
	case RIGHTUP_LEFTDOWN:
		for (int i = 1; i < leftEdge; i++)
		{
			m_nRecord[x - i][y + i][RIGHTUP_LEFTDOWN] = ANALYZED;
		}
		for (int i = 1; i < rightEdge; i++)
		{
			m_nRecord[x + i][y - i][RIGHTUP_LEFTDOWN] = ANALYZED;
		}

		break;
	}
}

//�ڵ�ǰλ����������
void SetBoardType(int direction, int ***m_nRecord, int x, int y, int type)
{
	m_nRecord[x][y][direction] = type;
}

//�ж��Ƿ�������������������ֻ������ڷ�������ʱΪ����һ�ӵ������
bool isRecordedTwo(int direction, int ***m_nRecord, int x, int y)
{
	if (m_nRecord[x][y][direction] == TWO)
	{
		return true;
	}
	return false;
}

//������������
void SetLeftBoardType(int direction, int ***m_nRecord, int x, int y, int leftEdge, int type)
{
	switch (direction)
	{
	case UP_DOWN:
		m_nRecord[x][y - (leftEdge - 1)][UP_DOWN] = type;
		break;
	case LEFTUP_RIGHTDOWN:
		m_nRecord[x - (leftEdge - 1)][y - (leftEdge - 1)][LEFTUP_RIGHTDOWN] = type;
		break;
	case LEFT_RIGHT:
		m_nRecord[x - (leftEdge - 1)][y][LEFT_RIGHT] = type;
		break;
	case RIGHTUP_LEFTDOWN:
		m_nRecord[x - (leftEdge - 1)][y + (leftEdge - 1)][RIGHTUP_LEFTDOWN] = type;
		break;
	}
}
//������������
void SetRightBoardType(int direction, int ***m_nRecord, int x, int y, int rightEdge, int type)
{
	switch (direction)
	{
	case UP_DOWN:
		m_nRecord[x][y + (rightEdge - 1)][UP_DOWN] = type;
		break;
	case LEFTUP_RIGHTDOWN:
		m_nRecord[x + (rightEdge - 1)][y + (rightEdge - 1)][LEFTUP_RIGHTDOWN] = type;
		break;
	case LEFT_RIGHT:
		m_nRecord[x + (rightEdge - 1)][y][LEFT_RIGHT] = type;
		break;
	case RIGHTUP_LEFTDOWN:
		m_nRecord[x + (rightEdge - 1)][y - (rightEdge - 1)][RIGHTUP_LEFTDOWN] = type;
		break;
	}
}
