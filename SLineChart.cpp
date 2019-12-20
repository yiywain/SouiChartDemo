#include "stdafx.h"
#include "SLineChart.h"
#include <GdiPlus.h>
#pragma comment(lib,"gdiplus")


SLineChart::SLineChart():
m_LineColor(RGBA(255,255, 255, 255)),
m_LineWidth(1),
m_FillRect(0),
m_DataEnd(0),
m_IsCurve(false),
m_DataPointSize(0),
m_DataPointCentreColor(RGBA(0, 0, 0, 0)),
m_CursorTip(true),
m_CursorInCoord(false),
m_CursorTipTextColor(RGBA(0,0,0,255)),
m_CursorTipColorBkgnd(RGBA(128,128,128,128)),
m_CursorLineColor(RGBA(128, 128, 128, 128)),
m_CursorLineWidth(1)
{

}


SLineChart::~SLineChart()
{

}

void SLineChart::CreateLine(int id, SStringT Name, COLORREF Color)
{
	LineTag t;
	t.Color = Color;
	t.Name = Name;
	m_Data[id]=t;
}

void SLineChart::AddData(int id, float Value)
{
	m_Data[id].Values.Add(Value);
}

void  SLineChart::Clear()
{
	m_Data.RemoveAll();
}



void SLineChart::SetLineData(int id, LineTag &LineData)
{
	m_Data[id] = LineData;
}


void SLineChart::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_CursorTip)
	{
		m_CursorPoint = point;
		InvalidateRect(&GetClientRect(), false);
	}
	return;
}

void SLineChart::OnMouseLeave()
{
	if (m_CursorTip)
	{
		m_CursorPoint.x = m_CursorPoint.y = 0;
		InvalidateRect(&GetClientRect());
	}
}



