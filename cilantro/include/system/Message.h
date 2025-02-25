#ifndef _MESSAGE_H_
#define _MESSAGE_H_

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
    ResourceMessage (size_t handle) : m_handle (handle) {}
    size_t GetHandle () const { return m_handle; }
protected:
    size_t m_handle;
};

/* gameobject messages */

class TransformUpdateMessage : public ResourceMessage
{
public:
    TransformUpdateMessage (size_t handle) : ResourceMessage (handle) {}
};

class SceneGraphUpdateMessage : public ResourceMessage
{
public:
    SceneGraphUpdateMessage (size_t handle) : ResourceMessage (handle) {}
};

class MeshObjectUpdateMessage : public ResourceMessage
{
public:
    MeshObjectUpdateMessage (size_t handle) : ResourceMessage (handle) {}
};

class LightUpdateMessage : public ResourceMessage
{
public:
    LightUpdateMessage (size_t handle) : ResourceMessage (handle) {}
};

/* material messages */
class MaterialUpdateMessage : public ResourceMessage
{
public:
    MaterialUpdateMessage (size_t handle) : ResourceMessage (handle) {}
};

class MaterialTextureUpdateMessage : public MaterialUpdateMessage
{
public:
    MaterialTextureUpdateMessage (size_t handle, int textureUnit) : MaterialUpdateMessage (handle), m_textureUnit (textureUnit) {}
    int GetTextureUnit () const { return m_textureUnit; }
protected:
    int m_textureUnit;
};

/* input controller messages */

class InputEventMessage : public Message
{
public:
    InputEventMessage (std::string event, int value) : m_event (event), m_value (value) {}
    std::string GetEvent () const { return m_event; }
    int GetValue () const { return m_value; }
protected:
    std::string m_event;
    int m_value;
};

class InputAxisMessage : public InputEventMessage
{
public:
    InputAxisMessage (std::string event, int value) : InputEventMessage (event, value) {}
};

#endif