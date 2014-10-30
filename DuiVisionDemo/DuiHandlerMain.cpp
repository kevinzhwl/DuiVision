#include "StdAfx.h"
#include "DuiHandlerMain.h"
#include "registry.h"
#include "DuiHandlerTrayMenu.h"

#define XML_ABOUT_DLG L"<?xml version=\"1.0\" encoding=\"utf-8\"?>\
<dlg name=\"dlg_about\" title=\"MsgBox\" width=\"450\" height=\"230\" appwin=\"0\" >\
	<base>\
		<imgbtn name=\"button.close\" pos=\"-45,0,-0,29\" skin=\"IDB_BT_CLOSE\" shortcut=\"ESC\"/>\
		<text name=\"title\" crtext=\"FFFFFF\" crmark=\"800000\" font=\"big\"\
				pos=\"10,5,200,25\" title=\"����[APP_NAME]\" mask=\"[APP_NAME]\" response=\"0\" />\
	</base>\
	<body>\
		<area name=\"area-1\" pos=\"0,0,-0,40\" begin-transparent=\"100\" end-transparent=\"30\" />\
		<area name=\"area-2\" pos=\"0,40,-0,-0\" begin-transparent=\"30\" end-transparent=\"30\" />\
		<area name=\"area-3\" pos=\"0,-37,-0,-36\" begin-transparent=\"70\" end-transparent=\"70\" />\
		<area name=\"area-4\" pos=\"0,-36,-0,-0\" begin-transparent=\"88\" end-transparent=\"88\" />\
		\
		<img name=\"icon\" pos=\"25,45\" width=\"128\" height=\"128\" image=\"skins\\scriptnet.jpg\" mode=\"normal\" framesize=\"0\" response=\"0\" />\
		\
		<text crtext=\"000000\" pos=\"170,45,-25,65\" title=\"[APP_NAME] [APP_VER]\" />\
		<text crtext=\"000000\" pos=\"170,65,-25,85\" title=\"��Ȩ����2013-2014\" />\
		<linkbtn name=\"linkbtn1\" crtext=\"800000\"\
			pos=\"170,100,-25,130\" title=\"���������\" href=\"http://www.blueantstudio.net\" />\
		<text crtext=\"000080\" pos=\"170,140,-25,160\" title=\"�˶Ի����ɶ����XML���ݴ���\" />\
		<button name=\"button.ok\" skin=\"IDB_BT_DEFAULT\" title=\"[OK]\" pos=\"-100,-30,-20,-6\" />\
	</body>\
</dlg>";

//////////////////////////////////////////////////////////////
// CDuiHandlerMain

CDuiHandlerMain::CDuiHandlerMain(void) : CDuiHandler()
{
	m_pDlg = NULL;
	m_uTimerAni = 0;
	m_nAniIndex = 0;
}

CDuiHandlerMain::~CDuiHandlerMain(void)
{
}

