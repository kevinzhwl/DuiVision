#include "StdAfx.h"
#include "DuiListCtrl.h"

#define	SCROLL_V	1	// �������ؼ�ID
#define	LISTBK_AREA	2	// ����Area�ؼ�ID

CDuiGridCtrl::CDuiGridCtrl(HWND hWnd, CDuiObject* pDuiObject)
			: CDuiPanel(hWnd, pDuiObject)
{
	CRect rcBk = CRect(0,0,0,0);
	CControlBase* pControlBase = new CArea(hWnd, this, LISTBK_AREA, rcBk, 100, 100);
 	m_vecControl.push_back(pControlBase);
	m_pControBkArea = (CControlBase*)pControlBase;

	m_strFontTitle = DuiSystem::GetDefaultFont();
	m_nFontTitleWidth = 12;
	m_fontTitleStyle = FontStyleRegular;

	m_clrText = Color(225, 64, 64, 64);
	m_clrTextHover = Color(128, 0, 0);
	m_clrTextDown = Color(0, 112, 235);
	m_clrTitle = Color(255, 32, 32, 32);
	m_clrSeperator = Color(200, 160, 160, 160);
	m_clrRowHover = Color(0, 128, 128, 128);	// ����ƶ�������ʾ�ı���ɫ,Ĭ����͸��ɫ
	m_nRowHeight = 50;
	m_nHeaderHeight = 0;
	m_nLeftPos = 0;

	m_pImageSeperator = NULL;
	m_sizeSeperator = CSize(0, 0);
	m_pImageCheckBox = NULL;
	m_sizeCheckBox = CSize(0, 0);

	m_nBkTransparent = 30;

	m_bDblClk = true;

	m_nHoverRow = 0;
	m_nDownRow = -1;
	m_bEnableDownRow = FALSE;
	m_bSingleLine = TRUE;
	m_bTextWrap = FALSE;

	m_bGridTooltip = TRUE;
	m_nTipRow = -1;
	m_nTipItem = -1;
	m_nTipVirtualTop = 0;

	m_nFirstViewRow = 0;
	m_nLastViewRow = 0;
	m_nVirtualTop = 0;
}

CDuiGridCtrl::~CDuiGridCtrl(void)
{
	if(m_pImageSeperator != NULL)
	{
		delete m_pImageSeperator;
		m_pImageSeperator = NULL;
	}
	if(m_pImageCheckBox != NULL)
	{
		delete m_pImageCheckBox;
		m_pImageCheckBox = NULL;
	}
}

// ͼƬ���Ե�ʵ��
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiGridCtrl, Seperator, 1)
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiGridCtrl, CheckBox, 6)

// ����XML�ڵ㣬�����ڵ��е�������Ϣ���õ���ǰ�ؼ���������
BOOL CDuiGridCtrl::Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl)
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

	// ��Ҫ���ܸ߶ȴ�����ʾ���߶ȲŻ���ʾ������
	m_pControScrollV->SetVisible(((int)m_vecRowInfo.size() * m_nRowHeight) > (m_rc.Height() - m_nHeaderHeight));
	((CScrollV*)m_pControScrollV)->SetScrollMaxRange((int)m_vecRowInfo.size() * m_nRowHeight);

	// �����²��cloumn�ڵ���Ϣ
	for (DuiXmlNode pColumnElem = pXmlElem.child(_T("column")); pColumnElem; pColumnElem=pColumnElem.next_sibling(_T("column")))
	{
		CString strTitle = pColumnElem.attribute(_T("title")).value();
		CString strClrText = pColumnElem.attribute(_T("crtext")).value();
		CString strWidth = pColumnElem.attribute(_T("width")).value();
		CString strAlign = pColumnElem.attribute(_T("align")).value();
		CString strVAlign = pColumnElem.attribute(_T("valign")).value();
		DuiSystem::Instance()->ParseDuiString(strTitle);
		Color clrText = CDuiObject::StringToColor(strClrText);
		int nWidth = -1;
		if(!strWidth.IsEmpty())
		{
			nWidth = _wtoi(strWidth);
		}
		UINT uAlignment = 0xFFFFUL;
		if(strAlign == L"left")
		{
			uAlignment = Align_Left;
		}else
		if(strAlign == L"center")
		{
			uAlignment = Align_Center;
		}else
		if(strAlign == L"right")
		{
			uAlignment = Align_Right;
		}
		UINT uVAlignment = 0xFFFFUL;
		if(strVAlign == L"top")
		{
			uVAlignment = VAlign_Top;
		}else
		if(strVAlign == L"middle")
		{
			uVAlignment = VAlign_Middle;
		}else
		if(strVAlign == L"bottom")
		{
			uVAlignment = VAlign_Bottom;
		}
		InsertColumn(-1, strTitle, nWidth, clrText, uAlignment, uVAlignment);
	}

	// �����²��row�ڵ���Ϣ
	for (DuiXmlNode pRowElem = pXmlElem.child(_T("row")); pRowElem; pRowElem=pRowElem.next_sibling(_T("row")))
	{
		CString strId = pRowElem.attribute(_T("id")).value();
		CString strCheck = pRowElem.attribute(_T("check")).value();
		CString strImage = pRowElem.attribute(_T("image")).value();
		CString strRightImage = pRowElem.attribute(_T("right-img")).value();
		CString strClrText = pRowElem.attribute(_T("crtext")).value();

		int nCheck = -1;
		if(!strCheck.IsEmpty())
		{
			nCheck = _wtoi(strCheck);
		}

		// ���ͼƬ,ͨ��Skin��ȡ
		CString strSkin = _T("");
		if(strImage.Find(_T("skin:")) == 0)
		{
			strSkin = DuiSystem::Instance()->GetSkin(strImage);
		}else
		{
			strSkin = strImage;
		}

		int nImageIndex = -1;
		strImage = _T("");
		if(strSkin.Find(_T(".")) != -1)
		{
			// ͼƬ�ļ�
			strImage = strSkin;
		}else
		if(!strSkin.IsEmpty())
		{
			// ͼƬ����
			nImageIndex = _wtoi(strSkin);
		}

		// �ұ�ͼƬ,ͨ��Skin��ȡ
		CString strRightSkin = _T("");
		if(strRightImage.Find(_T("skin:")) == 0)
		{
			strRightSkin = DuiSystem::Instance()->GetSkin(strRightImage);
		}else
		{
			strRightSkin = strRightImage;
		}

		int nRightImageIndex = -1;
		strRightImage = _T("");
		if(strRightSkin.Find(_T(".")) != -1)
		{
			// ͼƬ�ļ�
			strRightImage = strRightSkin;
		}else
		if(!strRightSkin.IsEmpty())
		{
			// ͼƬ����
			nRightImageIndex = _wtoi(strRightSkin);
		}

		Color clrText = CDuiObject::StringToColor(strClrText);

		InsertRow(-1, strId, nImageIndex, clrText, strImage, nRightImageIndex, strRightImage, nCheck);

		int nRowIndex = m_vecRowInfo.size()-1;
		int nItemIndex = 0;
		// �����²��item�ڵ���Ϣ
		for (DuiXmlNode pItemElem = pRowElem.child(_T("item")); pItemElem; pItemElem=pItemElem.next_sibling(_T("item")))
		{
			CString strTitle = pItemElem.attribute(_T("title")).value();
			CString strContent = pItemElem.attribute(_T("content")).value();
			CString strClrText = pItemElem.attribute(_T("crtext")).value();
			CString strImage = pItemElem.attribute(_T("image")).value();
			CString strLink = pItemElem.attribute(_T("link")).value();
			CString strLinkAction = pItemElem.attribute(_T("linkaction")).value();
			CString strFontTitle = pItemElem.attribute(_T("font-title")).value();
			DuiSystem::Instance()->ParseDuiString(strTitle);
			DuiSystem::Instance()->ParseDuiString(strContent);
			DuiSystem::Instance()->ParseDuiString(strLink);
			DuiSystem::Instance()->ParseDuiString(strLinkAction);
			Color clrText = clrText = CDuiObject::StringToColor(strClrText);

			// ͼƬ,ͨ��Skin��ȡ
			CString strSkin = _T("");
			if(strImage.Find(_T("skin:")) == 0)
			{
				strSkin = DuiSystem::Instance()->GetSkin(strImage);
			}else
			{
				strSkin = strImage;
			}

			int nImageIndex = -1;
			strImage = _T("");
			if(strSkin.Find(_T(".")) != -1)
			{
				// ͼƬ�ļ�
				strImage = strSkin;
			}else
			if(!strSkin.IsEmpty())
			{
				// ͼƬ����
				nImageIndex = _wtoi(strSkin);
			}

			BOOL bUseTitleFont = (strFontTitle == _T("1"));

			if(!strLink.IsEmpty())
			{
				SetSubItemLink(nRowIndex, nItemIndex, strLink, strLinkAction, nImageIndex, clrText, strImage);
			}else
			{
				SetSubItem(nRowIndex, nItemIndex, strTitle, strContent, bUseTitleFont, nImageIndex, clrText, strImage);
			}

			// �����²�Ŀؼ��ڵ���Ϣ
			GridItemInfo* pItemInfo = GetItemInfo(nRowIndex, nItemIndex);
			for (DuiXmlNode pControlElem = pItemElem.first_child(); pControlElem; pControlElem=pControlElem.next_sibling())
			{
				if((pControlElem != NULL) && (pItemInfo != NULL))
				{
					CString strControlName = pControlElem.name();
					CControlBase* pControl = _CreateControlByName(strControlName);
					if(pControl)
					{
						pControl->Load(pControlElem);
						pControl->SetVisible(FALSE);
						// ���ؼ�ָ����ӵ�gridctrl�ؼ����ӿؼ��б���
						m_vecControl.push_back(pControl);
						// ���ؼ�ָ����ӵ���Ԫ��Ŀؼ��б���(�����ڰ��յ�Ԫ������ӿؼ�)
						pItemInfo->vecControl.push_back(pControl);
					}
				}
			}

			nItemIndex++;
		}
	}

    return TRUE;
}

