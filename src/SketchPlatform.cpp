/* 
 * File:   SketchPlatform.cpp
 * Author: mich
 * 
 * Created on 24 luty 2011, 16:12
 */

#include "SketchPlatform.h"
#include "Workspace.h"

SketchPlatform::SketchPlatform(Ogre::SceneManager * sceneMgr, int markerId)
: BaseSketchObject(sceneMgr, "SketchPlatform") {
    objectClosesToPointer = -1;
    deletingPlatform = false;
    //cut here start
//        Ogre::Entity * ent = sceneMgr->createEntity(
//                "dupabladatest",
//                "Mesh.001.mesh");
//    Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create(
//            "dupabladatest",
//            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    //
    //    mat->getTechnique(0)->getPass(0)->createTextureUnitState();
    //    mat->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
    //    //    mat->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
    //    mat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, Ogre::LBS_TEXTURE, 0.5);
    //
//        Ogre::SceneNode * node = surfPlaneNode->createChildSceneNode(Ogre::Vector3(0, -1.0, 11.0),
//                        Ogre::Quaternion(Ogre::Degree(90), Ogre::Vector3::UNIT_Y));
//        node->setScale(0.03, 0.03, 0.03);
    //    ent->setMaterial(mat);
//        node->attachObject(ent);
    //    objects.push_back(node);
    //cut here end

    invisibleCount = 0;
}

bool SketchPlatform::choosePickUp() {
    Ogre::Real d = 100000.0;
    int closesObjId;
    Ogre::Vector3 p1;
    Ogre::Vector3 pp = workspc->pencil->getPointerPosition();
    for (int i = 0; i < objects.size(); i++) {
        Ogre::SceneNode * obj = objects[i];
        p1 = surfPlaneNode->convertLocalToWorldPosition(obj->getPosition());
        Ogre::Real dd = p1.squaredDistance(pp);
        if (dd < d) {
            d = dd;
            closesObjId = i;
        }
    }
    if (d < 6.0) {
        objectClosesToPointer = closesObjId;
        for (int i = 0; i < objects.size(); i++) {
            Ogre::Entity * ent = (Ogre::Entity*) objects[i]->getAttachedObject(0);
            //blad blad -- czy ten ent->getName nie ma byc getSubEntity(0)->getMaterial()
            Ogre::MaterialPtr mat = ent->getSubEntity(0)->getMaterial();
            //                        Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(ent->getName());

            if (i != closesObjId) mat->setDepthWriteEnabled(false);
            else mat->setDepthWriteEnabled(true);
        }
    } else if (objectClosesToPointer != -1) {
        objectClosesToPointer = -1;
        for (int i = 0; i < objects.size(); i++) {
            Ogre::Entity * ent = (Ogre::Entity*) objects[i]->getAttachedObject(0);
            //            blad blad
            Ogre::MaterialPtr mat = ent->getSubEntity(0)->getMaterial();
            //                        Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(ent->getName());

            mat->setDepthWriteEnabled(true);
        }
    }
    return objectClosesToPointer != -1;
}

bool SketchPlatform::pickUp() {
    if (objectClosesToPointer != -1) {
        for (int i = 0; i < objects.size(); i++) {
            Ogre::Entity * ent = (Ogre::Entity*) objects[i]->getAttachedObject(0);
            Ogre::MaterialPtr mat = ent->getSubEntity(0)->getMaterial();
//            Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(ent->getName());
            mat->setDepthWriteEnabled(true);
        }
        Ogre::LogManager::getSingletonPtr()->logMessage(
                Ogre::String("[Awsome]  picking up = ") + Ogre::StringConverter::toString(objectClosesToPointer));

        Ogre::SceneNode * node = objects[objectClosesToPointer];
        std::vector<Ogre::SceneNode*> obj2(objects);
        objects.clear();
        for (int i = 0; i < obj2.size(); i++)
            if (i != objectClosesToPointer) objects.push_back(obj2[i]);
        workspc->pencil->pickUpNode(node);
    }
    return false;
}

void SketchPlatform::putNode(Ogre::SceneNode* node) {
    Ogre::Vector3 worldPos = node->getParent()->convertLocalToWorldPosition(node->getPosition());
    Ogre::Quaternion worldOrient = node->getParent()->convertLocalToWorldOrientation(node->getOrientation());

    node->getParent()->removeChild(node);
    surfPlaneNode->addChild(node);
    node->setPosition(surfPlaneNode->convertWorldToLocalPosition(worldPos));
    node->setOrientation(surfPlaneNode->convertWorldToLocalOrientation(worldOrient));
    objects.push_back(node);
}

void SketchPlatform::updatePosition(MarkerInfo marker_info) {
    BaseSketchObject::updatePosition(marker_info);

    if (marker_info.isEmpty) {
        if (invisibleCount > 200) {
            if (invisibleCount == 400) {
                for (int i = 0; i < objects.size(); i++) {
                    Ogre::SceneNode * node = objects[i];
//                    node->getParent()->removeChild(node);
                    node->setVisible(false, true);
//                    node->detachAllObjects();
//                    sceneMgr->destroySceneNode(node);
                }
                objects.clear();
            } else {
                for (int i = 0; i < objects.size(); i++) {
                    Ogre::Entity * ent = (Ogre::Entity*) objects[i]->getAttachedObject(0);
                    Ogre::MaterialPtr mat = ent->getSubEntity(0)->getMaterial();
                    mat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, Ogre::LBS_TEXTURE,
                            1.0 * (1.0 - (((double) (invisibleCount - 200)) / 200.0)));
                    mat->setDepthWriteEnabled(false);
                }
                deletingPlatform = true;
                invisibleCount++;
            }
        } else {
            invisibleCount++;
        }

    } else {
        if (deletingPlatform) {
            for (int i = 0; i < objects.size(); i++) {
                Ogre::Entity * ent = (Ogre::Entity*) objects[i]->getAttachedObject(0);
                Ogre::MaterialPtr mat = ent->getSubEntity(0)->getMaterial();
                mat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, Ogre::LBS_TEXTURE,
                        0.5);
                mat->setDepthWriteEnabled(true);
            }
            deletingPlatform = false;
        }
        invisibleCount = 0;
    }
}

SketchPlatform::~SketchPlatform() {
}

