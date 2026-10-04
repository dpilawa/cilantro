// Test 05: a small first person shooting gallery.
//
// Walk around the arena (W/A/S/D + mouse) and shoot the floating targets with the left mouse button.
// Every hit lights up one pip of the score bar at the top of the screen. The score is also logged to the console.
//
// The demo uses only what the engine has: deferred PBR rendering with shadow mapping, the scene graph (the weapon,
// crosshair and score bar are children of the camera), the input controller and a per-frame GameObject for the game logic.

#include "cilantroengine.h"
#include "scene/Primitives.h"
#include "scene/GameScene.h"
#include "scene/GameObject.h"
#include "scene/PhongMaterial.h"
#include "scene/PBRMaterial.h"
#include "scene/MeshObject.h"
#include "scene/PerspectiveCamera.h"
#include "scene/PointLight.h"
#include "scene/DirectionalLight.h"
#include "resource/Mesh.h"
#include "resource/ResourceManager.h"
#include "graphics/SurfaceRenderStage.h"
#include "graphics/RenderStageNames.h"
#include "graphics/GLFWRenderer.h"
#include "input/GLFWInputController.h"
#include "math/Mathf.h"
#include "system/LogMessage.h"
#include "system/Game.h"
#include "system/Timer.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <random>
#include <string>
#include <vector>

using namespace cilantro;

namespace {

constexpr float ArenaHalfSize = 12.0f;
constexpr float EyeHeight = 1.7f;
constexpr float PlayerRadius = 0.4f;
constexpr float PlayerSpeed = 5.0f;
constexpr float MouseSensitivity = 0.1f;
constexpr float TargetRadius = 0.5f;
constexpr float TargetLifetime = 7.0f;
constexpr float PopTime = 0.25f;
constexpr float FlashTime = 0.05f;
constexpr float FireInterval = 0.15f;
constexpr float HiddenScale = 0.0001f;
constexpr std::size_t TargetCount = 6;
constexpr std::size_t ScorePips = 20;

struct Box
{
    float x, z, halfX, halfZ, height;
};

// pillars and crates: they block movement and shots
const std::vector<Box> obstacles {
    {  0.0f,   0.0f, 1.0f, 1.0f, 4.0f },
    { -6.0f,  -5.0f, 0.8f, 0.8f, 3.0f },
    {  6.5f,  -4.0f, 0.8f, 0.8f, 3.0f },
    { -5.0f,   6.0f, 1.5f, 0.7f, 1.2f },
    {  7.0f,   6.5f, 0.7f, 1.5f, 1.2f },
};

struct Target
{
    std::shared_ptr<MeshObject> object;
    Vector3f position;
    Vector3f velocity;
    float age = 0.0f;
    float pop = -1.0f; // negative: alive, otherwise time since the hit
};

// Game logic: invoked by the engine on every frame
class Director : public GameObject
{
public:
    Director (std::shared_ptr<GameScene> gameScene) : GameObject (gameScene), rng (std::random_device {} ()) {}

    void Setup ()
    {
        auto scene = GetGameScene ();
        auto objects = scene->GetGameObjectManager ();

        camera = objects->GetByName<PerspectiveCamera> ("camera");
        flash = objects->GetByName<MeshObject> ("flash");

        for (std::size_t i = 0; i < TargetCount; i++)
        {
            Target t;
            t.object = objects->GetByName<MeshObject> ("target" + std::to_string (i));
            Respawn (t);
            targets.push_back (t);
        }

        for (std::size_t i = 0; i < ScorePips; i++)
        {
            pips.push_back (objects->GetByName<MeshObject> ("pip" + std::to_string (i)));
        }
        UpdateScoreBar ();

        SetFlash (false);

        auto input = std::dynamic_pointer_cast<GLFWInputController> (scene->GetGame ()->GetInputController ());
        input->CreateInputEvent ("fire", EInputKey::MouseLeft, EInputTrigger::Press, {});
        input->BindInputEvent ("fire", [this]() { wantsToFire = true; });

        input->CreateInputAxis ("moveforward", EInputKey::KeyW, {}, 1.0f);
        input->CreateInputAxis ("moveforward", EInputKey::KeyS, {}, -1.0f);
        input->CreateInputAxis ("moveright", EInputKey::KeyD, {}, 1.0f);
        input->CreateInputAxis ("moveright", EInputKey::KeyA, {}, -1.0f);
        input->CreateInputAxis ("lookyaw", EInputAxis::MouseX, 1.0f);
        input->CreateInputAxis ("lookpitch", EInputAxis::MouseY, 1.0f);

        input->BindInputAxis ("moveforward", [this](float a) { moveForward = a; });
        input->BindInputAxis ("moveright", [this](float a) { moveRight = a; });
        input->BindInputAxis ("lookyaw", [this](float a) { Look (a, 0.0f); });
        input->BindInputAxis ("lookpitch", [this](float a) { Look (0.0f, a); });

        ApplyCamera ();
    }

