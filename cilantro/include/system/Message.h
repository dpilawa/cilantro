#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include "cilantroengine.h"
#include <string>

class Message 
{
public:
    Message() = default;
    virtual ~Message() = default;
};

class ResourceMessage : public Message
{
public:
    ResourceMessage (handle_t handle) : m_handle (handle) {}
    handle_t GetHandle () const { return m_handle; }
protected:
    handle_t m_handle;
};

/* gameobject messages */

class TransformUpdateMessage : public ResourceMessage
{
public:
    TransformUpdateMessage (handle_t handle) : ResourceMessage (handle) {}
};

class SceneGraphUpdateMessage : public ResourceMessage
{
public:
    SceneGraphUpdateMessage (handle_t handle) : ResourceMessage (handle) {}
};

class MeshObjectUpdateMessage : public ResourceMessage
{
public:
    MeshObjectUpdateMessage (handle_t handle) : ResourceMessage (handle) {}
};

class LightUpdateMessage : public ResourceMessage
{
public:
    LightUpdateMessage (handle_t handle) : ResourceMessage (handle) {}
};

/* material messages */
class MaterialUpdateMessage : public ResourceMessage
{
public:
    MaterialUpdateMessage (handle_t handle) : ResourceMessage (handle) {}
};

class MaterialTextureUpdateMessage : public MaterialUpdateMessage
{
public:
    MaterialTextureUpdateMessage (handle_t handle, int textureUnit) : MaterialUpdateMessage (handle), m_textureUnit (textureUnit) {}
    int GetTextureUnit () const { return m_textureUnit; }
protected:
    int m_textureUnit;
};

/* input controller messages */

class InputEventMessage : public Message
{
public:
    InputEventMessage (std::string event, float value) : m_event (event), m_value (value) {}
    std::string GetEvent () const { return m_event; }
    float GetValue () const { return m_value; }
protected:
    std::string m_event;
    float m_value;
};

class InputAxisMessage : public InputEventMessage
{
public:
    InputAxisMessage (std::string event, float value) : InputEventMessage (event, value) {}
};

#endif