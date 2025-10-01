#include "GameModeUI.h"
#include <cstdio>

namespace ArenaFighter {

GameModeUI::GameModeUI(const std::string& id, GameModeType mode)
    : UIPanel(id, XMFLOAT2(0, 0), XMFLOAT2(1920, 1080)),
      m_gameMode(mode),
      m_matchTimer(0.0f),
      m_roundTimer(0.0f),
      m_roundNumber(1) {
    
    setBackgroundColor(XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));
    
    // Player 1 UI (left side)
    m_player1UI = std::make_shared<CombatUI>("Player1UI", XMFLOAT2(50, 50));
    addChild(m_player1UI);
    
    // Player 2 UI (right side - mirrored)
    m_player2UI = std::make_shared<CombatUI>("Player2UI", XMFLOAT2(1570, 50));
    addChild(m_player2UI);
    
    // Timer display (center top)
    m_timerLabel = std::make_shared<UILabel>("Timer", XMFLOAT2(910, 30), "99", 48.0f);
    m_timerLabel->setCenterAlign(true);
    m_timerLabel->setTextColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
    addChild(m_timerLabel);
    
    // Game mode label
    std::string modeName;
    switch (mode) {
        case GameModeType::Ranked1v1: modeName = "RANKED 1V1"; break;
        case GameModeType::TeamDeathmatch: modeName = "TEAM DEATHMATCH"; break;
        case GameModeType::BeastMode: modeName = "BEAST MODE"; break;
        case GameModeType::ForGlory: modeName = "FOR GLORY"; break;
        case GameModeType::DimensionalRift: modeName = "DIMENSIONAL RIFT"; break;
        case GameModeType::Tournament: modeName = "TOURNAMENT"; break;
    }
    
    m_modeLabel = std::make_shared<UILabel>("GameMode", XMFLOAT2(910, 80), modeName, 24.0f);
    m_modeLabel->setCenterAlign(true);
    m_modeLabel->setTextColor(XMFLOAT4(0.8f, 0.7f, 0.2f, 1.0f));
    addChild(m_modeLabel);
}

void GameModeUI::update(float deltaTime) {
    UIPanel::update(deltaTime);
    
    // Update match timer
    m_matchTimer -= deltaTime;
    if (m_matchTimer < 0) m_matchTimer = 0;
    
    // Update timer display
    int seconds = static_cast<int>(m_matchTimer);
    char buffer[16];
    sprintf_s(buffer, "%02d", seconds);
    m_timerLabel->setText(buffer);
    
    // Flash timer when low
    if (m_matchTimer < 10.0f) {
        float flash = (static_cast<int>(m_matchTimer * 2) % 2) ? 1.0f : 0.5f;
        m_timerLabel->setTextColor(XMFLOAT4(1.0f, flash * 0.3f, flash * 0.3f, 1.0f));
    }
}

void GameModeUI::setMatchTime(float seconds) {
    m_matchTimer = seconds;
}

void GameModeUI::setRoundNumber(int round) {
    m_roundNumber = round;
}

void GameModeUI::updatePlayerHealth(int playerIndex, float health) {
    if (playerIndex == 0) {
        m_player1UI->setHealth(health);
    } else if (playerIndex == 1) {
        m_player2UI->setHealth(health);
    }
}

void GameModeUI::updatePlayerMana(int playerIndex, float mana) {
    if (playerIndex == 0) {
        m_player1UI->setMana(mana);
    } else if (playerIndex == 1) {
        m_player2UI->setMana(mana);
    }
}

void GameModeUI::updateCombo(int playerIndex, int combo) {
    if (combo > MAX_COMBO_HITS) {
        combo = MAX_COMBO_HITS;
    }
    
    if (playerIndex == 0) {
        m_player1UI->setComboCount(combo);
    } else if (playerIndex == 1) {
        m_player2UI->setComboCount(combo);
    }
}

float GameModeUI::getPlayerMana(int playerIndex) const {
    if (playerIndex == 0) {
        return m_player1UI->getManaValue();
    } else if (playerIndex == 1) {
        return m_player2UI->getManaValue();
    }
    return 0.0f;
}

bool GameModeUI::canUseMana(int playerIndex, float manaCost) const {
    return getPlayerMana(playerIndex) >= manaCost;
}

void GameModeUI::consumeMana(int playerIndex, float manaCost) {
    float currentMana = getPlayerMana(playerIndex);
    updatePlayerMana(playerIndex, currentMana - manaCost);
}

} // namespace ArenaFighter