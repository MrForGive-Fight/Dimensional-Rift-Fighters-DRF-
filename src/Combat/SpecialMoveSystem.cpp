#include "SpecialMoveSystem.h"
#include <algorithm>

namespace ArenaFighter {

SpecialMoveSystem::SpecialMoveSystem()
    : m_character(nullptr)
    , m_currentTime(0.0f) {
    m_inputBuffer.reserve(MAX_BUFFER_SIZE);
}

void SpecialMoveSystem::update(float deltaTime) {
    m_currentTime += deltaTime;
    
    // Update block state
    if (m_blockState.m_blockHeldTime > 0) {
        m_blockState.m_blockHeldTime += deltaTime;
        
        // Check if block should activate
        if (!m_blockState.m_isBlocking && 
            m_blockState.m_blockHeldTime >= BlockState::BLOCK_ACTIVATION_TIME) {
            m_blockState.m_isBlocking = true;
            m_blockState.m_canUseSpecials = false;
            
            if (m_character) {
                m_character->setBlocking(true);
            }
        }
    }
    
    // Update block stun
    if (m_blockState.m_blockStunFrames > 0) {
        m_blockState.m_blockStunFrames -= deltaTime * 60.0f; // Convert to frames
        if (m_blockState.m_blockStunFrames < 0) {
            m_blockState.m_blockStunFrames = 0;
        }
    }
    
    // Clean old inputs from buffer
    auto now = m_currentTime;
    m_inputBuffer.erase(
        std::remove_if(m_inputBuffer.begin(), m_inputBuffer.end(),
            [now](const InputBufferEntry& entry) {
                return (now - entry.m_timestamp) > INPUT_WINDOW;
            }),
        m_inputBuffer.end()
    );
}

void SpecialMoveSystem::handleSButtonPress() {
    m_blockState.m_blockStartTime = m_currentTime;
    m_blockState.m_blockHeldTime = 0.0f;
    
    // Add to input buffer
    addToInputBuffer(InputDirection::Neutral, true);
}

void SpecialMoveSystem::handleSButtonRelease() {
    // If we were blocking, stop
    if (m_blockState.m_isBlocking) {
        m_blockState.m_isBlocking = false;
        m_blockState.m_canUseSpecials = true;
        
        if (m_character) {
            m_character->setBlocking(false);
        }
    }
    
    // Reset block timer
    m_blockState.m_blockHeldTime = 0.0f;
    
    // Add to input buffer
    addToInputBuffer(InputDirection::Neutral, false);
}

void SpecialMoveSystem::handleDirectionalInput(InputDirection direction) {
    // Check if S is being held
    bool sHeld = m_blockState.m_blockHeldTime > 0 && 
                 m_blockState.m_blockHeldTime < BlockState::BLOCK_ACTIVATION_TIME;
    
    // If S is held and we haven't started blocking yet, this could be a special move
    if (sHeld && !m_blockState.m_isBlocking) {
        // Try to execute special move
        if (tryExecuteSpecialMove(direction)) {
            // Cancel block attempt since we used a special move
            m_blockState.reset();
        }
    } else if (direction == InputDirection::DownDown && sHeld) {
        // Special case for stance switch (Down+S)
        if (tryStanceSwitch()) {
            m_blockState.reset();
        }
    }
    
    // Always add to input buffer for combo detection
    addToInputBuffer(direction, sHeld);
}

bool SpecialMoveSystem::tryExecuteSpecialMove(InputDirection direction) {
    if (!canExecuteSpecialMove()) {
        return false;
    }
    
    if (!m_character) {
        return false;
    }
    
    // Get the special move for this direction
    SpecialMove* move = m_character->getSpecialMove(direction);
    if (!move) {
        return false;
    }
    
    // Check mana
    if (m_character->getCurrentMana() < move->m_manaCost) {
        return false;
    }
    
    // Execute the move
    m_character->consumeMana(static_cast<float>(move->m_manaCost));
    
    // Set character state
    m_character->startSpecialMove(
        move->m_name,
        move->m_startupFrames,
        move->m_activeFrames,
        move->m_recoveryFrames
    );
    
    // Execute callback if provided
    if (move->m_executeCallback) {
        move->m_executeCallback(m_character);
    }
    
    return true;
}

bool SpecialMoveSystem::canExecuteSpecialMove() const {
    // Can't use specials while blocking
    if (m_blockState.m_isBlocking || !m_blockState.m_canUseSpecials) {
        return false;
    }
    
    // Can't use specials during block stun
    if (m_blockState.m_blockStunFrames > 0) {
        return false;
    }
    
    // Check character state
    if (m_character) {
        // Can't use specials during hitstun, already in a move, etc
        if (m_character->isInHitstun() || 
            m_character->isPerformingMove() ||
            m_character->isKnockedDown()) {
            return false;
        }
    }
    
    return true;
}

bool SpecialMoveSystem::tryStanceSwitch() {
    if (!m_character) {
        return false;
    }
    
    // Check if character supports stance switching
    CharacterBase* stanceChar = dynamic_cast<CharacterBase*>(m_character);
    if (!stanceChar || !stanceChar->hasStanceSystem()) {
        return false;
    }
    
    // Can't switch during certain states
    if (m_character->isInHitstun() || 
        m_character->isPerformingMove() ||
        m_character->isBlocking()) {
        return false;
    }
    
    // Perform stance switch
    return stanceChar->switchStance();
}

void SpecialMoveSystem::addToInputBuffer(InputDirection direction, bool sHeld) {
    InputBufferEntry entry;
    entry.m_direction = direction;
    entry.m_sButtonHeld = sHeld;
    entry.m_timestamp = m_currentTime;
    
    m_inputBuffer.push_back(entry);
    
    // Keep buffer size limited
    if (m_inputBuffer.size() > MAX_BUFFER_SIZE) {
        m_inputBuffer.erase(m_inputBuffer.begin());
    }
}

InputDirection SpecialMoveSystem::getLastDirection() const {
    if (m_inputBuffer.empty()) {
        return InputDirection::Neutral;
    }
    
    // Find last directional input
    for (auto it = m_inputBuffer.rbegin(); it != m_inputBuffer.rend(); ++it) {
        if (it->m_direction != InputDirection::Neutral) {
            return it->m_direction;
        }
    }
    
    return InputDirection::Neutral;
}

} // namespace ArenaFighter