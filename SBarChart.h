/**
* Copyright (C) 2019-2050
* All rights reserved.
*
* @file       SBarChart.h
* @brief
* @version    v1.0
* @author     yiywain(逸远)
* @date       2019/12/19
* gitee  https://gitee.com/yiywain/soui-extend.git
* Describe    柱状图
*/

#pragma once
#include "SCoordAxis.h"
namespace SOUI
{
	class SBarChart :
		public SCoordAxis
	{
		SOUI_CLASS_NAME(SChart_bar, L"BarChart")
	public:

		struct BarDataType{
			float Value;
			COLORREF Color;
		};

		SBarChart();
		~SBarChart();

		void AddData(int id, float Value);

		void AddData(int id, float Value, COLORREF Color);

		void SetData(int id, SArray<BarDataType> &Data);

		SArray<BarDataType> &GetData(int id);

		void Clear(void);

		bool Remove(int id);

		//设置显示单位
		void SetUnit(SStringT Unit);

		void ShowValue(bool Show);

		void SetBarInterval(float Interval);

		void SetBarWidth(float Width);

		void UpdateWindow()
		{
			InvalidateRect(&GetClientRect());
		}
	protected:

		virtual void GetMaxMin(float &Max, float &Min);

		void OnPaint(IRenderTarget *pRT);

	protected:
		SOUI_MSG_MAP_BEGIN()
			MSG_WM_PAINT_EX(OnPaint)

			SOUI_MSG_MAP_END()

			SOUI_ATTRS_BEGIN()
			ATTR_COLOR(L"BarColor", m_BarColor, TRUE)
			ATTR_FLOAT(L"BarWidth", m_BarWidth, TRUE)
			ATTR_FLOAT(L"BarInterval", m_BarInterval, TRUE)
			ATTR_BOOL(L"ShowValue", m_ShowValue, TRUE)
			ATTR_STRINGT(L"Unit", m_Unit, TRUE)

			ATTR_FONT(L"BarTextFont", m_BarTextFont, TRUE)
			ATTR_COLOR(L"BarTextColor", m_BarTextColor, TRUE)
			SOUI_ATTRS_END()
	protected:
		float m_BarWidth;//单个柱的宽度 支持百分比
		float m_BarInterval;//柱间隔 支持百分比
		COLORREF m_BarColor;//柱的默认颜色
		bool m_ShowValue;//是否显示数值
		SStringT m_Unit;//显示数值的单位 不显示则无效

		SDpiAwareFont m_BarTextFont;//
		COLORREF m_BarTextColor;//
	private:
		typedef SMap<int, SArray<BarDataType>> CoordDataType;
		CoordDataType m_Data;
	};

}