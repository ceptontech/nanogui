/*
    src/widget.cpp -- Base class of all widgets

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/widget.h>
#include <nanogui/layout.h>
#include <nanogui/theme.h>
#include <nanogui/window.h>
#include <nanogui/opengl.h>
#include <nanogui/group.h>
#include <nanogui/screen.h>
#include <nanogui/serializer/core.h>

NAMESPACE_BEGIN(nanogui)

Widget::Widget(Widget *parent)
    : mParent(nullptr), mTheme(nullptr), mLayout(nullptr),
      mPos(Vector2i::Zero()), mSize(Vector2i::Zero()),
      mFixedSize(Vector2i::Zero()), mVisible(true), mEnabled(true),
      mFocused(false), mMouseFocus(false), mTooltip(""), mFontSize(-1.0f),
      mCursor(Cursor::Arrow) {
    if (parent)
        parent->addChild(this);
}

Widget::~Widget() {}

void Widget::dispose() {
  disposeImpl();

  if (mParent) mParent->removeChild(this);
}

void Widget::disposeImpl() {
  Widget *parent = mParent;
  while(parent) {
    parent->disposeWidget(this);
    parent = parent->parent();
  }

  for (auto child : mChildren) {
    child->disposeImpl();
  }
}

void Widget::setParent(Widget *parent) {
  if (mParent == parent) return;
  if (mParent) {
    mParent->removeChild(this);
  }
  mParent = parent;
}

bool Widget::hasGroup(Group *group) const {
  return (mGroups.count(group) != 0);
}

void Widget::addGroup(Group *group) {
  if (!group) return;
  mGroups.insert(group);
}

void Widget::removeGroup(Group *group) {
  mGroups.erase(group);
}

void Widget::setTheme(Theme *theme) {
    if (mTheme.get() == theme)
        return;
    mTheme = theme;
    for (auto child : mChildren)
        child->setTheme(theme);
}

int Widget::fontSize() const {
    return (mFontSize < 0 && mTheme) ? mTheme->mStandardFontSize : mFontSize;
}

Vector2i Widget::preferredSize(NVGcontext *ctx) const {
    if (mLayout)
        return mLayout->preferredSize(ctx, this);
    else
        return mSize;
}

void Widget::refresh() {
  for (auto c : mChildren) {
    c->refresh();
  }
}

void Widget::performLayout(NVGcontext *ctx) {
    if (mLayout) {
        mLayout->performLayout(ctx, this);
    } else {
        for (auto c : mChildren) {
            Vector2i pref = c->preferredSize(ctx), fix = c->fixedSize();
            c->setSize(Vector2i(
                fix[0] ? fix[0] : pref[0],
                fix[1] ? fix[1] : pref[1]
            ));
            c->performLayout(ctx);
        }
    }
}

Widget *Widget::findWidget(const Vector2i &p) {
    for (auto it = mChildren.rbegin(); it != mChildren.rend(); ++it) {
        Widget *child = *it;
        if (child->visible() && child->contains(p - mPos))
            return child->findWidget(p - mPos);
    }
    return contains(p) ? this : nullptr;
}

bool Widget::mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) {
    for (auto it = mChildren.rbegin(); it != mChildren.rend(); ++it) {
        Widget *child = *it;
        if (child->visible() && child->contains(p - mPos) &&
            child->mouseButtonEvent(p - mPos, button, down, modifiers))
            return true;
    }
    if (button == GLFW_MOUSE_BUTTON_1 && down && !mFocused)
        requestFocus();
    return false;
}

bool Widget::mouseMotionEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) {
    for (auto it = mChildren.rbegin(); it != mChildren.rend(); ++it) {
        Widget *child = *it;
        if (!child->visible())
            continue;
        bool contained = child->contains(p - mPos), prevContained = child->contains(p - mPos - rel);
        if (contained != prevContained)
            child->mouseEnterEvent(p, contained);
        if ((contained || prevContained) &&
            child->mouseMotionEvent(p - mPos, rel, button, modifiers))
            return true;
    }
    return false;
}

bool Widget::scrollEvent(const Vector2i &p, const Vector2f &rel) {
    for (auto it = mChildren.rbegin(); it != mChildren.rend(); ++it) {
        Widget *child = *it;
        if (!child->visible())
            continue;
        if (child->contains(p - mPos) && child->scrollEvent(p - mPos, rel))
            return true;
    }
    return false;
}

bool Widget::mouseDragEvent(const Vector2i &, const Vector2i &, int, int) {
    return false;
}

bool Widget::mouseEnterEvent(const Vector2i &, bool enter) {
    mMouseFocus = enter;
    return false;
}

bool Widget::focusEvent(bool focused) {
    mFocused = focused;
    return false;
}

bool Widget::keyboardEvent(int, int, int, int) {
    return false;
}

bool Widget::keyboardCharacterEvent(unsigned int) {
    return false;
}

bool Widget::visible() const {
  for (const auto &group : mGroups) {
    if (!group->visible()) return false;
  }
  return mVisible;
}

bool Widget::visibleRecursive() const {
    const Widget *widget = this;
    while (widget) {
        if (!widget->visible()) return false;
        widget = widget->parent();
    }
    return true;
}

bool Widget::hasChild(const Widget *widget) const {
  return (std::find(mChildren.begin(), mChildren.end(), widget) != mChildren.end());
}

void Widget::addChild(int index, Widget * widget) {
    assert(index <= childCount());
    if (!hasChild(widget)) {
      widget->incRef();
      widget->setParent(this);
      widget->setTheme(mTheme);
    }
    mChildren.insert(mChildren.begin() + index, widget);
}

void Widget::addChild(Widget * widget) {
    addChild(childCount(), widget);
}

void Widget::removeChild(Widget *widget) {
    if (!hasChild(widget)) return;
    mChildren.erase(std::remove(mChildren.begin(), mChildren.end(), widget), mChildren.end());
    widget->mParent = nullptr;
    widget->decRef();
}

void Widget::removeChild(int index) {
    Widget *widget = mChildren.at(index);
    mChildren.erase(mChildren.begin() + index);
    if (!hasChild(widget)) {
      widget->mParent = nullptr;
      widget->decRef();
    }
}

void Widget::removeChildren() {
  while (childCount() > 0) {
    removeChild(childCount() - 1);
  }
}

int Widget::childIndex(Widget *widget) const {
    auto it = std::find(mChildren.begin(), mChildren.end(), widget);
    if (it == mChildren.end())
        return -1;
    return it - mChildren.begin();
}

Screen *Widget::screen() {
    Widget *widget = this;
    while (true) {
        if (!widget)
            throw std::runtime_error(
                "Widget:internal error (could not find parent screen)");
        Screen *screen = dynamic_cast<Screen *>(widget);
        if (screen)
            return screen;
        widget = widget->parent();
    }
}

const Screen *Widget::screen() const {
    const Widget *widget = this;
    while (true) {
        if (!widget)
            throw std::runtime_error(
                "Widget:internal error (could not find parent screen)");
        const Screen *screen = dynamic_cast<const Screen *>(widget);
        if (screen)
            return screen;
        widget = widget->parent();
    }
}

Window *Widget::window() {
    Widget *widget = this;
    while (true) {
        if (!widget)
            throw std::runtime_error(
                "Widget:internal error (could not find parent window)");
        Window *window = dynamic_cast<Window *>(widget);
        if (window)
            return window;
        widget = widget->parent();
    }
}

const Window *Widget::window() const {
    const Widget *widget = this;
    while (true) {
        if (!widget)
            throw std::runtime_error(
                "Widget:internal error (could not find parent window)");
        const Window *window = dynamic_cast<const Window *>(widget);
        if (window)
            return window;
        widget = widget->parent();
    }
}

void Widget::requestFocus() {
    Widget *widget = this;
    while (widget->parent())
        widget = widget->parent();
    ((Screen *) widget)->updateFocus(this);
}

void Widget::draw(NVGcontext *ctx) {
    #if NANOGUI_SHOW_WIDGET_BOUNDS
        nvgStrokeWidth(ctx, 1.0f);
        nvgBeginPath(ctx);
        nvgRect(ctx, mPos.x() - 0.5f, mPos.y() - 0.5f, mSize.x() + 1, mSize.y() + 1);
        nvgStrokeColor(ctx, nvgRGBA(255, 0, 0, 255));
        nvgStroke(ctx);
    #endif

    if (mChildren.empty())
        return;

    nvgSave(ctx);
    nvgTranslate(ctx, mPos.x(), mPos.y());
    for (auto child : mChildren) {
        if (child->visible()) {
            nvgSave(ctx);
            nvgIntersectScissor(ctx, child->mPos.x(), child->mPos.y(), child->mSize.x(), child->mSize.y());
            child->draw(ctx);
            nvgRestore(ctx);
        }
    }
    nvgRestore(ctx);
}

void Widget::save(Serializer &s) const {
    s.set("position", mPos);
    s.set("size", mSize);
    s.set("fixedSize", mFixedSize);
    s.set("visible", mVisible);
    s.set("enabled", mEnabled);
    s.set("focused", mFocused);
    s.set("tooltip", mTooltip);
    s.set("fontSize", mFontSize);
    s.set("cursor", (int) mCursor);
}

bool Widget::load(Serializer &s) {
    if (!s.get("position", mPos)) return false;
    if (!s.get("size", mSize)) return false;
    if (!s.get("fixedSize", mFixedSize)) return false;
    if (!s.get("visible", mVisible)) return false;
    if (!s.get("enabled", mEnabled)) return false;
    if (!s.get("focused", mFocused)) return false;
    if (!s.get("tooltip", mTooltip)) return false;
    if (!s.get("fontSize", mFontSize)) return false;
    if (!s.get("cursor", mCursor)) return false;
    return true;
}

NAMESPACE_END(nanogui)