// ��ʼ��
void CDuiHandlerMain::OnInit()
{
	// ��ʼ������ͼ��
	DuiSystem::Instance()->InitTray(new CDuiHandlerTrayMenu());
	// ��tabҳ3ע���¼��������
	//DuiSystem::RegisterHandler(m_pDlg, new CDuiHandlerTab3(), _T("tab3"));
	// ������ҳע���¼��������
	//CDuiHandlerTools* pDuiHandlerTools = new CDuiHandlerTools();
	//pDuiHandlerTools->SetDialog(m_pDlg);
	//DuiSystem::RegisterHandler(m_pDlg, pDuiHandlerTools, _T("tab.tools"));
	//pDuiHandlerTools->OnInit();
	// ����������ʱ��
	m_uTimerAni = DuiSystem::AddDuiTimer(500);

	// ��ʼ��ԭ���ؼ�
	CDuiNativeWnd* pNativeWnd = (CDuiNativeWnd*)GetControl(_T("nativewnd_1"));
	if(pNativeWnd)
	{
		CEdit* pEdit = new CEdit;
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_MULTILINE | ES_AUTOVSCROLL;
		pEdit->Create(dwStyle, CRect(0,0,0,0), pNativeWnd->GetPaintWnd(), 1111);
		pNativeWnd->SetNativeWnd(pEdit);

		/*CListViewCtrlEx* pWndList = new CListViewCtrlEx;
		DWORD dwStyle = WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL | LVS_OWNERDRAWFIXED;
		pWndList->Create(pNativeWnd->GetPaintHWnd(), NULL, NULL,
			dwStyle, 0, 1112, NULL);
		pWndList->SetObserverWindow(pNativeWnd->GetPaintHWnd());
		pWndList->InsertColumn(0, L"��1", LVCFMT_CENTER, 50);
		pWndList->InsertColumn(1, L"��2", LVCFMT_CENTER, 150);
		pWndList->InsertColumn(2, L"��3", LVCFMT_CENTER, 100);
		pWndList->SetItemHeight(22);
		int nItem = pWndList->AppendTitle("row1", Color(80,80,80), LISTITEM_EXPANDABLE|LISTITEM_BOLD);
		pWndList->AppendSubItem(nItem, "test1", SUBITEM_LINK);
		pNativeWnd->SetNativeWnd(pWndList);*/
	}

	// ��ʾ���ؼ�ͨ��API�����ӿؼ�
	CDuiGridCtrl* pGridCtrl = (CDuiGridCtrl*)GetControl(_T("gridctrl_1"));
	if(pGridCtrl)
	{
		CLinkButton* pControl = (CLinkButton*)DuiSystem::CreateControlByName(L"linkbtn", NULL, NULL);
		if(pControl)
		{
			pControl->SetName(L"grid1_updateinfo_link");
			pControl->SetPosStr(L"100, 3, -10, 25");
			pControl->SetTitle(L"��������");
			pControl->SetLink(L"http://www.blueantstudio.net");
			pGridCtrl->AddSubItemControl(1, 0, pControl);
		}
	}

	// ��ʾ���ؼ�ͨ��API�����ӿؼ�
	CDuiTreeCtrl* pTreeCtrl = (CDuiTreeCtrl*)GetControl(_T("treectrl_1"));
	if(pTreeCtrl)
	{
		HTREEITEM hNode = pTreeCtrl->GetNodeWithId(L"1-0");
		CLinkButton* pControl = (CLinkButton*)DuiSystem::CreateControlByName(L"linkbtn", NULL, NULL);
		if(pControl)
		{
			pControl->SetName(L"tree1_updateinfo_link");
			pControl->SetPosStr(L"10, 5, -10, 25");
			pControl->SetTitle(L"��������");
			pControl->SetLink(L"http://www.blueantstudio.net");
			pTreeCtrl->AddSubItemControl(hNode, 2, pControl);
		}
	}
}

