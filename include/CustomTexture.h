/* 
 * File:   CameraTexture.h
 * Author: mich
 *
 * Created on 16 luty 2011, 14:48
 */

#ifndef CAMERATEXTURE_H
#define	CAMERATEXTURE_H

#include <Ogre.h>

#include <ARToolKitPlus/ARToolKitPlus.h>
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "CustomTexture.h"
#include "CameraReader.h"

class CustomTexture {
public:
    CustomTexture(int width, int height);
    virtual ~CustomTexture();

    Ogre::TexturePtr getCameraTexture();

    void updateTexture(unsigned char * rawData);
    void cleanTextureContents();

private:
    Ogre::TexturePtr mTexture;
    
    int color;
    int mTexWidth;
    int mTexHeight;
};

#endif	/* CAMERATEXTURE_H */

