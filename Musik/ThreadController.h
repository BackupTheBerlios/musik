/*
 *  ThreadController.h
 *
 *  This class is used for controlling a thread. Makes life easier.
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/
#ifndef THREADCONTROLLER_H
#define THREADCONTROLLER_H
class CThreadController
{
public:
	CThreadController()
	{
		m_pThread = NULL;
		m_bThreadDeleteCalled = false;
	}
	~CThreadController()
	{
		Empty();
	}
	void Attach(wxThread * thrd)
	{
		wxASSERT(!thrd->IsDetached()); // only non detached threads allowed
		Empty();
		m_pThread = thrd;
	}
	wxThreadError AttachAndRun(wxThread * thrd)
	{
		Attach(thrd);
		m_pThread->Create();
		return m_pThread->Run();
	}
	void Cancel()
	{
		Empty();
	}
	wxThread * Thread() { return m_pThread;}
	wxThread::ExitCode Join()
	{
		wxASSERT(m_pThread);
		wxThread::ExitCode rc = m_pThread->Wait();
		if(m_bThreadDeleteCalled)// we have called thread->Delete() ( see Empty() method)  so we dont delete the object right now. we are called recursive, because ->Delete() dispaches messyage while wyiting for thread ending
			return rc;
		delete m_pThread;
		m_pThread = NULL;
		return rc;
	}

	bool IsAlive()
	{
		return m_pThread && m_pThread->IsAlive();
	}
protected:
	void Empty()
	{
		if(m_pThread)
		{
			m_bThreadDeleteCalled = true;   // protect against deletion of the thread object in our Join() method, thread->Delete() does dispatch messages, if such a message leads to a call of Join() ,this would delete the thread object , => crash
			m_pThread->Delete();
			m_bThreadDeleteCalled = false;
			delete m_pThread;
			m_pThread = NULL;
		}
	}
private:
	bool m_bThreadDeleteCalled;
	wxThread *m_pThread;
};
#endif // THREADCONTROLLER_H