    void OnFrame () override
    {
        float dt = std::min (GetGameScene ()->GetTimer ()->GetFrameDeltaTime (), 0.1f);

        Move (dt);
        ApplyCamera ();

        fireCooldown -= dt;
        if (wantsToFire && fireCooldown <= 0.0f)
        {
            Fire ();
            fireCooldown = FireInterval;
        }
        wantsToFire = false;

        if (flashTimer > 0.0f)
        {
            flashTimer -= dt;
            if (flashTimer <= 0.0f)
            {
                SetFlash (false);
            }
        }

        for (auto& t : targets)
        {
            UpdateTarget (t, dt);
        }
    }

private:

    // mouse look
    void Look (float dx, float dy)
    {
        yaw += dx * MouseSensitivity;
        pitch = Mathf::Clamp (pitch + dy * MouseSensitivity, -89.0f, 89.0f);
    }

    Vector3f ViewDirection () const
    {
        float y = Mathf::Deg2Rad (yaw);
        float p = Mathf::Deg2Rad (pitch);

        return Vector3f (-std::sin (y) * std::cos (p), std::sin (p), -std::cos (y) * std::cos (p));
    }

    void ApplyCamera ()
    {
        camera->GetModelTransform ()->Translate (position)->Rotate (pitch, yaw, 0.0f);
    }

    // walk on the floor, stay inside the arena and out of the obstacles
    void Move (float dt)
    {
        float y = Mathf::Deg2Rad (yaw);
        Vector3f forward (-std::sin (y), 0.0f, -std::cos (y));
        Vector3f right (std::cos (y), 0.0f, -std::sin (y));

        Vector3f step = (forward * moveForward + right * moveRight);
        if (Mathf::Length (step) > 1.0f)
        {
            step = Mathf::Normalize (step);
        }
        step = step * (PlayerSpeed * dt);

        float limit = ArenaHalfSize - PlayerRadius;
        float x = Mathf::Clamp (position[0] + step[0], -limit, limit);
        float z = Mathf::Clamp (position[2] + step[2], -limit, limit);

        for (auto& b : obstacles)
        {
            float cx = Mathf::Clamp (x, b.x - b.halfX, b.x + b.halfX);
            float cz = Mathf::Clamp (z, b.z - b.halfZ, b.z + b.halfZ);
            float dx = x - cx;
            float dz = z - cz;
            float d = std::sqrt (dx * dx + dz * dz);

            if (d < PlayerRadius)
            {
                if (d > 1e-4f)
                {
                    x = cx + dx / d * PlayerRadius;
                    z = cz + dz / d * PlayerRadius;
                }
                else
                {
                    x = position[0];
                    z = position[2];
                }
            }
        }

        position = Vector3f (x, EyeHeight, z);
    }

    // distance along the ray to the sphere, or a negative number on a miss
    static float RaySphere (const Vector3f& origin, const Vector3f& dir, const Vector3f& center, float radius)
    {
        Vector3f oc = origin - center;
        float b = Mathf::Dot (oc, dir);
        float c = Mathf::Dot (oc, oc) - radius * radius;
        float disc = b * b - c;

        if (disc < 0.0f)
        {
            return -1.0f;
        }

        float t = -b - std::sqrt (disc);
        return t >= 0.0f ? t : -1.0f;
    }

    // distance along the ray to the box (slab method), or a negative number on a miss
    static float RayBox (const Vector3f& origin, const Vector3f& dir, const Box& b)
    {
        const float lo[3] { b.x - b.halfX, 0.0f, b.z - b.halfZ };
        const float hi[3] { b.x + b.halfX, b.height, b.z + b.halfZ };
        float tMin = 0.0f;
        float tMax = 1e9f;

        for (int i = 0; i < 3; i++)
        {
            if (std::abs (dir[i]) < 1e-6f)
            {
                if (origin[i] < lo[i] || origin[i] > hi[i])
                {
                    return -1.0f;
                }
            }
            else
            {
                float t0 = (lo[i] - origin[i]) / dir[i];
                float t1 = (hi[i] - origin[i]) / dir[i];
                tMin = std::max (tMin, std::min (t0, t1));
                tMax = std::min (tMax, std::max (t0, t1));
            }
        }

        return tMin <= tMax ? tMin : -1.0f;
    }

