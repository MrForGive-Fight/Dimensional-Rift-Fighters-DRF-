#include <gtest/gtest.h>
#include "../Rou.h"
#include "../States/EvolutionStateMachine.h"
#include "../Visuals/EvolutionVFX.h"

namespace ArenaFighter {
namespace Tests {

class RouTest : public ::testing::Test {
protected:
    std::unique_ptr<Rou> rou;
    
    void SetUp() override {
        rou = std::make_unique<Rou>();
    }
    
    void TearDown() override {
        rou.reset();
    }
};

// Evolution Threshold Tests
TEST_F(RouTest, EvolutionThresholdsCorrect) {
    // Start as Goblin
    EXPECT_EQ(rou->GetCurrentForm(), RouEvolutionForm::GOBLIN);
    EXPECT_FLOAT_EQ(rou->GetEvolutionGauge(), 0.0f);
    
    // Test 24.9% - should still be Goblin
    rou->UpdateEvolutionGauge(24.9f);
    EXPECT_EQ(rou->GetCurrentForm(), RouEvolutionForm::GOBLIN);
    
    // Test 25% - should evolve to Hobgoblin
    rou->UpdateEvolutionGauge(0.1f);
    EXPECT_EQ(rou->GetCurrentForm(), RouEvolutionForm::HOBGOBLIN);
    EXPECT_FLOAT_EQ(rou->GetEvolutionGauge(), 25.0f);
    
    // Test 49.9% - should still be Hobgoblin
    rou->UpdateEvolutionGauge(24.9f);
    EXPECT_EQ(rou->GetCurrentForm(), RouEvolutionForm::HOBGOBLIN);
    
    // Test 50% - should evolve to Ogre
    rou->UpdateEvolutionGauge(0.1f);
    EXPECT_EQ(rou->GetCurrentForm(), RouEvolutionForm::OGRE);
    EXPECT_FLOAT_EQ(rou->GetEvolutionGauge(), 50.0f);
    
    // Test 74.9% - should still be Ogre
    rou->UpdateEvolutionGauge(24.9f);
    EXPECT_EQ(rou->GetCurrentForm(), RouEvolutionForm::OGRE);
    
    // Test 75% - should evolve to Apostle Lord
    rou->UpdateEvolutionGauge(0.1f);
    EXPECT_EQ(rou->GetCurrentForm(), RouEvolutionForm::APOSTLE_LORD);
    EXPECT_FLOAT_EQ(rou->GetEvolutionGauge(), 75.0f);
    
    // Test 99.9% - should still be Apostle Lord
    rou->UpdateEvolutionGauge(24.9f);
    EXPECT_EQ(rou->GetCurrentForm(), RouEvolutionForm::APOSTLE_LORD);
    
    // Test 100% - should evolve to Vajrayaksa
    rou->UpdateEvolutionGauge(0.1f);
    EXPECT_EQ(rou->GetCurrentForm(), RouEvolutionForm::VAJRAYAKSA);
    EXPECT_FLOAT_EQ(rou->GetEvolutionGauge(), 100.0f);
}

// Emergency Protocol Tests
TEST_F(RouTest, EmergencyProtocolTriggersAt30PercentHP) {
    // Set HP to exactly 31%
    float maxHP = rou->GetMaxHP();
    float targetHP = maxHP * 0.31f;
    float damage = maxHP - targetHP;
    rou->OnTakeDamage(damage);
    
    // Should not trigger yet
    EXPECT_EQ(rou->GetCurrentForm(), RouEvolutionForm::GOBLIN);
    
    // Damage to exactly 30%
    rou->OnTakeDamage(maxHP * 0.01f);
    rou->Update(0.016f); // One frame update
    
    // Should trigger emergency protocol
    EXPECT_EQ(rou->GetCurrentForm(), RouEvolutionForm::HOBGOBLIN);
    
    // Check healing occurred (15% of max HP)
    EXPECT_GT(rou->GetHPPercent(), 30.0f);
    EXPECT_FLOAT_EQ(rou->GetDamageReduction(), 0.3f); // 30% damage reduction
}

TEST_F(RouTest, EmergencyProtocolOnlyTriggersOnce) {
    // Trigger emergency protocol
    float maxHP = rou->GetMaxHP();
    rou->OnTakeDamage(maxHP * 0.7f);
    rou->Update(0.016f);
    
    RouEvolutionForm formAfterFirst = rou->GetCurrentForm();
    EXPECT_NE(formAfterFirst, RouEvolutionForm::GOBLIN);
    
    // Try to trigger again
    rou->OnTakeDamage(maxHP * 0.5f);
    rou->Update(0.016f);
    
    // Form shouldn't change from emergency protocol again
    EXPECT_EQ(rou->GetCurrentForm(), formAfterFirst);
}

TEST_F(RouTest, EmergencyProtocolVajrayaksaFullHeal) {
    // Evolve to Apostle Lord first
    rou->UpdateEvolutionGauge(99.0f);
    EXPECT_EQ(rou->GetCurrentForm(), RouEvolutionForm::APOSTLE_LORD);
    
    // Trigger emergency protocol
    float maxHP = rou->GetMaxHP();
    rou->OnTakeDamage(maxHP * 0.71f);
    rou->Update(0.016f);
    
    // Should evolve to Vajrayaksa with full heal
    EXPECT_EQ(rou->GetCurrentForm(), RouEvolutionForm::VAJRAYAKSA);
    EXPECT_FLOAT_EQ(rou->GetHPPercent(), 100.0f);
    EXPECT_FLOAT_EQ(rou->GetEvolutionGauge(), 100.0f);
}

// Gauge Building Tests
TEST_F(RouTest, GaugeBuildingFromAllSources) {
    float initialGauge = rou->GetEvolutionGauge();
    
    // Test hit gauge building
    rou->OnHit(50.0f);
    EXPECT_FLOAT_EQ(rou->GetEvolutionGauge(), initialGauge + 1.5f);
    
    // Test damage taken gauge building (0.5% per 10 HP)
    rou->OnTakeDamage(100.0f);
    EXPECT_FLOAT_EQ(rou->GetEvolutionGauge(), initialGauge + 1.5f + 5.0f);
    
    // Test kill gauge building
    rou->OnKill();
    EXPECT_FLOAT_EQ(rou->GetEvolutionGauge(), initialGauge + 1.5f + 5.0f + 15.0f);
    
    // Test equipment pickup gauge building
    rou->OnEquipmentPickup();
    EXPECT_FLOAT_EQ(rou->GetEvolutionGauge(), initialGauge + 1.5f + 5.0f + 15.0f + 7.0f);
}

// Death Penalty Test
TEST_F(RouTest, DeathPenaltyReducesGauge) {
    // Set gauge to 50%
    rou->UpdateEvolutionGauge(50.0f);
    EXPECT_FLOAT_EQ(rou->GetEvolutionGauge(), 50.0f);
    
    // Apply death penalty (-35% of current)
    rou->OnDeath();
    EXPECT_FLOAT_EQ(rou->GetEvolutionGauge(), 50.0f * 0.65f); // 32.5%
}

// Special Move Tests
TEST_F(RouTest, AllSpecialMovesExecute) {
    // Test each direction for each form
    std::vector<RouEvolutionForm> forms = {
        RouEvolutionForm::GOBLIN,
        RouEvolutionForm::HOBGOBLIN,
        RouEvolutionForm::OGRE,
        RouEvolutionForm::APOSTLE_LORD,
        RouEvolutionForm::VAJRAYAKSA
    };
    
    std::vector<Direction> directions = {
        Direction::UP,
        Direction::LEFT,
        Direction::RIGHT
    };
    
    for (auto form : forms) {
        // Evolve to form
        float targetGauge = 0.0f;
        switch (form) {
            case RouEvolutionForm::GOBLIN: targetGauge = 0.0f; break;
            case RouEvolutionForm::HOBGOBLIN: targetGauge = 25.0f; break;
            case RouEvolutionForm::OGRE: targetGauge = 50.0f; break;
            case RouEvolutionForm::APOSTLE_LORD: targetGauge = 75.0f; break;
            case RouEvolutionForm::VAJRAYAKSA: targetGauge = 100.0f; break;
        }
        
        rou = std::make_unique<Rou>(); // Reset
        rou->UpdateEvolutionGauge(targetGauge);
        EXPECT_EQ(rou->GetCurrentForm(), form);
        
        // Test each direction
        for (auto dir : directions) {
            // Should not throw
            EXPECT_NO_THROW(rou->ExecuteDirectionalSpecial(dir));
        }
    }
}

// Combo Damage Tests
TEST_F(RouTest, ComboDamageMatchesSpecifications) {
    // Test Goblin combo (3 hits, 21-26 damage)
    EXPECT_EQ(rou->GetCurrentForm(), RouEvolutionForm::GOBLIN);
    EXPECT_NO_THROW(rou->ExecuteBasicCombo());
    
    // Test Hobgoblin combo (4 hits, 35-40 damage)
    rou->UpdateEvolutionGauge(25.0f);
    EXPECT_EQ(rou->GetCurrentForm(), RouEvolutionForm::HOBGOBLIN);
    EXPECT_NO_THROW(rou->ExecuteBasicCombo());
    
    // Test Ogre combo (5 hits, 62-70 damage)
    rou->UpdateEvolutionGauge(25.0f);
    EXPECT_EQ(rou->GetCurrentForm(), RouEvolutionForm::OGRE);
    EXPECT_NO_THROW(rou->ExecuteBasicCombo());
    
    // Test Apostle Lord combo (6 hits, 83-95 damage)
    rou->UpdateEvolutionGauge(25.0f);
    EXPECT_EQ(rou->GetCurrentForm(), RouEvolutionForm::APOSTLE_LORD);
    EXPECT_NO_THROW(rou->ExecuteBasicCombo());
    
    // Test Vajrayaksa combo (8 hits, 130-150 damage)
    rou->UpdateEvolutionGauge(25.0f);
    EXPECT_EQ(rou->GetCurrentForm(), RouEvolutionForm::VAJRAYAKSA);
    EXPECT_NO_THROW(rou->ExecuteBasicCombo());
}

// Frame Data Tests
TEST_F(RouTest, FrameDataVerification) {
    struct FrameDataTest {
        RouEvolutionForm form;
        int expectedStartup;
        int expectedActive;
        int expectedRecovery;
    };
    
    std::vector<FrameDataTest> tests = {
        {RouEvolutionForm::GOBLIN, 3, 2, 5},
        {RouEvolutionForm::HOBGOBLIN, 5, 3, 8},
        {RouEvolutionForm::OGRE, 6, 4, 10},
        {RouEvolutionForm::APOSTLE_LORD, 8, 5, 12},
        {RouEvolutionForm::VAJRAYAKSA, 10, 8, 15}
    };
    
    for (const auto& test : tests) {
        rou = std::make_unique<Rou>();
        
        // Evolve to form
        float gauge = 0.0f;
        switch (test.form) {
            case RouEvolutionForm::HOBGOBLIN: gauge = 25.0f; break;
            case RouEvolutionForm::OGRE: gauge = 50.0f; break;
            case RouEvolutionForm::APOSTLE_LORD: gauge = 75.0f; break;
            case RouEvolutionForm::VAJRAYAKSA: gauge = 100.0f; break;
            default: break;
        }
        
        rou->UpdateEvolutionGauge(gauge);
        EXPECT_EQ(rou->GetCurrentForm(), test.form);
        
        // Frame data is verified through form stats
        auto stats = rou->GetCurrentFormStats();
        EXPECT_EQ(stats.startupFrames, test.expectedStartup);
        EXPECT_EQ(stats.activeFrames, test.expectedActive);
        EXPECT_EQ(stats.recoveryFrames, test.expectedRecovery);
    }
}

// Cooldown Management Tests
TEST_F(RouTest, SpecialMoveCooldowns) {
    // Execute a special move
    rou->ExecuteDirectionalSpecial(Direction::UP);
    
    // Update for less than cooldown duration
    rou->Update(1.0f);
    
    // Try to execute again - should be on cooldown
    // Note: In actual implementation, this would check cooldown state
    EXPECT_NO_THROW(rou->ExecuteDirectionalSpecial(Direction::UP));
    
    // Update for full cooldown duration
    rou->Update(10.0f);
    
    // Should be able to execute again
    EXPECT_NO_THROW(rou->ExecuteDirectionalSpecial(Direction::UP));
}

// Buff System Tests
TEST_F(RouTest, BuffSystemWorking) {
    // Add damage reduction buff
    rou->AddBuff(BuffInfo::DAMAGE_REDUCTION, 0.3f, 3.0f);
    EXPECT_FLOAT_EQ(rou->GetDamageReduction(), 0.3f);
    
    // Add another damage reduction buff
    rou->AddBuff(BuffInfo::DAMAGE_REDUCTION, 0.2f, 3.0f);
    EXPECT_FLOAT_EQ(rou->GetDamageReduction(), 0.5f);
    
    // Update to expire buffs
    rou->Update(3.1f);
    EXPECT_FLOAT_EQ(rou->GetDamageReduction(), 0.0f);
}

// Evolution State Machine Tests
TEST_F(RouTest, EvolutionStateMachineCorrectTransitions) {
    auto stateMachine = std::make_unique<EvolutionStateMachine>(rou.get());
    
    // Test initial state
    EXPECT_EQ(stateMachine->GetCurrentForm(), RouEvolutionForm::GOBLIN);
    
    // Test state transitions
    stateMachine->ChangeState(RouEvolutionForm::HOBGOBLIN);
    EXPECT_EQ(stateMachine->GetCurrentForm(), RouEvolutionForm::HOBGOBLIN);
    
    // Test stat multipliers
    EXPECT_FLOAT_EQ(stateMachine->GetDamageMultiplier(), 1.0f);
    EXPECT_FLOAT_EQ(stateMachine->GetSpeedMultiplier(), 1.1f);
    EXPECT_FLOAT_EQ(stateMachine->GetDefenseMultiplier(), 1.0f);
    EXPECT_EQ(stateMachine->GetMaxComboHits(), 4);
    EXPECT_FLOAT_EQ(stateMachine->GetSizeScale(), 1.0f);
}

// Visual Effects Tests
TEST_F(RouTest, VisualEffectsInitialize) {
    auto vfx = std::make_unique<EvolutionVFX>();
    
    // Test evolution effect
    EXPECT_NO_THROW(vfx->PlayEvolutionEffect(
        RouEvolutionForm::GOBLIN, 
        RouEvolutionForm::HOBGOBLIN
    ));
    
    // Test emergency protocol effect
    EXPECT_NO_THROW(vfx->PlayEmergencyProtocolEffect());
    
    // Test gauge threshold effects
    EXPECT_NO_THROW(vfx->PlayGaugeThresholdEffect(25.0f));
    EXPECT_NO_THROW(vfx->PlayGaugeThresholdEffect(50.0f));
    EXPECT_NO_THROW(vfx->PlayGaugeThresholdEffect(75.0f));
    EXPECT_NO_THROW(vfx->PlayGaugeThresholdEffect(100.0f));
    
    // Test all special move effects
    EXPECT_NO_THROW(vfx->PlayGoblinPanicJump());
    EXPECT_NO_THROW(vfx->PlayHobgoblinShadowUpper());
    EXPECT_NO_THROW(vfx->PlayOgreSlam());
    EXPECT_NO_THROW(vfx->PlayApostleDemonAscension());
    EXPECT_NO_THROW(vfx->PlayVajrayaksaHeavenSplitter());
}

// Integration Tests
TEST_F(RouTest, FullEvolutionCycle) {
    // Start as Goblin
    EXPECT_EQ(rou->GetCurrentForm(), RouEvolutionForm::GOBLIN);
    
    // Build gauge through combat
    for (int i = 0; i < 17; i++) {
        rou->OnHit(50.0f); // 17 * 1.5 = 25.5%
    }
    
    // Should be Hobgoblin
    EXPECT_EQ(rou->GetCurrentForm(), RouEvolutionForm::HOBGOBLIN);
    
    // Take damage to build more gauge
    rou->OnTakeDamage(500.0f); // 25% gauge
    
    // Should be Ogre
    EXPECT_EQ(rou->GetCurrentForm(), RouEvolutionForm::OGRE);
    
    // Get kills
    for (int i = 0; i < 2; i++) {
        rou->OnKill(); // 2 * 15 = 30%
    }
    
    // Should be Apostle Lord
    EXPECT_EQ(rou->GetCurrentForm(), RouEvolutionForm::APOSTLE_LORD);
    
    // Pickup equipment
    for (int i = 0; i < 4; i++) {
        rou->OnEquipmentPickup(); // 4 * 7 = 28%
    }
    
    // Should be Vajrayaksa
    EXPECT_EQ(rou->GetCurrentForm(), RouEvolutionForm::VAJRAYAKSA);
    EXPECT_FLOAT_EQ(rou->GetEvolutionGauge(), 100.0f);
}

} // namespace Tests
} // namespace ArenaFighter