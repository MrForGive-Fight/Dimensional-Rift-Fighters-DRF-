#pragma once

#include "CharacterBase.h"
#include <stdexcept>
#include <string>

namespace ArenaFighter {

/**
 * @brief Validation helper to ensure skill system rules are followed
 * 
 * Enforces:
 * - Special moves (S+Direction) have NO cooldowns
 * - Gear skills (AS, AD, ASD, SD) HAVE cooldowns
 */
class SkillValidation {
public:
    /**
     * @brief Validate a special move has no cooldown
     * @throws std::invalid_argument if validation fails
     */
    static void ValidateSpecialMove(const SpecialMove& move) {
        // Special moves should not have cooldown field at all
        // This is enforced by the struct definition
        
        // Validate mana cost is in expected range
        if (move.manaCost < 10.0f || move.manaCost > 70.0f) {
            throw std::invalid_argument(
                "Special move '" + move.name + 
                "' has invalid mana cost. Expected 10-70, got " + 
                std::to_string(move.manaCost)
            );
        }
        
        // Validate frame data
        if (move.startupFrames < 3 || move.startupFrames > 60) {
            throw std::invalid_argument(
                "Special move '" + move.name + 
                "' has invalid startup frames. Expected 3-60, got " + 
                std::to_string(move.startupFrames)
            );
        }
    }
    
    /**
     * @brief Validate a gear skill has appropriate cooldown
     * @throws std::invalid_argument if validation fails
     */
    static void ValidateGearSkill(const GearSkill& skill) {
        // Gear skills MUST have cooldowns
        if (skill.cooldown <= 0.0f) {
            throw std::invalid_argument(
                "Gear skill '" + skill.name + 
                "' is missing cooldown! Gear skills must have cooldowns (5-30s typically)"
            );
        }
        
        // Validate cooldown is in expected range
        if (skill.cooldown < 3.0f || skill.cooldown > 60.0f) {
            throw std::invalid_argument(
                "Gear skill '" + skill.name + 
                "' has invalid cooldown. Expected 3-60s, got " + 
                std::to_string(skill.cooldown) + "s"
            );
        }
        
        // Validate mana cost
        if (skill.manaCost < 15.0f || skill.manaCost > 60.0f) {
            throw std::invalid_argument(
                "Gear skill '" + skill.name + 
                "' has invalid mana cost. Expected 15-60, got " + 
                std::to_string(skill.manaCost)
            );
        }
    }
    
    /**
     * @brief Validate all skills for a character
     */
    static void ValidateCharacterSkills(const CharacterBase* character) {
        if (!character) return;
        
        // Validate all special moves
        for (const auto& [direction, move] : character->GetAllSpecialMoves()) {
            try {
                ValidateSpecialMove(move);
            } catch (const std::exception& e) {
                throw std::invalid_argument(
                    "Character '" + character->GetName() + "': " + e.what()
                );
            }
        }
        
        // Validate all gear skills
        const auto& gearSkills = character->GetGearSkills();
        for (int i = 0; i < 8; ++i) {
            try {
                ValidateGearSkill(gearSkills[i]);
            } catch (const std::exception& e) {
                throw std::invalid_argument(
                    "Character '" + character->GetName() + 
                    "' Gear Skill " + std::to_string(i) + ": " + e.what()
                );
            }
        }
    }
    
    /**
     * @brief Check if a skill follows the correct pattern
     */
    static bool IsSpecialMoveValid(const SpecialMove& move) {
        try {
            ValidateSpecialMove(move);
            return true;
        } catch (...) {
            return false;
        }
    }
    
    static bool IsGearSkillValid(const GearSkill& skill) {
        try {
            ValidateGearSkill(skill);
            return true;
        } catch (...) {
            return false;
        }
    }
};

} // namespace ArenaFighter