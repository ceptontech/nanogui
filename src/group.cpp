#include <nanogui/group.h>

NAMESPACE_BEGIN(nanogui)

void Group::removeWidget(Widget *widget) {
  mWidgets.erase(std::remove(mWidgets.begin(), mWidgets.end(), widget),
                 mWidgets.end());
}

bool Group::visible() const {
  for (const auto &w : mWidgets) {
    if (!w->visible()) return false;
  }
  return true;
}

bool Group::visibleRecursive() const {
  for (const auto &w : mWidgets) {
    if (!w->visibleRecursive()) return false;
  }
  return true;
}

void Group::setVisible(bool val) {
  for (const auto &w : mWidgets) {
    w->setVisible(val);
  }
}

NAMESPACE_END(nanogui)
