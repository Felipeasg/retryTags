/*******************************************************************************
 * Copyright (c) 2006 - 2016 ACIN, fortiss GmbH
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *  Alois Zoitl, Rene Smodic, Ingo Hegny, Martin Melik Merkiumians - initial API and implementation and/or initial documentation
 *  Alois Zoitl - extracted common functions to new base class CThreadBase
 *******************************************************************************/
#ifndef _FORTE_THREAD_H_
#define _FORTE_THREAD_H_

#include <pthread.h>
#include "threadbase.h"
#include "forte_sync.h"

class CPosixThread : public CThreadBase {

public:

	explicit CPosixThread(long pa_nStackSize = 0);

	virtual ~CPosixThread();

	void start(void);

	virtual void join(void);

protected:

private:

	static void * threadFunction(void *arguments);

	CSyncObject mJoinMutex;

	pthread_t m_stThreadID;
	
	long m_nStackSize;

	char *m_pacStack;

	CPosixThread(const CPosixThread&);
    CPosixThread& operator = (const CPosixThread &);
};

typedef CPosixThread CThread;  //allows that doxygen can generate better documentation

#endif
