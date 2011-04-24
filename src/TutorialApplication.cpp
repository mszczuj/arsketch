#include "TutorialApplication.h"

TutorialApplication::TutorialApplication(void) {
    isGroundNodeInitialized = objectJustLeft = false;
    last_marker_id = -1;
}

TutorialApplication::~TutorialApplication(void) {
}

void TutorialApplication::createCameraBackground() {
    mCustomTexture = new CustomTexture(640, 480);
    //
    Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create("Background", "General");
    Ogre::TextureUnitState* tus = material->getTechnique(0)->getPass(0)->createTextureUnitState();
    tus->setTextureName(mCustomTexture->getCameraTexture()->getName());
    material->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
    material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
    material->getTechnique(0)->getPass(0)->setLightingEnabled(false);

    // Create background rectangle covering the whole screen
    Ogre::Rectangle2D* rect = new Ogre::Rectangle2D(true);
    rect->setCorners(-1.0, 1.0, 1.0, -1.0);
    rect->setMaterial("Background");

    // Render the background before everything else
    rect->setRenderQueueGroup(Ogre::RENDER_QUEUE_BACKGROUND);
    rect->setBoundingBox(Ogre::AxisAlignedBox(-100000.0 * Ogre::Vector3::UNIT_SCALE, 100000.0 * Ogre::Vector3::UNIT_SCALE));

    Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode("Background");
    node->attachObject(rect);
}

void TutorialApplication::createCamera(void) {
    mCamera = mSceneMgr->createCamera("camera");
    mCamera->setNearClipDistance(1);
    mCamera->setFarClipDistance(1000);
    mCamera->setPosition(Ogre::Vector3(0, 0, 0));
    //    mCamera->lookAt(Ogre::Vector3(0, 0, -300));
    mCamera->setFOVy(Ogre::Degree(40)); //FOVy camera Ogre = 40°

    mCameraMan = new OgreBites::SdkCameraMan(mCamera); // create a default camera controller
}

void TutorialApplication::createViewports() {
    mViewport = mWindow->addViewport(mCamera);
    mCamera->setAspectRatio(Ogre::Real(mViewport->getActualWidth()) / Ogre::Real(mViewport->getActualHeight()));

    mCameraNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("cameraNode");
    mCameraNode->setPosition(0, 0, 0);
    mCameraNode->lookAt(Ogre::Vector3(0, 0, 1), Ogre::Node::TS_WORLD);
    mCameraNode->attachObject(mCamera);
    mCameraNode->setFixedYawAxis(true, Ogre::Vector3::UNIT_Y);
}

void TutorialApplication::createWorkspace() {
    Workspace * workspc = new Workspace;

    mObjectChooser = new ObjectChooser(mSceneMgr, MARKER_ID_OBJECT_CHOOSER);

    mSketchPlatform = new SketchPlatform(mSceneMgr, MARKER_ID_SKETCH_PLATFORM);
    mSketchPlatform->isAlwaysVisible = true;
    mPencil = new Pencil(mSceneMgr, MARKER_ID_PENCIL);

    workspc->objectChooser = mObjectChooser;
    workspc->sketchPlatform = mSketchPlatform;
    workspc->pencil = mPencil;

    mObjectChooser->workspc = workspc;
    mSketchPlatform->workspc = workspc;
    mPencil->workspc = workspc;
}

void TutorialApplication::initializeMaterials() {
    //    Ogre::MaterialPtr myManualObjectMaterial = Ogre::MaterialManager::getSingleton().create("lineMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    //    myManualObjectMaterial->setReceiveShadows(false);
    //    myManualObjectMaterial->getTechnique(0)->setLightingEnabled(true);
    //    myManualObjectMaterial->getTechnique(0)->getPass(0)->setDiffuse(0, 0, 1, 0);
    //    myManualObjectMaterial->getTechnique(0)->getPass(0)->setAmbient(0, 0, 1);
    //    myManualObjectMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(0, 0, 1);
}

void TutorialApplication::createScene(void) {

    mCamReader = new CameraReader(640, 480);
    mTracker = new ARTracker(640, 480);
    initializeMaterials();
    createCameraBackground();
    createWorkspace();

    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));
    Ogre::Light* l = mSceneMgr->createLight("MainLight");
    l->setPosition(-30, 30, -150);

    mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
}

