/**
* Copyright (C) 2019-2050
* All rights reserved.
*
* @file       SLineChart.h
* @brief
* @version    v1.0
* @author     yiywain(逸远)
* @date       2019/12/19
* gitee  https://gitee.com/yiywain/soui-extend.git
* Describe    折线图曲线图
*/

#pragma once
#include "SCoordAxis.h"

namespace SOUI
{

	class SLineChart :
		public SCoordAxis
	{
		SOUI_CLASS_NAME(SChart_bar, L"LineChart")
	public:
		struct LineTag
		{
			SStringT Name;
			SArray<float> Values;
			COLORREF Color;
			LineTag()
			{
				Name = L"";
				Color = RGBA(0, 0, 0, 255);
			}
			LineTag(SStringT Name, SArray<float> &Values, COLORREF Color)
			{
				this->Name = Name;
				this->Values = Values;
				this->Color = Color;
			}
		};

		SLineChart();
		~SLineChart();

		//创建一条线
		void CreateLine(int id, SStringT Name, COLORREF Color);

		//向线添加数据
		void AddData(int id, float Value);

		//清空所有数据
		void Clear();

		//修改线的数据
		void SetLineData(int id, LineTag &LineData);

		//获取线的数据
		LineTag &GetLineData(int id);

		void SetLineValue(int id, int index, float Value);

		//设置线宽
		void SetLineWidth(int Width);

		//设置填充透明通道 仅折线图有效
		void SetFillRect(int Alpha);

		//设置数据到哪里结束 支持百分比
		void SetDataEnd(float End);

		//设置是否为曲线图
		void SetCurveChart(bool Curve);

		void PopTopPushBack(int id, float Value);

		void UpdateWindow()
		{
			InvalidateRect(&GetClientRect());
		}
	protected:
		virtual void GetMaxMin(float &Max, float &Min);

		void OnPaint(IRenderTarget *pRT);
		void OnMouseMove(UINT nFlags, CPoint point);
		void OnMouseLeave();
	protected:
		SOUI_MSG_MAP_BEGIN()
			MSG_WM_PAINT_EX(OnPaint)
			MSG_WM_MOUSEMOVE(OnMouseMove)
			MSG_WM_MOUSELEAVE(OnMouseLeave)
			SOUI_MSG_MAP_END()

			SOUI_ATTRS_BEGIN()
			ATTR_COLOR(L"LineColor", m_LineColor, TRUE)
			ATTR_INT(L"LineWidth", m_LineWidth, TRUE)
			ATTR_INT(L"FillRect", m_FillRect, TRUE)
			ATTR_FLOAT(L"DataEnd", m_DataEnd, TRUE)
			ATTR_BOOL(L"IsCurve", m_IsCurve, TRUE)
			ATTR_INT(L"DataPointSize", m_DataPointSize, TRUE)
			ATTR_COLOR(L"DataPointCentreColor", m_DataPointCentreColor, TRUE)

			ATTR_BOOL(L"CursorTip", m_CursorTip, TRUE)
			ATTR_FONT(L"CursorTipFont", m_CursorTipFont, TRUE)
			ATTR_COLOR(L"CursorTipTextColor", m_CursorTipTextColor, TRUE)
			ATTR_COLOR(L"CursorTipColorBkgnd", m_CursorTipColorBkgnd, TRUE)

			ATTR_COLOR(L"CursorLineColor", m_CursorLineColor, TRUE)
			ATTR_INT(L"CursorLineWidth", m_CursorLineWidth, TRUE)
			SOUI_ATTRS_END()
	protected:
		COLORREF m_LineColor;
		int m_LineWidth;
		int m_FillRect;//填充区域透明通道 0为不填充0-255
		float m_DataEnd;//数据结束位置 0为不设置 支持百分比
		bool m_IsCurve;//是否是曲线 曲线暂不支持区域填充

		INT m_DataPointSize;//数据点大小
		COLORREF m_DataPointCentreColor;//数据点颜色

		BOOL m_CursorTip; //是否响应鼠标
		SDpiAwareFont m_CursorTipFont;//鼠标提示字体
		COLORREF m_CursorTipTextColor;//鼠标提示字体颜色
		COLORREF m_CursorTipColorBkgnd;
		COLORREF m_CursorLineColor;
		INT m_CursorLineWidth;

		typedef SMap<int, LineTag> CoordLineData;
		CoordLineData m_Data;
	private:
		bool m_CursorInCoord;
		CPoint m_CursorPoint;
	};

}