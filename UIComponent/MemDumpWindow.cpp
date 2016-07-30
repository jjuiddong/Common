
#include "stdafx.h"
#include "MemDumpWindow.h"



cMemDumpWindow::cMemDumpWindow()
	: m_bmpSize(0, 0)
	, m_isDisplayASCII(false)
	, m_isDisplaySymbol(false)
	, m_BlockW(30)
	, m_BlockH(30)
{
	m_brushes[0].CreateSolidBrush(RGB(255, 255, 255));
	m_brushes[1].CreateSolidBrush(RGB(255, 0, 0));
	m_brushes[2].CreateSolidBrush(RGB(0, 0, 0));
}

cMemDumpWindow::~cMemDumpWindow()
{

}


BEGIN_MESSAGE_MAP(cMemDumpWindow, CScrollView)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


void cMemDumpWindow::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	sizeTotal.cx = sizeTotal.cy = 10;
	SetScrollSizes(MM_TEXT, sizeTotal);
}


void cMemDumpWindow::OnDraw(CDC* pDC)
{
	CRect cr;
	GetClientRect(cr);

	CDC memDC;
	CBitmap *pOldBitmap;
	memDC.CreateCompatibleDC(pDC);
	if ((m_bmpSize.cx != cr.Width()) || (m_bmpSize.cy != cr.Height()))
	{
		m_Bitmap.DeleteObject();
		m_Bitmap.CreateCompatibleBitmap(pDC, cr.Width(), cr.Height());
		m_bmpSize = CSize(cr.Width(), cr.Height());
	}
	pOldBitmap = memDC.SelectObject(&m_Bitmap);


	const auto oldBrush = memDC.SelectObject(m_brushes[0]);
	const int cellPerLine = cr.Width() / m_BlockW;
	if (cellPerLine > 0)
	{
		const auto oldObject = memDC.SelectObject(m_brushes[2]);
		memDC.Rectangle(cr);
		memDC.SelectObject(oldObject);

		// 심볼 출력
		if (m_isDisplaySymbol)
		{
			int xIdx = 0;
			int yIdx = 0;
			int symIdx = 0;
			for each (auto &sym in m_symbols)
			{
				int dispBytes = sym.bytes;
				while(dispBytes > 0)
				{
					if (cellPerLine <= xIdx)
					{
						xIdx = 0;
						yIdx++;
					}

					const int dispSize = (cellPerLine > xIdx + dispBytes) ? dispBytes : (cellPerLine - xIdx);
					const int x = xIdx * m_BlockW;
					const int y = yIdx * m_BlockH;
					CRect r(x, y, x + (dispSize*m_BlockW), y + m_BlockH);
					memDC.Rectangle(r);

					if (sym.showMem)
					{
						// 메모리 덤프 출력
						const int startX = xIdx + (yIdx * cellPerLine);
						for (int i = startX; i < startX + dispSize; ++i)
						{
							if ((int)m_dump.size() <= i)
								continue;

							int x = (i % cellPerLine) * m_BlockW;
							int y = (i / cellPerLine) * m_BlockH;

							const auto oldBrush = memDC.SelectObject(m_dump[i].change ? m_brushes[1] : m_brushes[0]);
							memDC.Rectangle(CRect(x, y, x + m_BlockW, y + m_BlockH));

							memDC.SetBkColor(m_dump[i].change ? RGB(255, 0, 0) : RGB(255, 255, 255));
							CString str;
							if (m_isDisplayASCII)
							{
								str.Format(L"%c", (char)m_dump[i].c);
							}
							else
							{
								str.Format(L"%x", m_dump[i].c);
							}
							memDC.TextOutW(x + 5, y + 10, str);
							memDC.SelectObject(oldBrush);
						}


					}
					else
					{
						memDC.SetBkColor(RGB(255, 255, 255));
						CString str;
						str.Format(L"$%d", symIdx+1);
						r.OffsetRect(CPoint(0, 10));
						memDC.DrawText(str, r, DT_CENTER);
					}

					xIdx += dispSize;
					dispBytes -= dispSize;
				}

				++symIdx;
			}

		}
		else
		{
			// 메모리 덤프 출력
			for (u_int i = 0; i < m_dump.size(); ++i)
			{
				int x = (i % cellPerLine) * m_BlockW;
				int y = (i / cellPerLine) * m_BlockH;

				memDC.SelectObject(m_dump[i].change ? m_brushes[1] : m_brushes[0]);
				memDC.Rectangle(CRect(x, y, x + m_BlockW, y + m_BlockH));

				memDC.SetBkColor(m_dump[i].change? RGB(255,0,0):RGB(255,255,255));
				CString str;
				if (m_isDisplayASCII)
				{
					str.Format(L"%c", (char)m_dump[i].c);
				}
				else
				{
					str.Format(L"%x", m_dump[i].c);
				}
				memDC.TextOutW(x+5, y+10, str);
			}
		}
	}
	memDC.SelectObject(oldBrush);

	pDC->BitBlt(0, 0, cr.Width(), cr.Height(), &memDC, 0, 0, SRCCOPY);
	memDC.SelectObject(pOldBitmap);
	memDC.DeleteDC();
}


