
#include <map>
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

#include "Object.h"
#include "Room.h"
#include "Sprite.h"

int frameAddressCounter = 0;

std::map<std::string, Sprite> sprites;
// std::vector<Sprite> sprites;
std::string selectedSpriteName = "";
// int selectedSpriteIndex = -1;

std::map<std::string, Object> objects;
std::string selectedObjectName = "";

std::map<std::string, Room> rooms;
// std::map<std::string, std::shared_ptr<LeafLayout>> roomScreenLayouts;
std::string selectedRoomName = "";

void fillPage(GeneralPage &);

void addSpriteFrameAddressTextInput();
void popSpriteFrameAddressTextInputs();

// output:
// name of the sprite
// FPS
// frames of the sprite
// if the required fields were filled
std::tuple<std::string, double, std::vector<std::string>, bool> extractNewSpriteAttributes();

// output:
// name of the sprite
// if the required fields were filled
std::tuple<std::string, bool> extractNewRoomAttributes();

void generateSpritesListWidgets(const Sprite &);
void removeSpriteRadioButtonWidget(const std::string &);

bool isSelectedObjectChanged() {
  static std::string lastValue = "";
  if (selectedObjectName == "")
    return false;
  const std::string &currentObjectName = objects.find(selectedObjectName)->second.getName();
  if (lastValue != currentObjectName) {
    lastValue = currentObjectName;
    return true;
  }
  return false;
}

bool isSelectedSpriteChanged() {
  static std::string lastValue = "";
  if (selectedSpriteName == "")
    return false;
  const std::string &currentSpriteName = sprites.find(selectedSpriteName)->second.getName();
  if (lastValue != currentSpriteName) {
    lastValue = currentSpriteName;
    return true;
  }
  return false;
}

