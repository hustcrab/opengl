#include <map>
#include <vector>
#include <iostream>

typedef		unsigned int				uint;
typedef		std::vector<int>			CRow;
typedef		std::vector<CRow>			CLabyrinth;

/*

[
0[0	1	2	3	4	5]
1[0	1	2	3	4	5]
2[0	1	2	3	4	5]
]

*/

class CPoint
{

public:

	int	col;
	int	row;

public:

	CPoint(int c = 0, int r = 0)
		: col(c)
		, row(r)
	{
		return;
	}

	CPoint& operator=(const CPoint& pt)
	{
		col = pt.col;
		row = pt.row;
		return *this;
	}

	bool operator==(const CPoint& pt)
	{
		return col == pt.col && row == pt.row;
	}

	bool allRight()
	{
		return col >= 0 && row >= 0;
	}

};

typedef		std::vector<CPoint>			CRoute;

class CLabyrinthAI
{

protected:

	CLabyrinth		m_xLabyrinth;
	CPoint			m_ptBeginning;
	CPoint			m_ptEnding;
	CRoute			m_vRoute;

public:

	enum{ Beginning = -1, Ending = -2 };
	enum{ CanntGo = 0, CanGo = 1 };
	enum{ FoundEnding = 0, NotFoundEnding = 1 };

protected:

	bool isRepeat(const CPoint& pt)
	{
		bool bRes = false;
		CRoute::iterator it = m_vRoute.begin();
		for (; it != m_vRoute.end(); it++){
			CPoint pt0 = *it;
			if (pt0 == pt){
				bRes = true;
				break;
			}
		}
		return bRes;
	}

	void advance(const CPoint& ptTo)
	{
		m_vRoute.push_back(ptTo);
	}

	void back()
	{
		m_vRoute.pop_back();
	}

	bool isBeginning(const CPoint& pt)
	{
		return m_ptBeginning == pt;
	}

	bool isEnding(const CPoint& pt)
	{
		return m_ptEnding == pt;
	}

	CPoint canUp(const CPoint& ptCurrent)
	{
		CPoint ptRes = CPoint(-1, -1);
		int col = ptCurrent.col;
		int row = ptCurrent.row;
		if (row > 0){
			CPoint ptNext = CPoint(col, row - 1);
			if (!isRepeat(ptNext)){
				int nAttr = m_xLabyrinth[ptNext.row][ptNext.col];
				if (nAttr == CanGo || nAttr == Ending){
					ptRes = ptNext;
				}
			}
		}
		return ptRes;
	}

	CPoint canDown(const CPoint& ptCurrent)
	{
		CPoint ptRes = CPoint(-1, -1);
		int col = ptCurrent.col;
		int row = ptCurrent.row;
		if (row < m_xLabyrinth.size() - 1){
			CPoint ptNext = CPoint(col, row + 1);
			if (!isRepeat(ptNext)){
				int nAttr = m_xLabyrinth[ptNext.row][ptNext.col];
				if (nAttr == CanGo || nAttr == Ending){
					ptRes = ptNext;
				}
			}
		}
		return ptRes;
	}

	CPoint canLeft(const CPoint& ptCurrent)
	{
		CPoint ptRes = CPoint(-1, -1);
		int col = ptCurrent.col;
		int row = ptCurrent.row;
		if (col > 0){
			CPoint ptNext = CPoint(col - 1, row);
			if (!isRepeat(ptNext)){
				int nAttr = m_xLabyrinth[ptNext.row][ptNext.col];
				if (nAttr == CanGo || nAttr == Ending){
					ptRes = ptNext;
				}
			}
		}
		return ptRes;
	}

	CPoint canRight(const CPoint& ptCurrent)
	{
		CPoint ptRes = CPoint(-1, -1);
		int col = ptCurrent.col;
		int row = ptCurrent.row;
		if (col < m_xLabyrinth[0].size() - 1){
			CPoint ptNext = CPoint(col + 1, row);
			if (!isRepeat(ptNext)){
				int nAttr = m_xLabyrinth[ptNext.row][ptNext.col];
				if (nAttr == CanGo || nAttr == Ending){
					ptRes = ptNext;
				}
			}
		}
		return ptRes;
	}

