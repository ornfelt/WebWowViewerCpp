#ifndef WOWMAPVIEWERREVIVED_WOWSCENE_H_H
#define WOWMAPVIEWERREVIVED_WOWSCENE_H_H

#include <cstdint>
typedef uint32_t animTime_t;

#include <string>
#include "config.h"
#include "controllable.h"

class IFileRequest {
public:
    virtual void requestFile(const char* fileName) = 0;
};

class IFileRequester {
public:
    virtual void setFileRequestProcessor(IFileRequest * requestProcessor) = 0;
    virtual void provideFile(const char* fileName, unsigned char* data, int fileLength) = 0;
    virtual void rejectFile(const char* fileName) = 0;
};

class WoWScene : public IFileRequester {

public:
    virtual void draw(animTime_t deltaTime) = 0;
    virtual void setScreenSize(int canvWidth, int canvHeight) = 0;
    virtual void switchCameras() = 0;

    virtual IControllable* getCurrentContollable() = 0;
};

extern WoWScene * createWoWScene(Config *config, IFileRequest * requestProcessor, int canvWidth, int canvHeight);

#endif //WOWMAPVIEWERREVIVED_WOWSCENE_H_H
