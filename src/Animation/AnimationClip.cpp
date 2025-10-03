#include "AnimationClip.h"
#include "SkeletonData.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include "ozz/base/io/archive.h"
#include "ozz/base/io/stream.h"

namespace ArenaFighter {

bool AnimationClip::LoadFromFile(const std::string& filepath) {
    // Open file stream
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "AnimationClip: Failed to open file: " << filepath << std::endl;
        return false;
    }

    // Create ozz stream wrapper
    ozz::io::IArchive archive(&file);
    if (!archive.TestTag<ozz::animation::Animation>()) {
        std::cerr << "AnimationClip: Invalid animation file format: " << filepath << std::endl;
        return false;
    }

    // Load animation
    m_animation = std::make_unique<ozz::animation::Animation>();
    archive >> *m_animation;

    if (!m_animation) {
        std::cerr << "AnimationClip: Failed to load animation from: " << filepath << std::endl;
        return false;
    }

    m_filepath = filepath;

    // Extract name from filepath if not set
    if (m_name.empty()) {
        size_t lastSlash = filepath.find_last_of("/\\");
        size_t lastDot = filepath.find_last_of('.');
        if (lastSlash != std::string::npos && lastDot != std::string::npos) {
            m_name = filepath.substr(lastSlash + 1, lastDot - lastSlash - 1);
        } else {
            m_name = filepath;
        }
    }

    std::cout << "AnimationClip: Loaded '" << m_name << "' ("
              << GetDuration() << "s) from: " << filepath << std::endl;

    return true;
}

float AnimationClip::GetDuration() const {
    if (!m_animation) return 0.0f;
    return m_animation->duration();
}

void AnimationClip::SetPlaybackTime(float time) {
    if (!m_animation) return;

    const float duration = GetDuration();
    if (m_loop) {
        // Wrap time for looping
        while (time < 0.0f) time += duration;
        while (time > duration) time -= duration;
    } else {
        // Clamp for non-looping
        time = std::max(0.0f, std::min(time, duration));
    }

    m_playbackTime = time;
}

float AnimationClip::GetPlaybackRatio() const {
    const float duration = GetDuration();
    if (duration <= 0.0f) return 0.0f;
    return m_playbackTime / duration;
}

void AnimationClip::SetPlaybackRatio(float ratio) {
    const float duration = GetDuration();
    SetPlaybackTime(ratio * duration);
}

void AnimationClip::Update(float deltaTime) {
    if (!m_isPlaying || !m_animation) return;

    // Update playback time
    m_playbackTime += deltaTime * m_playbackSpeed;

    // Handle looping or clamping
    const float duration = GetDuration();
    if (m_loop) {
        while (m_playbackTime >= duration) {
            m_playbackTime -= duration;
        }
        while (m_playbackTime < 0.0f) {
            m_playbackTime += duration;
        }
    } else {
        // Clamp and stop at end
        if (m_playbackTime >= duration) {
            m_playbackTime = duration;
            m_isPlaying = false;
        } else if (m_playbackTime < 0.0f) {
            m_playbackTime = 0.0f;
            if (m_playbackSpeed < 0.0f) {
                m_isPlaying = false;
            }
        }
    }
}

bool AnimationClip::Sample(const SkeletonData& skeleton,
                          ozz::span<ozz::math::SoaTransform> output) {
    if (!m_animation || !skeleton.IsValid()) {
        return false;
    }

    // Verify skeleton matches animation
    if (m_animation->num_tracks() != skeleton.GetNumJoints()) {
        std::cerr << "AnimationClip: Skeleton joint count mismatch! "
                  << "Animation: " << m_animation->num_tracks()
                  << ", Skeleton: " << skeleton.GetNumJoints() << std::endl;
        return false;
    }

    // Setup sampling job
    m_samplingJob.animation = m_animation.get();
    m_samplingJob.context = nullptr;  // Will be set by caller if needed
    m_samplingJob.ratio = GetPlaybackRatio();
    m_samplingJob.output = output;

    // Run sampling job
    if (!m_samplingJob.Run()) {
        std::cerr << "AnimationClip: Sampling job failed!" << std::endl;
        return false;
    }

    return true;
}

} // namespace ArenaFighter
