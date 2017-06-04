#ifndef TAG_H
#define TAG_H


#include <string>
#include <vector>
#include "forte_sync.h"
#include "forte_thread.h"
#include "libplctag.h"

using namespace std;

#define PLCTAG_ERR_RECONNECTING -100

class Tag : public CThread
{
public:

    static Tag *create(string attributes);

    int read(int timeout);
    int write(int timeout);
    int status();
    int size();
    void setRetryTime(int newRetryMS);

    int close();

    /* Data routines */
    uint32_t getUInt32(int offset);
    int setUint32(int offset, uint32_t val);

    uint8_t getUInt8(int offset);
    int setUint8(int offset, uint8_t val);

    float getFloat32(int offset);
    int setFloat32(int offset, float val);

    void checkRetry();

    static void disconnect();

protected:
    virtual void run();

private:
    Tag(string attrs);
    ~Tag();

    void createTag();

    int checkResponse(int rc);

    int retryMs;
    string attributeString;
    unsigned long nextRetry;
    plc_tag tag;

    static vector<Tag*> retryTags;
    static bool init;
    static bool isAlive;


    static CSyncObject m_oSync;

};



#endif // TAG_H
