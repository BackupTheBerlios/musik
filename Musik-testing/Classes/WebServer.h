/*
 *  WebServer.h
 *
 *  Musik's internal webserver. 
 *  Someday this will be quite the little jewel.
 *  
 *  Copyright (c) 2003 Simon Windmill
 *	Contributors: Casey Langen  (casey@bak.rr.com), Dustin Carter
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif
#include "wx/socket.h"

#define MUSIK_SERVER_ID 2000

#define MAX_BUF_LEN 1024

class CMusikWebServer
{
public:
	CMusikWebServer();
	~CMusikWebServer();

public:
	void Start();
	void Stop();
	bool IsRunning() { return m_bRunning; };

	void ProcessRequest(wxString reqstr);
	void Listen();

private:
	int ReadLine(wxString& outstr);
	void WriteLine(wxString str);

	bool m_bRunning;
	bool m_bListenLock;

	wxSocketBase *pSocket;
	wxSocketServer *pServer;
};

#endif