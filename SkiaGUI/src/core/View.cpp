#include "View.h"

View::SizePrefs View::getSizePrefs()
    {
    if(!view1 && !view2)
        return defaultSizePrefs;
    auto getPrefMinMax = [this](int idxDir) ->MinMax {
        MinMax prefs1 = view1->getSizePrefs()[idxDir];
        MinMax prefs2 = view2->getSizePrefs()[idxDir];
        MinMax buildMinMax;
        buildMinMax.min = prefs1.min + prefs2.min;
        buildMinMax.max = prefs1.max + prefs2.max;
        buildMinMax.prefSize = std::min(prefs1.prefSize, prefs2.prefSize);

        return buildMinMax;
        };
    SizePrefs buildPrefs;
    buildPrefs.mmWidth = getPrefMinMax(SizePrefs::WIDHT);
    buildPrefs.mmHeight = getPrefMinMax(SizePrefs::HEIGHT);
    return buildPrefs;
    }

void View::_drawMe()
    {
    if (!view1)
        {
        SkPaint paint({ 0,0,0, 1 });
        paint.setStyle(SkPaint::Style::kStroke_Style);
        SkRect rBkg = rect.makeOffset(-rect.fLeft, -rect.fTop);
        Canvas().drawRect(rBkg, paint);
        }
    if (view1)
        {
        view1->drawAll();
        view2->drawAll();
        SkPaint paint({ 1, 0, 0, 1 });
        Canvas().drawRect(rectGap, paint);
        }
    else if (area)
        area->drawAll();
    }

void View::trickleResizeEvent()
    {
    if (view1)
        view1->trickleResizeEvent();
    if (view2)
        view2->trickleResizeEvent();
    else if (area)
        area->trickleResizeEvent();
    }
bool View::trickleMouseMoveEvent(SDL_MouseMotionEvent& event)
    {
    if (!hitTest(event.x, event.y))
        return false;
    if (!area && hitTest(rectGap, event.x, event.y))
        window->setCursor(splitDirection == Direction::LEFT_RIGHT ? SDL_SYSTEM_CURSOR_SIZEWE : SDL_SYSTEM_CURSOR_SIZENS);
    else
        window->setCursor();

    bool res = false;
    if (view1)
        res |= view1->trickleMouseMoveEvent(event);
    if (view2)
        res |= view2->trickleMouseMoveEvent(event);
    else if (area)
        res |= area->trickleMouseMoveEvent(event);
    return res;
    }
bool View::trickleMouseUpEvent(SDL_MouseButtonEvent& event)
    {
    if (!hitTest(event.x, event.y))
        return false;
    bool res = false;
    if (view1)
        res |= view1->trickleMouseUpEvent(event);
    if (view2)
        res |= view2->trickleMouseUpEvent(event);
    else if (area)
        res |= area->trickleMouseUpEvent(event);
    return res;
    }
bool View::trickleMouseDownEvent(SDL_MouseButtonEvent& event)
    {
    if (!hitTest(event.x, event.y))
        return false;
    bool res = false;
    if (view1)
        res |= view1->trickleMouseDownEvent(event);
    if (view2)
        res |= view2->trickleMouseDownEvent(event);
    else if (area)
        res |= area->trickleMouseDownEvent(event);
    return res;
    }
bool View::trickleMouseWheelEvent(SDL_MouseWheelEvent_EX& event)
    {
    if (!hitTest(event.x, event.y))
        return false;
    bool res = false;
    if (view1)
        res |= view1->trickleMouseWheelEvent(event);
    if (view2)
        res |= view2->trickleMouseWheelEvent(event);
    else if (area)
        res |= area->trickleMouseWheelEvent(event);
    return res;
    }
void View::trickleIdle()
    {
    if (view1)
        view1->trickleIdle();
    if (view2)
        view2->trickleIdle();
    else if (area)
        area->trickleIdle();
    }
void View::trickleKeyDown(SDL_KeyboardEvent& event)
    {
    if (view1)
        view1->trickleKeyDown(event);
    if (view2)
        view2->trickleKeyDown(event);
    else if (area)
        area->trickleKeyDown(event);

    }
void View::trickleTextEvent(SDL_TextInputEvent& event)
    {
    if (view1)
        view1->trickleTextEvent(event);
    if (view2)
        view2->trickleTextEvent(event);
    else if (area)
        area->trickleTextEvent(event);
     }

 void View::setContent(UIArea* area) 
    { 
    if (myCreatedView)
        delete myCreatedView;
    view1 = view2 = myCreatedView = nullptr;
    this->area = area;
    area->parent = this;
    _resizeContent();
    }

