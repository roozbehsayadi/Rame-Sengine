
#include <memory>
#include <stdlib.h>
#include <tuple>
#include <vector>

#include "RUI/RUI.h"
#include "RUI/layout/ColumnLayout.h"
#include "RUI/layout/LeafLayout.h"
#include "RUI/layout/RowLayout.h"
#include "RUI/utils/Rect.h"
#include "RUI/widgets/ButtonWidget.h"
#include "RUI/widgets/CheckboxWidget.h"
#include "RUI/widgets/ImageWidget.h"
#include "RUI/widgets/RadioButtonWidget.h"
#include "RUI/widgets/ScreenWidget.h"
#include "RUI/widgets/TextInputWidget.h"
#include "RUI/widgets/elements/ScreenObject.h"
#include "RUI/window/GeneralPage.h"

#include "Sprite.h"

int frameAddressCounter = 0;

std::vector<Sprite> sprites;
int selectedSpriteIndex = -1;

void fillPage(GeneralPage &);

void addSpriteFrameAddressTextInput();
void popSpriteFrameAddressTextInputs();

// output:
// name of the sprite
// FPS
// frames of the sprite
// if the required fields were filled
std::tuple<std::string, double, std::vector<std::string>, bool> extractNewSpriteAttributes();

void generateSpritesListWidgets(const Sprite &);

int main() {
  GeneralPage page("main_1", "rui");

  RUI::getInstance().addWindow(&page);

  fillPage(page);

  RUI::getInstance().render();

  auto addFrameButton =
      std::dynamic_pointer_cast<ButtonWidget>(RUI::getInstance().getWidget("add_frame_to_sprite").first);
  auto popFrameButton =
      std::dynamic_pointer_cast<ButtonWidget>(RUI::getInstance().getWidget("pop_frame_from_sprite").first);
  auto addSpriteButton =
      std::dynamic_pointer_cast<ButtonWidget>(RUI::getInstance().getWidget("add_sprite_to_sprites").first);

  bool quit = false;
  while (!quit) {
    quit = RUI::getInstance().handleEvents();

    if (addFrameButton->isClicked()) {
      addSpriteFrameAddressTextInput();
    }
    if (popFrameButton->isClicked()) {
      popSpriteFrameAddressTextInputs();
    }
    if (addSpriteButton->isClicked()) {
      auto [spriteName, spriteFPs, spriteFileNames, valid] = extractNewSpriteAttributes();
      if (valid) {
        Sprite sprite(spriteName, spriteFPs);
        for (auto fileName : spriteFileNames)
          sprite.insertFrame(fileName);

        sprites.push_back(sprite);
        generateSpritesListWidgets(sprite);
      } else {
        std::cerr << "invalid!" << std::endl;
      }
    }

    RUI::getInstance().render();
  }

  return EXIT_SUCCESS;
}

void fillPage(GeneralPage &page) {
  auto grid = page.getGrid();

  auto spritesAndObjectsRow = std::make_shared<RowLayout>("sprites_and_objects", 0.9, 0.3, 0.0, 0.0, 0.05, 0.02);

  // Sprites Section

  // Create a sprite
  auto spritesColumn = std::make_shared<ColumnLayout>("sprite_frames", 0.4, 0.9, 0.0, 0.0, 0.02, 0.05);

  auto spriteAddressColumn =
      std::make_shared<ColumnLayout>("sprite_address_column", 0.95, 0.45, 0.0, 0.0, 0.025, 0.025);
  spritesColumn->addChild(spriteAddressColumn);

  auto spriteButtonsRow = std::make_shared<RowLayout>("sprite_buttons_column", 0.95, 0.2, 0.0, 0.0, 0.025, 0.025);

  auto spriteNameAndFpsRow = std::make_shared<RowLayout>("sprite_name_and_fps_row", 0.95, 0.20, 0.0, 0.0, 0.025, 0.025);

  auto spriteNameLeaf = std::make_shared<LeafLayout>("sprite_name_column", 0.45, 0.95, 0.0, 0.0, 0.025, 0.025);
  auto spriteNameWidget = std::make_shared<TextInputWidget>("sprite_name_text_input");
  spriteNameLeaf->setWidget(spriteNameWidget);

  spriteNameAndFpsRow->addChild(spriteNameLeaf);

  auto spriteFpsLeaf = std::make_shared<LeafLayout>("sprite_fps_leaf", 0.45, 0.95, 0.0, 0.0, 0.025, 0.025);
  auto spriteFpsWidget = std::make_shared<TextInputWidget>("sprite_fps_text_input");
  spriteFpsLeaf->setWidget(spriteFpsWidget);

  spriteNameAndFpsRow->addChild(spriteFpsLeaf);

  spritesColumn->addChild(spriteNameAndFpsRow);

  spritesColumn->addChild(spriteButtonsRow);

  auto spriteAddFrameLeaf = std::make_shared<LeafLayout>("sprite_add_frame_layout", 0.3, 0.9, 0.0, 0.0, 0.016, 0.05);
  auto spriteAddFrameWidget = std::make_shared<ButtonWidget>("add_frame_to_sprite", "Add Frame");
  spriteAddFrameLeaf->setWidget(spriteAddFrameWidget);

  auto spriteRemoveFrameLeaf = std::make_shared<LeafLayout>("sprite_pop_frame_layout", 0.3, 0.9, 0.0, 0.0, 0.016, 0.05);
  auto spriteRemoveFrameWidget = std::make_shared<ButtonWidget>("pop_frame_from_sprite", "Pop Frame");
  spriteRemoveFrameLeaf->setWidget(spriteRemoveFrameWidget);

  auto spriteAddSpriteLeaf = std::make_shared<LeafLayout>("sprite_add_sprite_layout", 0.3, 0.9, 0.0, 0.0, 0.016, 0.05);
  auto spriteAddSpriteWidget = std::make_shared<ButtonWidget>("add_sprite_to_sprites", "Create");
  spriteAddSpriteLeaf->setWidget(spriteAddSpriteWidget);

  spriteButtonsRow->addChild(spriteAddFrameLeaf);
  spriteButtonsRow->addChild(spriteRemoveFrameLeaf);
  spriteButtonsRow->addChild(spriteAddSpriteLeaf);

  spritesAndObjectsRow->addChild(spritesColumn);
  grid->addChild(spritesAndObjectsRow);

  for (auto i = 0u; i < 10; i++) {
    addSpriteFrameAddressTextInput();
  }

  // List of sprites
  auto spritesListColumn = std::make_shared<ColumnLayout>("sprites_list_column", 0.3, 0.9, 0.0, 0.0, 0.05, 0.05);

  spritesAndObjectsRow->addChild(spritesListColumn);
}

