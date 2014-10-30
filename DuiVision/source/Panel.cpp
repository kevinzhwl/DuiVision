#include "StdAfx.h"
#include "Panel.h"

#define	SCROLL_V	1	// �������ؼ�ID

CDuiPanel::CDuiPanel(HWND hWnd, CDuiObject* pDuiObject)
			: CControlBaseFont(hWnd, pDuiObject)
{
	m_bEnableScroll = TRUE;
	m_nScrollWidth = 8;

	CRect rcScroll = CRect(0,0,0,0);
	rcScroll.top;
	rcScroll.left = rcScroll.right - m_nScrollWidth;

 	CControlBase * pControlBase = NULL;
 	pControlBase = new CScrollV(hWnd, this, SCROLL_V, rcScroll);
 	m_vecControl.push_back(pControlBase);
	m_pControScrollV = (CControlBaseFont*)pControlBase;

	m_strXmlFile = _T("");
	m_nVirtualHeight = 0;
	m_nVirtualTop = 0;

	m_hPluginHandle = NULL;
	m_strPluginFile = _T("");
	m_pDuiPluginObject = NULL;

	m_bInit = false;
}

CDuiPanel::~CDuiPanel(void)
{
	// �ͷŽ���������Ͷ�̬��
	if(m_pDuiPluginObject != NULL)
	{
		m_pDuiPluginObject->Release();
		m_pDuiPluginObject = NULL;
	}

	if(m_hPluginHandle != NULL)
	{
		FreeLibrary(m_hPluginHandle);
		m_hPluginHandle = NULL;
	}
}

// ���ݿؼ��������ؼ�ʵ��
CControlBase* CDuiPanel::_CreateControlByName(LPCTSTR lpszName)
{
	// ����Ѿ������˴��ھ��,ʹ�ô��ھ�������ؼ�
	HWND hWnd = m_hWnd;
	if(hWnd != NULL)
	{
		return DuiSystem::CreateControlByName(lpszName, hWnd, this);
	}

	// ���Ҹ��Ի���Ĵ��ھ��,ͨ�����Ի����������ؼ�
	CDuiObject* pParentObj = GetParent();
	while((pParentObj != NULL) && (!pParentObj->IsClass(_T("dlg"))))
	{
		pParentObj = ((CControlBase*)pParentObj)->GetParent();
	}

	if((pParentObj != NULL) && pParentObj->IsClass(_T("dlg")))
	{
		return DuiSystem::CreateControlByName(lpszName, ((CDlgBase*)pParentObj)->GetSafeHwnd(), this);
	}

	return NULL;
}

// ����XML�ڵ㣬�����ڵ��е�������Ϣ���õ���ǰ�ؼ���������
BOOL CDuiPanel::Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl)
{
	if(!__super::Load(pXmlElem))
	{
		return FALSE;
	}

    // ʹ��XML�ڵ��ʼ��div�ؼ�
	if(pXmlElem != NULL)
	{
		InitUI(m_rc, pXmlElem);
	}

    return TRUE;
}

// ����XML�ļ�
BOOL CDuiPanel::LoadXmlFile(CString strFileName)
{
	DuiXmlDocument xmlDoc;
	DuiXmlNode pDivElem;

	if(!DuiSystem::Instance()->LoadXmlFile(xmlDoc, strFileName))
	{
		DuiSystem::LogEvent(DUIV_LOG_LEVELERROR, L"CDuiPanel::LoadXmlFile %s failed", strFileName);
		return FALSE;
	}

	m_strXmlFile = strFileName;
	pDivElem = xmlDoc.child(_T("div"));
	if(pDivElem == NULL)
	{
		DuiSystem::LogEvent(DUIV_LOG_LEVELERROR, L"CDuiPanel::LoadXmlFile %s failed, not found div node", strFileName);
		return FALSE;
	}

	// ����div�ڵ�����
	if(!Load(pDivElem))
	{
		DuiSystem::LogEvent(DUIV_LOG_LEVELERROR, L"CDuiPanel::LoadXmlFile %s failed, load div node fail", strFileName);
		return FALSE;
	}

	return TRUE;
}

