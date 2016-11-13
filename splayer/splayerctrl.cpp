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
*	@brief		Set play position
*	@retval		Nothing
************************************************************/
void CSplayerCtrl::SetPlayPos(int nPos)
{
	m_PlayPos = nPos;
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
	if (m_PlayPos <= 0)
	{
		m_PlayPos = 0;

		return;
	}


	m_PlayPos--;

	Play();
}


/************************************************************
*	@brief		On play button clicked
*	@retval		Nothing
************************************************************/
void CSplayerCtrl::Play()
{
	if (m_PlayPos < 0 || m_PlayPos >= m_PlayList.size())

		return;


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
	if (m_PlayPos >= m_PlayList.size() - 1)
	{
		m_PlayPos = m_PlayList.size() - 1;

		return;
	}


	m_PlayPos++;

	Play();
}