// �����
BOOL CDuiGridCtrl::InsertColumn(int nColumn, CString strTitle, int nWidth, Color clrText, UINT uAlignment, UINT uVAlignment)
{
	GridColumnInfo columnInfo;
	columnInfo.strTitle = strTitle;
	columnInfo.clrText = clrText;
	columnInfo.nWidth = nWidth;
	columnInfo.uAlignment = uAlignment;
	columnInfo.uVAlignment = uVAlignment;
	if(nColumn <= -1 || nColumn >= (int)m_vecColumnInfo.size())
	{
		m_vecColumnInfo.push_back(columnInfo);
	}
	else
	{
		m_vecColumnInfo.insert(m_vecColumnInfo.begin() + nColumn, columnInfo);
	}

	int nXPos = 0;
	int nYPos = 0;

	for(size_t i = 0; i < m_vecColumnInfo.size(); i++)
	{
		GridColumnInfo &columnInfoTemp = m_vecColumnInfo.at(i);
		int nWidth = columnInfoTemp.nWidth;
		if(nWidth == -1)
		{
			nWidth = m_rc.Width() - nXPos;
		}
		columnInfoTemp.rcHeader.SetRect(nXPos, nYPos, nXPos + nWidth, nYPos + m_nRowHeight);
		nXPos += columnInfoTemp.nWidth;
	}

	UpdateControl(true);
	return true;
}

// �����
int CDuiGridCtrl::InsertRow(int nRow, CString strId, int nImageIndex, Color clrText, CString strImage,
							 int nRightImageIndex, CString strRightImage, int nCheck)
{
	GridRowInfo rowInfo;
	rowInfo.strId = strId;
	rowInfo.nCheck = nCheck;
	rowInfo.nImageIndex = nImageIndex;
	rowInfo.sizeImage.SetSize(0, 0);
	rowInfo.nRightImageIndex = nRightImageIndex;
	rowInfo.sizeRightImage.SetSize(0, 0);
	rowInfo.bRowColor = FALSE;
	rowInfo.clrText = clrText;
	rowInfo.nHoverItem = -1;
	if(clrText.GetValue() != Color(0, 0, 0, 0).GetValue())
	{
		rowInfo.bRowColor = TRUE;
	}

	// ���ͼƬ
	if(!strImage.IsEmpty())
	{
		// ʹ��������ָ����ͼƬ
		if(DuiSystem::Instance()->LoadImageFile(strImage, m_bImageUseECM, rowInfo.pImage))
		{
			rowInfo.sizeImage.SetSize(rowInfo.pImage->GetWidth() / 1, rowInfo.pImage->GetHeight());
		}
	}else
	{
		// ʹ������ͼƬ
		rowInfo.pImage = NULL;
		if((rowInfo.nImageIndex != -1) && (m_pImage != NULL) && (m_pImage->GetLastStatus() == Ok))
		{
			rowInfo.sizeImage.SetSize(m_sizeImage.cx, m_sizeImage.cy);
		}
	}

	// �ұ�ͼƬ
	if(!strRightImage.IsEmpty())
	{
		// ʹ��������ָ����ͼƬ
		if(DuiSystem::Instance()->LoadImageFile(strRightImage, m_bImageUseECM, rowInfo.pRightImage))
		{
			rowInfo.sizeRightImage.SetSize(rowInfo.pRightImage->GetWidth() / 1, rowInfo.pRightImage->GetHeight());
		}
	}else
	{
		// ʹ������ͼƬ
		rowInfo.pRightImage = NULL;
		if((rowInfo.nRightImageIndex != -1) && (m_pImage != NULL) && (m_pImage->GetLastStatus() == Ok))
		{
			rowInfo.sizeRightImage.SetSize(m_sizeImage.cx, m_sizeImage.cy);
		}
	}

	return InsertRow(nRow, rowInfo);
}

// �����
int CDuiGridCtrl::InsertRow(int nRow, GridRowInfo &rowInfo)
{
	int nRetRow = -1;
	if(nRow <= -1 || nRow >= (int)m_vecRowInfo.size())
	{
		m_vecRowInfo.push_back(rowInfo);
		nRetRow = m_vecRowInfo.size()-1;
	}
	else
	{
		m_vecRowInfo.insert(m_vecRowInfo.begin() + nRow, rowInfo);
		nRetRow = nRow;
	}

	// �������б���е�λ��
	CalcRowsPos();	

	UpdateControl(true);
	return nRetRow;
}

