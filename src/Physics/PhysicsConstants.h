#pragma once

namespace ArenaFighter {
namespace Physics {

// LSFDC Physics Constants
namespace Constants {
    // Gravity and Movement
    constexpr float GRAVITY = -1200.0f;              // Arcade-style gravity
    constexpr float MAX_FALL_SPEED = -800.0f;       // Terminal velocity
    constexpr float GROUND_FRICTION = 0.85f;        // Ground movement friction
    constexpr float AIR_FRICTION = 0.95f;           // Air movement friction
    constexpr float PUSHBACK_FRICTION = 0.9f;       // Combat pushback deceleration
    
    // Jump Physics
    constexpr float JUMP_FORCE = 450.0f;            // Standard jump force
    constexpr float SHORT_HOP_FORCE = 300.0f;       // Short hop force
    constexpr float SUPER_JUMP_FORCE = 600.0f;      // Super jump force
    constexpr float DOUBLE_JUMP_FORCE = 400.0f;     // Double jump force
    constexpr float WALL_JUMP_FORCE_X = 350.0f;     // Horizontal wall jump force
    constexpr float WALL_JUMP_FORCE_Y = 400.0f;     // Vertical wall jump force
    
    // Dash Physics
    constexpr float GROUND_DASH_SPEED = 500.0f;     // Ground dash speed
    constexpr float AIR_DASH_SPEED = 450.0f;        // Air dash speed
    constexpr float BACK_DASH_SPEED = 400.0f;       // Back dash speed
    constexpr int DASH_DURATION = 15;               // Dash duration in frames
    constexpr int AIR_DASH_COOLDOWN = 30;           // Frames before can air dash again
    
    // Stage Boundaries
    constexpr float STAGE_LEFT = -400.0f;           // Left boundary
    constexpr float STAGE_RIGHT = 400.0f;           // Right boundary
    constexpr float STAGE_GROUND = 0.0f;            // Ground level
    constexpr float STAGE_CEILING = 600.0f;         // Ceiling height
    constexpr float WALL_BOUNCE_FACTOR = 0.7f;      // Wall bounce dampening
    
    // Collision Detection
    constexpr float OVERLAP_TOLERANCE = 1.0f;       // LSFDC pixel overlap tolerance
    constexpr float PUSHBOX_SEPARATION = 2.0f;      // Minimum separation between pushboxes
    constexpr float CORNER_PUSH_THRESHOLD = 50.0f;  // Distance from wall for corner push
    
    // Hit Detection
    constexpr int HIT_FREEZE_FRAMES = 4;            // Frames of hit freeze on impact
    constexpr float TRADE_WINDOW = 3.0f;            // Frame window for trades
    constexpr float COUNTER_HIT_MULTIPLIER = 1.5f;  // Counter hit damage/hitstun multiplier
    
    // Knockback
    constexpr float MIN_KNOCKBACK = 50.0f;          // Minimum knockback force
    constexpr float MAX_KNOCKBACK = 500.0f;         // Maximum knockback force
    constexpr float GROUND_BOUNCE_THRESHOLD = 200.0f; // Y velocity for ground bounce
    constexpr float WALL_BOUNCE_THRESHOLD = 300.0f;   // X velocity for wall bounce
    constexpr float TECH_WINDOW = 10.0f;              // Frames to tech knockdown
    
    // Character Movement
    constexpr float WALK_SPEED = 150.0f;            // Base walk speed
    constexpr float RUN_SPEED = 250.0f;             // Base run speed
    constexpr float AIR_SPEED_MULTIPLIER = 0.3f;    // Air control multiplier
    constexpr float CROUCH_HEIGHT_MULTIPLIER = 0.6f; // Hurtbox reduction when crouching
}

// Collision Layers (bit flags)
namespace Layers {
    constexpr int NONE = 0;
    constexpr int PLAYER1 = 1 << 0;
    constexpr int PLAYER2 = 1 << 1;
    constexpr int PLAYER3 = 1 << 2;
    constexpr int PLAYER4 = 1 << 3;
    constexpr int PROJECTILE_P1 = 1 << 4;
    constexpr int PROJECTILE_P2 = 1 << 5;
    constexpr int PROJECTILE_P3 = 1 << 6;
    constexpr int PROJECTILE_P4 = 1 << 7;
    constexpr int ENVIRONMENT = 1 << 8;
    constexpr int TRIGGER = 1 << 9;
    constexpr int ITEM = 1 << 10;
    constexpr int PLATFORM = 1 << 11;
    
    // Layer masks
    constexpr int ALL_PLAYERS = PLAYER1 | PLAYER2 | PLAYER3 | PLAYER4;
    constexpr int ALL_PROJECTILES = PROJECTILE_P1 | PROJECTILE_P2 | PROJECTILE_P3 | PROJECTILE_P4;
    constexpr int SOLID_OBJECTS = ALL_PLAYERS | ENVIRONMENT | PLATFORM;
}

// Hit Priority Levels
namespace Priority {
    constexpr int LIGHT = 1;
    constexpr int MEDIUM = 2;
    constexpr int HEAVY = 3;
    constexpr int SPECIAL = 4;
    constexpr int SUPER = 5;
    constexpr int UNBLOCKABLE = 10;
}

// Spatial Grid Configuration
namespace SpatialGrid {
    constexpr float CELL_SIZE = 50.0f;              // Size of each grid cell
    constexpr float PADDING = 100.0f;               // Extra padding around stage
}

} // namespace Physics
} // namespace ArenaFighter