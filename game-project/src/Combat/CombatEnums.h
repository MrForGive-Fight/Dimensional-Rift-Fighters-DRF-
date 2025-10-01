#pragma once

namespace ArenaFighter {

enum class CharacterCategory {
    System,
    GodsHeroes,
    Murim,
    Cultivation,
    Animal,
    Monsters,
    Chaos
};

enum class DamageType {
    Physical,
    Magical,
    True,       // Ignores defense
    Elemental,
    Mixed
};

enum class ElementType {
    None,
    Fire,
    Ice,
    Lightning,
    Earth,
    Wind,
    Water,
    Light,
    Dark,
    Void
};

enum class AttackProperty {
    None = 0,
    Projectile = 1 << 0,
    Piercing = 1 << 1,
    Area = 1 << 2,
    Grab = 1 << 3,
    Unblockable = 1 << 4,
    Counter = 1 << 5,
    Lifesteal = 1 << 6,
    Guard_Break = 1 << 7
};

enum class CharacterState {
    Idle,
    Moving,
    Jumping,
    Attacking,
    Defending,
    Hit_Stun,
    Block_Stun,
    Knocked_Down,
    Getting_Up,
    Dead
};

enum class InputCommand {
    None = 0,
    Up = 1 << 0,
    Down = 1 << 1,
    Left = 1 << 2,
    Right = 1 << 3,
    Light_Attack = 1 << 4,
    Medium_Attack = 1 << 5,
    Heavy_Attack = 1 << 6,
    Special = 1 << 7,
    Guard = 1 << 8,
    Dash = 1 << 9,
    Jump = 1 << 10,
    Gear_Switch = 1 << 11,
    Character_Switch = 1 << 12
};

} // namespace ArenaFighter