// ���ñ��������(���ֱ����)
BOOL CDuiGridCtrl::SetSubItem(int nRow, int nItem, CString strTitle, CString strContent, BOOL bUseTitleFont, int nImageIndex, Color clrText, CString strImage)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return FALSE;
	}
	if((nItem < 0) || (nItem >= (int)m_vecColumnInfo.size()))
	{
		return FALSE;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	for(int i = rowInfo.vecItemInfo.size(); i <= nItem; i++)
	{
		GridItemInfo itemInfo;
		itemInfo.strTitle = _T("");
		itemInfo.strContent = _T("");
		itemInfo.nImageIndex = -1;
		itemInfo.pImage = NULL;
		itemInfo.sizeImage = CSize(0,0);
		itemInfo.clrText = Color(0, 0, 0, 0);
		itemInfo.strLink = _T("");
		itemInfo.strLinkAction = _T("");
		itemInfo.bNeedTitleTip = FALSE;
		itemInfo.bNeedContentTip = FALSE;
		itemInfo.bUseTitleFont = bUseTitleFont;
		GridColumnInfo &columnInfo = m_vecColumnInfo.at(i);
		itemInfo.rcItem.SetRect(columnInfo.rcHeader.left, rowInfo.rcRow.top,
				columnInfo.rcHeader.right, rowInfo.rcRow.bottom);
		rowInfo.vecItemInfo.push_back(itemInfo);
	}

	GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(nItem);
	itemInfo.strTitle = strTitle;
	itemInfo.strContent = strContent;
	itemInfo.nImageIndex = nImageIndex;
	itemInfo.sizeImage = CSize(0,0);
	itemInfo.clrText = clrText;

	GridColumnInfo &columnInfo = m_vecColumnInfo.at(nItem);
	itemInfo.rcItem.SetRect(columnInfo.rcHeader.left, rowInfo.rcRow.top,
			columnInfo.rcHeader.right, rowInfo.rcRow.bottom);

	// ͼƬ
	if(!strImage.IsEmpty())
	{
		// ʹ��������ָ����ͼƬ
		if(DuiSystem::Instance()->LoadImageFile(strImage, m_bImageUseECM, itemInfo.pImage))
		{
			itemInfo.sizeImage.SetSize(itemInfo.pImage->GetWidth() / 1, itemInfo.pImage->GetHeight());
		}
	}else
	{
		// ʹ������ͼƬ
		itemInfo.pImage = NULL;
		if((itemInfo.nImageIndex != -1) && (m_pImage != NULL) && (m_pImage->GetLastStatus() == Ok))
		{
			itemInfo.sizeImage.SetSize(m_sizeImage.cx, m_sizeImage.cy);
		}
	}

	return TRUE;
}

// ���ñ��������(���ӱ����)
BOOL CDuiGridCtrl::SetSubItemLink(int nRow, int nItem, CString strLink, CString strLinkAction, int nImageIndex, Color clrText, CString strImage)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return FALSE;
	}
	if((nItem < 0) || (nItem >= (int)m_vecColumnInfo.size()))
	{
		return FALSE;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	for(int i = rowInfo.vecItemInfo.size(); i <= nItem; i++)
	{
		GridItemInfo itemInfo;
		itemInfo.strTitle = _T("");
		itemInfo.strContent = _T("");
		itemInfo.nImageIndex = -1;
		itemInfo.pImage = NULL;
		itemInfo.sizeImage = CSize(0,0);
		itemInfo.clrText = Color(0, 0, 0, 0);
		itemInfo.strLink = _T("");
		itemInfo.strLinkAction = _T("");
		itemInfo.bNeedTitleTip = FALSE;
		itemInfo.bNeedContentTip = FALSE;
		itemInfo.bUseTitleFont = FALSE;
		GridColumnInfo &columnInfo = m_vecColumnInfo.at(i);
		itemInfo.rcItem.SetRect(columnInfo.rcHeader.left, rowInfo.rcRow.top,
				columnInfo.rcHeader.right, rowInfo.rcRow.bottom);
		rowInfo.vecItemInfo.push_back(itemInfo);
	}

	GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(nItem);
	itemInfo.strLink = strLink;
	itemInfo.strLinkAction = strLinkAction;
	itemInfo.nImageIndex = nImageIndex;
	itemInfo.sizeImage = CSize(0,0);
	itemInfo.clrText = clrText;

	GridColumnInfo &columnInfo = m_vecColumnInfo.at(nItem);
	itemInfo.rcItem.SetRect(columnInfo.rcHeader.left, rowInfo.rcRow.top,
			columnInfo.rcHeader.right, rowInfo.rcRow.bottom);

	// ͼƬ
	if(!strImage.IsEmpty())
	{
		// ʹ��������ָ����ͼƬ
		if(DuiSystem::Instance()->LoadImageFile(strImage, m_bImageUseECM, itemInfo.pImage))
		{
			itemInfo.sizeImage.SetSize(itemInfo.pImage->GetWidth() / 1, itemInfo.pImage->GetHeight());
		}
	}else
	{
		// ʹ������ͼƬ
		itemInfo.pImage = NULL;
		if((itemInfo.nImageIndex != -1) && (m_pImage != NULL) && (m_pImage->GetLastStatus() == Ok))
		{
			itemInfo.sizeImage.SetSize(m_sizeImage.cx, m_sizeImage.cy);
		}
	}

	return TRUE;
}

// �����������ӿؼ�
BOOL CDuiGridCtrl::AddSubItemControl(int nRow, int nItem, CControlBase* pControl)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return FALSE;
	}
	if((nItem < 0) || (nItem >= (int)m_vecColumnInfo.size()))
	{
		return FALSE;
	}

	GridItemInfo* pItemInfo = GetItemInfo(nRow, nItem);
	if(pItemInfo == NULL)
	{
		SetSubItem(nRow, nItem, L"");
		pItemInfo = GetItemInfo(nRow, nItem);
	}
	if(pItemInfo == NULL)
	{
		return FALSE;
	}

	if(pControl)
	{
		pControl->SetParent(this);
		pControl->SetVisible(FALSE);
		// ���ؼ�ָ����ӵ�gridctrl�ؼ����ӿؼ��б���
		m_vecControl.push_back(pControl);
		// ���ؼ�ָ����ӵ���Ԫ��Ŀؼ��б���(�����ڰ��յ�Ԫ������ӿؼ�)
		pItemInfo->vecControl.push_back(pControl);
		UpdateControl(true);
	}

	return TRUE;
}

// ɾ��������ָ���ӿؼ�(ͨ���ؼ�����ָ��ɾ��)
BOOL CDuiGridCtrl::DeleteSubItemControl(CControlBase* pControl)
{
	// �������е�Ԫ��,ɾ����Ӧ�Ŀؼ���������
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		GridRowInfo &rowInfo = m_vecRowInfo.at(i);
		for(size_t j = 0; j < rowInfo.vecItemInfo.size(); j++)
		{
			GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(j);
			vector<CControlBase*>::iterator it;
			for(it=itemInfo.vecControl.begin(); it!=itemInfo.vecControl.end(); ++it)
			{
				CControlBase* _pControl = *it;
				if(_pControl == pControl)
				{
					itemInfo.vecControl.erase(it);
					break;
				}
			}
		}
	}

	// ɾ���ӿؼ��ж�Ӧ�Ŀؼ�����
	RemoveControl(pControl);

	return TRUE;
}

// ɾ��������ָ���ӿؼ�(ͨ���ؼ����Ϳؼ�IDɾ��)
BOOL CDuiGridCtrl::DeleteSubItemControl(CString strControlName, UINT uControlID)
{
	// �������е�Ԫ��,ɾ����Ӧ�Ŀؼ���������
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		GridRowInfo &rowInfo = m_vecRowInfo.at(i);
		for(size_t j = 0; j < rowInfo.vecItemInfo.size(); j++)
		{
			GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(j);
			vector<CControlBase*>::iterator it;
			for(it=itemInfo.vecControl.begin(); it!=itemInfo.vecControl.end(); ++it)
			{
				CControlBase* _pControl = *it;
				if(_pControl && _pControl->IsThisObject(uControlID, strControlName))
				{
					itemInfo.vecControl.erase(it);
					break;
				}
			}
		}
	}

	// ɾ���ӿؼ��ж�Ӧ�Ŀؼ�����
	RemoveControl(strControlName, uControlID);

	return TRUE;
}

// ɾ����
BOOL CDuiGridCtrl::DeleteRow(int nRow)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return FALSE;
	}

	// �����е����е�Ԫ��,ɾ����Ӧ���ӿؼ�����
	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	for(size_t j = 0; j < rowInfo.vecItemInfo.size(); j++)
	{
		GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(j);
		vector<CControlBase*>::iterator it;
		for(it=itemInfo.vecControl.begin(); it!=itemInfo.vecControl.end(); ++it)
		{
			CControlBase* _pControl = *it;
			// ɾ����Ԫ��������ӿؼ�
			RemoveControl(_pControl);
		}
	}

	// ɾ������Ϣ
	int nIndex = 0;
	vector<GridRowInfo>::iterator it;
	for(it=m_vecRowInfo.begin();it!=m_vecRowInfo.end();++it)
	{
		if(nIndex == nRow)
		{
			m_vecRowInfo.erase(it);
			break;
		}
		nIndex++;
	}

	// �������б���е�λ��
	CalcRowsPos();

	UpdateControl(true);
	return true;
}

