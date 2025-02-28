#include <pybind11/pybind11.h>

#include "system/Game.h"
#include "scene/GameScene.h"

// Create Python bindings

PYBIND11_MODULE(pycilantro, m) {
    
    pybind11::class_<CGame>(m, "CGame")
        .def("Initialize", &CGame::Initialize);
        //.def("CreateGameScene", &CGame::CreateGameScene<CGameScene>);
        
}