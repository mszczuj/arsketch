/* 
 * File:   ObjectChooser.cpp
 * Author: mich
 * 
 * Created on 23 luty 2011, 15:51
 */

#include <OGRE/Ogre.h>
#include "ObjectChooser.h"
#include "Workspace.h"
#include <boost/math/distributions/normal.hpp>

ObjectChooser::ObjectChooser(Ogre::SceneManager * sceneMgr, int marker_id) :
BaseSketchObject(sceneMgr, "ObjectChooser") {
    this->marker_id = marker_id;
    mesh_id = 0;
    actualOffset = 0.0;
    last_color_id = 1;
    isResizingTablePlane = false;
    initializeTable();
    initializeToolboxObjects();
    initializeToolboxColors();
    neutralPositionSet = false;
    lastColorPicked = mode = -1;
}

void ObjectChooser::setMode(int mode) {
    if (this->mode != mode) {

        if (mode != OC_OBJECT_CHOOSE_MODE) {
            for (int i = 0; i < toolboxObjects.size(); i++)
                toolboxObjects[i]->setVisible(false, true);
            for (int i = 0; i < colorObjects.size(); i++)
                colorObjects[i]->setVisible(true, true);
        } else {
            for (int i = 0; i < colorObjects.size(); i++)
                colorObjects[i]->setVisible(false, true);
            for (int i = 0; i < toolboxObjects.size(); i++)
                toolboxObjects[i]->setVisible(true, true);
        }
        Ogre::LogManager::getSingletonPtr()->logMessage(
                Ogre::String("[Awsome] changing mode"));
        this->mode = mode;
    }

}

Ogre::SceneNode * ObjectChooser::createNewObject(Ogre::String name) {
    int newMeshId = mesh_id++;
    Ogre::String objName = name + "::" + name + Ogre::StringConverter::toString(newMeshId);

    Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create(
            objName,
            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    mat->getTechnique(0)->getPass(0)->createTextureUnitState();
    mat->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
    //    mat->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
    mat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, Ogre::LBS_TEXTURE, 0.5);

    Ogre::Entity * ent = sceneMgr->createEntity(
            objName,
            name + ".mesh");

    ent->setCastShadows(true);
    ent->setMaterial(mat);
    Ogre::SceneNode * node = surfPlaneNode->createChildSceneNode();

    node->attachObject(ent);
    return node;
}

void ObjectChooser::initializeToolboxObjects() {
    objectNames.push_back("Sphere");
    objectNames.push_back("Osmioscian");
    objectNames.push_back("Cube");
    objectNames.push_back("Cylinder");
    objectNames.push_back("Cylinder_pusty");
    objectNames.push_back("Jajo");
    objectNames.push_back("Podstawka");
    objectNames.push_back("Stozek");
    objectNames.push_back("Stozek_uciety");
    objectNames.push_back("Torus");
    objectNames.push_back("Polsfera");

    step = 360.0 / objectNames.size() - 1;

    for (int i = 0; i < objectNames.size(); i++) {
        Ogre::SceneNode * node = createNewObject(objectNames[i]);
        Ogre::Vector3 tr = Ogre::Vector3(
                Toolbox_Radius * Ogre::Math::Cos(step * i),
                Toolbox_Radius * Ogre::Math::Sin(step * i),
                5);
        node->setPosition(tr);
        toolboxObjects.push_back(node);
    }

}

void ObjectChooser::initializeToolboxColors() {
    std::string colors[] = {"White", "Yellow", "Fuchsia", "Red", "Silver", "Gray", "Olive", "Purple", "Maroon", "Aqua", "Lime", "Teal", "Green", "Blue", "Navy", "Black"};

    int colorIdx = -1;
    for (int x = 0; x < 4; x++)
        for (int y = 0; y < 4; y++) {
            Ogre::Entity * ent = sceneMgr->createEntity(
                    Ogre::String("Template/") + colors[++colorIdx],
                    "Cube.mesh");

            ent->setCastShadows(true);
            //            ent->setMaterialName(objName);
            Ogre::Vector3 tr = Ogre::Vector3(3.0 + x * 4.0, 3.0 + y * 4.0, 1.0);
            Ogre::SceneNode * node = surfPlaneNode->createChildSceneNode(tr);
            //
            Ogre::LogManager::getSingletonPtr()->logMessage(
                    Ogre::String("[Awsome] loading color = ") + Ogre::String("Template/") + colors[colorIdx]);

            Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().
                    getByName(Ogre::String("Template/") + colors[colorIdx]);

            mat->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
            mat->getTechnique(0)->getPass(0)->setDepthWriteEnabled(true);
            mat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, Ogre::LBS_TEXTURE, 0.5);

            ent->setMaterial(mat);
            node->attachObject(ent);
            node->setPosition(tr);
            node->setScale(1.0, 1.0, 0.01);
            colorObjects.push_back(node);
        }
}

