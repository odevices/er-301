#pragma once

#include <od/extras/ReferenceCounted.h>
#include <od/graphics/Cursor.h>
#include <od/graphics/FrameBuffer.h>
#include <od/graphics/text/Utils.h>
#include <vector>

namespace od
{

  enum Visibility
  {
    hiddenState,
    visibleState
  };

  class GraphicContext;
  class Graphic : public ReferenceCounted
  {
  public:
    Graphic(int left, int bottom, int width, int height);
    virtual ~Graphic();

    // do not call these on stack allocated widgets
    // (unless you attach/own them and never release/disown)
    void addChild(Graphic *graphic);
    void addChildOnce(Graphic *graphic);
    void removeChild(Graphic *graphic);
    void clear();

    bool contains(Graphic *graphic);
    void expandToPly(int padding = 0);
    bool intersectsWithButton(int button);

    void setForegroundColor(Color color);
    void setBackgroundColor(Color color);
    void setBorderColor(Color color);
    void setBorder(int thickness);
    void setCornerRadius(int bl, int tl, int tr, int br);
    void setOpaque(bool value);

    virtual void setPosition(int left, int bottom);
    virtual void setSize(int width, int height, bool updateSaved = true);
    void setCenter(int x, int y);
    void hide();
    void show();
    bool isHidden();
    bool hasParent();

    int mLeft, mBottom, mWidth, mHeight;

#ifndef SWIGLUA
    virtual void draw(FrameBuffer &fb);
    virtual void notifyVisible();
    virtual void notifyHidden();
    virtual void notifyContentsChanged();

    void updateWorldCoordinates();
    Point getCenter();
    Size getSize();
    Rect getRect();
    Rect getWorldRect();
    Rect getBoundingRect();

    bool intersects(Graphic &w);
    bool intersects(Rect &r);
    bool visible(FrameBuffer &fb);
    bool notVisible(FrameBuffer &fb);

    int mWorldLeft, mWorldBottom;
    Color mForeground = WHITE;
    Color mBackground = BLACK;
    Color mBorderColor = WHITE;
    int mBorder = 0;
    int mBorderRadiusBL = 0;
    int mBorderRadiusTL = 0;
    int mBorderRadiusBR = 0;
    int mBorderRadiusTR = 0;
    bool mHidden = false;
    bool mOpaque = false;
    CursorState mCursorState;

    // Global tween parameter that oscillates between -1 and 1.
    // All Graphic instances should use this shared tween parameter to sync animations.
    static void updateTween();
    static void resetTween();
    static float sTween;
    static float sTweenStep;
#endif

  protected:
    friend GraphicContext;
    Visibility mVisibility = hiddenState;
    std::vector<Graphic *> mChildren;
    Graphic *mpParent;

  private:
    int mSavedWidth, mSavedHeight;
    bool mSizeSaved = false;

    void saveSize();
    void restoreSize();
  };

  static inline bool intersects(Graphic &w1, Graphic &w2)
  {
    return !(w2.mLeft > w1.mLeft + w1.mWidth || w2.mLeft + w2.mWidth < w1.mLeft || w2.mBottom + w2.mHeight < w1.mBottom || w2.mBottom > w1.mBottom + w1.mHeight);
  }

  extern Graphic NullGraphic;

} /* namespace od */
