/*
-----------------------------------------------------------------------------
Filename:    TutorialApplication.h
-----------------------------------------------------------------------------

This source file is part of the
   ___                 __    __ _ _    _ 
  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
/ \_// (_| | | |  __/  \  /\  /| |   <| |
\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
      |___/                              
      Tutorial Framework
      http://www.ogre3d.org/tikiwiki/
-----------------------------------------------------------------------------
 */
#ifndef __TutorialApplication_h_
#define __TutorialApplication_h_

#include "BaseApplication.h"
#include "BaseSketchObject.h"

#include "ARTracker.h"
#include "config.h"
#include "CustomTexture.h"
#include "Workspace.h"

class TutorialApplication : public BaseApplication {
public:
    TutorialApplication(void);
    virtual ~TutorialApplication(void);

protected:
    virtual void createCamera(void);
    virtual void createViewports(void);
    virtual void createCameraBackground(void);
    virtual void createScene(void);
    virtual void createWorkspace(void);

    virtual void configureGround(void);

    virtual void initializeMaterials(void);

    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
    virtual bool keyPressed(const OIS::KeyEvent &arg);

    CustomTexture* mCustomTexture;
    ARTracker * mTracker;
    Ogre::SceneNode* headNode ;
    Ogre::SceneNode* planeNode;
    Ogre::SceneNode* planeNode2;

    Ogre::Viewport *mViewport;
    Ogre::SceneNode* mCameraNode;
    Ogre::SceneNode* mGroundNode;
    bool isGroundNodeInitialized;

    CameraReader * mCamReader;
    //-----workspace
    ObjectChooser * mObjectChooser;
    SketchPlatform * mSketchPlatform;
    Pencil * mPencil;

    int last_marker_id;

    bool objectJustLeft;
    bool objectJustPickedUp;
    bool lastObjectChooserVisible;
};

#endif // #ifndef __TutorialApplication_h_
