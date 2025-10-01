#pragma once

#include "Yuito.h"

namespace ArenaFighter {

// ============================================================================
// UNDEAD CONTRACTS (Weapon Slot - S+D)
// ============================================================================

// Tier 1: Bone Soldier
class BoneSoldier : public Pet {
public:
    BoneSoldier();
    void UpdateAI(float deltaTime) override;
    void Attack() override;
    void OnDeath() override;  // Explodes for 5 damage

private:
    float attackTimer = 0.0f;
};

// Tier 2: Little Skeleton
class LittleSkeleton : public Pet {
public:
    LittleSkeleton();
    void UpdateAI(float deltaTime) override;
    void Attack() override;
    void OnDeath() override;

private:
    float dodgeTimer = 0.0f;
    float boneThrowTimer = 0.0f;
    bool isProtectingYuito = false;
};

// Tier 3: Skeleton King
class SkeletonKing : public Pet {
public:
    SkeletonKing();
    void UpdateAI(float deltaTime) override;
    void Attack() override;
    void OnDeath() override;

    // Special abilities
    void SummonBoneSoldiers();
    void CreateBoneBarrier();
    void ActivateDeathAura();
    bool Resurrect();  // Resurrects once at 30% HP

private:
    float summonTimer = 0.0f;
    float barrierTimer = 0.0f;
    bool hasResurrected = false;
    bool deathAuraActive = true;
};

// ============================================================================
// DRAGON CONTRACTS (Helmet Slot - A+D)
// ============================================================================

// Tier 1: Fire Drake
class FireDrake : public Pet {
public:
    FireDrake();
    void UpdateAI(float deltaTime) override;
    void Attack() override;
    void OnDeath() override;

private:
    float fireballTimer = 0.0f;
    float maintainDistance = 100.0f;  // Stay away from enemies
};

// Tier 2: Inferno Dragon
class InfernoDragon : public Pet {
public:
    InfernoDragon();
    void UpdateAI(float deltaTime) override;
    void Attack() override;
    void OnDeath() override;

    void FireBreath();
    void DiveBomb();
    void CreateFireWall();

private:
    float breathTimer = 0.0f;
    float diveBombTimer = 0.0f;
    bool isAirborne = true;
};

// Tier 3: Chaos Dragon
class ChaosDragon : public Pet {
public:
    ChaosDragon();
    void UpdateAI(float deltaTime) override;
    void Attack() override;
    void OnDeath() override;

    void RandomElementAttack();
    void Teleport();
    void CreateDimensionalRift();
    void OpenEscapePortal();

private:
    float teleportTimer = 0.0f;
    float riftTimer = 0.0f;
    int currentElement = 0;  // Cycles through elements
};

// ============================================================================
// BEAST CONTRACTS (Armor Slot - A+S)
// ============================================================================

// Tier 1: Spirit Wolf
class SpiritWolf : public Pet {
public:
    SpiritWolf();
    void UpdateAI(float deltaTime) override;
    void Attack() override;
    void OnDeath() override;

    bool TryDodge();  // 25% chance

private:
    float hitAndRunTimer = 0.0f;
    int packCount = 1;  // Gets stronger with more wolves
};

// Tier 2: Thunder Tiger
class ThunderTiger : public Pet {
public:
    ThunderTiger();
    void UpdateAI(float deltaTime) override;
    void Attack() override;
    void OnDeath() override;

    void FearRoar();  // 8s cooldown
    void LightningPounce();
    bool StunOnThirdHit();

private:
    float roarTimer = 0.0f;
    int hitCount = 0;
    float pounceTimer = 0.0f;
};

// Tier 3: Void Beast
class VoidBeast : public Pet {
public:
    VoidBeast();
    void UpdateAI(float deltaTime) override;
    void Attack() override;
    void OnDeath() override;

    void CreateVoidZone();
    void PhaseYuitoThroughDanger();
    void TeleportThreat();

private:
    float voidZoneTimer = 0.0f;
    float phaseTimer = 0.0f;
    bool isGuardingYuito = true;
};

// ============================================================================
// MYTHIC CONTRACTS (Trinket Slot - A+S+D)
// ============================================================================

// Tier 1: Guardian Golem
class GuardianGolem : public Pet {
public:
    GuardianGolem();
    void UpdateAI(float deltaTime) override;
    void Attack() override;
    void OnDeath() override;

    void TauntEnemies();
    void PositionBetweenYuitoAndDanger();

private:
    float tauntTimer = 0.0f;
    bool alwaysProtecting = true;
};

// Tier 2: Phoenix
class Phoenix : public Pet {
public:
    Phoenix();
    void UpdateAI(float deltaTime) override;
    void Attack() override;
    void OnDeath() override;  // Resurrects once

    void HealYuito();
    void CreateHealingZone();
    bool Resurrect();

private:
    float healTimer = 0.0f;
    float zoneTimer = 0.0f;
    bool hasResurrected = false;
};

// Tier 3: Chaos Titan
class ChaosTitan : public Pet {
public:
    ChaosTitan();
    void UpdateAI(float deltaTime) override;
    void Attack() override;
    void OnDeath() override;

    void EarthquakeAttack();
    void GrabAndThrow();
    void BecomeMoreAggressive();  // When low HP

private:
    float earthquakeTimer = 0.0f;
    float grabTimer = 0.0f;
    bool isEnraged = false;
};

} // namespace ArenaFighter
