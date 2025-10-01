#pragma once
#include "../Components/TabbedPanel.h"
#include "../Components/GameButton.h"
#include "../Components/ContextMenu.h"
#include <vector>
#include <memory>

namespace ArenaFighter {

struct PlayerInfo {
    std::string name;
    std::string status;
    int level;
    bool isOnline;
    bool inGame;
    int ping;
    void* avatar;
};

struct GuildInfo {
    std::string name;
    std::string rank;
    int memberCount;
    int maxMembers;
    std::string announcement;
};

class SocialWindow : public TabbedPanel {
public:
    SocialWindow();
    ~SocialWindow();
    
    void Initialize();
    void Update(float deltaTime) override;
    
    // Friend management
    void AddFriend(const std::string& playerName);
    void RemoveFriend(const std::string& playerName);
    void BlockPlayer(const std::string& playerName);
    
    // Guild management
    void JoinGuild(const std::string& guildName);
    void LeaveGuild();
    void PromoteMember(const std::string& memberName);
    
private:
    // Friends tab
    std::vector<PlayerInfo> m_friendsList;
    int m_selectedFriend;
    std::unique_ptr<ContextMenu> m_friendContextMenu;
    
    // Guild tab
    GuildInfo m_guildInfo;
    std::vector<PlayerInfo> m_guildMembers;
    int m_selectedGuildMember;
    std::unique_ptr<ContextMenu> m_guildContextMenu;
    
    // Blacklist tab
    std::vector<std::string> m_blacklist;
    int m_selectedBlacklist;
    
    // Search
    char m_searchBuffer[64];
    std::string m_searchText;
    
    // Action buttons
    std::unique_ptr<GameButton> m_addFriendBtn;
    std::unique_ptr<GameButton> m_whisperBtn;
    std::unique_ptr<GameButton> m_inviteBtn;
    std::unique_ptr<GameButton> m_viewProfileBtn;
    
    // Tab content
    void CreateFriendsTab();
    void CreateGuildTab();
    void CreateBlacklistTab();
    
    // Rendering
    void RenderFriendsList();
    void RenderGuildInfo();
    void RenderGuildMembers();
    void RenderBlacklist();
    void RenderPlayerEntry(const PlayerInfo& player, ImVec2 position, bool selected);
    void RenderOnlineIndicator(ImVec2 position, bool online, bool inGame);
    
    // Context menus
    void ShowFriendContextMenu(const PlayerInfo& player);
    void ShowGuildMemberContextMenu(const PlayerInfo& member);
    
    // Search functionality
    void FilterBySearch();
    bool MatchesSearch(const std::string& name) const;
};

// Add friend dialog
class AddFriendDialog : public UIPanel {
public:
    AddFriendDialog();
    
    void Show();
    void Render() override;
    
    void SetOnAdd(std::function<void(const std::string&)> callback) {
        m_onAdd = callback;
    }
    
private:
    char m_nameBuffer[32];
    std::unique_ptr<GameButton> m_addBtn;
    std::unique_ptr<GameButton> m_cancelBtn;
    std::function<void(const std::string&)> m_onAdd;
};

} // namespace ArenaFighter