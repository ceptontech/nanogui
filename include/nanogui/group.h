#pragma once

#include <nanogui/widget.h>
#include <vector>

NAMESPACE_BEGIN(nanogui)

class Widget;

class NANOGUI_EXPORT Group : public Object {
 public:
  void addWidget(Widget *widget) { mWidgets.push_back(widget); }
  void removeWidget(Widget *widget);

  const std::vector<Widget *> &widgets() const { return mWidgets; }
  int widgetCount() const { return mWidgets.size(); }

  bool visible() const;
  bool visibleRecursive() const;
  void setVisible(bool val);

  void destroy();

 private:
  friend Widget;

  std::vector<Widget *> mWidgets;
};

NAMESPACE_END(nanogui)
