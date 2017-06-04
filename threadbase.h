#ifndef _THREADBASE_H_
#define _THREADBASE_H_

class CThreadBase{

public:
	bool isAlive(void) const{
          return mAlive;
        }

	virtual void end();

	virtual void join() = 0;

protected:
    CThreadBase();
	virtual ~CThreadBase();

	void setAlive(bool paVal) {
          mAlive = paVal;
    }

	virtual void run() = 0;

private:

	volatile  bool mAlive;

	CThreadBase(const CThreadBase&);
	CThreadBase& operator = (const CThreadBase &);

};

#endif
