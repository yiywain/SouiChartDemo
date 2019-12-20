#include "stdafx.h"
#include "SCoordAxis.h"


SCoordAxis::SCoordAxis():
m_CoordMargin(50,50,50,50),
m_ScaleNumber(5),
m_LowerValue(0),
m_IsAutoScope(true),
m_ReserveValue(0.15),
m_CoordWidth(1),
m_XScaleLength(1),
m_YScaleLength(1),
m_ScaleLineColor(RGBA(200, 200, 200, 255)),
m_ScaleLineWidth(1),
m_Decimal(0),
m_CoordColor(RGBA(0, 0, 0, 255)),
m_AxisType(AXIS_LENGTHWAYS),
m_PullOver(true),
m_ShowOrigin(true)
{
	m_TextOffset = 8; 
}


SCoordAxis::~SCoordAxis()
{


}

void SCoordAxis::OnPaint(IRenderTarget *pRT)
{
	//绘制轴线
	SPainter painter;
	BeforePaint(pRT, painter);

	CAutoRefPtr<IRenderObj> curPen = pRT->GetCurrentObject(OT_PEN);

	//计算矩形
	CRect rcClient = GetClientRect();
	CRect rectDraw = rcClient;
	
	//绘制坐标轴
	CRect Margin = m_CoordMargin;
	POINT Coord[3];
	Coord[0].x = rectDraw.left + Margin.left;
	Coord[0].y = rectDraw.top + Margin.top;
	Coord[1].x = Coord[0].x;
	Coord[1].y = rectDraw.bottom - Margin.bottom;
	Coord[2].x = rectDraw.right - Margin.right;
	Coord[2].y = Coord[1].y;


	m_DataRect.left = Coord[0].x + m_CoordWidth;
	m_DataRect.top = Coord[0].y;
	m_DataRect.right = Coord[2].x - m_CoordWidth;
	m_DataRect.bottom = Coord[1].y;

	CAutoRefPtr<SOUI::IPen> Pen;
	pRT->CreatePen(PS_SOLID, m_CoordColor, m_CoordWidth, &Pen);
	pRT->SelectObject(Pen);
	pRT->DrawLines(Coord,3);


	int Count = m_Text.GetCount();

	//获取数值的最小值和最大值
	m_MinValue = m_MaxValue = 0;
	GetMaxMin(m_MaxValue, m_MinValue);

	if (m_IsAutoScope)
	{
		m_MinValue -= m_ReserveValue>1 ? m_ReserveValue : m_ReserveValue*(m_MaxValue - m_MinValue);
		m_MaxValue += m_ReserveValue>1 ? m_ReserveValue : m_ReserveValue*(m_MaxValue - m_MinValue);
	}
	else
		m_MinValue = m_LowerValue;

	m_ValueSpace = (m_MaxValue - m_MinValue) / (float)(m_ScaleNumber);//标线间距数值
	
	m_XSignRatio = 0;//X方向标线比例
	m_XCount = 0;
	m_YSignRatio = 0;//Y方向标线比例
	m_YCount = 0;
	m_ValueRatio=0;//数值比例
	//纵向
	if (m_AxisType == AXIS_LENGTHWAYS)
	{
		m_XCount = m_PullOver ? Count - 1 : Count + 1;
		m_YCount = m_ScaleNumber;
		m_XSignRatio = m_DataRect.Width() / (float)m_XCount;
		m_YSignRatio = m_DataRect.Height() / (float)m_YCount;
		m_ValueRatio = m_DataRect.Height() / (m_MaxValue - m_MinValue);
	}//横向
	else if (m_AxisType==AXIS_CROSSWISE)
	{
		m_XCount = m_ScaleNumber;
		m_YCount = m_PullOver ? Count - 1 : Count + 1;
		m_XSignRatio = m_DataRect.Width() / (float)m_XCount;
		m_YSignRatio = m_DataRect.Height() / (float)m_YCount;
		m_ValueRatio = m_DataRect.Width() / (m_MaxValue - m_MinValue);
	}

	Pen.Release();
	pRT->CreatePen(PS_SOLID, m_ScaleLineColor, m_ScaleLineWidth, &Pen);
	pRT->SelectObject(Pen);

	//绘制X轴上标线与文字
	int n = 0;
	for (int i = 0; i <= m_XCount&&m_XCount>0&& m_XScaleLength>0; i++)
	{
		POINT Coord[2] = {0};
		Coord[0].x = m_DataRect.left + m_XSignRatio*i;
		Coord[0].y = m_DataRect.bottom - (m_XScaleLength > 1.0 ? m_XScaleLength : m_XScaleLength*m_DataRect.Height());
		Coord[1].x = Coord[0].x;
		Coord[1].y = m_DataRect.bottom;

		if (i > 0)
			pRT->DrawLines(Coord, 2);
		
		SIZE size;
		if (m_AxisType == AXIS_LENGTHWAYS && ((m_PullOver&&i>=0) || (!m_PullOver&&i>0)) && n<Count)
		{
			SStringT *pStr = &m_Text[n++];
			pRT->MeasureText(*pStr, pStr->GetLength(),&size);
			pRT->TextOut(Coord[1].x - size.cx / 2, Coord[1].y + m_TextOffset, *pStr, -1);
		}
		else if (m_AxisType == AXIS_CROSSWISE&&(m_ShowOrigin||i>0))
		{
			SStringT Str;
			NumberToScaleStr(m_MinValue + m_ValueSpace*(i), m_Decimal, Str);
			pRT->MeasureText(Str, Str.GetLength(), &size);
			pRT->TextOut(Coord[1].x - size.cx / 2, Coord[1].y + m_TextOffset, Str, -1);
		}
	}

	//绘制Y轴上标线与文字
	n = 0;
	for (int i = 0; i <= m_YCount&&m_YCount>0 && m_YScaleLength>0; i++)
	{
		POINT Coord[2] = { 0 };
		Coord[0].x = m_DataRect.left;
		Coord[0].y = m_DataRect.bottom - m_YSignRatio*i;
		Coord[1].x = Coord[0].x + (m_YScaleLength > 1.0 ? m_YScaleLength : m_YScaleLength*m_DataRect.Width());
		Coord[1].y = Coord[0].y;

		if (i>0)
			pRT->DrawLines(Coord, 2);

		SIZE size;
		if (m_AxisType == AXIS_CROSSWISE && ((m_PullOver&&i>=0) || (!m_PullOver&&i>0)) && n<Count)
		{
			SStringT *pStr = &m_Text[n++];
			pRT->MeasureText(*pStr, pStr->GetLength(), &size);
			pRT->TextOut(Coord[0].x - size.cx - m_TextOffset, Coord[0].y - size.cy / 2, *pStr, -1);
		}
		else if (m_AxisType == AXIS_LENGTHWAYS && (m_ShowOrigin || i>0))
		{
			SStringT Str;
			NumberToScaleStr(m_MinValue + m_ValueSpace*(i), m_Decimal, Str);
			pRT->MeasureText(Str, Str.GetLength(), &size);
			pRT->TextOut(Coord[0].x - size.cx - m_TextOffset, Coord[0].y - size.cy / 2, Str, -1);
		}
	}

	pRT->SelectObject(curPen);
	AfterPaint(pRT, painter);
}


