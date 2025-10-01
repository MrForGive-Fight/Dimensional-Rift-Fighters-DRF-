#include "TrainingMode.h"
#include <sstream>
#include <iomanip>

namespace ArenaFighter {

TrainingMode::TrainingMode(const TrainingSettings& settings)
    : GameMode(MatchConfig()),
      m_settings(settings),
      m_recordingTime(0.0f),
      m_playbackTime(0.0f),
      m_isRecording(false),
      m_isPlaying(false),
      m_comboDamage(0.0f),
      m_comboManaUsed(0.0f) {
    
    // Configure for training
    m_config.maxPlayers = 2;
    m_config.infiniteTime = true; // No time limit
    m_config.allowPause = true;
    
    // Initialize stats
    m_stats = SessionStats();
    
    // Set default positions
    m_resetPositions[0] = XMFLOAT3(-200.0f, 0.0f, 0.0f);
    m_resetPositions[1] = XMFLOAT3(200.0f, 0.0f, 0.0f);
}

void TrainingMode::initialize() {
    GameMode::initialize();
    
    // Start directly in training mode
    setState(MatchState::InProgress);
    
    // Clear history
    m_frameDataHistory.clear();
    m_currentCombo.clear();
}

void TrainingMode::update(float deltaTime) {
    // Update session time
    m_stats.sessionTime += deltaTime;
    
    // Skip normal win condition checks
    if (m_currentState == MatchState::InProgress) {
        // Update physics
        m_physicsEngine->update(deltaTime);
        
        // Update combat
        m_combatSystem->update(deltaTime);
        
        // Update players
        for (int i = 0; i < m_players.size(); ++i) {
            auto& player = m_players[i];
            player->update(deltaTime);
            
            // Apply training settings
            if (i == 0) { // Player 1
                if (m_settings.infiniteHealth) {
                    player->setHealth(BASE_HEALTH);
                } else if (m_settings.autoHealthRegen && player->getHealth() < BASE_HEALTH) {
                    player->setHealth(std::min(player->getHealth() + 50.0f * deltaTime, BASE_HEALTH));
                }
                
                if (m_settings.infiniteMana) {
                    player->setMana(BASE_MANA);
                } else if (m_settings.autoManaRegen) {
                    float currentMana = player->getMana();
                    player->setMana(std::min(currentMana + MANA_REGEN_RATE * 2.0f * deltaTime, BASE_MANA));
                }
            }
        }
        
        // Update dummy
        updateDummy(deltaTime);
        
        // Update recording/playback
        if (m_isRecording) {
            updateRecording(deltaTime);
        } else if (m_isPlaying) {
            updatePlayback(deltaTime);
        }
        
        // Update tracking
        updateFrameDataTracking();
        
        // Check for position reset
        checkAutoReset();
        
        // Update UI
        updateUI();
    }
}

void TrainingMode::render() {
    GameMode::render();
    
    // Render training-specific UI
    displayTrainingInfo();
    
    // Show hitboxes if enabled
    if (m_settings.showHitboxes) {
        // This would be handled by the rendering system
        // Setting a flag that the renderer checks
    }
}

void TrainingMode::updateDummy(float deltaTime) {
    if (!m_dummy || m_players.size() < 2) {
        return;
    }
    
    // Apply dummy settings
    if (m_settings.infiniteHealth) {
        m_dummy->setHealth(BASE_HEALTH);
    }
    
    if (m_settings.infiniteMana) {
        m_dummy->setMana(BASE_MANA);
    }
    
    // Process dummy behavior
    processDummyBehavior();
}

void TrainingMode::processDummyBehavior() {
    if (!m_dummy) return;
    
    InputCommand dummyInput;
    
    switch (m_settings.dummyBehavior) {
        case TrainingSettings::Stand:
            // No input - just standing
            break;
            
        case TrainingSettings::Crouch:
            dummyInput.direction = InputDirection::Down;
            break;
            
        case TrainingSettings::Jump:
            // Jump repeatedly
            if (m_dummy->getCurrentState() == CharacterState::Idle) {
                dummyInput.action = InputAction::Jump;
            }
            break;
            
        case TrainingSettings::Block:
            dummyInput.action = InputAction::Block;
            dummyInput.direction = InputDirection::Back;
            break;
            
        case TrainingSettings::CPU:
            // Use AI from SinglePlayerMode
            // This would require extracting AI logic
            break;
            
        case TrainingSettings::Record:
            // Recording handled separately
            break;
            
        case TrainingSettings::Playback:
            // Playback handled separately
            break;
    }
    
    if (m_settings.dummyBehavior != TrainingSettings::Record &&
        m_settings.dummyBehavior != TrainingSettings::Playback) {
        handleInput(1, dummyInput);
    }
}

void TrainingMode::updateRecording(float deltaTime) {
    m_recordingTime += deltaTime;
    
    // Recording is handled in handleInput
    // Just track the time here
}

void TrainingMode::updatePlayback(float deltaTime) {
    if (m_recordedInputs.empty()) {
        m_isPlaying = false;
        return;
    }
    
    m_playbackTime += deltaTime;
    
    // Find and execute inputs for current time
    while (!m_recordedInputs.empty()) {
        const RecordedInput& nextInput = m_recordedInputs.front();
        
        if (nextInput.timestamp <= m_playbackTime) {
            // Execute this input
            handleInput(1, nextInput.command);
            m_recordedInputs.erase(m_recordedInputs.begin());
        } else {
            break; // Wait for next frame
        }
    }
    
    // Loop playback
    if (m_recordedInputs.empty() && m_currentRecording.size() > 0) {
        m_recordedInputs = m_currentRecording;
        m_playbackTime = 0.0f;
    }
}

void TrainingMode::updateFrameDataTracking() {
    if (!m_settings.showFrameData || m_players.empty()) {
        return;
    }
    
    // Track current move's frame data
    auto& player = m_players[0];
    CharacterState state = player->getCurrentState();
    
    if (state == CharacterState::Attacking || state == CharacterState::Special) {
        // Get current move info (would need to be exposed by character)
        // For now, using placeholder data
        m_currentFrameData.moveName = "Current Move";
        m_currentFrameData.startupFrames = 10;
        m_currentFrameData.activeFrames = 5;
        m_currentFrameData.recoveryFrames = 15;
        m_currentFrameData.totalFrames = 30;
        m_currentFrameData.damage = 100.0f;
        m_currentFrameData.manaCost = 20.0f;
        
        // Add to history
        if (m_frameDataHistory.empty() || 
            m_frameDataHistory.back().moveName != m_currentFrameData.moveName) {
            m_frameDataHistory.push_back(m_currentFrameData);
            
            // Limit history size
            if (m_frameDataHistory.size() > 10) {
                m_frameDataHistory.pop_front();
            }
        }
    }
}

void TrainingMode::checkAutoReset() {
    if (!m_settings.autoReset || m_players.size() < 2) {
        return;
    }
    
    // Calculate distance between players
    float distance = getDistanceToPlayer();
    
    // Reset if too far apart or if someone is knocked down
    bool shouldReset = distance > m_settings.resetDistance;
    
    for (auto& player : m_players) {
        if (player->getCurrentState() == CharacterState::KnockedDown) {
            shouldReset = true;
            break;
        }
    }
    
    if (shouldReset) {
        resetPositions();
    }
}

void TrainingMode::displayTrainingInfo() {
    // This would integrate with the UI system
    // For now, just tracking what should be displayed
    
    if (m_settings.showFrameData && !m_frameDataHistory.empty()) {
        // Display frame data overlay
    }
    
    if (m_settings.showInputs) {
        // Display input history
    }
    
    if (m_settings.showDamage && m_comboDamage > 0) {
        // Display combo damage
    }
}

void TrainingMode::setSettings(const TrainingSettings& settings) {
    m_settings = settings;
    
    // Apply immediate changes
    if (m_settings.dummyBehavior == TrainingSettings::Playback && hasRecording()) {
        playRecording();
    }
}

void TrainingMode::toggleSetting(const std::string& settingName) {
    if (settingName == "infiniteHealth") {
        m_settings.infiniteHealth = !m_settings.infiniteHealth;
    } else if (settingName == "infiniteMana") {
        m_settings.infiniteMana = !m_settings.infiniteMana;
    } else if (settingName == "showFrameData") {
        m_settings.showFrameData = !m_settings.showFrameData;
    } else if (settingName == "showHitboxes") {
        m_settings.showHitboxes = !m_settings.showHitboxes;
    } else if (settingName == "showInputs") {
        m_settings.showInputs = !m_settings.showInputs;
    } else if (settingName == "showDamage") {
        m_settings.showDamage = !m_settings.showDamage;
    } else if (settingName == "autoReset") {
        m_settings.autoReset = !m_settings.autoReset;
    }
}

void TrainingMode::startRecording() {
    if (!m_isRecording) {
        m_isRecording = true;
        m_isPlaying = false;
        m_recordingTime = 0.0f;
        m_currentRecording.clear();
        m_settings.dummyBehavior = TrainingSettings::Record;
    }
}

void TrainingMode::stopRecording() {
    if (m_isRecording) {
        m_isRecording = false;
        m_recordedInputs = m_currentRecording;
    }
}

void TrainingMode::playRecording() {
    if (hasRecording() && !m_isPlaying) {
        m_isPlaying = true;
        m_isRecording = false;
        m_playbackTime = 0.0f;
        m_recordedInputs = m_currentRecording;
        m_settings.dummyBehavior = TrainingSettings::Playback;
    }
}

void TrainingMode::clearRecording() {
    m_recordedInputs.clear();
    m_currentRecording.clear();
    m_isRecording = false;
    m_isPlaying = false;
}

void TrainingMode::resetPositions() {
    for (int i = 0; i < m_players.size(); ++i) {
        m_players[i]->setPosition(m_resetPositions[i]);
        m_players[i]->setHealth(BASE_HEALTH);
        m_players[i]->setMana(BASE_MANA);
        m_players[i]->resetCombo();
        m_players[i]->setState(CharacterState::Idle);
    }
    
    resetCombo();
}

void TrainingMode::savePositions() {
    for (int i = 0; i < m_players.size(); ++i) {
        m_resetPositions[i] = m_players[i]->getPosition();
    }
}

void TrainingMode::setStartingDistance(float distance) {
    m_settings.resetDistance = distance;
    m_resetPositions[0] = XMFLOAT3(-distance / 2.0f, 0.0f, 0.0f);
    m_resetPositions[1] = XMFLOAT3(distance / 2.0f, 0.0f, 0.0f);
}

void TrainingMode::resetCombo() {
    m_currentCombo.clear();
    m_comboDamage = 0.0f;
    m_comboManaUsed = 0.0f;
}

void TrainingMode::resetStats() {
    m_stats = SessionStats();
}

void TrainingMode::setDummyBehavior(TrainingSettings::DummyState behavior) {
    m_settings.dummyBehavior = behavior;
    
    if (behavior == TrainingSettings::Record) {
        startRecording();
    } else if (behavior == TrainingSettings::Playback) {
        playRecording();
    }
}

float TrainingMode::getDistanceToPlayer() const {
    if (m_players.size() < 2) {
        return 0.0f;
    }
    
    XMFLOAT3 p1Pos = m_players[0]->getPosition();
    XMFLOAT3 p2Pos = m_players[1]->getPosition();
    
    return std::abs(p1Pos.x - p2Pos.x);
}

} // namespace ArenaFighter