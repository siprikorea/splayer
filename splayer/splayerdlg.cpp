
// splayerDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "splayer.h"
#include "splayerDlg.h"
#include "afxdialogex.h"
#include "../libzplay/libzplay.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace libZPlay;


ZPlay* g_ZPlay = NULL;


BEGIN_MESSAGE_MAP(CSplayerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


CSplayerDlg::CSplayerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SPLAYER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	g_ZPlay = CreateZPlay();
}

CSplayerDlg::~CSplayerDlg()
{
	g_ZPlay->Release();
}

void CSplayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BOOL CSplayerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);


	return TRUE;
}

void CSplayerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CSplayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
