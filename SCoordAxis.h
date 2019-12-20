/**
* Copyright (C) 2019-2050
* All rights reserved.
*
* @file		  SCoordAxis.h
* @brief
* @version    v1.0
* @author     yiywain(逸远)
* @date       2019/12/19
* gitee  https://gitee.com/yiywain/soui-extend.git
* Describe   坐标系基础类
*/

#pragma once
namespace SOUI
{

class SCoordAxis : public SWindow
{
	SOUI_CLASS_NAME(SChart_bar, L"CoordAxis")
public:
	SCoordAxis();
	virtual ~SCoordAxis();

	enum AXIS_TYPE{
		AXIS_LENGTHWAYS,//纵向
		AXIS_CROSSWISE//横向
	};
	//添加坐标显示文本
	void AddText(SStringT &Text);

	//清空
	void ClearText(void);

	void SetTextByIndex(int Index,SStringT &Text);

	SStringT GetTextByIndex(int Index);

	//设置坐标系外边距
	void SetCoordMargin(RECT &rect);

	//设置坐标颜色以及线宽
	void SetCoord(COLORREF Color, int Width);

	//设置坐标系刻度线长度宽度以及颜色
	void SetCoordScale(float xLen, float yLen, int Width, COLORREF Color);

	//设置自动计算刻度范围 ReserveValue 最小值与刻度最小的距离
	void SetAutoScope(float ReserveValue);

	//设置固定刻度的范围
	void SetLockScope(float LowerValue);

	//设置显示的小数位数
	void SetDecimal(int Number);

	//设置刻度数量
	void SetScaleNumber(int Number);
	
protected:
	void OnPaint(IRenderTarget *pRT);

//	virtual CRect GetMargin(void)=0;//提供坐标轴的外边距
	virtual void GetMaxMin(float &Max, float &Min)=0;

	CRect &GetDataDrawRect(void);//获取数据绘制区域
	float GetValueRatio(void);

	bool NumberToScaleStr(float value, int Digit, SStringT& str);
private:
	int MeasureText(IRenderTarget *pRT, SStringT Text, int TextHeight, SIZE *size);

protected:
	SOUI_MSG_MAP_BEGIN()
		//MSG_WM_CREATE(OnCreate)
		MSG_WM_PAINT_EX(OnPaint)
	SOUI_MSG_MAP_END()

	SOUI_ATTRS_BEGIN()
		ATTR_RECT(L"CoordMargin", m_CoordMargin,TRUE)
		ATTR_INT(L"ScaleNumber", m_ScaleNumber, TRUE)	
		ATTR_FLOAT(L"LowerNumber", m_LowerValue, TRUE)
		ATTR_BOOL(L"AutoScope", m_IsAutoScope, TRUE)
		ATTR_FLOAT(L"ReserveValue", m_ReserveValue, TRUE)
		ATTR_INT(L"CoordWidth", m_CoordWidth, TRUE)
		ATTR_COLOR(L"CoordColor", m_CoordColor, TRUE)
		ATTR_FLOAT(L"XScaleLength", m_XScaleLength, TRUE)
		ATTR_FLOAT(L"YScaleLength", m_YScaleLength, TRUE)
		ATTR_COLOR(L"ScaleLineColor", m_ScaleLineColor, TRUE)
		ATTR_INT(L"ScaleLineWidth", m_ScaleLineWidth, TRUE)
		ATTR_INT(L"Decimal", m_Decimal, TRUE)
		ATTR_BOOL(L"PullOver", m_PullOver,TRUE)
		ATTR_BOOL(L"ShowOrigin", m_ShowOrigin, TRUE)
		
		ATTR_ENUM_BEGIN(L"AxisType", AXIS_TYPE, TRUE)
			ATTR_ENUM_VALUE(L"lengthways", AXIS_LENGTHWAYS)//轴类型
			ATTR_ENUM_VALUE(L"crosswise", AXIS_CROSSWISE)//轴类型
		ATTR_ENUM_END(m_AxisType)
	SOUI_ATTRS_END()

protected:
	CRect m_CoordMargin;

	int m_ScaleNumber;//刻度数量
	float m_LowerValue;//刻度最小值
	bool m_IsAutoScope;//是否自动计算刻度
	float m_ReserveValue;//自动计算刻度需要一个保留值//支持百分比 //这个值是双向的
	float m_XScaleLength;//X轴刻度长度//支持百分比
	float m_YScaleLength;//Y轴刻度长度//支持百分比
	COLORREF m_ScaleLineColor;//刻度线颜色
	int m_ScaleLineWidth;//刻度线宽度

	int m_Decimal;//小数位
	COLORREF m_CoordColor;//坐标线颜色
	int m_CoordWidth;//坐标线宽度
	AXIS_TYPE m_AxisType;//轴类型
	bool m_PullOver;//小标题间隔是否靠边
	bool m_ShowOrigin;//是否显示原点数值

	//绘制时有效的数据
	int m_XCount;
	int m_YCount;
	float m_XSignRatio;
	float m_YSignRatio;
	float m_ValueRatio;
	float m_MinValue;
	float m_MaxValue;
	float m_ValueSpace;

	SArray<SStringT> m_Text;
	CRect m_DataRect;//数据能绘制的区域
private:
	int m_TextOffset;
};

}