// Ƥ����Ϣ����(ʵ��Ƥ���ı���ͻ�ȡ)
LRESULT CDuiHandlerMain::OnDuiMsgSkin(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if(Msg == MSG_GET_SKIN_TYPE)	// ��ȡSkin����
	{
		CRegistryUtil reg(HKEY_CURRENT_USER);
		int nBkType = reg.GetDWordValue(NULL, REG_CONFIG_SUBKEY, REG_CONFIG_BKTYPE);
		*(int*)wParam = nBkType;
		return TRUE;
	}else
	if(Msg == MSG_GET_SKIN_VALUE)	// ��ȡSkinֵ
	{
		CRegistryUtil reg(HKEY_CURRENT_USER);
		if(wParam == BKTYPE_IMAGE_RESOURCE)
		{
			*(int*)lParam = reg.GetDWordValue(NULL, REG_CONFIG_SUBKEY, REG_CONFIG_BKPIC_RES);
			return TRUE;
		}else
		if(wParam == BKTYPE_COLOR)
		{
			*(COLORREF*)lParam = reg.GetDWordValue(NULL, REG_CONFIG_SUBKEY, REG_CONFIG_BKCOLOR);
			return TRUE;
		}else
		if(wParam == BKTYPE_IMAGE_FILE)
		{
			*(CString*)lParam = reg.GetStringValue(NULL, REG_CONFIG_SUBKEY, REG_CONFIG_BKPIC_FILE);
			return TRUE;
		}
	}else
	if(Msg == MSG_SET_SKIN_VALUE)	// ����Skinֵ
	{
		CRegistryUtil reg(HKEY_CURRENT_USER);
		reg.SetDWordValue(HKEY_CURRENT_USER, REG_CONFIG_SUBKEY, REG_CONFIG_BKTYPE, wParam);
		if(wParam == BKTYPE_IMAGE_RESOURCE)
		{
			reg.SetDWordValue(HKEY_CURRENT_USER, REG_CONFIG_SUBKEY, REG_CONFIG_BKPIC_RES, lParam);
		}else
		if(wParam == BKTYPE_COLOR)
		{
			reg.SetDWordValue(HKEY_CURRENT_USER, REG_CONFIG_SUBKEY, REG_CONFIG_BKCOLOR, lParam);
		}else
		if(wParam == BKTYPE_IMAGE_FILE)
		{
			CString* pstrImgFile = (CString*)lParam;
			reg.SetStringValue(HKEY_CURRENT_USER, REG_CONFIG_SUBKEY, REG_CONFIG_BKPIC_FILE, *pstrImgFile);
		}
		return TRUE;
	}
	return FALSE;
}

// ����˫����Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgTrayIconDClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::ShowDuiDialog(_T("dlg_login"), NULL);
	return TRUE;
}

// �������������Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgTrayIconLButtonDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	//DuiSystem::ShowDuiDialog(_T("dlg_about"), NULL);
	return TRUE;
}

// ��ʾ��ʾ����1��Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgNotifyButton1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::AddDuiNotifyMsgTask(_T("��ʾ��Ϣ1��\n�����ʾ���񣬴��ڴ�С����Ϊ400*250���˴���5���Ӻ��Զ��ر�..."), MB_ICONINFORMATION | 2, _T("�Զ������"), 5000, 400, 250);
	return TRUE;
}

// ��ʾ��ʾ����2��Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgNotifyButton2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::AddDuiNotifyMsgTask(_T("��ʾ��Ϣ2��\n�����ʾ���񣬴��ڴ�С����Ϊ300*200���˴���3���Ӻ��Զ��ر�..."), MB_ICONINFORMATION | 2, _T(""), 3000, 300, 200);
	return TRUE;
}

// ��ʾ��ʾ����3��Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgNotifyButton3(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// �Զ����Notify��ʾ����
	DuiSystem::Instance()->CreateNotifyMsgBox(_T("dlg_notifymsg"));
	// ���ñ��������
	DuiSystem::SetNotifyMsgBoxControlTitle(_T("notify.title"), _T("�Զ�����ʾ����"));
	DuiSystem::SetNotifyMsgBoxControlTitle(_T("notify.text"), _T("������ʽ��ʾ���ڣ����Զ��رգ���СΪ250*300����ʾȷ����ť��................."));
	// ����ͼ��
	DuiSystem::SetNotifyMsgBoxControlImage(_T("notify.icon"), _T("skin:IDB_ICON_INFO"));
	// ��������,5������,��Ҫ�Ѳ��õ��������ص�
	DuiSystem::SetNotifyMsgBoxControlVisible(_T("notify.type.0"), TRUE);
	DuiSystem::SetNotifyMsgBoxControlVisible(_T("notify.type.1"), FALSE);
	DuiSystem::SetNotifyMsgBoxControlVisible(_T("notify.type.2"), FALSE);
	DuiSystem::SetNotifyMsgBoxControlVisible(_T("notify.type.3"), FALSE);
	DuiSystem::SetNotifyMsgBoxControlVisible(_T("notify.type.4"), FALSE);
	// ���ô�С
	DuiSystem::SetNotifyMsgBoxSize(250, 300);
	// ��ʾ����
	DuiSystem::ShowNotifyMsgBox();
	return TRUE;
}