void SLineChart::OnPaint(IRenderTarget *pRT)
{
	__super::OnPaint(pRT);
	SPainter painter;
	BeforePaint(pRT, painter);
	CRect rcClient = GetClientRect();
	CRect CurrentDrawRect;
	CurrentDrawRect = m_DataRect;
	
	if (!m_PullOver&&m_AxisType == AXIS_LENGTHWAYS)
	{
		CurrentDrawRect.left += (LONG)m_XSignRatio;
		CurrentDrawRect.right -= (LONG)m_XSignRatio;
	}
	else if (!m_PullOver&&m_AxisType == AXIS_CROSSWISE)
	{
		CurrentDrawRect.top += (LONG)m_YSignRatio;
		CurrentDrawRect.bottom -= (LONG)m_YSignRatio;
	}
	
	//对于m_DataEnd的支持	
	float DataWidth;
	if (m_DataEnd&&m_AxisType == AXIS_LENGTHWAYS)
	{
		DataWidth = m_DataEnd>1 ? m_XSignRatio*m_DataEnd:CurrentDrawRect.Width()*(float)m_DataEnd;
		CurrentDrawRect.right = CurrentDrawRect.left + (LONG)DataWidth;
	}
	else if (m_DataEnd&&m_AxisType == AXIS_CROSSWISE)
	{
		DataWidth = m_DataEnd>1 ? m_YSignRatio*m_DataEnd : CurrentDrawRect.Height()*(float)m_DataEnd;
		CurrentDrawRect.top = CurrentDrawRect.bottom - (LONG)DataWidth;
	}

	SOUI::SPOSITION pos = m_Data.GetStartPosition();
	bool CountAligned=true;//数据是否是对齐的
	size_t AlignedCount = 0;//CountAligned为真 有效
	int m = 0;

	SArray<SArray<POINT>> Pointsx;//线上的点坐标

	//每条线的绘制
	while (pos)
	{
		SArray<POINT> Points;
		CoordLineData::CPair *p = m_Data.GetNext(pos);

		SArray<Gdiplus::PointF> pos;

		if (m == 0)
			m = p->m_value.Values.GetCount();
		else if (m != p->m_value.Values.GetCount())
			CountAligned = false;

		AlignedCount = p->m_value.Values.GetCount() > AlignedCount ? p->m_value.Values.GetCount() : AlignedCount;

		size_t Count = p->m_value.Values.GetCount() - 1;
		Count = Count < m_Text.GetCount() ? m_Text.GetCount() - 1 : Count;
		for (size_t i = 0; i < p->m_value.Values.GetCount(); i++)
		{
			POINT po;
			if (m_AxisType == AXIS_LENGTHWAYS)
			{
				po.x = GetDataDrawRect().left + (m_PullOver ? 0 : m_XSignRatio) + CurrentDrawRect.Width() / (float)(Count)*i;
				po.y = GetDataDrawRect().bottom - (m_ValueRatio*(p->m_value.Values[i] - m_MinValue));
			}
			else if (m_AxisType == AXIS_CROSSWISE)
			{
				po.x = GetDataDrawRect().left + (m_ValueRatio*(p->m_value.Values[i] - m_MinValue));
				po.y = GetDataDrawRect().bottom - (m_PullOver ? 0 : m_YSignRatio) - CurrentDrawRect.Height() / (float)Count*i;
			}

			Points.Add(po);
			
			if (m_FillRect||m_IsCurve)
			{
				Gdiplus::PointF Pof;
				Pof.X = (float)po.x;
				Pof.Y = (float)po.y;
				pos.Add(Pof);
			}
		}

		if (!m_IsCurve)
		{
			CAutoRefPtr<SOUI::IPen> Pen;
			pRT->CreatePen(PS_SOLID, p->m_value.Color, m_LineWidth, &Pen);
			pRT->SelectObject(Pen);
			pRT->DrawLines(&Points.GetAt(0), Points.GetCount());
		}
		else
		{

			////曲线的绘制
			HDC hdc = pRT->GetDC();
			Gdiplus::Graphics gps(hdc);
			gps.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
			Gdiplus::Pen curPen(Gdiplus::Color(GetAValue(p->m_value.Color), GetRValue(p->m_value.Color), GetGValue(p->m_value.Color), GetBValue(p->m_value.Color)), m_LineWidth);
			gps.DrawCurve(&curPen, &pos[0], pos.GetCount());
			pRT->ReleaseDC(hdc);
		}
		//数据点的绘制
		if (m_DataPointSize > 0)
		{
			CAutoRefPtr<SOUI::IPen> pPen;
			pRT->CreatePen(PS_SOLID, p->m_value.Color, m_LineWidth, &pPen);
			pRT->SelectObject(pPen);

			CAutoRefPtr<SOUI::IBrush> pBrush;
			CAutoRefPtr<SOUI::IBrush> poldBrush;
			poldBrush = (IBrush*)pRT->GetCurrentObject(OT_BRUSH);
			pRT->CreateSolidColorBrush(m_DataPointCentreColor == RGBA(0, 0, 0, 0) ? p->m_value.Color : m_DataPointCentreColor, &pBrush);
			pRT->SelectObject(pBrush);
			for (size_t i = 0; i < Points.GetCount(); i++)
			{
				CRect Rect;
				Rect.left = Points[i].x - m_DataPointSize / 2;
				Rect.top = Points[i].y - m_DataPointSize / 2;
				Rect.bottom = Rect.top + m_DataPointSize;
				Rect.right = Rect.left + m_DataPointSize;
				pRT->FillSolidEllipse(Rect, p->m_value.Color);
			}
			pRT->SelectObject(poldBrush);
		}
		//绘制填充
		if (m_FillRect&&(!m_IsCurve))
		{
			//闭合线段
			if (m_AxisType == AXIS_LENGTHWAYS)
			{
				Gdiplus::PointF Pof;
				Pof.X = pos[pos.GetCount() - 1].X;
				Pof.Y = m_DataRect.bottom;
				pos.Add(Pof);
				Pof.X = pos[0].X;
				Pof.Y = m_DataRect.bottom;
				pos.Add(Pof);
				Pof.X = pos[0].X;
				Pof.Y = pos[0].Y;
				pos.Add(Pof);
			}
			else if (m_AxisType == AXIS_CROSSWISE)
			{
				Gdiplus::PointF Pof;
				Pof.X = m_DataRect.left;
				Pof.Y = pos[pos.GetCount() - 1].Y;
				pos.Add(Pof);
				Pof.X = m_DataRect.left;
				Pof.Y = pos[0].Y;
				pos.Add(Pof);
				Pof.X = pos[0].X;
				Pof.Y = pos[0].Y;
				pos.Add(Pof);
			}

			//绘制闭合区域
			HDC hdc=pRT->GetDC();
			Gdiplus::Graphics gps(hdc);
			gps.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
			Gdiplus::SolidBrush blackBrush(Gdiplus::Color(m_FillRect, GetRValue(p->m_value.Color), GetGValue(p->m_value.Color), GetBValue(p->m_value.Color)));
			gps.FillPolygon(&blackBrush, &pos[0], pos.GetCount());
			pRT->ReleaseDC(hdc);
		}
		
		Pointsx.Add(Points);
	}

	//对于鼠标的响应 //线上的点需要对齐才能生效
	if (CountAligned&&m_CursorTip&&m_CursorPoint.x != 0 && m_CursorPoint.y != 0)
	{
		m_CursorInCoord = false;
		if (CurrentDrawRect.PtInRect(m_CursorPoint))
		{
			int index=0;
			CPoint CursorLinePos[2];
			//绘制一条跟随光标的线
			if(m_AxisType == AXIS_LENGTHWAYS)
			{

				AlignedCount--;
				float space = (CurrentDrawRect.Width() / (float)AlignedCount);
				index = ((m_CursorPoint.x + (space / 2) - CurrentDrawRect.left) / (space));

				CursorLinePos[1].x = CursorLinePos[0].x = CurrentDrawRect.left + index* space;
				CursorLinePos[0].y = CurrentDrawRect.top;
				CursorLinePos[1].y = CurrentDrawRect.bottom;
			}
			else if (m_AxisType == AXIS_CROSSWISE)
			{
				AlignedCount--;
				float space = (CurrentDrawRect.Height() / (float)AlignedCount);
				index = ((CurrentDrawRect.bottom-(m_CursorPoint.y - space/2)) / space);

				CursorLinePos[1].y = CursorLinePos[0].y = CurrentDrawRect.bottom - index* space;
				CursorLinePos[0].x = CurrentDrawRect.left;
				CursorLinePos[1].x = CurrentDrawRect.right;
			}

			CAutoRefPtr<SOUI::IPen> pPen;
			pRT->CreatePen(PS_SOLID, m_CursorLineColor, m_CursorLineWidth, &pPen);
			pRT->SelectObject(pPen);
			pRT->DrawLines(CursorLinePos, 2);

			//准备绘制提示框
			CRect rect;
			rect.top = m_CursorPoint.y + 15;
			rect.left = m_CursorPoint.x + 15;

			SOUI::SPOSITION pos = m_Data.GetStartPosition();
			int MaxTextWidth=0;
			int MaxTextHeight = 0;
			SArray<SStringT> TipTexts;
			int n = 0;
			SIZE size;
			while (pos)
			{
				CoordLineData::CPair *p = m_Data.GetNext(pos);
				SStringT str;
				SStringT strValue;

				NumberToScaleStr(p->m_value.Values[index], m_Decimal,strValue);
				str.Format(L"%s:%s", p->m_value.Name, strValue);
				TipTexts.Add(str);

				pRT->MeasureText(str, str.GetLength(), &size);
				if (MaxTextWidth < size.cx) MaxTextWidth = size.cx;
				MaxTextHeight += size.cy;

				//数据点放大显示
				CAutoRefPtr<SOUI::IBrush> pBrush;
				CAutoRefPtr<SOUI::IPen> pPen;
				pRT->CreateSolidColorBrush(m_DataPointCentreColor == RGBA(0, 0, 0, 0) ? p->m_value.Color : m_DataPointCentreColor, &pBrush);
				pRT->CreatePen(PS_SOLID, p->m_value.Color, 1, &pPen);
				pRT->SelectObject(pPen);
				pRT->SelectObject(pBrush);
				CRect Rect;
				Rect.left = Pointsx[n][index].x - (m_DataPointSize*1.8f) / 2.f;
				Rect.top = Pointsx[n][index].y - (m_DataPointSize*1.8f) / 2.f;
				Rect.bottom = Rect.top + m_DataPointSize*1.8f;
				Rect.right = Rect.left + m_DataPointSize*1.8f;
				pRT->FillSolidEllipse(Rect, p->m_value.Color);
				n++;
			}

			rect.bottom = rect.top + MaxTextHeight;
			rect.right = rect.left + MaxTextWidth;

			//调整提示框位置
			if (rect.bottom > rcClient.bottom) rect.OffsetRect(CPoint(0, -(rect.bottom - rcClient.bottom)));
			if (rect.right > rcClient.right) rect.OffsetRect(CPoint(-(rect.right - rcClient.right), 0));

			//绘制提示框
			pRT->FillSolidRoundRect(rect, CPoint(2, 2), m_CursorTipColorBkgnd);
			if (m_CursorTipFont.GetFontPtr() != NULL)	pRT->SelectObject(m_CursorTipFont.GetFontPtr());
			pRT->SetTextColor(m_CursorTipTextColor);
			
			//绘制提示框中的文字
			for (size_t i = 0; i < TipTexts.GetCount();i++)
			{
				pRT->DrawText(TipTexts[i], TipTexts[i].GetLength(),rect, DT_LEFT | DT_VCENTER /*| DT_SINGLELINE*/);
				rect.OffsetRect(0, size.cy);
			}

			m_CursorInCoord = true;
		}
	}

	AfterPaint(pRT, painter);
}

