#include "CharacterSelectionScreen.h"
#include <algorithm>

namespace ArenaFighter {

CharacterSelectionScreen::CharacterSelectionScreen(ID3D11Device* dev, ID3D11DeviceContext* ctx, int slots)
    : m_device(dev), m_context(ctx), m_maxSlots(slots), m_currentCategory("All") {
    m_selectedCharacterIndices.resize(m_maxSlots, -1);
    initializeCharacterRoster();
    buildUI();
}

void CharacterSelectionScreen::setCallbacks(
    std::function<void()> confirmCallback,
    std::function<void()> cancelCallback
) {
    m_onConfirmSelection = confirmCallback;
    m_onCancel = cancelCallback;
}

std::vector<CharacterData> CharacterSelectionScreen::getSelectedCharacters() const {
    std::vector<CharacterData> result;
    for (int index : m_selectedCharacterIndices) {
        if (index >= 0 && index < static_cast<int>(m_characterRoster.size())) {
            result.push_back(m_characterRoster[index]);
        }
    }
    return result;
}

void CharacterSelectionScreen::setMaxSlots(int slots) {
    m_maxSlots = slots;
    m_selectedCharacterIndices.clear();
    m_selectedCharacterIndices.resize(m_maxSlots, -1);
    refreshSlotPanels();
}

void CharacterSelectionScreen::initializeCharacterRoster() {
    m_characterRoster.reserve(50);
    
    // System category
    m_characterRoster.push_back(createCharacter("GaoPeng", "Gao Peng", "System", 1, 85, 60, 75));
    m_characterRoster.push_back(createCharacter("SuPing", "Su Ping", "System", 1, 80, 65, 80));
    
    // GodsHeroes category
    m_characterRoster.push_back(createCharacter("HyoudouKotetsu", "Hyoudou Kotetsu", "GodsHeroes", 1, 90, 70, 85));
    m_characterRoster.push_back(createCharacter("SeoJunho", "Seo Jun-ho - The Frost", "GodsHeroes", 1, 75, 80, 90));
    
    // Murim category
    m_characterRoster.push_back(createCharacter("HyukWoonSung", "Hyuk Woon Sung", "Murim", 1, 88, 72, 92));
    m_characterRoster.push_back(createCharacter("ChunAhYoung", "Chun Ah Young", "Murim", 1, 82, 78, 88));
    
    // Cultivation category
    m_characterRoster.push_back(createCharacter("TangSan", "Tang San", "Cultivation", 1, 85, 75, 90));
    m_characterRoster.push_back(createCharacter("QianRenxue", "Qian Renxue", "Cultivation", 1, 83, 80, 87));
    
    // Animal category
    m_characterRoster.push_back(createCharacter("BaiXiuxiu", "Bai Xiuxiu", "Animal", 1, 80, 70, 85));
    m_characterRoster.push_back(createCharacter("TangWulin", "Tang Wulin", "Animal", 1, 92, 88, 78));
    
    // Monsters category
    m_characterRoster.push_back(createCharacter("Rou", "Rou", "Monsters", 1, 95, 85, 75));
    m_characterRoster.push_back(createCharacter("CrimsonAuthority", "Crimson Authority", "Monsters", 1, 90, 90, 80));
    
    // Chaos category
    m_characterRoster.push_back(createCharacter("GearWeaver", "Gear Weaver", "Chaos", 1, 85, 75, 95));
    
    // Initialize gears for all characters
    for (auto& character : m_characterRoster) {
        character.m_owned = true;
        character.initializeDefaultGears();
    }
}

CharacterData CharacterSelectionScreen::createCharacter(const std::string& id, const std::string& name, 
                              const std::string& category, int level,
                              int atk, int def, int spc) {
    CharacterData character;
    character.m_id = id;
    character.m_name = name;
    character.m_category = category;
    character.m_level = level;
    character.m_attack = atk;
    character.m_defense = BASE_DEFENSE + (def - 60);
    character.m_special = spc;
    character.m_speed = BASE_SPEED;
    character.m_owned = false;
    return character;
}

void CharacterSelectionScreen::buildUI() {
    m_rootPanel = std::make_shared<UIPanel>("CharSelect", XMFLOAT2(0, 0), XMFLOAT2(1920, 1080));
    m_rootPanel->setBackgroundColor(XMFLOAT4(0.05f, 0.05f, 0.1f, 1.0f));
    m_rootPanel->setBorder(false);
    
    auto title = std::make_shared<UILabel>("Title", XMFLOAT2(50, 30), "CHARACTER SELECTION", 42.0f);
    title->setTextColor(XMFLOAT4(0.9f, 0.7f, 0.2f, 1.0f));
    m_rootPanel->addChild(title);
    
    auto modeInfo = std::make_shared<UILabel>("ModeInfo", XMFLOAT2(1500, 40), 
        "Max Slots: " + std::to_string(m_maxSlots), 20.0f);
    modeInfo->setTextColor(XMFLOAT4(0.7f, 0.9f, 1.0f, 1.0f));
    m_rootPanel->addChild(modeInfo);
    
    auto slotsPanel = createCharacterSlotsPanel();
    m_rootPanel->addChild(slotsPanel);
    
    auto categoryPanel = createCategoryPanel();
    m_rootPanel->addChild(categoryPanel);
    
    m_characterListPanel = createCharacterListPanel();
    m_rootPanel->addChild(m_characterListPanel);
    
    auto confirmBtn = std::make_shared<UIButton>("ConfirmBtn", XMFLOAT2(1600, 980), XMFLOAT2(250, 60), "CONFIRM");
    confirmBtn->setColors(
        XMFLOAT4(0.2f, 0.7f, 0.2f, 1.0f),
        XMFLOAT4(0.3f, 0.8f, 0.3f, 1.0f),
        XMFLOAT4(0.15f, 0.6f, 0.15f, 1.0f)
    );
    confirmBtn->setOnClick([this]() {
        if (canConfirm() && m_onConfirmSelection) {
            m_onConfirmSelection();
        }
    });
    m_rootPanel->addChild(confirmBtn);
    
    auto cancelBtn = std::make_shared<UIButton>("CancelBtn", XMFLOAT2(1320, 980), XMFLOAT2(250, 60), "CANCEL");
    cancelBtn->setColors(
        XMFLOAT4(0.7f, 0.2f, 0.2f, 1.0f),
        XMFLOAT4(0.8f, 0.3f, 0.3f, 1.0f),
        XMFLOAT4(0.6f, 0.15f, 0.15f, 1.0f)
    );
    cancelBtn->setOnClick([this]() {
        if (m_onCancel) m_onCancel();
    });
    m_rootPanel->addChild(cancelBtn);
}

std::shared_ptr<UIPanel> CharacterSelectionScreen::createCharacterSlotsPanel() {
    auto panel = std::make_shared<UIPanel>("SlotsPanel", XMFLOAT2(50, 120), XMFLOAT2(1820, 200));
    panel->setBackgroundColor(XMFLOAT4(0.08f, 0.12f, 0.18f, 0.9f));
    
    auto title = std::make_shared<UILabel>("SlotsTitle", XMFLOAT2(20, 10), "Selected Characters", 24.0f);
    title->setTextColor(XMFLOAT4(1.0f, 0.9f, 0.3f, 1.0f));
    panel->addChild(title);
    
    float slotWidth = 560;
    float slotSpacing = 30;
    float startX = 20;
    
    m_slotPanels.clear();
    for (int i = 0; i < m_maxSlots; i++) {
        auto slot = std::make_shared<UIPanel>(
            "Slot" + std::to_string(i),
            XMFLOAT2(startX + i * (slotWidth + slotSpacing), 50),
            XMFLOAT2(slotWidth, 130)
        );
        slot->setBackgroundColor(XMFLOAT4(0.15f, 0.2f, 0.25f, 0.8f));
        
        auto slotNum = std::make_shared<UILabel>(
            "SlotNum" + std::to_string(i),
            XMFLOAT2(10, 10),
            "Slot " + std::to_string(i + 1),
            18.0f
        );
        slot->addChild(slotNum);
        
        auto emptyText = std::make_shared<UILabel>(
            "EmptyText" + std::to_string(i),
            XMFLOAT2(200, 50),
            "< Empty >",
            20.0f
        );
        emptyText->setTextColor(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
        slot->addChild(emptyText);
        
        auto clearBtn = std::make_shared<UIButton>(
            "ClearBtn" + std::to_string(i),
            XMFLOAT2(slotWidth - 80, 45),
            XMFLOAT2(70, 40),
            "Clear"
        );
        clearBtn->setColors(
            XMFLOAT4(0.5f, 0.2f, 0.2f, 1.0f),
            XMFLOAT4(0.6f, 0.3f, 0.3f, 1.0f),
            XMFLOAT4(0.4f, 0.15f, 0.15f, 1.0f)
        );
        clearBtn->setEnabled(false);
        int slotIndex = i;
        clearBtn->setOnClick([this, slotIndex]() {
            clearSlot(slotIndex);
        });
        slot->addChild(clearBtn);
        
        panel->addChild(slot);
        m_slotPanels.push_back(slot);
    }
    
    return panel;
}

std::shared_ptr<UIPanel> CharacterSelectionScreen::createCategoryPanel() {
    auto panel = std::make_shared<UIPanel>("CategoryPanel", XMFLOAT2(50, 340), XMFLOAT2(1820, 80));
    panel->setBackgroundColor(XMFLOAT4(0.08f, 0.12f, 0.18f, 0.9f));
    
    auto title = std::make_shared<UILabel>("CatTitle", XMFLOAT2(20, 10), "Filter by Category:", 20.0f);
    panel->addChild(title);
    
    std::vector<std::string> categories = {
        "All", "System", "GodsHeroes", "Murim", "Cultivation", "Animal", "Monsters", "Chaos"
    };
    
    float btnX = 220;
    for (const auto& category : categories) {
        auto btn = std::make_shared<UIButton>(
            "Cat_" + category,
            XMFLOAT2(btnX, 15),
            XMFLOAT2(160, 50),
            category == "GodsHeroes" ? "Gods/Heroes" : category
        );
        
        btn->setColors(
            XMFLOAT4(0.2f, 0.3f, 0.45f, 1.0f),
            XMFLOAT4(0.3f, 0.4f, 0.55f, 1.0f),
            XMFLOAT4(0.15f, 0.25f, 0.4f, 1.0f)
        );
        
        std::string cat = category;
        btn->setOnClick([this, cat]() {
            filterByCategory(cat);
        });
        
        panel->addChild(btn);
        btnX += 170;
    }
    
    return panel;
}

std::shared_ptr<UIPanel> CharacterSelectionScreen::createCharacterListPanel() {
    auto panel = std::make_shared<UIPanel>("CharListPanel", XMFLOAT2(50, 440), XMFLOAT2(1820, 520));
    panel->setBackgroundColor(XMFLOAT4(0.08f, 0.12f, 0.18f, 0.9f));
    
    populateCharacterList(panel);
    
    return panel;
}

void CharacterSelectionScreen::populateCharacterList(std::shared_ptr<UIPanel> panel) {
    panel->clearChildren();
    
    int columns = 6;
    float cardWidth = 280;
    float cardHeight = 150;
    float spacing = 15;
    float startX = 20;
    float startY = 20;
    
    int row = 0;
    int col = 0;
    
    for (size_t i = 0; i < m_characterRoster.size(); i++) {
        CharacterData& character = m_characterRoster[i];
        
        if (m_currentCategory != "All" && character.m_category != m_currentCategory) {
            continue;
        }
        
        if (!character.m_owned) {
            continue;
        }
        
        float x = startX + col * (cardWidth + spacing);
        float y = startY + row * (cardHeight + spacing);
        
        auto card = createCharacterCard(static_cast<int>(i), XMFLOAT2(x, y), XMFLOAT2(cardWidth, cardHeight));
        panel->addChild(card);
        
        col++;
        if (col >= columns) {
            col = 0;
            row++;
        }
    }
    
    // Add gear selection panel after character selection
    auto gearPanel = createGearSelectionPanel();
    m_rootPanel->addChild(gearPanel);
}

std::shared_ptr<UIPanel> CharacterSelectionScreen::createGearSelectionPanel() {
    auto panel = std::make_shared<UIPanel>("GearSelectionPanel", XMFLOAT2(50, 780), XMFLOAT2(1820, 180));
    panel->setBackgroundColor(XMFLOAT4(0.08f, 0.12f, 0.18f, 0.9f));
    
    auto title = std::make_shared<UILabel>("GearTitle", XMFLOAT2(20, 10), "Gear Loadout Selection", 24.0f);
    title->setTextColor(XMFLOAT4(1.0f, 0.9f, 0.3f, 1.0f));
    panel->addChild(title);
    
    auto note = std::make_shared<UILabel>("GearNote", XMFLOAT2(20, 40), 
        "Each character has 4 gears with 2 skills each (8 total skills). Default loadout will be used.", 16.0f);
    note->setTextColor(XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f));
    panel->addChild(note);
    
    // Display selected character's gears
    auto gearsInfo = std::make_shared<UILabel>("GearsInfo", XMFLOAT2(20, 70), 
        "Select a character to view their gear loadout", 18.0f);
    gearsInfo->setTextColor(XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f));
    panel->addChild(gearsInfo);
    
    return panel;
}