// ��ʾ��Ϣ�Ի���1��Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgMsgBoxButton1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::DuiMessageBox(NULL, _T("��ʾ�Ի���1��Ĭ�ϵĶԻ���\n1.����ʹ�ó���Ĭ�ϱ���\n2.��ťʹ��Ĭ�ϵ�ȷ��(MB_OK)��ť\n3.ͼ��ʹ��Ĭ�ϵ���Ϣͼ��"));
	return TRUE;
}

// ��ʾ��Ϣ�Ի���2��Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgMsgBoxButton2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::DuiMessageBox(NULL, _T("��ʾ�Ի���2��\n1.����ʹ���Զ������\n2.��ťʹ��ȷ��ȡ��(MB_OKCANCEL)��ť\n3.ͼ��ʹ�ô���ͼ��"), _T("�Զ������"), MB_OKCANCEL|MB_ICONERROR);
	return TRUE;
}

// ��ʾ��Ϣ�Ի���3��Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgMsgBoxButton3(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::DuiMessageBox(NULL, _T("��ʾ�Ի���3��\n1.����ʹ�ó���Ĭ�ϱ���\n2.��ťʹ���Ƿ�ȡ��(MB_YESNOCANCEL)��ť\n3.ͼ��ʹ�ü��ͼ��\n4.�Զ���Ի���Ĵ�С"), _T(""), MB_YESNOCANCEL, 370, 200);
	return TRUE;
}

// ������ʾ��ʱ����Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgTimerButton1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::AddDuiTimer(20000, _T("timer_notify"));
	DuiSystem::DuiMessageBox(NULL, _T("��ʾ��ʱ������������ʱ����20�룡"));
	return TRUE;
}

// ֹͣ��ʾ��ʱ����Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgTimerButton2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::RemoveDuiTimer(_T("timer_notify"));
	DuiSystem::DuiMessageBox(NULL, _T("��ʾ��ʱ����ֹͣ��"));
	return TRUE;
}

// ��ʾXML�Ի�����Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgXmlDlgButton(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// ��ʾͨ�������XML���ݼ��ش���
	CString strDlgXmlContent = XML_ABOUT_DLG;
	CDlgBase* pDlg = DuiSystem::CreateDuiDialog(strDlgXmlContent, GetControlDialog(uID), _T(""), TRUE, 0, TRUE);
	if(pDlg != NULL)
	{
		int nResponse = pDlg->DoModal();
		DuiSystem::Instance()->RemoveDuiDialog(pDlg);
	}
	return TRUE;
}

// ������tabҳ�е�ĳ��ҳ����Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgHideTabButton(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)(pDlg->GetControl(_T("tabctrl.main")));
	if(pTabCtrl)
	{
		pTabCtrl->SetItemVisible(7, FALSE);
		//pTabCtrl->DeleteItem(6);
		//pTabCtrl->DeleteItem(L"tab_6");
	}
	return TRUE;
}

// ��ʾ��tabҳ�е�ĳ��ҳ����Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgShowTabButton(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)(pDlg->GetControl(_T("tabctrl.main")));
	if(pTabCtrl)
	{
		pTabCtrl->SetItemVisible(7, TRUE);
	}
	return TRUE;
}

// ɾ��Webҳ��Tabҳ��Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgDelWebTabButton(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)(pDlg->GetControl(_T("tabctrl.main")));
	if(pTabCtrl)
	{
		pTabCtrl->DeleteItem(L"tab.activex");
	}
	return TRUE;
}

// ����Webҳ��Tabҳ��Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgLoadWebTabButton(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)(pDlg->GetControl(_T("tabctrl.main")));
	if(pTabCtrl)
	{
		pTabCtrl->LoadTabXml(L"duivision\\tab_ext_web.xml");
	}
	return TRUE;
}

