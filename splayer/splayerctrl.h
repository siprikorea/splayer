#pragma once


#include <afxcmn.h>
#include <afxwin.h>
#include <list>
#include "../libzplay/libzplay.h"


class CSplayerCtrl
{
public:
	CSplayerCtrl();

	virtual ~CSplayerCtrl();


	void				AddPlayList(std::list<CString> playList);

	std::list<CString>	GetPlayList();


	int					GetPlayPos();


	void				Prev();

	void				Play(int nPos);

	void				Stop();

	void				Next();


protected:
	libZPlay::ZPlay*	m_pZPlay;

	std::list<CString>	m_PlayList;

	int					m_PlayPos;
};
