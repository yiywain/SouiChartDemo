#include "StdAfx.h"
#include <math.h>
#include "SPieChart.h"
#include <GdiPlus.h>
#pragma comment(lib,"gdiplus")

SPieChart::SPieChart(void):
m_Sum(0),
m_PieSize(0.8),
m_ShowTip(true),
m_TipLineLen(10),
m_TipColor(RGBA(0,0,0,0)),
m_CursorTip(true),
m_CursorTipColorBkgnd(RGBA(128,128,128,128)),
m_CursorEnlargeRadius(7),
m_CursorTipTextColor(RGBA(0, 0, 0, 255)),
m_Decimal(0),
m_SeriesName(L""),
m_Decrease(0),
m_HollowSize(0),
m_ShowHollowText(false),
m_HollowTextColor(RGBA(0,0,0,0)),
m_CursorInPie(false)
{


}

SPieChart::~SPieChart(void)
{
}

int SPieChart::AddPie(PIE &TagPie)
{
	int id;
	m_Sum += TagPie.Value;
	if (m_VecPies.size() > 0)
		id = m_VecPies.back().id + 1;
	else
		id = 0;
	TagPie.id = id;
	m_VecPies.push_back(TagPie);
	return id;
}

void SPieChart::Clear()
{
	m_VecPies.clear();
}

void SPieChart::SetPie(int id, PIE &TagPie)
{
	for (size_t i = 0; i < m_VecPies.size();i++)
	{
		if (m_VecPies[i].id == id)
		{
			m_VecPies[i] = TagPie;
			m_VecPies[i].id = id;
			break;
		}
	}
}

void SPieChart::RemoveSectorById(int Id)
{
	std::vector<PIE>::iterator it = m_VecPies.begin();
	while (it != m_VecPies.end())
	{
		if (it->id == Id)
		{
			it=m_VecPies.erase(it);
			break;
		}
		else
			it++;
	}
}

PIE* SPieChart::GetSectorById(int Id)
{

	for (size_t i=0;i<m_VecPies.size();i++)
	{
		if (m_VecPies[i].id == Id)
			return &m_VecPies[i];
	}
	return NULL;
}

void SPieChart::SetPieSize(float Width)
{
	m_PieSize = Width;
}

void SPieChart::SetSectorMin(float Decrease)
{
	m_Decrease = Decrease;
}

void SPieChart::SetHollow(float Size, bool ShowHollowText)
{
	m_HollowSize = Size;
	m_ShowHollowText = ShowHollowText;
}

//文本的位置
struct TipSite{
	Gdiplus::PointF Pos[3];
	Gdiplus::RectF Rect;
	TipSite(Gdiplus::PointF *Pos, Gdiplus::RectF &Rect)
	{
		this->Pos[0] = Pos[0];
		this->Pos[1] = Pos[1];
		this->Pos[2] = Pos[2];
		this->Rect = Rect;
	}
};
struct _Arg
{
	float BeginArg;
	float EndArg;
	int x;//圆的中心x
	int y;//圆的中心y
	int MinR;//最小半径
	int MaxR;//最大半径
	float Wdith;//圆矩形宽度
	_Arg(float BeginArg, float EndArg, int x, int y, int MinR, int MaxR, float Wdith)
	{
		this->BeginArg = BeginArg;
		this->EndArg = EndArg;
		this->x = x;
		this->y = y;
		this->MinR = MinR;
		this->MaxR = MaxR;
		this->Wdith = Wdith;
	}
};