bool ObjectChooser::tryPickUp() {
    if (mode == OC_OBJECT_CHOOSE_MODE) {
        Ogre::Vector3 pointerPos = workspc->pencil->getPointerPosition();
        Ogre::SceneNode * pickedObject = NULL;
        int pickedObject_id = 0;
        for (int i = 0; i < toolboxObjects.size(); i++) {
            Ogre::SceneNode * obj = toolboxObjects[i];
            if (checkIfPointInSphere(3.0,
                    surfPlaneNode->convertLocalToWorldPosition((obj)->getPosition()),
                    pointerPos)) {
                Ogre::LogManager::getSingletonPtr()->logMessage(
                        Ogre::String("[Awsome]  picking up = ") + Ogre::StringConverter::toString(i));
                workspc->pencil->pickUpNode(obj);
                pickedObject = obj;
                pickedObject_id = i;
                break;
            }
        }

        if (pickedObject != NULL) {
            setNeutralPosition();
            Ogre::LogManager::getSingletonPtr()->logMessage(
                    Ogre::String("[Awsome  registering new = ") + Ogre::StringConverter::toString(pickedObject_id));

            Ogre::SceneNode * node = createNewObject(objectNames[pickedObject_id]);
            node->translate(Ogre::Vector3(
                    Toolbox_Radius * Ogre::Math::Cos(step * pickedObject_id),
                    Toolbox_Radius * Ogre::Math::Sin(step * pickedObject_id), 5));
            toolboxObjects[pickedObject_id] = node;

            //            updateToolbox(actualPos);
        }
    } else if (mode == OC_COLOR_CHOOSE_MODE) {
        Ogre::Vector3 lpos = surfPlaneNode->convertWorldToLocalPosition(
                workspc->pencil->getPointerPosition());

        for (int i = 0; i < colorObjects.size(); i++) {
            Ogre::Vector3 pos = colorObjects[i]->getPosition();
            if ((lpos.x < pos.x + 2.0 && lpos.x > pos.x - 2.0) &&
                    (lpos.y < pos.y + 2.0 && lpos.y > pos.y - 2.0)) {
                colorObjects[i]->setScale(1.0, 1.0, 1.0);
                if (lpos.z < 3.0 && lpos.z > -2.0 && lastColorPicked != i && workspc->pencil->isObjectAttachedOnTop()) {
                    Ogre::LogManager::getSingletonPtr()->logMessage(Ogre::String("[Awsome] color picked "));
                    lastColorPicked = i;
                    Ogre::SceneNode * obj = workspc->pencil->getAttachedObject();
                    Ogre::Entity * ent = (Ogre::Entity*) obj->getAttachedObject(0);
                    Ogre::Entity * colorEnt = (Ogre::Entity*) colorObjects[i]->getAttachedObject(0);
                    Ogre::MaterialPtr colorMat = colorEnt->getSubEntity(0)->getMaterial();

//                    Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().c
//                            getByName(colorMat,);
//                    ent->setMaterial(mat);

//                    Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().clo
                    Ogre::MaterialPtr mat = colorMat->clone(
                            "pickedColor" + Ogre::StringConverter::toString(++last_color_id));

                    ent->setMaterial(mat);

                    //tutaj trzeba raczej sklonowac obiekt a nie
                    //no oczywiscie zalezy co robi metoda set material name
                    //                    Ogre::Entity * entColor = (Ogre::Entity *) colorObjects[i]->getAttachedObject(0);
                    //sklonuj i wymysl nowa nazwe
                    //                    entColor->getSubEntity(0)->getMaterial()->clone()


                    //                    usun material ktory juz ma (mozesz sprawdzic licznik uzycia na sharedptr)
                    //                    Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().remove()

                    //                    ent->setMaterialName(colorObjects[i]->getAttachedObject(0)->getName());
                }
            } else
                colorObjects[i]->setScale(1.0, 1.0, 0.1);
        }
    }
    return false;
}

void ObjectChooser::configureTable(BaseSketchObject * bso) {
    //    if (checkIfPointInSphere(2.0,
    //            surfPlaneNode->convertLocalToWorldPosition(tableCornerNode->getPosition()),
    //            bso->getPosition())) {
    //        tableCornerNode->setPosition(surfPlaneNode->convertWorldToLocalPosition(bso->getPosition()));
    //        tableCornerNode->setVisible(true);
    //    } else tableCornerNode->setVisible(false);
}

