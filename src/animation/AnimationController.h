/**
 * @file AnimationController.h
 * @since 2026
 * @author Dev Patel
 *
 * Procedural animation controller for gun parts.
 * Drives slide, trigger and recoil animations in response to input.
 * Uses glm::mix for smooth interpolation between keyframes.
 *
 * Usage:
 *  Caliber::AnimationController anim(gunModel);
 *  anim.triggerFire();
 *  anim.update(deltaTime);
 */

#pragma once

#include<string>
#include<functional>
#include<glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include "renderer/Model.h"

namespace Caliber{

enum class AnimState{
    Idle,
    Fire,
    Reload,
    Inspect

};


// single animated part
struct AnimatedPart{
    MeshInstance* instance {nullptr};
    glm::mat4 baseTransform {glm::mat4(1.0f)}; // --> rest pose(idle)
    glm::mat4 targetTransform {glm::mat4(1.0f)}; // --> animated pose
    float blendWeight {0.0f}; // 0 = base , 1 = target

};


class AnimationController{
public:
    explicit AnimationController(Model& model);

    void update(float deltaTime);
    // triggers
    void triggerFire();
    void triggerReload();
    void triggerInspect();

    [[nodiscard]] AnimState getState() const { return m_state; }
    [[nodiscard]] glm::vec3 getRecoilOffset() const { return m_recoilOffset; }
    [[nodiscard]] glm::vec3 getRecoilRotation() const { return m_recoilRotation; }


private:
    Model& m_model;
    float m_stateTime {0.0f}; // time in current state
    AnimState m_state;

    // individual Parts
    AnimatedPart m_slide;
    AnimatedPart m_trigger;
    AnimatedPart m_magazine;

    // Gun recoil
    glm::vec3 m_recoilOffset {0.0f};
    glm::vec3 m_recoilRotation {0.0f};


    // animation timinigs
    static constexpr float SLIDE_BACK_TIME = 0.08f;
    static constexpr float SLIDE_RETURN_TIME = 0.15f;
    static constexpr float RECOIL_UP_TIME = 0.05f;
    static constexpr float RECOIL_DOWN_TIME = 0.20f;
    static constexpr float TRIGGER_PULL_TIME = 0.04f;
    static constexpr float TRIGGER_RESET_TIME = 0.10f;

    void updateFire(float deltaTime);
    void updateIdle(float deltaTime);
    void updateReload(float deltaTime);

    void initParts();

};




} // namespace caliber




