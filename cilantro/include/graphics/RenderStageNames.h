#pragma once

namespace cilantro {

// names of render stages created by the renderer, usable in pipeline links
namespace RenderStageNames {

inline constexpr const char* ShadowMap = "shadow_map";
inline constexpr const char* Forward = "forward";
inline constexpr const char* DeferredGeometry = "deferred_geometry";

// stage owning the framebuffer that all deferred lighting stages draw to (i.e. the output of lighting)
inline constexpr const char* DeferredLighting = "deferred_lighting";

} // namespace RenderStageNames

} // namespace cilantro
