#pragma once
#include "..\pch.h"
#include "stringview.h"


#include "core/UIElement.h"
#include "EditorText.h"

class SkCanvas;
class SkShaper;

namespace SkEd {

class Editor : public UIElement
    {
    protected:
        EditorText txt;
        void keyDown(SDL_KeyboardEvent& event, SDLSkiaWindow& window) override;
        void textInput(SDL_TextInputEvent& event, SDLSkiaWindow& window) override;
        void _mouseDown(SDL_MouseButtonEvent& event, SDLSkiaWindow& window) override;
        void _mouseUp(SDL_MouseButtonEvent& event, SDLSkiaWindow& window) override;
        void _resize(SDL_WindowEvent& event, SDLSkiaWindow& window) override;
        void drawMe(SDLSkiaWindow& window) override;
        void onIdle(SDLSkiaWindow& window) override;
        bool moveCursor(Movement m, bool shift, SDLSkiaWindow& window);
        bool moveTo(TextPosition pos, bool shift, SDLSkiaWindow& window);
        bool editMode = false;
        SkScalar scrollPos = 0;  // window pixel position in file
        SkScalar fMargin = 10;
    public:
        SkScalar getHeight() const { return rect.height(); }
        void setFont(SkFont font) { txt.setFont(font); }
        void insert(const char* text) { txt.insert(TextPosition{ 0, 0 }, text, strlen(text)); }

        //const SkFont& font() const { return fFont; }

    //struct Text {
    //    const std::vector<TextParagraph>& fParas;
    //    struct Iterator {
    //        std::vector<TextParagraph>::const_iterator fPtr;
    //        StringView operator*() { return fPtr->fText.view(); }
    //        void operator++() { ++fPtr; }
    //        bool operator!=(const Iterator& other) const { return fPtr != other.fPtr; }
    //    };
    //    Iterator begin() const { return Iterator{fParas.begin()}; }
    //    Iterator end() const { return Iterator{fParas.end()}; }
    //};
    // Loop over all the lines of text.  The lines are not '\0'- or '\n'-terminated.
    // For example, to dump the entire file to standard output:
    //     for (SkPlainTextEditor::StringView str : editor.text()) {
    //         std::cout.write(str.data, str.size) << '\n';
    //     }
    //Text text() const { return Text{fParas}; }


    // If dst is nullptr, returns size of given selection.
    // Otherwise, fill dst with a copy of the selection, and return the amount copied.
    size_t copy(TextPosition pos1, TextPosition pos2, char* dst = nullptr) const;

    struct PaintOpts {
    };
    void paint(SkCanvas* canvas);

protected:

    bool scroll(SkScalar delta, SDLSkiaWindow& window);


};
}  // namespace SkPlainTextEditor

