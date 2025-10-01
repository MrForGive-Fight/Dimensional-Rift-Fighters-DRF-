#pragma once

#include "UIPanel.h"
#include "UILabel.h"
#include "CombatUI.h"
#include <memory>

namespace ArenaFighter {

constexpr int MAX_COMBO_HITS = 15;

enum class GameModeType {
    Ranked1v1,
    TeamDeathmatch,
    BeastMode,
    ForGlory,
    DimensionalRift,
    Tournament
};

class GameModeUI : public UIPanel {
private:
    GameModeType m_gameMode;
    std::shared_ptr<CombatUI> m_player1UI;
    std::shared_ptr<CombatUI> m_player2UI;
    std::shared_ptr<UILabel> m_timerLabel;
    std::shared_ptr<UILabel> m_modeLabel;
    
    float m_matchTimer;
    float m_roundTimer;
    int m_roundNumber;

public:
    GameModeUI(const std::string& id, GameModeType mode);
    
    void update(float deltaTime) override;
    void setMatchTime(float seconds);
    void setRoundNumber(int round);
    void updatePlayerHealth(int playerIndex, float health);
    void updatePlayerMana(int playerIndex, float mana);
    void updateCombo(int playerIndex, int combo);
    
    float getPlayerMana(int playerIndex) const;
    bool canUseMana(int playerIndex, float manaCost) const;
    void consumeMana(int playerIndex, float manaCost);
    
    GameModeType getGameMode() const { return m_gameMode; }
};

} // namespace ArenaFighter