// ��XML����ͼƬ��Ϣ����
HRESULT CDuiPanel::OnAttributeImageScroll(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	// ͨ��Skin��ȡ
	CString strSkin = _T("");
	if(strValue.Find(_T("skin:")) == 0)
	{
		strSkin = DuiSystem::Instance()->GetSkin(strValue);
		if (strSkin.IsEmpty()) return E_FAIL;
	}else
	{
		strSkin = strValue;
	}

	if(strSkin.Find(_T(".")) != -1)	// ����ͼƬ�ļ�
	{
		CString strImgFile = strSkin;
		if(strSkin.Find(_T(":")) != -1)
		{
			strImgFile = strSkin;
		}
		if(!m_pControScrollV->SetBitmap(strImgFile))
		{
			return E_FAIL;
		}
	}else	// ����ͼƬ��Դ
	{
		UINT nResourceID = _wtoi(strSkin);
		if(!m_pControScrollV->SetBitmap(nResourceID, TEXT("PNG")))
		{
			if(!m_pControScrollV->SetBitmap(nResourceID, TEXT("BMP")))
			{
				return E_FAIL;
			}
		}
	}

	return bLoading?S_FALSE:S_OK;
}

// ��XML����XML����,����XML�ļ���������Ϊdiv�����ڵ�����
HRESULT CDuiPanel::OnAttributeXml(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	DuiXmlDocument xmlDoc;
	DuiXmlNode pDivElem;

	if(DuiSystem::Instance()->LoadXmlFile(xmlDoc, strValue))
	{
		m_strXmlFile = strValue;
		pDivElem = xmlDoc.child(_T("div"));
		if(pDivElem != NULL)
		{
			// ����div�ڵ�����
			Load(pDivElem);
		}
	}

	return bLoading?S_FALSE:S_OK;
}

// ��XML����plugin����,���ؽ������ļ���̬��
HRESULT CDuiPanel::OnAttributePlugin(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	HINSTANCE hPluginHandle = NULL;
	LPVOID pPluginObj = NULL;

	if(DuiSystem::Instance()->LoadPluginFile(strValue, CEncodingUtil::AnsiToUnicode(IID_IDuiPluginPanel), hPluginHandle, pPluginObj))
	{
		m_strPluginFile = strValue;
		m_hPluginHandle = hPluginHandle;
		m_pDuiPluginObject = (IDuiPluginPanel*)pPluginObj;
		// ��ʼ��������
		UINT nIDTemplate = 0;
		HWND hWnd = NULL;
		CDlgBase* pParentDlg = GetParentDialog();
		if(pParentDlg != NULL)
		{
			nIDTemplate = pParentDlg->GetIDTemplate();
			hWnd = pParentDlg->GetSafeHwnd();
		}
		m_pDuiPluginObject->OnInit(nIDTemplate, hWnd, CEncodingUtil::UnicodeToAnsi(GetName()), m_rc);
	}

	return bLoading?S_FALSE:S_OK;
}

// ��ʼ�����ڿؼ�
void CDuiPanel::InitUI(CRect rcClient, DuiXmlNode pNode)
{
	CRect rcTemp;
	int nStartX = 0;
	int nStartY = 0;
	CControlBase * pControlBase = NULL;

	// �������д��ڿؼ�
	if(pNode)
	{
		m_nVirtualHeight = 0;
		for (DuiXmlNode pControlElem = pNode.first_child(); pControlElem; pControlElem=pControlElem.next_sibling())
		{
			if(pControlElem != NULL)
			{
				CString strControlName = pControlElem.name();
				CControlBase* pControl = _CreateControlByName(strControlName);
				if(pControl)
				{
					pControl->Load(pControlElem);
					if(pControl->IsClass(CArea::GetClassName()) || pControl->IsClass(CFrame::GetClassName()))
					{
						// Area��Frame������Ӧ���,����ӵ�Area�б���
						m_vecArea.push_back(pControl);
					}else
					{
						m_vecControl.push_back(pControl);
					}

					CRect rcCtrl = pControl->GetRect();
					if(rcCtrl.bottom > m_nVirtualHeight)
					{
						m_nVirtualHeight = rcCtrl.bottom - m_rc.top;
					}
				}
			}
		}

		// ��Ҫ���ܸ߶ȴ�����ʾ���߶ȲŻ���ʾ������
		m_pControScrollV->SetVisible(m_nVirtualHeight > m_rc.Height());
		((CScrollV*)m_pControScrollV)->SetScrollMaxRange(m_nVirtualHeight);
	}

	m_bInit = true;
}