BOOL cMemDumpWindow::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}


void cMemDumpWindow::UpdateDump(const char *buff, const int size)
{
	if ((int)m_dump.size() != size)
	{
		if (size > 0)
		{
			m_dump.clear();
			m_dump.resize(size);
		}
	}

	RET(size <= 0);

	for (int i = 0; i < size; ++i)
	{
		m_dump[i].change = (m_dump[i].c != *(unsigned char*)&buff[i]);
		m_dump[i].c = buff[i];
	}

	InvalidateRect(NULL);
}


// 화면에 심볼정보를 출력할지를 결정한다.
void cMemDumpWindow::SetDisplaySymbol(const bool isSymbol, cProtocolParser *protocol) 
{ 
	m_isDisplaySymbol = isSymbol;

	// 심볼 정보를 업데이트 한다.
	if (protocol && !protocol->m_fields.empty())
	{
		m_symbols.clear();
		m_symbols.reserve(protocol->m_fields.size());

		for each (auto field in protocol->m_fields)
		{
			sSymbol sym = { field.bytes, false };
			m_symbols.push_back(sym);
		}
	}
}


void cMemDumpWindow::OnLButtonDown(UINT nFlags, CPoint point)
{
	const int symIdx = GetSelectSymbol(point);
	if (symIdx >= 0)
	{
		m_symbols[symIdx].showMem = !m_symbols[symIdx].showMem;
		InvalidateRect(NULL);
	}

	CScrollView::OnLButtonDown(nFlags, point);
}


void cMemDumpWindow::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	const int symIdx = GetSelectSymbol(point);
	if (symIdx >= 0)
	{
		m_symbols[symIdx].showMem = !m_symbols[symIdx].showMem;
		InvalidateRect(NULL);
	}

	CScrollView::OnLButtonDblClk(nFlags, point);
}


// 선택한 심볼 인덱스를 리턴한다. 없다면 -1 리턴
int cMemDumpWindow::GetSelectSymbol(const CPoint &pt)
{
	RETV(!m_isDisplaySymbol, -1);

	CRect cr;
	GetClientRect(cr);
	const int cellPerLine = cr.Width() / m_BlockW;
	if (cellPerLine <= 0)
		return -1;

	int xIdx = 0;
	int yIdx = 0;
	int symIdx = 0;
	for each (auto &sym in m_symbols)
	{
		int dispBytes = sym.bytes;
		while (dispBytes > 0)
		{
			if (cellPerLine <= xIdx)
			{
				xIdx = 0;
				yIdx++;
			}

			const int dispSize = (cellPerLine > xIdx + dispBytes) ? dispBytes : (cellPerLine - xIdx);
			const int x = xIdx * m_BlockW;
			const int y = yIdx * m_BlockH;
			CRect r(x, y, x + (dispSize*m_BlockW), y + m_BlockH);
			if (r.PtInRect(pt))
			{
				// Find!!
				return symIdx;
			}

			xIdx += dispSize;
			dispBytes -= dispSize;
		}

		++symIdx;
	}

	return -1;
}
