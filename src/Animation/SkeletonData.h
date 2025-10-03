#pragma once

#include <memory>
#include <string>
#include <vector>
#include "ozz/animation/runtime/skeleton.h"
#include "ozz/base/maths/soa_transform.h"

namespace ArenaFighter {

/**
 * SkeletonData - Manages skeletal hierarchy and bone data
 *
 * Wraps ozz-animation's skeleton structure and provides:
 * - Bone hierarchy management
 * - Joint name lookups
 * - Bone index queries
 * - Skeleton metadata
 */
class SkeletonData {
public:
    SkeletonData() = default;
    ~SkeletonData() = default;

    // Delete copy, allow move
    SkeletonData(const SkeletonData&) = delete;
    SkeletonData& operator=(const SkeletonData&) = delete;
    SkeletonData(SkeletonData&&) = default;
    SkeletonData& operator=(SkeletonData&&) = default;

    /**
     * Load skeleton from .ozz skeleton file
     * @param filepath Path to the .ozz skeleton file
     * @return true if loaded successfully
     */
    bool LoadFromFile(const std::string& filepath);

    /**
     * Check if skeleton is loaded and valid
     */
    bool IsValid() const { return m_skeleton != nullptr; }

    /**
     * Get the ozz skeleton (read-only)
     */
    const ozz::animation::Skeleton* GetSkeleton() const { return m_skeleton.get(); }

    /**
     * Get number of joints in the skeleton
     */
    int GetNumJoints() const;

    /**
     * Get number of SoA (Structure of Arrays) elements
     * ozz uses SoA format for SIMD optimization (4 joints per SoA element)
     */
    int GetNumSoaJoints() const;

    /**
     * Find joint index by name
     * @param jointName Name of the joint to find
     * @return Joint index, or -1 if not found
     */
    int FindJointByName(const std::string& jointName) const;

    /**
     * Get joint name by index
     * @param jointIndex Index of the joint
     * @return Joint name, or empty string if invalid index
     */
    std::string GetJointName(int jointIndex) const;

    /**
     * Get all joint names in the skeleton
     */
    std::vector<std::string> GetAllJointNames() const;

    /**
     * Get parent joint index for a given joint
     * @param jointIndex Index of the joint
     * @return Parent joint index, or -1 if root or invalid
     */
    int GetParentJoint(int jointIndex) const;

    /**
     * Check if a joint is a root joint (has no parent)
     */
    bool IsRootJoint(int jointIndex) const;

    /**
     * Get filepath of loaded skeleton
     */
    const std::string& GetFilePath() const { return m_filepath; }

private:
    std::unique_ptr<ozz::animation::Skeleton> m_skeleton;
    std::string m_filepath;
};

} // namespace ArenaFighter
