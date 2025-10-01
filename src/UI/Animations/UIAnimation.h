#pragma once
#include <functional>
#include <imgui.h>

namespace ArenaFighter {

enum class AnimationType {
    Linear,
    EaseIn,
    EaseOut,
    EaseInOut,
    Bounce,
    Elastic,
    Circular
};

enum class AnimationProperty {
    PositionX,
    PositionY,
    Width,
    Height,
    Alpha,
    Rotation,
    Scale,
    ColorR,
    ColorG,
    ColorB
};

class UIAnimation {
public:
    UIAnimation();
    virtual ~UIAnimation() = default;
    
    // Control
    void Play();
    void Pause();
    void Stop();
    void Reset();
    
    // Configuration
    void SetDuration(float duration) { m_duration = duration; }
    void SetDelay(float delay) { m_delay = delay; }
    void SetLoop(bool loop) { m_loop = loop; }
    void SetReverse(bool reverse) { m_reverse = reverse; }
    void SetAnimationType(AnimationType type) { m_animationType = type; }
    
    // State
    bool IsPlaying() const { return m_isPlaying; }
    bool IsFinished() const { return m_isFinished; }
    float GetProgress() const { return m_progress; }
    
    // Update
    virtual void Update(float deltaTime);
    
    // Callbacks
    void SetOnComplete(std::function<void()> callback) { m_onComplete = callback; }
    void SetOnUpdate(std::function<void(float)> callback) { m_onUpdate = callback; }
    
protected:
    // Override in derived classes
    virtual void ApplyAnimation(float progress) = 0;
    
    // Easing functions
    float ApplyEasing(float t);
    float Linear(float t) { return t; }
    float EaseIn(float t) { return t * t; }
    float EaseOut(float t) { return t * (2.0f - t); }
    float EaseInOut(float t);
    float Bounce(float t);
    float Elastic(float t);
    float Circular(float t);
    
protected:
    float m_duration;
    float m_delay;
    float m_elapsedTime;
    float m_progress;
    bool m_isPlaying;
    bool m_isFinished;
    bool m_loop;
    bool m_reverse;
    AnimationType m_animationType;
    
    std::function<void()> m_onComplete;
    std::function<void(float)> m_onUpdate;
};

// Property animation for animating specific values
template<typename T>
class PropertyAnimation : public UIAnimation {
public:
    PropertyAnimation(T* target, T startValue, T endValue)
        : m_target(target), m_startValue(startValue), m_endValue(endValue) {}
    
protected:
    void ApplyAnimation(float progress) override {
        if (m_target) {
            *m_target = Lerp(m_startValue, m_endValue, progress);
        }
    }
    
private:
    T* m_target;
    T m_startValue;
    T m_endValue;
    
    T Lerp(const T& a, const T& b, float t);
};

// Specializations for common types
template<>
inline float PropertyAnimation<float>::Lerp(const float& a, const float& b, float t) {
    return a + (b - a) * t;
}

template<>
inline ImVec2 PropertyAnimation<ImVec2>::Lerp(const ImVec2& a, const ImVec2& b, float t) {
    return ImVec2(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t);
}

template<>
inline ImVec4 PropertyAnimation<ImVec4>::Lerp(const ImVec4& a, const ImVec4& b, float t) {
    return ImVec4(
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t,
        a.z + (b.z - a.z) * t,
        a.w + (b.w - a.w) * t
    );
}

// Sequence animation for chaining animations
class SequenceAnimation : public UIAnimation {
public:
    SequenceAnimation();
    
    void AddAnimation(std::unique_ptr<UIAnimation> animation);
    void Update(float deltaTime) override;
    
protected:
    void ApplyAnimation(float progress) override;
    
private:
    std::vector<std::unique_ptr<UIAnimation>> m_animations;
    size_t m_currentIndex;
};

// Parallel animation for running multiple animations simultaneously
class ParallelAnimation : public UIAnimation {
public:
    ParallelAnimation();
    
    void AddAnimation(std::unique_ptr<UIAnimation> animation);
    void Update(float deltaTime) override;
    
protected:
    void ApplyAnimation(float progress) override;
    
private:
    std::vector<std::unique_ptr<UIAnimation>> m_animations;
};

} // namespace ArenaFighter