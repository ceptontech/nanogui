#pragma once

#include <set>

#include <nanogui/widget.h>

NAMESPACE_BEGIN(nanogui)

class Widget;

class NANOGUI_EXPORT Group : public Object {
 public:
  void addWidget(Widget *widget);
  void removeWidget(Widget *widget);

  void addGroup(Group *group);
  void removeGroup(Group *group);

  const std::set<Widget *> &widgets() const { return mWidgets; }
  size_t widgetCount() const { return mWidgets.size(); }

  bool visible() const;
  bool visibleRecursive() const;
  void setVisible(bool val);

  void destroy();

 private:
  friend Widget;

  std::set<Widget *> mWidgets;
};

NAMESPACE_END(nanogui)