    void Fire ()
    {
        SetFlash (true);
        flashTimer = FlashTime;

        Vector3f dir = ViewDirection ();

        // the nearest obstacle in the line of fire hides everything behind it
        float wall = 1e9f;
        for (auto& b : obstacles)
        {
            float t = RayBox (position, dir, b);
            if (t >= 0.0f)
            {
                wall = std::min (wall, t);
            }
        }

        Target* hit = nullptr;
        float best = wall;
        for (auto& t : targets)
        {
            if (t.pop >= 0.0f)
            {
                continue;
            }
            float d = RaySphere (position, dir, t.position, TargetRadius);
            if (d >= 0.0f && d < best)
            {
                best = d;
                hit = &t;
            }
        }

        if (hit != nullptr)
        {
            hit->pop = 0.0f;
            score++;
            LogMessage () << "Hit! Score:" << score;
            UpdateScoreBar ();
        }
    }

    // the muzzle flash sits at the end of the barrel while shooting and behind the camera otherwise
    void SetFlash (bool on)
    {
        flash->GetModelTransform ()->Translate (0.15f, -0.12f, on ? -0.43f : 1.0f);
    }

    void UpdateScoreBar ()
    {
        for (std::size_t i = 0; i < pips.size (); i++)
        {
            // unlit pips are parked behind the camera
            float x = (static_cast<float> (i) - (ScorePips - 1) * 0.5f) * 0.014f;
            pips[i]->GetModelTransform ()->Translate (x, 0.1f, i < score % (ScorePips + 1) ? -0.3f : 1.0f);
        }
    }

    float Random (float lo, float hi)
    {
        return std::uniform_real_distribution<float> (lo, hi) (rng);
    }

    bool Blocked (float x, float z) const
    {
        for (auto& b : obstacles)
        {
            if (std::abs (x - b.x) < b.halfX + TargetRadius && std::abs (z - b.z) < b.halfZ + TargetRadius)
            {
                return true;
            }
        }
        return false;
    }

    void Respawn (Target& t)
    {
        float x, z;
        do
        {
            x = Random (-ArenaHalfSize + 2.0f, ArenaHalfSize - 2.0f);
            z = Random (-ArenaHalfSize + 2.0f, ArenaHalfSize - 2.0f);
        }
        while (Blocked (x, z) || (std::abs (x - position[0]) < 3.0f && std::abs (z - position[2]) < 3.0f));

        t.position = Vector3f (x, Random (1.0f, 3.5f), z);
        t.velocity = Vector3f (Random (-1.5f, 1.5f), Random (-0.6f, 0.6f), Random (-1.5f, 1.5f));
        t.age = 0.0f;
        t.pop = -1.0f;
        t.object->GetModelTransform ()->Translate (t.position)->Scale (TargetRadius);
    }

    void UpdateTarget (Target& t, float dt)
    {
        auto transform = t.object->GetModelTransform ();

        if (t.pop >= 0.0f)
        {
            // hit: swell and vanish, then come back somewhere else
            t.pop += dt;
            if (t.pop >= PopTime)
            {
                Respawn (t);
                return;
            }
            float u = t.pop / PopTime;
            transform->Scale (std::max (TargetRadius * (1.0f + 0.8f * u) * (1.0f - u * u), HiddenScale));
            return;
        }

        t.age += dt;
        if (t.age > TargetLifetime)
        {
            // missed for too long, move it
            Respawn (t);
            return;
        }

        Vector3f next = t.position + t.velocity * dt;

        // bounce off the walls, the floor, the ceiling and the obstacles
        float limit = ArenaHalfSize - TargetRadius;
        if (std::abs (next[0]) > limit) { t.velocity[0] = -t.velocity[0]; next[0] = t.position[0]; }
        if (std::abs (next[2]) > limit) { t.velocity[2] = -t.velocity[2]; next[2] = t.position[2]; }
        if (next[1] < 0.8f || next[1] > 4.0f) { t.velocity[1] = -t.velocity[1]; next[1] = t.position[1]; }
        if (Blocked (next[0], next[2]) && next[1] < 4.5f)
        {
            t.velocity[0] = -t.velocity[0];
            t.velocity[2] = -t.velocity[2];
            next = t.position;
        }

        t.position = next;
        transform->Translate (t.position);
    }

