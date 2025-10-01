#include "CharacterBase.h"
#include "Systems/StanceSystem.h"
#include "Systems/PetSystem.h"
#include "Systems/EvolutionSystem.h"

namespace ArenaFighter {

CharacterBase::CharacterBase(const std::string& name, CharacterCategory category)
    : m_name(name), m_category(category) {
    
    // Initialize default gear skills
    for (int gear = 0; gear < 4; gear++) {
        for (int slot = 0; slot < 2; slot++) {
            int index = gear * 2 + slot;
            m_gearSkills[index] = {
                "Skill " + std::to_string(index + 1),
                "default_animation",
                20.0f,  // manaCost
                100.0f, // baseDamage
                5.0f,   // range
                10,     // startupFrames
                3,      // activeFrames
                15      // recoveryFrames
            };
        }
    }
}

void CharacterBase::EnableStanceSystem() {
    if (!m_stanceSystem) {
        m_stanceSystem = std::make_unique<StanceSystem>();
    }
}

void CharacterBase::EnablePetSystem() {
    if (!m_petSystem) {
        m_petSystem = std::make_unique<PetSystem>();
    }
}

void CharacterBase::EnableEvolutionSystem() {
    if (!m_evolutionSystem) {
        m_evolutionSystem = std::make_unique<EvolutionSystem>();
    }
}

void CharacterBase::Update(float deltaTime) {
    // Update all active systems
    if (m_stanceSystem) {
        m_stanceSystem->Update(deltaTime);
    }
    
    if (m_petSystem) {
        // TODO: Pass owner position
        DirectX::XMFLOAT3 ownerPos = {0, 0, 0};
        m_petSystem->Update(deltaTime, ownerPos);
    }
    
    if (m_evolutionSystem) {
        m_evolutionSystem->Update(deltaTime);
    }
}

// Character Factory Implementations

std::unique_ptr<CharacterBase> CharacterFactory::CreateSeoJunho() {
    auto character = std::make_unique<CharacterBase>("Seo Jun-ho", CharacterCategory::Murim);
    
    // Enable stance system for martial arts
    character->EnableStanceSystem();
    auto stanceSystem = character->GetStanceSystem();
    
    // Add Frost-based stances
    stanceSystem->AddStance("Ice Wall", FrostStances::IceWall);
    stanceSystem->AddStance("Frost Blade", FrostStances::FrostBlade);
    stanceSystem->AddStance("Absolute Zero", FrostStances::AbsoluteZero);
    
    // Configure frost-themed gear skills
    auto& skills = character->GetGearSkills();
    
    // Gear 1: Basic Frost Skills
    skills[0] = {"Frost Strike", "frost_strike_anim", 15.0f, 120.0f, 6.0f, 8, 3, 12};
    skills[1] = {"Ice Shard", "ice_shard_anim", 20.0f, 100.0f, 10.0f, 10, 2, 15, true}; // projectile
    
    // Gear 2: Defensive Skills
    skills[2] = {"Frozen Armor", "frozen_armor_anim", 30.0f, 0.0f, 0.0f, 5, 120, 10};
    skills[3] = {"Ice Mirror", "ice_mirror_anim", 25.0f, 80.0f, 5.0f, 12, 3, 20, false, true}; // invincible
    
    // Gear 3: Area Control
    skills[4] = {"Blizzard", "blizzard_anim", 40.0f, 150.0f, 8.0f, 20, 60, 30};
    skills[5] = {"Frozen Domain", "frozen_domain_anim", 35.0f, 0.0f, 12.0f, 15, 180, 20};
    
    // Gear 4: Ultimate Skills
    skills[6] = {"Thousand Ice Petals", "ice_petals_anim", 50.0f, 200.0f, 10.0f, 25, 5, 35};
    skills[7] = {"Absolute Zero Field", "absolute_zero_anim", 60.0f, 250.0f, 15.0f, 30, 10, 40};
    
    return character;
}

std::unique_ptr<CharacterBase> CharacterFactory::CreateSuPing() {
    auto character = std::make_unique<CharacterBase>("Su Ping", CharacterCategory::Cultivation);
    
    // Enable pet system for pet store owner
    character->EnablePetSystem();
    auto petSystem = character->GetPetSystem();
    
    // Add starter pets
    petSystem->AddPet(PetStoreCreatures::CreatePurplePython());
    petSystem->AddPet(PetStoreCreatures::CreateLightningRat());
    
    // Configure pet-summoning gear skills
    auto& skills = character->GetGearSkills();
    
    // Gear 1: Pet Commands
    skills[0] = {"Pet Attack", "pet_attack_anim", 10.0f, 0.0f, 15.0f, 5, 2, 8};
    skills[1] = {"Pet Guard", "pet_guard_anim", 15.0f, 0.0f, 5.0f, 8, 120, 10};
    
    // Gear 2: Summoning
    skills[2] = {"Summon Beast", "summon_beast_anim", 30.0f, 0.0f, 0.0f, 20, 3, 25};
    skills[3] = {"Beast Whistle", "beast_whistle_anim", 20.0f, 0.0f, 20.0f, 10, 2, 15};
    
    // Gear 3: Pet Enhancement
    skills[4] = {"Wild Instinct", "wild_instinct_anim", 25.0f, 0.0f, 10.0f, 15, 180, 20};
    skills[5] = {"Pack Leader", "pack_leader_anim", 35.0f, 0.0f, 15.0f, 12, 240, 18};
    
    // Gear 4: Ultimate Pet Skills
    skills[6] = {"Legendary Summon", "legendary_summon_anim", 60.0f, 0.0f, 0.0f, 30, 5, 40};
    skills[7] = {"Beast King's Roar", "beast_roar_anim", 50.0f, 180.0f, 12.0f, 25, 4, 35};
    
    return character;
}

std::unique_ptr<CharacterBase> CharacterFactory::CreateRou() {
    auto character = std::make_unique<CharacterBase>("Rou", CharacterCategory::Monsters);
    
    // Enable evolution system for Re:Monster protagonist
    character->EnableEvolutionSystem();
    auto evolutionSystem = character->GetEvolutionSystem();
    
    // Set up evolution tree
    evolutionSystem->SetBaseForm("Goblin");
    evolutionSystem->AddEvolution(RouEvolutions::CreateGoblin());
    evolutionSystem->AddEvolution(RouEvolutions::CreateHobgoblin());
    
    // Configure devour-based gear skills
    auto& skills = character->GetGearSkills();
    
    // Gear 1: Basic Combat
    skills[0] = {"Devour", "devour_anim", 20.0f, 80.0f, 3.0f, 15, 5, 20};
    skills[1] = {"Intimidate", "intimidate_anim", 15.0f, 0.0f, 8.0f, 10, 3, 15};
    
    // Gear 2: Absorbed Skills (dynamic based on devoured enemies)
    skills[2] = {"Absorbed Skill 1", "absorbed1_anim", 25.0f, 100.0f, 6.0f, 12, 3, 18};
    skills[3] = {"Absorbed Skill 2", "absorbed2_anim", 25.0f, 100.0f, 6.0f, 12, 3, 18};
    
    // Gear 3: Evolution Powers
    skills[4] = {"Evolution Burst", "evo_burst_anim", 40.0f, 150.0f, 8.0f, 20, 4, 25};
    skills[5] = {"Adaptive Defense", "adaptive_def_anim", 30.0f, 0.0f, 0.0f, 10, 180, 15};
    
    // Gear 4: Ultimate Evolution
    skills[6] = {"Apex Predator", "apex_predator_anim", 60.0f, 200.0f, 10.0f, 25, 5, 30};
    skills[7] = {"Genetic Overload", "genetic_overload_anim", 70.0f, 250.0f, 12.0f, 30, 10, 40};
    
    return character;
}

std::unique_ptr<CharacterBase> CharacterFactory::CreateCharacter(const std::string& characterType) {
    // Character name mapping
    if (characterType == "SeoJunho" || characterType == "FrostMartialArtist") {
        return CreateSeoJunho();
    } else if (characterType == "SuPing" || characterType == "PetStoreOwner") {
        return CreateSuPing();
    } else if (characterType == "Rou" || characterType == "EvolutionMonster") {
        return CreateRou();
    }
    
    // Default: Create a basic character
    return std::make_unique<CharacterBase>(characterType, CharacterCategory::System);
}

} // namespace ArenaFighter