// ��������߶�
void CDuiPanel::CalcVirtualHeight()
{
	CRect rcTemp;
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			rcTemp = pControlBase->GetRect();
			if(rcTemp.bottom > m_nVirtualHeight)
			{
				// ˢ��Panel������߶�
				m_nVirtualHeight = rcTemp.bottom - m_rc.top;
			}
		}
	}

	// ��Ҫ���ܸ߶ȴ�����ʾ���߶ȲŻ���ʾ������
	m_pControScrollV->SetVisible(m_nVirtualHeight > m_rc.Height());
	((CScrollV*)m_pControScrollV)->SetScrollMaxRange(m_nVirtualHeight);
}

void CDuiPanel::SetControlRect(CRect rc)
{
	m_rc = rc;

	if(m_pDuiPluginObject)
	{
		m_pDuiPluginObject->SetRect(rc);
	}

	CRect rcTemp;
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			UINT uControlID = pControlBase->GetControlID();
			if(SCROLL_V == uControlID)
			{
				rcTemp = m_rc;
				rcTemp.left = rcTemp.right - m_nScrollWidth;
			}else
			{
				rcTemp = pControlBase->GetRect();
				if(rcTemp.bottom > m_nVirtualHeight)
				{
					// ˢ��Panel������߶�
					m_nVirtualHeight = rcTemp.bottom - m_rc.top;
				}
				continue;
			}
			pControlBase->SetRect(rcTemp);			
		}
	}

	// ��Ҫ���ܸ߶ȴ�����ʾ���߶ȲŻ���ʾ������
	m_pControScrollV->SetVisible(m_nVirtualHeight > m_rc.Height());
	((CScrollV*)m_pControScrollV)->SetScrollMaxRange(m_nVirtualHeight);
}

// �������ÿؼ��ɼ��Եĺ�������Ҫ�����ӿؼ��ĺ���
void CDuiPanel::SetControlVisible(BOOL bIsVisible)
{
	__super::SetControlVisible(bIsVisible);

	// ����ÿ���ӿؼ���ԭ��Windows�ؼ��Ŀɼ���
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			if(pControlBase->IsClass(_T("div")) || pControlBase->IsClass(_T("tabctrl")))
			{
				pControlBase->SetControlVisible(bIsVisible);
			}else
			{
				// Panel�ɼ��Ա仯ʱ��,ֻ������ԭ���ؼ�,��������ʾԭ���ؼ�
				//if(!bIsVisible)
				{
					pControlBase->SetControlWndVisible(bIsVisible);
				}
			}
		}
	}

	if(m_pDuiPluginObject)
	{
		m_pDuiPluginObject->SetVisible(bIsVisible);
	}
}

// �������ÿؼ��Ƿ���õĺ�������Ҫ�����ӿؼ��ĺ���
void CDuiPanel::SetControlDisable(BOOL bIsDisable)
{
	__super::SetControlDisable(bIsDisable);

	// ����ÿ���ӿؼ���ԭ��Windows�ؼ��Ŀɼ���
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			pControlBase->SetControlDisable(bIsDisable);
		}
	}

	if(m_pDuiPluginObject)
	{
		m_pDuiPluginObject->SetDisable(bIsDisable);
	}
}

// �������ÿؼ�����״̬�ĺ���
BOOL CDuiPanel::SetControlFocus(BOOL bFocus)
{
	if(m_pDuiPluginObject)
	{
		return m_pDuiPluginObject->SetFocus(bFocus);
	}

	return __super::SetControlFocus(bFocus);
}

void CDuiPanel::DrawControl(CDC &dc, CRect rcUpdate)
{
	if(m_pDuiPluginObject)
	{
		m_pDuiPluginObject->DrawControl(dc, rcUpdate);
	}
}

