#include "EquipmentScreen.h"

namespace ArenaFighter {

EquipmentScreen::EquipmentScreen(ID3D11Device* dev, ID3D11DeviceContext* ctx)
    : m_device(dev), 
      m_context(ctx), 
      m_selectedItem(nullptr),
      m_selectedSlot(EquipmentSlot::Weapon),
      m_currentCategory("All") {
    initializeInventory();
    buildUI();
}

void EquipmentScreen::setCallbacks(
    std::function<void()> equipChangedCallback,
    std::function<void()> closeCallback
) {
    m_onEquipmentChanged = equipChangedCallback;
    m_onClose = closeCallback;
}

void EquipmentScreen::initializeInventory() {
    m_inventory.reserve(50);
    
    // Weapons
    auto spear1 = createSampleItem(
        "spear_divine", "Divine Heavenly Spear", "Spear of the Heavenly Demon",
        EquipmentSlot::Weapon, ItemRarity::Legendary, 50, 0, 10
    );
    spear1->m_skill1 = {"glassy_rain", "Glassy Death Rain", "Rain of light spears", 25.0f, 200.0f, 0.0f, 300.0f, 18, 5, 20};
    spear1->m_skill2 = {"spear_thrust", "Divine Thrust", "Piercing thrust attack", 15.0f, 150.0f, 0.0f, 200.0f, 10, 3, 15};
    m_inventory.push_back(spear1);
    
    auto spear2 = createSampleItem(
        "spear_frost", "Frost Spear", "Imbued with ice essence",
        EquipmentSlot::Weapon, ItemRarity::Epic, 35, 0, 5
    );
    spear2->m_skill1 = {"ice_pierce", "Ice Pierce", "Freezing spear strike", 20.0f, 120.0f, 0.0f, 150.0f, 12, 4, 16};
    spear2->m_skill2 = {"frost_wave", "Frost Wave", "AoE ice attack", 30.0f, 100.0f, 0.0f, 250.0f, 15, 6, 18};
    m_inventory.push_back(spear2);
    
    // Helmets
    auto helm1 = createSampleItem(
        "helm_demon", "Crown of the Demon", "Increases spiritual energy",
        EquipmentSlot::Helmet, ItemRarity::Epic, 0, 25, 0
    );
    helm1->m_manaBonus = 20;
    helm1->m_skill1 = {"spear_aura", "Spear Aura", "Extends attack range", 0.0f, 0.0f, 5.0f, 0.0f, 0, 0, 0}; // Buff skill
    helm1->m_skill2 = {"focus_mind", "Focus Mind", "Increases mana regen", 10.0f, 0.0f, 10.0f, 0.0f, 0, 0, 0};
    m_inventory.push_back(helm1);
    
    // Armor
    auto armor1 = createSampleItem(
        "armor_cult", "Heavenly Demon Robes", "Robes of the cult leader",
        EquipmentSlot::Armor, ItemRarity::Legendary, 20, 60, 0
    );
    armor1->m_healthBonus = 200;
    armor1->m_skill1 = {"dragon_flow", "Flow of Divine Dragon", "Summons protective dragon", 40.0f, 0.0f, 5.0f, 0.0f, 20, 0, 0};
    armor1->m_skill2 = {"demon_shield", "Demon Shield", "Damage reduction shield", 30.0f, 0.0f, 8.0f, 0.0f, 15, 0, 0};
    m_inventory.push_back(armor1);
    
    // Trinkets
    auto trinket1 = createSampleItem(
        "trinket_soul", "Soul Fragment", "Fragment of fallen master's power",
        EquipmentSlot::Trinket, ItemRarity::Unique, 0, 0, 20
    );
    trinket1->m_skill1 = {"thunder_spear", "Thunderous Flying Spear", "Lightning spear projectile", 35.0f, 300.0f, 0.0f, 500.0f, 22, 8, 25};
    trinket1->m_skill2 = {"soul_burst", "Soul Burst", "AoE explosion", 50.0f, 400.0f, 0.0f, 300.0f, 25, 10, 30};
    m_inventory.push_back(trinket1);
    
    // Fashion items (no combat skills)
    auto fashion1 = createSampleItem(
        "hair_demon", "Demon Lord Hair", "Flowing dark hair",
        EquipmentSlot::Fashion_Hair, ItemRarity::Rare, 0, 0, 0
    );
    m_inventory.push_back(fashion1);
    
    auto fashion2 = createSampleItem(
        "face_mask", "Battle Mask", "Intimidating war mask",
        EquipmentSlot::Fashion_Face, ItemRarity::Uncommon, 0, 0, 0
    );
    m_inventory.push_back(fashion2);
}

std::shared_ptr<EquipmentItem> EquipmentScreen::createSampleItem(
    const std::string& id, const std::string& name, const std::string& desc,
    EquipmentSlot slot, ItemRarity rarity, int atk, int def, int spd) {
    
    auto item = std::make_shared<EquipmentItem>();
    item->m_id = id;
    item->m_name = name;
    item->m_description = desc;
    item->m_slot = slot;
    item->m_rarity = rarity;
    item->m_attackBonus = atk;
    item->m_defenseBonus = def;
    item->m_speedBonus = spd;
    
    return item;
}

void EquipmentScreen::buildUI() {
    m_rootPanel = std::make_shared<UIPanel>("EquipmentScreen", XMFLOAT2(0, 0), XMFLOAT2(1920, 1080));
    m_rootPanel->setBackgroundColor(XMFLOAT4(0.05f, 0.05f, 0.1f, 1.0f));
    m_rootPanel->setBorder(false);
    
    auto title = std::make_shared<UILabel>("Title", XMFLOAT2(50, 30), "EQUIPMENT & INVENTORY", 42.0f);
    title->setTextColor(XMFLOAT4(0.9f, 0.7f, 0.2f, 1.0f));
    m_rootPanel->addChild(title);
    
    createCharacterPreviewPanel();
    createEquipmentSlotsPanel();
    createTotalStatsPanel();
    createInventoryPanel();
    createItemDetailsPanel();
    createCategoryTabs();
    
    auto closeBtn = std::make_shared<UIButton>("CloseBtn", XMFLOAT2(1750, 30), XMFLOAT2(150, 50), "CLOSE");
    closeBtn->setColors(
        XMFLOAT4(0.7f, 0.2f, 0.2f, 1.0f),
        XMFLOAT4(0.8f, 0.3f, 0.3f, 1.0f),
        XMFLOAT4(0.6f, 0.15f, 0.15f, 1.0f)
    );
    closeBtn->setOnClick([this]() {
        if (m_onClose) m_onClose();
    });
    m_rootPanel->addChild(closeBtn);
}

void EquipmentScreen::createCharacterPreviewPanel() {
    m_characterPreviewPanel = std::make_shared<UIPanel>("CharPreview", XMFLOAT2(50, 120), XMFLOAT2(400, 500));
    m_characterPreviewPanel->setBackgroundColor(XMFLOAT4(0.08f, 0.12f, 0.18f, 0.9f));
    
    auto label = std::make_shared<UILabel>("PreviewLabel", XMFLOAT2(120, 230), "CHARACTER", 20.0f);
    label->setTextColor(XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f));
    m_characterPreviewPanel->addChild(label);
    
