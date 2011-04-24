/* 
 * File:   ARTracker.h
 * Author: mich
 *
 * Created on 16 luty 2011, 19:13
 */

#ifndef ARTRACKER_H
#define	ARTRACKER_H
#include <OGRE/Ogre.h>
#include <ARToolKitPlus/TrackerSingleMarker.h>


class MarkerInfo {
public:
    MarkerInfo() { isEmpty = true; };
    int marker_id;
    Ogre::Vector3 position;
    Ogre::Quaternion orientation;
    bool isEmpty;
};

class ARTracker {
public:
    ARTracker(int width, int height);
    virtual ~ARTracker();

    const std::vector<MarkerInfo> update(unsigned char * data) const;

    Ogre::Matrix4 getProjectionMatrix() { return convert(mTracker->getProjectionMatrix()); };
    
private:
    ARToolKitPlus::TrackerSingleMarker* mTracker;
    ARToolKitPlus::CornerPoints trackedPoints;

    Ogre::Matrix4 convert(const ARFloat* _trans) const ;
    
    int mWidth, mHeight;

    Ogre::Quaternion mRot90x;
};

#endif	/* ARTRACKER_H */
