#pragma once


#include <afxcmn.h>
#include <afxwin.h>
#include <list>
#include "splayerbtn.h"
#include "splayerctrl.h"


class CSplayerDlg : public CDialogEx
{
public:
	CSplayerDlg(CWnd* pParent = NULL);

	virtual ~CSplayerDlg();


	enum { IDD = IDD_SPLAYER_DIALOG };


protected:
	DECLARE_MESSAGE_MAP()

	virtual void		DoDataExchange(CDataExchange* pDX);

	virtual BOOL		OnInitDialog();

	int					GetSelectedItem();

	void				UpdatePlayList();

	void				UpdateSelect();

	afx_msg void		OnBnClickedPrev();

	afx_msg void		OnBnClickedPlay();

	afx_msg void		OnBnClickedStop();

	afx_msg void		OnBnClickedNext();

	afx_msg void		OnDropFiles(HDROP hDropInfo);

	afx_msg void		OnPaint();

	afx_msg HCURSOR		OnQueryDragIcon();


protected:
	HICON				m_hIcon;

	CStatic				m_Select;

	CListCtrl			m_PlayList;
	
	CButton				m_Prev;
	
	CSplayerButton		m_Play;
	
	CButton				m_Stop;
	
	CButton				m_Next;

	CSplayerCtrl		m_PlayCtrl;
};
