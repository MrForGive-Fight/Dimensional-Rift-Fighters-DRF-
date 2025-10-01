#pragma once

#include "GameMode.h"
#include <queue>
#include <memory>

namespace ArenaFighter {

// Dimensional Rift difficulty levels
enum class RiftDifficulty {
    Easy,
    Normal,
    Hard,
    Nightmare,
    Chaos
};

// AI companion types
enum class CompanionType {
    Tank,      // High health, draws aggro
    Healer,    // Heals allies
    DPS,       // High damage output
    Support    // Buffs and debuffs
};

// Dungeon room types
enum class RoomType {
    Combat,     // Fight enemies
    Elite,      // Mini-boss fight
    Boss,       // Boss fight
    Treasure,   // Loot room
    Challenge,  // Special challenge
    Rest        // Healing/save point
};

// Room structure
struct DungeonRoom {
    RoomType type;
    int roomId;
    std::vector<int> connectedRooms;
    bool isCleared;
    bool isLocked;
    float difficultyMultiplier;
    std::vector<std::shared_ptr<CharacterBase>> enemies;
    std::vector<std::pair<ItemType, Vector3>> rewards;
};

// AI Companion
struct AICompanion {
    std::shared_ptr<CharacterBase> character;
    CompanionType type;
    int level;
    float trustLevel; // Affects AI effectiveness
    bool isActive;
};

// Dungeon progress
struct RiftProgress {
    int currentRoom;
    int roomsCleared;
    int enemiesDefeated;
    int treasuresFound;
    int companionsLost;
    float totalTime;
    int score;
};

// Dimensional Rift configuration
struct DimensionalRiftConfig : public MatchConfig {
    RiftDifficulty difficulty = RiftDifficulty::Normal;
    int dungeonSize = 20;              // Number of rooms
    int maxCompanions = 3;             // AI companions
    bool allowRevives = true;          // Can revive companions
    float companionReviveTime = 10.0f; // Time to revive
    bool scalingDifficulty = true;     // Difficulty increases with progress
    float lootDropRate = 0.3f;         // Chance for loot drops
};

// Dimensional Rift Mode: Dungeon crawler with AI companions
class DimensionalRiftMode : public GameMode {
private:
    DimensionalRiftConfig m_riftConfig;
    
    // Dungeon structure
    std::vector<DungeonRoom> m_dungeon;
    int m_currentRoomId;
    DungeonRoom* m_currentRoom;
    
    // Player and companions
    std::shared_ptr<CharacterBase> m_playerCharacter;
    std::vector<AICompanion> m_companions;
    std::queue<std::pair<int, float>> m_reviveQueue; // companion index, time
    
    // Progress tracking
    RiftProgress m_progress;
    std::map<int, float> m_roomClearTimes;
    
    // Enemy management
    std::vector<std::shared_ptr<CharacterBase>> m_activeEnemies;
    float m_enemySpawnTimer;
    
    // Loot system
    std::vector<std::pair<ItemType, int>> m_inventory; // item type, quantity
    float m_lootMultiplier;
    
    // AI Director
    float m_tensionLevel;
    float m_lastCombatTime;
    
    // Dungeon generation
    void generateDungeon();
    void generateRoom(DungeonRoom& room);
    void connectRooms();
    void populateRoom(DungeonRoom& room);
    
    // Room management
    void enterRoom(int roomId);
    void clearCurrentRoom();
    void unlockConnectedRooms();
    bool isRoomAccessible(int roomId) const;
    
    // Companion management
    void spawnCompanions();
    void updateCompanionAI(float deltaTime);
    void commandCompanion(int index, const Vector3& target);
    void reviveCompanion(int index);
    void upgradeCompanion(int index);
    
    // Enemy management
    void spawnEnemies(const DungeonRoom& room);
    void updateEnemyAI(float deltaTime);
    void onEnemyDefeated(std::shared_ptr<CharacterBase> enemy);
    
    // Loot system
    void dropLoot(const Vector3& position);
    void collectLoot(const std::pair<ItemType, Vector3>& loot);
    void useInventoryItem(ItemType type);
    
    // AI Director
    void updateAIDirector(float deltaTime);
    void adjustDifficulty();
    void triggerEvent();
    
    // Round management overrides
    virtual void startRound() override;
    virtual void endRound(int winnerId, WinCondition condition) override;
    virtual bool checkWinConditions() override;
    virtual int calculateRoundWinner() override;

public:
    explicit DimensionalRiftMode(const DimensionalRiftConfig& config);
    virtual ~DimensionalRiftMode() = default;
    
    // Game mode implementation
    virtual void initialize() override;
    virtual void update(float deltaTime) override;
    virtual void render() override;
    virtual void shutdown() override;
    
    // Player management
    virtual void addPlayer(std::shared_ptr<CharacterBase> character) override;
    
    // Input handling
    virtual void handleInput(int playerId, const InputCommand& input) override;
    void handleDungeonCommand(const std::string& command);
    
    // Room navigation
    void moveToRoom(int roomId);
    std::vector<int> getAvailableRooms() const;
    DungeonRoom getCurrentRoom() const { return *m_currentRoom; }
    
    // Companion interaction
    void selectCompanion(CompanionType type);
    void dismissCompanion(int index);
    std::vector<AICompanion> getCompanions() const { return m_companions; }
    AICompanion* getCompanion(int index);
    
    // Progress tracking
    RiftProgress getProgress() const { return m_progress; }
    float getCompletionPercentage() const;
    int calculateScore() const;
    
    // Inventory management
    std::vector<std::pair<ItemType, int>> getInventory() const { return m_inventory; }
    bool hasItem(ItemType type) const;
    int getItemCount(ItemType type) const;
    
    // Difficulty adjustment
    void setDifficulty(RiftDifficulty difficulty);
    RiftDifficulty getDifficulty() const { return m_riftConfig.difficulty; }
    float getDifficultyMultiplier() const;
    
    // Configuration
    void setRiftConfig(const DimensionalRiftConfig& config) { m_riftConfig = config; }
    DimensionalRiftConfig getRiftConfig() const { return m_riftConfig; }
    
    // Game mode properties
    virtual std::string getModeName() const override { return "Dimensional Rift"; }
    virtual GameModeType getModeType() const override { return GameModeType::DimensionalRift; }
    virtual bool supportsOnline() const override { return false; } // Single player
    virtual int getMinPlayers() const override { return 1; }
    virtual int getMaxPlayers() const override { return 1; }
    
    // Dungeon map
    std::vector<DungeonRoom> getDungeonLayout() const { return m_dungeon; }
    bool isDungeonComplete() const;
};

} // namespace ArenaFighter