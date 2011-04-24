/* 
 * File:   Pencil.cpp
 * Author: mich
 * 
 * Created on 24 luty 2011, 18:50
 */

#include "Pencil.h"
#include "Workspace.h"

Pencil::Pencil(Ogre::SceneManager * sceneMgr, int marker_id)
: BaseSketchObject(sceneMgr, "pencil") {

    attachedObject = NULL;
    objectAttachedOnTop = pencilBusy = false;
    Ogre::Entity * ent = sceneMgr->createEntity("pencil_pointer", "Pointer.mesh");
    ent->setCastShadows(true);
    pointerNode = surfPlaneNode->createChildSceneNode(Ogre::Vector3(0, 5, 0));
    pointerNode->setScale(0.5, 0.5, 0.5);
    //    pointerNode->rotate(Ogre::Quaternion(Ogre::Degree(90), Ogre::Vector3::UNIT_Z), Ogre::Node::TS_LOCAL);
    pointerNode->attachObject(ent);
}

void Pencil::updatePosition(MarkerInfo marker_info) {
    BaseSketchObject::updatePosition(marker_info);

    pointerNode->setInheritOrientation(false);
    Ogre::Quaternion q = surfPlaneNode->getOrientation();
    q.y = q.y * -1;
    q.x = q.x * -1;
    pointerNode->setOrientation( q);
    pointerNode->rotate( Ogre::Quaternion(Ogre::Degree(90), Ogre::Vector3::UNIT_Z), Ogre::Node::TS_LOCAL );

    //     * Ogre::Quaternion(Ogre::Degree(90), Ogre::Vector3::UNIT_X)
    //            * Ogre::Quaternion(Ogre::Degree(90), Ogre::Vector3::UNIT_Y)
    //    pointerNode->setOrientation(
    //        surfPlaneNode->convertWorldToLocalOrientation(surfPlaneNode->getOrientation()) *
    //        Ogre::Quaternion(Ogre::Degree(90), Ogre::Vector3::UNIT_X));
    //    pointerNode->rotate(Ogre::Quaternion(Ogre::Degree(1), Ogre::Vector3::UNIT_X), Ogre::Node::TS_LOCAL);
}

void Pencil::setScale(Ogre::Real scale) {
    if (attachedObject != NULL) {
        scale *= 0.3;
        if (scale > 0.1 && scale < 15.0 ) {
//            this->scale = scale;
//            attachedObject->setScale(startingScale + scale, startingScale + scale, startingScale + scale);
        }
    }
}

Ogre::Vector3 Pencil::getPointerPosition() {
    return surfPlaneNode->convertLocalToWorldPosition(pointerNode->getPosition());
}

void Pencil::createIndicator() {
    Ogre::ManualObject * circle = sceneMgr->createManualObject(name + "::indicator");
    float const radius = 2.0;
    float const accuracy = 35;
    circle->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);
    unsigned point_index = 0;
    for (float theta = 0; theta <= 2 * Ogre::Math::PI; theta += Ogre::Math::PI / accuracy) {
        circle->position(radius * cos(theta), 0, radius * sin(theta));
        circle->index(point_index++);
    }
    circle->index(0); // Rejoins the last point to the first.
    circle->end();

    indicatorNode = surfPlaneNode->createChildSceneNode();
    indicatorNode->attachObject(circle);
    indicatorVisible = true;
}

void Pencil::pickUpNode(Ogre::SceneNode* node) {
    Ogre::Quaternion worldOrient = node->getParent()->convertLocalToWorldOrientation(node->getOrientation());
    startingScale = node->getScale().x;
    node->getParent()->removeChild(node);
    surfPlaneNode->addChild(node);
    node->setPosition(pointerNode->getPosition());
    node->setOrientation(surfPlaneNode->convertWorldToLocalOrientation(worldOrient));
    attachedObject = node;
    pencilBusy = true;
    this->pointerNode->setVisible(false);
    Ogre::LogManager::getSingletonPtr()->logMessage(
            Ogre::String("[Awsome] object picked up to pencil "));
}

void Pencil::putNode() {
    workspc->sketchPlatform->putNode(attachedObject);
    pencilBusy = false;
    objectAttachedOnTop = false;
    this->pointerNode->setVisible(true);
    Ogre::LogManager::getSingletonPtr()->logMessage(
            Ogre::String("[Awsome] object left from pencil "));
}

void Pencil::moveLastObjectOnTop() {
    if (attachedObject != NULL) {
        pointerNode->setVisible(true);
        attachedObject->getParent()->removeChild(attachedObject);
        surfPlaneNode->addChild(attachedObject);
        attachedObject->setPosition(0, 0, 0);
        attachedObject->translate(Ogre::Vector3(0, 0, 3.0));
        pencilBusy = false;
        objectAttachedOnTop = true;

        Ogre::LogManager::getSingletonPtr()->logMessage(
                Ogre::String("[Awsome] Moving last object to platform "));
    }
}

void Pencil::moveObjectOnTopToPointer() {
    if (isObjectAttachedOnTop()) {
        attachedObject->getParent()->removeChild(attachedObject);
        surfPlaneNode->addChild(attachedObject);
        attachedObject->setPosition(pointerNode->getPosition());
        pencilBusy = true;
        objectAttachedOnTop = false;
    }
}

Pencil::~Pencil() {
}