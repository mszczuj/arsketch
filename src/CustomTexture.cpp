/* 
 * File:   CameraTexture.cpp
 * Author: mich
 * 
 * Created on 16 luty 2011, 14:48
 */

#include "CustomTexture.h"
#define NEW(t, n) ((t*)malloc((n)*sizeof(t)))

CustomTexture::CustomTexture(int width, int height) {
    mTexWidth = width;
    mTexHeight = height;

    color = 0;

    Ogre::LogManager::getSingletonPtr()->logMessage(
            Ogre::String("[Fucking Awesome] Creating texture with dimensions ") +
            Ogre::StringConverter::toString(mTexWidth) + "x" +
            Ogre::StringConverter::toString(mTexHeight) + ".");

    mTexture = Ogre::TextureManager::getSingleton().createManual(
            "CameraTexture", // name
            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            Ogre::TEX_TYPE_2D, // texture type
            mTexWidth,
            mTexHeight,
            0, // number of mipmaps
            Ogre::PF_B8G8R8, // pixel format
            //Ogre::PF_BYTE_L, // pixel format
            Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE// usage
            );

    this->cleanTextureContents();
}

CustomTexture::~CustomTexture() {
}

void CustomTexture::cleanTextureContents() {
    unsigned int idx;
    int x, y;

    // OGRE TEXTURE LOCK
    // get the texture pixel buffer
    int texw = mTexture->getWidth();
    int texh = mTexture->getHeight();
    Ogre::HardwarePixelBufferSharedPtr pixelBuffer = mTexture->getBuffer();

    // lock the pixel buffer and get a pixel box
    pixelBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
    const Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();

    Ogre::uint8* pDest = static_cast<Ogre::uint8*> (pixelBox.data);

    // FILL!
    for (x = 0, y = 0; y < texh;) {
        idx = (x * 4) + y * pixelBox.rowPitch * 4;

        // paint
        pDest[idx] = 100; //b
        pDest[idx + 1] = 255; //g
        pDest[idx + 2] = 100; //r
        pDest[idx + 3] = 100; //a

        x++;
        if (x >= texw) {
            x = 0;
            y++;
        }
    }

    // UNLOCK EVERYTHING!
    // unlock the pixel buffer
    pixelBuffer->unlock();
    // OGRE END
}

Ogre::TexturePtr CustomTexture::getCameraTexture() {
    return mTexture;
}

void CustomTexture::updateTexture(unsigned char * rawData) {
    Ogre::HardwarePixelBufferSharedPtr pixelBuffer = mTexture->getBuffer();
    unsigned char* buffer = (unsigned char*) pixelBuffer->lock(0, mTexHeight * mTexWidth * 4, Ogre::HardwareBuffer::HBL_DISCARD);
    int fh = mTexHeight; // frame->height;
    int fw = mTexWidth; // frame->width;
    for (int y = 0; y < fh; y++) {
        for (int x = 0; x < fw; x++) {
            buffer[((y * fw) + x)*4 + 0] = rawData[ ((y * fw) + x) * 3 + 0]; //B
            buffer[((y * fw) + x)*4 + 1] = rawData[ ((y * fw) + x) * 3 + 1]; //G
            buffer[((y * fw) + x)*4 + 2] = rawData[ ((y * fw) + x) * 3 + 2]; //R
            //                buffer[((y * fw) + x)*3 + 0] = rawlum[ ((y * fw) + x) * 3 + 0]; // LUM
            //                buffer[((y * fw) + x)*3 + 1] = rawlum[ ((y * fw) + x) * 3 + 0]; // LUM
            //                buffer[((y * fw) + x)*3 + 2] = rawlum[ ((y * fw) + x) * 3 + 0]; // LUM

        }
    }
    pixelBuffer->unlock();
}