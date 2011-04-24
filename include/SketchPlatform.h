/* 
 * File:   SketchPlatform.h
 * Author: mich
 *
 * Created on 24 luty 2011, 16:12
 */

#ifndef SKETCHPLATFORM_H
#define	SKETCHPLATFORM_H

#include "BaseSketchObject.h"
class SketchPlatform : public BaseSketchObject {
public:
    SketchPlatform(Ogre::SceneManager * sceneMgr, int markerId);
    void putNode(Ogre::SceneNode * node);
    bool choosePickUp();
    bool pickUp();

    virtual void updatePosition(MarkerInfo marker_info);
    
    virtual ~SketchPlatform();
    
protected:
    std::vector<Ogre::SceneNode*> objects;
    int objectClosesToPointer;
    int invisibleCount;
    bool deletingPlatform;
};

#endif	/* SKETCHPLATFORM_H */

