#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "forte_thread.h"
#include "tag.h"
#include "utils.h"

/**
 * To create tags with cpppo:
 * $ enip_server FLOW_RATE=REAL PROGRAM:SP=REAL MV=REAL PID_AM=BOOL PID_AM=BOOL FLAG=BOOL PV=REAL -v
 *
 * To write tags with cpppo:
 * $ enip_client MV=50.0 PV=60.0 FLOW_RATE=20.0 PROGRAM:SP=10.0 FLAG='(BOOL)TRUE' PID_AM='(BOOL)TRUE'
 * $ enip_client MV=10.0 PV=20.0 FLOW_RATE=30.0 PROGRAM:SP=40.0 FLAG='(BOOL)FALSE' PID_AM='(BOOL)FALSE'
 * 
 * In this program you can stop enip_server and when you recreate (with enip_sever) the tags it will create them again...
 *
 */
#define PV_TAG_PATH "protocol=ab_eip&gateway=127.0.0.1&path=1,0&cpu=compactlogix&elem_size=4&element_count=1&name=FLOW_RATE&debug=1"
#define SP_TAG_PATH "protocol=ab_eip&gateway=127.0.0.1&path=1,0&cpu=compactlogix&elem_size=4&element_count=1&name=PROGRAM:SP&debug=1"
#define MV_TAG_PATH "protocol=ab_eip&gateway=127.0.0.1&path=1,0&cpu=compactlogix&elem_size=4&element_count=1&name=MV&debug=1"
#define PID_AM_TAG_PATH "protocol=ab_eip&gateway=127.0.0.1&path=1,0&cpu=compactlogix&elem_size=1&element_count=1&name=PID_AM&debug=1"
#define FLAG_TAG_PATH "protocol=ab_eip&gateway=127.0.0.1&path=1,0&cpu=compactlogix&elem_size=1&element_count=1&name=FLAG&debug=1"
#define FLOWRATE_TAG_PATH "protocol=ab_eip&gateway=127.0.0.1&path=1,0&cpu=compactlogix&elem_size=4&element_count=1&name=PV&debug=1"

/* Catch Signal Handler functio */                                                                            
void sigpipe_handler(int signum){                                                                     
                                                                                                              
    printf("Caught signal SIGPIPE %d\n",signum);                                                              
}                                                                                                             
       

class MainThread : public CThread {

public:
	MainThread();

protected:

	virtual void run();

private:

	Tag *m_PVTag;
	Tag *m_SPTag;
	Tag *m_MVTag;
	Tag *m_PIDAMTag;
	Tag *m_FLAGTag;
	Tag *m_FLOWRATETag;
};

MainThread::MainThread() {

	m_PVTag = Tag::create(PV_TAG_PATH);
	m_SPTag = Tag::create(SP_TAG_PATH);
	m_MVTag = Tag::create(MV_TAG_PATH);
	m_PIDAMTag = Tag::create(PID_AM_TAG_PATH);
	m_FLAGTag = Tag::create(FLAG_TAG_PATH);
	m_FLOWRATETag = Tag::create(FLOWRATE_TAG_PATH);

	m_PVTag->setRetryTime(5000);
	m_SPTag->setRetryTime(5000);
	m_MVTag->setRetryTime(5000);
	m_PIDAMTag->setRetryTime(5000);
	m_FLAGTag->setRetryTime(5000);
	m_FLOWRATETag->setRetryTime(5000);
}

void MainThread::run() {

	while(1) {

		DEBUG_PRINT("Thread running\n");

		int rc = PLCTAG_STATUS_OK;

		rc = m_PVTag->read(50);
		if(rc == PLCTAG_STATUS_OK) {
			float value = m_PVTag->getFloat32(0);
			printf("PV: %f\n", value);
		} else {
			printf("Error to read PV tag: %s\n", plc_tag_decode_error(rc));
		}

		rc = m_SPTag->read(50);
		if(rc == PLCTAG_STATUS_OK) {
			float value = m_SPTag->getFloat32(0);
			printf("SP: %f\n", value);
		} else {
			printf("Error to read SP tag: %s\n", plc_tag_decode_error(rc));
		}

		rc = m_MVTag->read(50);
		if(rc == PLCTAG_STATUS_OK) {
			float value = m_MVTag->getFloat32(0);
			printf("MV: %f\n", value);
		} else {
			printf("Error to read MV tag: %s\n", plc_tag_decode_error(rc));
		}

		rc = m_FLOWRATETag->read(50);
		if(rc == PLCTAG_STATUS_OK) {
			float value = m_FLOWRATETag->getFloat32(0);
			printf("FLOW RATE: %f\n", value);
		} else {
			printf("Error to read FLOWRATE tag: %s\n", plc_tag_decode_error(rc));
		}

		rc = m_PIDAMTag->read(50);
		if(rc == PLCTAG_STATUS_OK) {
			uint8_t value = m_PIDAMTag->getUInt8(0);
			printf("PIDAM: %s\n", value == 0 ? "FALSE" : "TRUE");
		} else {
			printf("Error to read PIDAM tag: %s\n", plc_tag_decode_error(rc));
		}

		rc = m_FLAGTag->read(50);
		if(rc == PLCTAG_STATUS_OK) {
			uint8_t value = m_FLAGTag->getUInt8(0);
			printf("FLAG_TAG: %s\n", value == 0 ? "FALSE" : "TRUE");
		} else {
			printf("Error to read FLAG_TAG tag: %s\n", plc_tag_decode_error(rc));
		}
		printf("\n");

		//usleep(100000);	

		sleep(1);
	}

}

int main(int argc, char **argv){


    /* Catch Signal Handler SIGPIPE */                                                                        
    signal(SIGPIPE, sigpipe_handler);

	MainThread *t = new MainThread();

	t->start();
	while(1) {

		sleep(1);
	}







}