// ��������λ��
void CDuiGridCtrl::CalcRowsPos()
{
	int nXPos = 0;//m_rc.left;
	int nYPos = 0;//m_rc.top;

	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		GridRowInfo &rowInfoTemp = m_vecRowInfo.at(i);
		int nItemWidth = m_rc.Width() - m_nScrollWidth;
		rowInfoTemp.rcRow.SetRect(nXPos, nYPos, nXPos + nItemWidth, nYPos + m_nRowHeight);

		rowInfoTemp.rcCheck.SetRect(0,0,0,0);

		nYPos += m_nRowHeight;
	}

	// ��Ҫ���ܸ߶ȴ�����ʾ���߶ȲŻ���ʾ������
	m_pControScrollV->SetVisible(((int)m_vecRowInfo.size() * m_nRowHeight) > (m_rc.Height() - m_nHeaderHeight));
	((CScrollV*)m_pControScrollV)->SetScrollMaxRange(m_vecRowInfo.size() * m_nRowHeight);
}

// ��ȡĳһ������Ϣ
GridRowInfo* CDuiGridCtrl::GetRowInfo(int nRow)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return NULL;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	return &rowInfo;
}

// ��ȡĳһ����Ԫ����Ϣ
GridItemInfo* CDuiGridCtrl::GetItemInfo(int nRow, int nItem)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return NULL;
	}

	if((nItem < 0) || (nItem >= (int)m_vecColumnInfo.size()))
	{
		return NULL;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	if(nItem >= (int)rowInfo.vecItemInfo.size())
	{
		return NULL;
	}

	GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(nItem);

	return &itemInfo;
}

// ����ĳһ���е���ɫ
void CDuiGridCtrl::SetRowColor(int nRow, Color clrText)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	rowInfo.bRowColor = TRUE;
	rowInfo.clrText = clrText;
}

// ����ĳһ���еļ���״̬
void CDuiGridCtrl::SetRowCheck(int nRow, int nCheck)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	rowInfo.nCheck = nCheck;
}

// ��ȡĳһ�еļ���״̬
int CDuiGridCtrl::GetRowCheck(int nRow)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return -1;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	return rowInfo.nCheck;
}

// ����б�
void CDuiGridCtrl::ClearItems()
{
	// ɾ�������ӿؼ�
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		GridRowInfo &rowInfo = m_vecRowInfo.at(i);
		for(size_t j = 0; j < rowInfo.vecItemInfo.size(); j++)
		{
			GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(j);
			vector<CControlBase*>::iterator it;
			for(it=itemInfo.vecControl.begin(); it!=itemInfo.vecControl.end(); ++it)
			{
				CControlBase* pControl = *it;
				RemoveControl(pControl);
			}
		}
	}

	m_vecRowInfo.clear();
	m_pControScrollV->SetVisible(FALSE);
	UpdateControl(true);
}

// ��XML����Font-title����
HRESULT CDuiGridCtrl::OnAttributeFontTitle(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	DuiFontInfo fontInfo;
	BOOL bFindFont = DuiSystem::Instance()->GetFont(strValue, fontInfo);
	if (!bFindFont) return E_FAIL;

	m_strFontTitle = fontInfo.strFont;
	m_nFontTitleWidth = fontInfo.nFontWidth;	
	m_fontTitleStyle = fontInfo.fontStyle;

	return bLoading?S_FALSE:S_OK;
}

void CDuiGridCtrl::SetControlRect(CRect rc)
{
	m_rc = rc;
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
				rcTemp.top += m_nHeaderHeight;
				rcTemp.left = rcTemp.right - m_nScrollWidth;
			}else
			if(LISTBK_AREA == uControlID)
			{
				rcTemp = m_rc;
				rcTemp.top += m_nHeaderHeight;
				rcTemp.right -= m_nScrollWidth;
			}else
			{
				continue;
			}
			pControlBase->SetRect(rcTemp);			
		}
	}

	// ���¼��������е�λ��
	int nXPos = 0;
	int nYPos = 0;
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		GridRowInfo &rowInfoTemp = m_vecRowInfo.at(i);
		int nItemWidth = m_rc.Width() - m_nScrollWidth;
		rowInfoTemp.rcRow.SetRect(nXPos, nYPos, nXPos + nItemWidth, nYPos + m_nRowHeight);

		nYPos += m_nRowHeight;
	}

	// ��Ҫ���ܸ߶ȴ�����ʾ���߶ȲŻ���ʾ������
	m_pControScrollV->SetVisible(((int)m_vecRowInfo.size() * m_nRowHeight) > (m_rc.Height() - m_nHeaderHeight));
	((CScrollV*)m_pControScrollV)->SetScrollMaxRange((int)m_vecRowInfo.size() * m_nRowHeight);
}

// �ж�ָ��������λ���Ƿ���ĳһ����
BOOL CDuiGridCtrl::PtInRow(CPoint point, GridRowInfo& rowInfo)
{
	CRect rc = rowInfo.rcRow;
	// rcRow�����ǲ����нڵ�ʱ�������İ��տؼ�������ʾ����Ϊ���յ�����,��Ҫת��Ϊ�������
	rc.OffsetRect(m_rc.left, m_rc.top+m_nHeaderHeight-m_nVirtualTop);
	return rc.PtInRect(point);
}

// �ж�ָ��������λ���Ƿ���ĳһ�еļ�����
BOOL CDuiGridCtrl::PtInRowCheck(CPoint point, GridRowInfo& rowInfo)
{
	CRect rc = rowInfo.rcCheck;
	// rcCheck�����ǻ�ͼʱ�������İ��տؼ�������ʾ����Ϊ���յ�����,��Ҫת��Ϊ�������
	rc.OffsetRect(m_rc.left, m_rc.top+m_nHeaderHeight-m_nVirtualTop);
	return rc.PtInRect(point);
}

// �ж�ָ��������λ���Ƿ���ĳһ�е�ĳ������,����������
int CDuiGridCtrl::PtInRowItem(CPoint point, GridRowInfo& rowInfo)
{
	for(size_t i = 0; i < rowInfo.vecItemInfo.size(); i++)
	{
		GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(i);
		CRect rc = itemInfo.rcItem;
		// rcItem�����ǻ�ͼʱ�������İ��տؼ�������ʾ����Ϊ���յ�����,��Ҫת��Ϊ�������
		rc.OffsetRect(m_rc.left + ((i == 0) ? m_nLeftPos : 0), m_rc.top+m_nHeaderHeight-m_nVirtualTop);
		if(i == 0)
		{
			rc.right -= m_nLeftPos;
		}
		if(rc.PtInRect(point))
		{
			return i;
		}
	}

	return -1;
}

// ���õ�Ԫ���Tooltip
void CDuiGridCtrl::SetGridTooltip(int nRow, int nItem, CString strTooltip)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return;
	}

	CDlgBase* pDlg = GetParentDialog();
	if(pDlg && ((m_nTipRow != nRow) || (m_nTipItem != nItem) || (m_nTipVirtualTop != m_nVirtualTop)))
	{
		GridItemInfo* pGridInfo = GetItemInfo(nRow, nItem);
		if(pGridInfo && (pGridInfo->bNeedTitleTip || pGridInfo->bNeedContentTip))
		{
			CRect rc = pGridInfo->rcItem;
			rc.OffsetRect(m_rc.left, m_rc.top-m_nVirtualTop+m_nHeaderHeight);
			pDlg->SetTooltip(this, strTooltip, rc, TRUE);
		}else
		{
			pDlg->ClearTooltip();
		}
		m_nTipRow = nRow;
		m_nTipItem = nItem;
		m_nTipVirtualTop = m_nVirtualTop;
	}
}

