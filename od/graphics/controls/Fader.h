#pragma once

#include <od/objects/measurement/LoudnessProbe.h>
#include <od/graphics/Graphic.h>
#include <od/objects/Parameter.h>
#include <od/graphics/controls/SoftButton.h>
#include <od/graphics/controls/Readout.h>

namespace od
{

  class DialMap;
  class Fader : public Graphic
  {
  public:
    Fader(int left, int bottom, int width, int height);
    virtual ~Fader();

#ifndef SWIGLUA
    virtual void draw(FrameBuffer &fb);
#endif

    void setParameter(Parameter *param);

    void setValueParameter(Parameter *param);
    Parameter *getValueParameter();

    void setTargetParameter(Parameter *param);
    Parameter *getTargetParameter();

    void setControlParameter(Parameter *param);
    Parameter *getControlParameter();

    void setRangeObject(Object *rangeObject);
    Object *getRangeObject();

    // metering
    void watchOutlets(Outlet *left, Outlet *right);

    // visual attributes
    void setLabel(const std::string &text);
    const std::string &getLabel();
    void setAttributes(ReadoutUnits units, DialMap *map, Scaling scaling = linearScaling);
    void setScaling(Scaling scaling);

    // readout encapsulation
    void setMap(DialMap *map);
    void setUnits(ReadoutUnits units);
    void setPrecision(int p);
    void setTextBelow(float value, const std::string &text);
    void setTextAbove(float value, const std::string &text);
    void save();
    void restore();
    void zero();
    void encoder(int change, bool shifted, bool fine);

    void grayed();
    void ungrayed();

    void highlightTarget();
    void highlightValue();

  protected:
    Parameter *mpValueParameter = 0;
    Parameter *mpTargetParameter = 0;
    Parameter *mpControlParameter = 0;
    Readout mReadout;
    Scaling mScaling = linearScaling;
    float mScale = 0.5f;
    float mScaledMinimum = 0.0f;
    float mScaledZero = 0.0f;
    int mFaderX = 0;
    bool mHighlightTarget = true;

    Object *mpRangeObject = 0;
    Parameter *mpMinimum = 0;
    Parameter *mpMaximum = 0;

    LoudnessProbe mLeftProbe;
    LoudnessProbe mRightProbe;
    Outlet *mpLeftOutlet = 0;
    Outlet *mpRightOutlet = 0;
    float mLeftDecibels = 0;
    float mRightDecibels = 0;

    SoftButton mNameLabel{" ---- ", 1};

    void calculateLeftMeter();
    void calculateRightMeter();

    void connectProbes();
    void disconnectProbes();
    void clearOutlets();
    std::string getLeftWatchName();
    std::string getRightWatchName();

    virtual void notifyHidden();
    virtual void notifyVisible();

    void commitChanges(bool force);

    // convert the given value according to the current scaling
    float scale(float x);
    void refreshScaling();
  };

} /* namespace od */