    std::shared_ptr<PerspectiveCamera> camera;
    std::shared_ptr<MeshObject> flash;
    std::vector<Target> targets;
    std::vector<std::shared_ptr<MeshObject>> pips;
    std::mt19937 rng;

    Vector3f position { 0.0f, EyeHeight, 9.0f };
    float yaw = 0.0f;
    float pitch = 0.0f;
    float moveForward = 0.0f;
    float moveRight = 0.0f;

    bool wantsToFire = false;
    float fireCooldown = 0.0f;
    float flashTimer = 0.0f;
    std::size_t score = 0;
};

} // namespace

int main (int argc, char* argv [])
{
    auto game = std::make_shared<Game> ();
    game->Initialize ();

    auto scene = game->Create<GameScene> ("scene");
    auto renderer = scene->Create<GLFWRenderer> (1280, 720, true, true, "Test 05 - shooting gallery", false, true, true);
    auto inputController = game->Create<GLFWInputController> ();

    renderer->Create<SurfaceRenderStage> ("hdr_postprocess")
        ->SetShaderProgram ("post_hdr_shader")
        ->SetColorAttachmentsFramebufferLink (PipelineLink (RenderStageNames::DeferredLighting));

    renderer->Create<SurfaceRenderStage> ("fxaa_postprocess")
        ->SetShaderProgram ("post_fxaa_shader")
        ->SetRenderStageParameterFloat ("fMaxSpan", 4.0f)
        ->SetRenderStageParameterVector2f ("vInvResolution", Vector2f (1.0f / renderer->GetWidth (), 1.0f / renderer->GetHeight ()))
        ->SetColorAttachmentsFramebufferLink (EPipelineLink::LINK_PREVIOUS);

    renderer->Create<SurfaceRenderStage> ("gamma_postprocess+screen")
        ->SetShaderProgram ("post_gamma_shader")
        ->SetRenderStageParameterFloat ("fGamma", 2.1f)
        ->SetColorAttachmentsFramebufferLink (EPipelineLink::LINK_PREVIOUS)
        ->SetFramebufferEnabled (false);

    inputController->CreateInputEvent ("exit", EInputKey::KeyEsc, EInputTrigger::Press, {});
    inputController->BindInputEvent ("exit", [ & ]() { game->Stop (); });

    inputController->CreateInputEvent ("mousemode", EInputKey::KeySpace, EInputTrigger::Release, {});
    inputController->BindInputEvent ("mousemode", [ & ]() { inputController->SetMouseGameMode (!inputController->IsGameMode ()); });

    // materials
    scene->Create<PBRMaterial> ("floorMaterial")
        ->SetAlbedo (Vector3f (0.25f, 0.27f, 0.3f))->SetMetallic (0.2f)->SetRoughness (0.6f);
    scene->Create<PBRMaterial> ("wallMaterial")
        ->SetAlbedo (Vector3f (0.5f, 0.5f, 0.55f))->SetMetallic (0.0f)->SetRoughness (0.9f);
    scene->Create<PBRMaterial> ("pillarMaterial")
        ->SetAlbedo (Vector3f (0.7f, 0.45f, 0.2f))->SetMetallic (0.7f)->SetRoughness (0.35f);
    scene->Create<PBRMaterial> ("crateMaterial")
        ->SetAlbedo (Vector3f (0.2f, 0.4f, 0.6f))->SetMetallic (0.1f)->SetRoughness (0.5f);
    scene->Create<PBRMaterial> ("targetMaterial")
        ->SetAlbedo (Vector3f (0.9f, 0.1f, 0.05f))->SetMetallic (0.3f)->SetRoughness (0.2f);
    scene->Create<PBRMaterial> ("gunMaterial")
        ->SetAlbedo (Vector3f (0.1f, 0.1f, 0.12f))->SetMetallic (0.9f)->SetRoughness (0.3f);
    scene->Create<PhongMaterial> ("flashMaterial")
        ->SetEmissive (Vector3f (6.0f, 4.0f, 1.5f))->SetDiffuse (Vector3f (0.0f, 0.0f, 0.0f));
    scene->Create<PhongMaterial> ("crosshairMaterial")
        ->SetEmissive (Vector3f (0.2f, 3.0f, 0.4f))->SetDiffuse (Vector3f (0.0f, 0.0f, 0.0f));
    scene->Create<PhongMaterial> ("pipMaterial")
        ->SetEmissive (Vector3f (3.0f, 2.4f, 0.2f))->SetDiffuse (Vector3f (0.0f, 0.0f, 0.0f));

    // meshes
    auto resources = game->GetResourceManager ();
    Primitives::GenerateCube (resources->Create<Mesh> ("cubeMesh"));
    Primitives::GenerateSphere (resources->Create<Mesh> ("sphereMesh"), 3);

    // arena (the cube mesh spans -1..1 in every axis)
    const float h = ArenaHalfSize;
    scene->Create<MeshObject> ("floor", "cubeMesh", "floorMaterial")
        ->GetModelTransform ()->Scale (h, 0.1f, h)->Translate (0.0f, -0.1f, 0.0f);

    const float wallHeight = 3.0f;
    scene->Create<MeshObject> ("wallN", "cubeMesh", "wallMaterial")
        ->GetModelTransform ()->Scale (h + 0.5f, wallHeight, 0.25f)->Translate (0.0f, wallHeight, -h - 0.25f);
    scene->Create<MeshObject> ("wallS", "cubeMesh", "wallMaterial")
        ->GetModelTransform ()->Scale (h + 0.5f, wallHeight, 0.25f)->Translate (0.0f, wallHeight, h + 0.25f);
    scene->Create<MeshObject> ("wallW", "cubeMesh", "wallMaterial")
        ->GetModelTransform ()->Scale (0.25f, wallHeight, h + 0.5f)->Translate (-h - 0.25f, wallHeight, 0.0f);
    scene->Create<MeshObject> ("wallE", "cubeMesh", "wallMaterial")
        ->GetModelTransform ()->Scale (0.25f, wallHeight, h + 0.5f)->Translate (h + 0.25f, wallHeight, 0.0f);

    for (std::size_t i = 0; i < obstacles.size (); i++)
    {
        const Box& b = obstacles[i];
        scene->Create<MeshObject> ("obstacle" + std::to_string (i), "cubeMesh", b.height > 2.0f ? "pillarMaterial" : "crateMaterial")
            ->GetModelTransform ()->Scale (b.halfX, b.height * 0.5f, b.halfZ)->Translate (b.x, b.height * 0.5f, b.z);
    }

    // targets
    for (std::size_t i = 0; i < TargetCount; i++)
    {
        scene->Create<MeshObject> ("target" + std::to_string (i), "sphereMesh", "targetMaterial");
    }

    // camera with a weapon, a crosshair and the score bar attached to it
    scene->Create<PerspectiveCamera> ("camera", 70.0f, 0.05f, 100.0f);
    scene->SetActiveCamera ("camera");

    scene->Create<MeshObject> ("gun", "cubeMesh", "gunMaterial")
        ->SetParentObject ("camera")
        ->GetModelTransform ()->Scale (0.012f, 0.016f, 0.1f)->Translate (0.15f, -0.13f, -0.3f);
    scene->Create<MeshObject> ("flash", "sphereMesh", "flashMaterial")
        ->SetParentObject ("camera")
        ->GetModelTransform ()->Scale (0.03f);
    scene->Create<MeshObject> ("crosshair", "sphereMesh", "crosshairMaterial")
        ->SetParentObject ("camera")
        ->GetModelTransform ()->Scale (0.0012f)->Translate (0.0f, 0.0f, -0.3f);

    for (std::size_t i = 0; i < ScorePips; i++)
    {
        float x = (static_cast<float> (i) - (ScorePips - 1) * 0.5f) * 0.014f;
        scene->Create<MeshObject> ("pip" + std::to_string (i), "cubeMesh", "pipMaterial")
            ->SetParentObject ("camera")
            ->GetModelTransform ()->Scale (0.005f)->Translate (x, 0.1f, 1.0f);
    }

    // lights
    scene->Create<DirectionalLight> ("sun")
        ->SetColor (Vector3f (1.2f, 1.1f, 1.0f))
        ->SetEnabled (true)
        ->GetModelTransform ()->Rotate (50.0f, -30.0f, 0.0f);

    scene->Create<PointLight> ("lamp")
        ->SetLinearAttenuationFactor (0.0f)
        ->SetQuadraticAttenuationFactor (0.05f)
        ->SetColor (Vector3f (8.0f, 6.0f, 4.0f))
        ->SetEnabled (true)
        ->GetModelTransform ()->Translate (0.0f, 6.0f, 0.0f);

    // game logic
    auto director = scene->Create<Director> ("director");

    director->Setup ();

    inputController->SetMouseGameMode (true);

    LogMessage () << "Shooting gallery: W/A/S/D and mouse to move, left click to shoot, Space releases the mouse, Esc quits";

    game->Run ();

    game->Deinitialize ();

    return 0;
}
