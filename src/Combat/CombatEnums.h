#pragma once

namespace ArenaFighter {

// Constants from CLAUDE.md
constexpr float MANA_REGEN = 5.0f; // Per second
constexpr float BASE_HEALTH = 1000.0f;
constexpr float BASE_MANA = 100.0f;
constexpr float BASE_DEFENSE = 100.0f;
constexpr float BASE_SPEED = 100.0f;

enum class AttackType {
    Light,
    Medium,
    Heavy,
    Special,
    Ultimate
};

enum class DamageType {
    Physical,
    Magical,
    True
};

enum class HitState {
    Normal,
    Counter,
    Critical,
    Punish
};

} // namespace ArenaFighter