#pragma once
#include "../pch.h"
#include "UIArea.h"


class View : public UIArea
    {
    private:
        View* myCreatedView = nullptr;
        SkRect oldSize;
    protected:
        UIArea* area = nullptr; //always fills the view.
        //OR
        View* view1 = nullptr; // can provide a min and max size.
        View* view2 = nullptr;

        enum class Direction { LEFT_RIGHT, ABOVE_BELOW};
        Direction splitDirection = Direction::LEFT_RIGHT;
        SkScalar splitPoint = -1;
        void _resizeContent() override;
        SkScalar mindTheGap = 4;
    public:
        enum class PrefSize { PROPORTIONAL = 0, DONTCARE = -1, FIXED = 1 };
        struct MinMax {
            SkScalar min = 0;
            SkScalar max = SK_ScalarMax/2; // half, to avoid overflow.
            PrefSize prefSize = PrefSize::PROPORTIONAL;
            };
        struct SizePrefs {
            static const int WIDHT = 0;
            static const int HEIGHT = 1;
            MinMax mmWidth;
            MinMax mmHeight;
            MinMax& operator [](int i) { return i ? mmHeight : mmWidth; }
            };
        SizePrefs sizePrefs;
        SizePrefs getSizePrefs();
        SizePrefs defaultSizePrefs;
        void _drawMe() override;
        virtual void trickleResizeEvent() override;
        virtual bool trickleMouseMoveEvent(SDL_MouseMotionEvent& event) override;
        virtual bool trickleMouseUpEvent(SDL_MouseButtonEvent& event) override;
        virtual bool trickleMouseDownEvent(SDL_MouseButtonEvent& event) override;
        virtual void trickleIdle() override;
        virtual void trickleKeyDown(SDL_KeyboardEvent& event) override;
        virtual void trickleTextEvent(SDL_TextInputEvent& event) override;

        void setContent(UIArea* area);
        enum class Location { BELOW, ABOVE, LEFT, RIGHT };
        void splitView(View* secondView, Location loc);
        friend class SDLSkiaWindow;
        friend class WindowClient;
        ~View() { if (myCreatedView) delete myCreatedView; }
    };