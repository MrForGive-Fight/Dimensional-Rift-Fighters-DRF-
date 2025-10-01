#include "InputBuffer.h"
#include <algorithm>
#include <iostream>
#include <cmath>

namespace ArenaFighter {

// InputBuffer implementation

InputBuffer::InputBuffer(uint32_t playerId)
    : m_playerId(playerId)
    , m_writeIndex(0)
    , m_lastConfirmedFrame(0)
    , m_lastReceivedFrame(0)
    , m_oldestFrame(0)
    , m_lastInput(0)
    , m_predictionAccuracy(1.0f)
    , m_predictionHits(0)
    , m_predictionMisses(0) {
    
    m_buffer.resize(BUFFER_SIZE);
}

void InputBuffer::AddInput(const InputFrame& input) {
    size_t index = GetBufferIndex(input.frame);
    m_buffer[index] = input;
    
    // Update tracking
    if (input.frame > m_lastReceivedFrame) {
        m_lastReceivedFrame = input.frame;
    }
    
    if (!input.predicted) {
        m_lastInput = input.inputMask;
    }
    
    // Update oldest frame if needed
    if (m_oldestFrame == 0 || input.frame < m_oldestFrame) {
        m_oldestFrame = input.frame;
    }
}

void InputBuffer::AddPredictedInput(uint32_t frame, uint32_t predictedMask) {
    InputFrame predictedInput;
    predictedInput.frame = frame;
    predictedInput.inputMask = predictedMask;
    predictedInput.inputId = 0;
    predictedInput.timestamp = static_cast<uint16_t>(frame);
    predictedInput.confirmed = false;
    predictedInput.predicted = true;
    
    AddInput(predictedInput);
}

std::optional<InputFrame> InputBuffer::GetInput(uint32_t frame) const {
    if (!IsFrameInBuffer(frame)) {
        return std::nullopt;
    }
    
    size_t index = GetBufferIndex(frame);
    const InputFrame& input = m_buffer[index];
    
    if (input.frame == frame) {
        return input;
    }
    
    return std::nullopt;
}

uint32_t InputBuffer::GetInputMask(uint32_t frame) const {
    auto input = GetInput(frame);
    if (input.has_value()) {
        return input->inputMask;
    }
    
    // Return last known input as fallback
    return m_lastInput;
}

void InputBuffer::ConfirmInput(uint32_t frame, uint32_t inputMask, uint16_t inputId) {
    if (!IsFrameInBuffer(frame)) {
        return;
    }
    
    size_t index = GetBufferIndex(frame);
    InputFrame& input = m_buffer[index];
    
    // Check prediction accuracy
    if (input.frame == frame && input.predicted) {
        if (input.inputMask == inputMask) {
            m_predictionHits++;
        } else {
            m_predictionMisses++;
        }
        
        // Update accuracy (rolling average)
        uint32_t total = m_predictionHits + m_predictionMisses;
        if (total > 0) {
            m_predictionAccuracy = static_cast<float>(m_predictionHits) / total;
        }
    }
    
    // Update input with confirmed data
    input.frame = frame;
    input.inputMask = inputMask;
    input.inputId = inputId;
    input.confirmed = true;
    input.predicted = false;
    
    // Update last confirmed frame
    if (frame > m_lastConfirmedFrame) {
        m_lastConfirmedFrame = frame;
    }
}

void InputBuffer::ConfirmFramesUpTo(uint32_t frame) {
    for (uint32_t f = m_lastConfirmedFrame + 1; f <= frame && f <= m_lastReceivedFrame; ++f) {
        if (!IsFrameInBuffer(f)) continue;
        
        size_t index = GetBufferIndex(f);
        m_buffer[index].confirmed = true;
    }
    
    if (frame > m_lastConfirmedFrame) {
        m_lastConfirmedFrame = frame;
    }
}

bool InputBuffer::NeedsRollback(uint32_t currentFrame) const {
    // Check if we have unconfirmed frames that would require rollback
    uint32_t unconfirmedStart = m_lastConfirmedFrame + 1;
    
    if (unconfirmedStart >= currentFrame) {
        return false; // All frames up to current are confirmed
    }
    
    // Check if the gap is within rollback limit
    uint32_t rollbackFrames = currentFrame - unconfirmedStart;
    return rollbackFrames > 0 && rollbackFrames <= MAX_ROLLBACK;
}

std::vector<uint32_t> InputBuffer::GetUnconfirmedFrames() const {
    std::vector<uint32_t> unconfirmed;
    
    for (uint32_t frame = m_lastConfirmedFrame + 1; frame <= m_lastReceivedFrame; ++frame) {
        if (!IsFrameInBuffer(frame)) continue;
        
        size_t index = GetBufferIndex(frame);
        if (m_buffer[index].frame == frame && !m_buffer[index].confirmed) {
            unconfirmed.push_back(frame);
        }
    }
    
    return unconfirmed;
}

uint32_t InputBuffer::PredictNextInput() const {
    // Simple prediction: return last input
    // More sophisticated prediction could analyze patterns
    return m_lastInput;
}

void InputBuffer::UpdatePrediction(uint32_t frame, uint32_t actualInput) {
    if (!IsFrameInBuffer(frame)) return;
    
    size_t index = GetBufferIndex(frame);
    InputFrame& input = m_buffer[index];
    
    if (input.frame == frame && input.predicted) {
        if (input.inputMask != actualInput) {
            // Misprediction - update the input
            input.inputMask = actualInput;
            input.predicted = false;
        }
    }
}

void InputBuffer::Clear() {
    for (auto& input : m_buffer) {
        input = InputFrame{};
    }
    
    m_writeIndex = 0;
    m_lastConfirmedFrame = 0;
    m_lastReceivedFrame = 0;
    m_oldestFrame = 0;
    m_lastInput = 0;
    m_predictionHits = 0;
    m_predictionMisses = 0;
    m_predictionAccuracy = 1.0f;
}

void InputBuffer::RemoveOldFrames(uint32_t currentFrame) {
    if (currentFrame <= BUFFER_SIZE) {
        return; // Not enough frames to remove yet
    }
    
    uint32_t cutoffFrame = currentFrame - BUFFER_SIZE + 10; // Keep some buffer
    
    // Update oldest frame
    if (cutoffFrame > m_oldestFrame) {
        m_oldestFrame = cutoffFrame;
    }
}

size_t InputBuffer::GetBufferUsage() const {
    if (m_lastReceivedFrame == 0) return 0;
    
    uint32_t framesStored = m_lastReceivedFrame - m_oldestFrame + 1;
    return std::min(static_cast<size_t>(framesStored), BUFFER_SIZE);
}

void InputBuffer::PrintBufferState() const {
    std::cout << "InputBuffer[Player " << m_playerId << "]:\n";
    std::cout << "  Last Confirmed: " << m_lastConfirmedFrame << "\n";
    std::cout << "  Last Received: " << m_lastReceivedFrame << "\n";
    std::cout << "  Oldest Frame: " << m_oldestFrame << "\n";
    std::cout << "  Buffer Usage: " << GetBufferUsage() << "/" << BUFFER_SIZE << "\n";
    std::cout << "  Prediction Accuracy: " << (m_predictionAccuracy * 100.0f) << "%\n";
}

size_t InputBuffer::GetBufferIndex(uint32_t frame) const {
    return frame % BUFFER_SIZE;
}

bool InputBuffer::IsFrameInBuffer(uint32_t frame) const {
    if (frame < m_oldestFrame || m_lastReceivedFrame == 0) {
        return false;
    }
    
    uint32_t frameAge = m_lastReceivedFrame > frame ? m_lastReceivedFrame - frame : 0;
    return frameAge < BUFFER_SIZE;
}

// InputBufferManager implementation

InputBufferManager::InputBufferManager() : m_currentFrame(0) {}

void InputBufferManager::AddPlayer(uint32_t playerId) {
    if (m_playerBuffers.find(playerId) == m_playerBuffers.end()) {
        m_playerBuffers[playerId] = std::make_unique<InputBuffer>(playerId);
    }
}

void InputBufferManager::RemovePlayer(uint32_t playerId) {
    m_playerBuffers.erase(playerId);
}

void InputBufferManager::ClearAllBuffers() {
    for (auto& [playerId, buffer] : m_playerBuffers) {
        buffer->Clear();
    }
}

void InputBufferManager::AddInput(uint32_t playerId, const InputFrame& input) {
    auto it = m_playerBuffers.find(playerId);
    if (it != m_playerBuffers.end()) {
        it->second->AddInput(input);
    }
}

std::optional<InputFrame> InputBufferManager::GetInput(uint32_t playerId, uint32_t frame) const {
    auto it = m_playerBuffers.find(playerId);
    if (it != m_playerBuffers.end()) {
        return it->second->GetInput(frame);
    }
    return std::nullopt;
}

bool InputBufferManager::CheckForRollback(uint32_t currentFrame) const {
    for (const auto& [playerId, buffer] : m_playerBuffers) {
        if (buffer->NeedsRollback(currentFrame)) {
            return true;
        }
    }
    return false;
}

uint32_t InputBufferManager::GetEarliestRollbackFrame(uint32_t currentFrame) const {
    uint32_t earliest = currentFrame;
    
    for (const auto& [playerId, buffer] : m_playerBuffers) {
        uint32_t lastConfirmed = buffer->GetLastConfirmedFrame();
        if (lastConfirmed < earliest) {
            earliest = lastConfirmed + 1;
        }
    }
    
    return earliest;
}

void InputBufferManager::ConfirmFrame(uint32_t frame) {
    m_currentFrame = frame;
    
    for (auto& [playerId, buffer] : m_playerBuffers) {
        buffer->ConfirmFramesUpTo(frame);
    }
}

void InputBufferManager::PredictMissingInputs(uint32_t currentFrame) {
    for (auto& [playerId, buffer] : m_playerBuffers) {
        uint32_t lastReceived = buffer->GetLastReceivedFrame();
        
        // Predict inputs for frames we haven't received yet
        for (uint32_t frame = lastReceived + 1; frame <= currentFrame; ++frame) {
            if (!buffer->GetInput(frame).has_value()) {
                uint32_t predicted = buffer->PredictNextInput();
                buffer->AddPredictedInput(frame, predicted);
            }
        }
    }
}

float InputBufferManager::GetAveragePredictionAccuracy() const {
    if (m_playerBuffers.empty()) return 1.0f;
    
    float totalAccuracy = 0.0f;
    for (const auto& [playerId, buffer] : m_playerBuffers) {
        totalAccuracy += buffer->GetPredictionAccuracy();
    }
    
    return totalAccuracy / m_playerBuffers.size();
}

InputBuffer* InputBufferManager::GetPlayerBuffer(uint32_t playerId) {
    auto it = m_playerBuffers.find(playerId);
    return it != m_playerBuffers.end() ? it->second.get() : nullptr;
}

const InputBuffer* InputBufferManager::GetPlayerBuffer(uint32_t playerId) const {
    auto it = m_playerBuffers.find(playerId);
    return it != m_playerBuffers.end() ? it->second.get() : nullptr;
}

void InputBufferManager::PrintAllBufferStates() const {
    std::cout << "=== Input Buffer Manager State ===\n";
    std::cout << "Current Frame: " << m_currentFrame << "\n";
    std::cout << "Average Prediction Accuracy: " << (GetAveragePredictionAccuracy() * 100.0f) << "%\n";
    std::cout << "Player Count: " << m_playerBuffers.size() << "\n\n";
    
    for (const auto& [playerId, buffer] : m_playerBuffers) {
        buffer->PrintBufferState();
        std::cout << "\n";
    }
}

// InputInterpolator implementation

InputInterpolator::InputInterpolator() {}

void InputInterpolator::AddInput(uint32_t frame, uint32_t inputMask) {
    TimedInput input;
    input.frame = frame;
    input.inputMask = inputMask;
    input.timestamp = static_cast<float>(frame) / 60.0f; // Assuming 60fps
    
    m_inputHistory.push_back(input);
    
    // Keep history size limited
    if (m_inputHistory.size() > MAX_HISTORY) {
        m_inputHistory.erase(m_inputHistory.begin());
    }
}

InputInterpolator::InterpolatedInput InputInterpolator::GetInterpolatedInput(float frameTime) const {
    InterpolatedInput result = {0.0f, 0.0f, 0.0f, 0.0f, 0};
    
    if (m_inputHistory.empty()) {
        return result;
    }
    
    // Find the two inputs to interpolate between
    const TimedInput* prev = nullptr;
    const TimedInput* next = nullptr;
    
    for (size_t i = 0; i < m_inputHistory.size(); ++i) {
        if (m_inputHistory[i].timestamp <= frameTime) {
            prev = &m_inputHistory[i];
        } else {
            next = &m_inputHistory[i];
            break;
        }
    }
    
    // If we only have one bound, use it directly
    if (!prev && next) {
        ExtractAnalogValues(next->inputMask, result.moveX, result.moveY, 
                          result.lookX, result.lookY);
        result.buttons = next->inputMask & 0xFFFF; // Lower 16 bits for buttons
        return result;
    }
    
    if (prev && !next) {
        ExtractAnalogValues(prev->inputMask, result.moveX, result.moveY, 
                          result.lookX, result.lookY);
        result.buttons = prev->inputMask & 0xFFFF;
        return result;
    }
    
    if (prev && next) {
        // Interpolate between prev and next
        float t = (frameTime - prev->timestamp) / (next->timestamp - prev->timestamp);
        t = std::clamp(t, 0.0f, 1.0f);
        
        float prevMoveX, prevMoveY, prevLookX, prevLookY;
        float nextMoveX, nextMoveY, nextLookX, nextLookY;
        
        ExtractAnalogValues(prev->inputMask, prevMoveX, prevMoveY, prevLookX, prevLookY);
        ExtractAnalogValues(next->inputMask, nextMoveX, nextMoveY, nextLookX, nextLookY);
        
        // Linear interpolation for analog values
        result.moveX = prevMoveX + (nextMoveX - prevMoveX) * t;
        result.moveY = prevMoveY + (nextMoveY - prevMoveY) * t;
        result.lookX = prevLookX + (nextLookX - prevLookX) * t;
        result.lookY = prevLookY + (nextLookY - prevLookY) * t;
        
        // Use the most recent buttons (no interpolation)
        result.buttons = next->inputMask & 0xFFFF;
    }
    
    return result;
}

void InputInterpolator::ClearOldInputs(uint32_t currentFrame) {
    float currentTime = static_cast<float>(currentFrame) / 60.0f;
    float cutoffTime = currentTime - 1.0f; // Keep last second
    
    m_inputHistory.erase(
        std::remove_if(m_inputHistory.begin(), m_inputHistory.end(),
                      [cutoffTime](const TimedInput& input) {
                          return input.timestamp < cutoffTime;
                      }),
        m_inputHistory.end()
    );
}

void InputInterpolator::ExtractAnalogValues(uint32_t inputMask, float& moveX, float& moveY, 
                                           float& lookX, float& lookY) const {
    // Extract analog values from upper 16 bits
    // This is a simplified example - real implementation would depend on input encoding
    uint16_t analogData = (inputMask >> 16) & 0xFFFF;
    
    // Split into 4 4-bit values normalized to -1 to 1
    moveX = ((analogData & 0xF) - 7.5f) / 7.5f;
    moveY = (((analogData >> 4) & 0xF) - 7.5f) / 7.5f;
    lookX = (((analogData >> 8) & 0xF) - 7.5f) / 7.5f;
    lookY = (((analogData >> 12) & 0xF) - 7.5f) / 7.5f;
    
    // Clamp values
    moveX = std::clamp(moveX, -1.0f, 1.0f);
    moveY = std::clamp(moveY, -1.0f, 1.0f);
    lookX = std::clamp(lookX, -1.0f, 1.0f);
    lookY = std::clamp(lookY, -1.0f, 1.0f);
}

} // namespace ArenaFighter