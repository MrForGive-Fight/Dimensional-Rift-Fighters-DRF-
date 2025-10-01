#pragma once

namespace ArenaFighter {

// Forward declaration
class Rou;

// Base state for all evolution forms
class FormState {
public:
    virtual ~FormState() = default;
    
    virtual void Enter(Rou* rou) = 0;
    virtual void Exit(Rou* rou) = 0;
    virtual void Update(Rou* rou, float deltaTime) = 0;
    
    virtual float GetDamageMultiplier() const = 0;
    virtual float GetSpeedMultiplier() const = 0;
    virtual float GetDefenseMultiplier() const = 0;
    virtual int GetMaxComboHits() const = 0;
    virtual float GetSizeScale() const = 0;
};

// Goblin Form State (0-24% gauge)
class GoblinFormState : public FormState {
public:
    void Enter(Rou* rou) override;
    void Exit(Rou* rou) override;
    void Update(Rou* rou, float deltaTime) override;
    
    float GetDamageMultiplier() const override { return 0.85f; }
    float GetSpeedMultiplier() const override { return 1.3f; }
    float GetDefenseMultiplier() const override { return 1.15f; }
    int GetMaxComboHits() const override { return 3; }
    float GetSizeScale() const override { return 0.8f; }
};

// Hobgoblin Form State (25-49% gauge)
class HobgoblinFormState : public FormState {
public:
    void Enter(Rou* rou) override;
    void Exit(Rou* rou) override;
    void Update(Rou* rou, float deltaTime) override;
    
    float GetDamageMultiplier() const override { return 1.0f; }
    float GetSpeedMultiplier() const override { return 1.1f; }
    float GetDefenseMultiplier() const override { return 1.0f; }
    int GetMaxComboHits() const override { return 4; }
    float GetSizeScale() const override { return 1.0f; }
};

// Ogre Form State (50-74% gauge)
class OgreFormState : public FormState {
public:
    void Enter(Rou* rou) override;
    void Exit(Rou* rou) override;
    void Update(Rou* rou, float deltaTime) override;
    
    float GetDamageMultiplier() const override { return 1.25f; }
    float GetSpeedMultiplier() const override { return 0.9f; }
    float GetDefenseMultiplier() const override { return 0.85f; }
    int GetMaxComboHits() const override { return 5; }
    float GetSizeScale() const override { return 2.5f; }
};

// Apostle Lord Form State (75-99% gauge)
class ApostleLordFormState : public FormState {
public:
    void Enter(Rou* rou) override;
    void Exit(Rou* rou) override;
    void Update(Rou* rou, float deltaTime) override;
    
    float GetDamageMultiplier() const override { return 1.4f; }
    float GetSpeedMultiplier() const override { return 1.0f; }
    float GetDefenseMultiplier() const override { return 0.7f; }
    int GetMaxComboHits() const override { return 6; }
    float GetSizeScale() const override { return 2.0f; }
};

// Vajrayaksa Form State (100% gauge)
class VajrayaksaFormState : public FormState {
public:
    void Enter(Rou* rou) override;
    void Exit(Rou* rou) override;
    void Update(Rou* rou, float deltaTime) override;
    
    float GetDamageMultiplier() const override { return 1.6f; }
    float GetSpeedMultiplier() const override { return 1.1f; }
    float GetDefenseMultiplier() const override { return 0.5f; }
    int GetMaxComboHits() const override { return 8; }
    float GetSizeScale() const override { return 2.5f; }
    
private:
    float m_auraIntensity = 0.0f;
    bool m_isFullPower = false;
};

} // namespace ArenaFighter