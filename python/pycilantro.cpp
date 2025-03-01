#include <pybind11/pybind11.h>

#include "system/Game.h"
#include "scene/GameScene.h"
#include "resource/ResourceManager.h"

namespace py = pybind11;

//template CGameScene& CGame::Create<CGameScene, std::string>(std::string&&);
//template CGameScene& CResourceManager::Create<CGameScene, std::string>(std::string&&);

// Create Python bindings

PYBIND11_MODULE(pycilantro, m) {
    
    py::class_<CGame>(m, "CGame")
        .def("Initialize", &CGame::Initialize);
        //.def("CreateGameScene", &CGame::Create<CGameScene>);
        
}