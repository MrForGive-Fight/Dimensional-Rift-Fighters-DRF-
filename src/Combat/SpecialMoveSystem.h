#pragma once

#include <string>
#include <functional>
#include <chrono>
#include "../Characters/CharacterBase.h"

namespace ArenaFighter {

// Input directions for special moves
enum class InputDirection {
    Neutral,    // No directional input
    Up,         // S+↑
    Down,       // S+↓ 
    Left,       // S+←
    Right,      // S+→
    DownDown    // Down+S (stance switch)
};

// Special move data structure
struct SpecialMove {
    std::string m_name;
    std::string m_description;
    InputDirection m_direction;
    int m_manaCost;              // Mana cost (no cooldown)
    float m_startupFrames;       // Frames before hitbox appears
    float m_activeFrames;        // Frames hitbox is active
    float m_recoveryFrames;      // Frames after hitbox disappears
    
    // Visual data
    std::string m_animation;
    std::string m_effect;
    std::string m_visual;
    
    // Callback for move execution
    std::function<void(CharacterBase*)> m_executeCallback;
    
    SpecialMove()
        : m_direction(InputDirection::Neutral)
        , m_manaCost(0)
        , m_startupFrames(0)
        , m_activeFrames(0)
        , m_recoveryFrames(0) {}
};

// Block state information
struct BlockState {
    bool m_isBlocking;
    float m_blockHeldTime;
    float m_blockStartTime;
    bool m_canUseSpecials;       // False while blocking
    float m_blockStunFrames;     // Frames stuck in block after being hit
    
    static constexpr float BLOCK_ACTIVATION_TIME = 1.0f; // 1 second hold to activate
    
    BlockState()
        : m_isBlocking(false)
        , m_blockHeldTime(0.0f)
        , m_blockStartTime(0.0f)
        , m_canUseSpecials(true)
        , m_blockStunFrames(0.0f) {}
        
    void reset() {
        m_isBlocking = false;
        m_blockHeldTime = 0.0f;
        m_canUseSpecials = true;
        m_blockStunFrames = 0.0f;
    }
};

// Input buffer for special move detection
struct InputBufferEntry {
    InputDirection m_direction;
    bool m_sButtonHeld;
    float m_timestamp;
};

class SpecialMoveSystem {
private:
    // Input state
    BlockState m_blockState;
    std::vector<InputBufferEntry> m_inputBuffer;
    static constexpr size_t MAX_BUFFER_SIZE = 10;
    static constexpr float INPUT_WINDOW = 0.5f; // 500ms window for inputs
    
    // Current character reference
    CharacterBase* m_character;
    
    // Timing
    float m_currentTime;
    
public:
    SpecialMoveSystem();
    ~SpecialMoveSystem() = default;
    
    // Initialize with character
    void setCharacter(CharacterBase* character) { m_character = character; }
    
    // Update system
    void update(float deltaTime);
    
    // Input handling
    void handleSButtonPress();
    void handleSButtonRelease();
    void handleDirectionalInput(InputDirection direction);
    
    // Special move execution
    bool tryExecuteSpecialMove(InputDirection direction);
    bool canExecuteSpecialMove() const;
    
    // Block system
    bool isBlocking() const { return m_blockState.m_isBlocking; }
    float getBlockHeldTime() const { return m_blockState.m_blockHeldTime; }
    void applyBlockStun(float frames) { m_blockState.m_blockStunFrames = frames; }
    
    // Stance switching (Down+S)
    bool tryStanceSwitch();
    
    // Input buffer management
    void addToInputBuffer(InputDirection direction, bool sHeld);
    void clearInputBuffer() { m_inputBuffer.clear(); }
    InputDirection getLastDirection() const;
    
    // Frame data helpers
    static float framesToSeconds(float frames) { return frames / 60.0f; }
    static float secondsToFrames(float seconds) { return seconds * 60.0f; }
};

// Special move factory for creating character-specific moves
class SpecialMoveFactory {
public:
    // Create Hyuk Woon Sung light stance moves
    static SpecialMove createSpearSeaImpact() {
        SpecialMove move;
        move.m_name = "Spear Sea Impact (5th Bond)";
        move.m_direction = InputDirection::Up;
        move.m_manaCost = 25;
        move.m_startupFrames = 15;
        move.m_activeFrames = 60;
        move.m_recoveryFrames = 20;
        move.m_animation = "SpearSeaImpact";
        move.m_effect = "Creates 10 spear geysers erupting from ground";
        move.m_visual = "Blue energy spears burst from earth like water";
        return move;
    }
    
    static SpecialMove createDivineWind() {
        SpecialMove move;
        move.m_name = "Divine Wind of the Past (3rd Bond)";
        move.m_direction = InputDirection::Right;
        move.m_manaCost = 20;
        move.m_startupFrames = 10;
        move.m_activeFrames = 120;
        move.m_recoveryFrames = 15;
        move.m_animation = "DivineWind";
        move.m_effect = "Travels forward as tornado, pulls enemies in";
        move.m_visual = "Blue wind cyclone with spear slashes visible inside";
        return move;
    }
    
    static SpecialMove createLightningStitching() {
        SpecialMove move;
        move.m_name = "Lightning Stitching Art";
        move.m_direction = InputDirection::Left;
        move.m_manaCost = 30;
        move.m_startupFrames = 8;
        move.m_activeFrames = 45;
        move.m_recoveryFrames = 18;
        move.m_animation = "LightningStitching";
        move.m_effect = "Backflips while throwing 5 energy needles";
        move.m_visual = "Blue lightning connects between needles creating a web";
        return move;
    }
    
    // Dark stance moves would go here...
};

} // namespace ArenaFighter