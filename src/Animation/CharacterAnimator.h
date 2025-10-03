#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include "ozz/base/maths/soa_transform.h"
#include "ozz/base/maths/vec_float.h"

namespace ArenaFighter {

class SkeletonData;
class AnimationClip;
class AnimationController;

/**
 * CharacterAnimator - High-level animation interface for characters
 *
 * Provides:
 * - Simple animation playback interface
 * - Animation asset management
 * - State-based animation control
 * - Integration with character systems
 */
class CharacterAnimator {
public:
    CharacterAnimator();
    ~CharacterAnimator();

    // Delete copy, allow move
    CharacterAnimator(const CharacterAnimator&) = delete;
    CharacterAnimator& operator=(const CharacterAnimator&) = delete;
    CharacterAnimator(CharacterAnimator&&) = default;
    CharacterAnimator& operator=(CharacterAnimator&&) = default;

    /**
     * Initialize animator with skeleton
     * @param skeletonPath Path to .ozz skeleton file
     * @return true if initialized successfully
     */
    bool Initialize(const std::string& skeletonPath);

    /**
     * Load animation clip
     * @param name Unique name for this animation
     * @param filepath Path to .ozz animation file
     * @return true if loaded successfully
     */
    bool LoadAnimation(const std::string& name, const std::string& filepath);

    /**
     * Register animation state for state machine
     * @param stateName State name
     * @param animationName Animation to use for this state
     * @param loop Whether to loop
     * @param speed Playback speed multiplier
     */
    void RegisterAnimationState(const std::string& stateName,
                               const std::string& animationName,
                               bool loop = true, float speed = 1.0f);

    /**
     * Register transition between animation states
     */
    void RegisterStateTransition(const std::string& fromState,
                                const std::string& toState,
                                float duration = 0.3f,
                                bool interruptible = true);

    /**
     * Set default/idle animation state
     */
    void SetDefaultState(const std::string& stateName);

    /**
     * Play animation state
     * @param stateName State to transition to
     * @param forceRestart Restart even if already playing this state
     */
    void PlayState(const std::string& stateName, bool forceRestart = false);

    /**
     * Get current animation state name
     */
    const std::string& GetCurrentState() const;

    /**
     * Check if currently transitioning between states
     */
    bool IsTransitioning() const;

    /**
     * Update animator
     * @param deltaTime Time since last update in seconds
     */
    void Update(float deltaTime);

    /**
     * Get final bone transforms in local space (SoA format)
     */
    const std::vector<ozz::math::SoaTransform>& GetLocalTransforms() const {
        return m_localTransforms;
    }

    /**
     * Get final bone transforms in model space
     * Model space = accumulated transforms from root to each bone
     */
    const std::vector<ozz::math::Float4x4>& GetModelTransforms() const {
        return m_modelTransforms;
    }

    /**
     * Get skeleton data
     */
    const SkeletonData* GetSkeleton() const { return m_skeleton.get(); }

    /**
     * Check if animator is valid and ready
     */
    bool IsValid() const;

    /**
     * Find bone index by name
     */
    int FindBone(const std::string& boneName) const;

    /**
     * Get bone transform in model space
     */
    const ozz::math::Float4x4* GetBoneTransform(int boneIndex) const;

    /**
     * Get bone position in model space
     */
    ozz::math::Float3 GetBonePosition(int boneIndex) const;

    /**
     * Get all loaded animation names
     */
    std::vector<std::string> GetLoadedAnimations() const;

    /**
     * Reset animator to default state
     */
    void Reset();

private:
    // Update model space transforms from local transforms
    void UpdateModelTransforms();

private:
    // Core animation components
    std::unique_ptr<SkeletonData> m_skeleton;
    std::unique_ptr<AnimationController> m_controller;

    // Animation clips (owned by animator)
    std::unordered_map<std::string, std::unique_ptr<AnimationClip>> m_animations;

    // Transform buffers
    std::vector<ozz::math::SoaTransform> m_localTransforms;  // Local space (SoA)
    std::vector<ozz::math::Float4x4> m_modelTransforms;      // Model space (matrices)

    bool m_initialized = false;
};

} // namespace ArenaFighter