	int findRoute(const CPoint& ptCurrent)
	{
		int nRes = NotFoundEnding;
		CPoint ptNext = CPoint(-1, -1);

		advance(ptCurrent);

		//std::cout << "route(" << ptCurrent.row << ", " << ptCurrent.col << ")" << std::endl;
		if (isEnding(ptCurrent)){
			nRes = FoundEnding;
		}
		else{
			ptNext = canUp(ptCurrent);
			if (ptNext.allRight()){
				if (findRoute(ptNext) == FoundEnding){
					nRes = FoundEnding;
					return nRes;
				}
			}
			ptNext = canLeft(ptCurrent);
			if (ptNext.allRight()){
				if (findRoute(ptNext) == FoundEnding){
					nRes = FoundEnding;
					return nRes;
				}
			}
			ptNext = canDown(ptCurrent);
			if (ptNext.allRight()){
				if (findRoute(ptNext) == FoundEnding){
					nRes = FoundEnding;
					return nRes;
				}
			}
			ptNext = canRight(ptCurrent);
			if (ptNext.allRight()){
				if (findRoute(ptNext) == FoundEnding){
					nRes = FoundEnding;
					return nRes;
				}
			}
		}

		if (nRes != FoundEnding){
			back();
		}

		return nRes;
	}

public:

	CLabyrinthAI()
	{
		return;
	}

	CLabyrinthAI(const CLabyrinth& vLabyrinth)
	{
		m_xLabyrinth = vLabyrinth;
		getBeginning();
		getEnding();
	}

	void setLabyrinth(const CLabyrinth& vLabyrinth)
	{
		m_xLabyrinth = vLabyrinth;
	}

	void getBeginning()
	{
		uint nRow = 0;
		for (; nRow < m_xLabyrinth.size(); nRow++){
			CRow xRow = m_xLabyrinth[nRow];
			uint nCol = 0;
			for (; nCol < xRow.size(); nCol++){
				int n = xRow[nCol];
				if (n == Beginning){
					m_ptBeginning = CPoint(nCol, nRow);
					break;
				}
			}
		}
	}

	void getEnding()
	{
		uint nRow = 0;
		for (; nRow < m_xLabyrinth.size(); nRow++){
			CRow xRow = m_xLabyrinth[nRow];
			uint nCol = 0;
			for (; nCol < xRow.size(); nCol++){
				int n = xRow[nCol];
				if (n == Ending){
					m_ptEnding = CPoint(nCol, nRow);
					break;
				}
			}
		}
	}

	void AI()
	{
		findRoute(m_ptBeginning);
		CRoute::iterator it = m_vRoute.begin();
		for (; it != m_vRoute.end(); it++){
			CPoint pt = *it;
			std::cout << "(" << pt.row << ", " << pt.col << ")";
			if (it != m_vRoute.end() - 1){
				std::cout << "->";
			}
			else{
				std::cout << std::endl;
			}
		}
	}

};

int main()
{
	int vLabyrinthArray[][4] = {
		{ 1, -1, 1, 1 }
		, { 1, 1, 1, 1 }
		, { 1, 1, 1, 1 }
		, { 1, 1, 1, 1 }
		, { 1, 1, 1, 1 }
		, { -2, 1, 1, 1 }
	};

	int nRowNum = sizeof(vLabyrinthArray) / sizeof(vLabyrinthArray[0]);
	int nColNum = sizeof(vLabyrinthArray[0]) / sizeof(int);

	CLabyrinth vLabyrinth;
	for (int row = 0; row < nRowNum; row++){
		CRow xRow;
		for (int col = 0; col < nColNum; col++){
			int n = vLabyrinthArray[row][col];
			xRow.push_back(n);
		}
		vLabyrinth.push_back(xRow);
	}

	CLabyrinthAI xAI(vLabyrinth);
	xAI.AI();

	system("Pause");

	return 0;
}