// �˵�1��Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgMenuButton1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiButton* pButton = (CDuiButton*)GetControl(_T("menu_1"));
	CDuiMenu *pDuiMenu = new CDuiMenu(DuiSystem::GetDefaultFont(), 12);
	pDuiMenu->SetParent(pButton);
	CPoint point;
	CRect rc = pButton->GetRect();
	point.SetPoint(rc.left, rc.bottom);
	if(pDlg != NULL)
	{
		pDlg->ClientToScreen(&point);
	}
	// ��ʾ����ڲ˵�����ʱ����Ĳ˵������ʾ���⡢�ɼ��ԡ��Ƿ���á��Ƿ�ѡ�������
	// �����ڵ���LoadXmlFile֮ǰͨ���˵���������������Ӧ�˵��������
	pDuiMenu->SetItemTitle(L"item_login", L"��֤--�޸�Title");
	pDuiMenu->SetItemCheck(L"item_setup", 0);
	pDuiMenu->SetItemVisible(L"item_help", FALSE);
	pDuiMenu->SetItemDisable(L"item_about", TRUE);
	if(pDuiMenu->LoadXmlFile(_T("menu_tray"), pDlg, point, WM_DUI_MENU))
	{
		pDuiMenu->ShowWindow(SW_SHOW);
	}
	return TRUE;
}

// �˵�2��Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgMenuButton2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiButton* pButton = (CDuiButton*)GetControl(_T("menu_2"));
	CDuiMenu *pDuiMenu = new CDuiMenu(DuiSystem::GetDefaultFont(), 12);
	pDuiMenu->SetParent(pButton);
	CPoint point;
	CRect rc = pButton->GetRect();
	point.SetPoint(rc.left, rc.bottom);
	if(pDlg != NULL)
	{
		pDlg->ClientToScreen(&point);
	}
	if(pDuiMenu->LoadXmlFile(_T("duivision\\menu_2.xml"), pDlg, point, WM_DUI_MENU))
	{
		pDuiMenu->ShowWindow(SW_SHOW);
	}
	return TRUE;
}

// �˵�3��Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgMenuButton3(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiButton* pButton = (CDuiButton*)GetControl(_T("menu_3"));
	CDuiMenu *pDuiMenu = new CDuiMenu(DuiSystem::GetDefaultFont(), 12);
	pDuiMenu->SetParent(pButton);
	CPoint point;
	CRect rc = pButton->GetRect();
	point.SetPoint(rc.left, rc.bottom);
	if(pDlg != NULL)
	{
		pDlg->ClientToScreen(&point);
	}
	if(pDuiMenu->LoadXmlFile(_T("duivision\\menu_3.xml"), pDlg, point, WM_DUI_MENU))
	{
		pDuiMenu->ShowWindow(SW_SHOW);
	}
	return TRUE;
}

// �б�ؼ������Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgListCtrl1Click(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// ������б�ؼ�ĳһ��,��ʾ���е���ϸ��Ϣ,���������wParam��ʾ�ؼ��к�
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiListCtrl* pListCtrl = (CDuiListCtrl*)GetControl(_T("listctrl_1"));
	ListRowInfo* pRowInfo = pListCtrl->GetItemInfo(wParam);
	if(pDlg && pRowInfo)
	{
		CDlgPopup* pDlgPopup = new CDlgPopup;
		CRect rc = pListCtrl->GetRect();
		rc.OffsetRect(50, 30);
		pDlgPopup->LoadXmlFile(_T("xml:dlg_notice"));
		CControlBaseFont* pControlTitle = (CControlBaseFont*)(pDlgPopup->GetControl(_T("notice.title")));
		if(pControlTitle)
		{
			pControlTitle->SetTitle(pRowInfo->strTitle);
		}
		CControlBaseFont* pControlTime = (CControlBaseFont*)(pDlgPopup->GetControl(_T("notice.time")));
		if(pControlTime)
		{
			pControlTime->SetTitle(pRowInfo->strTime);
		}
		CControlBaseFont* pControlContent = (CControlBaseFont*)(pDlgPopup->GetControl(_T("notice.content")));
		if(pControlContent)
		{
			//CString strTmp = L"www\nwww\nj";
			pControlContent->SetTitle(pRowInfo->strContent);
		}
		pDlg->OpenDlgPopup(pDlgPopup, rc, 0);
	}
	return TRUE;
}

