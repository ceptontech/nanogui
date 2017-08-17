#pragma once

#include <set>

#include <nanogui/widget.h>

NAMESPACE_BEGIN(nanogui)

class Widget;

class NANOGUI_EXPORT Group : public Object {
 public:
  void addWidget(Widget *widget) { mWidgets.insert(widget); }
  void removeWidget(Widget *widget) {mWidgets.erase(widget); }

  const std::set<Widget *> &widgets() const { return mWidgets; }
  int widgetCount() const { return mWidgets.size(); }

  bool visible() const;
  bool visibleRecursive() const;
  void setVisible(bool val);

  void destroy();

 private:
  friend Widget;

  std::set<Widget *> mWidgets;
};

NAMESPACE_END(nanogui)
