#include "CharacterAnimator.h"
#include "SkeletonData.h"
#include "AnimationClip.h"
#include "AnimationController.h"
#include <iostream>
#include "ozz/animation/runtime/local_to_model_job.h"

namespace ArenaFighter {

CharacterAnimator::CharacterAnimator() {
    m_skeleton = std::make_unique<SkeletonData>();
    m_controller = std::make_unique<AnimationController>();
}

CharacterAnimator::~CharacterAnimator() = default;

bool CharacterAnimator::Initialize(const std::string& skeletonPath) {
    // Load skeleton
    if (!m_skeleton->LoadFromFile(skeletonPath)) {
        std::cerr << "CharacterAnimator: Failed to load skeleton: " << skeletonPath << std::endl;
        return false;
    }

    // Initialize controller
    if (!m_controller->Initialize(*m_skeleton)) {
        std::cerr << "CharacterAnimator: Failed to initialize controller" << std::endl;
        return false;
    }

    // Allocate transform buffers
    const int numSoaJoints = m_skeleton->GetNumSoaJoints();
    const int numJoints = m_skeleton->GetNumJoints();

    m_localTransforms.resize(numSoaJoints);
    m_modelTransforms.resize(numJoints);

    m_initialized = true;

    std::cout << "CharacterAnimator: Initialized with skeleton: " << skeletonPath << std::endl;
    return true;
}

bool CharacterAnimator::LoadAnimation(const std::string& name, const std::string& filepath) {
    if (!m_initialized) {
        std::cerr << "CharacterAnimator: Not initialized!" << std::endl;
        return false;
    }

    // Check if already loaded
    if (m_animations.find(name) != m_animations.end()) {
        std::cerr << "CharacterAnimator: Animation already loaded: " << name << std::endl;
        return false;
    }

    // Create and load animation clip
    auto clip = std::make_unique<AnimationClip>();
    if (!clip->LoadFromFile(filepath)) {
        std::cerr << "CharacterAnimator: Failed to load animation: " << filepath << std::endl;
        return false;
    }

    clip->SetName(name);
    m_animations[name] = std::move(clip);

    std::cout << "CharacterAnimator: Loaded animation '" << name << "' from: " << filepath << std::endl;
    return true;
}

void CharacterAnimator::RegisterAnimationState(const std::string& stateName,
                                              const std::string& animationName,
                                              bool loop, float speed) {
    if (!m_initialized) {
        std::cerr << "CharacterAnimator: Not initialized!" << std::endl;
        return;
    }

    // Find animation
    auto it = m_animations.find(animationName);
    if (it == m_animations.end()) {
        std::cerr << "CharacterAnimator: Animation not found: " << animationName << std::endl;
        return;
    }

    // Register state with controller
    m_controller->RegisterState(stateName, it->second.get(), loop, speed);
}

void CharacterAnimator::RegisterStateTransition(const std::string& fromState,
                                               const std::string& toState,
                                               float duration,
                                               bool interruptible) {
    if (!m_initialized) {
        std::cerr << "CharacterAnimator: Not initialized!" << std::endl;
        return;
    }

    m_controller->RegisterTransition(fromState, toState, duration, interruptible);
}

void CharacterAnimator::SetDefaultState(const std::string& stateName) {
    if (!m_initialized) {
        std::cerr << "CharacterAnimator: Not initialized!" << std::endl;
        return;
    }

    m_controller->SetDefaultState(stateName);
}

void CharacterAnimator::PlayState(const std::string& stateName, bool forceRestart) {
    if (!m_initialized) {
        std::cerr << "CharacterAnimator: Not initialized!" << std::endl;
        return;
    }

    m_controller->TransitionTo(stateName, forceRestart);
}

const std::string& CharacterAnimator::GetCurrentState() const {
    return m_controller->GetCurrentState();
}

bool CharacterAnimator::IsTransitioning() const {
    return m_controller->IsTransitioning();
}

void CharacterAnimator::Update(float deltaTime) {
    if (!m_initialized) {
        return;
    }

    // Update animation controller
    m_controller->Update(deltaTime);

    // Get blended output in local space
    if (m_controller->GetOutput(ozz::make_span(m_localTransforms))) {
        // Convert local space to model space
        UpdateModelTransforms();
    }
}

bool CharacterAnimator::IsValid() const {
    return m_initialized && m_skeleton && m_skeleton->IsValid();
}

int CharacterAnimator::FindBone(const std::string& boneName) const {
    if (!m_skeleton) return -1;
    return m_skeleton->FindJointByName(boneName);
}

const ozz::math::Float4x4* CharacterAnimator::GetBoneTransform(int boneIndex) const {
    if (boneIndex < 0 || boneIndex >= static_cast<int>(m_modelTransforms.size())) {
        return nullptr;
    }
    return &m_modelTransforms[boneIndex];
}

ozz::math::Float3 CharacterAnimator::GetBonePosition(int boneIndex) const {
    const auto* transform = GetBoneTransform(boneIndex);
    if (!transform) {
        return ozz::math::Float3::zero();
    }

    // Extract position from transform matrix (column 3)
    return ozz::math::Float3(transform->cols[3].x,
                            transform->cols[3].y,
                            transform->cols[3].z);
}

std::vector<std::string> CharacterAnimator::GetLoadedAnimations() const {
    std::vector<std::string> names;
    names.reserve(m_animations.size());

    for (const auto& [name, clip] : m_animations) {
        names.push_back(name);
    }

    return names;
}

void CharacterAnimator::Reset() {
    if (!m_initialized) return;

    m_controller->Reset();
}

void CharacterAnimator::UpdateModelTransforms() {
    if (!m_skeleton || !m_skeleton->IsValid()) {
        return;
    }

    // Setup local-to-model job
    ozz::animation::LocalToModelJob localToModelJob;
    localToModelJob.skeleton = m_skeleton->GetSkeleton();
    localToModelJob.input = ozz::make_span(m_localTransforms);
    localToModelJob.output = ozz::make_span(m_modelTransforms);

    // Run conversion
    if (!localToModelJob.Run()) {
        std::cerr << "CharacterAnimator: Local-to-model conversion failed!" << std::endl;
    }
}

} // namespace ArenaFighter