//简单排版位置
void AdjustTextSite(int CenterX,int CenterY, std::vector<TipSite> &TipSites)
{
	//矩形相交处理
	int count = TipSites.size();
	for (int i = 0; i<count; i++)
	{
		int last =(i-1)<0?count-1:i-1;
		//右下角
		if (TipSites[i].Pos[1].X>CenterX&&TipSites[i].Pos[1].Y>CenterY)
		{
			if (TipSites[i].Rect.GetTop()<TipSites[last].Rect.GetBottom())
			{
				int k = TipSites[last].Rect.GetBottom() - TipSites[i].Rect.Y;
				TipSites[i].Rect.Y += abs(TipSites[last].Rect.GetBottom()-TipSites[i].Rect.Y );
				TipSites[i].Pos[2].Y = TipSites[i].Rect.Y +  TipSites[i].Rect.Height/2;
			}
		}//左上角
		else if (TipSites[i].Pos[1].X<CenterX&&TipSites[i].Pos[1].Y<CenterY)
		{
			if (TipSites[i].Rect.GetBottom()>TipSites[last].Rect.GetTop())
			{		
				TipSites[i].Rect.Y -= abs(TipSites[i].Rect.GetBottom() - TipSites[last].Rect.Y);
				TipSites[i].Pos[2].Y = TipSites[i].Rect.Y + TipSites[i].Rect.Height / 2;
			}
		}
	}
	
	for (int i = count-1; i >-1; i--)
	{
		int last = (i + 1) < count ? i + 1 : count-i;
		//右上角
		if (TipSites[i].Pos[1].X>CenterX&&TipSites[i].Pos[1].Y<CenterY)
		{
			if (TipSites[i].Rect.GetBottom()>TipSites[last].Rect.GetTop())
			{
				TipSites[i].Rect.Y -= abs(TipSites[i].Rect.GetBottom() - TipSites[last].Rect.Y);
				TipSites[i].Pos[2].Y = TipSites[i].Rect.Y + TipSites[i].Rect.Height / 2;
			}
		}else//左下角
		if (TipSites[i].Pos[1].X<CenterX&&TipSites[i].Pos[1].Y>CenterY)
		{
			if (TipSites[i].Rect.GetTop()<TipSites[last].Rect.GetBottom())
			{
				TipSites[i].Rect.Y += abs(TipSites[last].Rect.GetBottom()-TipSites[i].Rect.GetTop());
				TipSites[i].Pos[2].Y = TipSites[i].Rect.Y + TipSites[i].Rect.Height / 2;
			}
		}

	}
	
}


