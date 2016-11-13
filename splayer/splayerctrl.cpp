#include "stdafx.h"
#include "splayerctrl.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace libZPlay;


/************************************************************
*	@brief		Constructor
*	@retval		Nothing
************************************************************/
CSplayerCtrl::CSplayerCtrl()
{
	m_pZPlay	= CreateZPlay();

	m_PlayPos	= -1;
}


/************************************************************
*	@brief		Destructor
*	@retval		Nothing
************************************************************/
CSplayerCtrl::~CSplayerCtrl()
{
	m_pZPlay->Release();
}


/************************************************************
*	@brief		Add play list
*	@retval		Nothing
************************************************************/
void CSplayerCtrl::AddPlayList(std::list<CString> playList)
{
	m_PlayList.insert(m_PlayList.end(), playList.begin(), playList.end());
}


/************************************************************
*	@brief		Add play list
*	@retval		Nothing
************************************************************/
std::list<CString> CSplayerCtrl::GetPlayList()
{
	return m_PlayList;
}


/************************************************************
*	@brief		Get play position
*	@retval		Nothing
************************************************************/
int CSplayerCtrl::GetPlayPos()
{
	return m_PlayPos;
}


/************************************************************
*	@brief		On previous button clicked
*	@retval		Nothing
************************************************************/
void CSplayerCtrl::Prev()
{
	Play(m_PlayPos - 1);
}


/************************************************************
*	@brief		On play button clicked
*	@retval		Nothing
************************************************************/
void CSplayerCtrl::Play(int nPos)
{
	if (nPos < 0 || nPos >= m_PlayList.size())

		return;


	m_PlayPos = nPos;

	auto it = m_PlayList.begin();

	std::advance(it, m_PlayPos);


	m_pZPlay->OpenFileW(*it, sfAutodetect);

	m_pZPlay->Play();
}


/************************************************************
*	@brief		On stop button clicked
*	@retval		Nothing
************************************************************/
void CSplayerCtrl::Stop()
{
	m_pZPlay->Stop();
}


/************************************************************
*	@brief		On next button clicked
*	@retval		Nothing
************************************************************/
void CSplayerCtrl::Next()
{
	Play(m_PlayPos + 1);
}
