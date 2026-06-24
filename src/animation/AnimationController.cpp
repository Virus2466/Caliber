#include "animation/AnimationController.h"
#include "glm/common.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/trigonometric.hpp"
#include "renderer/Model.h"
#include<iostream>
#include <glm/gtc/matrix_transform.hpp>
#include<glfw/glfw3.h>


namespace Caliber{

AnimationController::AnimationController(Model& model) : m_model(model) { initParts(); }

void AnimationController::initParts(){
    // find named mesh instances
    m_slide.instance = m_model.findMesh("Slide_Helwan_0");
    m_trigger.instance = m_model.findMesh("Trigger_Helwan_0");
    m_magazine.instance = m_model.findMesh("Magazine_Helwan_Mag_0");

    // store base transforms - idle
    if(m_slide.instance){
        m_slide.baseTransform = m_slide.instance->localTransform;
    }
    if(m_trigger.instance){
        m_trigger.baseTransform = m_trigger.instance->localTransform;
    }                                       
    if(m_magazine.instance){
        m_magazine.baseTransform = m_magazine.instance->localTransform;
    }

    std::cout << "[Anim] Slide Found:  " << (m_slide.instance != nullptr) << "\n";
    std::cout << "[Anim] Trigger Found:  " << (m_trigger.instance != nullptr) << "\n";
    std::cout << "[Anim] Magazine Found:  " << (m_magazine.instance != nullptr) << "\n";
}

void AnimationController::triggerFire() {
    if(m_state != AnimState::Idle) return;
    m_state = AnimState::Fire;
    m_stateTime = 0.0f;
    std::cout << "[Anim] Fire Triggered\n";
}

void AnimationController::triggerReload() {
    if(m_state != AnimState::Idle) return;
    m_state = AnimState::Reload;
    m_stateTime = 0.0f;
    std::cout << "[Anim] Reload Triggered\n";
}


void AnimationController::triggerInspect() {
    if(m_state != AnimState::Idle) return;
    m_state = AnimState::Inspect;
    m_stateTime = 0.0f;
    std::cout << "[Anim] Inspect Triggered\n";
}

void AnimationController::update(float deltaTime){
    m_stateTime += deltaTime;

    switch(m_state){
        case AnimState::Idle: 
            updateIdle(deltaTime); 
            break;
        case AnimState::Fire:
            updateFire(deltaTime); 
            break;

        case AnimState::Reload:
            updateReload(deltaTime);
            break;
    
        default:
            break;
    }   
}


void AnimationController::updateIdle(float deltaTime){
    // subtle breating sway - very little up/down
    float sway = sin(static_cast<float>(glfwGetTime()) * 1.5f) * 0.0005f;
    m_recoilOffset = glm::vec3(0.0f, sway,0.0f);
    m_recoilRotation = glm::vec3(0.0f);
}


void AnimationController::updateFire(float deltaTime){
    float totatFireTime = SLIDE_BACK_TIME + SLIDE_RETURN_TIME;

    // phase 1 --> slide moves back
    if(m_stateTime < SLIDE_BACK_TIME){
        float t = m_stateTime / SLIDE_BACK_TIME;
        t = t * t * (3.0f - 2.0f * t);

        if(m_slide.instance){
            glm::mat4 slideAnim = m_slide.baseTransform;
            slideAnim = glm::translate(slideAnim, glm::vec3(0.0f , glm::mix(0.0f , 0.8f , t) , 0.0f));
            m_slide.instance->localTransform = slideAnim;
        }

        if(m_trigger.instance){
            glm::mat4 trigAnim = m_trigger.baseTransform;
            trigAnim = glm::rotate(trigAnim, glm::mix(0.0f , glm::radians(-15.0f), t) , glm::vec3(1.0f,0.0f,0.0f));
            m_trigger.instance->localTransform = trigAnim;
        }

        float recoilT = m_stateTime / RECOIL_UP_TIME;
        recoilT = glm::clamp(recoilT , 0.0f , 1.0f);
        m_recoilOffset = glm::vec3(0.0f ,
                            glm::mix(0.0f, 0.2f , recoilT),
                            glm::mix(0.0f ,-0.01f,recoilT));

        m_recoilRotation = glm::vec3(glm::mix(0.0f , -5.0f, recoilT),0.0f , 0.0f);
    }
    // phase 2 - slide returns forward
    else if(m_stateTime < totatFireTime){
        float t = (m_stateTime - SLIDE_BACK_TIME) / SLIDE_RETURN_TIME;
        t = t * t * (3.0f - 2.0f * t); // smoothstep

        if(m_slide.instance){
            glm::mat4 slideAnim = m_slide.baseTransform;
            slideAnim = glm::translate(slideAnim, glm::vec3(0.0f , glm::mix(0.8f , 0.0f , t) , 0.0f));
            m_slide.instance->localTransform = slideAnim;
        }

        if(m_trigger.instance){
            glm::mat4 trigAnim = m_trigger.baseTransform;
            trigAnim = glm::rotate(trigAnim, glm::mix(-15.0f,0.0f , t) * glm::pi<float>() / 180.0f , 
            glm::vec3(1.0f , 0.0f , 0.0f));

            m_trigger.instance->localTransform = trigAnim;
        }

        float recoilT = (m_stateTime - SLIDE_BACK_TIME) / RECOIL_DOWN_TIME;
        recoilT = glm::clamp(recoilT , 0.0f , 1.0f);
        m_recoilOffset = glm::vec3(0.0f , glm::mix(0.02f, 0.0f , recoilT),
                        glm::mix(-0.01f,0.0f , recoilT));
        m_recoilRotation = glm::vec3(glm::mix(-5.0f,0.0f,recoilT),0.0f , 0.0f);
    }
    else{
        // reset everything
        if(m_slide.instance){
            m_slide.instance->localTransform = m_slide.baseTransform;
        }
        if(m_trigger.instance){
            m_trigger.instance->localTransform = m_trigger.baseTransform;
        }

        m_recoilOffset = glm::vec3(0.0f);
        m_recoilRotation = glm::vec3(0.0f);
        m_state = AnimState::Idle;
        m_stateTime = 0.0f;
    }
}


void AnimationController::updateReload([[maybe_unused]] float deltaTime){
    float totalReloadTime = 1.5f;

    // phase 1 --> magazine drops
    if(m_stateTime < 0.3f){
        float t = m_stateTime / 0.3f;
        t = t * t; // ease in

        if(m_magazine.instance){
            glm::mat4 magAnim = m_magazine.baseTransform;
            //calculate the diagonal
            float dropZ = glm::mix(0.0f, -1.5f , t);
            float slantY = glm::mix(0.0f , 0.4f , t);

            magAnim = glm::translate(magAnim,glm::vec3(0.0f , slantY , dropZ));
            m_magazine.instance->localTransform = magAnim;
        }
    }
    // phase 2 --> magainze returns
    else if(m_stateTime > 0.8f && m_stateTime < 1.2f){
        float t = (m_stateTime - 0.8f) / 0.4f;
        t = glm::clamp(t,0.0f , 1.0f);

        if(m_magazine.instance){
            glm::mat4 magAnim = m_magazine.baseTransform;
            // reverse the diagonal path
            float dropZ = glm::mix(-1.5f , 0.0f , t);
            float slantY = glm::mix(0.4f , 0.0f , t);

            magAnim = glm::translate(magAnim, glm::vec3(0.0f , slantY, dropZ));
            m_magazine.instance->localTransform = magAnim;
        }

    }
    else if(m_stateTime >= totalReloadTime){
        if(m_magazine.instance){
            m_magazine.instance->localTransform = m_magazine.baseTransform;
        }
        m_state = AnimState::Idle;
        m_stateTime = 0.0f;
    }
}


}// namespace caliber