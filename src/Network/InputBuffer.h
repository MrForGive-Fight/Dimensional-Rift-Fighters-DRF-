#pragma once

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <optional>

namespace ArenaFighter {

// Input data structure
struct InputFrame {
    uint32_t frame;
    uint32_t inputMask;
    uint16_t inputId;
    uint16_t timestamp;
    bool confirmed;      // Server confirmed this input
    bool predicted;      // This input was predicted, not received
};

// Circular buffer for storing inputs with rollback support
class InputBuffer {
public:
    static constexpr size_t BUFFER_SIZE = 120; // 2 seconds at 60fps
    static constexpr size_t MAX_ROLLBACK = 7;  // Max 7 frames rollback
    
    InputBuffer(uint32_t playerId);
    ~InputBuffer() = default;
    
    // Add input to buffer
    void AddInput(const InputFrame& input);
    void AddPredictedInput(uint32_t frame, uint32_t predictedMask);
    
    // Retrieve input for specific frame
    std::optional<InputFrame> GetInput(uint32_t frame) const;
    uint32_t GetInputMask(uint32_t frame) const;
    
    // Confirm inputs from server
    void ConfirmInput(uint32_t frame, uint32_t inputMask, uint16_t inputId);
    void ConfirmFramesUpTo(uint32_t frame);
    
    // Rollback support
    uint32_t GetLastConfirmedFrame() const { return m_lastConfirmedFrame; }
    uint32_t GetLastReceivedFrame() const { return m_lastReceivedFrame; }
    bool NeedsRollback(uint32_t currentFrame) const;
    std::vector<uint32_t> GetUnconfirmedFrames() const;
    
    // Prediction helpers
    uint32_t PredictNextInput() const;
    void UpdatePrediction(uint32_t frame, uint32_t actualInput);
    float GetPredictionAccuracy() const { return m_predictionAccuracy; }
    
    // Buffer management
    void Clear();
    void RemoveOldFrames(uint32_t currentFrame);
    size_t GetBufferUsage() const;
    
    // Debug info
    void PrintBufferState() const;
    uint32_t GetPlayerId() const { return m_playerId; }

private:
    uint32_t m_playerId;
    std::vector<InputFrame> m_buffer;
    size_t m_writeIndex;
    
    // Frame tracking
    uint32_t m_lastConfirmedFrame;
    uint32_t m_lastReceivedFrame;
    uint32_t m_oldestFrame;
    
    // Prediction stats
    uint32_t m_lastInput;
    float m_predictionAccuracy;
    uint32_t m_predictionHits;
    uint32_t m_predictionMisses;
    
    // Helper functions
    size_t GetBufferIndex(uint32_t frame) const;
    bool IsFrameInBuffer(uint32_t frame) const;
};

// Input buffer manager for all players
class InputBufferManager {
public:
    InputBufferManager();
    ~InputBufferManager() = default;
    
    // Player management
    void AddPlayer(uint32_t playerId);
    void RemovePlayer(uint32_t playerId);
    void ClearAllBuffers();
    
    // Input operations
    void AddInput(uint32_t playerId, const InputFrame& input);
    std::optional<InputFrame> GetInput(uint32_t playerId, uint32_t frame) const;
    
    // Rollback operations
    bool CheckForRollback(uint32_t currentFrame) const;
    uint32_t GetEarliestRollbackFrame(uint32_t currentFrame) const;
    void ConfirmFrame(uint32_t frame);
    
    // Prediction
    void PredictMissingInputs(uint32_t currentFrame);
    float GetAveragePredictionAccuracy() const;
    
    // Buffer access
    InputBuffer* GetPlayerBuffer(uint32_t playerId);
    const InputBuffer* GetPlayerBuffer(uint32_t playerId) const;
    
    // Debug
    void PrintAllBufferStates() const;

private:
    std::unordered_map<uint32_t, std::unique_ptr<InputBuffer>> m_playerBuffers;
    uint32_t m_currentFrame;
};

// Input interpolation for smoother playback
class InputInterpolator {
public:
    struct InterpolatedInput {
        float moveX, moveY;
        float lookX, lookY;
        uint32_t buttons;
    };
    
    InputInterpolator();
    
    // Add raw input
    void AddInput(uint32_t frame, uint32_t inputMask);
    
    // Get interpolated input for rendering
    InterpolatedInput GetInterpolatedInput(float frameTime) const;
    
    // Clear old inputs
    void ClearOldInputs(uint32_t currentFrame);

private:
    struct TimedInput {
        uint32_t frame;
        uint32_t inputMask;
        float timestamp;
    };
    
    std::vector<TimedInput> m_inputHistory;
    static constexpr size_t MAX_HISTORY = 10;
    
    // Extract analog values from input mask
    void ExtractAnalogValues(uint32_t inputMask, float& moveX, float& moveY, float& lookX, float& lookY) const;
};

} // namespace ArenaFighter