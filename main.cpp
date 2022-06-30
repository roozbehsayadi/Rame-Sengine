
#include <memory>
#include <stdlib.h>

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

void fillPage(GeneralPage &, const std::string &, const std::string &);
bool showScreen = true, showImage = true, showButton = true, placeholder = false;
int radioVariable = 1;

int main() {
  GeneralPage page("main_1", "rui");
  fillPage(page, "1", "img1");


  // GeneralPage page2("main_2", "rui2");
  // fillPage(page2, "2", "img2");

  // auto rui = RUI::getInstance();
  RUI::getInstance().addWindow(&page);
  // rui.addWindow(&page2);

  int characterCounter = 0;

  RUI::getInstance().render();
  bool quit = false;
  while (!quit) {
    quit = RUI::getInstance().handleEvents();

    auto pressedKey = RUI::getInstance().getPressedKey("main_1");
    auto modifiers = RUI::getInstance().getKeyboardModifiers("main_1");

    // do your stuff here. for example:
    auto button1 = RUI::getInstance().getWidget("button1").first;
    auto buttonLeaf = RUI::getInstance().getLayout("leaf1");
    auto leaf1 = RUI::getInstance().getLayout("big leaf1");
    auto leaf2 = RUI::getInstance().getLayout("big leaf 21");
    auto screen = std::dynamic_pointer_cast<ScreenWidget>(RUI::getInstance().getWidget("screen").first);

    if (button1->isClicked()) {
      Rect tempRect = {300.0, 300.0, -1.0, -1.0};
      screen->insertObject(std::make_shared<ScreenObject>("character_" + std::to_string(characterCounter),
                                                          "assets/images/enemy.png", tempRect));
      characterCounter++;
    }
    leaf1->setHidden(!showScreen);
    leaf2->setHidden(!showImage);
    buttonLeaf->setHidden(!showButton);

    RUI::getInstance().render();
  }

  return EXIT_SUCCESS;
}

