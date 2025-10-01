#include "TournamentMode.h"
#include <algorithm>
#include <random>
#include <cmath>

namespace ArenaFighter {

TournamentMode::TournamentMode(const TournamentConfig& config)
    : GameMode(config)
    , m_tournamentConfig(config)
    , m_tournamentType(config.type)
    , m_currentMatchId(-1)
    , m_currentMatch(nullptr)
    , m_totalRounds(0)
    , m_currentRound(0)
    , m_tournamentStarted(false)
    , m_tournamentComplete(false)
    , m_tournamentStartTime(0.0f) {
    
    // Update base config
    m_config.maxPlayers = 2; // Each match is 1v1
}

void TournamentMode::initialize() {
    GameMode::initialize();
    
    // Tournament initialized but not started
    // Players need to register first
}

void TournamentMode::update(float deltaTime) {
    GameMode::update(deltaTime);
    
    if (!m_tournamentStarted || m_tournamentComplete) return;
    
    // Check current match status
    if (m_currentMatch) {
        if (m_currentMatch->state == TournamentMatchState::InProgress) {
            m_currentMatch->duration += deltaTime;
            
            // Check for timeout
            if (m_currentMatch->duration > m_tournamentConfig.matchTimeLimit) {
                // Force end match - higher seed wins
                int winnerId = (m_participants[m_currentMatch->player1Id].seed < 
                               m_participants[m_currentMatch->player2Id].seed) ?
                               m_currentMatch->player1Id : m_currentMatch->player2Id;
                completeCurrentMatch(winnerId);
            }
        }
    } else if (!m_pendingMatches.empty()) {
        // Start next match
        startNextMatch();
    } else if (isRoundComplete(m_currentRound)) {
        // Advance to next round
        m_currentRound++;
        if (m_currentRound >= m_totalRounds) {
            m_tournamentComplete = true;
        }
    }
}

void TournamentMode::render() {
    GameMode::render();
    
    if (m_gameUI) {
        // Render tournament bracket
        m_gameUI->renderTournamentBracket(m_bracketStructure, m_matches);
        
        // Render current match info
        if (m_currentMatch) {
            m_gameUI->renderMatchInfo(*m_currentMatch);
        }
        
        // Render leaderboard
        auto topPlayers = getTopPlayers(8);
        m_gameUI->renderTournamentLeaderboard(topPlayers);
    }
}

void TournamentMode::shutdown() {
    m_participants.clear();
    m_matches.clear();
    while (!m_pendingMatches.empty()) {
        m_pendingMatches.pop();
    }
    m_bracketStructure.clear();
    m_lowerBracket.clear();
    m_spectatorIds.clear();
    m_spectatorTargets.clear();
    
    GameMode::shutdown();
}

void TournamentMode::addPlayer(std::shared_ptr<CharacterBase> character) {
    // In tournament mode, players are added when matches start
    // This is called internally when starting a match
    GameMode::addPlayer(character);
}

void TournamentMode::removePlayer(int playerId) {
    // Handle player leaving mid-tournament
    if (m_tournamentStarted && !m_tournamentComplete) {
        forfeitMatch(playerId);
    }
    
    GameMode::removePlayer(playerId);
}

void TournamentMode::registerParticipant(int playerId, const std::string& name, int seed) {
    if (m_tournamentStarted) return;
    if (m_participants.size() >= m_tournamentConfig.maxParticipants) return;
    
    TournamentPlayer player;
    player.playerId = playerId;
    player.playerName = name;
    player.seed = (seed < 0) ? m_participants.size() + 1 : seed;
    player.wins = 0;
    player.losses = 0;
    player.gamesWon = 0;
    player.gamesLost = 0;
    player.isEliminated = false;
    player.currentMatchId = -1;
    
    m_participants[playerId] = player;
    
    // Notify UI
    if (m_gameUI) {
        m_gameUI->onPlayerRegistered(playerId, name);
    }
}

void TournamentMode::unregisterParticipant(int playerId) {
    if (m_tournamentStarted) return;
    
    m_participants.erase(playerId);
}

void TournamentMode::startTournament() {
    if (m_tournamentStarted) return;
    if (m_participants.size() < 2) return;
    
    m_tournamentStarted = true;
    m_tournamentStartTime = 0.0f; // Will be set on first update
    
    // Seed players if needed
    if (m_tournamentConfig.randomSeeding) {
        seedPlayers();
    }
    
    // Generate bracket based on tournament type
    generateBracket();
    
    // Start first match
    if (!m_pendingMatches.empty()) {
        startNextMatch();
    }
}

void TournamentMode::pauseTournament() {
    if (m_currentMatch && m_currentMatch->state == TournamentMatchState::InProgress) {
        pauseGame();
    }
}

void TournamentMode::resumeTournament() {
    if (m_currentMatch && m_currentState == MatchState::Paused) {
        resumeGame();
    }
}

void TournamentMode::cancelTournament() {
    m_tournamentComplete = true;
    
    // Cancel current match
    if (m_currentMatch) {
        m_currentMatch->state = TournamentMatchState::Cancelled;
    }
    
    // Notify all participants
    if (m_gameUI) {
        m_gameUI->onTournamentCancelled();
    }
}

void TournamentMode::generateBracket() {
    switch (m_tournamentType) {
        case TournamentType::SingleElimination:
            generateSingleElimination();
            break;
            
        case TournamentType::DoubleElimination:
            generateDoubleElimination();
            break;
            
        case TournamentType::RoundRobin:
            generateRoundRobin();
            break;
            
        case TournamentType::Swiss:
            generateSwiss();
            break;
            
        case TournamentType::KingOfTheHill:
            // King of the Hill is dynamic, no bracket pre-generation
            m_totalRounds = m_participants.size() - 1;
            break;
    }
}

void TournamentMode::generateSingleElimination() {
    int playerCount = m_participants.size();
    m_totalRounds = static_cast<int>(std::ceil(std::log2(playerCount)));
    
    // Create bracket structure
    m_bracketStructure.clear();
    m_bracketStructure.resize(m_totalRounds);
    
    // Sort players by seed
    std::vector<TournamentPlayer> sortedPlayers;
    for (const auto& [id, player] : m_participants) {
        sortedPlayers.push_back(player);
    }
    std::sort(sortedPlayers.begin(), sortedPlayers.end(),
        [](const auto& a, const auto& b) { return a.seed < b.seed; });
    
    // Calculate number of byes needed
    int bracketSize = 1 << m_totalRounds; // 2^rounds
    int byeCount = bracketSize - playerCount;
    
    // Create first round matches
    int matchId = 0;
    int playerIndex = 0;
    
    for (int i = 0; i < bracketSize / 2; ++i) {
        BracketPosition pos{0, i, false};
        
        if (byeCount > 0 && i < byeCount) {
            // Give bye to highest seed
            // This match will auto-advance
            createMatch(sortedPlayers[playerIndex].playerId, -1, pos);
            playerIndex++;
        } else {
            // Normal match
            if (playerIndex < sortedPlayers.size() - 1) {
                int p1 = sortedPlayers[playerIndex].playerId;
                int p2 = sortedPlayers[playerIndex + 1].playerId;
                createMatch(p1, p2, pos);
                playerIndex += 2;
            }
        }
        
        m_bracketStructure[0].push_back(matchId++);
    }
    
    // Create placeholder matches for subsequent rounds
    for (int round = 1; round < m_totalRounds; ++round) {
        int matchesInRound = bracketSize / (2 << round);
        for (int i = 0; i < matchesInRound; ++i) {
            BracketPosition pos{round, i, false};
            createMatch(-1, -1, pos); // Players TBD
            m_bracketStructure[round].push_back(matchId++);
        }
    }
}

void TournamentMode::generateDoubleElimination() {
    // Double elimination has winner's bracket and loser's bracket
    int playerCount = m_participants.size();
    
    // Generate winner's bracket first (same as single elim)
    generateSingleElimination();
    
    // Add loser's bracket
    // Losers bracket has (2 * rounds - 1) rounds
    int loserRounds = (m_totalRounds * 2) - 1;
    m_lowerBracket.resize(loserRounds);
    
    // Create loser bracket matches
    int matchId = m_matches.size();
    for (int round = 0; round < loserRounds; ++round) {
        int matchesInRound;
        if (round % 2 == 0) {
            // Even rounds: new losers enter
            matchesInRound = (1 << (m_totalRounds - 1 - round / 2));
        } else {
            // Odd rounds: losers play each other
            matchesInRound = (1 << (m_totalRounds - 1 - (round + 1) / 2));
        }
        
        for (int i = 0; i < matchesInRound; ++i) {
            BracketPosition pos{round, i, true};
            createMatch(-1, -1, pos); // Players TBD
            m_lowerBracket[round].push_back(matchId++);
        }
    }
    
    // Grand finals
    BracketPosition grandFinalsPos{m_totalRounds + loserRounds, 0, false};
    createMatch(-1, -1, grandFinalsPos);
}

void TournamentMode::generateRoundRobin() {
    // Everyone plays everyone once
    std::vector<int> playerIds;
    for (const auto& [id, player] : m_participants) {
        playerIds.push_back(id);
    }
    
    int playerCount = playerIds.size();
    m_totalRounds = playerCount - 1;
    m_bracketStructure.resize(m_totalRounds);
    
    int matchId = 0;
    
    // Generate all pairings
    for (int round = 0; round < m_totalRounds; ++round) {
        for (int i = 0; i < playerCount / 2; ++i) {
            int p1Index = i;
            int p2Index = playerCount - 1 - i;
            
            if (p1Index != p2Index) {
                BracketPosition pos{round, i, false};
                createMatch(playerIds[p1Index], playerIds[p2Index], pos);
                m_bracketStructure[round].push_back(matchId++);
            }
        }
        
        // Rotate players (except first)
        if (playerCount > 2) {
            int temp = playerIds[playerCount - 1];
            for (int i = playerCount - 1; i > 1; --i) {
                playerIds[i] = playerIds[i - 1];
            }
            playerIds[1] = temp;
        }
    }
}

void TournamentMode::generateSwiss() {
    // Swiss system - players with similar records play each other
    // Initial round is like round robin first round
    int playerCount = m_participants.size();
    m_totalRounds = static_cast<int>(std::ceil(std::log2(playerCount)));
    m_bracketStructure.resize(m_totalRounds);
    
    // First round: pair by seed
    std::vector<int> playerIds;
    for (const auto& [id, player] : m_participants) {
        playerIds.push_back(id);
    }
    
    std::sort(playerIds.begin(), playerIds.end(),
        [this](int a, int b) {
            return m_participants[a].seed < m_participants[b].seed;
        });
    
    int matchId = 0;
    for (int i = 0; i < playerCount / 2; ++i) {
        BracketPosition pos{0, i, false};
        createMatch(playerIds[i * 2], playerIds[i * 2 + 1], pos);
        m_bracketStructure[0].push_back(matchId++);
    }
    
    // Subsequent rounds will be generated dynamically based on standings
}

void TournamentMode::seedPlayers() {
    std::vector<int> playerIds;
    for (auto& [id, player] : m_participants) {
        playerIds.push_back(id);
    }
    
    // Random shuffle
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(playerIds.begin(), playerIds.end(), g);
    
    // Assign seeds
    for (int i = 0; i < playerIds.size(); ++i) {
        m_participants[playerIds[i]].seed = i + 1;
    }
}

void TournamentMode::createMatch(int player1Id, int player2Id, BracketPosition position) {
    TournamentMatch match;
    match.matchId = m_matches.size();
    match.player1Id = player1Id;
    match.player2Id = player2Id;
    match.winnerId = -1;
    match.loserId = -1;
    match.state = TournamentMatchState::Pending;
    match.position = position;
    match.bestOf = m_tournamentConfig.bestOfGames;
    match.startTime = 0.0f;
    match.duration = 0.0f;
    
    m_matches.push_back(match);
    
    // Add to pending queue if both players are set
    if (player1Id >= 0 && player2Id >= 0) {
        m_pendingMatches.push(match.matchId);
    }
}

void TournamentMode::startNextMatch() {
    if (m_pendingMatches.empty()) return;
    
    int matchId = m_pendingMatches.front();
    m_pendingMatches.pop();
    
    startMatch(matchId);
}

void TournamentMode::startMatch(int matchId) {
    if (matchId < 0 || matchId >= m_matches.size()) return;
    
    m_currentMatchId = matchId;
    m_currentMatch = &m_matches[matchId];
    m_currentMatch->state = TournamentMatchState::InProgress;
    m_currentMatch->startTime = m_tournamentStartTime;
    
    // Update player states
    m_participants[m_currentMatch->player1Id].currentMatchId = matchId;
    m_participants[m_currentMatch->player2Id].currentMatchId = matchId;
    
    // Load players into game
    // This would interface with character selection in real implementation
    
    // Start the game round
    setState(MatchState::RoundStart);
}

void TournamentMode::completeCurrentMatch(int winnerId) {
    if (!m_currentMatch) return;
    
    m_currentMatch->winnerId = winnerId;
    m_currentMatch->loserId = (winnerId == m_currentMatch->player1Id) ? 
                               m_currentMatch->player2Id : m_currentMatch->player1Id;
    m_currentMatch->state = TournamentMatchState::Completed;
    
    // Update player records
    m_participants[winnerId].wins++;
    m_participants[winnerId].matchHistory.push_back(m_currentMatchId);
    
    m_participants[m_currentMatch->loserId].losses++;
    m_participants[m_currentMatch->loserId].matchHistory.push_back(m_currentMatchId);
    
    // Update game wins/losses
    for (int gameWinner : m_currentMatch->gameWinners) {
        if (gameWinner == m_currentMatch->player1Id) {
            m_participants[m_currentMatch->player1Id].gamesWon++;
            m_participants[m_currentMatch->player2Id].gamesLost++;
        } else {
            m_participants[m_currentMatch->player2Id].gamesWon++;
            m_participants[m_currentMatch->player1Id].gamesLost++;
        }
    }
    
    // Handle advancement based on tournament type
    switch (m_tournamentType) {
        case TournamentType::SingleElimination:
            eliminatePlayer(m_currentMatch->loserId);
            advanceWinner(m_currentMatchId);
            break;
            
        case TournamentType::DoubleElimination:
            if (m_currentMatch->position.isLowerBracket) {
                eliminatePlayer(m_currentMatch->loserId);
            } else {
                sendToLosersBracket(m_currentMatch->loserId);
            }
            advanceWinner(m_currentMatchId);
            break;
            
        case TournamentType::RoundRobin:
        case TournamentType::Swiss:
            // No elimination, just record results
            break;
            
        case TournamentType::KingOfTheHill:
            // Winner stays for next match
            if (!m_pendingMatches.empty()) {
                int nextChallenger = m_pendingMatches.front();
                m_pendingMatches.pop();
                createMatch(winnerId, nextChallenger, 
                           BracketPosition{m_currentRound + 1, 0, false});
            }
            break;
    }
    
    // Clear current match
    m_currentMatch = nullptr;
    m_currentMatchId = -1;
    
    // Check if tournament is complete
    if (m_tournamentType == TournamentType::SingleElimination ||
        m_tournamentType == TournamentType::DoubleElimination) {
        int activePlayers = 0;
        for (const auto& [id, player] : m_participants) {
            if (!player.isEliminated) activePlayers++;
        }
        if (activePlayers <= 1) {
            m_tournamentComplete = true;
        }
    }
}

void TournamentMode::advanceWinner(int matchId) {
    if (matchId < 0 || matchId >= m_matches.size()) return;
    
    const TournamentMatch& match = m_matches[matchId];
    TournamentMatch* nextMatch = getNextMatch(matchId);
    
    if (nextMatch) {
        // Place winner in next match
        if (nextMatch->player1Id < 0) {
            nextMatch->player1Id = match.winnerId;
        } else {
            nextMatch->player2Id = match.winnerId;
        }
        
        // If both players are set, add to pending
        if (nextMatch->player1Id >= 0 && nextMatch->player2Id >= 0) {
            m_pendingMatches.push(nextMatch->matchId);
        }
    }
}

void TournamentMode::sendToLosersBracket(int playerId) {
    // Find appropriate position in loser's bracket
    // Implementation depends on specific double elimination rules
}

void TournamentMode::forfeitMatch(int playerId) {
    if (!m_currentMatch) return;
    
    if (m_currentMatch->player1Id == playerId || m_currentMatch->player2Id == playerId) {
        int winnerId = (m_currentMatch->player1Id == playerId) ? 
                       m_currentMatch->player2Id : m_currentMatch->player1Id;
        completeCurrentMatch(winnerId);
    }
}

void TournamentMode::eliminatePlayer(int playerId) {
    if (m_participants.find(playerId) != m_participants.end()) {
        m_participants[playerId].isEliminated = true;
        m_participants[playerId].currentMatchId = -1;
    }
}

bool TournamentMode::isPlayerActive(int playerId) const {
    auto it = m_participants.find(playerId);
    return it != m_participants.end() && !it->second.isEliminated;
}

int TournamentMode::getPlayerOpponent(int playerId) const {
    if (m_currentMatch) {
        if (m_currentMatch->player1Id == playerId) return m_currentMatch->player2Id;
        if (m_currentMatch->player2Id == playerId) return m_currentMatch->player1Id;
    }
    return -1;
}

TournamentMatch* TournamentMode::getNextMatch(int matchId) const {
    // Find the match that this match feeds into
    const TournamentMatch& match = m_matches[matchId];
    int nextRound = match.position.round + 1;
    int nextMatchIndex = match.position.matchIndex / 2;
    
    if (nextRound < m_bracketStructure.size()) {
        if (nextMatchIndex < m_bracketStructure[nextRound].size()) {
            int nextMatchId = m_bracketStructure[nextRound][nextMatchIndex];
            return const_cast<TournamentMatch*>(&m_matches[nextMatchId]);
        }
    }
    
    return nullptr;
}

std::vector<TournamentMatch*> TournamentMode::getRoundMatches(int round) const {
    std::vector<TournamentMatch*> roundMatches;
    
    for (auto& match : const_cast<std::vector<TournamentMatch>&>(m_matches)) {
        if (match.position.round == round) {
            roundMatches.push_back(&match);
        }
    }
    
    return roundMatches;
}

bool TournamentMode::isRoundComplete(int round) const {
    auto roundMatches = getRoundMatches(round);
    
    for (const auto* match : roundMatches) {
        if (match->state != TournamentMatchState::Completed) {
            return false;
        }
    }
    
    return true;
}

void TournamentMode::addSpectator(int spectatorId) {
    m_spectatorIds.push_back(spectatorId);
    
    // Default to watching current match
    if (m_currentMatch) {
        m_spectatorTargets[spectatorId] = m_currentMatch->player1Id;
    }
}

void TournamentMode::removeSpectator(int spectatorId) {
    m_spectatorIds.erase(
        std::remove(m_spectatorIds.begin(), m_spectatorIds.end(), spectatorId),
        m_spectatorIds.end()
    );
    m_spectatorTargets.erase(spectatorId);
}

void TournamentMode::switchSpectatorTarget(int spectatorId, int targetPlayerId) {
    if (isPlayerActive(targetPlayerId)) {
        m_spectatorTargets[spectatorId] = targetPlayerId;
    }
}

void TournamentMode::handleInput(int playerId, const InputCommand& input) {
    // Check if player is in current match
    if (m_currentMatch && 
        (m_currentMatch->player1Id == playerId || m_currentMatch->player2Id == playerId)) {
        GameMode::handleInput(playerId, input);
    }
}

void TournamentMode::handleSpectatorInput(int spectatorId, const InputCommand& input) {
    // Handle spectator-specific commands
    if (input.command == "next_player") {
        // Cycle to next active player
        auto it = m_spectatorTargets.find(spectatorId);
        if (it != m_spectatorTargets.end()) {
            // Find next active player
            for (const auto& [id, player] : m_participants) {
                if (!player.isEliminated && id > it->second) {
                    switchSpectatorTarget(spectatorId, id);
                    break;
                }
            }
        }
    }
}

void TournamentMode::startRound() {
    GameMode::startRound();
    
    // Track game results for best-of series
    if (m_currentMatch) {
        m_currentMatch->gameWinners.clear();
    }
}

void TournamentMode::endRound(int winnerId, WinCondition condition) {
    if (m_currentMatch) {
        m_currentMatch->gameWinners.push_back(winnerId);
        
        // Check if match is complete (best of X)
        int p1Wins = std::count(m_currentMatch->gameWinners.begin(), 
                               m_currentMatch->gameWinners.end(), 
                               m_currentMatch->player1Id);
        int p2Wins = std::count(m_currentMatch->gameWinners.begin(), 
                               m_currentMatch->gameWinners.end(), 
                               m_currentMatch->player2Id);
        
        int winsNeeded = (m_currentMatch->bestOf + 1) / 2;
        
        if (p1Wins >= winsNeeded) {
            completeCurrentMatch(m_currentMatch->player1Id);
        } else if (p2Wins >= winsNeeded) {
            completeCurrentMatch(m_currentMatch->player2Id);
        } else {
            // Play another game
            startRound();
            return;
        }
    }
    
    GameMode::endRound(winnerId, condition);
}

bool TournamentMode::checkWinConditions() {
    // Tournament mode uses standard win conditions per game
    return GameMode::checkWinConditions();
}

int TournamentMode::calculateRoundWinner() {
    // Tournament mode uses standard winner calculation per game
    return GameMode::calculateRoundWinner();
}

void TournamentMode::endMatch(int matchId, int winnerId) {
    if (matchId == m_currentMatchId) {
        completeCurrentMatch(winnerId);
    }
}

void TournamentMode::reportMatchResult(int matchId, int winnerId) {
    if (matchId >= 0 && matchId < m_matches.size()) {
        m_matches[matchId].winnerId = winnerId;
        m_matches[matchId].state = TournamentMatchState::Completed;
        advanceWinner(matchId);
    }
}

int TournamentMode::getTournamentWinner() const {
    if (!m_tournamentComplete) return -1;
    
    // Find player with no losses (single elim) or fewest losses
    int winnerId = -1;
    int minLosses = INT_MAX;
    
    for (const auto& [id, player] : m_participants) {
        if (player.losses < minLosses) {
            minLosses = player.losses;
            winnerId = id;
        }
    }
    
    return winnerId;
}

std::vector<int> TournamentMode::getTopPlayers(int count) const {
    std::vector<TournamentPlayer> players;
    for (const auto& [id, player] : m_participants) {
        players.push_back(player);
    }
    
    // Sort by wins (desc), then by losses (asc)
    std::sort(players.begin(), players.end(),
        [](const auto& a, const auto& b) {
            if (a.wins != b.wins) return a.wins > b.wins;
            if (a.losses != b.losses) return a.losses < b.losses;
            return a.gamesWon - a.gamesLost > b.gamesWon - b.gamesLost;
        });
    
    std::vector<int> topIds;
    for (int i = 0; i < std::min(count, static_cast<int>(players.size())); ++i) {
        topIds.push_back(players[i].playerId);
    }
    
    return topIds;
}

TournamentPlayer TournamentMode::getParticipant(int playerId) const {
    auto it = m_participants.find(playerId);
    if (it != m_participants.end()) {
        return it->second;
    }
    return TournamentPlayer();
}

std::vector<TournamentPlayer> TournamentMode::getAllParticipants() const {
    std::vector<TournamentPlayer> players;
    for (const auto& [id, player] : m_participants) {
        players.push_back(player);
    }
    return players;
}

TournamentMatch* TournamentMode::getMatch(int matchId) {
    if (matchId >= 0 && matchId < m_matches.size()) {
        return &m_matches[matchId];
    }
    return nullptr;
}

std::vector<TournamentMatch> TournamentMode::getPendingMatches() const {
    std::vector<TournamentMatch> pending;
    
    for (const auto& match : m_matches) {
        if (match.state == TournamentMatchState::Pending) {
            pending.push_back(match);
        }
    }
    
    return pending;
}

float TournamentMode::getAverageMatchDuration() const {
    if (m_matches.empty()) return 0.0f;
    
    float totalDuration = 0.0f;
    int completedMatches = 0;
    
    for (const auto& match : m_matches) {
        if (match.state == TournamentMatchState::Completed) {
            totalDuration += match.duration;
            completedMatches++;
        }
    }
    
    return completedMatches > 0 ? totalDuration / completedMatches : 0.0f;
}

int TournamentMode::getLongestMatch() const {
    int longestId = -1;
    float longestDuration = 0.0f;
    
    for (const auto& match : m_matches) {
        if (match.duration > longestDuration) {
            longestDuration = match.duration;
            longestId = match.matchId;
        }
    }
    
    return longestId;
}

std::map<int, float> TournamentMode::getPlayerWinRates() const {
    std::map<int, float> winRates;
    
    for (const auto& [id, player] : m_participants) {
        int totalGames = player.gamesWon + player.gamesLost;
        if (totalGames > 0) {
            winRates[id] = static_cast<float>(player.gamesWon) / totalGames;
        } else {
            winRates[id] = 0.0f;
        }
    }
    
    return winRates;
}

} // namespace ArenaFighter