#pragma once
#include "../Components/TabbedPanel.h"
#include "../Components/GameButton.h"
#include "../Components/Slider.h"
#include <memory>
#include <array>

namespace ArenaFighter {

class SettingsWindow : public TabbedPanel {
public:
    SettingsWindow();
    ~SettingsWindow();
    
    void Initialize();
    void SaveSettings();
    void LoadSettings();
    void ResetToDefaults();
    
private:
    // Graphics settings
    struct GraphicsSettings {
        int quality; // 0=Low, 1=Medium, 2=High, 3=Custom
        int resolution;
        bool fullscreen;
        bool vsync;
        int antiAliasing;
        int shadowQuality;
        int textureQuality;
        int effectQuality;
        float renderScale;
    } m_graphics;
    
    // Sound settings
    struct SoundSettings {
        float masterVolume;
        float bgmVolume;
        float sfxVolume;
        float voiceVolume;
        bool muteWhenMinimized;
        int audioDevice;
    } m_sound;
    
    // Control settings
    struct ControlBinding {
        std::string action;
        int keyCode;
        int altKeyCode;
        bool isRebinding;
    };
    std::vector<ControlBinding> m_keyBindings;
    int m_rebindingIndex;
    
    // Joystick settings
    struct JoystickSettings {
        float deadzone;
        float sensitivity;
        bool invertY;
        std::array<int, 16> buttonMappings;
    } m_joystick;
    
    // UI Components
    std::unique_ptr<GameButton> m_applyBtn;
    std::unique_ptr<GameButton> m_resetBtn;
    std::unique_ptr<GameButton> m_closeBtn;
    
    // Tab content creation
    void CreateGraphicsTab();
    void CreateSoundTab();
    void CreateControlsTab();
    void CreateJoystickTab();
    void CreateGameplayTab();
    
    // Graphics tab components
    void RenderQualityPresets();
    void RenderGraphicsOptions();
    void UpdateGraphicsPreset(int preset);
    
    // Sound tab components
    void RenderVolumeSliders();
    void RenderAudioDeviceSelection();
    
    // Controls tab components
    void RenderKeyBindings();
    void StartRebinding(int index);
    void FinishRebinding(int keyCode);
    void CancelRebinding();
    
    // Joystick tab components
    void RenderJoystickVisualization();
    void RenderButtonMappings();
    
    // Callbacks
    void OnApply();
    void OnReset();
    void OnClose();
};

// Custom slider component for settings
class SettingsSlider : public Slider {
public:
    SettingsSlider(const std::string& label, float min, float max, float* value);
    
    void Render() override;
    
private:
    std::string m_label;
    float m_min;
    float m_max;
    float* m_value;
    bool m_showPercentage;
};

// Radio button group for settings
class RadioButtonGroup {
public:
    RadioButtonGroup(const std::string& label);
    
    void AddOption(const std::string& option);
    void SetSelected(int index) { m_selectedIndex = index; }
    int GetSelected() const { return m_selectedIndex; }
    
    void Render();
    
private:
    std::string m_label;
    std::vector<std::string> m_options;
    int m_selectedIndex;
    float m_animationProgress;
};

} // namespace ArenaFighter