void fillPage(GeneralPage &page, const std::string &index, const std::string &imageSlug) {
  auto grid = page.getGrid();

  auto r = std::make_shared<RowLayout>("row asli" + index, 0.80, 0.20, 0.0, 0.10, 0.10, 0.05);
  auto c = std::make_shared<ColumnLayout>("c1" + index, 0.20, 0.30, 0.0, 0.0, 0.04, 0.25);
  c->addChild(std::make_shared<RowLayout>("row dakheli" + index, 0.80, 0.90, 0.0, 0.0, 0.10, 0.05));
  r->addChild(c);
  auto l = std::make_shared<LeafLayout>("leaf" + index, 0.10, 0.50, 0.1, 0.2, 0.025, 0.25);
  auto button = std::make_shared<ButtonWidget>("button" + index, "Button");
  l->setWidget(button);
  r->addChild(l);

  auto checkboxColumn = std::make_shared<ColumnLayout>("checkbox_column" + index, 0.3, 0.8, 0.0, 0.0, 0.04, 0.0);
  auto checkboxContainer1 = std::make_shared<LeafLayout>("checkbox_container_1", 1.0, 0.24, 0.0, 0.0, 0.0, 0.0);
  auto checkBox1 = std::make_shared<CheckboxWidget>("checkbox_1", showScreen, "show screen");
  checkboxContainer1->setWidget(checkBox1);
  checkboxColumn->addChild(checkboxContainer1);

  auto checkboxContainer2 = std::make_shared<LeafLayout>("checkbox_container_2", 1.0, 0.24, 0.0, 0.0, 0.0, 0.0);
  auto checkBox2 = std::make_shared<CheckboxWidget>("checkbox_2", showImage, "show image 2");
  checkboxContainer2->setWidget(checkBox2);
  checkboxColumn->addChild(checkboxContainer2);

  auto checkboxContainer3 = std::make_shared<LeafLayout>("checkbox_container_3", 1.0, 0.24, 0.0, 0.0, 0.0, 0.0);
  auto checkBox3 = std::make_shared<CheckboxWidget>("checkbox_3", showButton, "button visibility");
  checkboxContainer3->setWidget(checkBox3);
  checkboxColumn->addChild(checkboxContainer3);

  auto checkBoxContainer4 = std::make_shared<LeafLayout>("checkbox_container_4", 1.0, 0.24, 0.0, 0.0, 0.0, 0.0);
  auto checkBox4 = std::make_shared<CheckboxWidget>("checkbox_4", placeholder, "nothing");
  checkBox4->setEnabled(false);
  checkBoxContainer4->setWidget(checkBox4);
  checkboxColumn->addChild(checkBoxContainer4);

  r->addChild(checkboxColumn);

  auto radioColumn = std::make_shared<ColumnLayout>("radio_column" + index, 0.3, 0.8, 0.0, 0.0, 0.04, 0.0);
  auto radioContainer1 = std::make_shared<LeafLayout>("radio_container_1", 1.0, 0.25, 0.0, 0.0, 0.0, 0.0);
  auto radio1 = std::make_shared<RadioButtonWidget<int>>("radio_1", radioVariable, "1", 1);
  radioContainer1->setWidget(radio1);
  radioColumn->addChild(radioContainer1);

  auto radioContainer2 = std::make_shared<LeafLayout>("radio_container_2", 1.0, 0.25, 0.0, 0.0, 0.0, 0.0);
  auto radio2 = std::make_shared<RadioButtonWidget<int>>("radio_2", radioVariable, "2", 2);
  radioContainer2->setWidget(radio2);
  radioColumn->addChild(radioContainer2);

  auto radioContainer3 = std::make_shared<LeafLayout>("radio_container_3", 1.0, 0.25, 0.0, 0.0, 0.0, 0.0);
  auto radio3 = std::make_shared<RadioButtonWidget<int>>("radio_3", radioVariable, "3", 3);
  radioContainer3->setWidget(radio3);
  radioColumn->addChild(radioContainer3);

  r->addChild(radioColumn);

  auto textInputColumn = std::make_shared<ColumnLayout>("text_input_column", 0.25, 0.8, 0.0, 0.0, 0.03, 0.0);

  auto textContainer1 = std::make_shared<LeafLayout>("leaf_text_1", 0.9, 0.2, 0.0, 0.0, 0.05, 0.05);
  auto textInput1 = std::make_shared<TextInputWidget>("text_input_1");
  textContainer1->setWidget(textInput1);
  textInputColumn->addChild(textContainer1);

  auto textContainer2 = std::make_shared<LeafLayout>("leaf_text_2", 0.9, 0.2, 0.0, 0.0, 0.05, 0.05);
  auto textInput2 = std::make_shared<TextInputWidget>("text_input_2");
  textContainer2->setWidget(textInput2);
  textInputColumn->addChild(textContainer2);

  auto textContainer3 = std::make_shared<LeafLayout>("leaf_text_3", 0.9, 0.2, 0.0, 0.0, 0.05, 0.05);
  auto textInput3 = std::make_shared<TextInputWidget>("text_input_3");
  textContainer3->setWidget(textInput3);
  textInputColumn->addChild(textContainer3);

  r->addChild(textInputColumn);

  grid->addChild(r);

  auto bigColumn = std::make_shared<ColumnLayout>("big column" + index, 0.80, 0.60, 0.0, 0.0, 0.1, 0.05);
  auto l2 = std::make_shared<LeafLayout>("big leaf" + index, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0);

  auto screenWidget = std::make_shared<ScreenWidget>("screen");
  l2->setWidget(screenWidget);
  bigColumn->addChild(l2);

  Rect tempRect = {50.0, 50.0, -1.0, -1.0};
  auto screenObject1 = std::make_shared<ScreenObject>("enemy", "assets/images/enemy.png", tempRect);
  tempRect = {200.0, 200.0, -1.0, -1.0};
  auto screenObject2 = std::make_shared<ScreenObject>("player", "assets/images/player.png", tempRect);

  screenWidget->insertObject(screenObject1);
  screenWidget->insertObject(screenObject2);

  grid->addChild(bigColumn);

  auto bigColumn2 = std::make_shared<ColumnLayout>("big column 2" + index, 0.80, 0.60, 0.05, 0.05, 0.1, 0.05);
  auto l3 = std::make_shared<LeafLayout>("big leaf 2" + index, 0.90, 0.90, 0.0, 0.0, 0.0, 0.0);

  auto img2 = std::make_shared<ImageWidget>(imageSlug + "2", "assets/images/" + imageSlug + ".png");
  l3->setWidget(img2);
  bigColumn2->addChild(l3);

  grid->addChild(bigColumn2);
}
