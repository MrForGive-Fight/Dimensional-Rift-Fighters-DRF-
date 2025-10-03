#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "ozz/base/maths/soa_transform.h"

namespace ArenaFighter {

class SkeletonData;
class AnimationClip;
class AnimationBlender;

/**
 * AnimationState - Represents a single animation state
 */
struct AnimationState {
    std::string name;
    AnimationClip* clip = nullptr;
    bool loop = true;
    float speed = 1.0f;

    AnimationState() = default;
    AnimationState(const std::string& n, AnimationClip* c, bool l = true, float s = 1.0f)
        : name(n), clip(c), loop(l), speed(s) {}
};

/**
 * AnimationTransition - Defines transition between two states
 */
struct AnimationTransition {
    std::string fromState;
    std::string toState;
    float transitionDuration = 0.3f;  // Default 300ms blend time
    bool interruptible = true;        // Can be interrupted by other transitions

    AnimationTransition() = default;
    AnimationTransition(const std::string& from, const std::string& to,
                       float duration = 0.3f, bool interrupt = true)
        : fromState(from), toState(to), transitionDuration(duration), interruptible(interrupt) {}
};

/**
 * AnimationController - State machine for animation playback
 *
 * Features:
 * - State-based animation system
 * - Smooth transitions between states
 * - Transition blending
 * - State priority system
 * - Animation events
 */
class AnimationController {
public:
    AnimationController();
    ~AnimationController();

    // Delete copy, allow move
    AnimationController(const AnimationController&) = delete;
    AnimationController& operator=(const AnimationController&) = delete;
    AnimationController(AnimationController&&) = default;
    AnimationController& operator=(AnimationController&&) = default;

    /**
     * Initialize controller with a skeleton
     */
    bool Initialize(const SkeletonData& skeleton);

    /**
     * Register an animation state
     * @param name Unique state name
     * @param clip Animation clip for this state
     * @param loop Whether to loop the animation
     * @param speed Playback speed multiplier
     */
    void RegisterState(const std::string& name, AnimationClip* clip,
                      bool loop = true, float speed = 1.0f);

    /**
     * Register a transition between states
     */
    void RegisterTransition(const std::string& fromState, const std::string& toState,
                          float duration = 0.3f, bool interruptible = true);

    /**
     * Set the default/idle state
     */
    void SetDefaultState(const std::string& stateName);

    /**
     * Transition to a new state
     * @param stateName Target state name
     * @param forceTransition Force transition even if already in this state
     * @return true if transition started
     */
    bool TransitionTo(const std::string& stateName, bool forceTransition = false);

    /**
     * Get current state name
     */
    const std::string& GetCurrentState() const { return m_currentStateName; }

    /**
     * Get target state name (if transitioning)
     */
    const std::string& GetTargetState() const { return m_targetStateName; }

    /**
     * Check if currently transitioning
     */
    bool IsTransitioning() const { return m_isTransitioning; }

    /**
     * Get transition progress (0.0 to 1.0)
     */
    float GetTransitionProgress() const;

    /**
     * Update controller
     * @param deltaTime Time since last update in seconds
     */
    void Update(float deltaTime);

    /**
     * Get final blended output
     * @param output Output SoA transforms
     * @return true if successful
     */
    bool GetOutput(ozz::span<ozz::math::SoaTransform> output);

    /**
     * Reset to default state immediately (no transition)
     */
    void Reset();

    /**
     * Get state by name
     */
    AnimationState* GetState(const std::string& name);

    /**
     * Check if state exists
     */
    bool HasState(const std::string& name) const;

    /**
     * Get all registered state names
     */
    std::vector<std::string> GetAllStateNames() const;

private:
    // Helper: Find transition between two states
    AnimationTransition* FindTransition(const std::string& from, const std::string& to);

    // Helper: Start transition
    void StartTransition(const std::string& targetState, float duration);

    // Helper: Complete transition
    void CompleteTransition();

private:
    const SkeletonData* m_skeleton = nullptr;

    // States and transitions
    std::unordered_map<std::string, AnimationState> m_states;
    std::vector<AnimationTransition> m_transitions;

    // Current state
    std::string m_currentStateName;
    std::string m_defaultStateName;

    // Transition state
    bool m_isTransitioning = false;
    std::string m_targetStateName;
    float m_transitionTime = 0.0f;
    float m_transitionDuration = 0.3f;

    // Blending
    std::unique_ptr<AnimationBlender> m_blender;

    // Output buffer
    std::vector<ozz::math::SoaTransform> m_outputBuffer;
};

} // namespace ArenaFighter