// ���Tooltip
void CDuiGridCtrl::ClearGridTooltip()
{
	CDlgBase* pDlg = GetParentDialog();
	if(pDlg)
	{
		pDlg->ClearTooltip();
		m_nTipRow = -1;
		m_nTipItem = -1;
		m_nTipVirtualTop = 0;
	}
}

// ����ƶ��¼�����
BOOL CDuiGridCtrl::OnControlMouseMove(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	int nOldHoverRow = m_nHoverRow;
	BOOL bHoverItemChange = FALSE;
	int nOldHoverItem = -1;

	if(m_rc.PtInRect(point))
	{
		if((m_nHoverRow >= 0) && (m_nHoverRow < (int)m_vecRowInfo.size()))
		{
			GridRowInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
			nOldHoverItem = rowInfo.nHoverItem;
			if(PtInRow(point, rowInfo))
			{
				rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
				if(nOldHoverItem != rowInfo.nHoverItem)
				{
					bHoverItemChange = TRUE;
					UpdateControl(TRUE);
				}

				if(m_bGridTooltip)	// ���õ�Ԫ��Tooltip
				{
					GridItemInfo* pGridInfo = GetItemInfo(m_nHoverRow, rowInfo.nHoverItem);
					if(pGridInfo && pGridInfo->bNeedTitleTip)
					{
						SetGridTooltip(m_nHoverRow, rowInfo.nHoverItem, pGridInfo->strTitle);
					}else
					if(pGridInfo && pGridInfo->bNeedContentTip)
					{
						SetGridTooltip(m_nHoverRow, rowInfo.nHoverItem, pGridInfo->strContent);
					}else
					{
						ClearGridTooltip();
					}
				}

				return false;
			}
			m_nHoverRow = -1;		
		}

		BOOL bMousenDown = false;
		if((m_nDownRow >= 0) && (m_nDownRow < (int)m_vecRowInfo.size()))
		{
			GridRowInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
			nOldHoverItem = rowInfo.nHoverItem;
			if(PtInRow(point, rowInfo))
			{
				rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
				bMousenDown = true;
				m_nHoverRow = -1;
				if(nOldHoverItem != rowInfo.nHoverItem)
				{
					bHoverItemChange = TRUE;
				}

				if(m_bGridTooltip)	// ���õ�Ԫ��Tooltip
				{
					GridItemInfo* pGridInfo = GetItemInfo(m_nDownRow, rowInfo.nHoverItem);
					if(pGridInfo && pGridInfo->bNeedTitleTip)
					{
						SetGridTooltip(m_nDownRow, rowInfo.nHoverItem, pGridInfo->strTitle);
					}else
					if(pGridInfo && pGridInfo->bNeedContentTip)
					{
						SetGridTooltip(m_nDownRow, rowInfo.nHoverItem, pGridInfo->strContent);
					}else
					{
						ClearGridTooltip();
					}
				}
			}		
		}

		if(!bMousenDown)
		{
			for(size_t i = 0; i < m_vecRowInfo.size(); i++)
			{
				GridRowInfo &rowInfo = m_vecRowInfo.at(i);
				nOldHoverItem = rowInfo.nHoverItem;
				if(PtInRow(point, rowInfo))
				{
					rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
					m_nHoverRow = i;
					if(nOldHoverItem != rowInfo.nHoverItem)
					{
						bHoverItemChange = TRUE;
					}
					break;
				}
			}
		}
	}
	else
	{
		m_nHoverRow = -1;
	}

	if((nOldHoverRow != m_nHoverRow) || bHoverItemChange)
	{
		UpdateControl(TRUE);
		return true;
	}
	
	return false;
}

// �����������¼�����
BOOL CDuiGridCtrl::OnControlLButtonDown(UINT nFlags, CPoint point)
{
	//AfxMessageBox(_T("CDuiGridCtrl::OnControlLButtonDown"));
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	// ���ô��ڽ���,��������޷����й����¼��Ĵ���
	SetWindowFocus();

	if((m_nHoverRow >= 0) && (m_nHoverRow < (int)m_vecRowInfo.size()))
	{
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
		if(PtInRow(point, rowInfo) && !PtInRowCheck(point, rowInfo))	// �����¼�ֻ�����ſ�ʱ�򴥷�
		{
			rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
			if(m_nDownRow != m_nHoverRow)
			{
				if(m_bEnableDownRow)
				{
					m_nDownRow = m_nHoverRow;
					m_nHoverRow = -1;
				}

				SendMessage(MSG_BUTTON_DOWN, m_bEnableDownRow ? m_nDownRow : m_nHoverRow, rowInfo.nHoverItem);

				UpdateControl(TRUE);

				return true;
			}
		}	
	}else
	if((m_nDownRow >= 0) && (m_nDownRow < (int)m_vecRowInfo.size()))
	{
		// �������Ļ���֮ǰ������У�Ҳͬ���ᷢ��������¼�
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
		if(PtInRow(point, rowInfo)&& !PtInRowCheck(point, rowInfo))	// �����¼�ֻ�����ſ�ʱ�򴥷�
		{
			rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
			SendMessage(MSG_BUTTON_DOWN, m_nDownRow, rowInfo.nHoverItem);
			return true;
		}
	}
	
	return false;
}

// �������ſ��¼�����
BOOL CDuiGridCtrl::OnControlLButtonUp(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	if((m_nHoverRow >= 0) && (m_nHoverRow < (int)m_vecRowInfo.size()))
	{
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
		if(PtInRow(point, rowInfo))
		{
			if(PtInRowCheck(point, rowInfo))	// ����״̬�ı�
			{
				rowInfo.nCheck = ((rowInfo.nCheck == 1) ? 0 : 1);
				SendMessage(MSG_BUTTON_UP, m_nHoverRow, rowInfo.nCheck);
				UpdateControl(TRUE);

				return true;
			}
		}	
	}else
	if((m_nDownRow >= 0) && (m_nDownRow < (int)m_vecRowInfo.size()))
	{
		// �������Ļ���֮ǰ������У�Ҳͬ���ᷢ��������¼�
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
		if(PtInRow(point, rowInfo))
		{
			if(PtInRowCheck(point, rowInfo))	// ����״̬�ı�
			{
				rowInfo.nCheck = ((rowInfo.nCheck == 1) ? 0 : 1);
				SendMessage(MSG_BUTTON_UP, m_nDownRow, rowInfo.nCheck);
				UpdateControl(TRUE);

				return true;
			}
		}
	}

	return false;
}

// ������˫���¼�����
BOOL CDuiGridCtrl::OnControlLButtonDblClk(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	if(m_rc.PtInRect(point))
	{
		for(size_t i = 0; i < m_vecRowInfo.size(); i++)
		{
			GridRowInfo &rowInfo = m_vecRowInfo.at(i);
			if(PtInRow(point, rowInfo) && !PtInRowCheck(point, rowInfo))
			{
				int nClickItem = PtInRowItem(point, rowInfo);
				SendMessage(MSG_BUTTON_DBLCLK, i, nClickItem);
				return true;
			}
		}
	}

	return false;
}

// �����¼�����
BOOL CDuiGridCtrl::OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point)
{
	if(((int)m_vecRowInfo.size() * m_nRowHeight) <= m_rc.Height())
	{
		return false;
	}

	// ���¹�����,��ˢ�½���
	CScrollV* pScroll = (CScrollV*)m_pControScrollV;
	if(pScroll->ScrollRow((nFlags == SB_LINEDOWN) ? 1 : -1))
	{
		UpdateControl(true);
	}

	return true;
}

