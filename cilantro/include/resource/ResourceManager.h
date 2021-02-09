#ifndef _RESOURCEMANAGER_H_
#define _RESOURCEMANAGER_H_

#include "cilantroengine.h"
#include "resource/Resource.h"
#include "system/LogMessage.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

template <typename Base = Resource>
class ResourceManager
{
public:
    __EAPI ResourceManager();
    __EAPI virtual ~ResourceManager();

    template <typename T>
    __EAPI unsigned int Load(const std::string& name, const std::string& path = std::string ());

    template <typename T>
    __EAPI unsigned int Create(const std::string& name);

    template <typename T>
    __EAPI std::shared_ptr<T> GetByHandle(unsigned int handle);

    template <typename T>
    __EAPI std::shared_ptr<T> GetByName(const std::string& name);

private:
    unsigned int handle;
    std::vector<std::shared_ptr<Base>> resources;
    std::unordered_map<std::string, unsigned int> resourceNames;
};

// deduction guide for cland (c++17)
ResourceManager() -> ResourceManager<Resource>;

#endif