#pragma once

#include "UIPanel.h"
#include "UIButton.h"
#include "UILabel.h"
#include "CharacterData.h"
#include <vector>
#include <functional>
#include <memory>
#include <d3d11.h>

namespace ArenaFighter {

class CharacterSelectionScreen {
private:
    std::shared_ptr<UIPanel> m_rootPanel;
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_context;
    
    std::vector<int> m_selectedCharacterIndices;
    int m_maxSlots;
    
    std::vector<CharacterData> m_characterRoster;
    std::string m_currentCategory;
    
    std::shared_ptr<UIPanel> m_characterListPanel;
    std::vector<std::shared_ptr<UIPanel>> m_slotPanels;
    
    std::function<void()> m_onConfirmSelection;
    std::function<void()> m_onCancel;

public:
    CharacterSelectionScreen(ID3D11Device* dev, ID3D11DeviceContext* ctx, int slots = 3);
    
    std::shared_ptr<UIPanel> getRootPanel() { return m_rootPanel; }
    
    void setCallbacks(
        std::function<void()> confirmCallback,
        std::function<void()> cancelCallback
    );
    
    std::vector<CharacterData> getSelectedCharacters() const;
    void setMaxSlots(int slots);

private:
    void initializeCharacterRoster();
    CharacterData createCharacter(const std::string& id, const std::string& name, 
                                  const std::string& category, int level,
                                  int atk, int def, int spc);
    
    void buildUI();
    std::shared_ptr<UIPanel> createCharacterSlotsPanel();
    std::shared_ptr<UIPanel> createCategoryPanel();
    std::shared_ptr<UIPanel> createCharacterListPanel();
    std::shared_ptr<UIPanel> createCharacterCard(int characterIndex, XMFLOAT2 pos, XMFLOAT2 size);
    
    void populateCharacterList(std::shared_ptr<UIPanel> panel);
    void selectCharacter(int characterIndex);
    void clearSlot(int slotIndex);
    void updateSlotDisplay(int slotIndex);
    void filterByCategory(const std::string& category);
    void refreshCharacterList();
    void refreshSlotPanels();
    
    bool isCharacterSelected(int characterIndex) const;
    bool canConfirm() const;
    
    template<typename T>
    std::shared_ptr<T> findChildByName(std::shared_ptr<UIPanel> parent, const std::string& name);
    
    std::shared_ptr<UIPanel> createGearSelectionPanel();
};

} // namespace ArenaFighter