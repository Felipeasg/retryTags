#include "forte_thread.h"
//#include "../devlog.h"
#include <errno.h>
#include <cstring>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include "criticalregion.h"
#include <stdio.h>

void CPosixThread::start(void){

// Get new Thread
  if(0 == m_stThreadID){

    //If the stackSize is not 0 we have to set it
    if(m_nStackSize){
      pthread_attr_t stAttr;

      if(pthread_attr_init(&stAttr)){
        printf("Error could not get the default thread attributes! %s\n", strerror(errno));
        return;
      }
  #ifdef __CYGWIN__
      if (pthread_attr_setstacksize (&stAttr, m_nStackSize)){
        printf("Error could not set the stacksize for the thread! %s\n", strerror(errno));
        return;
      }
  #else
      //if (pthread_attr_setstacksize(&stAttr, m_nStackSize)) {
      if(pthread_attr_setstack(&stAttr, m_pacStack, m_nStackSize)){
        printf("Error could not set the stacksize for the thread! %s\n", strerror(errno));
        return;
      }
  #endif
      if(pthread_create(&m_stThreadID, &stAttr, threadFunction, this)){
        printf("Error could not create the thread! %s\n", strerror(errno));
        return;
      }
      if(pthread_attr_destroy(&stAttr)){
        printf("Error could not free the thread attributes! %s\n", strerror(errno));
        return;
      }
    }
    else{
      if(pthread_create(&m_stThreadID, NULL, threadFunction, this)){
        printf("Error could not create the thread! %s\n", strerror(errno));
        return;
      }
    }
    // Detach because we don't care about the thread anymore/don't need to join. To cleanup either call pthread_detach or pthread_join
    pthread_detach(m_stThreadID);
  }
  //wait till the thread is up and running
  do{
    //pthread_yield();
    sleep(1);
  }while(!isAlive());

}

void * CPosixThread::threadFunction(void *arguments){
  // Get pointer to CThread object out of void pointer
  CPosixThread *pThread = static_cast<CPosixThread *>(arguments);

  // if pointer is ok
  if(0 != pThread){
    CCriticalRegion criticalRegion(pThread->mJoinMutex);
    pThread->setAlive(true);
    pThread->run();
    pThread->setAlive(false);
    pThread->m_stThreadID = 0;
  }
  else{
    printf("pThread pointer is 0!");
  }
  return 0;
}

CPosixThread::CPosixThread(long pa_nStackSize) :
      m_stThreadID(0), m_nStackSize(pa_nStackSize), m_pacStack(0){

  if(0 != m_nStackSize){
    m_pacStack = new char[m_nStackSize];
  }
}

CPosixThread::~CPosixThread(){
  if(0 != m_stThreadID){
    end();
  }
  if(0 != m_nStackSize){
    delete[] m_pacStack;
  }
}

void CPosixThread::join(void){
  if(0 != m_stThreadID){
    CCriticalRegion criticalRegion(mJoinMutex);
  }
}