// ���ӿؼ�(֧�ֹ�����ʾ)
BOOL CDuiPanel::DrawSubControls(CDC &dc, CRect rcUpdate)
{
	// �������Ҫ������,��ֱ��ʹ�ø��ؼ��Ļ�ͼ����
	if(!m_bEnableScroll || (m_nVirtualHeight < m_rc.Height()))
	{
		return __super::DrawSubControls(dc, rcUpdate);
	}

	// ������ʾλ��
	CScrollV* pScrollV = (CScrollV*)m_pControScrollV;
	int nCurPos = pScrollV->GetScrollCurrentPos();	// ��ǰtopλ��
	int nMaxRange = pScrollV->GetScrollMaxRange();
	int nVirtualTop = (nMaxRange > 0) ? nCurPos*(m_nVirtualHeight-m_rc.Height())/nMaxRange : 0;	// ��ǰ��ʾ��������ͼƬ��ʲôλ�ÿ�ʼ��ͼƬ

	// ���ݹ�����λ�û��ӿؼ�
	// ��ʼ��������ʾdc,�����Ʊ�����������ʾdc
	CBitmap	virtualBitmap;
	CDC virtualDC;
	virtualDC.CreateCompatibleDC(&dc);
	virtualBitmap.CreateCompatibleBitmap(&dc, m_rc.right-m_nScrollWidth, m_rc.top + m_nVirtualHeight);
	CBitmap* pOldVirtualBitmap = virtualDC.SelectObject(&virtualBitmap);
	virtualDC.BitBlt(m_rc.left, m_rc.top+nVirtualTop, m_rc.Width()-m_nScrollWidth, m_rc.Height(), &dc, m_rc.left, m_rc.top, SRCCOPY);	// �������Ƶ�������ʾdc

	// ������������ʾ�������
	rcUpdate.OffsetRect(0, nVirtualTop);

	// ���ӿؼ�������dc
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase && pControlBase->GetVisible() && (pControlBase != m_pControScrollV))
		{
			// �������λ�÷����˱仯,��Ҫ�ػ������ӿؼ�,��������ٶȺ���
			if(m_nVirtualTop != nVirtualTop)
			{
				pControlBase->UpdateControl(true);
			}
			pControlBase->Draw(virtualDC, rcUpdate);		
		}
	}

	m_pControScrollV->Draw(dc, rcUpdate);

	m_nVirtualTop = nVirtualTop;

	// ������ʾdc���Ƶ�dc
	dc.BitBlt(m_rc.left, m_rc.top, m_rc.Width()-m_nScrollWidth, m_rc.Height(), &virtualDC, m_rc.left, m_rc.top + nVirtualTop, SRCCOPY);

	// �ͷ�������ʾdc
	virtualDC.SelectObject(pOldVirtualBitmap);
	virtualDC.DeleteDC();
	virtualBitmap.DeleteObject();

	return TRUE;
}

// �������任
BOOL CDuiPanel::OnMousePointChange(CPoint& point)
{
	if(m_pDuiPluginObject)
	{
		return m_pDuiPluginObject->OnMousePointChange(point);
	}

	if(m_bEnableScroll && (m_nVirtualHeight > m_rc.Height()))
	{
		// ������ʾλ��
		CScrollV* pScrollV = (CScrollV*)m_pControScrollV;
		int nCurPos = pScrollV->GetScrollCurrentPos();	// ��ǰtopλ��
		int nMaxRange = pScrollV->GetScrollMaxRange();
		int nVirtualTop = (nMaxRange > 0) ? nCurPos*(m_nVirtualHeight-m_rc.Height())/nMaxRange : 0;	// ��ǰ��ʾ��������ͼƬ��ʲôλ�ÿ�ʼ��ͼƬ

		point.Offset(0, nVirtualTop);

		return TRUE;
	}

	return FALSE;
}

// �ж�����Ƿ��ڿؼ�����Ӧ������
BOOL CDuiPanel::OnCheckMouseResponse(UINT nFlags, CPoint point)
{
	if(m_pDuiPluginObject)
	{
		return m_pDuiPluginObject->OnCheckMouseResponse(nFlags, point);
	}
	return TRUE;
}

// ����ƶ��¼�����
BOOL CDuiPanel::OnControlMouseMove(UINT nFlags, CPoint point)
{
	if(m_pDuiPluginObject)
	{
		return m_pDuiPluginObject->OnMouseMove(nFlags, point);
	}
	return FALSE;
}

// �����������¼�����
BOOL CDuiPanel::OnControlLButtonDown(UINT nFlags, CPoint point)
{
	if(m_pDuiPluginObject)
	{
		return m_pDuiPluginObject->OnLButtonDown(nFlags, point);
	}
	return FALSE;
}

