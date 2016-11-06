#pragma once


class CSplayerDlg : public CDialogEx
{
public:
	CSplayerDlg(CWnd* pParent = NULL);
	virtual ~CSplayerDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SPLAYER_DIALOG };
#endif


protected:
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

protected:
	HICON m_hIcon;
};