// �б�ؼ������Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgListCtrl2Click(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// ������б�ؼ�ĳһ��,��ʾ���е���ϸ��Ϣ
	// ���������wParam��ʾ�ؼ��к�,lParam��ʾ��������ӵ�����(����1��2�ֱ�Ϊ0��1)
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiListCtrl* pListCtrl = (CDuiListCtrl*)GetControl(_T("listctrl_2"));
	ListRowInfo* pRowInfo = pListCtrl->GetItemInfo(wParam);
	if(pDlg && pRowInfo)
	{
		if(lParam != -1)
		{
			// ������е�ĳ������
			CString strMsg;
			strMsg.Format(_T("������б��е�����\n�����ݣ�%s\n���������ţ�%d"), pRowInfo->strTitle, lParam);
			DuiSystem::DuiMessageBox(NULL, strMsg);
		}else
		{
			CDlgPopup* pDlgPopup = new CDlgPopup;
			CRect rc = pListCtrl->GetRect();
			rc.OffsetRect(50, 30);
			pDlgPopup->LoadXmlFile(_T("xml:dlg_notice"));
			CControlBaseFont* pControlTitle = (CControlBaseFont*)(pDlgPopup->GetControl(_T("notice.title")));
			if(pControlTitle)
			{
				pControlTitle->SetTitle(pRowInfo->strTitle);
			}
			CControlBaseFont* pControlTime = (CControlBaseFont*)(pDlgPopup->GetControl(_T("notice.time")));
			if(pControlTime)
			{
				pControlTime->SetTitle(pRowInfo->strTime);
			}
			CControlBaseFont* pControlContent = (CControlBaseFont*)(pDlgPopup->GetControl(_T("notice.content")));
			if(pControlContent)
			{
				pControlContent->SetTitle(pRowInfo->strContent);
			}
			pDlg->OpenDlgPopup(pDlgPopup, rc, 0);
		}
	}

	return TRUE;
}

// ���ؼ���ɾ����ť�ӿؼ������Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgGridCtrlDelBtnClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// ����˱��ؼ���ɾ���а�ť�ӿؼ�
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiGridCtrl* pGridCtrl = (CDuiGridCtrl*)GetControl(_T("gridctrl_1"));
	pGridCtrl->DeleteRow(3);
	
	return TRUE;
}

// ���ؼ������Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgTreeCtrlClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDuiTreeCtrl* pTreeCtrl = (CDuiTreeCtrl*)GetControl(_T("treectrl_1"));
	HTREEITEM hNode = (HTREEITEM)wParam;	// ���ڵ���	
	int nItem = lParam;	// ��������ؼ��ĵڼ���
	if(nItem == 0)
	{
		// ��������һ��,��չ���������˽ڵ�
		pTreeCtrl->ToggleNode(hNode);
	}
	return TRUE;
}

// ��ʾϵͳ���öԻ���˵���Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgMenuOption(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = DuiSystem::CreateDuiDialog(_T("dlg_option"), m_pDlg, _T(""), TRUE, 0, TRUE);
	if(pDlg == NULL)
	{
		return FALSE;
	}

	// IP��ַ
	pDlg->SetControlValue(L"config.server.ip", L"title", L"192.168.1.1");

	int nResponse = pDlg->DoModal();
	DuiSystem::Instance()->RemoveDuiDialog(pDlg);
	return TRUE;
}

