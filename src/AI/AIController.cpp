#include "AIController.h"
#include "../Characters/CharacterBase.h"
#include <fstream>
#include <sstream>

namespace ArenaFighter {

AIController::AIController() {
    RegisterDefaultNodes();
}

AIController::~AIController() = default;

bool AIController::Initialize(const std::string& treeXML) {
    try {
        // Load tree from file
        m_tree = std::make_unique<BT::Tree>(m_factory.createTreeFromFile(treeXML));
        m_blackboard = BT::Blackboard::create();
        return true;
    }
    catch (const std::exception& e) {
        // Log error
        return false;
    }
}

bool AIController::LoadTreeFromString(const std::string& xmlContent) {
    try {
        m_tree = std::make_unique<BT::Tree>(m_factory.createTreeFromText(xmlContent));
        m_blackboard = BT::Blackboard::create();
        return true;
    }
    catch (const std::exception& e) {
        return false;
    }
}

void AIController::SetControlledCharacter(CharacterBase* character) {
    m_controlledCharacter = character;

    // Update blackboard with character reference
    if (m_blackboard && character) {
        m_blackboard->set("character", character);
    }
}

void AIController::Update(float deltaTime) {
    if (!m_isActive || !m_tree || !m_controlledCharacter) {
        return;
    }

    // Accumulate time for fixed tick rate
    m_tickAccumulator += deltaTime;

    // Tick behavior tree at fixed intervals
    while (m_tickAccumulator >= m_tickInterval) {
        m_tree->tickOnce();
        m_tickAccumulator -= m_tickInterval;
    }
}

bool AIController::HasBlackboardValue(const std::string& key) const {
    if (!m_blackboard) {
        return false;
    }

    return m_blackboard->getKeys().count(key) > 0;
}

void AIController::Reset() {
    if (m_tree) {
        m_tree->haltTree();
    }

    if (m_blackboard) {
        // Clear all blackboard values except character
        auto character = m_controlledCharacter;
        m_blackboard = BT::Blackboard::create();
        if (character) {
            m_blackboard->set("character", character);
        }
    }

    m_tickAccumulator = 0.0f;
}

void AIController::RegisterDefaultNodes() {
    // TODO: Register custom action nodes here
    // Example:
    // m_factory.registerNodeType<AttackNode>("Attack");
    // m_factory.registerNodeType<MoveToTargetNode>("MoveToTarget");
    // m_factory.registerNodeType<UseSkillNode>("UseSkill");
    // m_factory.registerNodeType<FleeNode>("Flee");
}

// Preset Behavior Trees

std::string AIController::CreateAggressiveAI() {
    return R"(
<root BTCPP_format="4">
    <BehaviorTree ID="AggressiveAI">
        <Sequence>
            <Fallback>
                <Condition ID="IsTargetInRange"/>
                <Action ID="MoveToTarget"/>
            </Fallback>
            <Fallback>
                <Sequence>
                    <Condition ID="CanUseSkill"/>
                    <Action ID="UseStrongestSkill"/>
                </Sequence>
                <Action ID="BasicAttack"/>
            </Fallback>
        </Sequence>
    </BehaviorTree>
</root>
)";
}

std::string AIController::CreateDefensiveAI() {
    return R"(
<root BTCPP_format="4">
    <BehaviorTree ID="DefensiveAI">
        <Sequence>
            <Fallback>
                <Sequence>
                    <Condition ID="IsHealthLow"/>
                    <Action ID="Defend"/>
                </Sequence>
                <Sequence>
                    <Condition ID="IsTargetInRange"/>
                    <Action ID="CounterAttack"/>
                </Sequence>
                <Action ID="GuardPosition"/>
            </Fallback>
        </Sequence>
    </BehaviorTree>
</root>
)";
}

std::string AIController::CreateSupportAI() {
    return R"(
<root BTCPP_format="4">
    <BehaviorTree ID="SupportAI">
        <Sequence>
            <Fallback>
                <Sequence>
                    <Condition ID="IsAllyHealthLow"/>
                    <Action ID="HealAlly"/>
                </Sequence>
                <Sequence>
                    <Condition ID="CanBuffAlly"/>
                    <Action ID="BuffAlly"/>
                </Sequence>
                <Action ID="FollowOwner"/>
            </Fallback>
        </Sequence>
    </BehaviorTree>
</root>
)";
}

std::string AIController::CreateRangedAI() {
    return R"(
<root BTCPP_format="4">
    <BehaviorTree ID="RangedAI">
        <Sequence>
            <Fallback>
                <Sequence>
                    <Condition ID="IsTargetTooClose"/>
                    <Action ID="KeepDistance"/>
                </Sequence>
                <Sequence>
                    <Condition ID="IsInShootingRange"/>
                    <Action ID="RangedAttack"/>
                </Sequence>
                <Action ID="MoveToOptimalRange"/>
            </Fallback>
        </Sequence>
    </BehaviorTree>
</root>
)";
}

std::string AIController::CreateTankAI() {
    return R"(
<root BTCPP_format="4">
    <BehaviorTree ID="TankAI">
        <Sequence>
            <Action ID="DrawAggro"/>
            <Fallback>
                <Sequence>
                    <Condition ID="IsHealthCritical"/>
                    <Action ID="UseDefensiveSkill"/>
                </Sequence>
                <Sequence>
                    <Condition ID="IsTargetInRange"/>
                    <Action ID="TauntAttack"/>
                </Sequence>
                <Action ID="PositionForBlock"/>
            </Fallback>
        </Sequence>
    </BehaviorTree>
</root>
)";
}

std::string AIController::CreateAssassinAI() {
    return R"(
<root BTCPP_format="4">
    <BehaviorTree ID="AssassinAI">
        <Sequence>
            <Fallback>
                <Sequence>
                    <Condition ID="IsStealthed"/>
                    <Action ID="ApproachFromBehind"/>
                    <Action ID="CriticalStrike"/>
                </Sequence>
                <Sequence>
                    <Condition ID="CanStealth"/>
                    <Action ID="EnterStealth"/>
                </Sequence>
                <Sequence>
                    <Condition ID="IsHealthLow"/>
                    <Action ID="Disengage"/>
                </Sequence>
                <Action ID="QuickAttack"/>
            </Fallback>
        </Sequence>
    </BehaviorTree>
</root>
)";
}

} // namespace ArenaFighter
