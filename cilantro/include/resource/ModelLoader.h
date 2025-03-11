#ifndef _MODELLOADER_H_
#define _MODELLOADER_H_

#include "cilantroengine.h"
#include <string>

namespace cilantro {

class __CEAPI ModelLoader
{
public:
    __EAPI ModelLoader ();
    __EAPI virtual ~ModelLoader ();

    virtual void Load (std::string sceneName, std::string path) = 0;
 
};

} // namespace cilantro

#endif