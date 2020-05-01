#pragma once
#include "..\..\pch.h"
#include "TextSpan.h"


#include "core/UIElement.h"
#include "EditorView.h"

class SkCanvas;
class SkShaper;

namespace SkEd {

class Editor : public UIElement
    {
    protected:
        void keyDown(SDL_KeyboardEvent& event) override;
        void textInput(SDL_TextInputEvent& event) override;
        void _mouseDown(SDL_MouseButtonEvent& event) override;
        void _mouseUp(SDL_MouseButtonEvent& event) override;
        void _resizeContent(SDL_WindowEvent& event) override;
        void drawMe() override;
        void onIdle() override;
        bool moveCursor(Movement m, bool shift);
        bool setCursor(TextPosition pos, bool shift);
        bool editMode = false;
        SkScalar scrollPos = 0;  // window pixel position in file
        SkScalar fMargin = 10;
    public:
        Editor();
        EditorView txt;
        SkScalar getHeight() const { return rect.height(); }
        void setFont(SkFont font) { txt.setFont(font); }
        void insert(const char* text) { txt.doc->insert(text, strlen(text)); }
        void scrollCursorInView();
        void setEditMode(bool set);
        //const SkFont& font() const { return fFont; }

    void paint(SkCanvas* canvas);

protected:

    bool scroll(SkScalar delta);


};
}  // namespace SkPlainTextEditor

