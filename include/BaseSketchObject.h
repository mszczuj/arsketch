/* 
 * File:   BaseSketchObject.h
 * Author: mich
 *
 * Created on 23 luty 2011, 15:49
 */

#ifndef BASESKETCHOBJECT_H
#define	BASESKETCHOBJECT_H

class Workspace;

#include "ARTracker.h"
#include "DynamicLines.h"
#include <OGRE/Ogre.h>

class BaseSketchObject {
public:
    BaseSketchObject(Ogre::SceneManager * sceneMgr, Ogre::String name);
    virtual ~BaseSketchObject();

    virtual void updatePosition(MarkerInfo marker_info);
    bool checkIfPointInSphere(Ogre::Real r, Ogre::Vector3 sphereCenter, Ogre::Vector3 point);
    Ogre::Vector3 getPosition(void);
    Ogre::Quaternion getOrientation(void);

    bool isAlwaysVisible;

    bool isVisible() {
        return visible;
    };

    void setGroundNode(Ogre::SceneNode * node) {
        groundNode = node;
    };

    virtual void createIndicator(void);
    Workspace * workspc;
    bool lastVisibleState;

protected:

    MarkerInfo mMarkerInfo;
    Ogre::SceneManager * sceneMgr;
    Ogre::SceneNode * surfPlaneNode;
    Ogre::SceneNode * lineNode;
    Ogre::SceneNode * indicatorNode;
    DynamicLines * lines;
    Ogre::String name;
    Ogre::SceneNode * groundNode;   

    Ogre::Vector3 compensatePosition(Ogre::Vector3 position);
    void attachNormal(void);
    bool indicatorVisible;
    std::deque<Ogre::Vector3> last_positions;

private:
    void initializeSurfPlane(void);
    bool visible;

    double ema_alpha;
    double ema_denom;
    int visibleChangeDelay;
    int visibleDelayConuter;
};


#endif	/* BASESKETCHOBJECT_H */

