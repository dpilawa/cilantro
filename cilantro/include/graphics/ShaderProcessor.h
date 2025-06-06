#ifndef __SHADERPROCESSOR_H__
#define __SHADERPROCESSOR_H__

#include "cilantroengine.h"
#include <unordered_map>
#include <unordered_set>
#include <string>

namespace cilantro {

class ShaderProcessor 
{
public:
    ShaderProcessor (const std::unordered_map<std::string, std::string>& globals);
    virtual ~ShaderProcessor();

    std::string ProcessShader (const std::string& filename);
    void Reset ();

private:
    const std::unordered_map<std::string, std::string>& m_globals;
    std::unordered_set<std::string> m_includedFiles;

    void Trim (std::string& str);
};

}

#endif
