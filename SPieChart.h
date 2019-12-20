/**
* Copyright (C) 2019-2050
* All rights reserved.
*
* @file       SPieChart.h
* @brief
* @version    v1.0
* @author     yiywain(逸远)
* @date       2019/12/19
* gitee  https://gitee.com/yiywain/soui-extend.git
* Describe    饼图
*/

#pragma once

#include <string>
#include <vector>
namespace SOUI
{
	class SPieChart;
	typedef struct _tagPie
	{
		friend SPieChart;
		float		Value;
		COLORREF	Color;
		SStringT StrTip;

		_tagPie(){
			Value = 0;
			Color = RGB(0, 0, 0);
			StrTip = L"无";
		}
		_tagPie(float Value, COLORREF color, const SStringT& strTip){
			this->Value = Value;
			this->Color = color;
			this->StrTip = strTip;
		}
	private:
		int id;
	}PIE;


	typedef std::vector<PIE>	PIES;

	//饼图控件 支持实心饼图 空心饼图 玫瑰图
	class SPieChart : public SWindow
	{
		SOUI_CLASS_NAME(SPieChart, L"PieChart")
	public:
		SPieChart(void);
		~SPieChart(void);

		void SetSeriesName(LPCTSTR Name);
		//添加数据
		int AddPie(PIE &TagPie);

		//清空所有数据
		void Clear();

		//修改饼图
		void SetPie(int id, PIE &TagPie);

		//移除其中一个扇形数据
		void RemoveSectorById(int Id);

		//通过id获取饼图数据
		PIE* GetSectorById(int Id);

		//设置饼图相对于控件的大小可以为百分比
		void SetPieSize(float Width);

		//扇形递减最小的半径与最大半径的比值  推荐0.5
		void SetSectorMin(float Decrease = 0.5);

		//设置空心圆大小
		void SetHollow(float Size, bool ShowHollowText = true);
		void UpdateWindow()
		{
			InvalidateRect(&GetClientRect());
		}
	protected:

		void OnPaint(IRenderTarget *pRT);
		void OnMouseMove(UINT nFlags, CPoint point);
		void OnMouseLeave();

	private:

	protected:
		SOUI_MSG_MAP_BEGIN()
			MSG_WM_PAINT_EX(OnPaint)
			MSG_WM_MOUSEMOVE(OnMouseMove)
			MSG_WM_MOUSELEAVE(OnMouseLeave)
			SOUI_MSG_MAP_END()

			SOUI_ATTRS_BEGIN()
			ATTR_FLOAT(L"PieSize", m_PieSize, TRUE)

			ATTR_BOOL(L"ShowTip", m_ShowTip, TRUE)
			ATTR_INT(L"TipLineLen", m_TipLineLen, TRUE)
			ATTR_FONT(L"TipFont", m_TipFont, TRUE)
			ATTR_COLOR(L"TipColor", m_TipColor, TRUE)

			ATTR_BOOL(L"CursorTip", m_CursorTip, TRUE)
			ATTR_COLOR(L"CursorTipColorBkgnd", m_CursorTipColorBkgnd, TRUE)
			ATTR_INT(L"CursorEnlargeRadius", m_CursorEnlargeRadius, TRUE)
			ATTR_FONT(L"CursorTipFont", m_CursorTipFont, TRUE)
			ATTR_COLOR(L"CursorTipTextColor", m_CursorTipTextColor, TRUE)

			ATTR_BOOL(L"Decimal", m_Decimal, TRUE)
			ATTR_STRINGT(L"SeriesName", m_SeriesName, TRUE)
			ATTR_FLOAT(L"Decrease", m_Decrease, TRUE)

			ATTR_FLOAT(L"HollowSize", m_HollowSize, TRUE)
			ATTR_BOOL(L"ShowHollowText", m_ShowHollowText, TRUE)
			ATTR_FONT(L"HollowFont", m_HollowTextFont, TRUE)
			ATTR_COLOR(L"HollowTextColor", m_HollowTextColor, TRUE)
			SOUI_ATTRS_END()
	protected:
		PIES		m_VecPies;

		float m_PieSize;//圆的大小 可以为百分比

		bool m_ShowTip; //是否显示提示文本
		int m_TipLineLen;//是提示折线长度
		SDpiAwareFont m_TipFont;//提示字体
		COLORREF m_TipColor;//提示线与字体颜色 (默认为饼图颜色)

		bool m_CursorTip;//是否响应鼠标
		COLORREF m_CursorTipColorBkgnd;//提示框背景颜色
		int m_CursorEnlargeRadius;//鼠标选中的放大半径
		SDpiAwareFont m_CursorTipFont;//鼠标提示字体
		COLORREF m_CursorTipTextColor;//鼠标提示字体颜色

		int m_Decimal;//数据显示的小数位数
		SStringT m_SeriesName;//系列名称

		//玫瑰图
		float m_Decrease;//百分比参数 最后一个扇与最大一个扇形比

		//空心饼图
		float m_HollowSize;//空心圆大小 绝对值和百分比
		bool m_ShowHollowText;//是否显示中空文字 m_HollowSize>0有效
		SDpiAwareFont m_HollowTextFont;//中心字体
		COLORREF m_HollowTextColor;//中心文字颜色 (默认为饼图颜色)
	private:
		bool m_CursorInPie;
		float m_Sum;
		CPoint m_CursorPoint;
	};
}