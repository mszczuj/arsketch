/* 
 * File:   Workspace.h
 * Author: mich
 *
 * Created on 27 luty 2011, 17:10
 */

#ifndef WORKSPACE_H
#define	WORKSPACE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "Pencil.h"
#include "ObjectChooser.h"
#include "SketchPlatform.h"


class Workspace {
public:
    Pencil  * pencil;
    SketchPlatform  * sketchPlatform;
    ObjectChooser * objectChooser;
};


#ifdef	__cplusplus
}
#endif

#endif	/* WORKSPACE_H */