//Rex added
BOOL CDuiGridCtrl::OnControlRButtonDown(UINT nFlags, CPoint point)
{
	//AfxMessageBox(_T("CDuiGridCtrl::OnControlLButtonDown"));
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	// ���ô��ڽ���,��������޷����й����¼��Ĵ���
	SetWindowFocus();

	if((m_nHoverRow >= 0) && (m_nHoverRow < (int)m_vecRowInfo.size()))
	{
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
		if(PtInRow(point, rowInfo) && !PtInRowCheck(point, rowInfo))	// �����¼�ֻ�����ſ�ʱ�򴥷�
		{
			rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
			if(m_nDownRow != m_nHoverRow)
			{
				if(m_bEnableDownRow)
				{
					m_nDownRow = m_nHoverRow;
					m_nHoverRow = -1;
				}

				SendMessage(MSG_RBUTTON_DOWN, m_bEnableDownRow ? m_nDownRow : m_nHoverRow, rowInfo.nHoverItem);

				UpdateControl(TRUE);

				return true;
			}
		}	
	} else if((m_nDownRow >= 0) && (m_nDownRow < (int)m_vecRowInfo.size())) {
		// �������Ļ���֮ǰ������У�Ҳͬ���ᷢ��������¼�
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
		if(PtInRow(point, rowInfo)&& !PtInRowCheck(point, rowInfo))	// �����¼�ֻ�����ſ�ʱ�򴥷�
		{
			rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
			SendMessage(MSG_RBUTTON_DOWN, m_nDownRow, rowInfo.nHoverItem);
			return true;
		}
	}
	
	return false;
}

// ����Ҽ��ſ��¼�����
BOOL CDuiGridCtrl::OnControlRButtonUp(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	if((m_nHoverRow >= 0) && (m_nHoverRow < (int)m_vecRowInfo.size()))
	{
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
		if(PtInRow(point, rowInfo))
		{
			//ȡ����������Ϣ���ܷ���
			if(!PtInRowCheck(point, rowInfo))	// ����״̬�ı�
			{
				rowInfo.nCheck = ((rowInfo.nCheck == 1) ? 0 : 1);
				SendMessage(MSG_RBUTTON_UP, m_nHoverRow, rowInfo.nCheck);
				UpdateControl(TRUE);

				return true;
			}
		}	
	}else
	if((m_nDownRow >= 0) && (m_nDownRow < (int)m_vecRowInfo.size()))
	{
		// �������Ļ���֮ǰ������У�Ҳͬ���ᷢ��������¼�
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
		if(PtInRow(point, rowInfo))
		{
			if(PtInRowCheck(point, rowInfo))	// ����״̬�ı�
			{
				rowInfo.nCheck = ((rowInfo.nCheck == 1) ? 0 : 1);
				SendMessage(MSG_RBUTTON_UP, m_nDownRow, rowInfo.nCheck);
				UpdateControl(TRUE);

				return true;
			}
		}
	}

	return false;
}
//End

// ��Ϣ��Ӧ
LRESULT CDuiGridCtrl::OnMessage(UINT uID, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if((uID == SCROLL_V) && (Msg == MSG_SCROLL_CHANGE))
	{
		// ����ǹ�������λ�ñ���¼�,��ˢ����ʾ
		UpdateControl(true);
	}else
	if((uID == GetID()) && (Msg == MSG_BUTTON_DOWN) && (lParam != -1))
	{
		// ������е�ĳ������
		GridRowInfo* pRowInfo = GetRowInfo(wParam);
		if(pRowInfo && (lParam >= 0) && (lParam < (int)pRowInfo->vecItemInfo.size()))
		{
			GridItemInfo &itemInfo = pRowInfo->vecItemInfo.at(lParam);
			// ת��ΪMSG_BUTTON_UP��Ϣ,��ΪDuiSystem������ʱ��ֻ����MSG_BUTTON_UP��Ϣ
			if(!itemInfo.strLinkAction.IsEmpty())
			{
				DuiSystem::AddDuiActionTask(uID, MSG_BUTTON_UP, wParam, lParam, GetName(), itemInfo.strLinkAction, GetParent());
			}
		}
	}

	return __super::OnMessage(uID, Msg, wParam, lParam); 
}