void SPieChart::OnPaint(IRenderTarget *pRT)
{
	SPainter painter;
	BeforePaint(pRT, painter);

	CAutoRefPtr<IRenderObj> curBrush = pRT->GetCurrentObject(OT_BRUSH);
	CAutoRefPtr<IRenderObj> curPen = pRT->GetCurrentObject(OT_PEN);
	CAutoRefPtr<IRenderObj> curFont = pRT->GetCurrentObject(OT_FONT);
	COLORREF curTextColor = pRT->GetTextColor();


	CRect rcClient = GetClientRect();

	Gdiplus::RectF rectDraw;
	rectDraw.X = rcClient.left;
	rectDraw.Y = rcClient.top;
	rectDraw.Width = rcClient.Width();
	rectDraw.Height = rcClient.Height();

	float f=rectDraw.Height = rectDraw.Width = rectDraw.Width > rectDraw.Height ? rectDraw.Height: rectDraw.Width;

	rectDraw.Height = rectDraw.Width = f*(m_PieSize<1) ? m_PieSize*f : m_PieSize;
	
	rectDraw.X += (f - rectDraw.Width) / 2.f;
	rectDraw.Y += (f - rectDraw.Width) / 2.f;


	rectDraw.Offset(Gdiplus::PointF(rcClient.CenterPoint().x - (rectDraw.X + rectDraw.Width/2), rcClient.CenterPoint().y - (rectDraw.Y + rectDraw.Height / 2)));

	float nSweep = 0;
	float r = rectDraw.Height / 2.f;
	float CenterX = rectDraw.X + rectDraw.Width / 2.f;
	float CenterY = rectDraw.Y + rectDraw.Width / 2.f;
	const double pi = acos(-1.0);


	HDC hDC = pRT->GetDC();
	Gdiplus::Graphics graph(hDC);
	graph.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	//使用SOUI的字体
	CAutoRefPtr<SOUI::IFont> pIFont;
	pIFont = (SOUI::IFont*)pRT->GetCurrentObject(OT_FONT);
	Gdiplus::FontFamily fontfamily(pIFont->FamilyName());

	Gdiplus::Font font(&fontfamily, abs(pIFont->TextSize()), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
	Gdiplus::PointF po;
	Gdiplus::RectF TxtRect;

	std::vector<_Arg> _Args;//角度
	std::vector<TipSite> TextRects;//存储了文字绘制的位置
	float  Decrease = r*m_Decrease/(float)m_VecPies.size();
	for (size_t i = 0; i < m_VecPies.size(); i++)
	{
		Gdiplus::Color color(GetAValue(m_VecPies[i].Color), GetRValue(m_VecPies[i].Color), GetGValue(m_VecPies[i].Color), GetBValue(m_VecPies[i].Color));
		Gdiplus::SolidBrush hBrush(color);
		hBrush.SetColor(color);
		float nTemp = m_VecPies[i].Value / m_Sum*360.f;
		float PieWidth = rectDraw.Width - Decrease*i * 2;
		graph.FillPie(&hBrush, rectDraw.X + Decrease*i, rectDraw.Y + Decrease*i, PieWidth, PieWidth, nSweep, nTemp);

		if (m_ShowTip)
		{
			Gdiplus::Pen curPen(color, 2);
			Gdiplus::PointF Pof[3];

			Pof[0].X = CenterX + PieWidth/2*cos(-(nSweep + abs(nTemp) / 2.f)* pi / 180.f);
			Pof[0].Y = CenterY - PieWidth/2*sin(-(nSweep + abs(nTemp) / 2.f)* pi / 180.f);

			Pof[1].X = CenterX + (r + m_TipLineLen)*cos(-(nSweep + abs(nTemp) / 2.f)* pi / 180.f);
			Pof[1].Y = CenterY - (r + m_TipLineLen)*sin(-(nSweep + abs(nTemp) / 2.f)* pi / 180.f);
			Pof[2].X = Pof[1].X>CenterX ? Pof[1].X + m_TipLineLen : Pof[1].X - m_TipLineLen;
			Pof[2].Y = Pof[1].Y;

			po = Pof[2];
			graph.MeasureString(m_VecPies[i].StrTip, m_VecPies[i].StrTip.GetLength(), &font, po, &TxtRect);
			if (po.X < CenterX)
				po.X -= TxtRect.Width;
			po.Y -= TxtRect.Height / 2;
			TxtRect.X = po.X;
			TxtRect.Y = po.Y;

			TextRects.push_back(TipSite(Pof, TxtRect));
		}
		if (m_CursorTip)
		{
			_Args.push_back(_Arg(nSweep, nSweep + nTemp, rectDraw.X + Decrease*i, rectDraw.Y + Decrease*i, 0, (rectDraw.Width - Decrease*i * 2)/2, rectDraw.Width - Decrease*i * 2));
		}
		nSweep += nTemp;
	}

	//调整布局
	AdjustTextSite(CenterX,CenterY, TextRects);
	//绘制文字与线
	for (size_t i=0;i<TextRects.size();i++)
	{
		TipSite &t=TextRects[i];
		COLORREF LineTextColor = m_TipColor != RGBA(0, 0, 0, 0) ? m_TipColor : m_VecPies[i].Color;

		Gdiplus::Color color(GetAValue(LineTextColor), GetRValue(LineTextColor), GetGValue(LineTextColor), GetBValue(LineTextColor));
		Gdiplus::SolidBrush hBrush(color);

		graph.DrawLines(&Gdiplus::Pen(color, 2), t.Pos, 3);

		CRect rect;
		rect.top = t.Rect.GetTop();
		rect.left = t.Rect.GetLeft();
		rect.bottom = t.Rect.GetBottom();
		rect.right = t.Rect.GetRight();

		pRT->SetTextColor(LineTextColor);
		if (m_TipFont.GetFontPtr()!=NULL) pRT->SelectObject(m_TipFont.GetFontPtr());
		pRT->DrawText(m_VecPies[i].StrTip, m_VecPies[i].StrTip.GetLength(), rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}

	float HollowWidth = m_HollowSize<1 ? r*m_HollowSize * 2 : m_HollowSize;
	if (m_HollowSize>0)
	{
		Gdiplus::SolidBrush hBrush(Gdiplus::Color(GetAValue(GetStyle().m_crBg), GetRValue(GetStyle().m_crBg), GetGValue(GetStyle().m_crBg), GetBValue(GetStyle().m_crBg)));
		graph.FillEllipse(&hBrush, CenterX - HollowWidth / 2, CenterY - HollowWidth / 2, HollowWidth, HollowWidth);
	}

	//响应鼠标显示提示框
	if (m_CursorTip&&m_CursorPoint.x>0 && m_CursorPoint.y>0)
	{
		m_CursorInPie = false;
		float angle = atan((m_CursorPoint.y-CenterY)/(m_CursorPoint.x - CenterX));//弧度
		float theta = angle*(180 / pi);//角度

		if(m_CursorPoint.y > CenterY&&m_CursorPoint.x<CenterX)
			theta = 90+(90+theta);
		if (m_CursorPoint.y < CenterY&&m_CursorPoint.x<CenterX)
			theta += 180;
		if (m_CursorPoint.y < CenterY&&m_CursorPoint.x>CenterX)
			theta = 270 + (90 + theta);


		//SStringW str;
		//str.Format(L"     弧度=%.2f角度%.2f",angle, theta);
		//pRT->SetTextColor(RGBA(255,0,0,255));
		//pRT->TextOut(m_CursorPoint.x, m_CursorPoint.y, str, -1);

		float rr = sqrt(pow(CenterX - m_CursorPoint.x, 2) + pow(CenterY - m_CursorPoint.y,2));

		for (size_t n = 0; n<_Args.size(); n++) 
		{
			_Arg &a=_Args[n];
			if (rr<a.MaxR&&rr>(HollowWidth/2)&&a.BeginArg<theta&&a.EndArg>theta)
				{


					//扇形放大
					Gdiplus::Color color(GetAValue(m_VecPies[n].Color), GetRValue(m_VecPies[n].Color), GetGValue(m_VecPies[n].Color), GetBValue(m_VecPies[n].Color));
					Gdiplus::SolidBrush hBrush(color);
					hBrush.SetColor(color);
					graph.FillPie(&hBrush, a.x - m_CursorEnlargeRadius, a.y - m_CursorEnlargeRadius, (int)a.Wdith + m_CursorEnlargeRadius * 2, (int)a.Wdith + m_CursorEnlargeRadius * 2, a.BeginArg, a.EndArg - a.BeginArg);

					//////
					if (m_HollowSize>0)//绘制一个颜色为背景色的圆覆盖在饼图上
					{
						Gdiplus::SolidBrush hBrush(Gdiplus::Color(GetAValue(GetStyle().m_crBg), GetRValue(GetStyle().m_crBg), GetGValue(GetStyle().m_crBg), GetBValue(GetStyle().m_crBg)));
						graph.FillEllipse(&hBrush, CenterX - HollowWidth / 2, CenterY - HollowWidth / 2, HollowWidth, HollowWidth);
						if (m_ShowHollowText)//绘制中间文字
						{
							CRect TextRect;
							TextRect.top =  (CenterY - HollowWidth / 2) ;
							TextRect.bottom = TextRect.top + HollowWidth;
							TextRect.left = (CenterX - HollowWidth / 2) ;
							TextRect.right = TextRect.left + HollowWidth;
							pRT->SetTextColor(m_HollowTextColor != RGBA(0, 0, 0, 0) ? m_HollowTextColor : m_VecPies[n].Color);
							SOUI::IFontPtr poldFont=NULL;
							if (m_HollowTextFont.GetFontPtr() != NULL)	pRT->SelectObject(m_HollowTextFont.GetFontPtr(), (IRenderObj**)&poldFont);
							pRT->DrawText(m_VecPies[n].StrTip, m_VecPies[n].StrTip.GetLength(), TextRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							if (poldFont != NULL) pRT->SelectObject(poldFont);
						}
					}

					//准备绘制提示框
					CRect rect;
					rect.top = m_CursorPoint.y + 15;
					rect.left = m_CursorPoint.x + 15;
					SStringT str;
					str.Format(SStringT().Format(L"%%s\n%%s:%%.%df(%%.2f%%%%)", m_Decimal), m_SeriesName, m_VecPies[n].StrTip, m_VecPies[n].Value, (a.EndArg - a.BeginArg) / 360 * 100);
					SIZE size[2];
					pRT->MeasureText(str.Left(str.Find(L"\n")), -1, &size[0]);
					pRT->MeasureText(str.Right(str.GetLength() - str.Find(L"\n") - 1), -1, &size[1]);
					rect.bottom = rect.top + size[0].cy + size[1].cy + 2;
					rect.right = rect.left + (size[0].cx>size[1].cx ? size[0].cx : size[1].cx) + 2;

					//调整提示框位置
					if (rect.bottom > rcClient.bottom) rect.OffsetRect(CPoint(0, -(rect.bottom - rcClient.bottom)));
					if (rect.right > rcClient.right) rect.OffsetRect(CPoint(-(rect.right - rcClient.right), 0));
					//绘制提示框
					pRT->FillSolidRoundRect(rect, CPoint(2, 2), m_CursorTipColorBkgnd);
					pRT->SetTextColor(m_CursorTipTextColor);
					SOUI::IFontPtr poldFont = NULL;
					if (m_CursorTipFont.GetFontPtr() != NULL)	pRT->SelectObject(m_CursorTipFont.GetFontPtr(), (IRenderObj**)&poldFont);
					pRT->DrawText(str, str.GetLength(), rect, DT_LEFT | DT_VCENTER /*| DT_SINGLELINE*/);
					if (poldFont != NULL) pRT->SelectObject(poldFont);
					m_CursorInPie = true;
					break;
				}
			}
		}

	pRT->ReleaseDC(hDC);

	pRT->SelectObject(curBrush);
	pRT->SelectObject(curPen);
	pRT->SelectObject(curFont);
	pRT->SetTextColor(curTextColor);

	AfterPaint(pRT, painter);
}

void SPieChart::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_CursorTip)
	{
		m_CursorPoint = point;
		InvalidateRect(&GetClientRect(),false);
		if (m_CursorInPie)
			SetCursor(LoadCursor(NULL, IDC_HAND));
		else
			SetCursor(LoadCursor(NULL, IDC_ARROW));
	}
	return ;

}

void SPieChart::OnMouseLeave()
{
	if (m_CursorTip)
	{
		m_CursorPoint.x = m_CursorPoint.y = 0;
		InvalidateRect(&GetClientRect());
	}
}

void SPieChart::SetSeriesName(LPCTSTR Name)
{
	m_SeriesName = Name;
}