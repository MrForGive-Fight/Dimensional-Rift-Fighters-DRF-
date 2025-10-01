#pragma once

#include <vector>
#include <unordered_map>
#include <DirectXMath.h>
#include "Collider.h"

namespace ArenaFighter {

// Spatial grid for broad-phase collision detection
class SpatialGrid {
public:
    SpatialGrid(float minX, float minY, float maxX, float maxY, float cellSize);
    ~SpatialGrid() = default;
    
    // Clear all colliders from grid
    void Clear();
    
    // Insert a collider into the grid
    void Insert(Collider* collider);
    
    // Remove a collider from the grid
    void Remove(Collider* collider);
    
    // Get all colliders in cells that overlap with given AABB
    std::vector<Collider*> GetCollidersInAABB(const AABB& aabb) const;
    
    // Get all colliders within a radius
    std::vector<Collider*> GetCollidersInRadius(const DirectX::XMFLOAT2& center, float radius) const;
    
    // Get all active cells (cells containing colliders)
    std::vector<std::vector<Collider*>> GetActiveCells() const;
    
    // Debug info
    int GetCellCount() const { return m_gridWidth * m_gridHeight; }
    int GetActiveCellCount() const { return static_cast<int>(m_cells.size()); }
    
private:
    // Grid dimensions
    float m_minX, m_minY, m_maxX, m_maxY;
    float m_cellSize;
    int m_gridWidth, m_gridHeight;
    
    // Storage - using hash map for sparse grid
    struct CellKey {
        int x, y;
        
        bool operator==(const CellKey& other) const {
            return x == other.x && y == other.y;
        }
    };
    
    struct CellKeyHash {
        std::size_t operator()(const CellKey& key) const {
            return std::hash<int>()(key.x) ^ (std::hash<int>()(key.y) << 1);
        }
    };
    
    std::unordered_map<CellKey, std::vector<Collider*>, CellKeyHash> m_cells;
    
    // Helper methods
    CellKey GetCellKey(float x, float y) const;
    std::vector<CellKey> GetCellKeysForAABB(const AABB& aabb) const;
    bool IsValidCell(int x, int y) const;
};

} // namespace ArenaFighter