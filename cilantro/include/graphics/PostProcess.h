#ifndef _POSTPROCESS_H_
#define _POSTPROCESS_H_

#include "cilantroengine.h"

class PostProcess
{
public:
    __EAPI PostProcess ();
    __EAPI virtual ~PostProcess ();
private:
    virtual void Initialize () = 0;
};

#endif