bool isSelectedRoomChanged() {
  static std::string lastValue = "";
  if (selectedRoomName == "") {
    lastValue = "";
    return false;
  }
  const std::string &currentRoomName = rooms.find(selectedRoomName)->second.getName();
  if (lastValue != currentRoomName) {
    lastValue = currentRoomName;
    return true;
  }
  return false;
}

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
  auto removeSpriteButton =
      std::dynamic_pointer_cast<ButtonWidget>(RUI::getInstance().getWidget("remove_sprite_button").first);
  auto createObjectButton =
      std::dynamic_pointer_cast<ButtonWidget>(RUI::getInstance().getWidget("object_create_button").first);
  auto createObjectText =
      std::dynamic_pointer_cast<TextInputWidget>(RUI::getInstance().getWidget("object_name_text_input").first);
  auto spritePreviewImage =
      std::dynamic_pointer_cast<ImageWidget>(RUI::getInstance().getWidget("sprite_preview_thumbnail").first);
  auto createRoomButton =
      std::dynamic_pointer_cast<ButtonWidget>(RUI::getInstance().getWidget("create_room_button").first);

  bool quit = false;
  while (!quit) {
    // std::cout << selectedSpriteIndex << std::endl;
    quit = RUI::getInstance().handleEvents();

    if (isSelectedObjectChanged())
      selectedSpriteName = objects.find(selectedObjectName)->second.getSpriteName();

    if (isSelectedSpriteChanged())
      spritePreviewImage->changeImagePath(sprites.find(selectedSpriteName)->second.getFirstFrame()->getImagePath());

    if (isSelectedRoomChanged()) {
      for (auto &[roomName, roomObject] : rooms) {
        auto roomLayout = RUI::getInstance().getLayout("room_screen_layout_" + roomName);
        if (roomName == selectedRoomName)
          roomLayout->show();
        else
          roomLayout->hide();
      }
    }

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

        sprites.insert({sprite.getName(), sprite});
        generateSpritesListWidgets(sprite);
      } else {
        std::cerr << "invalid!" << std::endl;
      }
    }
    if (removeSpriteButton->isClicked()) {
      if (selectedSpriteName != "") {
        sprites.erase(selectedSpriteName);
        removeSpriteRadioButtonWidget(selectedSpriteName);
        selectedSpriteName = "";
      }
    }
    if (createObjectButton->isClicked()) {
      std::string objectName = createObjectText->getText();
      if (objectName != "" && selectedSpriteName != "") {
        Object object("object_" + objectName, sprites[selectedSpriteName]);
        objects.insert({objectName, object});

        auto objectsListColumn =
            std::dynamic_pointer_cast<ColumnLayout>(RUI::getInstance().getLayout("objects_list_column"));
        auto newObjectLeaf = std::make_shared<LeafLayout>("object_leaf_" + objectName, 0.9, 0.2, 0.0, 0.0, 0.05, 0.05);
        auto newObjectWidget = std::make_shared<RadioButtonWidget<std::string>>(
            "object_select_radio_button_" + objectName, selectedObjectName, objectName, objectName);
        newObjectLeaf->setWidget(newObjectWidget);
        objectsListColumn->addChild(newObjectLeaf);
      }
    }
    if (createRoomButton->isClicked()) {
      auto [roomName, valid] = extractNewRoomAttributes();
      if (valid) {
        Room room(roomName);
        rooms.insert({roomName, room});

        auto currentRoomLayout =
            std::make_shared<LeafLayout>("room_screen_layout_" + roomName, 0.9, 0.5, 0.0, 0.0, 0.05, 0.025);
        currentRoomLayout->hide();
        page.getGrid()->addChild(currentRoomLayout);
        auto currentRoomScreen = std::make_shared<ScreenWidget>("room_screen_" + roomName);
        currentRoomLayout->setWidget(currentRoomScreen);

        auto roomListColumn = std::dynamic_pointer_cast<ColumnLayout>(RUI::getInstance().getLayout("room_list_column"));
        auto newRoomItemLeaf =
            std::make_shared<LeafLayout>("room_item_leaf_" + roomName, 0.9, 0.2, 0.0, 0.0, 0.05, 0.01);
        auto newRoomItem = std::make_shared<RadioButtonWidget<std::string>>("room_item_" + roomName, selectedRoomName,
                                                                            roomName, roomName);
        newRoomItemLeaf->setWidget(newRoomItem);

        roomListColumn->addChild(newRoomItemLeaf);
      } else {
        std::cerr << "invalid room information" << std::endl;
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
  auto spritesColumn = std::make_shared<ColumnLayout>("sprite_frames", 0.4, 0.9, 0.0, 0.0, 0.01, 0.05);

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
  auto spritesListColumn = std::make_shared<ColumnLayout>("sprites_list_column", 0.2, 0.9, 0.0, 0.0, 0.01, 0.05);

  auto spritesListRadioButtons =
      std::make_shared<ColumnLayout>("sprites_list_radio_buttons", 0.95, 0.75, 0.0, 0.0, 0.025, 0.025);
  spritesListColumn->addChild(spritesListRadioButtons);

  auto spritesListRemoveLeaf = std::make_shared<LeafLayout>("remove_sprite_leaf", 0.33, 0.15, 0.0, 0.0, 0.335, 0.025);
  auto spritesListRemoveButton = std::make_shared<ButtonWidget>("remove_sprite_button", "Remove");
  spritesListRemoveLeaf->setWidget(spritesListRemoveButton);

  spritesListColumn->addChild(spritesListRemoveLeaf);

  spritesAndObjectsRow->addChild(spritesListColumn);

  // Create an object and list of them
  auto objectCreateAndListColumn =
      std::make_shared<ColumnLayout>("object_creation_and_list_column", 0.34, 0.9, 0.0, 0.0, 0.01, 0.05);

  auto objectCreateColumn = std::make_shared<ColumnLayout>("create_object_column", 0.9, 0.4, 0.0, 0.0, 0.05, 0.05);

  auto objectNameLeaf = std::make_shared<LeafLayout>("object_name_leaf", 0.9, 0.5, 0.0, 0.0, 0.05, 0.05);
  auto objectNameText = std::make_shared<TextInputWidget>("object_name_text_input");
  objectNameLeaf->setWidget(objectNameText);

  objectCreateColumn->addChild(objectNameLeaf);

  auto objectCreateObjectButtonLeaf =
      std::make_shared<LeafLayout>("object_create_leaf", 0.25, 0.3, 0.0, 0.0, 0.375, 0.05);
  auto objectCreateButton = std::make_shared<ButtonWidget>("object_create_button", "Create");
  objectCreateObjectButtonLeaf->setWidget(objectCreateButton);

  objectCreateColumn->addChild(objectCreateObjectButtonLeaf);

  objectCreateAndListColumn->addChild(objectCreateColumn);

  auto objectListColumn = std::make_shared<ColumnLayout>("objects_list_column", 0.9, 0.4, 0.0, 0.0, 0.05, 0.05);

  objectCreateAndListColumn->addChild(objectListColumn);

  spritesAndObjectsRow->addChild(objectCreateAndListColumn);

  // Preview of sprites and objects row
  auto spritePreviewAndRoomListRow =
      std::make_shared<RowLayout>("sprite_preview_and_room_list_row", 0.9, 0.3, 0.0, 0.0, 0.05, 0.02);

  auto spritePreviewImageLeaf =
      std::make_shared<LeafLayout>("sprite_preview_thumbnail_leaf", 0.45, 0.9, 0.0, 0.0, 0.025, 0.05);

  spritePreviewAndRoomListRow->addChild(spritePreviewImageLeaf);

  auto spritePreviewImage = std::make_shared<ImageWidget>("sprite_preview_thumbnail", "");

  spritePreviewImageLeaf->setWidget(spritePreviewImage);

  auto roomListAndButtonsColumn = std::make_shared<ColumnLayout>("room_stuff_column", 0.45, 0.9, 0.0, 0.0, 0.025, 0.05);

  spritePreviewAndRoomListRow->addChild(roomListAndButtonsColumn);

  auto roomListColumn = std::make_shared<ColumnLayout>("room_list_column", 0.9, 0.65, 0.0, 0.0, 0.05, 0.025);

  roomListAndButtonsColumn->addChild(roomListColumn);

  auto roomNameAndButtonRow =
      std::make_shared<RowLayout>("room_name_and_create_button_row", 0.9, 0.25, 0.0, 0.0, 0.05, 0.025);

  auto createRoomTextInputLeaf =
      std::make_shared<LeafLayout>("create_room_text_input_leaf", 0.45, 0.9, 0.0, 0.0, 0.025, 0.05);
  auto createRoomTextInput = std::make_shared<TextInputWidget>("room_name_text_input");
  createRoomTextInputLeaf->setWidget(createRoomTextInput);

  auto createRoomButtonLeaf = std::make_shared<LeafLayout>("create_room_button_leaf", 0.45, 0.9, 0.0, 0.0, 0.025, 0.05);
  auto createRoomButton = std::make_shared<ButtonWidget>("create_room_button", "Create Room");
  createRoomButtonLeaf->setWidget(createRoomButton);

  roomNameAndButtonRow->addChild(createRoomTextInputLeaf);
  roomNameAndButtonRow->addChild(createRoomButtonLeaf);

  roomListAndButtonsColumn->addChild(roomNameAndButtonRow);

  grid->addChild(spritePreviewAndRoomListRow);
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

std::tuple<std::string, bool> extractNewRoomAttributes() {
  auto roomNameTextInput =
      std::dynamic_pointer_cast<TextInputWidget>(RUI::getInstance().getWidget("room_name_text_input").first);
  auto enteredRoomName = roomNameTextInput->getText();
  if (enteredRoomName == "")
    return {"", false};
  return {enteredRoomName, true};
}

void generateSpritesListWidgets(const Sprite &sprite) {
  auto spritesListColumn =
      std::dynamic_pointer_cast<ColumnLayout>(RUI::getInstance().getLayout("sprites_list_radio_buttons"));

  auto spriteLeaf =
      std::make_shared<LeafLayout>("sprite_list_leaf_" + sprite.getName(), 0.95, 0.2, 0.0, 0.0, 0.025, 0.025);

  auto spriteWidget = std::make_shared<RadioButtonWidget<std::string>>(
      "sprite_list_radio_button_" + sprite.getName(), selectedSpriteName, sprite.getName(), sprite.getName());

  spriteLeaf->setWidget(spriteWidget);
  spritesListColumn->addChild(spriteLeaf);
}

void removeSpriteRadioButtonWidget(const std::string &spriteName) {
  auto spritesListColumn =
      std::dynamic_pointer_cast<ColumnLayout>(RUI::getInstance().getLayout("sprites_list_radio_buttons"));
  spritesListColumn->removeChild("sprite_list_leaf_" + spriteName);
}