// ϵͳ���öԻ����ȷ�ϰ�ť��Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgOptionDlgOK(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	if(pDlg == NULL)
	{
		return FALSE;
	}

	// IP��ַ
	CString strServerAddr = L"";
	CDuiEdit* pControlIp = static_cast<CDuiEdit*>(pDlg->GetControl(L"config.server.ip"));
	if(pControlIp)
	{
		strServerAddr = pControlIp->GetEditText();
		if(strServerAddr == L"")
		{
			// ��ַ����Ϊ����ʾ
			return TRUE;
		}
	}

	pDlg->DoOK();
	return TRUE;
}

// ��ʾFlash1��ť��Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgBtnShowFlash1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiFlashCtrl* pFlashCtrl = (CDuiFlashCtrl*)(pDlg->GetControl(_T("flash_ctrl_1")));
	if(pFlashCtrl)
	{
		pFlashCtrl->Navigate(L"flash/afternoon.swf");
	}
	return TRUE;
}

// ��ʾFlash2��ť��Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgBtnShowFlash2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiFlashCtrl* pFlashCtrl = (CDuiFlashCtrl*)(pDlg->GetControl(_T("flash_ctrl_1")));
	if(pFlashCtrl)
	{
		pFlashCtrl->Navigate(L"flash/morning.swf");
	}
	return TRUE;
}

// ��ʾFlash3��ť��Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgBtnShowFlash3(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiFlashCtrl* pFlashCtrl = (CDuiFlashCtrl*)(pDlg->GetControl(_T("flash_ctrl_1")));
	if(pFlashCtrl)
	{
		pFlashCtrl->Navigate(L"flash/night.swf");
	}
	return TRUE;
}

// ��ʾFlash4��ť��Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgBtnShowFlash4(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiFlashCtrl* pFlashCtrl = (CDuiFlashCtrl*)(pDlg->GetControl(_T("flash_ctrl_1")));
	if(pFlashCtrl)
	{
		pFlashCtrl->Navigate(L"flash/noon.swf");
	}
	return TRUE;
}

/*
// DUI�¼�����
LRESULT CDuiHandlerMain::OnDuiMessage(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::LogEvent(LOG_LEVEL_DEBUG, _T("CDuiHandlerMain::OnDuiMessage:uID=%d, name=%s, msg=%d, wParam=%d, lParam=%d"),
						uID, strName, Msg, wParam, lParam);

    return 0;
}
*/
// DUI��ʱ���¼�����
void CDuiHandlerMain::OnTimer(UINT uTimerID, CString strTimerName)
{
    if(uTimerID == m_uTimerAni)
	{
		CDuiProgress* pProgress2 = (CDuiProgress*)GetControl(_T("progress_2"));
		if(pProgress2)
		{
			pProgress2->SetProgress(m_nAniIndex*10);
		}

		m_nAniIndex++;
		if(m_nAniIndex > 10)
		{
			m_nAniIndex = 0;
		}
	}else
	if(_T("timer_notify") == strTimerName)
	{
		// ��ʱ��ʾ��ʾ��
		DuiSystem::AddDuiNotifyMsgTask(_T("�����ɶ�ʱ����������ʾ��\n�������ڣ�20��\n�˴���5���Ӻ��Զ��ر�..."), MB_ICONINFORMATION | 2, _T(""), 5000);
	}
}

// ���̼���Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgInterprocess(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// �����в���,���Զ������в������д���,Ҳ����ֱ����ʾ������
	DUI_INTERPROCESS_MSG* pInterMsg = (DUI_INTERPROCESS_MSG*)lParam;
	CString strCmd = pInterMsg->wInfo;
	if(!strCmd.IsEmpty())
	{
		DuiSystem::DuiMessageBox(NULL, L"ִ���������в���:" + strCmd);
	}else
	{
		CDlgBase* pDlg = DuiSystem::Instance()->GetDuiDialog(L"dlg_main");
		if(pDlg)
		{
			pDlg->SetForegroundWindow();
			pDlg->ShowWindow(SW_NORMAL);
			pDlg->ShowWindow(SW_SHOW);
			pDlg->BringWindowToTop();
		}
	}
	return TRUE;
}