CRect &SCoordAxis::GetDataDrawRect(void)
{
	return m_DataRect;
}


bool SCoordAxis::NumberToScaleStr(float value, int Digit, SStringT& str)
{
	SStringT ss;
	ss.Format(L"%%.%df", Digit);
	str.Format(ss, value);
	return true;
}

int SCoordAxis::MeasureText(IRenderTarget *pRT, SStringT Text, int TextHeight, SIZE *size)
{
	LOGFONT lf = { 0 };
	CAutoRefPtr<SOUI::IFont> Fonts;
	CAutoRefPtr<SOUI::IFont> oldFont;
	lf.lfHeight = TextHeight;
	GETRENDERFACTORY->CreateFont(&Fonts, lf);
	pRT->SelectObject(Fonts, (IRenderObj**)&oldFont);
	HRESULT hresult = pRT->MeasureText(Text, Text.GetLength(), size);
	pRT->SelectObject(oldFont);
	return hresult;
}


void SCoordAxis::SetCoord(COLORREF Color, int Width)
{
	m_CoordColor = Color;
	m_CoordWidth = Width;
}

void SCoordAxis::SetCoordScale(float xLen, float yLen, int Width, COLORREF Color)
{
	m_XScaleLength = xLen;
	m_YScaleLength = yLen;
	m_ScaleLineWidth = Width;
	m_ScaleLineColor = Color;
}

void SCoordAxis::SetAutoScope(float ReserveValue)
{
	m_IsAutoScope = true;
	m_ReserveValue = ReserveValue;
}

void SCoordAxis::SetLockScope(float LowerValue)
{
	m_LowerValue = LowerValue;
}

void SCoordAxis::SetDecimal(int Number)
{
	m_Decimal = Number;
}

void SCoordAxis::SetScaleNumber(int Number)
{
	m_ScaleNumber = Number;
}

void SCoordAxis::SetCoordMargin(RECT &rect)
{
	m_CoordMargin = rect;
}

void SCoordAxis::AddText(SStringT &Text)
{
	m_Text.Add(Text);
}
void SCoordAxis::ClearText(void)
{
	m_Text.RemoveAll();
}

void SCoordAxis::SetTextByIndex(int Index, SStringT &Text)
{
	if (Index>0 && Index<(int)m_Text.GetCount())
		m_Text[Index] = Text;
}

SStringT SCoordAxis::GetTextByIndex(int Index)
{
	if (Index > 0 && Index < (int)m_Text.GetCount())
		return m_Text[Index];
	SASSERT(FALSE);
	return L"";
}