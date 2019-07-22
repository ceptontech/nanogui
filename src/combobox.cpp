/*
    src/combobox.cpp -- simple combo box widget based on a popup button

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/combobox.h>
#include <nanogui/layout.h>
#include <nanogui/serializer/core.h>
#include <cassert>

#include <iostream>

NAMESPACE_BEGIN(nanogui)

ComboBox::ComboBox(Widget *parent) : PopupButton(parent), mSelectedIndex(0) {}

ComboBox::ComboBox(Widget *parent, const std::vector<std::string> &items)
    : PopupButton(parent), mSelectedIndex(0) {
  setItems(items);
}

ComboBox::ComboBox(Widget *parent, const std::vector<std::string> &items,
                   const std::vector<std::string> &itemsShort)
    : PopupButton(parent), mSelectedIndex(0) {
  setItems(items, itemsShort);
}

void ComboBox::setSelectedIndex(int idx) {
  if (itemCount() == 0) return;
  if (idx >= itemCount()) idx = 0;
  const std::vector<Widget *> &children = popup()->children();
  if (mSelectedIndex < itemCount())
    ((Button *)children[mSelectedIndex])->setPushed(false);
  ((Button *)children[idx])->setPushed(true);
  mSelectedIndex = idx;
  setCaption(mItemsShort[idx]);
}

void ComboBox::setItems(const std::vector<std::string> &items,
                        const std::vector<std::string> &itemsShort,
                        const std::vector<std::string> &tooltips) {
  assert(itemsShort.empty() || (itemsShort.size() == items.size()));
  assert(tooltips.empty() || (tooltips.size() == items.size()));
  if (mSelectedIndex < 0 || mSelectedIndex >= (int)items.size())
    mSelectedIndex = 0;
  mItems = items;
  mItemsShort = (itemsShort.empty()) ? items : itemsShort;
  mPopup->removeChildren();
  mPopup->setLayout(new GroupLayout(10));
  int index = 0;
  for (int i = 0; i < items.size(); ++i) {
    const auto &str = items[i];
    Button *button = new Button(mPopup, str);
    button->setFlags(Button::RadioButton);
    button->setCallback([&, index] {
      mSelectedIndex = index;
      setCaption(mItemsShort[index]);
      setPushed(false);
      popup()->setVisible(false);
      if (mCallback) mCallback(index);
    });
    if (!tooltips.empty()) button->setTooltip(tooltips[i]);
    index++;
  }
  setSelectedIndex(mSelectedIndex);
}

bool ComboBox::scrollEvent(const Vector2i &p, const Vector2f &rel) {
  if (rel.y() < 0) {
    setSelectedIndex(std::min(mSelectedIndex + 1, (int)(items().size() - 1)));
    if (mCallback) mCallback(mSelectedIndex);
    return true;
  } else if (rel.y() > 0) {
    setSelectedIndex(std::max(mSelectedIndex - 1, 0));
    if (mCallback) mCallback(mSelectedIndex);
    return true;
  }
  return Widget::scrollEvent(p, rel);
}

void ComboBox::save(Serializer &s) const {
  Widget::save(s);
  s.set("items", mItems);
  s.set("itemsShort", mItemsShort);
  s.set("selectedIndex", mSelectedIndex);
}

bool ComboBox::load(Serializer &s) {
  if (!Widget::load(s)) return false;
  if (!s.get("items", mItems)) return false;
  if (!s.get("itemsShort", mItemsShort)) return false;
  if (!s.get("selectedIndex", mSelectedIndex)) return false;
  return true;
}

NAMESPACE_END(nanogui)
