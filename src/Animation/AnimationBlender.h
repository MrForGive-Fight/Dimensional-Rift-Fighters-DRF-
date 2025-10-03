#pragma once

#include <memory>
#include <vector>
#include "ozz/animation/runtime/blending_job.h"
#include "ozz/base/maths/soa_transform.h"

namespace ArenaFighter {

class SkeletonData;
class AnimationClip;

/**
 * AnimationLayer - Represents a single animation layer with weight
 */
struct AnimationLayer {
    AnimationClip* clip = nullptr;
    float weight = 1.0f;

    // Joint mask for partial blending (optional)
    std::vector<float> jointWeights;

    AnimationLayer() = default;
    AnimationLayer(AnimationClip* c, float w = 1.0f) : clip(c), weight(w) {}
};

/**
 * AnimationBlender - Blends multiple animations together
 *
 * Supports:
 * - Multi-layer animation blending
 * - Smooth transitions between animations
 * - Per-joint weight masks for partial blending
 * - Additive blending
 */
class AnimationBlender {
public:
    AnimationBlender() = default;
    ~AnimationBlender() = default;

    // Delete copy, allow move
    AnimationBlender(const AnimationBlender&) = delete;
    AnimationBlender& operator=(const AnimationBlender&) = delete;
    AnimationBlender(AnimationBlender&&) = default;
    AnimationBlender& operator=(AnimationBlender&&) = default;

    /**
     * Initialize blender for a specific skeleton
     * Allocates necessary buffers
     */
    bool Initialize(const SkeletonData& skeleton);

    /**
     * Add animation layer
     * @param clip Animation clip to add
     * @param weight Blend weight (0.0 to 1.0)
     * @return Layer index
     */
    int AddLayer(AnimationClip* clip, float weight = 1.0f);

    /**
     * Remove all layers
     */
    void ClearLayers();

    /**
     * Get number of active layers
     */
    size_t GetLayerCount() const { return m_layers.size(); }

    /**
     * Get layer by index
     */
    AnimationLayer* GetLayer(size_t index);

    /**
     * Set weight for a specific layer
     */
    void SetLayerWeight(size_t index, float weight);

    /**
     * Get weight for a specific layer
     */
    float GetLayerWeight(size_t index) const;

    /**
     * Set per-joint weights for a layer (partial blending)
     * Example: Blend upper body only while lower body uses different animation
     * @param layerIndex Layer to set weights for
     * @param jointWeights Array of weights (one per joint)
     */
    void SetLayerJointWeights(size_t layerIndex, const std::vector<float>& jointWeights);

    /**
     * Blend all active layers and output result
     * @param skeleton Skeleton to blend for
     * @param output Output SoA transforms
     * @return true if blending succeeded
     */
    bool Blend(const SkeletonData& skeleton,
               ozz::span<ozz::math::SoaTransform> output);

    /**
     * Blend two animations with a blend factor
     * Convenience method for simple A-to-B transitions
     * @param skeleton Skeleton to blend for
     * @param clipA First animation
     * @param clipB Second animation
     * @param blendFactor Blend factor (0.0 = all A, 1.0 = all B)
     * @param output Output SoA transforms
     * @return true if blending succeeded
     */
    static bool BlendTwo(const SkeletonData& skeleton,
                        AnimationClip& clipA,
                        AnimationClip& clipB,
                        float blendFactor,
                        ozz::span<ozz::math::SoaTransform> output);

private:
    std::vector<AnimationLayer> m_layers;

    // Blending buffers
    std::vector<ozz::math::SoaTransform> m_blendBuffer;

    // ozz blending job
    ozz::animation::BlendingJob m_blendingJob;

    // Cached skeleton size
    int m_numSoaJoints = 0;
};

} // namespace ArenaFighter
