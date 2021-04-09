#ifndef _MODELLOADER_H_
#define _MODELLOADER_H_

#include "cilantroengine.h"
#include <string>

class ModelLoader
{
public:
    ModelLoader ();
    virtual ~ModelLoader ();

    virtual void Load (std::string path) = 0;
 
};

#endif