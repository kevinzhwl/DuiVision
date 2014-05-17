#pragma once
#include "ControlBase.h"

class CStaticText : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CStaticText, "text")
public:
	CStaticText(HWND hWnd, CDuiObject* pDuiObject);
	CStaticText(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString strTitle, 
		Color clr = Color(254, 0, 0, 0), CString strFont = DuiSystem::GetDefaultFont(), int nFontWidth = 12, FontStyle fontStyle = FontStyleRegular, BOOL bIsVisible = TRUE);
	virtual ~CStaticText(void);

	void SetMarkText(CString strMark);
	void SetTitleMarkText(CString strTitle, CString strMark, int nStart = 0);
	void SetMarkText(CString strMark, Color clrMark = Color(237, 128, 27), int nStart = 0);
	void SetMarkText(CString strTitle, CString strMark, Color clrMark = Color(237, 128, 27), int nStart = 0);

	void SetTextColor(Color clrText) { m_clrText = clrText; };

	int GetVirtualHeight();

	HRESULT OnAttributeBackColor(const CStringA& strValue, BOOL bLoading);
	HRESULT OnAttributeTextHoverColor(const CStringA& strValue, BOOL bLoading);
	HRESULT OnAttributeTextShadowColor(const CStringA& strValue, BOOL bLoading);
	HRESULT OnAttributeImageScroll(const CStringA& strValue, BOOL bLoading);

	// ��Ϣ��Ӧ
	virtual LRESULT OnBaseMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnMessage(UINT uID, UINT Msg, WPARAM wParam, LPARAM lParam);

protected:
	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point);

	virtual void SetControlRect(CRect rc);
	virtual void DrawControl(CDC &dc, CRect rcUpdate);

public:
	CControlBaseFont*	m_pControScrollV;	// ��ֱ������
	CControlBase*		m_pControBkArea;	// ����Area
	BOOL				m_bScrollV;			// �Ƿ����ô�ֱ������
	BOOL				m_bEnableHover;		// �Ƿ���������ƶ�ʱ���ɫ
	BOOL				m_bEnableShadow;	// �Ƿ���������Ӱ
	bool				m_bBack;			// �Ƿ���䱳��
	Color				m_clrBack;			// ����ɫ
	Color				m_clrText;			// ������ɫ
	Color				m_clrTextHover;		// �ȵ�������ɫ
	Color				m_clrTextShadow;	// ��Ӱ������ɫ
	Color				m_clrMark;			// �����ɫ
	CString				m_strMark;			// �������
	int					m_nStart;			// �����ʼλ��
	enumButtonState		m_enButtonState;	// ״̬
	int					m_nVirtualHeight;	// ��ʾȫ��������Ҫ������ĸ߶�
	int					m_nBkTransparent;	// ����͸����

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE("img-scroll", OnAttributeImageScroll)
		DUI_COLOR_ATTRIBUTE("crtext", m_clrText, FALSE)
		DUI_CUSTOM_ATTRIBUTE("crhover", OnAttributeTextHoverColor)
		DUI_CUSTOM_ATTRIBUTE("crshadow", OnAttributeTextShadowColor)
		DUI_CUSTOM_ATTRIBUTE("crback", OnAttributeBackColor)
		DUI_COLOR_ATTRIBUTE("crmark", m_clrMark, FALSE)
		DUI_TSTRING_ATTRIBUTE("mask", m_strMark, FALSE)
		DUI_INT_ATTRIBUTE("bk-transparent", m_nBkTransparent, FALSE)
	DUI_DECLARE_ATTRIBUTES_END()
};