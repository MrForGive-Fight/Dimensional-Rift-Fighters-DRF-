#pragma once

#include "GameMode.h"
#include <deque>
#include <vector>

namespace ArenaFighter {

// Training mode settings
struct TrainingSettings {
    // Health/Mana options
    bool infiniteHealth = false;
    bool infiniteMana = false;
    bool autoHealthRegen = true;
    bool autoManaRegen = true;
    
    // Dummy options
    enum DummyState {
        Stand,          // Standing still
        Crouch,         // Crouching
        Jump,           // Jumping
        Block,          // Blocking
        CPU,            // AI controlled
        Record,         // Record actions
        Playback        // Playback recorded
    };
    DummyState dummyBehavior = Stand;
    
    // Display options
    bool showFrameData = true;
    bool showHitboxes = false;
    bool showInputs = true;
    bool showDamage = true;
    
    // Position reset
    bool autoReset = false;
    float resetDistance = 300.0f;
};

// Frame data display info
struct FrameDataDisplay {
    std::string moveName;
    int startupFrames;
    int activeFrames;
    int recoveryFrames;
    int totalFrames;
    int blockAdvantage;
    int hitAdvantage;
    float damage;
    float manaCost;
};

// Input recording for dummy
struct RecordedInput {
    float timestamp;
    InputCommand command;
};

class TrainingMode : public GameMode {
private:
    TrainingSettings m_settings;
    std::shared_ptr<CharacterBase> m_dummy;
    
    // Recording system
    std::vector<RecordedInput> m_recordedInputs;
    std::vector<RecordedInput> m_currentRecording;
    float m_recordingTime;
    float m_playbackTime;
    bool m_isRecording;
    bool m_isPlaying;
    
    // Frame data tracking
    FrameDataDisplay m_currentFrameData;
    std::deque<FrameDataDisplay> m_frameDataHistory;
    
    // Combo tracking
    std::vector<std::string> m_currentCombo;
    float m_comboDamage;
    float m_comboManaUsed;
    
    // Position reset
    XMFLOAT3 m_resetPositions[2];
    
    // Statistics
    struct SessionStats {
        int totalHits;
        int totalBlocks;
        float totalDamage;
        float peakDamage;
        int longestCombo;
        float sessionTime;
    };
    SessionStats m_stats;
    
    // Training mode specific methods
    void updateDummy(float deltaTime);
    void processDummyBehavior();
    void updateRecording(float deltaTime);
    void updatePlayback(float deltaTime);
    void updateFrameDataTracking();
    void checkAutoReset();
    void displayTrainingInfo();

public:
    TrainingMode(const TrainingSettings& settings = TrainingSettings());
    virtual ~TrainingMode() = default;
    
    // Override base methods
    void initialize() override;
    void update(float deltaTime) override;
    void render() override;
    bool checkWinConditions() override { return false; } // Never end in training
    
    // Settings management
    void setSettings(const TrainingSettings& settings);
    TrainingSettings getSettings() const { return m_settings; }
    void toggleSetting(const std::string& settingName);
    
    // Recording system
    void startRecording();
    void stopRecording();
    void playRecording();
    void clearRecording();
    bool isRecording() const { return m_isRecording; }
    bool hasRecording() const { return !m_recordedInputs.empty(); }
    
    // Position management
    void resetPositions();
    void savePositions();
    void setStartingDistance(float distance);
    
    // Frame data
    FrameDataDisplay getCurrentFrameData() const { return m_currentFrameData; }
    std::deque<FrameDataDisplay> getFrameDataHistory() const { return m_frameDataHistory; }
    
    // Combo display
    std::vector<std::string> getCurrentCombo() const { return m_currentCombo; }
    float getComboDamage() const { return m_comboDamage; }
    void resetCombo();
    
    // Statistics
    SessionStats getSessionStats() const { return m_stats; }
    void resetStats();
    
    // Mode specific implementations
    std::string getModeName() const override { return "Training"; }
    GameModeType getModeType() const override { return GameModeType::Ranked1v1; } // Reuse UI
    bool supportsOnline() const override { return false; }
    int getMinPlayers() const override { return 1; }
    int getMaxPlayers() const override { return 2; }
    
    // Training specific features
    void setDummyBehavior(TrainingSettings::DummyState behavior);
    void toggleHitboxDisplay() { m_settings.showHitboxes = !m_settings.showHitboxes; }
    void toggleFrameDataDisplay() { m_settings.showFrameData = !m_settings.showFrameData; }
};

} // namespace ArenaFighter