// ����ɿ��¼�����
BOOL CDuiPanel::OnControlLButtonUp(UINT nFlags, CPoint point)
{
	if(m_pDuiPluginObject)
	{
		return m_pDuiPluginObject->OnLButtonUp(nFlags, point);
	}
	return FALSE;
}

// �����¼�����
BOOL CDuiPanel::OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(m_pDuiPluginObject)
	{
		return m_pDuiPluginObject->OnKeyDown(nChar, nRepCnt, nFlags);
	}
	return FALSE;
}

// ��ʱ���¼�����
BOOL CDuiPanel::OnControlTimer()
{
	if(m_pDuiPluginObject)
	{
		return m_pDuiPluginObject->OnTimer(0, "");
	}
	return __super::OnControlTimer();
}

// �����¼�����
BOOL CDuiPanel::OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point)
{
	if(m_pDuiPluginObject)
	{
		m_pDuiPluginObject->OnScroll(bVertical, nFlags, point);
	}

	if(!m_bEnableScroll || (m_nVirtualHeight < m_rc.Height()))
	{
		return false;
	}

	// ���¹�����,��ˢ�½���
	CScrollV* pScroll = (CScrollV*)m_pControScrollV;
	if(pScroll->ScrollRow((nFlags == SB_LINEDOWN) ? 1 : -1))
	{
		UpdateControl(true);
		// ˢ�¹�����
		pScroll->UpdateControl(true);
	}

	return true;
}

// ��Ϣ��Ӧ
LRESULT CDuiPanel::OnBaseMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	OnMessage(uID, uMsg, wParam, lParam);
	return 0L; 
}

// �ؼ�����ˢ��
LRESULT CDuiPanel::OnControlUpdate(CRect rcUpdate, BOOL bUpdate, CControlBase *pUpdateControlBase) 
{
	if(pUpdateControlBase == NULL) return 0;

	if(bUpdate)
	{
		pUpdateControlBase->SetUpdate(FALSE);
	}

	int nVirtualTop = 0;
	if(m_nVirtualHeight > m_rc.Height())
	{
		// ������ʾλ��
		CScrollV* pScrollV = (CScrollV*)m_pControScrollV;
		int nCurPos = pScrollV->GetScrollCurrentPos();	// ��ǰtopλ��
		int nMaxRange = pScrollV->GetScrollMaxRange();
		nVirtualTop = (nMaxRange > 0) ? nCurPos*(m_nVirtualHeight-m_rc.Height())/nMaxRange : 0;	// ��ǰ��ʾ��������ͼƬ��ʲôλ�ÿ�ʼ��ͼƬ
	}

	CRect rcAllUpDate = rcUpdate;

	if(m_bInit)
	{
		BOOL bFind = false;

		for (size_t i = 0; i < m_vecControl.size(); i++)
		{
			CControlBase * pControlBase = m_vecControl.at(i);
			if (pControlBase)
			{
				if(bFind)
				{
					if (pControlBase->GetVisible() && !(pControlBase->GetRect() & rcUpdate).IsRectEmpty())
					{
						rcAllUpDate |= pControlBase->GetRect();
						pControlBase->SetUpdate(FALSE);
					}
				}
				else if(pControlBase == pUpdateControlBase)
				{
					bFind = true;
				}
			}
		}
	}

	// ���չ���λ���޸ĸ�������
	rcAllUpDate.OffsetRect(0, -nVirtualTop);
	rcAllUpDate |= m_rc;

	InvalidateRect(&rcAllUpDate);

	return 0L; 
};

// ��Ϣ����
LRESULT CDuiPanel::OnMessage(UINT uID, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// ����ÿ���ؼ������ĸ��ؼ��Ǵ�ID��������¼��Ĵ���
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase && (pControlBase->GetControlID() == uID))
		{
			return pControlBase->OnMessage(uID, Msg, wParam, lParam);
		}
	}

	if(m_pDuiPluginObject)
	{
		return m_pDuiPluginObject->OnMessage(uID, Msg, wParam, lParam);
	}

	// ����¼�δ����,����ø������Ϣ����,���ջ��͸����¼����������д���
	return __super::OnMessage(uID, Msg, wParam, lParam);
}