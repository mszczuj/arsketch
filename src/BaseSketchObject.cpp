/* 
 * File:   BaseSketchObject.cpp
 * Author: mich
 * 
 * Created on 23 luty 2011, 15:49
 */

#include "BaseSketchObject.h"
#include "DynamicLines.h"

BaseSketchObject::BaseSketchObject(Ogre::SceneManager * sceneMgr, Ogre::String name) :
last_positions(20, Ogre::Vector3(0, 0, 0)) {
    this->sceneMgr = sceneMgr;
    this->name = name;
    groundNode = NULL;
    indicatorVisible = visible = lastVisibleState = isAlwaysVisible = false;
    ;
    initializeSurfPlane();
    attachNormal();
    ema_alpha = 0.8;
    ema_denom = 0.0;
    for (int i; i < 5; i++)
        ema_denom += Ogre::Math::Pow(ema_alpha, i);

    visibleChangeDelay = 15; //change visibility after 0.5 second (depending on fps)
    visibleDelayConuter = 0;
}

BaseSketchObject::~BaseSketchObject() {
}

void BaseSketchObject::createIndicator() {
    Ogre::Entity * ent = sceneMgr->createEntity(name + "::indicator", "Cube.mesh");
    ent->setCastShadows(true);
    indicatorNode = surfPlaneNode->createChildSceneNode(Ogre::Vector3(0, 0, -5));
    indicatorNode->attachObject(ent);
    indicatorVisible = true;
}

void BaseSketchObject::updatePosition(MarkerInfo marker_info) {
    lastVisibleState = visible;
    if (!marker_info.isEmpty) {
        visible = true;
        visibleDelayConuter = 0;
        surfPlaneNode->setVisible(true, false);
        mMarkerInfo = marker_info;
        surfPlaneNode->setPosition(compensatePosition(marker_info.position));

        if (!marker_info.orientation.isNaN()) {
            surfPlaneNode->setOrientation(marker_info.orientation);
            if (groundNode != NULL && indicatorVisible) {
                Ogre::LogManager::getSingletonPtr()->logMessage(
                        Ogre::String("[Awesome] Ground confgiured to orientation"));

                indicatorNode->setOrientation(
                        surfPlaneNode->convertLocalToWorldOrientation(
                        groundNode->getOrientation()));
                indicatorNode->rotate(Ogre::Quaternion(Ogre::Radian(90), Ogre::Vector3::UNIT_X));

                Ogre::Plane pl(groundNode->getOrientation() * Ogre::Vector3::UNIT_Z,
                        groundNode->getPosition());
                Ogre::Ray ray(
                        surfPlaneNode->getPosition(),
                        groundNode->getOrientation() * Ogre::Vector3::NEGATIVE_UNIT_Z);

                std::pair<bool, Ogre::Real> res = ray.intersects(pl);
                if (res.first) {
                    Ogre::Vector3 ipos = indicatorNode->getPosition();
                    ipos.z = res.second * (-1.0);
                    indicatorNode->setPosition(ipos);
                }
            }
        }
    } else {
        if (isAlwaysVisible) {
        } else {
            if (visibleDelayConuter > visibleChangeDelay) {
                surfPlaneNode->setVisible(false, false);
                visible = false;
            } else visibleDelayConuter++;
        }
    }
}

Ogre::Vector3 BaseSketchObject::getPosition() {

    return surfPlaneNode->getPosition();
}

Ogre::Quaternion BaseSketchObject::getOrientation() {

    return surfPlaneNode->getOrientation();
}

bool BaseSketchObject::checkIfPointInSphere(Ogre::Real r, Ogre::Vector3 sc, Ogre::Vector3 p) {
    Ogre::Real in = ((p[0] - sc[0]) * (p[0] - sc[0]))
            + ((p[1] - sc[1])*(p[1] - sc[1]))
            + ((p[2] - sc[2])*(p[2] - sc[2]));

    return in < r * r;
}

void BaseSketchObject::initializeSurfPlane() {

    Ogre::Plane plane(Ogre::Vector3::UNIT_Z, 0);

    Ogre::MeshManager::getSingleton().createPlane(name + "::SurfPlane",
            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            plane, 5, 5, 1, 1, true, 1, 5, 5, Ogre::Vector3(-1, 0, 0));

    Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().
            getByName(Ogre::String("Anims/Test"));

    Ogre::Entity* planeEnt = sceneMgr->createEntity(name + "::SurfPlaneEnt", name + "::SurfPlane");
    planeEnt->setMaterial(mat);
    planeEnt->setCastShadows(true);
    surfPlaneNode = sceneMgr->getRootSceneNode()->createChildSceneNode(name + "::SurfPlaneEnt");
    surfPlaneNode->attachObject(planeEnt);
}

Ogre::Vector3 BaseSketchObject::compensatePosition(Ogre::Vector3 position) {
    Ogre::Vector3 pos(0, 0, 0);
    last_positions.push_front(position);
    last_positions.pop_back();

    for (int x = 0; x < 3; x++) {

        //        for (int y = 0; y < last_positions.size(); y++)
        for (int y = 0; y < 5; y++)
            pos[x] += Ogre::Math::Pow(ema_alpha, y) * last_positions[y][x];
        pos[x] = pos[x] / ema_denom;
    }
    return pos;
}

void BaseSketchObject::attachNormal() {
    lines = new DynamicLines(Ogre::RenderOperation::OT_LINE_LIST);
    lines->addPoint(0, 0, 0);
    lines->addPoint(0, 0, 10);
    lines->update();
    surfPlaneNode->attachObject(lines);
    //    lineNode = surfPlaneNode->createChildSceneNode();
    //    lineNode->attachObject(lines);
}
