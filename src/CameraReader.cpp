/* 
 * File:   CameraReader.cpp
 * Author: mich
 * 
 * Created on 17 luty 2011, 16:49
 */

#include "CameraReader.h"

#include <pthread.h>
#define NEW(t, n) ((t*)malloc((n)*sizeof(t)))

int maxi(int a, int b) {
    return a > b ? a : b;
}

int mini(int a, int b) {
    return a < b ? a : b;
}

int maxi3(int a, int b, int c) {
    return maxi(a, maxi(b, c));
}

int mini3(int a, int b, int c) {
    return mini(a, mini(b, c));
}

void *captureThreadLoop(void *arg) {
    CapThread *t = (CapThread *) arg;

    while (1) {
        IplImage *frame = cvQueryFrame(t->captureDevice);

        pthread_mutex_lock(&t->frameImageLock);
        {
            t->frameImage->origin = frame->origin;
            cvCopy(frame, t->frameImage, 0);
        }
        pthread_mutex_unlock(&t->frameImageLock);
    }
}

void CameraReader::startCapture() {
    mCapThread = NEW(CapThread, 0);
    mCapThread->captureDevice = mCapture;
    mCapThread->frameImage = mFrameImage;
    pthread_mutex_init(&mCapThread->frameImageLock, NULL);
    pthread_create(&mCapThread->tid, NULL, captureThreadLoop, mCapThread);
    captureStarted = true;
}

void CameraReader::calcRawData() {
    pthread_mutex_lock(&mCapThread->frameImageLock);
    {
        IplImage * frame = mCapThread->frameImage;
        for (int y = 0; y < mHeight; ++y) {
            unsigned char *pin = (unsigned char *) (frame->imageData) + frame->widthStep * y;
            unsigned char *plum = mRawLum + mWidth * y;
            memcpy(mRawBgr + 3 * mWidth*y, pin, 3 * mWidth);
            for (int x = 0; x < mWidth; ++x, pin += 3, plum += 1) {
                *plum = (maxi3(pin[0], pin[1], pin[2]) + mini3(pin[0], pin[1], pin[2]) + 1) >> 1;
            }
        }
    }
    pthread_mutex_unlock(&mCapThread->frameImageLock);
}

CameraReader::CameraReader(int width, int height) {
    mWidth = width;
    mHeight = height;
    captureStarted = false;
    mRawBgr = NEW(unsigned char, width * height * 3);
    mRawLum = NEW(unsigned char, width * height);

    mFrameImage = cvCreateImage(cvSize(width, height), 8, 3);
    mCapture = cvCaptureFromCAM(1);
    if (mCapture)
        Ogre::LogManager::getSingletonPtr()->logMessage(Ogre::String("[Awesome] Created Camera Reader"));
}

CameraReader::~CameraReader() {
}

