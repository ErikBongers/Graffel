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
        view1->drawAll();
    if (view2)
        view2->drawAll();
    }

void View::splitView(View* secondView, Location loc)
    {
    View* clone = new View(*this);
    clone->parent = this;
    clone->view = this; //TODO: is both parent and view needed?
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

    SDL_WindowEvent event;
    event.type = SDL_EventType::SDL_WINDOWEVENT;
    _resizeContent(event);
    }

void View::_resizeContent(SDL_WindowEvent& event)
    {
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
        view1->_resizeContent(event);
        view2->_resizeContent(event);
        }
    oldSize = rect;
    getWindow()->setInvalid();
    }