bool TutorialApplication::frameRenderingQueued(const Ogre::FrameEvent& evt) {
    bool r = BaseApplication::frameRenderingQueued(evt);

    if (!mCamReader->captureStarted) mCamReader->startCapture();
    mCamReader->calcRawData();
    mCustomTexture->updateTexture(mCamReader->getRawBgr());
    std::vector<MarkerInfo> detected = mTracker->update(mCamReader->getRawBgr());
    MarkerInfo miObjectChooser;
    MarkerInfo miSketchPlatform;
    MarkerInfo miPencil;
    miObjectChooser.isEmpty = true;

    for (int i; i < detected.size(); i++) {
        MarkerInfo minfo = detected[i];
        if (minfo.marker_id == MARKER_ID_OBJECT_CHOOSER) {
            miObjectChooser = minfo;
            if (mPencil->isObjectAttachedOnTop()) mPencil->moveObjectOnTopToPointer();
            mObjectChooser->setMode(OC_OBJECT_CHOOSE_MODE);
            if (last_marker_id != MARKER_ID_OBJECT_CHOOSER) {
//                mPencil->setScale(mPencil->startingScale);
                mObjectChooser->setNeutralPosition();
            }

            last_marker_id = MARKER_ID_OBJECT_CHOOSER;
        } else if (minfo.marker_id == MARKER_ID_COLOR_CHOOSER) {
            miObjectChooser = minfo;
            if (!mPencil->isObjectAttachedOnTop()) mPencil->moveLastObjectOnTop();
            mObjectChooser->setMode(OC_COLOR_CHOOSE_MODE);
            if (last_marker_id != MARKER_ID_COLOR_CHOOSER) {
//                mPencil->setScale(mPencil->startingScale);
            }
            last_marker_id = MARKER_ID_COLOR_CHOOSER;
        } else if (minfo.marker_id == MARKER_ID_SKETCH_PLATFORM) miSketchPlatform = minfo;
        else if (minfo.marker_id == MARKER_ID_PENCIL) miPencil = minfo;
        else
            Ogre::LogManager::getSingletonPtr()->logMessage(
                Ogre::String("[Awsome] Unused markerId = " + Ogre::StringConverter::toString(minfo.marker_id)));
    }

    mObjectChooser->updatePosition(miObjectChooser);
    mSketchPlatform->updatePosition(miSketchPlatform);
    mPencil->updatePosition(miPencil);

    if (mObjectChooser->isVisible() && !mObjectChooser->lastVisibleState)
        mObjectChooser->setNeutralPosition();

    if (!mPencil->isPencilBusy()) {
        mObjectChooser->tryPickUp();
        if (objectJustLeft) objectJustLeft = mSketchPlatform->choosePickUp();
        else {
            if (mSketchPlatform->choosePickUp() && !mObjectChooser->isVisible()) {
                mSketchPlatform->pickUp();
                objectJustPickedUp = true;
            }
        }
    } else {
        if (objectJustPickedUp) objectJustPickedUp = !mObjectChooser->isVisible();
        else if (!mObjectChooser->isVisible()) {
            mPencil->putNode();
            objectJustLeft = true;
        }
    }
    return r;
}

void TutorialApplication::configureGround() {
    if (!isGroundNodeInitialized) {
        mPencil->createIndicator();
        Ogre::LogManager::getSingletonPtr()->logMessage(
                Ogre::String("[Awesome] Ground created"));
        Ogre::Plane plane(Ogre::Vector3::UNIT_Z, 0);

        Ogre::MeshManager::getSingleton().createPlane("groundPlane",
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                plane, 30, 30, 2, 2, true, 1, 5, 5, Ogre::Vector3(0, 1, 0));

        //        Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create("FullTransparentMat", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        //      Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("plains.material");

        //        mat->getTechnique(0)->getPass(0)->createTextureUnitState();
        //        mat->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
        //        mat->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
        //        mat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, Ogre::LBS_TEXTURE, 0.2);

        Ogre::Entity* planeEnt = mSceneMgr->createEntity("groundEntity", "groundPlane");
        planeEnt->setMaterialName("FullTransparentMat");
        mGroundNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("groundEntity");
        mGroundNode->attachObject(planeEnt);
        isGroundNodeInitialized = true;
        mPencil->setGroundNode(mGroundNode);
        //        mGroundNode->setVisible(false);
    }


    mGroundNode->setPosition(mObjectChooser->getPosition());
    mGroundNode->setOrientation(mObjectChooser->getOrientation());
    Ogre::LogManager::getSingletonPtr()->logMessage(
            Ogre::String("[Awesome] Ground confgiured to orientation") +
            Ogre::StringConverter::toString(mObjectChooser->getOrientation()));
}

bool TutorialApplication::keyPressed(const OIS::KeyEvent & arg) {
    bool r = BaseApplication::keyPressed(arg);

    if (arg.key == OIS::KC_Z) configureGround();

    return r;
}

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else

    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        TutorialApplication app;

        try {
            app.go();
        } catch (Ogre::Exception& e) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                    e.getFullDescription().c_str() << std::endl;
#endif
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif
