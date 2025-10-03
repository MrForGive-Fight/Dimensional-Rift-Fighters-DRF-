#include "AnimationController.h"
#include "SkeletonData.h"
#include "AnimationClip.h"
#include "AnimationBlender.h"
#include <iostream>
#include <algorithm>

namespace ArenaFighter {

AnimationController::AnimationController() {
    m_blender = std::make_unique<AnimationBlender>();
}

AnimationController::~AnimationController() = default;

bool AnimationController::Initialize(const SkeletonData& skeleton) {
    if (!skeleton.IsValid()) {
        std::cerr << "AnimationController: Invalid skeleton!" << std::endl;
        return false;
    }

    m_skeleton = &skeleton;

    // Initialize blender
    if (!m_blender->Initialize(skeleton)) {
        std::cerr << "AnimationController: Failed to initialize blender!" << std::endl;
        return false;
    }

    // Allocate output buffer
    m_outputBuffer.resize(skeleton.GetNumSoaJoints());

    std::cout << "AnimationController: Initialized" << std::endl;
    return true;
}

void AnimationController::RegisterState(const std::string& name, AnimationClip* clip,
                                       bool loop, float speed) {
    if (!clip) {
        std::cerr << "AnimationController: Cannot register null clip for state: " << name << std::endl;
        return;
    }

    AnimationState state(name, clip, loop, speed);
    m_states[name] = state;

    // Configure clip
    clip->SetLooping(loop);
    clip->SetPlaybackSpeed(speed);

    std::cout << "AnimationController: Registered state '" << name << "' (loop: "
              << (loop ? "yes" : "no") << ", speed: " << speed << ")" << std::endl;
}

void AnimationController::RegisterTransition(const std::string& fromState,
                                            const std::string& toState,
                                            float duration, bool interruptible) {
    // Verify states exist
    if (m_states.find(fromState) == m_states.end()) {
        std::cerr << "AnimationController: From-state not found: " << fromState << std::endl;
        return;
    }

    if (m_states.find(toState) == m_states.end()) {
        std::cerr << "AnimationController: To-state not found: " << toState << std::endl;
        return;
    }

    AnimationTransition transition(fromState, toState, duration, interruptible);
    m_transitions.push_back(transition);

    std::cout << "AnimationController: Registered transition '" << fromState
              << "' -> '" << toState << "' (" << duration << "s)" << std::endl;
}

void AnimationController::SetDefaultState(const std::string& stateName) {
    if (m_states.find(stateName) == m_states.end()) {
        std::cerr << "AnimationController: Default state not found: " << stateName << std::endl;
        return;
    }

    m_defaultStateName = stateName;

    // If no current state, set as current
    if (m_currentStateName.empty()) {
        m_currentStateName = stateName;
        auto* state = GetState(m_currentStateName);
        if (state && state->clip) {
            state->clip->Play();
        }
    }

    std::cout << "AnimationController: Set default state to '" << stateName << "'" << std::endl;
}

bool AnimationController::TransitionTo(const std::string& stateName, bool forceTransition) {
    // Check if state exists
    if (m_states.find(stateName) == m_states.end()) {
        std::cerr << "AnimationController: Target state not found: " << stateName << std::endl;
        return false;
    }

    // Already in this state and not forcing
    if (m_currentStateName == stateName && !forceTransition) {
        return false;
    }

    // Already transitioning to this state
    if (m_isTransitioning && m_targetStateName == stateName) {
        return false;
    }

    // Find transition
    auto* transition = FindTransition(m_currentStateName, stateName);

    if (transition) {
        // Check if current transition is interruptible
        if (m_isTransitioning) {
            auto* currentTransition = FindTransition(m_currentStateName, m_targetStateName);
            if (currentTransition && !currentTransition->interruptible) {
                std::cerr << "AnimationController: Cannot interrupt current transition" << std::endl;
                return false;
            }
        }

        StartTransition(stateName, transition->transitionDuration);
    } else {
        // No transition defined, use default duration
        StartTransition(stateName, 0.3f);
    }

    return true;
}

float AnimationController::GetTransitionProgress() const {
    if (!m_isTransitioning || m_transitionDuration <= 0.0f) {
        return 1.0f;
    }

    return std::min(1.0f, m_transitionTime / m_transitionDuration);
}

void AnimationController::Update(float deltaTime) {
    if (!m_skeleton) return;

    auto* currentState = GetState(m_currentStateName);
    if (!currentState || !currentState->clip) {
        return;
    }

    // Update current animation
    currentState->clip->Update(deltaTime);

    // Handle transition
    if (m_isTransitioning) {
        auto* targetState = GetState(m_targetStateName);
        if (targetState && targetState->clip) {
            // Update target animation
            targetState->clip->Update(deltaTime);

            // Update transition time
            m_transitionTime += deltaTime;

            // Check if transition complete
            if (m_transitionTime >= m_transitionDuration) {
                CompleteTransition();
            }
        } else {
            // Invalid target state, cancel transition
            m_isTransitioning = false;
            m_targetStateName.clear();
        }
    }
}

bool AnimationController::GetOutput(ozz::span<ozz::math::SoaTransform> output) {
    if (!m_skeleton) {
        std::cerr << "AnimationController: Not initialized!" << std::endl;
        return false;
    }

    auto* currentState = GetState(m_currentStateName);
    if (!currentState || !currentState->clip) {
        std::cerr << "AnimationController: No valid current state!" << std::endl;
        return false;
    }

    // If not transitioning, just sample current animation
    if (!m_isTransitioning) {
        return currentState->clip->Sample(*m_skeleton, output);
    }

    // Transitioning - blend current and target animations
    auto* targetState = GetState(m_targetStateName);
    if (!targetState || !targetState->clip) {
        // Invalid target, just use current
        return currentState->clip->Sample(*m_skeleton, output);
    }

    // Blend between current and target
    float blendFactor = GetTransitionProgress();
    return AnimationBlender::BlendTwo(*m_skeleton,
                                     *currentState->clip,
                                     *targetState->clip,
                                     blendFactor,
                                     output);
}

void AnimationController::Reset() {
    m_isTransitioning = false;
    m_targetStateName.clear();
    m_transitionTime = 0.0f;

    // Reset to default state
    if (!m_defaultStateName.empty()) {
        m_currentStateName = m_defaultStateName;
        auto* state = GetState(m_currentStateName);
        if (state && state->clip) {
            state->clip->Stop();
            state->clip->Play();
        }
    }
}

AnimationState* AnimationController::GetState(const std::string& name) {
    auto it = m_states.find(name);
    if (it != m_states.end()) {
        return &it->second;
    }
    return nullptr;
}

bool AnimationController::HasState(const std::string& name) const {
    return m_states.find(name) != m_states.end();
}

std::vector<std::string> AnimationController::GetAllStateNames() const {
    std::vector<std::string> names;
    names.reserve(m_states.size());

    for (const auto& [name, state] : m_states) {
        names.push_back(name);
    }

    return names;
}

AnimationTransition* AnimationController::FindTransition(const std::string& from,
                                                        const std::string& to) {
    for (auto& transition : m_transitions) {
        if (transition.fromState == from && transition.toState == to) {
            return &transition;
        }
    }
    return nullptr;
}

void AnimationController::StartTransition(const std::string& targetState, float duration) {
    m_isTransitioning = true;
    m_targetStateName = targetState;
    m_transitionTime = 0.0f;
    m_transitionDuration = duration;

    // Start playing target animation
    auto* state = GetState(targetState);
    if (state && state->clip) {
        state->clip->Reset();
        state->clip->Play();
    }

    std::cout << "AnimationController: Transitioning '" << m_currentStateName
              << "' -> '" << targetState << "' (" << duration << "s)" << std::endl;
}

void AnimationController::CompleteTransition() {
    // Stop old animation
    auto* oldState = GetState(m_currentStateName);
    if (oldState && oldState->clip) {
        oldState->clip->Pause();
    }

    // Switch to new state
    m_currentStateName = m_targetStateName;
    m_targetStateName.clear();
    m_isTransitioning = false;
    m_transitionTime = 0.0f;

    std::cout << "AnimationController: Transition complete, now in state '"
              << m_currentStateName << "'" << std::endl;
}

} // namespace ArenaFighter