void SLineChart::GetMaxMin(float &Max, float &Min)
{
	SOUI::SPOSITION pos = m_Data.GetStartPosition();
	bool flag = false;
	while (pos)
	{
		CoordLineData::CPair *p = m_Data.GetNext(pos);
		for (size_t i = 0; i<p->m_value.Values.GetCount(); i++)
		{
			float TempValue = p->m_value.Values[i];
			if (i == 0 && !flag)
			{
				Max = Min = TempValue;
				flag = true;
			}

			if (Min > TempValue)
				Min = TempValue;
			if (Max < TempValue)
				Max = TempValue;
		}
	}
}


void SLineChart::SetLineWidth(int Width)
{
	m_LineWidth = Width;
}

void SLineChart::SetFillRect(int Alpha)
{
	m_FillRect = Alpha;
}

void SLineChart::SetDataEnd(float End)
{
	m_DataEnd = End;
}

void SLineChart::SetCurveChart(bool Curve)
{
	m_IsCurve = Curve;
}

void SLineChart::SetLineValue(int id, int index, float Value)
{
	m_Data[id].Values[index] = Value;
}

SLineChart::LineTag &SLineChart::GetLineData(int id)
{
	return m_Data[id];
}

void SLineChart::PopTopPushBack(int id,float Value)
{
	for (size_t i = 0; i < m_Data[id].Values.GetCount(); i++)
	{
		if (i > 0)
		{ 
			m_Data[id].Values[i - 1] = m_Data[id].Values[i];
		}
	}
	m_Data[id].Values[m_Data[id].Values.GetCount() - 1] = Value;
}