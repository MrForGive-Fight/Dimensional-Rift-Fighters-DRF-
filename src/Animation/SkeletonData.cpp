#include "SkeletonData.h"
#include <iostream>
#include <fstream>
#include "ozz/base/io/archive.h"
#include "ozz/base/io/stream.h"

namespace ArenaFighter {

bool SkeletonData::LoadFromFile(const std::string& filepath) {
    // Open file stream
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "SkeletonData: Failed to open file: " << filepath << std::endl;
        return false;
    }

    // Create ozz stream wrapper
    ozz::io::IArchive archive(&file);
    if (!archive.TestTag<ozz::animation::Skeleton>()) {
        std::cerr << "SkeletonData: Invalid skeleton file format: " << filepath << std::endl;
        return false;
    }

    // Load skeleton
    m_skeleton = std::make_unique<ozz::animation::Skeleton>();
    archive >> *m_skeleton;

    if (!m_skeleton) {
        std::cerr << "SkeletonData: Failed to load skeleton from: " << filepath << std::endl;
        return false;
    }

    m_filepath = filepath;
    std::cout << "SkeletonData: Loaded skeleton with " << GetNumJoints()
              << " joints from: " << filepath << std::endl;

    return true;
}

int SkeletonData::GetNumJoints() const {
    if (!m_skeleton) return 0;
    return m_skeleton->num_joints();
}

int SkeletonData::GetNumSoaJoints() const {
    if (!m_skeleton) return 0;
    return m_skeleton->num_soa_joints();
}

int SkeletonData::FindJointByName(const std::string& jointName) const {
    if (!m_skeleton) return -1;

    const auto& names = m_skeleton->joint_names();
    for (int i = 0; i < m_skeleton->num_joints(); ++i) {
        if (names[i] == jointName) {
            return i;
        }
    }

    return -1;
}

std::string SkeletonData::GetJointName(int jointIndex) const {
    if (!m_skeleton || jointIndex < 0 || jointIndex >= m_skeleton->num_joints()) {
        return "";
    }

    return m_skeleton->joint_names()[jointIndex];
}

std::vector<std::string> SkeletonData::GetAllJointNames() const {
    std::vector<std::string> names;

    if (!m_skeleton) return names;

    const auto& joint_names = m_skeleton->joint_names();
    names.reserve(joint_names.size());

    for (const auto& name : joint_names) {
        names.push_back(name);
    }

    return names;
}

int SkeletonData::GetParentJoint(int jointIndex) const {
    if (!m_skeleton || jointIndex < 0 || jointIndex >= m_skeleton->num_joints()) {
        return -1;
    }

    // ozz stores parent indices as an array
    const auto& parents = m_skeleton->joint_parents();
    return parents[jointIndex];
}

bool SkeletonData::IsRootJoint(int jointIndex) const {
    if (!m_skeleton || jointIndex < 0 || jointIndex >= m_skeleton->num_joints()) {
        return false;
    }

    // Root joints have parent index of -1 (kNoParent)
    return m_skeleton->joint_parents()[jointIndex] == ozz::animation::Skeleton::kNoParent;
}

} // namespace ArenaFighter
