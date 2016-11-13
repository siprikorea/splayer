#include "stdafx.h"
#include "splayer.h"
#include "splayerdlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP(CSplayerDlg, CDialogEx)
	ON_BN_CLICKED(IDC_PREV, &CSplayerDlg::OnBnClickedPrev)
	ON_BN_CLICKED(IDC_PLAY, &CSplayerDlg::OnBnClickedPlay)
	ON_BN_CLICKED(IDC_STOP, &CSplayerDlg::OnBnClickedStop)
	ON_BN_CLICKED(IDC_NEXT, &CSplayerDlg::OnBnClickedNext)
	ON_WM_DROPFILES()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


/************************************************************
*	@brief		Constructor
*	@retval		Nothing
************************************************************/
CSplayerDlg::CSplayerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SPLAYER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


/************************************************************
*	@brief		Destructor
*	@retval		Nothing
************************************************************/
CSplayerDlg::~CSplayerDlg()
{
}


/************************************************************
*	@brief		Do Data Exchange
*	@param[in]	pDX				Data Exchange
*	@retval		Nothing
************************************************************/
void CSplayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SELECT, m_Select);
	DDX_Control(pDX, IDC_PLAYLIST, m_PlayList);
	DDX_Control(pDX, IDC_PREV, m_Prev);
	DDX_Control(pDX, IDC_PLAY, m_Play);
	DDX_Control(pDX, IDC_STOP, m_Stop);
	DDX_Control(pDX, IDC_NEXT, m_Next);
}


/************************************************************
*	@brief		On Initalize Dialog
*	@retval		TRUE
*	@retval		FALSE
************************************************************/
BOOL CSplayerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	SetIcon(m_hIcon, TRUE);

	SetIcon(m_hIcon, FALSE);


	DragAcceptFiles();

	m_PlayList.DragAcceptFiles();


	LVCOLUMN col;

	col.mask = LVCF_TEXT;

	col.pszText = _T("");

	m_PlayList.InsertColumn(0, &col);

	m_PlayList.ModifyStyle(0, LVS_SHOWSELALWAYS);

	m_PlayList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);


	UpdatePlayList();


	return TRUE;
}


/************************************************************
*	@brief		Update play list
*	@retval		Nothing
************************************************************/
void CSplayerDlg::UpdatePlayList()
{
	m_PlayList.DeleteAllItems();


	LVCOLUMN col;

	col.mask = LVCF_FMT | LVCF_TEXT;

	col.fmt = LVCFMT_LEFT;

	col.pszText = _T("Name");

	m_PlayList.SetColumn(0, &col);


	m_PlayList.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);


	auto playList = m_PlayCtrl.GetPlayList();

	int nItem = 0;

	for (auto it = playList.begin(); it != playList.end(); it++)
	{
		LVITEM	lvItem;

		lvItem.mask		= LVIF_TEXT;

		lvItem.iItem	= nItem++;

		lvItem.iSubItem	= 0;

		lvItem.pszText	= PathFindFileName(*it);

		m_PlayList.InsertItem(&lvItem);
	}
}


/************************************************************
*	@brief		Update selection
*	@retval		Nothing
************************************************************/
void CSplayerDlg::UpdateSelect()
{
	// Release selected item
	POSITION pos = m_PlayList.GetFirstSelectedItemPosition();

	int nSel = -1;

	if (pos)
	{
		nSel = m_PlayList.GetNextSelectedItem(pos);
	}

	m_PlayList.SetItemState(nSel, 0, LVIS_SELECTED);


	// Select item
	int nPos = m_PlayCtrl.GetPlayPos();

	m_PlayList.SetItemState(nPos, LVIS_SELECTED, LVIS_SELECTED);
}



/************************************************************
*	@brief		On paint
*	@retval		Nothing
************************************************************/
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


/************************************************************
*	@brief		On query drag icon
*	@retval		Nothing
************************************************************/
HCURSOR CSplayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


/************************************************************
*	@brief		On previous button clicked
*	@retval		Nothing
************************************************************/
void CSplayerDlg::OnBnClickedPrev()
{
	m_PlayCtrl.Prev();


	UpdateSelect();
}


/************************************************************
*	@brief		On play button clicked
*	@retval		Nothing
************************************************************/
void CSplayerDlg::OnBnClickedPlay()
{
	POSITION pos = m_PlayList.GetFirstSelectedItemPosition();

	int nSel = -1;

	if (pos)
	{
		nSel = m_PlayList.GetNextSelectedItem(pos);
	}


	m_PlayCtrl.Play(nSel);
}


/************************************************************
*	@brief		On stop button clicked
*	@retval		Nothing
************************************************************/
void CSplayerDlg::OnBnClickedStop()
{
	m_PlayCtrl.Stop();
}


/************************************************************
*	@brief		On next button clicked
*	@retval		Nothing
************************************************************/
void CSplayerDlg::OnBnClickedNext()
{
	m_PlayCtrl.Next();


	UpdateSelect();
}


/************************************************************
*	@brief		On drop files
*	@retval		Nothing
************************************************************/
void CSplayerDlg::OnDropFiles(HDROP hDropInfo)
{
	CString strFile;

	DWORD nBuffer = 0;


	std::list<CString> playList;


	// Get the number of files dropped
	int nFilesDropped = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);

	for (int i = 0; i<nFilesDropped; i++)
	{
		// Get the buffer size of the file.
		nBuffer = DragQueryFile(hDropInfo, i, NULL, 0);

		// Get path and name of the file
		DragQueryFile(hDropInfo, i, strFile.GetBuffer(nBuffer + 1), nBuffer + 1);

		strFile.ReleaseBuffer();

		playList.insert(playList.end(), strFile);
	}


	// Free the memory block containing the dropped-file information
	DragFinish(hDropInfo);


	m_PlayCtrl.AddPlayList(playList);


	// Update play list
	UpdatePlayList();
}