    auto previewNote = std::make_shared<UILabel>("PreviewNote", XMFLOAT2(100, 260), "3D PREVIEW", 16.0f);
    previewNote->setTextColor(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
    m_characterPreviewPanel->addChild(previewNote);
    
    m_rootPanel->addChild(m_characterPreviewPanel);
}

void EquipmentScreen::createEquipmentSlotsPanel() {
    m_equipmentSlotsPanel = std::make_shared<UIPanel>("EquipSlots", XMFLOAT2(470, 120), XMFLOAT2(450, 500));
    m_equipmentSlotsPanel->setBackgroundColor(XMFLOAT4(0.08f, 0.12f, 0.18f, 0.9f));
    
    auto title = std::make_shared<UILabel>("SlotsTitle", XMFLOAT2(20, 15), "EQUIPPED GEAR", 22.0f);
    title->setTextColor(XMFLOAT4(1.0f, 0.9f, 0.3f, 1.0f));
    m_equipmentSlotsPanel->addChild(title);
    
    struct SlotInfo {
        std::string name;
        EquipmentSlot slot;
        std::string keyBinding;
        XMFLOAT2 position;
    };
    
    std::vector<SlotInfo> slots = {
        {"Weapon", EquipmentSlot::Weapon, "Gear 1", XMFLOAT2(20, 60)},
        {"Helmet", EquipmentSlot::Helmet, "Gear 2", XMFLOAT2(20, 140)},
        {"Armor", EquipmentSlot::Armor, "Gear 3", XMFLOAT2(20, 220)},
        {"Trinket", EquipmentSlot::Trinket, "Gear 4", XMFLOAT2(20, 300)},
        {"Hair", EquipmentSlot::Fashion_Hair, "", XMFLOAT2(20, 380)},
    };
    
    for (const auto& slotInfo : slots) {
        auto slot = createEquipmentSlot(slotInfo.name, slotInfo.slot, 
                                       slotInfo.keyBinding, slotInfo.position);
        m_equipmentSlotsPanel->addChild(slot);
    }
    
    m_rootPanel->addChild(m_equipmentSlotsPanel);
}

std::shared_ptr<UIPanel> EquipmentScreen::createEquipmentSlot(const std::string& name, EquipmentSlot slot,
                                              const std::string& keyBinding, XMFLOAT2 pos) {
    auto panel = std::make_shared<UIPanel>("EquipSlot_" + name, pos, XMFLOAT2(410, 70));
    panel->setBackgroundColor(XMFLOAT4(0.15f, 0.2f, 0.25f, 0.8f));
    
    auto iconPanel = std::make_shared<UIPanel>("Icon_" + name, XMFLOAT2(5, 5), XMFLOAT2(60, 60));
    iconPanel->setBackgroundColor(XMFLOAT4(0.2f, 0.25f, 0.3f, 0.9f));
    panel->addChild(iconPanel);
    
    auto nameLabel = std::make_shared<UILabel>("SlotName_" + name, XMFLOAT2(75, 8), name, 18.0f);
    nameLabel->setTextColor(XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f));
    panel->addChild(nameLabel);
    
