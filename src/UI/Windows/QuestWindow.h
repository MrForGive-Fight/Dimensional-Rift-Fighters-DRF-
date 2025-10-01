#pragma once
#include "../Components/UIPanel.h"
#include "../Components/GameButton.h"
#include "../Components/TabbedPanel.h"
#include <vector>
#include <memory>

namespace ArenaFighter {

struct QuestInfo {
    std::string id;
    std::string name;
    std::string description;
    std::string category;
    int currentProgress;
    int maxProgress;
    bool isCompleted;
    bool isActive;
    
    // Rewards
    struct Reward {
        std::string type; // "exp", "gold", "item"
        std::string name;
        int quantity;
        void* icon;
    };
    std::vector<Reward> rewards;
    
    // Requirements
    int levelRequirement;
    std::vector<std::string> prerequisiteQuests;
};

class QuestWindow : public UIPanel {
public:
    QuestWindow();
    ~QuestWindow();
    
    void Initialize();
    void Update(float deltaTime) override;
    void Render() override;
    
    // Quest management
    void RefreshQuestList();
    void AcceptQuest(const std::string& questId);
    void AbandonQuest(const std::string& questId);
    void CompleteQuest(const std::string& questId);
    
private:
    // Quest categories
    struct QuestCategory {
        std::string name;
        std::vector<QuestInfo> quests;
        std::unique_ptr<GameButton> tabButton;
        int completedCount;
        int totalCount;
    };
    
    std::vector<QuestCategory> m_categories;
    int m_selectedCategory;
    int m_selectedQuest;
    
    // UI Components
    std::unique_ptr<UIPanel> m_questListPanel;
    std::unique_ptr<UIPanel> m_questDetailsPanel;
    std::unique_ptr<GameButton> m_participateBtn;
    std::unique_ptr<GameButton> m_abandonBtn;
    std::unique_ptr<GameButton> m_trackBtn;
    
    // Scroll positions
    ImVec2 m_listScrollPos;
    ImVec2 m_detailsScrollPos;
    
    // Rendering sections
    void RenderCategoryTabs();
    void RenderQuestList();
    void RenderQuestDetails();
    void RenderQuestEntry(const QuestInfo& quest, ImVec2 position, bool selected);
    void RenderProgressBar(ImVec2 position, ImVec2 size, float progress, const std::string& text);
    void RenderRewards(const std::vector<QuestInfo::Reward>& rewards, ImVec2 position);
    
    // Filtering
    bool m_showCompleted;
    bool m_showActive;
    std::string m_searchFilter;
    
    void FilterQuests();
    bool MatchesFilter(const QuestInfo& quest) const;
};

// Daily quest tracker overlay
class DailyQuestTracker : public UIPanel {
public:
    DailyQuestTracker();
    
    void SetTrackedQuests(const std::vector<QuestInfo>& quests);
    void Update(float deltaTime) override;
    void Render() override;
    
    void SetCompact(bool compact) { m_compactMode = compact; }
    
private:
    std::vector<QuestInfo> m_trackedQuests;
    bool m_compactMode;
    float m_fadeTimer;
    
    void RenderCompact();
    void RenderExpanded();
};

} // namespace ArenaFighter