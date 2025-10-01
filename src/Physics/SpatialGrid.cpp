#include "SpatialGrid.h"
#include <algorithm>
#include <cmath>

namespace ArenaFighter {

SpatialGrid::SpatialGrid(float minX, float minY, float maxX, float maxY, float cellSize)
    : m_minX(minX)
    , m_minY(minY)
    , m_maxX(maxX)
    , m_maxY(maxY)
    , m_cellSize(cellSize) {
    
    // Calculate grid dimensions
    m_gridWidth = static_cast<int>(std::ceil((maxX - minX) / cellSize));
    m_gridHeight = static_cast<int>(std::ceil((maxY - minY) / cellSize));
}

void SpatialGrid::Clear() {
    m_cells.clear();
}

void SpatialGrid::Insert(Collider* collider) {
    if (!collider || !collider->IsActive()) return;
    
    // Get AABB of collider
    AABB aabb = collider->GetAABB();
    
    // Get all cells this collider overlaps
    auto cellKeys = GetCellKeysForAABB(aabb);
    
    // Insert into each cell
    for (const auto& key : cellKeys) {
        m_cells[key].push_back(collider);
    }
}

void SpatialGrid::Remove(Collider* collider) {
    if (!collider) return;
    
    // Get AABB of collider
    AABB aabb = collider->GetAABB();
    
    // Get all cells this collider might be in
    auto cellKeys = GetCellKeysForAABB(aabb);
    
    // Remove from each cell
    for (const auto& key : cellKeys) {
        auto it = m_cells.find(key);
        if (it != m_cells.end()) {
            auto& colliders = it->second;
            colliders.erase(
                std::remove(colliders.begin(), colliders.end(), collider),
                colliders.end()
            );
            
            // Remove empty cells
            if (colliders.empty()) {
                m_cells.erase(it);
            }
        }
    }
}

std::vector<Collider*> SpatialGrid::GetCollidersInAABB(const AABB& aabb) const {
    std::vector<Collider*> result;
    std::unordered_map<Collider*, bool> addedColliders;
    
    // Get all cells that overlap with AABB
    auto cellKeys = GetCellKeysForAABB(aabb);
    
    // Collect unique colliders from all cells
    for (const auto& key : cellKeys) {
        auto it = m_cells.find(key);
        if (it != m_cells.end()) {
            for (auto* collider : it->second) {
                if (!addedColliders[collider]) {
                    result.push_back(collider);
                    addedColliders[collider] = true;
                }
            }
        }
    }
    
    return result;
}

std::vector<Collider*> SpatialGrid::GetCollidersInRadius(const DirectX::XMFLOAT2& center, float radius) const {
    // Convert radius query to AABB query
    AABB queryAABB;
    queryAABB.min.x = center.x - radius;
    queryAABB.min.y = center.y - radius;
    queryAABB.max.x = center.x + radius;
    queryAABB.max.y = center.y + radius;
    
    // Get colliders in AABB
    auto candidates = GetCollidersInAABB(queryAABB);
    
    // Filter by actual radius
    std::vector<Collider*> result;
    float radiusSq = radius * radius;
    
    for (auto* collider : candidates) {
        AABB colliderAABB = collider->GetAABB();
        DirectX::XMFLOAT2 colliderCenter = colliderAABB.GetCenter();
        
        float dx = colliderCenter.x - center.x;
        float dy = colliderCenter.y - center.y;
        float distSq = dx * dx + dy * dy;
        
        if (distSq <= radiusSq) {
            result.push_back(collider);
        }
    }
    
    return result;
}

std::vector<std::vector<Collider*>> SpatialGrid::GetActiveCells() const {
    std::vector<std::vector<Collider*>> result;
    
    for (const auto& [key, colliders] : m_cells) {
        if (!colliders.empty()) {
            result.push_back(colliders);
        }
    }
    
    return result;
}

SpatialGrid::CellKey SpatialGrid::GetCellKey(float x, float y) const {
    CellKey key;
    key.x = static_cast<int>(std::floor((x - m_minX) / m_cellSize));
    key.y = static_cast<int>(std::floor((y - m_minY) / m_cellSize));
    return key;
}

std::vector<SpatialGrid::CellKey> SpatialGrid::GetCellKeysForAABB(const AABB& aabb) const {
    std::vector<CellKey> keys;
    
    // Get cell range
    int minCellX = static_cast<int>(std::floor((aabb.min.x - m_minX) / m_cellSize));
    int maxCellX = static_cast<int>(std::floor((aabb.max.x - m_minX) / m_cellSize));
    int minCellY = static_cast<int>(std::floor((aabb.min.y - m_minY) / m_cellSize));
    int maxCellY = static_cast<int>(std::floor((aabb.max.y - m_minY) / m_cellSize));
    
    // Clamp to grid bounds
    minCellX = std::max(0, minCellX);
    maxCellX = std::min(m_gridWidth - 1, maxCellX);
    minCellY = std::max(0, minCellY);
    maxCellY = std::min(m_gridHeight - 1, maxCellY);
    
    // Add all cells in range
    for (int y = minCellY; y <= maxCellY; ++y) {
        for (int x = minCellX; x <= maxCellX; ++x) {
            keys.push_back({x, y});
        }
    }
    
    return keys;
}

bool SpatialGrid::IsValidCell(int x, int y) const {
    return x >= 0 && x < m_gridWidth && y >= 0 && y < m_gridHeight;
}

} // namespace ArenaFighter