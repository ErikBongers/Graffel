#include "pch.h"
#include "core/SDLSkiaWindow.h"
#include "controls/Button.h"
#include "controls/InfiniteCanvas.h"
#include "texteditor/editor.h"

class ImageButton : public Button
    {
    public:
        ImageButton() { highlightColor = SkColorSetARGB(20, 255, 255, 255); }
        SkScalar xOffset = 0, yOffset = 0;
        sk_sp<SkImage> img = nullptr;
        void drawMe(SDLSkiaWindow& window) override
            {
            SkPaint paint;
            paint.setAntiAlias(true);
            paint.setAlpha(mouseOver ? 255 : 180);
            if(img)
                window.Canvas().drawImage(img, xOffset, yOffset, &paint);
            }
    };

class Bullet : public UIElement
    {
    public:
        SkColor color = SK_ColorRED;
    void drawMe(SDLSkiaWindow& window) override
        {
        SkPaint paint;
        paint.setColor(color);
        SkScalar radius = rect.width() / 2;
        window.Canvas().drawCircle(SkPoint::Make(radius, radius), radius, paint);
        }
    };

class Curve : public UIElement
    {
    public:
    Bullet* p1;
    Bullet* p2;
    Bullet* p3;
    Bullet* p4;
    SkColor color;
    SkRect bounds;
    SkPath outline;

    protected:
    void drawMe(SDLSkiaWindow& window) override
        {
        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setStyle(SkPaint::kStroke_Style);
        paint.setColor(SkColorSetARGB(230, 255, 255, 255));
        SkScalar radius = rect.width() / 2;
        SkPoint pp1 = SkPoint::Make(p1->rect.fLeft, p1->rect.fTop);
        SkPoint pp2 = SkPoint::Make(p2->rect.fLeft, p2->rect.fTop);
        SkPoint pp3 = SkPoint::Make(p3->rect.fLeft, p3->rect.fTop);
        SkPoint pp4 = SkPoint::Make(p4->rect.fLeft, p4->rect.fTop);
        window.Canvas().drawLine(pp1, pp2, paint);
        window.Canvas().drawLine(pp3, pp4, paint);
        SkPath path;
        path.moveTo(pp1)
            .cubicTo(pp2, pp3, pp4);
        outline.reset();
        paint.setStrokeWidth(5);
        paint.getFillPath(path, &outline);
        paint.setStrokeWidth(0);
        paint.setColor(color);
        window.Canvas().drawPath(outline, paint);
        paint.setColor(SkColorSetARGB(200, 255, 255, 255));
        bounds = path.getBounds();
        window.Canvas().drawRect(bounds, paint);
        };

    };

class TestWindowClient : public WindowClient
    {
    private:
        UIElement full;
        UIElement toolbar;
        Button button1;
        Button button2;
        InfiniteCanvas infiniteCanvas;
        UIElement square1;
        Bullet bullet;

        Bullet blueChild;

        Curve curve1;
        Bullet p1;
        Bullet p2;
        Bullet p3;
        Bullet p4;

        ImageButton imgButton;
        ImageButton imgButton2;
        ImageButton imgButton3;
            
        SkPlainTextEditor::Editor editor1;

    public:
        void initialize(SDLSkiaWindow& window) override;
        void update(SDLSkiaWindow& window) override;
        void draw(SDLSkiaWindow& window) override;
        void mouseMoved(SDL_MouseMotionEvent& event, SDLSkiaWindow& window) override;
        void mouseDown(SDL_MouseButtonEvent& event, SDLSkiaWindow& window) override;
        void mouseUp(SDL_MouseButtonEvent& event, SDLSkiaWindow& window) override;
        void mouseWheel(SDL_MouseWheelEvent& event, SDLSkiaWindow& window) override;
        void resize(SDL_WindowEvent& event, SDLSkiaWindow& window) override;
    };