#pragma once

#include "GameMode.h"
#include <string>
#include <queue>
#include <map>

namespace ArenaFighter {

// Tournament types
enum class TournamentType {
    SingleElimination,   // Lose once, eliminated
    DoubleElimination,  // Lose twice, eliminated
    RoundRobin,         // Everyone plays everyone
    Swiss,              // Swiss system pairing
    KingOfTheHill       // Winner stays, loser replaced
};

// Tournament match state
enum class TournamentMatchState {
    Pending,      // Not started
    InProgress,   // Currently playing
    Completed,    // Match finished
    Cancelled     // Match cancelled
};

// Tournament bracket position
struct BracketPosition {
    int round;
    int matchIndex;
    bool isLowerBracket; // For double elimination
};

// Tournament match
struct TournamentMatch {
    int matchId;
    int player1Id;
    int player2Id;
    int winnerId;
    int loserId;
    TournamentMatchState state;
    BracketPosition position;
    int bestOf; // Best of X games
    std::vector<int> gameWinners; // Winner of each game
    float startTime;
    float duration;
};

// Tournament player info
struct TournamentPlayer {
    int playerId;
    std::string playerName;
    int seed;
    int wins;
    int losses;
    int gamesWon;
    int gamesLost;
    bool isEliminated;
    int currentMatchId;
    std::vector<int> matchHistory;
};

// Tournament configuration
struct TournamentConfig : public MatchConfig {
    TournamentType type = TournamentType::SingleElimination;
    int maxParticipants = 16;          // Max players
    int bestOfGames = 3;               // Best of X per match
    bool allowSpectators = true;       // Allow spectating
    bool autoAdvance = true;           // Auto-advance on forfeit
    float matchTimeLimit = 600.0f;     // 10 minute match limit
    bool randomSeeding = false;        // Random vs skill-based seeding
    std::string tournamentName = "Community Tournament";
};

// Tournament Mode: Player-created tournaments with brackets
class TournamentMode : public GameMode {
private:
    TournamentConfig m_tournamentConfig;
    TournamentType m_tournamentType;
    
    // Tournament structure
    std::map<int, TournamentPlayer> m_participants;
    std::vector<TournamentMatch> m_matches;
    std::queue<int> m_pendingMatches;
    int m_currentMatchId;
    TournamentMatch* m_currentMatch;
    
    // Bracket structure
    int m_totalRounds;
    int m_currentRound;
    std::vector<std::vector<int>> m_bracketStructure; // [round][match indices]
    std::vector<std::vector<int>> m_lowerBracket;     // For double elim
    
    // Spectator management
    std::vector<int> m_spectatorIds;
    std::map<int, int> m_spectatorTargets; // spectator -> player being watched
    
    // Tournament state
    bool m_tournamentStarted;
    bool m_tournamentComplete;
    float m_tournamentStartTime;
    
    // Bracket generation
    void generateBracket();
    void generateSingleElimination();
    void generateDoubleElimination();
    void generateRoundRobin();
    void generateSwiss();
    void seedPlayers();
    
    // Match management
    void createMatch(int player1Id, int player2Id, BracketPosition position);
    void startNextMatch();
    void completeCurrentMatch(int winnerId);
    void advanceWinner(int matchId);
    void sendToLosersBracket(int playerId);
    void forfeitMatch(int playerId);
    
    // Player management
    void eliminatePlayer(int playerId);
    bool isPlayerActive(int playerId) const;
    int getPlayerOpponent(int playerId) const;
    
    // Bracket navigation
    TournamentMatch* getNextMatch(int matchId) const;
    std::vector<TournamentMatch*> getRoundMatches(int round) const;
    bool isRoundComplete(int round) const;
    
    // Spectator system
    void addSpectator(int spectatorId);
    void removeSpectator(int spectatorId);
    void switchSpectatorTarget(int spectatorId, int targetPlayerId);
    
    // Round management overrides
    virtual void startRound() override;
    virtual void endRound(int winnerId, WinCondition condition) override;
    virtual bool checkWinConditions() override;
    virtual int calculateRoundWinner() override;

public:
    explicit TournamentMode(const TournamentConfig& config);
    virtual ~TournamentMode() = default;
    
    // Game mode implementation
    virtual void initialize() override;
    virtual void update(float deltaTime) override;
    virtual void render() override;
    virtual void shutdown() override;
    
    // Player management
    virtual void addPlayer(std::shared_ptr<CharacterBase> character) override;
    virtual void removePlayer(int playerId) override;
    
    // Tournament management
    void registerParticipant(int playerId, const std::string& name, int seed = -1);
    void unregisterParticipant(int playerId);
    void startTournament();
    void pauseTournament();
    void resumeTournament();
    void cancelTournament();
    
    // Match control
    void startMatch(int matchId);
    void endMatch(int matchId, int winnerId);
    void reportMatchResult(int matchId, int winnerId);
    
    // Input handling
    virtual void handleInput(int playerId, const InputCommand& input) override;
    void handleSpectatorInput(int spectatorId, const InputCommand& input);
    
    // Tournament queries
    bool isTournamentStarted() const { return m_tournamentStarted; }
    bool isTournamentComplete() const { return m_tournamentComplete; }
    int getTournamentWinner() const;
    std::vector<int> getTopPlayers(int count) const;
    
    // Participant info
    TournamentPlayer getParticipant(int playerId) const;
    std::vector<TournamentPlayer> getAllParticipants() const;
    int getParticipantCount() const { return m_participants.size(); }
    
    // Match info
    TournamentMatch* getCurrentMatch() { return m_currentMatch; }
    TournamentMatch* getMatch(int matchId);
    std::vector<TournamentMatch> getAllMatches() const { return m_matches; }
    std::vector<TournamentMatch> getPendingMatches() const;
    
    // Bracket info
    std::vector<std::vector<int>> getBracketStructure() const { return m_bracketStructure; }
    int getCurrentRound() const { return m_currentRound; }
    int getTotalRounds() const { return m_totalRounds; }
    
    // Spectator info
    int getSpectatorCount() const { return m_spectatorIds.size(); }
    std::vector<int> getSpectators() const { return m_spectatorIds; }
    
    // Configuration
    void setTournamentConfig(const TournamentConfig& config) { m_tournamentConfig = config; }
    TournamentConfig getTournamentConfig() const { return m_tournamentConfig; }
    
    // Game mode properties
    virtual std::string getModeName() const override { 
        return "Tournament: " + m_tournamentConfig.tournamentName; 
    }
    virtual GameModeType getModeType() const override { return GameModeType::Tournament; }
    virtual bool supportsOnline() const override { return true; }
    virtual int getMinPlayers() const override { return 2; }
    virtual int getMaxPlayers() const override { return m_tournamentConfig.maxParticipants; }
    
    // Statistics
    float getAverageMatchDuration() const;
    int getLongestMatch() const;
    std::map<int, float> getPlayerWinRates() const;
};

} // namespace ArenaFighter