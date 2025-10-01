#pragma once

#include "CombatEnums.h"

namespace ArenaFighter {

/**
 * @brief Frame data structure for attacks following LSFDC standards
 * 
 * Frame data defines the timing properties of attacks:
 * - Startup: Frames before attack becomes active
 * - Active: Frames where attack can hit
 * - Recovery: Frames after active until neutral
 * - Total: Startup + Active + Recovery
 */
struct FrameData {
    // Timing properties (in frames, 60 FPS)
    int startupFrames;      // Time before hitbox appears
    int activeFrames;       // Time hitbox is active
    int recoveryFrames;     // Time after hitbox disappears
    
    // Hit properties
    int hitstunFrames;      // Stun on hit
    int blockstunFrames;    // Stun on block
    float hitstunDecay;     // Hitstun multiplier in combos
    
    // Damage and cost
    float baseDamage;       // Base damage value
    float manaCost;         // Mana required to use
    float meterGain;        // Meter gained on hit
    
    // Movement properties
    float forwardMovement;  // Forward movement during attack
    float verticalMovement; // Vertical movement (for anti-airs, etc.)
    float knockbackForce;   // Base knockback strength
    float knockbackAngle;   // Knockback direction (degrees)
    
    // Hit properties
    AttackType attackType;
    HitState hitProperty;   // Normal, Counter, Critical, Punish
    bool canCancel;         // Can cancel into other moves
    bool isProjectile;      // Creates a projectile
    bool isGrab;           // Unblockable grab
    bool isArmored;        // Has armor frames
    
    // Cancel windows
    int cancelWindowStart;  // Frame when cancels become available
    int cancelWindowEnd;    // Frame when cancel window closes
    
    // Constructor with defaults
    FrameData() 
        : startupFrames(10)
        , activeFrames(3)
        , recoveryFrames(15)
        , hitstunFrames(15)
        , blockstunFrames(11)
        , hitstunDecay(0.95f)
        , baseDamage(100.0f)
        , manaCost(10.0f)
        , meterGain(10.0f)
        , forwardMovement(0.0f)
        , verticalMovement(0.0f)
        , knockbackForce(5.0f)
        , knockbackAngle(45.0f)
        , attackType(AttackType::Medium)
        , hitProperty(HitState::Normal)
        , canCancel(false)
        , isProjectile(false)
        , isGrab(false)
        , isArmored(false)
        , cancelWindowStart(-1)
        , cancelWindowEnd(-1) {}
    
    // Helper methods
    int GetTotalFrames() const { 
        return startupFrames + activeFrames + recoveryFrames; 
    }
    
    bool IsInStartup(int currentFrame) const {
        return currentFrame < startupFrames;
    }
    
    bool IsActive(int currentFrame) const {
        return currentFrame >= startupFrames && 
               currentFrame < (startupFrames + activeFrames);
    }
    
    bool IsInRecovery(int currentFrame) const {
        return currentFrame >= (startupFrames + activeFrames);
    }
    
    bool CanBeCanceled(int currentFrame) const {
        if (!canCancel || cancelWindowStart < 0) return false;
        return currentFrame >= cancelWindowStart && 
               currentFrame <= cancelWindowEnd;
    }
    
    float GetFrameAdvantageOnHit() const {
        return static_cast<float>(hitstunFrames - recoveryFrames);
    }
    
    float GetFrameAdvantageOnBlock() const {
        return static_cast<float>(blockstunFrames - recoveryFrames);
    }
};

/**
 * @brief Standard frame data presets following CLAUDE.md guidelines
 */
namespace FrameDataPresets {
    // Light attacks: Fast startup, low damage
    inline FrameData CreateLightAttack() {
        FrameData data;
        data.startupFrames = 5;
        data.activeFrames = 2;
        data.recoveryFrames = 8;
        data.hitstunFrames = 12;
        data.blockstunFrames = 8;
        data.baseDamage = 50.0f;
        data.manaCost = 5.0f;
        data.attackType = AttackType::Light;
        data.canCancel = true;
        data.cancelWindowStart = 7;
        data.cancelWindowEnd = 12;
        return data;
    }
    
    // Medium attacks: Balanced
    inline FrameData CreateMediumAttack() {
        FrameData data;
        data.startupFrames = 10;
        data.activeFrames = 3;
        data.recoveryFrames = 15;
        data.hitstunFrames = 18;
        data.blockstunFrames = 14;
        data.baseDamage = 100.0f;
        data.manaCost = 15.0f;
        data.attackType = AttackType::Medium;
        data.canCancel = true;
        data.cancelWindowStart = 13;
        data.cancelWindowEnd = 20;
        return data;
    }
    
    // Heavy attacks: Slow but powerful
    inline FrameData CreateHeavyAttack() {
        FrameData data;
        data.startupFrames = 18;
        data.activeFrames = 5;
        data.recoveryFrames = 25;
        data.hitstunFrames = 25;
        data.blockstunFrames = 20;
        data.baseDamage = 180.0f;
        data.manaCost = 25.0f;
        data.attackType = AttackType::Heavy;
        data.knockbackForce = 10.0f;
        data.canCancel = false;
        return data;
    }
    
    // Special moves: Unique properties
    inline FrameData CreateSpecialMove() {
        FrameData data;
        data.startupFrames = 15;
        data.activeFrames = 4;
        data.recoveryFrames = 20;
        data.hitstunFrames = 22;
        data.blockstunFrames = 16;
        data.baseDamage = 150.0f;
        data.manaCost = 30.0f;
        data.attackType = AttackType::Special;
        data.meterGain = 20.0f;
        return data;
    }
    
    // Ultimate skills: High cost, high reward
    inline FrameData CreateUltimateSkill() {
        FrameData data;
        data.startupFrames = 25;
        data.activeFrames = 8;
        data.recoveryFrames = 35;
        data.hitstunFrames = 40;
        data.blockstunFrames = 25;
        data.baseDamage = 300.0f;
        data.manaCost = 60.0f;
        data.attackType = AttackType::Ultimate;
        data.meterGain = 0.0f;  // Ultimates don't build meter
        data.knockbackForce = 20.0f;
        data.isArmored = true;
        return data;
    }
}

/**
 * @brief Combat state information for characters
 */
struct CombatState {
    int hitstunFrames = 0;
    int blockstunFrames = 0;
    int armorFrames = 0;
    int invulnerabilityFrames = 0;
    bool isCounterHit = false;
    bool isAirborne = false;
    bool isCrouching = false;
    float comboScaling = 1.0f;
};

} // namespace ArenaFighter