void CDuiGridCtrl::DrawControl(CDC &dc, CRect rcUpdate)
{
	// �б�ͼ����:
	// 1.�б������߶�Ϊÿһ�и߶�*����
	// 2.�б���ʾ��top������scroll�ؼ���¼
	// 3.�ػ�ʱ��,����top����λ�ü������ʾ�ĵ�һ�е����,������ʾ�߶ȼ������ʾ�����һ�е����
	// 4.���ݼ��������ʾ����,����Ӧ�����ݵ��ڴ�dc��
	// 5.�������ʾ��top��������ڴ�dc�Ŀ���
	int nWidth = m_rc.Width() - m_nScrollWidth;	// ��ȥ�������Ŀ��
	int nHeightAll = m_vecRowInfo.size()*m_nRowHeight; // �ܵ�����߶� //m_rc.Height();
	CScrollV* pScrollV = (CScrollV*)m_pControScrollV;
	int nCurPos = pScrollV->GetScrollCurrentPos();	// ��ǰtopλ��
	int nMaxRange = pScrollV->GetScrollMaxRange();

	m_nVirtualTop = (nMaxRange > 0) ? nCurPos*(nHeightAll-m_rc.Height())/nMaxRange : 0;	// ��ǰ������λ�ö�Ӧ�������topλ��
	if(m_nVirtualTop < 0)
	{
		m_nVirtualTop = 0;
		pScrollV->SetScrollCurrentPos(0);
	}
	m_nFirstViewRow = m_nVirtualTop / m_nRowHeight;					// ��ʾ�ĵ�һ�����
	m_nLastViewRow = (m_nVirtualTop + m_rc.Height() - m_nHeaderHeight) / m_nRowHeight;	// ��ʾ�����һ�����
	if(m_nLastViewRow >= (int)m_vecRowInfo.size())
	{
		m_nLastViewRow = m_vecRowInfo.size() - 1;
	}
	if(m_nLastViewRow < 0)
	{
		m_nLastViewRow = 0;
	}
	int nHeightView = (m_nLastViewRow - m_nFirstViewRow +1) * m_nRowHeight;	// ��ʾ�漰��������߶�
	int nYViewPos = m_nVirtualTop - (m_nFirstViewRow * m_nRowHeight);		// �ڴ�dc��ʾ����Ļʱ���topλ��
	if(nYViewPos < 0)
	{
		nYViewPos = 0;
	}

	if(!m_bUpdate)
	{
		UpdateMemDC(dc, nWidth, nHeightView);

		Graphics graphics(m_memDC);
		
		m_memDC.BitBlt(0, 0, nWidth, nHeightView, &dc, m_rc.left, m_rc.top, WHITENESS);	// ����ɫ����
		DrawVerticalTransition(m_memDC, dc, CRect(0, nYViewPos, nWidth, m_rc.Height()+nYViewPos-m_nHeaderHeight),	// ����͸����
				m_rc, m_nBkTransparent, m_nBkTransparent);
		
		BSTR bsFontTitle = m_strFontTitle.AllocSysString();
		FontFamily fontFamilyTitle(bsFontTitle);
		Font fontTitle(&fontFamilyTitle, (REAL)m_nFontTitleWidth, m_fontTitleStyle, UnitPixel);
		::SysFreeString(bsFontTitle);

		BSTR bsFont = m_strFont.AllocSysString();
		FontFamily fontFamily(bsFont);
		Font font(&fontFamily, (REAL)m_nFontWidth, m_fontStyle, UnitPixel);
		::SysFreeString(bsFont);

		SolidBrush solidBrush(m_clrText);			// �������ֻ�ˢ
		SolidBrush solidBrushH(m_clrTextHover);		// �ȵ����ֻ�ˢ
		SolidBrush solidBrushD(m_clrTextDown);		// ��ǰ�л�ˢ
		SolidBrush solidBrushT(m_clrTitle);			// �������ֻ�ˢ
		SolidBrush solidBrushS(m_clrSeperator);		// �ָ��߻�ˢ

		graphics.SetTextRenderingHint( TextRenderingHintClearTypeGridFit );

		// ������ͨ���ֵ�ˮƽ�ʹ�ֱ���뷽ʽ
		DUI_STRING_ALIGN_DEFINE();

		strFormat.SetTrimming(StringTrimmingEllipsisWord);	// �Ե���Ϊ��λȥβ,��ȥ����ʹ��ʡ�Ժ�
		//strFormat.SetFormatFlags( StringFormatFlagsNoClip | StringFormatFlagsMeasureTrailingSpaces);
		if(!m_bTextWrap)
		{
			strFormat.SetFormatFlags(StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);	// ������
		}

		// �����ֶβ����м����
		StringFormat strFormatHeader;
		strFormatHeader.SetAlignment(StringAlignmentCenter);	// �м����
		strFormatHeader.SetLineAlignment(StringAlignmentCenter);	// �м����
		if(!m_bTextWrap)
		{
			strFormatHeader.SetFormatFlags(StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);	// ������
		}

		// ��������
		if((m_nHeaderHeight > 0) && (m_vecColumnInfo.size() > 0))
		{
			// ����Ԫ������
			int nPosItemX = 0;
			for(size_t j = 0; j < m_vecColumnInfo.size(); j++)
			{
				GridColumnInfo &columnInfo = m_vecColumnInfo.at(j);
				int nWidth = columnInfo.nWidth;
				if(j == 0)
				{
					nWidth += m_nLeftPos;
				}
				RectF rect((Gdiplus::REAL)nPosItemX, 0, (Gdiplus::REAL)nWidth, (Gdiplus::REAL)(m_nHeaderHeight-1));

				// ���б���
				CString strTitle = columnInfo.strTitle;
				BSTR bsTitle = m_strTitle.AllocSysString();
				graphics.DrawString(bsTitle, (INT)wcslen(bsTitle), &font, rect, &strFormatHeader, &solidBrushT);
				::SysFreeString(bsTitle);

				nPosItemX += nWidth;
			}
		}
		
		if(m_vecRowInfo.size() > 0)
		{
			for(int i = m_nFirstViewRow; i <= m_nLastViewRow && i < (int)m_vecRowInfo.size(); i++)
			{
				GridRowInfo &rowInfo = m_vecRowInfo.at(i);
				SolidBrush solidBrushRow(rowInfo.clrText);	// �ж������ɫ

				int nXPos = 0;
				int nVI = i - m_nFirstViewRow;

				// ����ƶ�����ʱ����ʾ�ı�����ɫ
				if(m_nHoverRow == i)
				{
					SolidBrush brush(m_clrRowHover);
					graphics.FillRectangle(&brush, 0, m_nHeaderHeight + nVI*m_nRowHeight, nWidth, m_nRowHeight);
				}

				// ������
				int nCheckImgY = 3;
				if((m_sizeCheckBox.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
				{
					nCheckImgY = (m_nRowHeight - m_sizeCheckBox.cy) / 2 + 1;
				}
				if((rowInfo.nCheck != -1) && (m_pImageCheckBox != NULL))
				{
					int nCheckImageIndex = ((m_nHoverRow == i) ? ((rowInfo.nCheck==1) ? 4 : 1) : ((rowInfo.nCheck==1) ? 2 : 0));
					graphics.DrawImage(m_pImageCheckBox, Rect(nXPos, m_nHeaderHeight + nVI*m_nRowHeight + nCheckImgY, m_sizeCheckBox.cx, m_sizeCheckBox.cy),
						nCheckImageIndex * m_sizeCheckBox.cx, 0, m_sizeCheckBox.cx, m_sizeCheckBox.cy, UnitPixel);
					rowInfo.rcCheck.SetRect(nXPos, i*m_nRowHeight + nCheckImgY, nXPos + m_sizeCheckBox.cx, i*m_nRowHeight + nCheckImgY + m_sizeCheckBox.cy);
					nXPos += (m_sizeCheckBox.cx + 3);
				}

				// �������ͼƬ
				int nImgY = 3;
				if(rowInfo.pImage != NULL)
				{
					if((rowInfo.sizeImage.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
					{
						nImgY = (m_nRowHeight - rowInfo.sizeImage.cy) / 2 + 1;
					}
					// ʹ��������ָ����ͼƬ
					graphics.DrawImage(rowInfo.pImage, Rect(nXPos, m_nHeaderHeight + nVI*m_nRowHeight + nImgY, rowInfo.sizeImage.cx, rowInfo.sizeImage.cy),
						0, 0, rowInfo.sizeImage.cx, rowInfo.sizeImage.cy, UnitPixel);
					nXPos += (rowInfo.sizeImage.cx + 3);
				}else
				if((rowInfo.nImageIndex != -1) && (m_pImage != NULL))
				{
					if((m_sizeImage.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
					{
						nImgY = (m_nRowHeight - m_sizeImage.cy) / 2 + 1;
					}
					// ʹ������ͼƬ
					graphics.DrawImage(m_pImage, Rect(nXPos, m_nHeaderHeight + nVI*m_nRowHeight + nImgY, m_sizeImage.cx, m_sizeImage.cy),
						rowInfo.nImageIndex*m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
					nXPos += (m_sizeImage.cx + 3);
				}

				// �����ұ�ͼƬ
				int nRightImageWidth = 0;
				nImgY = 3;
				if(rowInfo.pRightImage != NULL)
				{
					if((rowInfo.sizeRightImage.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
					{
						nImgY = (m_nRowHeight - rowInfo.sizeRightImage.cy) / 2 + 1;
					}
					// ʹ��������ָ����ͼƬ
					graphics.DrawImage(rowInfo.pRightImage, Rect(nWidth-rowInfo.sizeRightImage.cx-1, m_nHeaderHeight + nVI*m_nRowHeight + nImgY, rowInfo.sizeRightImage.cx, rowInfo.sizeRightImage.cy),
						0, 0, rowInfo.sizeRightImage.cx, rowInfo.sizeRightImage.cy, UnitPixel);
					nRightImageWidth = rowInfo.sizeRightImage.cx + 1;
				}else
				if((rowInfo.nRightImageIndex != -1) && (m_pImage != NULL))
				{
					if((m_sizeImage.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
					{
						nImgY = (m_nRowHeight - m_sizeImage.cy) / 2 + 1;
					}
					// ʹ������ͼƬ
					graphics.DrawImage(m_pImage, Rect(nWidth-m_sizeImage.cx-1, m_nHeaderHeight + nVI*m_nRowHeight + nImgY, m_sizeImage.cx, m_sizeImage.cy),
						rowInfo.nRightImageIndex*m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
					nRightImageWidth = m_sizeImage.cx + 1;
				}

				// ����Ԫ������
				int nPosItemX = (m_nLeftPos != 0) ? m_nLeftPos : nXPos;
				for(size_t j = 0; j < rowInfo.vecItemInfo.size(); j++)
				{
					GridColumnInfo &columnInfo = m_vecColumnInfo.at(j);
					GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(j);
					BOOL bSingleLine = (itemInfo.strContent.IsEmpty() || !itemInfo.strLink.IsEmpty());
					RectF rect((Gdiplus::REAL)nPosItemX,
						(Gdiplus::REAL)(m_nHeaderHeight + nVI*m_nRowHeight + 1),
						(Gdiplus::REAL)((j == 0) ? (itemInfo.rcItem.Width() - nPosItemX): itemInfo.rcItem.Width()),
						(Gdiplus::REAL)(bSingleLine ? (m_nRowHeight - 2) : (m_nRowHeight / 2 - 2)));
					if((int)(rect.GetRight()) > nWidth)
					{
						// ���һ����Ҫ��ȥ���������
						rect.Width -= m_nScrollWidth;
					}

					// ����Ԫ��ͼƬ
					int nItemImageX = 0;
					int nImgY = 3;
					if(itemInfo.pImage != NULL)
					{
						if((itemInfo.sizeImage.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
						{
							nImgY = (m_nRowHeight - rowInfo.sizeImage.cy) / 2 + 1;
						}
						// ʹ�õ�Ԫ��ָ����ͼƬ
						graphics.DrawImage(itemInfo.pImage, Rect(nPosItemX+nItemImageX, m_nHeaderHeight + nVI*m_nRowHeight + nImgY, itemInfo.sizeImage.cx, itemInfo.sizeImage.cy),
							0, 0, itemInfo.sizeImage.cx, itemInfo.sizeImage.cy, UnitPixel);
						nItemImageX += (itemInfo.sizeImage.cx + 3);
					}else
					if((itemInfo.nImageIndex != -1) && (m_pImage != NULL))
					{
						if((m_sizeImage.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
						{
							nImgY = (m_nRowHeight - m_sizeImage.cy) / 2 + 1;
						}
						// ʹ������ͼƬ
						graphics.DrawImage(m_pImage, Rect(nPosItemX+nItemImageX, m_nHeaderHeight + nVI*m_nRowHeight + nImgY, m_sizeImage.cx, m_sizeImage.cy),
							itemInfo.nImageIndex*m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
						nItemImageX += (m_sizeImage.cx + 3);
					}
					rect.Offset((Gdiplus::REAL)nItemImageX, 0);
					rect.Width -= (Gdiplus::REAL)nItemImageX;

					// ����Ԫ��������������
					SolidBrush solidBrushItem(m_clrText);
					if(m_nHoverRow == i)
					{
						solidBrushItem.SetColor(m_clrTextHover);
					}else
					if(m_nDownRow == i)
					{
						solidBrushItem.SetColor(m_clrTextDown);
					}else
					if(itemInfo.clrText.GetValue() != Color(0, 0, 0, 0).GetValue())
					{
						solidBrushItem.SetColor(itemInfo.clrText);
					}
					CString strItemTitle = itemInfo.strTitle;
					// �����Ƿ���Ҫ��ʾtip
					itemInfo.bNeedTitleTip = rect.Width < GetTextBounds(font, strItemTitle).Width;
					itemInfo.bNeedContentTip = rect.Width < GetTextBounds(font, itemInfo.strContent).Width;
					if(!itemInfo.strLink.IsEmpty())
					{
						strItemTitle = itemInfo.strLink;
						if((m_nHoverRow == i) && (rowInfo.nHoverItem == j))
						{
							solidBrushItem.SetColor(m_clrTextHover);
						}else
						{
							solidBrushItem.SetColor((itemInfo.clrText.GetValue() != Color(0, 0, 0, 0).GetValue()) ? itemInfo.clrText : m_clrText);
						}
					}

					// ���õ�Ԫ�����ֶ��뷽ʽ,ʹ���еĶ��뷽ʽ
					StringFormat strFormatColumn;
					UINT uAlignment = m_uAlignment;
					if(columnInfo.uAlignment != 0xFFFFUL)
					{
						uAlignment = columnInfo.uAlignment;
					}
					if(uAlignment == Align_Left)
					{
						strFormatColumn.SetAlignment(StringAlignmentNear);
					}else
					if(uAlignment == Align_Center)
					{
						strFormatColumn.SetAlignment(StringAlignmentCenter);
					}else
					if(uAlignment == Align_Right)
					{
						strFormatColumn.SetAlignment(StringAlignmentFar);
					}
					UINT uVAlignment = m_uVAlignment;
					if(columnInfo.uVAlignment != 0xFFFFUL)
					{
						uVAlignment = columnInfo.uVAlignment;
					}
					if(uVAlignment == VAlign_Top)
					{
						strFormatColumn.SetLineAlignment(StringAlignmentNear);
					}else
					if(uVAlignment == VAlign_Middle)
					{
						strFormatColumn.SetLineAlignment(StringAlignmentCenter);
					}else
					if(uVAlignment == VAlign_Bottom)
					{
						strFormatColumn.SetLineAlignment(StringAlignmentFar);
					}
					if(!m_bTextWrap)
					{
						strFormatColumn.SetFormatFlags(StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);	// ������
					}

					// ����bUseTitleFont�����ñ������廹����ͨ����
					BSTR bsItemTitle = strItemTitle.AllocSysString();
					graphics.DrawString(bsItemTitle, (INT)wcslen(bsItemTitle),
						itemInfo.bUseTitleFont ? &fontTitle : &font, rect, &strFormatColumn, itemInfo.bUseTitleFont ? &solidBrushT : &solidBrushItem);
					::SysFreeString(bsItemTitle);

					// ����Ԫ������
					if(!bSingleLine)
					{
						rect.Offset(0, (Gdiplus::REAL)m_nRowHeight / 2 + 2);
						rect.Height = (Gdiplus::REAL)m_nRowHeight / 2 - 4;
						BSTR bsItemContent = itemInfo.strContent.AllocSysString();
						graphics.DrawString(bsItemContent, (INT)wcslen(bsItemContent), &font, rect, &strFormatColumn, &solidBrushItem);
						::SysFreeString(bsItemContent);
					}

					// ���õ�Ԫ���ӿؼ���λ��
					for(size_t k = 0; k < itemInfo.vecControl.size(); k++)
					{
						CControlBase* pControl = itemInfo.vecControl.at(k);
						if(pControl)
						{
							CRect rcParent = CRect(nPosItemX, m_nHeaderHeight + nVI*m_nRowHeight + 1,
								(int)(rect.X+rect.Width), (nVI+1)*m_nRowHeight - 1);
							if((int)(rect.GetRight()) > nWidth)
							{
								// ���һ����Ҫ��ȥ���������
								rcParent.right -= m_nScrollWidth;
							}
							rcParent.OffsetRect(m_rc.left, m_rc.top - (nYViewPos + m_nHeaderHeight));
							pControl->SetPositionWithParent(rcParent);
							CRect rcControl = pControl->GetRect();
							// ֻ�е�ǰ����ʾ��Χ�ڵĿؼ�����Ϊ�ɼ�
							if((rcControl.top < m_rc.top) || (rcControl.bottom > m_rc.bottom))
							{
								pControl->SetVisible(FALSE);
							}else
							{
								pControl->SetVisible(TRUE);
							}
						}
					}

					if(j == 0)
					{
						// Ϊ��ʹ�ڶ��п�ʼ�Ƕ����,���Եڶ��п�ʼλ�ð��յ�һ�еĿ�ȼ���
						nPosItemX = itemInfo.rcItem.right;
					}else
					{
						nPosItemX += itemInfo.rcItem.Width();
					}
				}

				// ���ָ���(��������ģʽ)
				if(m_pImageSeperator != NULL)
				{
					// ʹ������ģʽ���Ի�ͼ
					graphics.DrawImage(m_pImageSeperator, RectF(0, (Gdiplus::REAL)(m_nHeaderHeight + (nVI+1)*m_nRowHeight), (Gdiplus::REAL)(nWidth-2), (Gdiplus::REAL)m_sizeSeperator.cy),
							0, 0, (Gdiplus::REAL)m_sizeSeperator.cx, (Gdiplus::REAL)m_sizeSeperator.cy, UnitPixel);
				}else
				{
					// δָ��ͼƬ,�򻭾���
					graphics.FillRectangle(&solidBrushS, 0, m_nHeaderHeight + (nVI+1)*m_nRowHeight, nWidth-2, 1);
				}
			}
		}
	}

	// ���������DC,ʹ����ķ�ʽ�ϲ�����
	if(m_nHeaderHeight > 0)
	{
		dc.BitBlt(m_rc.left,m_rc.top, nWidth, m_nHeaderHeight, &m_memDC, 0, 0, SRCCOPY);//SRCAND);
	}
	dc.BitBlt(m_rc.left,m_rc.top + m_nHeaderHeight, nWidth, m_rc.Height() - m_nHeaderHeight, &m_memDC, 0, nYViewPos + m_nHeaderHeight, SRCCOPY);//SRCAND);
}