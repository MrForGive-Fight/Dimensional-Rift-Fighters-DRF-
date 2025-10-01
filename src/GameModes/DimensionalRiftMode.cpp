#include "DimensionalRiftMode.h"
#include <algorithm>
#include <random>
#include <cmath>

namespace ArenaFighter {

DimensionalRiftMode::DimensionalRiftMode(const DimensionalRiftConfig& config)
    : GameMode(config)
    , m_riftConfig(config)
    , m_currentRoomId(0)
    , m_currentRoom(nullptr)
    , m_enemySpawnTimer(0.0f)
    , m_lootMultiplier(1.0f)
    , m_tensionLevel(0.0f)
    , m_lastCombatTime(0.0f) {
    
    // Update base config for single player
    m_config.maxPlayers = 1;
    m_config.infiniteTime = true; // No time limit in dungeons
    
    // Initialize progress
    m_progress = {0, 0, 0, 0, 0, 0.0f, 0};
}

void DimensionalRiftMode::initialize() {
    GameMode::initialize();
    
    // Generate the dungeon
    generateDungeon();
    
    // Spawn companions
    spawnCompanions();
    
    // Enter first room
    enterRoom(0);
}

void DimensionalRiftMode::update(float deltaTime) {
    GameMode::update(deltaTime);
    
    if (m_currentState != MatchState::InProgress) return;
    
    // Update progress time
    m_progress.totalTime += deltaTime;
    
    // Update AI director
    updateAIDirector(deltaTime);
    
    // Update companion AI
    updateCompanionAI(deltaTime);
    
    // Update enemy AI
    updateEnemyAI(deltaTime);
    
    // Process revive queue
    for (auto it = m_reviveQueue.begin(); it != m_reviveQueue.end();) {
        it->second -= deltaTime;
        if (it->second <= 0.0f) {
            reviveCompanion(it->first);
            it = m_reviveQueue.erase(it);
        } else {
            ++it;
        }
    }
    
    // Check room clear condition
    if (m_currentRoom && !m_currentRoom->isCleared && m_activeEnemies.empty()) {
        clearCurrentRoom();
    }
    
    // Check win/lose conditions
    if (checkWinConditions()) {
        int result = calculateRoundWinner();
        endRound(result, result > 0 ? WinCondition::Survival : WinCondition::Knockout);
    }
}

void DimensionalRiftMode::render() {
    GameMode::render();
    
    // Render dungeon-specific UI
    if (m_gameUI) {
        // Minimap
        m_gameUI->renderDungeonMap(m_dungeon, m_currentRoomId);
        
        // Companion status
        m_gameUI->renderCompanionStatus(m_companions);
        
        // Progress bar
        m_gameUI->renderProgress(m_progress);
        
        // Inventory
        m_gameUI->renderInventory(m_inventory);
    }
}

void DimensionalRiftMode::shutdown() {
    m_dungeon.clear();
    m_companions.clear();
    m_activeEnemies.clear();
    m_inventory.clear();
    m_currentRoom = nullptr;
    
    GameMode::shutdown();
}

void DimensionalRiftMode::addPlayer(std::shared_ptr<CharacterBase> character) {
    GameMode::addPlayer(character);
    m_playerCharacter = character;
}

void DimensionalRiftMode::handleInput(int playerId, const InputCommand& input) {
    // Handle special dungeon commands
    if (input.isSpecialCommand) {
        handleDungeonCommand(input.command);
        return;
    }
    
    // Normal combat input
    GameMode::handleInput(playerId, input);
}

void DimensionalRiftMode::handleDungeonCommand(const std::string& command) {
    // Parse dungeon-specific commands
    if (command.find("move_to_room") == 0) {
        int roomId = std::stoi(command.substr(12));
        moveToRoom(roomId);
    }
    else if (command.find("command_companion") == 0) {
        // Parse companion command
        // Format: "command_companion <index> <target_x> <target_y> <target_z>"
    }
    else if (command.find("use_item") == 0) {
        // Parse item usage
        // Format: "use_item <item_type>"
    }
}

void DimensionalRiftMode::generateDungeon() {
    m_dungeon.clear();
    m_dungeon.reserve(m_riftConfig.dungeonSize);
    
    // Create rooms
    for (int i = 0; i < m_riftConfig.dungeonSize; ++i) {
        DungeonRoom room;
        room.roomId = i;
        room.isCleared = false;
        room.isLocked = (i > 0); // First room unlocked
        room.difficultyMultiplier = 1.0f + (i * 0.1f); // Progressive difficulty
        
        // Assign room types
        if (i == 0) {
            room.type = RoomType::Rest; // Starting room
        }
        else if (i == m_riftConfig.dungeonSize - 1) {
            room.type = RoomType::Boss; // Final boss
        }
        else if (i % 5 == 0) {
            room.type = RoomType::Elite; // Mini-boss every 5 rooms
        }
        else if (i % 7 == 0) {
            room.type = RoomType::Treasure;
        }
        else if (i % 11 == 0) {
            room.type = RoomType::Rest;
        }
        else {
            // Random between combat and challenge
            room.type = (rand() % 3 == 0) ? RoomType::Challenge : RoomType::Combat;
        }
        
        m_dungeon.push_back(room);
    }
    
    // Connect rooms
    connectRooms();
    
    // Populate rooms with content
    for (auto& room : m_dungeon) {
        generateRoom(room);
    }
}

void DimensionalRiftMode::generateRoom(DungeonRoom& room) {
    populateRoom(room);
    
    // Add rewards based on room type
    switch (room.type) {
        case RoomType::Treasure:
            room.rewards.push_back({ItemType::HealthRestore, Vector3(0, 0, 0)});
            room.rewards.push_back({ItemType::ManaRestore, Vector3(5, 0, 0)});
            if (rand() % 2 == 0) {
                room.rewards.push_back({ItemType::DamageBoost, Vector3(-5, 0, 0)});
            }
            break;
            
        case RoomType::Elite:
        case RoomType::Boss:
            room.rewards.push_back({ItemType::InstantUltimate, Vector3(0, 0, 0)});
            break;
            
        default:
            // Random chance for loot
            if (static_cast<float>(rand()) / RAND_MAX < m_riftConfig.lootDropRate) {
                room.rewards.push_back({ItemType::HealthRestore, Vector3(0, 0, 0)});
            }
            break;
    }
}

void DimensionalRiftMode::connectRooms() {
    // Create a branching path structure
    for (int i = 0; i < m_dungeon.size(); ++i) {
        // Always connect to next room (main path)
        if (i < m_dungeon.size() - 1) {
            m_dungeon[i].connectedRooms.push_back(i + 1);
            m_dungeon[i + 1].connectedRooms.push_back(i);
        }
        
        // Add branching paths
        if (i > 0 && i < m_dungeon.size() - 2) {
            // 30% chance for additional connection
            if (rand() % 100 < 30) {
                int targetRoom = i + 2 + (rand() % 3);
                if (targetRoom < m_dungeon.size()) {
                    m_dungeon[i].connectedRooms.push_back(targetRoom);
                    m_dungeon[targetRoom].connectedRooms.push_back(i);
                }
            }
        }
    }
}

void DimensionalRiftMode::populateRoom(DungeonRoom& room) {
    int enemyCount = 0;
    
    switch (room.type) {
        case RoomType::Combat:
            enemyCount = 3 + (rand() % 3); // 3-5 enemies
            break;
            
        case RoomType::Challenge:
            enemyCount = 4 + (rand() % 3); // 4-6 enemies
            break;
            
        case RoomType::Elite:
            enemyCount = 1; // 1 elite enemy
            break;
            
        case RoomType::Boss:
            enemyCount = 1; // 1 boss
            break;
            
        case RoomType::Rest:
        case RoomType::Treasure:
            enemyCount = 0; // No enemies
            break;
    }
    
    // Difficulty scaling
    float diffMult = getDifficultyMultiplier() * room.difficultyMultiplier;
    
    // Note: Actual enemy creation would happen when entering the room
    // This just sets up the room configuration
}

void DimensionalRiftMode::enterRoom(int roomId) {
    if (roomId < 0 || roomId >= m_dungeon.size()) return;
    if (!isRoomAccessible(roomId)) return;
    
    // Clear previous room enemies
    m_activeEnemies.clear();
    
    // Set current room
    m_currentRoomId = roomId;
    m_currentRoom = &m_dungeon[roomId];
    m_progress.currentRoom = roomId;
    
    // Spawn enemies for this room
    if (!m_currentRoom->isCleared) {
        spawnEnemies(*m_currentRoom);
    }
    
    // Reset positions
    if (m_playerCharacter) {
        m_playerCharacter->setPosition(Vector3(0, 0, -20)); // Start position
    }
    
    // Position companions
    float companionSpacing = 5.0f;
    for (int i = 0; i < m_companions.size(); ++i) {
        if (m_companions[i].isActive && m_companions[i].character) {
            float xOffset = (i - m_companions.size() / 2.0f) * companionSpacing;
            m_companions[i].character->setPosition(Vector3(xOffset, 0, -25));
        }
    }
    
    // Notify UI
    if (m_gameUI) {
        m_gameUI->onRoomEntered(roomId, m_currentRoom->type);
    }
}

void DimensionalRiftMode::clearCurrentRoom() {
    if (!m_currentRoom) return;
    
    m_currentRoom->isCleared = true;
    m_progress.roomsCleared++;
    
    // Record clear time
    m_roomClearTimes[m_currentRoomId] = m_progress.totalTime;
    
    // Unlock connected rooms
    unlockConnectedRooms();
    
    // Drop rewards
    for (const auto& reward : m_currentRoom->rewards) {
        m_inventory.push_back({reward.first, 1});
    }
    
    // Update score
    m_progress.score += calculateRoomScore(*m_currentRoom);
    
    // Notify UI
    if (m_gameUI) {
        m_gameUI->onRoomCleared(m_currentRoomId);
    }
}

void DimensionalRiftMode::unlockConnectedRooms() {
    if (!m_currentRoom) return;
    
    for (int connectedId : m_currentRoom->connectedRooms) {
        if (connectedId >= 0 && connectedId < m_dungeon.size()) {
            m_dungeon[connectedId].isLocked = false;
        }
    }
}

bool DimensionalRiftMode::isRoomAccessible(int roomId) const {
    if (roomId < 0 || roomId >= m_dungeon.size()) return false;
    
    const auto& room = m_dungeon[roomId];
    if (room.isLocked) return false;
    
    // Check if connected to current room
    if (m_currentRoom) {
        auto& connected = m_currentRoom->connectedRooms;
        return std::find(connected.begin(), connected.end(), roomId) != connected.end();
    }
    
    return roomId == 0; // Only first room accessible initially
}

void DimensionalRiftMode::spawnCompanions() {
    m_companions.clear();
    
    // Create default companions based on configuration
    for (int i = 0; i < std::min(3, m_riftConfig.maxCompanions); ++i) {
        AICompanion companion;
        companion.level = 1;
        companion.trustLevel = 0.5f; // Neutral trust
        companion.isActive = true;
        
        // Assign types in order: Tank, Healer, DPS
        companion.type = static_cast<CompanionType>(i % 4);
        
        // Create character for companion
        // Note: This would use the CharacterFactory in real implementation
        companion.character = std::make_shared<CharacterBase>();
        
        // Set companion stats based on type
        auto stats = companion.character->getStats();
        switch (companion.type) {
            case CompanionType::Tank:
                stats.maxHealth *= 2.0f;
                stats.defense *= 1.5f;
                break;
                
            case CompanionType::Healer:
                stats.maxMana *= 1.5f;
                stats.manaRegen *= 2.0f;
                break;
                
            case CompanionType::DPS:
                stats.baseDamage *= 1.5f;
                stats.attackSpeed *= 1.2f;
                break;
                
            case CompanionType::Support:
                stats.maxMana *= 1.3f;
                stats.speed *= 1.2f;
                break;
        }
        companion.character->setStats(stats);
        
        m_companions.push_back(companion);
    }
}

void DimensionalRiftMode::updateCompanionAI(float deltaTime) {
    for (auto& companion : m_companions) {
        if (!companion.isActive || !companion.character || companion.character->isDead()) {
            continue;
        }
        
        // AI behavior based on type
        switch (companion.type) {
            case CompanionType::Tank:
                // Find closest enemy and engage
                if (!m_activeEnemies.empty()) {
                    auto target = m_activeEnemies[0];
                    companion.character->moveTowards(target->getPosition());
                    if (companion.character->isInRange(target.get())) {
                        companion.character->performAutoAttack(target->getPosition());
                    }
                }
                break;
                
            case CompanionType::Healer:
                // Heal lowest health ally
                {
                    std::shared_ptr<CharacterBase> lowestAlly = m_playerCharacter;
                    float lowestHealth = m_playerCharacter ? m_playerCharacter->getHealthPercentage() : 100.0f;
                    
                    for (const auto& other : m_companions) {
                        if (other.character && other.character->getHealthPercentage() < lowestHealth) {
                            lowestAlly = other.character;
                            lowestHealth = other.character->getHealthPercentage();
                        }
                    }
                    
                    if (lowestHealth < 0.7f && companion.character->getMana() >= 20.0f) {
                        // Cast heal (simplified)
                        companion.character->useMana(20.0f);
                        lowestAlly->heal(100.0f);
                    }
                }
                break;
                
            case CompanionType::DPS:
                // Attack highest priority target
                if (!m_activeEnemies.empty()) {
                    // Find elite/boss first, otherwise closest
                    auto target = m_activeEnemies[0];
                    companion.character->moveTowards(target->getPosition());
                    if (companion.character->isInRange(target.get())) {
                        companion.character->performAutoAttack(target->getPosition());
                    }
                }
                break;
                
            case CompanionType::Support:
                // Stay near player and buff/debuff
                if (m_playerCharacter) {
                    Vector3 playerPos = m_playerCharacter->getPosition();
                    float distance = companion.character->getDistanceTo(m_playerCharacter.get());
                    
                    if (distance > 10.0f) {
                        companion.character->moveTowards(playerPos);
                    }
                    
                    // Apply buffs when available
                    if (companion.character->getMana() >= 30.0f) {
                        companion.character->useMana(30.0f);
                        m_playerCharacter->applyBuff(BuffType::Damage, 1.2f, 10.0f);
                    }
                }
                break;
        }
        
        // Update trust level based on performance
        companion.trustLevel = std::min(1.0f, companion.trustLevel + deltaTime * 0.01f);
    }
}

void DimensionalRiftMode::commandCompanion(int index, const Vector3& target) {
    if (index >= 0 && index < m_companions.size()) {
        if (m_companions[index].isActive && m_companions[index].character) {
            // Override AI with manual command
            m_companions[index].character->moveTowards(target);
        }
    }
}

void DimensionalRiftMode::reviveCompanion(int index) {
    if (index >= 0 && index < m_companions.size()) {
        auto& companion = m_companions[index];
        if (companion.character) {
            companion.character->respawn();
            companion.isActive = true;
            
            // Position near player
            if (m_playerCharacter) {
                Vector3 playerPos = m_playerCharacter->getPosition();
                companion.character->setPosition(playerPos + Vector3(5, 0, 0));
            }
        }
    }
}

void DimensionalRiftMode::upgradeCompanion(int index) {
    if (index >= 0 && index < m_companions.size()) {
        auto& companion = m_companions[index];
        companion.level++;
        
        // Improve stats
        if (companion.character) {
            auto stats = companion.character->getStats();
            stats.maxHealth *= 1.1f;
            stats.baseDamage *= 1.1f;
            stats.defense *= 1.05f;
            companion.character->setStats(stats);
        }
    }
}

void DimensionalRiftMode::spawnEnemies(const DungeonRoom& room) {
    m_activeEnemies.clear();
    
    // Create enemies based on room configuration
    int enemyCount = 0;
    bool isElite = false;
    
    switch (room.type) {
        case RoomType::Combat:
            enemyCount = 3 + (rand() % 3);
            break;
            
        case RoomType::Challenge:
            enemyCount = 4 + (rand() % 3);
            break;
            
        case RoomType::Elite:
            enemyCount = 1;
            isElite = true;
            break;
            
        case RoomType::Boss:
            enemyCount = 1;
            isElite = true; // Boss is super-elite
            break;
            
        default:
            return; // No enemies
    }
    
    // Spawn enemies
    for (int i = 0; i < enemyCount; ++i) {
        auto enemy = std::make_shared<CharacterBase>();
        
        // Set enemy stats based on difficulty
        auto stats = enemy->getStats();
        float diffMult = getDifficultyMultiplier() * room.difficultyMultiplier;
        
        stats.maxHealth *= diffMult;
        stats.currentHealth = stats.maxHealth;
        stats.baseDamage *= diffMult;
        stats.defense *= (0.8f + 0.2f * diffMult); // Less defense scaling
        
        if (isElite) {
            stats.maxHealth *= 3.0f;
            stats.baseDamage *= 1.5f;
            enemy->setScale(1.5f);
        }
        
        if (room.type == RoomType::Boss) {
            stats.maxHealth *= 5.0f;
            stats.baseDamage *= 2.0f;
            enemy->setScale(2.0f);
        }
        
        enemy->setStats(stats);
        
        // Position enemies
        float angle = (2.0f * 3.14159f * i) / enemyCount;
        float radius = 10.0f;
        enemy->setPosition(Vector3(cos(angle) * radius, 0, sin(angle) * radius));
        
        m_activeEnemies.push_back(enemy);
    }
}

void DimensionalRiftMode::updateEnemyAI(float deltaTime) {
    for (auto it = m_activeEnemies.begin(); it != m_activeEnemies.end();) {
        auto& enemy = *it;
        
        if (enemy->isDead()) {
            onEnemyDefeated(enemy);
            it = m_activeEnemies.erase(it);
            continue;
        }
        
        // Simple enemy AI - attack closest target
        std::shared_ptr<CharacterBase> closestTarget = m_playerCharacter;
        float minDistance = enemy->getDistanceTo(m_playerCharacter.get());
        
        // Check companions too
        for (const auto& companion : m_companions) {
            if (companion.isActive && companion.character && !companion.character->isDead()) {
                float dist = enemy->getDistanceTo(companion.character.get());
                if (dist < minDistance) {
                    minDistance = dist;
                    closestTarget = companion.character;
                }
            }
        }
        
        // Move and attack
        if (closestTarget) {
            enemy->moveTowards(closestTarget->getPosition());
            if (enemy->isInRange(closestTarget.get())) {
                enemy->performAutoAttack(closestTarget->getPosition());
            }
        }
        
        ++it;
    }
}

void DimensionalRiftMode::onEnemyDefeated(std::shared_ptr<CharacterBase> enemy) {
    m_progress.enemiesDefeated++;
    
    // Drop loot
    if (static_cast<float>(rand()) / RAND_MAX < m_riftConfig.lootDropRate * m_lootMultiplier) {
        dropLoot(enemy->getPosition());
    }
    
    // Update combat timing
    m_lastCombatTime = m_progress.totalTime;
}

void DimensionalRiftMode::dropLoot(const Vector3& position) {
    // Random loot type
    int typeIndex = rand() % 8;
    ItemType lootType = static_cast<ItemType>(typeIndex);
    
    if (m_currentRoom) {
        m_currentRoom->rewards.push_back({lootType, position});
    }
}

void DimensionalRiftMode::collectLoot(const std::pair<ItemType, Vector3>& loot) {
    // Add to inventory
    bool found = false;
    for (auto& item : m_inventory) {
        if (item.first == loot.first) {
            item.second++;
            found = true;
            break;
        }
    }
    
    if (!found) {
        m_inventory.push_back({loot.first, 1});
    }
    
    m_progress.treasuresFound++;
}

void DimensionalRiftMode::useInventoryItem(ItemType type) {
    auto it = std::find_if(m_inventory.begin(), m_inventory.end(),
        [type](const auto& item) { return item.first == type; });
    
    if (it != m_inventory.end() && it->second > 0) {
        // Apply item effect to player
        if (m_playerCharacter) {
            // Similar to item effects in DeathMatchMode
            applyItemEffect(m_playerCharacter->getId(), type);
        }
        
        it->second--;
        if (it->second == 0) {
            m_inventory.erase(it);
        }
    }
}

void DimensionalRiftMode::updateAIDirector(float deltaTime) {
    // Calculate tension based on combat frequency
    float timeSinceCombat = m_progress.totalTime - m_lastCombatTime;
    
    if (m_activeEnemies.empty()) {
        m_tensionLevel -= deltaTime * 0.1f; // Decrease during peace
    } else {
        m_tensionLevel += deltaTime * 0.2f; // Increase during combat
    }
    
    m_tensionLevel = std::max(0.0f, std::min(1.0f, m_tensionLevel));
    
    // Adjust difficulty based on player performance
    if (m_riftConfig.scalingDifficulty) {
        adjustDifficulty();
    }
    
    // Trigger events based on tension
    if (m_tensionLevel > 0.8f && timeSinceCombat > 30.0f) {
        triggerEvent();
    }
}

void DimensionalRiftMode::adjustDifficulty() {
    // Calculate performance metrics
    float clearRate = m_progress.roomsCleared / std::max(1.0f, m_progress.totalTime / 60.0f);
    float deathRate = m_progress.companionsLost / std::max(1, m_progress.roomsCleared);
    
    // Adjust enemy stats for future rooms
    if (clearRate > 1.0f && deathRate < 0.5f) {
        // Player doing well, increase difficulty
        for (int i = m_currentRoomId + 1; i < m_dungeon.size(); ++i) {
            m_dungeon[i].difficultyMultiplier *= 1.1f;
        }
    } else if (clearRate < 0.5f || deathRate > 1.0f) {
        // Player struggling, decrease difficulty
        for (int i = m_currentRoomId + 1; i < m_dungeon.size(); ++i) {
            m_dungeon[i].difficultyMultiplier *= 0.9f;
        }
    }
}

void DimensionalRiftMode::triggerEvent() {
    // Random events to keep gameplay interesting
    int eventType = rand() % 3;
    
    switch (eventType) {
        case 0:
            // Ambush - spawn extra enemies
            if (m_currentRoom && m_currentRoom->type != RoomType::Rest) {
                spawnEnemies(*m_currentRoom);
            }
            break;
            
        case 1:
            // Treasure goblin - special enemy that drops loot
            // Implementation would spawn a special fast enemy
            break;
            
        case 2:
            // Environmental hazard
            // Implementation would create temporary hazards
            break;
    }
    
    m_tensionLevel = 0.5f; // Reset tension after event
}

void DimensionalRiftMode::startRound() {
    GameMode::startRound();
    
    // Reset progress
    m_progress = {0, 0, 0, 0, 0, 0.0f, 0};
    m_roomClearTimes.clear();
    
    // Regenerate dungeon for new run
    generateDungeon();
    spawnCompanions();
    
    // Start in first room
    enterRoom(0);
}

void DimensionalRiftMode::endRound(int winnerId, WinCondition condition) {
    // Calculate final score
    m_progress.score = calculateScore();
    
    GameMode::endRound(winnerId, condition);
}

bool DimensionalRiftMode::checkWinConditions() {
    // Win: Complete the dungeon (clear boss room)
    if (m_currentRoomId == m_dungeon.size() - 1 && m_currentRoom && m_currentRoom->isCleared) {
        return true;
    }
    
    // Lose: Player dies
    if (m_playerCharacter && m_playerCharacter->isDead()) {
        // Check if all companions are also dead
        bool anyAlive = false;
        for (const auto& companion : m_companions) {
            if (companion.isActive && companion.character && !companion.character->isDead()) {
                anyAlive = true;
                break;
            }
        }
        
        if (!anyAlive) {
            return true; // Total party wipe
        }
    }
    
    return false;
}

int DimensionalRiftMode::calculateRoundWinner() {
    // Check if dungeon was completed
    if (isDungeonComplete()) {
        return m_playerCharacter ? m_playerCharacter->getId() : 1;
    }
    
    // Player lost
    return -1;
}

void DimensionalRiftMode::moveToRoom(int roomId) {
    if (isRoomAccessible(roomId)) {
        enterRoom(roomId);
    }
}

std::vector<int> DimensionalRiftMode::getAvailableRooms() const {
    std::vector<int> available;
    
    if (m_currentRoom) {
        for (int roomId : m_currentRoom->connectedRooms) {
            if (!m_dungeon[roomId].isLocked) {
                available.push_back(roomId);
            }
        }
    }
    
    return available;
}

void DimensionalRiftMode::selectCompanion(CompanionType type) {
    // Find companion of this type
    for (auto& companion : m_companions) {
        if (companion.type == type) {
            companion.isActive = true;
            break;
        }
    }
}

void DimensionalRiftMode::dismissCompanion(int index) {
    if (index >= 0 && index < m_companions.size()) {
        m_companions[index].isActive = false;
    }
}

AICompanion* DimensionalRiftMode::getCompanion(int index) {
    if (index >= 0 && index < m_companions.size()) {
        return &m_companions[index];
    }
    return nullptr;
}

float DimensionalRiftMode::getCompletionPercentage() const {
    return (float)m_progress.roomsCleared / m_dungeon.size() * 100.0f;
}

int DimensionalRiftMode::calculateScore() const {
    int score = 0;
    
    // Base score for rooms cleared
    score += m_progress.roomsCleared * 1000;
    
    // Bonus for enemies defeated
    score += m_progress.enemiesDefeated * 50;
    
    // Bonus for treasures
    score += m_progress.treasuresFound * 200;
    
    // Time bonus (faster = better)
    float timeMinutes = m_progress.totalTime / 60.0f;
    if (timeMinutes < 30.0f) {
        score += static_cast<int>((30.0f - timeMinutes) * 100);
    }
    
    // Penalty for companion losses
    score -= m_progress.companionsLost * 500;
    
    // Difficulty multiplier
    score = static_cast<int>(score * getDifficultyMultiplier());
    
    return std::max(0, score);
}

void DimensionalRiftMode::setDifficulty(RiftDifficulty difficulty) {
    m_riftConfig.difficulty = difficulty;
}

float DimensionalRiftMode::getDifficultyMultiplier() const {
    switch (m_riftConfig.difficulty) {
        case RiftDifficulty::Easy:
            return 0.5f;
        case RiftDifficulty::Normal:
            return 1.0f;
        case RiftDifficulty::Hard:
            return 1.5f;
        case RiftDifficulty::Nightmare:
            return 2.0f;
        case RiftDifficulty::Chaos:
            return 3.0f;
    }
    return 1.0f;
}

bool DimensionalRiftMode::hasItem(ItemType type) const {
    return std::any_of(m_inventory.begin(), m_inventory.end(),
        [type](const auto& item) { return item.first == type && item.second > 0; });
}

int DimensionalRiftMode::getItemCount(ItemType type) const {
    auto it = std::find_if(m_inventory.begin(), m_inventory.end(),
        [type](const auto& item) { return item.first == type; });
    
    return it != m_inventory.end() ? it->second : 0;
}

bool DimensionalRiftMode::isDungeonComplete() const {
    // Check if boss room is cleared
    if (m_dungeon.size() > 0) {
        return m_dungeon.back().isCleared;
    }
    return false;
}

int DimensionalRiftMode::calculateRoomScore(const DungeonRoom& room) const {
    int score = 100; // Base score
    
    switch (room.type) {
        case RoomType::Combat:
            score = 100;
            break;
        case RoomType::Challenge:
            score = 150;
            break;
        case RoomType::Elite:
            score = 300;
            break;
        case RoomType::Boss:
            score = 1000;
            break;
        case RoomType::Treasure:
            score = 50;
            break;
        case RoomType::Rest:
            score = 25;
            break;
    }
    
    // Apply difficulty multiplier
    return static_cast<int>(score * room.difficultyMultiplier);
}

} // namespace ArenaFighter