    if (!keyBinding.empty()) {
        auto keyLabel = std::make_shared<UILabel>("Key_" + name, XMFLOAT2(75, 32), 
                                                   keyBinding + " (2 skills)", 14.0f);
        keyLabel->setTextColor(XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f));
        panel->addChild(keyLabel);
    }
    
    auto equippedItem = m_currentLoadout.getEquippedItem(slot);
    std::string itemName = equippedItem ? equippedItem->m_name : "< Empty >";
    auto itemLabel = std::make_shared<UILabel>("Item_" + name, XMFLOAT2(75, 48), itemName, 14.0f);
    itemLabel->setTextColor(equippedItem ? equippedItem->getRarityColor() : XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
    panel->addChild(itemLabel);
    
    auto unequipBtn = std::make_shared<UIButton>("Unequip_" + name, XMFLOAT2(320, 15), XMFLOAT2(80, 40), "Remove");
    unequipBtn->setColors(
        XMFLOAT4(0.4f, 0.3f, 0.3f, 1.0f),
        XMFLOAT4(0.5f, 0.4f, 0.4f, 1.0f),
        XMFLOAT4(0.35f, 0.25f, 0.25f, 1.0f)
    );
    unequipBtn->setEnabled(equippedItem != nullptr);
    unequipBtn->setOnClick([this, slot]() {
        unequipSlot(slot);
    });
    panel->addChild(unequipBtn);
    
    return panel;
}