void ObjectChooser::setNeutralPosition() {
    neutralPos = surfPlaneNode->getPosition();
    neutralPositionSet = true;
}

void ObjectChooser::updatePosition(MarkerInfo marker_info) {
    BaseSketchObject::updatePosition(marker_info);
    Ogre::Vector3 currPos = getPosition();
    Ogre::Real d = Ogre::Math::Sqr((neutralPos[0] - currPos[0]) * (neutralPos[0] - currPos[0]) +
            (neutralPos[1] - currPos[1]) * (neutralPos[1] - currPos[1]));

    //    last_positions
    //    tutaj dac liczenie preskosci

    Ogre::Real dist_x = 0.0;
    Ogre::Real dist_y = 0.0;
    for (int i = 0; i < last_positions.size(); i++) {
        dist_x += last_positions[i].x;
        dist_y += last_positions[i].y;
    }

    dist_x = currPos.x / ((double) dist_x / last_positions.size());
    dist_y = currPos.y / ((double) dist_y / last_positions.size());

    //    Ogre::LogManager::getSingletonPtr()->logMessage(
    //            Ogre::String("[Awsome] dist_x ") + Ogre::StringConverter::toString(Ogre::Real(round(dist_x * 100) - 100)));
    //    if (workspc->pencil->isPencilBusy()) workspc->pencil->setScale(d < 10.0 ? d : 10.0);

    dist_x = Ogre::Math::isNaN(dist_x) ? 0 : (round(dist_x * 100) - 100);
    dist_y = Ogre::Math::isNaN(dist_y) ? 0 : (round(dist_y * 100) - 100);
    updateToolbox(dist_x, dist_y);

    workspc->pencil->setScale(d);
}

void ObjectChooser::updateToolbox(Ogre::Real dist_x, Ogre::Real dist_y) {
    //    actualPos = position;
    Ogre::Real alpha;
    boost::math::normal nor(0.5, 0.20);
    actualOffset += (dist_x / 1500.0 + dist_y / 1500.0) / 2;

    //    Ogre::LogManager::getSingletonPtr()->logMessage(
    //            Ogre::String("[Awsome] offset = ") + Ogre::StringConverter::toString(Ogre::Real(actualOffset))
    //            + Ogre::String(" position change = ") + Ogre::StringConverter::toString(position));

    for (int i = 0; i < toolboxObjects.size(); i++) {
        alpha = ((double) i) / (toolboxObjects.size());
        alpha = boost::math::cdf(nor, alpha + (actualOffset) - Ogre::Math::Floor(alpha + (actualOffset)));
        alpha *= 360.0;
        Ogre::Vector3 tr = Ogre::Vector3(
                Toolbox_Radius * Ogre::Math::Cos(Ogre::Degree(alpha)),
                Toolbox_Radius * Ogre::Math::Sin(Ogre::Degree(alpha)),
                5);
        toolboxObjects[i]->setPosition(tr);
        alpha = boost::math::pdf(nor, ((double) ((int) alpha % 360)) / 360.0);
        alpha *= 0.8;
        alpha = alpha > 0.4 ? alpha : 0.4;
        toolboxObjects[i]->setScale(alpha, alpha, alpha);
    }
}

void ObjectChooser::initializeTable() {

    //    Ogre::Plane plane(Ogre::Vector3::UNIT_Z, 0);
    //    Ogre::MeshManager::getSingleton().createPlane(name + "::TablePlane",
    //            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
    //            plane, 5, 30, 2, 2, true, 1, 5, 5, Ogre::Vector3(-1, 0, 0));
    //
    //    Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create("TransparentMat", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    //    mat->getTechnique(0)->getPass(0)->createTextureUnitState();
    //    mat->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
    //    mat->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
    //    mat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, Ogre::LBS_TEXTURE, 0.2);
    //
    //    Ogre::Entity * planeEnt = sceneMgr->createEntity(name + "::TablePlaneEntity", name + "::TablePlane");
    //    planeEnt->setMaterialName("TransparentMat");
    //
    //    Ogre::Entity* cornerEnt = sceneMgr->createEntity(name + "TableCornerEntity", "Cube.mesh");
    //    tableCornerNode = surfPlaneNode->createChildSceneNode(Ogre::Vector3(-30 + 2.5, 0, 0.5));
    //    tableCornerNode->attachObject(cornerEnt);
    //    tableCornerNode->setVisible(false);
    //
    //    planeTableNode = surfPlaneNode->createChildSceneNode(Ogre::Vector3(-15 + 2.5, 0, 0.5));
    //    planeTableNode->attachObject(planeEnt);
}

ObjectChooser::~ObjectChooser() {
}

