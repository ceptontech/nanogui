#pragma once

#include <set>

#include <nanogui/widget.h>

NAMESPACE_BEGIN(nanogui)

class Widget;

class NANOGUI_EXPORT Group : public Object {
 public:
  bool visible() const { return mVisible; }
  void setVisible(bool val) { mVisible = val; }

  void destroy() { mIsDestroyed = true; }
  bool isDestroyed() const { return mIsDestroyed; }

 private:
  bool mVisible = true;
  bool mIsDestroyed = false;
};

NAMESPACE_END(nanogui)
