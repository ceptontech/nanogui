/*
    src/hscrollpanel.cpp -- Adds a horizontal scrollbar around a widget
    that is too big to fit into a certain area

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/hscrollpanel.h>
#include <nanogui/theme.h>
#include <nanogui/opengl.h>
#include <nanogui/serializer/core.h>

NAMESPACE_BEGIN(nanogui)

HScrollPanel::HScrollPanel(Widget *parent)
    : Widget(parent), mChildPreferredWidth(0), mScroll(0.0f), mUpdateLayout(false) { }

void HScrollPanel::performLayout(NVGcontext *ctx) {
    Widget::performLayout(ctx);

    if (mChildren.empty())
        return;
    if (mChildren.size() > 1)
        throw std::runtime_error("HScrollPanel should have one child.");

    Widget *child = mChildren[0];
    mChildPreferredWidth = child->preferredSize(ctx).x();

    if (mChildPreferredWidth > mSize.x()) {
        child->setPosition(Vector2i(-mScroll * (mChildPreferredWidth - mSize.x()), 0));
        child->setSize(Vector2i(mChildPreferredWidth, mSize.y() - 12));
    }
    else {
        child->setPosition(Vector2i::Zero());
        child->setSize(mSize - Vector2i(0, 12));
        mScroll = 0;
    }
    child->performLayout(ctx);
}

Vector2i HScrollPanel::preferredSize(NVGcontext *ctx) const {
    if (mChildren.empty())
        return Vector2i::Zero();
    return mChildren[0]->preferredSize(ctx) + Vector2i(0, 12);
}

bool HScrollPanel::mouseDragEvent(const Vector2i &p, const Vector2i &rel,
    int button, int modifiers) {
    if (!mChildren.empty() && mChildPreferredWidth > mSize.x()) {
        float scrollw = width() *
            std::min(1.0f, width() / (float)mChildPreferredWidth);

        mScroll = std::max((float) 0.0f, std::min((float) 1.0f,
            mScroll + rel.x() / (float)(mSize.x() - 8 - scrollw)));
        mUpdateLayout = true;
        return true;
    }
    else {
        return Widget::mouseDragEvent(p, rel, button, modifiers);
    }
}

bool HScrollPanel::scrollEvent(const Vector2i &p, const Vector2f &rel) {
    if (!mChildren.empty() && mChildPreferredWidth > mSize.x()) {
        float scrollAmount = rel.x() * (mSize.x() / 20.0f);
        float scrollw = width() *
            std::min(1.0f, width() / (float)mChildPreferredWidth);

        mScroll = std::max((float) 0.0f, std::min((float) 1.0f,
            mScroll - scrollAmount / (float)(mSize.x() - 8 - scrollw)));
        mUpdateLayout = true;
        return true;
    }
    else {
        return Widget::scrollEvent(p, rel);
    }
}

void HScrollPanel::draw(NVGcontext *ctx) {
    if (mChildren.empty())
        return;
    Widget *child = mChildren[0];
    child->setPosition(Vector2i(-mScroll * (mChildPreferredWidth - mSize.x()), 0));
    mChildPreferredWidth = child->preferredSize(ctx).x();
    float scrollw = width() *
        std::min(1.0f, width() / (float)mChildPreferredWidth);

    if (mUpdateLayout)
        child->performLayout(ctx);

    nvgSave(ctx);
    nvgTranslate(ctx, mPos.x(), mPos.y());
    nvgIntersectScissor(ctx, 0, 0, mSize.x(), mSize.y());
    if (child->visible())
        child->draw(ctx);
    nvgRestore(ctx);

    if (mChildPreferredWidth <= mSize.x())
        return;

    NVGpaint paint = nvgBoxGradient(
        ctx, mPos.x() + 4 + 1, mPos.y() + mSize.y() - 12 + 1,
        mSize.x() - 8, 8, 3, 4, Color(0, 32), Color(0, 92));
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos.x() + 4, mPos.y() + mSize.y() - 12,
        mSize.x() - 8, 8, 3);
    nvgFillPaint(ctx, paint);
    nvgFill(ctx);

    paint = nvgBoxGradient(
        ctx, mPos.x() + 4 + (mSize.x() - 8 - scrollw) * mScroll - 1,
        mPos.y() + mSize.y() - 12 - 1, scrollw, 8,
        3, 4, Color(220, 100), Color(128, 100));

    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos.x() + 4 + 1 + (mSize.x() - 8 - scrollw) * mScroll,
        mPos.y() + mSize.y() - 12 + 1, scrollw - 2, 8 - 2, 2);
    nvgFillPaint(ctx, paint);
    nvgFill(ctx);
}

void HScrollPanel::save(Serializer &s) const {
    Widget::save(s);
    s.set("childPreferredWidth", mChildPreferredWidth);
    s.set("scroll", mScroll);
}

bool HScrollPanel::load(Serializer &s) {
    if (!Widget::load(s)) return false;
    if (!s.get("childPreferredWidth", mChildPreferredWidth)) return false;
    if (!s.get("scroll", mScroll)) return false;
    return true;
}

NAMESPACE_END(nanogui)