void View::splitView(View* secondView, Location loc)
    {
    View* clone = new View(*this);
    clone->parent = this;
    clone->view = this; //TODO: is both parent and view needed?
    if (clone->area)
        {
        clone->area->parent = clone;
        this->area = nullptr;;
        }
    myCreatedView = clone;
    if (loc == Location::BELOW || loc == Location::RIGHT)
        {
        view1 = clone;
        view2 = secondView;
        view2->parent = view2->view = this;
        }
    else
        {
        view2 = clone;
        view1 = secondView;
        view1->parent = view1->view = this;
        }
    if (loc == Location::ABOVE || loc == Location::BELOW)
        splitDirection = Direction::ABOVE_BELOW;
    else
        splitDirection = Direction::LEFT_RIGHT;

    _resizeContent();
    }

void View::_resizeContent()
    {
    if (area)
        {
        area->rect = rect.makeOffset(-rect.fLeft, -rect.fTop);
        area->trickleResizeEvent();
        return;
        }
    SkScalar halfGap = mindTheGap / 2;
    if (view1 && view2)
        {
        if (splitPoint == -1)
            {
            if (splitDirection == Direction::LEFT_RIGHT)
                splitPoint = rect.width() / 2;
            else
                splitPoint = rect.height() / 2;
            }

        //recalculate splitPoint
        SizePrefs prefs1 = view1->getSizePrefs();
        SizePrefs prefs2 = view2->getSizePrefs();
        SkScalar mySize = splitDirection == Direction::LEFT_RIGHT ? rect.width() : rect.height();
        auto idxDir = (splitDirection == Direction::LEFT_RIGHT ? SizePrefs::WIDHT : SizePrefs::HEIGHT);
        if ((prefs1[idxDir].prefSize == PrefSize::PROPORTIONAL && prefs2[idxDir].prefSize == PrefSize::PROPORTIONAL)
            || (prefs1[idxDir].prefSize == PrefSize::DONTCARE && prefs2[idxDir].prefSize == PrefSize::DONTCARE))
            {
            if (!oldSize.isEmpty())
                {
                if(splitDirection == Direction::LEFT_RIGHT)
                    splitPoint = splitPoint * mySize / oldSize.width();
                else
                    splitPoint = splitPoint * mySize / oldSize.height();
                }
            }
        else if (prefs1[idxDir].prefSize == PrefSize::FIXED)
            splitPoint = prefs1[idxDir].min;
        else if (prefs2[idxDir].prefSize == PrefSize::FIXED)
            splitPoint = (splitDirection == Direction::LEFT_RIGHT ? rect.width() : rect.height()) - prefs2[idxDir].min;
        else if (prefs1[idxDir].prefSize == PrefSize::PROPORTIONAL)
            splitPoint = prefs1[idxDir].min;
        else if (prefs1[idxDir].prefSize == PrefSize::PROPORTIONAL)
            splitPoint = (splitDirection == Direction::LEFT_RIGHT ? rect.width() : rect.height()) - prefs2[idxDir].min;
        splitPoint = std::clamp(splitPoint, prefs1[idxDir].min, prefs1[idxDir].max);
        splitPoint = std::clamp(splitPoint, mySize - prefs2[idxDir].max, mySize - prefs2[idxDir].min);
    
        //resizing sub-views
        if (splitDirection == Direction::LEFT_RIGHT)
            {
            view1->rect = SkRect::MakeXYWH(0, 0, splitPoint - halfGap, rect.height());
            view2->rect = SkRect::MakeXYWH(splitPoint + halfGap, 0, rect.width() - splitPoint - halfGap, rect.height());
            }
        else
            {
            view1->rect = SkRect::MakeXYWH(0, 0, rect.width(), splitPoint - halfGap);
            view2->rect = SkRect::MakeXYWH(0, splitPoint + halfGap, rect.width(), rect.height() - splitPoint - halfGap);
            }
        view1->_resizeContent();
        view2->_resizeContent();
        }
    oldSize = rect;
    if(splitDirection == Direction::LEFT_RIGHT)
        rectGap = SkRect::MakeXYWH(splitPoint - halfGap, 0, mindTheGap, rect.height());
    else
        rectGap = SkRect::MakeXYWH(0, splitPoint - halfGap, rect.width(), mindTheGap);

    if(getWindow())
        window->setInvalid();
    }
