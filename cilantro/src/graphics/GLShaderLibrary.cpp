#include "graphics/GLShaderLibrary.h"
#include "graphics/GLShader.h"
#include "graphics/GLShaderProgram.h"
#include "graphics/GLUtils.h"
#include "graphics/GLTypes.h"
#include <vector>

namespace cilantro {

namespace {

// vertex attribute layouts (bound explicitly for GLSL older than 3.30)
enum class EVertexLayout { NONE, POSITION, QUAD, MESH };

struct SShaderSpec
{
    const char* name;
    const char* path;
    EShaderType type;
};

struct SSamplerBinding
{
    const char* name;
    int unit;
};

struct SBlockBinding
{
    const char* name;
    EGlUBOType binding;
};

struct SProgramSpec
{
    const char* name;
    std::vector<const char*> shaders;
    EVertexLayout layout;
    // sampler to texture unit assignments (applied for GLSL older than 4.30, newer versions use layout qualifiers)
    std::vector<SSamplerBinding> samplers;
    std::vector<SBlockBinding> uniformBlocks;
};

const std::vector<SShaderSpec> standardShaders = {
    { "default_vertex_shader", "shaders/default.vs", EShaderType::VERTEX_SHADER },
    { "flatquad_vertex_shader", "shaders/flatquad.vs", EShaderType::VERTEX_SHADER },
    { "pbr_forward_fragment_shader", "shaders/pbr_forward.fs", EShaderType::FRAGMENT_SHADER },
    { "pbr_deferred_geometrypass_fragment_shader", "shaders/pbr_deferred_geometrypass.fs", EShaderType::FRAGMENT_SHADER },
    { "pbr_deferred_lightingpass_fragment_shader", "shaders/pbr_deferred_lightingpass.fs", EShaderType::FRAGMENT_SHADER },
    { "blinnphong_forward_fragment_shader", "shaders/blinnphong_forward.fs", EShaderType::FRAGMENT_SHADER },
    { "blinnphong_deferred_geometrypass_fragment_shader", "shaders/blinnphong_deferred_geometrypass.fs", EShaderType::FRAGMENT_SHADER },
    { "blinnphong_deferred_lightingpass_fragment_shader", "shaders/blinnphong_deferred_lightingpass.fs", EShaderType::FRAGMENT_SHADER },
    { "flatquad_fragment_shader", "shaders/flatquad.fs", EShaderType::FRAGMENT_SHADER },
    { "post_hdr_fragment_shader", "shaders/post_hdr.fs", EShaderType::FRAGMENT_SHADER },
    { "post_gamma_fragment_shader", "shaders/post_gamma.fs", EShaderType::FRAGMENT_SHADER },
    { "post_fxaa_fragment_shader", "shaders/post_fxaa.fs", EShaderType::FRAGMENT_SHADER },
    { "shadowmap_vertex_shader", "shaders/shadowmap.vs", EShaderType::VERTEX_SHADER },
    { "shadowmap_directional_geometry_shader", "shaders/shadowmap_directional.gs", EShaderType::GEOMETRY_SHADER },
    { "shadowmap_spot_geometry_shader", "shaders/shadowmap_spot.gs", EShaderType::GEOMETRY_SHADER },
    { "shadowmap_point_geometry_shader", "shaders/shadowmap_point.gs", EShaderType::GEOMETRY_SHADER },
    { "shadowmap_fragment_shader", "shaders/shadowmap.fs", EShaderType::FRAGMENT_SHADER },
    { "aabb_vertex_shader", "shaders/aabb.vs", EShaderType::VERTEX_SHADER },
    { "aabb_fragment_shader", "shaders/aabb.fs", EShaderType::FRAGMENT_SHADER }
};

const SShaderSpec aabbComputeShader = { "aabb_compute_shader", "shaders/aabb.cs", EShaderType::COMPUTE_SHADER };

const std::vector<SProgramSpec> standardPrograms = {

    // PBR model (forward)
    { "pbr_forward_shader", { "default_vertex_shader", "pbr_forward_fragment_shader" }, EVertexLayout::MESH,
        { { "tAlbedo", 0 }, { "tNormal", 1 }, { "tMetallic", 2 }, { "tRoughness", 3 }, { "tAO", 4 }, { "tShadowMap", CILANTRO_SHADOW_MAP_BINDING } },
        { { "UniformMatricesBlock", UBO_MATRICES },
          { "UniformPointLightsBlock", UBO_POINTLIGHTS },
          { "UniformDirectionalLightsBlock", UBO_DIRECTIONALLIGHTS },
          { "UniformSpotLightsBlock", UBO_SPOTLIGHTS },
          { "UniformDirectionalLightViewMatricesBlock", UBO_DIRECTIONALLIGHTVIEWMATRICES },
          { "UniformSpotLightViewMatricesBlock", UBO_SPOTLIGHTVIEWMATRICES },
          { "UniformBoneTransformationsBlock", UBO_BONETRANSFORMATIONS } } },

    // PBR model (deferred, geometry pass)
    { "pbr_deferred_geometrypass_shader", { "default_vertex_shader", "pbr_deferred_geometrypass_fragment_shader" }, EVertexLayout::MESH,
        { { "tAlbedo", 0 }, { "tNormal", 1 }, { "tMetallic", 2 }, { "tRoughness", 3 }, { "tAO", 4 } },
        { { "UniformMatricesBlock", UBO_MATRICES },
          { "UniformBoneTransformationsBlock", UBO_BONETRANSFORMATIONS } } },

    // PBR model (deferred, lighting pass)
    { "pbr_deferred_lightingpass_shader", { "flatquad_vertex_shader", "pbr_deferred_lightingpass_fragment_shader" }, EVertexLayout::QUAD,
        { { "tPosition", 0 }, { "tNormal", 1 }, { "tAlbedo", 2 }, { "tMetallicRoughnessAO", 3 }, { "tUnused", 4 }, { "tShadowMap", CILANTRO_SHADOW_MAP_BINDING } },
        { { "UniformPointLightsBlock", UBO_POINTLIGHTS },
          { "UniformDirectionalLightsBlock", UBO_DIRECTIONALLIGHTS },
          { "UniformSpotLightsBlock", UBO_SPOTLIGHTS },
          { "UniformDirectionalLightViewMatricesBlock", UBO_DIRECTIONALLIGHTVIEWMATRICES },
          { "UniformSpotLightViewMatricesBlock", UBO_SPOTLIGHTVIEWMATRICES } } },

    // Blinn-Phong model (forward)
    { "blinnphong_forward_shader", { "default_vertex_shader", "blinnphong_forward_fragment_shader" }, EVertexLayout::MESH,
        { { "tDiffuse", 0 }, { "tNormal", 1 }, { "tSpecular", 2 }, { "tEmissive", 3 }, { "tShadowMap", CILANTRO_SHADOW_MAP_BINDING } },
        { { "UniformMatricesBlock", UBO_MATRICES },
          { "UniformPointLightsBlock", UBO_POINTLIGHTS },
          { "UniformDirectionalLightsBlock", UBO_DIRECTIONALLIGHTS },
          { "UniformSpotLightsBlock", UBO_SPOTLIGHTS },
          { "UniformDirectionalLightViewMatricesBlock", UBO_DIRECTIONALLIGHTVIEWMATRICES },
          { "UniformSpotLightViewMatricesBlock", UBO_SPOTLIGHTVIEWMATRICES },
          { "UniformBoneTransformationsBlock", UBO_BONETRANSFORMATIONS } } },

    // Blinn-Phong model (deferred, geometry pass)
    { "blinnphong_deferred_geometrypass_shader", { "default_vertex_shader", "blinnphong_deferred_geometrypass_fragment_shader" }, EVertexLayout::MESH,
        { { "tDiffuse", 0 }, { "tNormal", 1 }, { "tSpecular", 2 }, { "tEmissive", 3 } },
        { { "UniformMatricesBlock", UBO_MATRICES },
          { "UniformBoneTransformationsBlock", UBO_BONETRANSFORMATIONS } } },

    // Blinn-Phong model (deferred, lighting pass)
    { "blinnphong_deferred_lightingpass_shader", { "flatquad_vertex_shader", "blinnphong_deferred_lightingpass_fragment_shader" }, EVertexLayout::QUAD,
        { { "tPosition", 0 }, { "tNormal", 1 }, { "tDiffuse", 2 }, { "tEmissive", 3 }, { "tSpecular", 4 }, { "tShadowMap", CILANTRO_SHADOW_MAP_BINDING } },
        { { "UniformPointLightsBlock", UBO_POINTLIGHTS },
          { "UniformDirectionalLightsBlock", UBO_DIRECTIONALLIGHTS },
          { "UniformSpotLightsBlock", UBO_SPOTLIGHTS },
          { "UniformDirectionalLightViewMatricesBlock", UBO_DIRECTIONALLIGHTVIEWMATRICES },
          { "UniformSpotLightViewMatricesBlock", UBO_SPOTLIGHTVIEWMATRICES } } },

    // Screen quad rendering
    { "flatquad_shader", { "flatquad_vertex_shader", "flatquad_fragment_shader" }, EVertexLayout::QUAD,
        { { "fScreenTexture", 0 } }, {} },

    // Post-processing HDR
    { "post_hdr_shader", { "flatquad_vertex_shader", "post_hdr_fragment_shader" }, EVertexLayout::QUAD,
        { { "fScreenTexture", 0 } }, {} },

    // Post-processing gamma
    { "post_gamma_shader", { "flatquad_vertex_shader", "post_gamma_fragment_shader" }, EVertexLayout::QUAD,
        { { "fScreenTexture", 0 } }, {} },

    // Post-processing fxaa
    { "post_fxaa_shader", { "flatquad_vertex_shader", "post_fxaa_fragment_shader" }, EVertexLayout::QUAD,
        { { "fScreenTexture", 0 } }, {} },

    // Shadow map (directional)
    { "shadowmap_directional_shader", { "shadowmap_vertex_shader", "shadowmap_directional_geometry_shader", "shadowmap_fragment_shader" }, EVertexLayout::POSITION,
        {},
        { { "UniformDirectionalLightViewMatricesBlock", UBO_DIRECTIONALLIGHTVIEWMATRICES },
          { "UniformBoneTransformationsBlock", UBO_BONETRANSFORMATIONS } } },

    // Shadow map (spot)
    { "shadowmap_spot_shader", { "shadowmap_vertex_shader", "shadowmap_spot_geometry_shader", "shadowmap_fragment_shader" }, EVertexLayout::POSITION,
        {},
        { { "UniformSpotLightViewMatricesBlock", UBO_SPOTLIGHTVIEWMATRICES },
          { "UniformBoneTransformationsBlock", UBO_BONETRANSFORMATIONS } } },

    // Shadow map (point)
    { "shadowmap_point_shader", { "shadowmap_vertex_shader", "shadowmap_point_geometry_shader", "shadowmap_fragment_shader" }, EVertexLayout::POSITION,
        {},
        { { "UniformPointLightViewMatricesBlock", UBO_POINTLIGHTVIEWMATRICES },
          { "UniformBoneTransformationsBlock", UBO_BONETRANSFORMATIONS } } },

    // AABB rendering
    { "aabb_shader", { "aabb_vertex_shader", "aabb_fragment_shader" }, EVertexLayout::POSITION,
        {},
        { { "UniformMatricesBlock", UBO_MATRICES } } }
};

void BindVertexAttributes (GLuint programId, EVertexLayout layout)
{
    switch (layout)
    {
    case EVertexLayout::MESH:
        glBindAttribLocation (programId, 0, "vPosition");
        glBindAttribLocation (programId, 1, "vNormal");
        glBindAttribLocation (programId, 2, "vUV");
        glBindAttribLocation (programId, 3, "vTangent");
        glBindAttribLocation (programId, 4, "vBitangent");
        break;
    case EVertexLayout::QUAD:
        glBindAttribLocation (programId, 0, "vPosition");
        glBindAttribLocation (programId, 1, "vTextureCoordinates");
        break;
    case EVertexLayout::POSITION:
        glBindAttribLocation (programId, 0, "vPosition");
        break;
    case EVertexLayout::NONE:
        break;
    }
}

} // namespace

GLShaderLibrary::GLShaderLibrary (std::shared_ptr<ResourceManager<Resource>> shaderResources, std::shared_ptr<TShaderProgramManager> shaderPrograms)
    : m_shaderResources (shaderResources)
    , m_shaderPrograms (shaderPrograms)
{
}

GLShaderLibrary::~GLShaderLibrary ()
{
}

void GLShaderLibrary::Initialize ()
{
    LoadShaders ();
    CreatePrograms ();
}

void GLShaderLibrary::OnPointLightAdded (size_t directionalLightCount, size_t spotLightCount, size_t pointLightCount)
{
    // update invocation count in shadow map geometry shader
    auto shadowmapShader = m_shaderResources->GetByName<GLShader> ("shadowmap_point_geometry_shader");
    shadowmapShader->SetVariable ("ACTIVE_POINT_LIGHTS", std::to_string (pointLightCount));
    shadowmapShader->Compile ();

    auto shadowmapShaderProg = m_shaderPrograms->GetByName<GLShaderProgram> ("shadowmap_point_shader");
    shadowmapShaderProg->Link ();
    shadowmapShaderProg->BindUniformBlock ("UniformPointLightViewMatricesBlock", EGlUBOType::UBO_POINTLIGHTVIEWMATRICES);
    shadowmapShaderProg->BindUniformBlock ("UniformBoneTransformationsBlock", EGlUBOType::UBO_BONETRANSFORMATIONS);

    // set offset in shadow map texture array (directional + spot light count for point lights)
    shadowmapShaderProg->SetUniformInt ("textureArrayOffset", static_cast<int>(directionalLightCount + spotLightCount));
}

void GLShaderLibrary::OnDirectionalLightAdded (size_t directionalLightCount, size_t spotLightCount, size_t pointLightCount)
{
    // update invocation count in shadow map geometry shader
    auto shadowmapShader = m_shaderResources->GetByName<GLShader> ("shadowmap_directional_geometry_shader");
    shadowmapShader->SetVariable ("ACTIVE_DIRECTIONAL_LIGHTS", std::to_string (directionalLightCount));
    shadowmapShader->Compile ();

    auto shadowmapShaderProg = m_shaderPrograms->GetByName<GLShaderProgram> ("shadowmap_directional_shader");
    shadowmapShaderProg->Link ();
    shadowmapShaderProg->BindUniformBlock ("UniformDirectionalLightViewMatricesBlock", EGlUBOType::UBO_DIRECTIONALLIGHTVIEWMATRICES);
    shadowmapShaderProg->BindUniformBlock ("UniformBoneTransformationsBlock", EGlUBOType::UBO_BONETRANSFORMATIONS);

    // set offset in shadow map texture array (zero for directional lights)
    shadowmapShaderProg->SetUniformInt ("textureArrayOffset", 0);
    shadowmapShaderProg = m_shaderPrograms->GetByName<GLShaderProgram> ("shadowmap_spot_shader");
    shadowmapShaderProg->SetUniformInt ("textureArrayOffset", static_cast<int>(directionalLightCount));
    shadowmapShaderProg = m_shaderPrograms->GetByName<GLShaderProgram> ("shadowmap_point_shader");
    shadowmapShaderProg->SetUniformInt ("textureArrayOffset", static_cast<int>(directionalLightCount + spotLightCount));
}

void GLShaderLibrary::OnSpotLightAdded (size_t directionalLightCount, size_t spotLightCount, size_t pointLightCount)
{
    // update invocation count in shadow map geometry shader
    auto shadowmapShader = m_shaderResources->GetByName<GLShader> ("shadowmap_spot_geometry_shader");
    shadowmapShader->SetVariable ("ACTIVE_SPOT_LIGHTS", std::to_string (spotLightCount));
    shadowmapShader->Compile ();

    auto shadowmapShaderProg = m_shaderPrograms->GetByName<GLShaderProgram> ("shadowmap_spot_shader");
    shadowmapShaderProg->Link ();
    shadowmapShaderProg->BindUniformBlock ("UniformSpotLightViewMatricesBlock", EGlUBOType::UBO_SPOTLIGHTVIEWMATRICES);
    shadowmapShaderProg->BindUniformBlock ("UniformBoneTransformationsBlock", EGlUBOType::UBO_BONETRANSFORMATIONS);

    // set offset in shadow map texture array (directional light count for spot lights)
    shadowmapShaderProg->SetUniformInt ("textureArrayOffset", static_cast<int>(directionalLightCount));
    shadowmapShaderProg = m_shaderPrograms->GetByName<GLShaderProgram> ("shadowmap_point_shader");
    shadowmapShaderProg->SetUniformInt ("textureArrayOffset", static_cast<int>(directionalLightCount + spotLightCount));
}

void GLShaderLibrary::LoadShaders ()
{
    for (auto&& shader : standardShaders)
    {
        m_shaderResources->Load<GLShader> (shader.name, shader.path, shader.type);
    }

    if (GLUtils::GetGLSLVersion ().versionNumber >= 430)
    {
        m_shaderResources->Load<GLShader> (aabbComputeShader.name, aabbComputeShader.path, aabbComputeShader.type);
    }
}

void GLShaderLibrary::CreatePrograms ()
{
    const int glslVersion = GLUtils::GetGLSLVersion ().versionNumber;

    for (auto&& spec : standardPrograms)
    {
        auto p = m_shaderPrograms->Create<GLShaderProgram> (spec.name);

        for (auto&& shaderName : spec.shaders)
        {
            p->AttachShader (m_shaderResources->GetByName<GLShader> (shaderName));
        }

        p->Link ();
        p->Use ();

        if (glslVersion < 330)
        {
            BindVertexAttributes (p->GetProgramId (), spec.layout);
        }

        if (glslVersion < 430)
        {
            for (auto&& sampler : spec.samplers)
            {
                glUniform1i (glGetUniformLocation (p->GetProgramId (), sampler.name), sampler.unit);
            }
        }

        for (auto&& block : spec.uniformBlocks)
        {
            p->BindUniformBlock (block.name, block.binding);
        }

        GLUtils::CheckGLError (MSG_LOCATION);
    }

    if (glslVersion >= 430)
    {
        // AABB compute shader
        auto p = m_shaderPrograms->Create<GLShaderProgram> ("aabb_compute_shader");
        p->AttachShader (m_shaderResources->GetByName<GLShader> (aabbComputeShader.name));
        p->Link ();
        p->Use ();
        p->BindUniformBlock ("UniformBoneTransformationsBlock", EGlUBOType::UBO_BONETRANSFORMATIONS);
        p->BindShaderStorageBlock ("VertexBufferBlock", EGlSSBOType::SSBO_VERTICES);
        p->BindShaderStorageBlock ("BoneIndicesBufferBlock", EGlSSBOType::SSBO_BONEINDICES);
        p->BindShaderStorageBlock ("BoneWeightsBufferBlock", EGlSSBOType::SSBO_BONEWEIGHTS);
        p->BindShaderStorageBlock ("AABBBufferBlock", EGlSSBOType::SSBO_AABB);
        GLUtils::CheckGLError (MSG_LOCATION);
    }
}

} // namespace cilantro
