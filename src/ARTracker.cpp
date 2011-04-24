/*
 * File:   ARTracker.cpp
 * Author: mich
 *
 * Created on 16 luty 2011, 19:13
 */

#include <OGRE/OgrePixelFormat.h>
//#include <ARToolKitPlus/TrackerMultiMarker.h>

#include "ARTracker.h"

ARTracker::ARTracker(int width, int height) :
mRot90x(Ogre::Degree(270.f), Ogre::Vector3::UNIT_X) {

    mWidth = width;
    mHeight = height;

    mTracker = new ARToolKitPlus::TrackerSingleMarker(mWidth, mHeight, 8, 6, 6, 6, 0);
    mTracker->setPixelFormat(ARToolKitPlus::PIXEL_FORMAT_BGR);

    if (!mTracker->init("/home/mich/no_distortion.cal", 1.0f, 1000.0f)) {
        Ogre::LogManager::getSingletonPtr()->logMessage(
                Ogre::String("[NotGood] Loading config failed "));
    }

    mTracker->setPatternWidth(2.0);
    mTracker->setBorderWidth(0.25);
    mTracker->activateAutoThreshold(true);
    mTracker->setNumAutoThresholdRetries(10);
    //    mTracker->setThreshold(160);


    mTracker->setPoseEstimator(ARToolKitPlus::POSE_ESTIMATOR_RPP);
    mTracker->setUndistortionMode(ARToolKitPlus::UNDIST_STD);
    mTracker->setMarkerMode(ARToolKitPlus::MARKER_ID_SIMPLE);
}

const std::vector<MarkerInfo> ARTracker::update(unsigned char * data) const {
    std::vector<int> markerIds = mTracker->calc(data);
    std::vector<MarkerInfo> markers;

    for (int i = 0; i < markerIds.size(); i++) {
        mTracker->selectDetectedMarker(markerIds[i]);
        MarkerInfo posor;
        posor.isEmpty = false;
        posor.marker_id = markerIds[i];
        Ogre::Matrix4 m = convert(mTracker->getModelViewMatrix()).transpose();
        posor.position = m.getTrans();
        posor.orientation = m.extractQuaternion();
        posor.position[0] *= -2.3;
        posor.position[1] *= -2.3;
        if (!posor.orientation.isNaN()) {
            posor.orientation.x *= -1;
            posor.orientation.y *= -1;
        }
        posor.orientation = posor.orientation
                * Ogre::Quaternion(Ogre::Degree(180.f), Ogre::Vector3::UNIT_Y)
                * Ogre::Quaternion(Ogre::Degree(180.f), Ogre::Vector3::UNIT_X);
        
        markers.push_back(posor);
    }
    return markers;
}

Ogre::Matrix4 ARTracker::convert(const ARFloat * _trans) const {
    Ogre::Matrix4 m;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            m[i][j] = _trans[i * 4 + j];
    return m;
}

ARTracker::~ARTracker() {
}