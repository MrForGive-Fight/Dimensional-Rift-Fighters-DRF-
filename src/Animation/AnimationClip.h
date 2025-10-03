#pragma once

#include <memory>
#include <string>
#include "ozz/animation/runtime/animation.h"
#include "ozz/animation/runtime/sampling_job.h"
#include "ozz/base/maths/soa_transform.h"

namespace ArenaFighter {

class SkeletonData;

/**
 * AnimationClip - Represents a single animation sequence
 *
 * Manages animation data and playback state:
 * - Animation asset loading (.ozz files)
 * - Playback control (play, pause, stop, loop)
 * - Time tracking and normalization
 * - Sampling animations at specific times
 */
class AnimationClip {
public:
    AnimationClip() = default;
    ~AnimationClip() = default;

    // Delete copy, allow move
    AnimationClip(const AnimationClip&) = delete;
    AnimationClip& operator=(const AnimationClip&) = delete;
    AnimationClip(AnimationClip&&) = default;
    AnimationClip& operator=(AnimationClip&&) = default;

    /**
     * Load animation from .ozz animation file
     * @param filepath Path to the .ozz animation file
     * @return true if loaded successfully
     */
    bool LoadFromFile(const std::string& filepath);

    /**
     * Check if animation is loaded and valid
     */
    bool IsValid() const { return m_animation != nullptr; }

    /**
     * Get the ozz animation (read-only)
     */
    const ozz::animation::Animation* GetAnimation() const { return m_animation.get(); }

    /**
     * Get animation duration in seconds
     */
    float GetDuration() const;

    /**
     * Get current playback time in seconds
     */
    float GetPlaybackTime() const { return m_playbackTime; }

    /**
     * Set playback time in seconds (clamped to [0, duration])
     */
    void SetPlaybackTime(float time);

    /**
     * Get normalized playback ratio (0.0 to 1.0)
     */
    float GetPlaybackRatio() const;

    /**
     * Set normalized playback ratio (0.0 to 1.0)
     */
    void SetPlaybackRatio(float ratio);

    /**
     * Update animation playback
     * @param deltaTime Time since last update in seconds
     */
    void Update(float deltaTime);

    /**
     * Sample animation at current time
     * @param skeleton Skeleton to sample for
     * @param output Output SoA transforms (must be correctly sized)
     * @return true if sampling succeeded
     */
    bool Sample(const SkeletonData& skeleton,
                ozz::span<ozz::math::SoaTransform> output);

    // Playback control
    void Play() { m_isPlaying = true; }
    void Pause() { m_isPlaying = false; }
    void Stop() { m_isPlaying = false; m_playbackTime = 0.0f; }
    void Reset() { m_playbackTime = 0.0f; }

    bool IsPlaying() const { return m_isPlaying; }
    bool IsLooping() const { return m_loop; }

    void SetLooping(bool loop) { m_loop = loop; }
    void SetPlaybackSpeed(float speed) { m_playbackSpeed = speed; }
    float GetPlaybackSpeed() const { return m_playbackSpeed; }

    /**
     * Get filepath of loaded animation
     */
    const std::string& GetFilePath() const { return m_filepath; }

    /**
     * Get name of the animation (derived from filename)
     */
    const std::string& GetName() const { return m_name; }

    /**
     * Set custom name for the animation
     */
    void SetName(const std::string& name) { m_name = name; }

private:
    std::unique_ptr<ozz::animation::Animation> m_animation;
    std::string m_filepath;
    std::string m_name;

    // Playback state
    float m_playbackTime = 0.0f;
    float m_playbackSpeed = 1.0f;
    bool m_isPlaying = false;
    bool m_loop = true;

    // Sampling job (reused for performance)
    ozz::animation::SamplingJob m_samplingJob;
};

} // namespace ArenaFighter
