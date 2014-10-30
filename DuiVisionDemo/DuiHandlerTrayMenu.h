// DuiVision message handler base class
#pragma once
class CDuiObject;

// DUI事件处理类
class CDuiHandlerTrayMenu : public CDuiHandler
{
public:
  CDuiHandlerTrayMenu(void);
  virtual ~CDuiHandlerTrayMenu(void);

  BOOL m_checked;
  CString m_radio_name;
  virtual void OnInit();
	LRESULT OnDuiMsgMenuButton1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgMenuButton2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);

	// 消息处理定义
	DUI_DECLARE_MESSAGE_BEGIN(CDuiHandlerTrayMenu)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"menuitem_checkbox", MSG_BUTTON_CHECK, OnDuiMsgMenuButton1)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"menuitem_radio_1", MSG_BUTTON_CHECK, OnDuiMsgMenuButton2)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"menuitem_radio_2", MSG_BUTTON_CHECK, OnDuiMsgMenuButton2)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"menuitem_radio_3", MSG_BUTTON_CHECK, OnDuiMsgMenuButton2)
	DUI_DECLARE_MESSAGE_END()
};
