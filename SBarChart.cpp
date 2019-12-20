#include "stdafx.h"
#include "SBarChart.h"

SBarChart::SBarChart():
m_BarWidth(0.2),
m_BarColor(RGBA(255,0,0,255)),
m_BarInterval(0.4),
m_ShowValue(true),
m_Unit(_T("")),
m_BarTextColor(RGBA(0,0,0,0))
{
	m_PullOver = false;


}


SBarChart::~SBarChart()
{

}

void SBarChart::AddData(int id, float Value)
{	
	if (m_Data[id].GetCount() > 0)
		AddData(id, Value, m_Data[id][m_Data[id].GetCount() - 1].Color);
	else
		AddData(id, Value, m_BarColor);
}

void SBarChart::AddData(int id, float Value, COLORREF Color)
{
	BarDataType t;
	t.Value = Value;
	t.Color = Color;
	m_Data[id].Add(t);
}



void SBarChart::Clear(void)
{
	m_Data.RemoveAll();
}

bool SBarChart::Remove(int id)
{
	return m_Data.RemoveKey(id);
}

void SBarChart::OnPaint(IRenderTarget *pRT)
{
	__super::OnPaint(pRT);
	SPainter painter;
	BeforePaint(pRT, painter);

	CAutoRefPtr<IRenderObj> curBrush = pRT->GetCurrentObject(OT_BRUSH);
	CAutoRefPtr<IRenderObj> curPen = pRT->GetCurrentObject(OT_PEN);
	CAutoRefPtr<IRenderObj> curFont = pRT->GetCurrentObject(OT_FONT);
	COLORREF curTextColor=pRT->GetTextColor();

	SOUI::SPOSITION pos = m_Data.GetStartPosition();
	float BarMaxWidth;
	int BarTypeCount = m_Data.GetCount();
	int BarWidth = 0;
	int BarInterval = 0;
	int index=0;
	while (pos&&!m_PullOver)
	{
		CoordDataType::CPair *p = m_Data.GetNext(pos);
		for (size_t i = 0; i <p->m_value.GetCount(); i++)
		{
			CRect Bar;

			if (m_AxisType == AXIS_LENGTHWAYS)
			{
				BarInterval = m_BarInterval <= 1 ? m_XSignRatio / 2 * m_BarInterval : m_BarInterval;
				BarMaxWidth = m_XSignRatio-BarInterval;

				float SingleMaxWidth = (BarMaxWidth / (float)BarTypeCount);
				BarWidth = m_BarWidth <= 1 ? SingleMaxWidth*m_BarWidth : m_BarWidth;
				if (BarWidth >SingleMaxWidth)
					BarWidth = SingleMaxWidth;
				float SingleOffset = m_XSignRatio*(i)+m_XSignRatio / 2 + ((SingleMaxWidth*index) + SingleMaxWidth / 2) + BarInterval/2;//单个柱中心位置

				Bar.left = GetDataDrawRect().left + SingleOffset - BarWidth/2;
				Bar.right = Bar.left + BarWidth;
				Bar.top = GetDataDrawRect().bottom - (m_ValueRatio*(p->m_value[i].Value - m_MinValue));
				Bar.bottom = GetDataDrawRect().bottom;

				pRT->FillSolidRect(Bar, p->m_value[i].Color);

				if (m_ShowValue)
				{
					SStringT str;
					SIZE size;
					NumberToScaleStr(p->m_value[i].Value, m_Decimal,str);
					str += m_Unit;
					if (m_BarTextFont.GetFontPtr() != NULL)	pRT->SelectObject(m_BarTextFont.GetFontPtr());
					if (m_BarTextColor!=RGBA(0,0,0,0)) pRT->SetTextColor(m_BarTextColor);
					pRT->MeasureText(str, str.GetLength(), &size);
					pRT->TextOut(Bar.CenterPoint().x - size.cx/2,Bar.top - size.cy, str, -1);
				}
			}
			else if (m_AxisType == AXIS_CROSSWISE)
			{
				BarInterval = m_BarInterval <= 1 ? m_YSignRatio / 2 * m_BarInterval : m_BarInterval;
				BarMaxWidth = m_YSignRatio - BarInterval;

				float SingleMaxWidth = (BarMaxWidth / (float)BarTypeCount);
				BarWidth = m_BarWidth <= 1 ? SingleMaxWidth *m_BarWidth : m_BarWidth;
				if (BarWidth > SingleMaxWidth)
					BarWidth = SingleMaxWidth;
				float SingleOffset = m_YSignRatio*(i)+m_YSignRatio / 2 + ((SingleMaxWidth*index) + SingleMaxWidth / 2) + BarInterval / 2;//单个柱中心位置

				Bar.top = GetDataDrawRect().top + SingleOffset - BarWidth / 2;
				Bar.bottom = Bar.top + BarWidth;
				Bar.left = GetDataDrawRect().left;
				Bar.right = GetDataDrawRect().left + (m_ValueRatio*(p->m_value[i].Value - m_MinValue));

				pRT->FillSolidRect(Bar, p->m_value[0].Color);
				if (m_ShowValue)
				{
					SStringT str;
					SIZE size;
					NumberToScaleStr(p->m_value[i].Value, m_Decimal, str);
					str += m_Unit;
					if (m_BarTextFont.GetFontPtr() != NULL)	pRT->SelectObject(m_BarTextFont.GetFontPtr());
					if (m_BarTextColor != RGBA(0, 0, 0, 0)) pRT->SetTextColor(m_BarTextColor);
					pRT->MeasureText(str, str.GetLength(), &size);
					pRT->TextOut(Bar.right, Bar.top +(Bar.Height()/2-size.cy/2), str, -1);
				}
			}

		}
		index++;
	}
	pRT->SelectObject(curBrush);
	pRT->SelectObject(curPen);
	pRT->SelectObject(curFont);
	pRT->SetTextColor(curTextColor);

	AfterPaint(pRT, painter);
}



void SBarChart::GetMaxMin(float &Max, float &Min)
{
	SOUI::SPOSITION pos = m_Data.GetStartPosition();
	bool flag = false;
	while (pos)
	{
		CoordDataType::CPair *p = m_Data.GetNext(pos);
		for (size_t i = 0; i<p->m_value.GetCount(); i++)
		{
			float TempValue = p->m_value[i].Value;
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

void SBarChart::SetUnit(SStringT Unit)
{
	m_Unit = Unit;
}

void SBarChart::ShowValue(bool Show)
{
	m_ShowValue = Show;
}

void SBarChart::SetBarInterval(float Interval)
{
	m_BarInterval = Interval;
}

void SBarChart::SetBarWidth(float Width)
{
	m_BarWidth = Width;
}

void SBarChart::SetData(int id, SArray<BarDataType> &Data)
{
	m_Data[id] = Data;
}

SArray<SBarChart::BarDataType> &SBarChart::GetData(int id)
{
	return m_Data[id];
}