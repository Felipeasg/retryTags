#include "threadbase.h"

CThreadBase::CThreadBase() :
mAlive(false){

}

CThreadBase::~CThreadBase(){
}

void CThreadBase::end(void){
	setAlive(false);
	join();
}
