# DFR - Anime Arena Fighter Game Design Document

Based on Lost Saga mechanics with unique anime character roster

## Core Game Overview

### Game Type
- **Genre**: 3D Arena Fighter
- **Inspiration**: Lost Saga gameplay mechanics
- **Platform**: PC (Windows, DirectX 11)
- **Engine**: Custom C++20 with LSFDC patterns

### Unique Features
- Mana-based skill system (no cooldowns on special moves)
- Gear skills with cooldowns (weapon, helmet, armor, trinket)
- Stance switching system for select characters
- Character category system (7 unique categories)
- Weekly ranking system per character

## Character System

### Character Categories (7 Total)

1. **System**
   - Gao Peng
   - Su Ping

2. **Gods/Heroes**
   - Hyoudou Kotetsu
   - Seo Jun-ho - The Frost

3. **Murim**
   - Hyuk Woon Sung (Divine Heavenly Demon) - UNIQUE S-Tier
   - Chun Ah Young (Divine Maiden)

4. **Cultivation**
   - Tang San
   - Qian Renxue

5. **Animal**
   - Bai Xiuxiu
   - Tang Wulin (Golden Dragon King)

6. **Monsters**
   - Rou
   - Crimson Authority

7. **Chaos**
   - Gear Weaver

### Character Stats System

Instead of stat points, characters have **Mode Selection**:

1. **Attack Mode** - Increased melee damage
2. **Defense Mode** - Reduced damage taken
3. **Special Mode** - Faster skill regeneration
4. **Hybrid Mode** - Balanced attack and special
5. **Custom Mode** - Player-allocated stats

## Combat System

### Basic Controls
- **DDDD**: Basic attack combo chain
- **D Hold**: Charged power attack
- **Dash DD**: Rush attack combo
- **Jump D**: Aerial attack
- **Jump Hold D**: Diving attack

### Special Moves (Mana-Based)
- **S + ↑**: Upward special skill
- **S + →**: Forward special skill
- **S + ←**: Backward special skill
- **S + ↓**: Downward special skill
- **Down + S**: Stance switch (for applicable characters)
- **Hold S (1 second)**: Block (cannot use special moves while blocking)

### Gear Skills (Cooldown-Based)
- **AS**: Weapon skill
- **AD**: Helmet skill
- **ASD**: Armor skill
- **SD**: Trinket skill

### Resources
- **Mana**: 100 base, regenerates at 5/second
- **Qi/Ultimate Gauge**: Builds through combat, enables ultimate at 100

## Game Modes

### 1. Ranked 1v1
- **Week 1-2**: Single character slot
- **Week 3-4**: Three character slots
- Lost Saga Prisoner 1v1 mode style
- Weekly character rankings

### 2. For Glory (2 Options)
- **Option A**: 3v3 Team Battle
- **Option B**: 1v1 with 2 heroes (no switching, best of 2)

### 3. Death Match / Team Death Match
- 8 player free-for-all or team-based
- Random stat boost items spawn
- Random gear boxes spawn

### 4. Beast Mode (Giant Mode)
- 1 player becomes giant version of their character
- Other 7 players fight the giant
- Whoever defeats giant becomes next giant
- Winner is who deals most damage overall

### 5. Dimensional Rift Dungeons
- PvE mode fighting monsters and bosses
- Can bring up to 3 owned characters as AI companions
- Difficulty affects AI companion availability
- Boss progression: Mobs → Mini-boss → Main boss

### 6. Tournament Mode
- Player-created tournaments
- Custom prizes (gears/heroes)
- Friend or public tournaments

## Ranking System

### Weekly Rankings
- Win/loss record tracked per character
- Top performer gets "Weekly #1" badge
- Top 100 per character get recognition badges
- 4 weekly cycles = 1 monthly ranking

### Character-Specific Rankings
- Each character has individual leaderboard
- Encourages mastery of all characters
- Special rewards for character specialists

## Character Implementation: Hyuk Woon Sung

### Overview
- **Rarity**: Unique Mercenary (S-Tier)
- **Role**: Agile Dual-Path Master/Stance Specialist
- **Difficulty**: High (stance management required)

### Unique Mechanics

#### Stance System
- **Light Stance**: Orthodox Spear Arts (Blue energy)
- **Dark Stance**: Heavenly Demon Divine Arts (Red energy)
- Switch with Down+S, creates yin-yang shatter effect

#### Visual Design
- Spear changes color based on stance
- Light stance: Celestial blue effects, star motifs
- Dark stance: Dark red/black effects, demon motifs
- Ultimate form: Gold accents combining both energies

### Example Implementation Requirements
- Complex animation state machine for stance switching
- Particle effects for each skill
- Audio cues differentiated by stance
- Resource management (Mana + Qi)
- Combo system with stance-specific chains

## Technical Implementation Notes

### Following CLAUDE.md Standards
- Max 500 lines per file
- Namespace: ArenaFighter
- No cooldowns on special skills (mana only)
- 60Hz physics tick rate
- 30Hz network send rate
- Damage formula: `damage = baseDamage * (100.0f / (100.0f + defense))`

### Required Systems
1. Character state machine with stance support
2. Skill system with mana/cooldown hybrid
3. Ranking database and weekly reset
4. Tournament bracket generator
5. AI companion system for dungeons
6. Giant mode transformation system
7. Item spawn system for death match

## Next Implementation Steps

1. Create character base classes supporting stance system
2. Implement Hyuk Woon Sung as reference character
3. Set up game mode infrastructure
4. Create ranking and persistence system
5. Implement dungeon AI companions
6. Add tournament creation tools