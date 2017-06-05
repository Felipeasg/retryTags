#include <stdio.h>
#ifndef WIN32
#include <unistd.h>
#endif

#include "tag.h"
#include "string.h"
#include "utils.h"

bool Tag::init = false;
vector<Tag*> Tag::retryTags;
bool Tag::isAlive = true;
CSyncObject Tag::m_oSync;


Tag::Tag(string attrs)
{
    attributeString = attrs;
    retryMs = 0;
    nextRetry = 0;

    createTag();

	/* Start the thread just once time when the first tag is created. 
	   This is to "emulate" the java static block */
    if(Tag::init == false) {
        Tag::isAlive = false;

        this->start();

    }

    Tag::init = true;
}

Tag::~Tag()
{
    this->end();
    Tag::init = false;
    Tag::retryTags.clear();
}

void Tag::createTag()
{
    tag = plc_tag_create(attributeString.c_str());

    if(tag == PLC_TAG_NULL) {
    	DEBUG_PRINT("createTag: Tag attr: %s\n", attributeString.c_str());
    	DEBUG_PRINT("createTag: error To create tag.\n");
        tag = PLC_TAG_NULL;
    } else {
    	DEBUG_PRINT("createTag: Tag attr: %s\n", attributeString.c_str());
    	DEBUG_PRINT("createTag: tag created.\n");
    }

}

int Tag::checkResponse(int rc)
{
    DEBUG_PRINT("checkResponse\n");
    if(retryMs > 0 && rc < 0) {

        DEBUG_PRINT("checkResponse: retryMs = %d, rc = %d\n", retryMs, rc);

        if(nextRetry == 0) {

            DEBUG_PRINT("CheckResponse: closing tag\n");
            close();

			int64_t currentTime = time_ms();
			nextRetry = currentTime + retryMs;
            DEBUG_PRINT("checkResponse: set nextRetry = %d, currentTime = %ld\n", nextRetry, currentTime);
        }

        return PLCTAG_ERR_RECONNECTING;

    } else {
        nextRetry = 0;

        if(rc != PLCTAG_STATUS_OK) {
            DEBUG_PRINT("ERROR: %s\n", plc_tag_decode_error(rc));
        }
        return rc;
    }
}

Tag *Tag::create(string attributes)
{
    Tag *tmp = new Tag(attributes);

    if(tmp->tag == PLC_TAG_NULL) {

        DEBUG_PRINT("ERROR TO CREATE TAG %s\n", tmp->attributeString.c_str());

        tmp = NULL;
    } else {

        DEBUG_PRINT("TAG CREATED: %s\n", tmp->attributeString.c_str());
	
	    m_oSync.lock();
        Tag::retryTags.push_back(tmp);
	    m_oSync.unlock();
    }

    return tmp;
}

int Tag::read(int timeout)
{
    DEBUG_PRINT("read\n");

    if(tag != PLC_TAG_NULL) {
        DEBUG_PRINT("read: Tag is not null.\n");
        DEBUG_PRINT("read: call checkResponse.\n");

        return checkResponse(plc_tag_read(tag, timeout));

    } else {
        DEBUG_PRINT("Tag is null.\n");

        return checkResponse(PLCTAG_ERR_NULL_PTR);
    }
}

int Tag::write(int timeout)
{
    if(tag != PLC_TAG_NULL) {
        DEBUG_PRINT("Writing the tag!\n");

        return checkResponse(plc_tag_write(tag, timeout));
    } else {
        return checkResponse(PLCTAG_ERR_NULL_PTR);
    }
}

int Tag::status()
{
    if(tag != PLC_TAG_NULL) {
        return checkResponse(plc_tag_status(tag));
    } else {
        return checkResponse(PLCTAG_ERR_NULL_PTR);
    }
}

int Tag::size()
{
    if(tag != PLC_TAG_NULL) {
        return checkResponse(plc_tag_get_size(tag));
    } else {
        return checkResponse(PLCTAG_ERR_NULL_PTR);
    }
}

void Tag::setRetryTime(int newRetryMS)
{
    retryMs = newRetryMS;
}

int Tag::close()
{
    int rc = PLCTAG_ERR_NULL_PTR;

    if(tag != PLC_TAG_NULL) {
        rc = plc_tag_destroy(tag);
	}

    if(rc != PLCTAG_STATUS_OK) {
        DEBUG_PRINT("Fail to destroy!\n");
    }

    tag = PLC_TAG_NULL;

    return rc;
}

uint32_t Tag::getUInt32(int offset)
{
	return plc_tag_get_uint32(tag, offset);
}

int Tag::setUint32(int offset, uint32_t val)
{
    if(tag != PLC_TAG_NULL) {
    	return plc_tag_set_uint32(tag, offset, val);
    } else {
        return checkResponse(PLCTAG_ERR_NULL_PTR);
    }
}

uint8_t Tag::getUInt8(int offset)
{
	return plc_tag_get_uint8(tag, offset);
}

int Tag::setUint8(int offset, uint8_t val)
{

    if(tag != PLC_TAG_NULL) {
		return plc_tag_set_uint8(tag, offset, val);
    } else {
		return checkResponse(PLCTAG_ERR_NULL_PTR);
    }
}

float Tag::getFloat32(int offset)
{
	return plc_tag_get_float32(tag, offset);
}

int Tag::setFloat32(int offset, float val)
{
    if(tag != PLC_TAG_NULL) {
    	return plc_tag_set_float32(tag, offset, val);
    } else {
        return checkResponse(PLCTAG_ERR_NULL_PTR);
    }
}

void Tag::checkRetry()
{
    DEBUG_PRINT("checkRetry: \n");

    if(nextRetry == 0 || nextRetry > time_ms()) {

        DEBUG_PRINT("checkRetry: No retry: nextRetry = %ld currentTime = %ld\n", nextRetry, time_ms());

        return;
    }

    DEBUG_PRINT("checkRetry: timeelapsed: %d");
    DEBUG_PRINT("checkRetry: Recreating tag\n");
    createTag();

    nextRetry = 0;
}

void Tag::disconnect()
{
    Tag::isAlive = true;
}

void Tag::run()
{
    while(!Tag::isAlive) {

    
    			DEBUG_PRINT("\n**************************\n");
                DEBUG_PRINT("*   RETRY CHECK            *\n");
    			DEBUG_PRINT("\n**************************\n");

                m_oSync.lock();
                vector<Tag*>::iterator itEnd = Tag::retryTags.end();
                for(vector<Tag*>::iterator it = Tag::retryTags.begin(); it != itEnd; ++it) {

                    Tag *t = (*it);

                    if(t == NULL) {
                        // TODO: Delete OBject
                    } else {
                        t->checkRetry();
                    }
                }
                m_oSync.unlock();

#ifdef WIN32
        Spleep(1);
#else
        sleep_ms(1000);
#endif
    }

	/* If the thread is finished */
    Tag::init = false;

    vector<Tag*>::iterator itEnd = Tag::retryTags.end();
    for(vector<Tag*>::iterator it = Tag::retryTags.begin(); it != itEnd; ++it) {

        Tag *t = (*it);
        t->close();
    }

    Tag::retryTags.clear();
}