void addSpriteFrameAddressTextInput() {
  auto addressesColumn = std::dynamic_pointer_cast<ColumnLayout>(RUI::getInstance().getLayout("sprite_address_column"));

  auto leafLayout = std::make_shared<LeafLayout>("sprite_address_layout_" + std::to_string(frameAddressCounter), 0.95,
                                                 0.2, 0.0, 0.0, 0.025, 0.025);
  auto widget = std::make_shared<TextInputWidget>("sprite_address_text_input_" + std::to_string(frameAddressCounter));

  leafLayout->setWidget(widget);
  addressesColumn->addChild(leafLayout);

  frameAddressCounter++;
}

void popSpriteFrameAddressTextInputs() {

  if (frameAddressCounter <= 1)
    return;

  auto addressesColumn = std::dynamic_pointer_cast<ColumnLayout>(RUI::getInstance().getLayout("sprite_address_column"));

  frameAddressCounter--;

  addressesColumn->removeChild("sprite_address_layout_" + std::to_string(frameAddressCounter));
}

std::tuple<std::string, double, std::vector<std::string>, bool> extractNewSpriteAttributes() {
  auto spriteNameTextInputWidget =
      std::dynamic_pointer_cast<TextInputWidget>(RUI::getInstance().getWidget("sprite_name_text_input").first);
  auto spriteName = spriteNameTextInputWidget->getText();

  if (spriteName == "")
    return std::make_tuple<std::string, double, std::vector<std::string>, bool>("", 0.0, {}, false);

  auto spriteFpsTextInputWidget =
      std::dynamic_pointer_cast<TextInputWidget>(RUI::getInstance().getWidget("sprite_fps_text_input").first);

  double spriteFps;
  try {
    spriteFps = std::stod(spriteFpsTextInputWidget->getText());
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return std::make_tuple<std::string, double, std::vector<std::string>, bool>("", 0.0, {}, false);
  }

  if (spriteFps <= 0) {
    return std::make_tuple<std::string, double, std::vector<std::string>, bool>("", 0.0, {}, false);
  }

  std::vector<std::string> fileNames;
  for (auto i = 0u; i < frameAddressCounter; i++) {
    auto currentTextInput = std::dynamic_pointer_cast<TextInputWidget>(
        RUI::getInstance().getWidget("sprite_address_text_input_" + std::to_string(i)).first);
    auto currentAddress = currentTextInput->getText();
    if (currentAddress != "")
      fileNames.push_back(currentAddress);
  }

  if (fileNames.empty())
    return std::make_tuple<std::string, double, std::vector<std::string>, bool>("", 0.0, {}, false);

  return std::make_tuple(spriteName, spriteFps, fileNames, true);
}

void generateSpritesListWidgets(const Sprite &sprite) {
  auto spritesListColumn = std::dynamic_pointer_cast<ColumnLayout>(RUI::getInstance().getLayout("sprites_list_column"));

  auto spriteLeaf =
      std::make_shared<LeafLayout>("sprite_list_leaf_" + sprite.getName(), 0.95, 0.2, 0.0, 0.0, 0.025, 0.025);

  auto spriteWidget = std::make_shared<RadioButtonWidget<int>>(
      "sprite_list_radio_button_" + sprite.getName(), selectedSpriteIndex, sprite.getName(), sprites.size() - 1);

  spriteLeaf->setWidget(spriteWidget);
  spritesListColumn->addChild(spriteLeaf);
}