void EquipmentScreen::createTotalStatsPanel() {
    auto panel = std::make_shared<UIPanel>("TotalStats", XMFLOAT2(470, 640), XMFLOAT2(450, 120));
    panel->setBackgroundColor(XMFLOAT4(0.08f, 0.12f, 0.18f, 0.9f));
    
    auto title = std::make_shared<UILabel>("StatsTitle", XMFLOAT2(20, 15), "TOTAL STATS", 20.0f);
    title->setTextColor(XMFLOAT4(1.0f, 0.9f, 0.3f, 1.0f));
    panel->addChild(title);
    
    m_totalStatsLabel = std::make_shared<UILabel>("Stats", XMFLOAT2(20, 50), "", 18.0f);
    updateTotalStatsDisplay();
    panel->addChild(m_totalStatsLabel);
    
    m_rootPanel->addChild(panel);
}

void EquipmentScreen::createInventoryPanel() {
    m_inventoryPanel = std::make_shared<UIPanel>("InventoryPanel", XMFLOAT2(940, 120), XMFLOAT2(930, 640));
    m_inventoryPanel->setBackgroundColor(XMFLOAT4(0.08f, 0.12f, 0.18f, 0.9f));
    
    auto title = std::make_shared<UILabel>("InvTitle", XMFLOAT2(20, 15), "INVENTORY", 22.0f);
    title->setTextColor(XMFLOAT4(1.0f, 0.9f, 0.3f, 1.0f));
    m_inventoryPanel->addChild(title);
    
    populateInventoryGrid();
    
    m_rootPanel->addChild(m_inventoryPanel);
}

void EquipmentScreen::populateInventoryGrid() {
    // Clear existing items
    for (auto& child : m_inventoryPanel->getChildren()) {
        if (child->getId().find("InvItem_") == 0) {
            // This would need a proper implementation in UIPanel
        }
    }
    
    int columns = 6;
    float itemWidth = 140;
    float itemHeight = 140;
    float spacing = 10;
    float startX = 20;
    float startY = 60;
    
    int row = 0;
    int col = 0;
    
    for (const auto& item : m_inventory) {
        if (m_currentCategory != "All") {
            std::string itemCategory = getSlotCategory(item->m_slot);
            if (itemCategory != m_currentCategory) continue;
        }
        
        float x = startX + col * (itemWidth + spacing);
        float y = startY + row * (itemHeight + spacing);
        
        auto itemCard = createInventoryItemCard(item, XMFLOAT2(x, y), XMFLOAT2(itemWidth, itemHeight));
        m_inventoryPanel->addChild(itemCard);
        
        col++;
        if (col >= columns) {
            col = 0;
            row++;
        }
    }
}

std::shared_ptr<UIPanel> EquipmentScreen::createInventoryItemCard(std::shared_ptr<EquipmentItem> item,
                                                                  XMFLOAT2 pos, XMFLOAT2 size) {
    auto card = std::make_shared<UIPanel>("InvItem_" + item->m_id, pos, size);
    
    XMFLOAT4 bgColor = XMFLOAT4(0.15f, 0.2f, 0.25f, 0.8f);
    if (m_selectedItem == item) {
        bgColor = XMFLOAT4(0.25f, 0.35f, 0.3f, 0.9f);
    }
    card->setBackgroundColor(bgColor);
    
    // Item icon placeholder
    auto iconPanel = std::make_shared<UIPanel>("Icon_" + item->m_id, XMFLOAT2(10, 10), XMFLOAT2(size.x - 20, 60));
    iconPanel->setBackgroundColor(XMFLOAT4(0.2f, 0.25f, 0.3f, 0.9f));
    card->addChild(iconPanel);
    
    // Item name
    auto nameLabel = std::make_shared<UILabel>("Name_" + item->m_id, XMFLOAT2(10, 75), item->m_name, 14.0f);
    nameLabel->setTextColor(item->getRarityColor());
    card->addChild(nameLabel);
    
    // Stats summary
    std::string stats = "";
    if (item->m_attackBonus > 0) stats += "ATK+" + std::to_string(item->m_attackBonus) + " ";
    if (item->m_defenseBonus > 0) stats += "DEF+" + std::to_string(item->m_defenseBonus) + " ";
    if (item->m_speedBonus > 0) stats += "SPD+" + std::to_string(item->m_speedBonus);
    
    if (!stats.empty()) {
        auto statsLabel = std::make_shared<UILabel>("Stats_" + item->m_id, XMFLOAT2(10, 95), stats, 12.0f);
        statsLabel->setTextColor(XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f));
        card->addChild(statsLabel);
    }
    
    // Select button
    auto selectBtn = std::make_shared<UIButton>("Select_" + item->m_id, 
                                               XMFLOAT2(10, size.y - 35), 
                                               XMFLOAT2(size.x - 20, 25), 
                                               "Select");
    selectBtn->setColors(
        XMFLOAT4(0.3f, 0.4f, 0.5f, 1.0f),
        XMFLOAT4(0.4f, 0.5f, 0.6f, 1.0f),
        XMFLOAT4(0.25f, 0.35f, 0.45f, 1.0f)
    );
    selectBtn->setOnClick([this, item]() {
        selectItem(item);
    });
    card->addChild(selectBtn);
    
    return card;
}

