/* 
 * File:   Pencil.h
 * Author: mich
 *
 * Created on 24 luty 2011, 18:50
 */

#ifndef PENCIL_H
#define	PENCIL_H

#include "BaseSketchObject.h"

class Pencil : public BaseSketchObject {
public:
    Pencil(Ogre::SceneManager * sceneMgr, int marker_id);
    Ogre::Vector3 getPointerPosition();
    virtual ~Pencil();

    bool isPencilBusy() {
        return pencilBusy;
    };

    bool isObjectAttachedOnTop() {
        return objectAttachedOnTop;
    };
    void pickUpNode(Ogre::SceneNode * node);
    //    void putNode(SketchPlatform * platform);
    void putNode();
    void setScale(Ogre::Real scale);
    virtual void createIndicator(void);
    virtual void moveLastObjectOnTop(void);
    virtual void moveObjectOnTopToPointer(void);
    virtual void updatePosition(MarkerInfo marker_info);

    Ogre::SceneNode * getAttachedObject() {
        return attachedObject;
    };
    Ogre::Real startingScale;
protected:
    Ogre::SceneNode * pointerNode;
    Ogre::SceneNode * attachedObject;
    bool pencilBusy;
    bool objectAttachedOnTop;
    Ogre::Real scale;

private:
};

#endif	/* PENCIL_H */

