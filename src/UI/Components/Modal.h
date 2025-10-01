#pragma once
#include "UIPanel.h"
#include "GameButton.h"
#include <functional>
#include <memory>

namespace ArenaFighter {

class Modal : public UIPanel {
public:
    enum ModalType {
        INFORMATION,
        CONFIRMATION,
        WARNING,
        ERROR,
        CUSTOM
    };
    
    enum ButtonLayout {
        OK_ONLY,
        OK_CANCEL,
        YES_NO,
        YES_NO_CANCEL,
        CUSTOM_BUTTONS
    };
    
    Modal(const std::string& title, ModalType type = CUSTOM);
    ~Modal();
    
    // Content
    void SetMessage(const std::string& message) { m_message = message; }
    void SetIcon(void* icon) { m_icon = icon; }
    void SetButtonLayout(ButtonLayout layout);
    void AddCustomButton(const std::string& label, std::function<void()> callback);
    
    // Display
    void Show();
    void Hide();
    void ShowBlocking(); // Modal that blocks input to other windows
    
    // Callbacks
    void SetOnOk(std::function<void()> callback) { m_onOk = callback; }
    void SetOnCancel(std::function<void()> callback) { m_onCancel = callback; }
    void SetOnClose(std::function<void()> callback) { m_onClose = callback; }
    
    // Rendering
    void Render() override;
    void Update(float deltaTime) override;
    
private:
    std::string m_title;
    std::string m_message;
    ModalType m_type;
    ButtonLayout m_buttonLayout;
    void* m_icon;
    
    // Buttons
    std::vector<std::unique_ptr<GameButton>> m_buttons;
    
    // State
    bool m_isBlocking;
    float m_showAnimation;
    
    // Callbacks
    std::function<void()> m_onOk;
    std::function<void()> m_onCancel;
    std::function<void()> m_onClose;
    
    // Internal methods
    void CreateButtons();
    void CenterOnScreen();
    void RenderOverlay();
    void RenderHeader();
    void RenderContent();
    void RenderButtons();
    ImU32 GetTypeColor() const;
};

// Common modal dialogs
namespace Modals {
    void ShowErrorMessage(const std::string& title, const std::string& message);
    void ShowInfoMessage(const std::string& title, const std::string& message);
    void ShowConfirmation(const std::string& title, const std::string& message,
                         std::function<void()> onConfirm,
                         std::function<void()> onCancel = nullptr);
    
    // Input dialog
    class InputDialog : public Modal {
    public:
        InputDialog(const std::string& title, const std::string& prompt);
        
        void SetDefaultValue(const std::string& value);
        void SetMaxLength(int length) { m_maxLength = length; }
        void SetInputType(int type) { m_inputType = type; } // Text, Number, Password
        
        void SetOnSubmit(std::function<void(const std::string&)> callback) {
            m_onSubmit = callback;
        }
        
        void Render() override;
        
    private:
        std::string m_prompt;
        char m_inputBuffer[256];
        int m_maxLength;
        int m_inputType;
        std::function<void(const std::string&)> m_onSubmit;
    };
    
    // Progress dialog
    class ProgressDialog : public Modal {
    public:
        ProgressDialog(const std::string& title);
        
        void SetProgress(float progress) { m_progress = progress; }
        void SetStatusText(const std::string& text) { m_statusText = text; }
        void SetCancelable(bool cancelable);
        
        void Render() override;
        
    private:
        float m_progress;
        std::string m_statusText;
        bool m_cancelable;
        std::unique_ptr<GameButton> m_cancelButton;
    };
}

// Modal manager for stacking and focus management
class ModalManager {
public:
    static ModalManager& Instance();
    
    void ShowModal(std::shared_ptr<Modal> modal);
    void CloseTopModal();
    void CloseAllModals();
    
    void Update(float deltaTime);
    void Render();
    
    bool HasBlockingModal() const;
    
private:
    ModalManager() = default;
    std::vector<std::shared_ptr<Modal>> m_modalStack;
    float m_overlayAlpha;
};

} // namespace ArenaFighter