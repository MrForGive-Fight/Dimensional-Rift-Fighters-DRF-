#include "AnimationBlender.h"
#include "SkeletonData.h"
#include "AnimationClip.h"
#include <iostream>
#include <algorithm>

namespace ArenaFighter {

bool AnimationBlender::Initialize(const SkeletonData& skeleton) {
    if (!skeleton.IsValid()) {
        std::cerr << "AnimationBlender: Invalid skeleton!" << std::endl;
        return false;
    }

    m_numSoaJoints = skeleton.GetNumSoaJoints();

    // Allocate blend buffer
    m_blendBuffer.resize(m_numSoaJoints);

    std::cout << "AnimationBlender: Initialized for skeleton with "
              << skeleton.GetNumJoints() << " joints ("
              << m_numSoaJoints << " SoA)" << std::endl;

    return true;
}

int AnimationBlender::AddLayer(AnimationClip* clip, float weight) {
    if (!clip) {
        std::cerr << "AnimationBlender: Cannot add null clip!" << std::endl;
        return -1;
    }

    m_layers.emplace_back(clip, weight);
    return static_cast<int>(m_layers.size() - 1);
}

void AnimationBlender::ClearLayers() {
    m_layers.clear();
}

AnimationLayer* AnimationBlender::GetLayer(size_t index) {
    if (index >= m_layers.size()) {
        return nullptr;
    }
    return &m_layers[index];
}

void AnimationBlender::SetLayerWeight(size_t index, float weight) {
    if (index >= m_layers.size()) {
        std::cerr << "AnimationBlender: Invalid layer index: " << index << std::endl;
        return;
    }

    m_layers[index].weight = std::max(0.0f, std::min(1.0f, weight));
}

float AnimationBlender::GetLayerWeight(size_t index) const {
    if (index >= m_layers.size()) {
        return 0.0f;
    }
    return m_layers[index].weight;
}

void AnimationBlender::SetLayerJointWeights(size_t layerIndex,
                                           const std::vector<float>& jointWeights) {
    if (layerIndex >= m_layers.size()) {
        std::cerr << "AnimationBlender: Invalid layer index: " << layerIndex << std::endl;
        return;
    }

    m_layers[layerIndex].jointWeights = jointWeights;
}

bool AnimationBlender::Blend(const SkeletonData& skeleton,
                            ozz::span<ozz::math::SoaTransform> output) {
    if (!skeleton.IsValid()) {
        std::cerr << "AnimationBlender: Invalid skeleton!" << std::endl;
        return false;
    }

    if (m_layers.empty()) {
        // No layers to blend, output bind pose
        std::cerr << "AnimationBlender: No layers to blend!" << std::endl;
        return false;
    }

    // Single layer optimization
    if (m_layers.size() == 1) {
        auto& layer = m_layers[0];
        if (layer.clip && layer.clip->IsValid()) {
            return layer.clip->Sample(skeleton, output);
        }
        return false;
    }

    // Multi-layer blending
    // Build blending job layers
    std::vector<ozz::animation::BlendingJob::Layer> blendLayers;
    blendLayers.reserve(m_layers.size());

    // Temporary buffers for each layer's output
    std::vector<std::vector<ozz::math::SoaTransform>> layerOutputs(m_layers.size());

    for (size_t i = 0; i < m_layers.size(); ++i) {
        auto& layer = m_layers[i];

        if (!layer.clip || !layer.clip->IsValid()) {
            continue;
        }

        // Allocate output buffer for this layer
        layerOutputs[i].resize(m_numSoaJoints);

        // Sample animation
        if (!layer.clip->Sample(skeleton, ozz::make_span(layerOutputs[i]))) {
            std::cerr << "AnimationBlender: Failed to sample layer " << i << std::endl;
            continue;
        }

        // Create blending layer
        ozz::animation::BlendingJob::Layer blendLayer;
        blendLayer.transform = ozz::make_span(layerOutputs[i]);
        blendLayer.weight = layer.weight;

        // Set joint weights if available
        if (!layer.jointWeights.empty()) {
            blendLayer.joint_weights = ozz::make_span(layer.jointWeights);
        }

        blendLayers.push_back(blendLayer);
    }

    if (blendLayers.empty()) {
        std::cerr << "AnimationBlender: No valid layers to blend!" << std::endl;
        return false;
    }

    // Setup blending job
    m_blendingJob.layers = ozz::make_span(blendLayers);
    m_blendingJob.bind_pose = skeleton.GetSkeleton()->joint_rest_poses();
    m_blendingJob.output = output;

    // Run blending job
    if (!m_blendingJob.Run()) {
        std::cerr << "AnimationBlender: Blending job failed!" << std::endl;
        return false;
    }

    return true;
}

bool AnimationBlender::BlendTwo(const SkeletonData& skeleton,
                               AnimationClip& clipA,
                               AnimationClip& clipB,
                               float blendFactor,
                               ozz::span<ozz::math::SoaTransform> output) {
    if (!skeleton.IsValid() || !clipA.IsValid() || !clipB.IsValid()) {
        return false;
    }

    // Clamp blend factor
    blendFactor = std::max(0.0f, std::min(1.0f, blendFactor));

    const int numSoaJoints = skeleton.GetNumSoaJoints();

    // Temporary buffers
    std::vector<ozz::math::SoaTransform> bufferA(numSoaJoints);
    std::vector<ozz::math::SoaTransform> bufferB(numSoaJoints);

    // Sample both animations
    if (!clipA.Sample(skeleton, ozz::make_span(bufferA))) {
        return false;
    }

    if (!clipB.Sample(skeleton, ozz::make_span(bufferB))) {
        return false;
    }

    // Setup blending layers
    ozz::animation::BlendingJob::Layer layerA;
    layerA.transform = ozz::make_span(bufferA);
    layerA.weight = 1.0f - blendFactor;

    ozz::animation::BlendingJob::Layer layerB;
    layerB.transform = ozz::make_span(bufferB);
    layerB.weight = blendFactor;

    ozz::animation::BlendingJob::Layer layers[] = {layerA, layerB};

    // Setup and run blending job
    ozz::animation::BlendingJob blendJob;
    blendJob.layers = ozz::make_span(layers);
    blendJob.bind_pose = skeleton.GetSkeleton()->joint_rest_poses();
    blendJob.output = output;

    return blendJob.Run();
}

} // namespace ArenaFighter
