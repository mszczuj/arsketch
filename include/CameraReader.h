/* 
 * File:   CameraReader.h
 * Author: mich
 *
 * Created on 17 luty 2011, 16:49
 */

#ifndef CAMERAREADER_H
#define	CAMERAREADER_H

#include <Ogre.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

struct CapThread {
    pthread_t tid;
    CvCapture *captureDevice;
    IplImage *frameImage;
    pthread_mutex_t frameImageLock;
};

class CameraReader {
public:
    CameraReader(int width, int height);
    virtual ~CameraReader();

    void startCapture(void);
    void calcRawData(void);

    unsigned char * getRawBgr() const { return mRawBgr; } ;
    unsigned char * getRawLum() const { return mRawLum; } ;

    bool captureStarted;
    
private:
    unsigned char * mRawBgr;
    unsigned char * mRawLum;
    IplImage * mFrameImage;
    CvCapture * mCapture;
    CapThread * mCapThread;
    int mWidth, mHeight;
};

#endif	/* CAMERAREADER_H */

