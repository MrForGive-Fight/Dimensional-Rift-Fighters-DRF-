#pragma once
#include "../Components/UIPanel.h"
#include "../Components/GameButton.h"
#include "../Core/UITheme.h"
#include <vector>
#include <memory>

namespace ArenaFighter {

struct RoomInfo {
    std::string name;
    std::string mode;
    int currentPlayers;
    int maxPlayers;
    bool hasPassword;
    int ping;
};

class LobbyWindow : public UIPanel {
public:
    LobbyWindow();
    ~LobbyWindow();
    
    void Initialize();
    void Update(float deltaTime) override;
    void Render() override;
    
    // Room management
    void RefreshRoomList();
    void CreateRoom(const std::string& name, const std::string& mode, int maxPlayers);
    void JoinRoom(int roomId);
    void QuickJoin();
    
private:
    // Mode selection cards
    struct ModeCard {
        std::string modeName;
        std::string description;
        void* icon;
        int playerCount;
        std::unique_ptr<UIPanel> panel;
        std::unique_ptr<GameButton> selectButton;
        float hoverAnimation;
    };
    
    std::vector<ModeCard> m_modeCards;
    int m_selectedMode;
    
    // Room list
    std::vector<RoomInfo> m_rooms;
    int m_selectedRoom;
    ImVec2 m_scrollPosition;
    
    // Action buttons
    std::unique_ptr<GameButton> m_changeModeBtn;
    std::unique_ptr<GameButton> m_createRoomBtn;
    std::unique_ptr<GameButton> m_joinQuicklyBtn;
    std::unique_ptr<GameButton> m_refreshBtn;
    
    // Filter options
    bool m_showFullRooms;
    bool m_showPasswordRooms;
    std::string m_searchFilter;
    
    // Sections
    void RenderModeSelection();
    void RenderRoomList();
    void RenderRoomCard(const RoomInfo& room, ImVec2 position, bool selected);
    void RenderActionButtons();
    
    // Animations
    void AnimateModeCards(float deltaTime);
    float m_modeCardSpacing;
    float m_animationTime;
};

// Create room modal
class CreateRoomModal : public UIPanel {
public:
    CreateRoomModal();
    
    void Show();
    void Hide();
    void Render() override;
    
    // Callbacks
    void SetOnCreate(std::function<void(const std::string&, const std::string&, int, bool)> callback) {
        m_onCreate = callback;
    }
    
private:
    char m_roomName[64];
    int m_selectedMode;
    int m_maxPlayers;
    bool m_usePassword;
    char m_password[32];
    
    std::unique_ptr<GameButton> m_createBtn;
    std::unique_ptr<GameButton> m_cancelBtn;
    
    std::function<void(const std::string&, const std::string&, int, bool)> m_onCreate;
};

} // namespace ArenaFighter