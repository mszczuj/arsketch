/* 
 * File:   ObjectChooser.h
 * Author: mich
 *
 * Created on 23 luty 2011, 15:51
 */

#ifndef OBJECTCHOOSER_H
#define	OBJECTCHOOSER_H

#include "BaseSketchObject.h"


static const Ogre::Real Toolbox_Radius = 6.0;
static const int OC_OBJECT_CHOOSE_MODE = 1;
static const int OC_COLOR_CHOOSE_MODE = 2;

class ObjectChooser : public BaseSketchObject {
public:
    ObjectChooser(Ogre::SceneManager * sceneMgr, int markerId);
    virtual ~ObjectChooser();
    void configureTable(BaseSketchObject * bso);
    bool tryPickUp();
    virtual void updatePosition(MarkerInfo marker_info);
    virtual void setNeutralPosition(void);

    virtual void updateToolbox(Ogre::Real dist_x, Ogre::Real dist_y);
    void setMode(int mode);
 
    Ogre::Vector3 neutralPos;
    bool neutralPositionSet;

protected:
    void initializeTable(void);
    void initializeToolboxObjects();
    void initializeToolboxColors();
    int mode;
    std::vector<Ogre::SceneNode*> toolboxObjects;
    std::vector<Ogre::SceneNode*> colorObjects;
    std::vector<Ogre::String> objectNames;

private:
    int marker_id;
    Ogre::Real actualPos;
    double actualOffset;

    Ogre::Real step;
    Ogre::SceneNode * planeTableNode;
    Ogre::SceneNode * tableCornerNode;

    int lastColorPicked;
    
    bool isResizingTablePlane;
    Ogre::Vector3 tableEnd;
    int mesh_id;
    unsigned int last_color_id;

    Ogre::SceneNode * createNewObject(Ogre::String name);
};

#endif	/* OBJECTCHOOSER_H */