void EquipmentScreen::createItemDetailsPanel() {
    m_itemDetailsPanel = std::make_shared<UIPanel>("ItemDetails", XMFLOAT2(50, 640), XMFLOAT2(400, 340));
    m_itemDetailsPanel->setBackgroundColor(XMFLOAT4(0.08f, 0.12f, 0.18f, 0.9f));
    
    auto title = std::make_shared<UILabel>("DetailsTitle", XMFLOAT2(20, 15), "ITEM DETAILS", 20.0f);
    title->setTextColor(XMFLOAT4(1.0f, 0.9f, 0.3f, 1.0f));
    m_itemDetailsPanel->addChild(title);
    
    auto noSelection = std::make_shared<UILabel>("NoSelection", XMFLOAT2(100, 150), 
                                                "Select an item to view details", 16.0f);
    noSelection->setTextColor(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
    m_itemDetailsPanel->addChild(noSelection);
    
    m_rootPanel->addChild(m_itemDetailsPanel);
}

void EquipmentScreen::createCategoryTabs() {
    auto tabsPanel = std::make_shared<UIPanel>("CategoryTabs", XMFLOAT2(940, 780), XMFLOAT2(930, 60));
    tabsPanel->setBackgroundColor(XMFLOAT4(0.08f, 0.12f, 0.18f, 0.9f));
    
    std::vector<std::string> categories = {"All", "Combat", "Fashion", "Materials"};
    float tabX = 20;
    
    for (const auto& cat : categories) {
        auto tabBtn = std::make_shared<UIButton>("Tab_" + cat, XMFLOAT2(tabX, 10), XMFLOAT2(220, 40), cat);
        
        if (cat == m_currentCategory) {
            tabBtn->setColors(
                XMFLOAT4(0.3f, 0.5f, 0.7f, 1.0f),
                XMFLOAT4(0.4f, 0.6f, 0.8f, 1.0f),
                XMFLOAT4(0.25f, 0.45f, 0.65f, 1.0f)
            );
        } else {
            tabBtn->setColors(
                XMFLOAT4(0.2f, 0.3f, 0.4f, 1.0f),
                XMFLOAT4(0.3f, 0.4f, 0.5f, 1.0f),
                XMFLOAT4(0.15f, 0.25f, 0.35f, 1.0f)
            );
        }
        
        tabBtn->setOnClick([this, cat]() {
            filterByCategory(cat);
        });
        
        tabsPanel->addChild(tabBtn);
        tabX += 230;
    }
    
    m_rootPanel->addChild(tabsPanel);
}

void EquipmentScreen::equipItem(std::shared_ptr<EquipmentItem> item) {
    if (!item) return;
    
    m_currentLoadout.equipItem(item);
    refreshEquipmentSlots();
    updateTotalStatsDisplay();
    
    if (m_onEquipmentChanged) {
        m_onEquipmentChanged();
    }
}

void EquipmentScreen::unequipSlot(EquipmentSlot slot) {
    m_currentLoadout.unequipSlot(slot);
    refreshEquipmentSlots();
    updateTotalStatsDisplay();
    
    if (m_onEquipmentChanged) {
        m_onEquipmentChanged();
    }
}

void EquipmentScreen::selectItem(std::shared_ptr<EquipmentItem> item) {
    m_selectedItem = item;
    updateItemDetails();
    refreshInventory();
}

void EquipmentScreen::updateTotalStatsDisplay() {
    if (!m_totalStatsLabel) return;
    
    std::string stats = "Attack: " + std::to_string(m_currentLoadout.getTotalAttack()) +
                       "  Defense: " + std::to_string(m_currentLoadout.getTotalDefense()) +
                       "  Speed: " + std::to_string(m_currentLoadout.getTotalSpeed()) +
                       "\nHealth Bonus: " + std::to_string(m_currentLoadout.getTotalHealth()) +
                       "  Mana Bonus: " + std::to_string(m_currentLoadout.getTotalMana());
    
    m_totalStatsLabel->setText(stats);
}

void EquipmentScreen::updateItemDetails() {
    if (!m_itemDetailsPanel) return;
    
    // Clear existing details
    m_itemDetailsPanel->clearChildren();
    
    auto title = std::make_shared<UILabel>("DetailsTitle", XMFLOAT2(20, 15), "ITEM DETAILS", 20.0f);
    title->setTextColor(XMFLOAT4(1.0f, 0.9f, 0.3f, 1.0f));
    m_itemDetailsPanel->addChild(title);
    
    if (!m_selectedItem) {
        auto noSelection = std::make_shared<UILabel>("NoSelection", XMFLOAT2(100, 150), 
                                                    "Select an item to view details", 16.0f);
        noSelection->setTextColor(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
        m_itemDetailsPanel->addChild(noSelection);
        return;
    }
    
    // Item name
    auto nameLabel = std::make_shared<UILabel>("ItemName", XMFLOAT2(20, 50), 
                                               m_selectedItem->m_name, 22.0f);
    nameLabel->setTextColor(m_selectedItem->getRarityColor());
    m_itemDetailsPanel->addChild(nameLabel);
    
    // Description
    auto descLabel = std::make_shared<UILabel>("ItemDesc", XMFLOAT2(20, 80), 
                                               m_selectedItem->m_description, 14.0f);
    descLabel->setTextColor(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
    m_itemDetailsPanel->addChild(descLabel);
    
    // Stats
    float yPos = 120;
    if (m_selectedItem->m_attackBonus > 0) {
        auto atkLabel = std::make_shared<UILabel>("AtkBonus", XMFLOAT2(20, yPos), 
                                                  "Attack: +" + std::to_string(m_selectedItem->m_attackBonus), 16.0f);
        atkLabel->setTextColor(XMFLOAT4(0.9f, 0.6f, 0.6f, 1.0f));
        m_itemDetailsPanel->addChild(atkLabel);
        yPos += 25;
    }
    
    if (m_selectedItem->m_defenseBonus > 0) {
        auto defLabel = std::make_shared<UILabel>("DefBonus", XMFLOAT2(20, yPos),
                                                  "Defense: +" + std::to_string(m_selectedItem->m_defenseBonus), 16.0f);
        defLabel->setTextColor(XMFLOAT4(0.6f, 0.8f, 0.9f, 1.0f));
        m_itemDetailsPanel->addChild(defLabel);
        yPos += 25;
    }
    
    if (m_selectedItem->m_speedBonus > 0) {
        auto spdLabel = std::make_shared<UILabel>("SpdBonus", XMFLOAT2(20, yPos),
                                                  "Speed: +" + std::to_string(m_selectedItem->m_speedBonus), 16.0f);
        spdLabel->setTextColor(XMFLOAT4(0.9f, 0.9f, 0.6f, 1.0f));
        m_itemDetailsPanel->addChild(spdLabel);
        yPos += 25;
    }
    
    // Skills (for combat equipment)
    if (m_selectedItem->hasCombatSkills()) {
        yPos += 10;
        auto skillsTitle = std::make_shared<UILabel>("SkillsTitle", XMFLOAT2(20, yPos), "SKILLS:", 18.0f);
        skillsTitle->setTextColor(XMFLOAT4(1.0f, 0.8f, 0.2f, 1.0f));
        m_itemDetailsPanel->addChild(skillsTitle);
        yPos += 25;
        
        // Skill 1
        auto skill1Name = std::make_shared<UILabel>("Skill1Name", XMFLOAT2(20, yPos),
                                                    "• " + m_selectedItem->m_skill1.m_name, 16.0f);
        skill1Name->setTextColor(XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f));
        m_itemDetailsPanel->addChild(skill1Name);
        yPos += 20;
        
        auto skill1Desc = std::make_shared<UILabel>("Skill1Desc", XMFLOAT2(30, yPos),
                                                    m_selectedItem->m_skill1.m_description, 14.0f);
        skill1Desc->setTextColor(XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f));
        m_itemDetailsPanel->addChild(skill1Desc);
        yPos += 20;
        
        auto skill1Mana = std::make_shared<UILabel>("Skill1Mana", XMFLOAT2(30, yPos),
                                                    "Mana: " + std::to_string(static_cast<int>(m_selectedItem->m_skill1.m_manaCost)), 14.0f);
        skill1Mana->setTextColor(XMFLOAT4(0.3f, 0.5f, 0.9f, 1.0f));
        m_itemDetailsPanel->addChild(skill1Mana);
        yPos += 30;
        
        // Skill 2
        auto skill2Name = std::make_shared<UILabel>("Skill2Name", XMFLOAT2(20, yPos),
                                                    "• " + m_selectedItem->m_skill2.m_name, 16.0f);
        skill2Name->setTextColor(XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f));
        m_itemDetailsPanel->addChild(skill2Name);
        yPos += 20;
        
        auto skill2Desc = std::make_shared<UILabel>("Skill2Desc", XMFLOAT2(30, yPos),
                                                    m_selectedItem->m_skill2.m_description, 14.0f);
        skill2Desc->setTextColor(XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f));
        m_itemDetailsPanel->addChild(skill2Desc);
        yPos += 20;
        
        auto skill2Mana = std::make_shared<UILabel>("Skill2Mana", XMFLOAT2(30, yPos),
                                                    "Mana: " + std::to_string(static_cast<int>(m_selectedItem->m_skill2.m_manaCost)), 14.0f);
        skill2Mana->setTextColor(XMFLOAT4(0.3f, 0.5f, 0.9f, 1.0f));
        m_itemDetailsPanel->addChild(skill2Mana);
    }
    
    // Equip button
    auto equipBtn = std::make_shared<UIButton>("EquipBtn", XMFLOAT2(20, 290), XMFLOAT2(360, 40), "EQUIP ITEM");
    equipBtn->setColors(
        XMFLOAT4(0.2f, 0.6f, 0.3f, 1.0f),
        XMFLOAT4(0.3f, 0.7f, 0.4f, 1.0f),
        XMFLOAT4(0.15f, 0.5f, 0.25f, 1.0f)
    );
    equipBtn->setOnClick([this]() {
        if (m_selectedItem) {
            equipItem(m_selectedItem);
        }
    });
    m_itemDetailsPanel->addChild(equipBtn);
}

void EquipmentScreen::refreshEquipmentSlots() {
    createEquipmentSlotsPanel();
}

void EquipmentScreen::refreshInventory() {
    populateInventoryGrid();
}

void EquipmentScreen::filterByCategory(const std::string& category) {
    m_currentCategory = category;
    refreshInventory();
    createCategoryTabs();
}

std::string EquipmentScreen::getSlotCategory(EquipmentSlot slot) const {
    switch (slot) {
        case EquipmentSlot::Weapon:
        case EquipmentSlot::Helmet:
        case EquipmentSlot::Armor:
        case EquipmentSlot::Trinket:
            return "Combat";
        case EquipmentSlot::Fashion_Hair:
        case EquipmentSlot::Fashion_Face:
        case EquipmentSlot::Fashion_Body:
            return "Fashion";
        default:
            return "All";
    }
}