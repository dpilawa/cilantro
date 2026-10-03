#pragma once

namespace cilantro {

// names of engine shader programs referenced outside of the shader library
namespace ShaderProgramNames {

inline constexpr const char* ShadowMapDirectional = "shadowmap_directional_shader";
inline constexpr const char* ShadowMapSpot = "shadowmap_spot_shader";
inline constexpr const char* ShadowMapPoint = "shadowmap_point_shader";
inline constexpr const char* AABB = "aabb_shader";
inline constexpr const char* AABBCompute = "aabb_compute_shader";

} // namespace ShaderProgramNames

} // namespace cilantro
