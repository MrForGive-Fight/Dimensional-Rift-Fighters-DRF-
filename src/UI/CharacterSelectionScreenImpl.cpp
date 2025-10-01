#include "CharacterSelectionScreen.h"

namespace ArenaFighter {

// Continuation of CharacterSelectionScreen.cpp implementation

std::shared_ptr<UIPanel> CharacterSelectionScreen::createCharacterCard(int characterIndex, XMFLOAT2 pos, XMFLOAT2 size) {
    CharacterData& character = m_characterRoster[characterIndex];
    
    auto card = std::make_shared<UIPanel>("Card_" + character.m_id, pos, size);
    
    bool isSelected = isCharacterSelected(characterIndex);
    
    if (isSelected) {
        card->setBackgroundColor(XMFLOAT4(0.25f, 0.4f, 0.35f, 0.9f));
    } else {
        card->setBackgroundColor(XMFLOAT4(0.15f, 0.2f, 0.3f, 0.9f));
    }
    
    auto name = std::make_shared<UILabel>(
        "Name_" + character.m_id,
        XMFLOAT2(10, 10),
        character.m_name,
        18.0f
    );
    name->setTextColor(XMFLOAT4(1.0f, 0.95f, 0.7f, 1.0f));
    card->addChild(name);
    
    auto categoryLabel = std::make_shared<UILabel>(
        "Cat_" + character.m_id,
        XMFLOAT2(10, 35),
        character.m_category,
        12.0f
    );
    categoryLabel->setTextColor(XMFLOAT4(0.7f, 0.7f, 0.9f, 1.0f));
    card->addChild(categoryLabel);
    
    auto statsLabel = std::make_shared<UILabel>(
        "Stats_" + character.m_id,
        XMFLOAT2(10, 90),
        "ATK:" + std::to_string(character.m_attack) + " DEF:" + std::to_string(character.m_defense) + 
        " SPC:" + std::to_string(character.m_special),
        14.0f
    );
    card->addChild(statsLabel);
    
    auto selectBtn = std::make_shared<UIButton>(
        "Select_" + character.m_id,
        XMFLOAT2(10, 110),
        XMFLOAT2(size.x - 20, 35),
        isSelected ? "SELECTED" : "SELECT"
    );
    
    if (isSelected) {
        selectBtn->setColors(
            XMFLOAT4(0.3f, 0.6f, 0.3f, 1.0f),
            XMFLOAT4(0.4f, 0.7f, 0.4f, 1.0f),
            XMFLOAT4(0.25f, 0.55f, 0.25f, 1.0f)
        );
    } else {
        selectBtn->setColors(
            XMFLOAT4(0.3f, 0.5f, 0.8f, 1.0f),
            XMFLOAT4(0.4f, 0.6f, 0.9f, 1.0f),
            XMFLOAT4(0.25f, 0.45f, 0.75f, 1.0f)
        );
    }
    
    selectBtn->setOnClick([this, characterIndex]() {
        selectCharacter(characterIndex);
    });
    
    card->addChild(selectBtn);
    
    return card;
}

void CharacterSelectionScreen::selectCharacter(int characterIndex) {
    if (characterIndex < 0 || characterIndex >= static_cast<int>(m_characterRoster.size())) {
        return;
    }
    
    for (size_t i = 0; i < m_selectedCharacterIndices.size(); i++) {
        if (m_selectedCharacterIndices[i] == characterIndex) {
            return;
        }
    }
    
    for (size_t i = 0; i < m_selectedCharacterIndices.size(); i++) {
        if (m_selectedCharacterIndices[i] == -1) {
            m_selectedCharacterIndices[i] = characterIndex;
            updateSlotDisplay(static_cast<int>(i));
            refreshCharacterList();
            return;
        }
    }
    
    if (!m_selectedCharacterIndices.empty()) {
        m_selectedCharacterIndices[m_selectedCharacterIndices.size() - 1] = characterIndex;
        updateSlotDisplay(static_cast<int>(m_selectedCharacterIndices.size() - 1));
        refreshCharacterList();
    }
}

void CharacterSelectionScreen::clearSlot(int slotIndex) {
    if (slotIndex >= 0 && slotIndex < static_cast<int>(m_selectedCharacterIndices.size())) {
        m_selectedCharacterIndices[slotIndex] = -1;
        updateSlotDisplay(slotIndex);
        refreshCharacterList();
    }
}

void CharacterSelectionScreen::updateSlotDisplay(int slotIndex) {
    if (slotIndex < 0 || slotIndex >= static_cast<int>(m_slotPanels.size())) return;
    
    auto slot = m_slotPanels[slotIndex];
    int charIndex = m_selectedCharacterIndices[slotIndex];
    
    auto emptyText = findChildByName<UILabel>(slot, "EmptyText" + std::to_string(slotIndex));
    auto clearBtn = findChildByName<UIButton>(slot, "ClearBtn" + std::to_string(slotIndex));
    auto charNameLabel = findChildByName<UILabel>(slot, "CharName" + std::to_string(slotIndex));
    auto charStatsLabel = findChildByName<UILabel>(slot, "CharStats" + std::to_string(slotIndex));
    auto charCategoryLabel = findChildByName<UILabel>(slot, "CharCategory" + std::to_string(slotIndex));
    
    if (charIndex >= 0 && charIndex < static_cast<int>(m_characterRoster.size())) {
        CharacterData& character = m_characterRoster[charIndex];
        
        if (emptyText) emptyText->setVisible(false);
        if (clearBtn) clearBtn->setEnabled(true);
        
        if (!charNameLabel) {
            charNameLabel = std::make_shared<UILabel>(
                "CharName" + std::to_string(slotIndex),
                XMFLOAT2(10, 40),
                character.m_name,
                20.0f
            );
            charNameLabel->setTextColor(XMFLOAT4(1.0f, 0.95f, 0.7f, 1.0f));
            slot->addChild(charNameLabel);
        } else {
            charNameLabel->setText(character.m_name);
            charNameLabel->setVisible(true);
        }
        
        if (!charCategoryLabel) {
            charCategoryLabel = std::make_shared<UILabel>(
                "CharCategory" + std::to_string(slotIndex),
                XMFLOAT2(10, 65),
                character.m_category,
                14.0f
            );
            charCategoryLabel->setTextColor(XMFLOAT4(0.7f, 0.7f, 0.9f, 1.0f));
            slot->addChild(charCategoryLabel);
        } else {
            charCategoryLabel->setText(character.m_category);
            charCategoryLabel->setVisible(true);
        }
        
        std::string statsText = "ATK:" + std::to_string(character.m_attack) + 
                               " DEF:" + std::to_string(character.m_defense) + 
                               " SPC:" + std::to_string(character.m_special);
        
        if (!charStatsLabel) {
            charStatsLabel = std::make_shared<UILabel>(
                "CharStats" + std::to_string(slotIndex),
                XMFLOAT2(10, 90),
                statsText,
                14.0f
            );
            charStatsLabel->setTextColor(XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f));
            slot->addChild(charStatsLabel);
        } else {
            charStatsLabel->setText(statsText);
            charStatsLabel->setVisible(true);
        }
        
        slot->setBackgroundColor(XMFLOAT4(0.2f, 0.3f, 0.35f, 0.9f));
        
    } else {
        if (emptyText) emptyText->setVisible(true);
        if (clearBtn) clearBtn->setEnabled(false);
        
        if (charNameLabel) charNameLabel->setVisible(false);
        if (charCategoryLabel) charCategoryLabel->setVisible(false);
        if (charStatsLabel) charStatsLabel->setVisible(false);
        
        slot->setBackgroundColor(XMFLOAT4(0.15f, 0.2f, 0.25f, 0.8f));
    }
}

template<typename T>
std::shared_ptr<T> CharacterSelectionScreen::findChildByName(std::shared_ptr<UIPanel> parent, const std::string& name) {
    if (!parent) return nullptr;
    
    // This would need UIPanel to have a getChildren method
    // For now, returning nullptr as a placeholder
    return nullptr;
}

void CharacterSelectionScreen::filterByCategory(const std::string& category) {
    m_currentCategory = category;
    refreshCharacterList();
}

void CharacterSelectionScreen::refreshCharacterList() {
    m_characterListPanel->clearChildren();
    populateCharacterList(m_characterListPanel);
}

void CharacterSelectionScreen::refreshSlotPanels() {
    for (size_t i = 0; i < m_slotPanels.size(); i++) {
        updateSlotDisplay(static_cast<int>(i));
    }
}

bool CharacterSelectionScreen::isCharacterSelected(int characterIndex) const {
    for (int selectedIndex : m_selectedCharacterIndices) {
        if (selectedIndex == characterIndex) {
            return true;
        }
    }
    return false;
}

bool CharacterSelectionScreen::canConfirm() const {
    for (int index : m_selectedCharacterIndices) {
        if (index >= 0) {
            return true;
        }
    }
    return false;
}

} // namespace ArenaFighter