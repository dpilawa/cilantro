#pragma once

#include <string>

namespace cilantro {

// position of a linked render stage relative to the stage being rendered
enum class EPipelineLink { LINK_PREVIOUS, LINK_CURRENT };

// reference to a render stage in the render pipeline; it selects the framebuffer a stage reads from or draws to
// the stage is referenced either relative to the current stage or by its name
class PipelineLink
{
public:
    PipelineLink (EPipelineLink relative) : m_isNamed (false), m_relative (relative) {}
    PipelineLink (const std::string& stageName) : m_isNamed (true), m_relative (EPipelineLink::LINK_CURRENT), m_stageName (stageName) {}
    PipelineLink (const char* stageName) : PipelineLink (std::string (stageName)) {}

    bool IsNamed () const { return m_isNamed; }
    EPipelineLink GetRelative () const { return m_relative; }
    const std::string& GetStageName () const { return m_stageName; }

private:
    bool m_isNamed;
    EPipelineLink m_relative;
    std::string m_stageName;
};

} // namespace cilantro
