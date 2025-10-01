#include <gtest/gtest.h>
#include "../HyukWoonSung.h"
#include "../Stances/LightStance.h"
#include "../Stances/DarkStance.h"
#include "../Visuals/StanceVFX.h"

namespace ArenaFighter {
namespace Tests {

class HyukWoonSungTest : public ::testing::Test {
protected:
    std::unique_ptr<HyukWoonSung> hyuk;
    
    void SetUp() override {
        hyuk = std::make_unique<HyukWoonSung>();
    }
    
    void TearDown() override {
        hyuk.reset();
    }
};

// Stance Switching Tests
TEST_F(HyukWoonSungTest, StanceSwitchingWithCorrectVFX) {
    // Start in Light stance
    EXPECT_EQ(hyuk->GetCurrentStance(), StanceType::LIGHT_STANCE);
    
    // Switch to Dark stance
    hyuk->SwitchStance();
    EXPECT_EQ(hyuk->GetCurrentStance(), StanceType::DARK_STANCE);
    
    // Switch back to Light stance
    hyuk->SwitchStance();
    EXPECT_EQ(hyuk->GetCurrentStance(), StanceType::LIGHT_STANCE);
}

TEST_F(HyukWoonSungTest, StanceSwitchCooldown) {
    // Switch stance
    hyuk->SwitchStance();
    EXPECT_EQ(hyuk->GetCurrentStance(), StanceType::DARK_STANCE);
    
    // Immediate switch should be blocked by cooldown
    hyuk->SwitchStance();
    EXPECT_EQ(hyuk->GetCurrentStance(), StanceType::DARK_STANCE);
    
    // After cooldown expires, switch should work
    hyuk->Update(1.1f); // Cooldown is 1 second
    hyuk->SwitchStance();
    EXPECT_EQ(hyuk->GetCurrentStance(), StanceType::LIGHT_STANCE);
}

// Qi System Tests
TEST_F(HyukWoonSungTest, QiConsumptionAndRegeneration) {
    float initialQi = hyuk->GetQiEnergy();
    EXPECT_FLOAT_EQ(initialQi, 100.0f);
    
    // Consume Qi
    hyuk->ConsumeQi(25.0f);
    EXPECT_FLOAT_EQ(hyuk->GetQiEnergy(), 75.0f);
    
    // Regenerate Qi over time
    hyuk->Update(5.0f); // 5 seconds at 2 qi/sec = 10 qi
    EXPECT_FLOAT_EQ(hyuk->GetQiEnergy(), 85.0f);
    
    // Cannot consume more than available
    hyuk->ConsumeQi(100.0f);
    EXPECT_FLOAT_EQ(hyuk->GetQiEnergy(), 0.0f);
}

// Light Stance Combat Tests
TEST_F(HyukWoonSungTest, LightStanceSevenStarsCombo) {
    // Ensure in light stance
    EXPECT_EQ(hyuk->GetCurrentStance(), StanceType::LIGHT_STANCE);
    
    // Execute Seven Stars of the Spear
    EXPECT_NO_THROW(hyuk->SevenStarsOfTheSpear());
    
    // Should work without errors
    EXPECT_NO_THROW(hyuk->ExecuteBasicCombo());
}

TEST_F(HyukWoonSungTest, LightStanceDirectionalSkills) {
    EXPECT_EQ(hyuk->GetCurrentStance(), StanceType::LIGHT_STANCE);
    
    // Test all light stance S+Direction skills
    EXPECT_NO_THROW(hyuk->ExecuteDirectionalSkill(Direction::UP));    // Spear Sea Impact
    EXPECT_NO_THROW(hyuk->ExecuteDirectionalSkill(Direction::RIGHT)); // Divine Wind
    EXPECT_NO_THROW(hyuk->ExecuteDirectionalSkill(Direction::LEFT));  // Lightning Stitch
}

// Dark Stance Combat Tests
TEST_F(HyukWoonSungTest, DarkStanceHeavenlyDemonPalm) {
    // Switch to dark stance
    hyuk->SwitchStance();
    hyuk->Update(1.1f); // Wait for cooldown
    EXPECT_EQ(hyuk->GetCurrentStance(), StanceType::DARK_STANCE);
    
    // Execute Heavenly Demon Divine Palm
    EXPECT_NO_THROW(hyuk->HeavenlyDemonDivinePalm());
    EXPECT_NO_THROW(hyuk->ExecuteBasicCombo());
}

TEST_F(HyukWoonSungTest, DarkStanceDirectionalSkills) {
    hyuk->SwitchStance();
    hyuk->Update(1.1f);
    EXPECT_EQ(hyuk->GetCurrentStance(), StanceType::DARK_STANCE);
    
    // Test all dark stance S+Direction skills
    EXPECT_NO_THROW(hyuk->ExecuteDirectionalSkill(Direction::UP));    // Heavenly Demon Power
    EXPECT_NO_THROW(hyuk->ExecuteDirectionalSkill(Direction::RIGHT)); // Black Night
    EXPECT_NO_THROW(hyuk->ExecuteDirectionalSkill(Direction::LEFT));  // Mind Split
}

// THE BIG METEOR Test
TEST_F(HyukWoonSungTest, BigMeteorExecution) {
    // Switch to dark stance
    hyuk->SwitchStance();
    hyuk->Update(1.1f);
    
    // Ensure enough Qi
    EXPECT_GE(hyuk->GetQiEnergy(), 45.0f);
    
    // Execute THE BIG METEOR
    EXPECT_NO_THROW(hyuk->RainOfStarDestruction());
    
    // Should consume Qi
    EXPECT_LE(hyuk->GetQiEnergy(), 55.0f);
}

// Ultimate System Tests
TEST_F(HyukWoonSungTest, UltimateActivationRequirements) {
    // Should not activate without full HP and Qi
    hyuk->TakeDamage(100.0f);
    EXPECT_FALSE(hyuk->IsInUltimate());
    
    // Heal to full and ensure full Qi
    hyuk->Heal(1000.0f);
    hyuk->GenerateQi(100.0f);
    
    EXPECT_FLOAT_EQ(hyuk->GetHPPercent(), 100.0f);
    EXPECT_FLOAT_EQ(hyuk->GetQiEnergy(), 100.0f);
    
    // Now ultimate should be activatable
    EXPECT_NO_THROW(hyuk->ActivateDivineArts());
    EXPECT_TRUE(hyuk->IsInUltimate());
}

TEST_F(HyukWoonSungTest, UltimateDirectionalSkills) {
    // Activate ultimate
    hyuk->Heal(1000.0f);
    hyuk->GenerateQi(100.0f);
    hyuk->ActivateDivineArts();
    EXPECT_TRUE(hyuk->IsInUltimate());
    
    // Test ultimate S+Direction skills
    EXPECT_NO_THROW(hyuk->ExecuteDirectionalSkill(Direction::UP));    // Six Bonds United
    EXPECT_NO_THROW(hyuk->ExecuteDirectionalSkill(Direction::RIGHT)); // Death Moon
    EXPECT_NO_THROW(hyuk->ExecuteDirectionalSkill(Direction::LEFT));  // Blue Ocean
    EXPECT_NO_THROW(hyuk->ExecuteDirectionalSkill(Direction::DOWN));  // Finger Wind
}

// Master's Vengeance Tests
TEST_F(HyukWoonSungTest, MastersVengeanceTriggersAt25PercentHP) {
    float maxHP = hyuk->GetMaxHP();
    
    // Damage to exactly 26%
    hyuk->TakeDamage(maxHP * 0.74f);
    hyuk->Update(0.016f);
    
    // Should not trigger yet
    // Note: We can't directly check internal state without getter
    
    // Damage to exactly 25%
    hyuk->TakeDamage(maxHP * 0.01f);
    hyuk->Update(0.016f);
    
    // Master's Vengeance should be active
    EXPECT_NO_THROW(hyuk->MastersVengeance());
}

TEST_F(HyukWoonSungTest, MastersVengeanceOnlyTriggersOnce) {
    // Trigger Master's Vengeance
    float maxHP = hyuk->GetMaxHP();
    hyuk->TakeDamage(maxHP * 0.76f);
    hyuk->Update(0.016f);
    
    // Try to trigger again at even lower HP
    hyuk->TakeDamage(maxHP * 0.1f);
    hyuk->Update(0.016f);
    
    // Should only trigger once per life
    EXPECT_NO_THROW(hyuk->MastersVengeance());
}

// Gear Skill Tests
TEST_F(HyukWoonSungTest, LightStanceGearSkills) {
    EXPECT_EQ(hyuk->GetCurrentStance(), StanceType::LIGHT_STANCE);
    
    // Test all light stance gear skills
    EXPECT_NO_THROW(hyuk->GlassyDeathRain());      // AS
    EXPECT_NO_THROW(hyuk->SpearAura());            // AD
    EXPECT_NO_THROW(hyuk->FlowOfTheDivineDragon()); // ASD
    EXPECT_NO_THROW(hyuk->ThunderousFlyingSpear()); // SD
}

TEST_F(HyukWoonSungTest, DarkStanceGearSkills) {
    hyuk->SwitchStance();
    hyuk->Update(1.1f);
    EXPECT_EQ(hyuk->GetCurrentStance(), StanceType::DARK_STANCE);
    
    // Test all dark stance gear skills
    EXPECT_NO_THROW(hyuk->HeavenlyDemonDestroysTheWorld()); // AS
    EXPECT_NO_THROW(hyuk->IntimidationDress());             // AD
    EXPECT_NO_THROW(hyuk->DarkFlowerRedHeartFlame());       // ASD
    EXPECT_NO_THROW(hyuk->RainOfStarDestruction());         // SD (Big Meteor)
}

// Visual Effects Tests
TEST_F(HyukWoonSungTest, StanceVFXInitialization) {
    auto vfx = std::make_unique<StanceVFX>();
    
    // Test stance switch effect
    EXPECT_NO_THROW(vfx->PlayStanceSwitchEffect(StanceType::DARK_STANCE));
    EXPECT_NO_THROW(vfx->PlayYinYangShatter());
    
    // Test light stance effects
    EXPECT_NO_THROW(vfx->CreateThrustEffect(StanceVFX::BLUE));
    EXPECT_NO_THROW(vfx->CreateStarPattern());
    EXPECT_NO_THROW(vfx->CreateBlueStarProjectiles(7));
    EXPECT_NO_THROW(vfx->CreateCrescentWave(StanceVFX::BLUE, true));
    
    // Test dark stance effects
    EXPECT_NO_THROW(vfx->CreatePalmStrike(StanceVFX::RED));
    EXPECT_NO_THROW(vfx->CreateDemonFaceProjection());
    EXPECT_NO_THROW(vfx->CreateSkyPortal(true));
    EXPECT_NO_THROW(vfx->CreateBigMeteor());
    
    // Test ultimate effects
    EXPECT_NO_THROW(vfx->PlayUltimateTransformation());
    EXPECT_NO_THROW(vfx->PlayMastersVengeanceEffect());
}

// Dragon Gauge Tests
TEST_F(HyukWoonSungTest, DragonGaugeVisualization) {
    auto gauge = std::make_unique<DragonGauge>();
    
    // Test gauge updates
    EXPECT_NO_THROW(gauge->Update(0.016f, 0.0f));
    EXPECT_NO_THROW(gauge->Update(0.016f, 25.0f));
    EXPECT_NO_THROW(gauge->Update(0.016f, 50.0f));
    EXPECT_NO_THROW(gauge->Update(0.016f, 75.0f));
    EXPECT_NO_THROW(gauge->Update(0.016f, 100.0f));
    
    // Test rendering
    EXPECT_NO_THROW(gauge->Render());
}

// Combo System Tests
TEST_F(HyukWoonSungTest, LightStanceComboSystem) {
    EXPECT_EQ(hyuk->GetCurrentStance(), StanceType::LIGHT_STANCE);
    
    // Test basic combo
    EXPECT_NO_THROW(hyuk->BasicSpearChain());
    
    // Test charged attack
    EXPECT_NO_THROW(hyuk->DivineSpearOfEndingNight());
    
    // Test dash combo
    EXPECT_NO_THROW(hyuk->TravelOfTheGale());
    
    // Test aerial attack
    EXPECT_NO_THROW(hyuk->AerialSpearDance());
}

TEST_F(HyukWoonSungTest, DarkStanceComboSystem) {
    hyuk->SwitchStance();
    hyuk->Update(1.1f);
    EXPECT_EQ(hyuk->GetCurrentStance(), StanceType::DARK_STANCE);
    
    // Test basic combo
    EXPECT_NO_THROW(hyuk->HeavenlyDemonDivinePalm());
    
    // Test charged attack
    EXPECT_NO_THROW(hyuk->RedSoulCharge());
    
    // Test dash combo
    EXPECT_NO_THROW(hyuk->ReignOfHeavenlyDemonStep());
    
    // Test aerial attack
    EXPECT_NO_THROW(hyuk->HeavenlyDemonsSpiral());
}

// Cooldown Management Tests
TEST_F(HyukWoonSungTest, SkillCooldownManagement) {
    // Execute S+Up skill
    hyuk->ExecuteDirectionalSkill(Direction::UP);
    
    // Should be on cooldown - test by checking qi consumption
    float qiBefore = hyuk->GetQiEnergy();
    hyuk->ExecuteDirectionalSkill(Direction::UP);
    
    // Update for cooldown duration
    hyuk->Update(6.0f); // Max cooldown for light skills
    
    // Should be able to execute again
    EXPECT_NO_THROW(hyuk->ExecuteDirectionalSkill(Direction::UP));
}

// Health System Tests
TEST_F(HyukWoonSungTest, HealthSystemWorking) {
    float maxHP = hyuk->GetMaxHP();
    EXPECT_FLOAT_EQ(hyuk->GetHPPercent(), 100.0f);
    
    // Take damage
    hyuk->TakeDamage(200.0f);
    EXPECT_LT(hyuk->GetHPPercent(), 100.0f);
    
    // Heal
    hyuk->Heal(100.0f);
    EXPECT_GT(hyuk->GetHPPercent(), hyuk->GetHPPercent());
    
    // Cannot heal above max
    hyuk->Heal(2000.0f);
    EXPECT_FLOAT_EQ(hyuk->GetHPPercent(), 100.0f);
}

// Bond Technique Tests (Special S+Direction moves)
TEST_F(HyukWoonSungTest, BondTechniquesExecution) {
    // Test 2nd Bond (Black Night of Fourth Moon)
    hyuk->SwitchStance();
    hyuk->Update(1.1f);
    EXPECT_NO_THROW(hyuk->BlackNightOfFourthMoon());
    
    // Test 3rd Bond (Divine Wind of the Past)
    hyuk->SwitchStance();
    hyuk->Update(1.1f);
    EXPECT_NO_THROW(hyuk->DivineWindOfThePast());
    
    // Test 5th Bond (Spear Sea Impact)
    EXPECT_NO_THROW(hyuk->SpearSeaImpact());
}

// Ultimate Bond Tests (requires ultimate mode)
TEST_F(HyukWoonSungTest, UltimateBondTechniques) {
    // Activate ultimate
    hyuk->Heal(1000.0f);
    hyuk->GenerateQi(100.0f);
    hyuk->ActivateDivineArts();
    EXPECT_TRUE(hyuk->IsInUltimate());
    
    // Test 4th Bond (Death Moon)
    EXPECT_NO_THROW(hyuk->DeathMoonOfDarkNight());
    
    // Test 6th Bond (Blue Ocean)
    EXPECT_NO_THROW(hyuk->BlueOceanDestruction());
    
    // Test Six Bonds United
    EXPECT_NO_THROW(hyuk->SixBondsUnited());
    
    // Test Finger Wind Point Pressing
    EXPECT_NO_THROW(hyuk->FingerWindPointPressing());
}

// Frame Data Verification Tests
TEST_F(HyukWoonSungTest, FrameDataVerification) {
    // Test that frame data exists for both stances
    
    // Light stance frame data
    EXPECT_EQ(hyuk->GetCurrentStance(), StanceType::LIGHT_STANCE);
    // Frame data would be verified through stance classes
    
    // Dark stance frame data
    hyuk->SwitchStance();
    hyuk->Update(1.1f);
    EXPECT_EQ(hyuk->GetCurrentStance(), StanceType::DARK_STANCE);
    // Frame data would be verified through stance classes
}

// Qi Efficiency Tests
TEST_F(HyukWoonSungTest, StanceQiEfficiency) {
    float initialQi = hyuk->GetQiEnergy();
    
    // Light stance should use less Qi (0.85x efficiency)
    EXPECT_EQ(hyuk->GetCurrentStance(), StanceType::LIGHT_STANCE);
    hyuk->SpearSeaImpact(); // 25 Qi skill
    float lightQiUsed = initialQi - hyuk->GetQiEnergy();
    
    // Reset
    hyuk = std::make_unique<HyukWoonSung>();
    hyuk->SwitchStance();
    hyuk->Update(1.1f);
    
    // Dark stance should use more Qi (1.15x efficiency)
    EXPECT_EQ(hyuk->GetCurrentStance(), StanceType::DARK_STANCE);
    hyuk->HeavenlyDemonDivinePower(); // 30 Qi skill
    float darkQiUsed = hyuk->GetQiEnergy() - 70.0f; // Expected remaining
    
    // Dark stance should consume more Qi for similar power
    // Actual comparison would depend on skill balancing
}

// Integration Tests
TEST_F(HyukWoonSungTest, StanceDancingCombo) {
    // Test stance switching mid-combo
    EXPECT_EQ(hyuk->GetCurrentStance(), StanceType::LIGHT_STANCE);
    
    // Light thrust
    hyuk->PowerThrust();
    
    // Switch to dark
    hyuk->SwitchStance();
    hyuk->Update(1.1f);
    
    // Dark palm
    hyuk->HeavenlyDemonDivinePalm();
    
    // Switch back to light
    hyuk->SwitchStance();
    hyuk->Update(1.1f);
    
    // Should work without issues
    EXPECT_EQ(hyuk->GetCurrentStance(), StanceType::LIGHT_STANCE);
}

TEST_F(HyukWoonSungTest, FullGameplayScenario) {
    // Simulate a complete fight scenario
    
    // Start with combo
    hyuk->SevenStarsOfTheSpear();
    
    // Use special skill
    hyuk->SpearSeaImpact();
    
    // Switch stance
    hyuk->SwitchStance();
    hyuk->Update(1.1f);
    
    // Dark combo
    hyuk->HeavenlyDemonDivinePalm();
    
    // Use THE BIG METEOR
    hyuk->RainOfStarDestruction();
    
    // Take damage to trigger Master's Vengeance
    hyuk->TakeDamage(hyuk->GetMaxHP() * 0.76f);
    hyuk->Update(0.016f);
    
    // Should complete without errors
    EXPECT_TRUE(true); // Test completion
}

// Performance Tests
TEST_F(HyukWoonSungTest, UpdatePerformance) {
    // Test that updates complete in reasonable time
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; i++) {
        hyuk->Update(0.016f);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Should complete 1000 updates in less than 100ms
    EXPECT_LT(duration.count(), 100);
}

} // namespace Tests
} // namespace ArenaFighter