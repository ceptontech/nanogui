#include <nanogui/group.h>

NAMESPACE_BEGIN(nanogui)

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
  for (auto &w : mWidgets) {
    w->setVisible(val);
  }
}

NAMESPACE_END(nanogui)
