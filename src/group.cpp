#include <nanogui/group.h>

NAMESPACE_BEGIN(nanogui)

void Group::addWidget(Widget *widget) {
  mWidgets.insert(widget);
}

void Group::removeWidget(Widget *widget) {
  mWidgets.erase(widget);
}

void Group::addGroup(Group *group) {
  for (auto &w : group->widgets()) {
    addWidget(w);
  }
}

void Group::removeGroup(Group *group) {
  for (auto &w : group->widgets()) {
    removeWidget(w);
  }
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
  for (auto &w : mWidgets) {
    w->setVisible(val);
  }
}

NAMESPACE_END(nanogui)
