#pragma once

#include <memory>
#include <vector>
#include <string>

namespace ArenaFighter {

enum class GameMode {
    Arena_1v1,
    Arena_2v2,
    Arena_4v4,
    CrownControl,    // King of the Hill variant
    Survival,        // Wave-based PvE
    Tournament,      // Bracket system
    Training,        // Practice mode
    CustomLobby      // Player-hosted games
};

class GameModeBase {
public:
    virtual ~GameModeBase() = default;
    
    virtual bool Initialize() = 0;
    virtual void Start() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void End() = 0;
    
    virtual bool IsMatchComplete() const = 0;
    virtual int GetWinner() const = 0;
    
    GameMode GetType() const { return m_type; }
    
protected:
    GameMode m_type;
    bool m_isActive = false;
    float m_matchTime = 0.0f;
    int m_maxPlayers = 8;
};

class Arena1v1Mode : public GameModeBase {
public:
    Arena1v1Mode();
    bool Initialize() override;
    void Start() override;
    void Update(float deltaTime) override;
    void End() override;
    bool IsMatchComplete() const override;
    int GetWinner() const override;
    
private:
    static constexpr int ROUNDS_TO_WIN = 3;
    int m_player1Rounds = 0;
    int m_player2Rounds = 0;
    float m_roundTime = 99.0f;
};

class GameModesManager {
public:
    GameModesManager() = default;
    ~GameModesManager() = default;

    bool Initialize();
    void Shutdown();
    void Update(float deltaTime);
    void RenderUI(class RenderingSystem* renderer);

    // Mode Management
    bool StartGameMode(GameMode mode);
    void EndCurrentMode();
    GameModeBase* GetCurrentMode() { return m_currentMode.get(); }
    
    // Match Making
    void QueueForRankedMatch(int playerID);
    void CreateCustomLobby(const std::string& lobbyName, GameMode mode);
    void JoinLobby(const std::string& lobbyCode);
    
    // Tournament System
    void CreateTournament(int maxPlayers);
    void JoinTournament(int playerID);
    void StartTournament();
    
    // Statistics
    struct PlayerStats {
        int wins = 0;
        int losses = 0;
        int rankPoints = 1000;
        std::string rank = "Bronze";
    };
    PlayerStats GetPlayerStats(int playerID) const;

private:
    std::unique_ptr<GameModeBase> m_currentMode;
    std::vector<int> m_rankedQueue;
    
    // Ranking system
    void UpdateRankings(int winnerID, int loserID);
    std::string CalculateRank(int rankPoints) const;
};

} // namespace ArenaFighter