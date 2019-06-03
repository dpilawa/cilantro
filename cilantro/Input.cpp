#include "Input.h"


Input::Input (std::string name, float scale) : 
inputName (name), inputScale(scale)
{

}

Input::Input (std::string name) : 
inputName (name), inputScale(1.0f)
{

}

Input::~Input ()
{   
}


std::string Input::GetName () {
    return inputName;
}

float Input::GetScale() 
{
    return inputScale;
}