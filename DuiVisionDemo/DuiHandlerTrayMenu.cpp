#include "StdAfx.h"
#include "DuiHandlerTrayMenu.h"


//////////////////////////////////////////////////////////////
// CDuiHandlerTrayMenu

CDuiHandlerTrayMenu::CDuiHandlerTrayMenu(void) : CDuiHandler()
{
  m_checked = false;
  m_radio_name = "menuitem_radio_1";
}

CDuiHandlerTrayMenu::~CDuiHandlerTrayMenu(void)
{
}

// ��ʼ��
void CDuiHandlerTrayMenu::OnInit()
{
  {
     CMenuItem* pDuiMenuItem = (CMenuItem*) GetControl(_T("menuitem.radio"));
     if(pDuiMenuItem)
     {
       pDuiMenuItem->SetDuiHandler(this);
     }
  }
 

  {
    this->SetTitle(_T("close_app"),_T("�˳�APP"));
  }

  {
    CMenuItem* pDuiMenuItem = (CMenuItem*)GetControl(_T("menuitem_checkbox"));
    if(pDuiMenuItem)
    {
      pDuiMenuItem->SetCheck(m_checked);
    } 
  }

  {
    CMenuItem* pDuiMenuItem = (CMenuItem*)GetControl(m_radio_name);
    if(pDuiMenuItem)
    {
      pDuiMenuItem->ResetGroupCheck();
      pDuiMenuItem->SetCheck(TRUE);
    } 
  }


}

// ����˫����Ϣ����
LRESULT CDuiHandlerTrayMenu::OnDuiMsgMenuButton1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	m_checked = !m_checked;
	return TRUE;
}

// ����˫����Ϣ����
LRESULT CDuiHandlerTrayMenu::OnDuiMsgMenuButton2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	this->GetControl(uID);
  m_radio_name = strName;
	return TRUE;
}