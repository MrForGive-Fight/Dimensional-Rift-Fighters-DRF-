#pragma once

#include <behaviortree_cpp/bt_factory.h>
#include <behaviortree_cpp/behavior_tree.h>
#include <memory>
#include <string>

namespace ArenaFighter {

// Forward declarations
class CharacterBase;

/**
 * AIController - Behavior tree-based AI controller
 *
 * Used for:
 * - Yuito's 12 AI pets (Bone Soldier, Fire Drake, Spirit Wolf, etc.)
 * - Hyoudou's 3 god clones (Vulcanus, Mercurius, Diana)
 * - Miss Bat's blood puppets
 * - Enemy AI in PvE modes
 */
class AIController {
public:
    AIController();
    ~AIController();

    /**
     * Initialize AI with behavior tree XML
     * @param treeXML Path to behavior tree XML file
     * @return true if initialized successfully
     */
    bool Initialize(const std::string& treeXML);

    /**
     * Load behavior tree from XML string
     */
    bool LoadTreeFromString(const std::string& xmlContent);

    /**
     * Set the controlled character
     */
    void SetControlledCharacter(CharacterBase* character);

    /**
     * Get controlled character
     */
    CharacterBase* GetControlledCharacter() const { return m_controlledCharacter; }

    /**
     * Update AI (tick behavior tree)
     * @param deltaTime Time since last update
     */
    void Update(float deltaTime);

    /**
     * Set blackboard value (for AI state management)
     */
    template<typename T>
    void SetBlackboardValue(const std::string& key, const T& value) {
        if (m_blackboard) {
            m_blackboard->set(key, value);
        }
    }

    /**
     * Get blackboard value
     */
    template<typename T>
    T GetBlackboardValue(const std::string& key) const {
        if (m_blackboard) {
            return m_blackboard->get<T>(key);
        }
        return T();
    }

    /**
     * Check if blackboard has key
     */
    bool HasBlackboardValue(const std::string& key) const;

    /**
     * Reset the behavior tree
     */
    void Reset();

    /**
     * Check if AI is active
     */
    bool IsActive() const { return m_isActive; }

    /**
     * Activate/deactivate AI
     */
    void SetActive(bool active) { m_isActive = active; }

    /**
     * Get the behavior tree factory (for registering custom nodes)
     */
    BT::BehaviorTreeFactory& GetFactory() { return m_factory; }

    /**
     * Create preset behavior trees for common AI patterns
     */
    static std::string CreateAggressiveAI();      // Attack-focused
    static std::string CreateDefensiveAI();       // Guard/protect
    static std::string CreateSupportAI();         // Heal/buff allies
    static std::string CreateRangedAI();          // Stay at distance, shoot
    static std::string CreateTankAI();            // Draw aggro, absorb damage
    static std::string CreateAssassinAI();        // Stealth, critical hits

private:
    // Register default action nodes
    void RegisterDefaultNodes();

private:
    BT::BehaviorTreeFactory m_factory;
    std::unique_ptr<BT::Tree> m_tree;
    std::shared_ptr<BT::Blackboard> m_blackboard;

    CharacterBase* m_controlledCharacter = nullptr;
    bool m_isActive = true;
    float m_tickInterval = 0.1f;  // Tick every 100ms
    float m_tickAccumulator = 0.0f;
};

} // namespace ArenaFighter
