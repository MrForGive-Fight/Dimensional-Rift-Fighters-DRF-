# /generate-character

Generate a new character following DFR standards and LSFDC patterns.

## Usage
```
/generate-character <category> <name> [specialization]
```

## Parameters
- `category`: One of: System, GodsHeroes, Murim, Cultivation, Animal, Monsters, Chaos
- `name`: Character name (PascalCase)
- `specialization`: Optional - stance, pet, evolution, transformation, weaponmastery, cultivation, blessing

## Process

1. **Validate Input**
   - Check category exists in CharacterCategory enum
   - Ensure name follows naming conventions
   - Verify specialization is valid

2. **Create Character Files**
   - Generate header file in `src/Characters/<Category>/<Name>.h`
   - Generate implementation in `src/Characters/<Category>/<Name>.cpp`
   - Max 500 lines per file

3. **Implement Base Structure**
   ```cpp
   class <Name> : public CharacterBase {
   public:
       <Name>();
       virtual ~<Name>() = default;
       
       // Override required methods
       virtual void Update(float deltaTime) override;
       virtual void OnDamageDealt(float damage) override;
       virtual void OnDamageTaken(float damage) override;
   };
   ```

4. **Configure Gear Skills**
   - 4 gears × 2 skills = 8 total skills
   - Each skill has mana cost (no cooldowns)
   - Follow LSFDC combat formulas

5. **Add Specialized Systems**
   - Enable appropriate system based on specialization
   - Configure system-specific parameters

6. **Register Character**
   - Add to CharacterFactory
   - Update character roster

## Example
```
/generate-character Murim FrostMartialArtist stance
```

## Validation Rules
- All skills must have mana costs
- Base stats: 1000 HP, 100 Mana
- Follow LSFDC naming conventions
- Implement proper frame data for combat