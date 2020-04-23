#pragma once
#include "..\pch.h"
#include "modules/skplaintexteditor/include/stringslice.h"
#include "modules/skplaintexteditor/include/stringview.h"

#include "include/core/SkColor.h"
#include "include/core/SkFont.h"
#include "include/core/SkString.h"
#include "include/core/SkTextBlob.h"

#include <climits>
#include <cstdint>
#include <utility>
#include <vector>

#include "core/UIElement.h"

class SkCanvas;
class SkShaper;

namespace SkPlainTextEditor {

class Editor : public UIElement
    {
    public:
    struct TextPosition {
        size_t fTextByteIndex = SIZE_MAX;   // index into UTF-8 representation of line.
        size_t fParagraphIndex = SIZE_MAX;  // logical line, based on hard newline characters.
        };
    protected:
    enum class Movement {
        kNowhere,
        kLeft,
        kUp,
        kRight,
        kDown,
        kHome,
        kEnd,
        kWordLeft,
        kWordRight,
        };
    struct TextParagraph;

    void resetCursorBlink(SDLSkiaWindow& window);
    void keyDown(SDL_KeyboardEvent& event, SDLSkiaWindow& window) override;
    void textInput(SDL_TextInputEvent& event, SDLSkiaWindow& window) override;
    void _mouseDown(SDL_MouseButtonEvent& event, SDLSkiaWindow& window) override;
    void _mouseUp(SDL_MouseButtonEvent& event, SDLSkiaWindow& window) override;
    void drawMe(SDLSkiaWindow& window) override;
    void onIdle(SDLSkiaWindow& window) override;
    void setSize(int width, int height, SDLSkiaWindow& window);
    bool moveCursor(Editor::Movement m, bool shift, SDLSkiaWindow& window);
    bool moveTo(Editor::TextPosition pos, bool shift, SDLSkiaWindow& window);
    std::chrono::time_point<std::chrono::steady_clock> startCursorTime;
    bool showCursor;
    int fPos = 0;  // window pixel position in file
    int fMargin = 10;
    public:
    TextPosition fTextPos{ 0, 0 };
    TextPosition fMarkPos;
    bool fShiftDown = false;
    int getHeight() const { return rect.height(); }
    void setWidth(int w); // may force re-shape

    const SkFont& font() const { return fFont; }
    void setFont(SkFont font);
    // get size of line in canvas display units.
    int lineHeight(size_t index) const { return fParas[index].fHeight; }


    struct Text {
        const std::vector<TextParagraph>& fParas;
        struct Iterator {
            std::vector<TextParagraph>::const_iterator fPtr;
            StringView operator*() { return fPtr->fText.view(); }
            void operator++() { ++fPtr; }
            bool operator!=(const Iterator& other) const { return fPtr != other.fPtr; }
        };
        Iterator begin() const { return Iterator{fParas.begin()}; }
        Iterator end() const { return Iterator{fParas.end()}; }
    };
    // Loop over all the lines of text.  The lines are not '\0'- or '\n'-terminated.
    // For example, to dump the entire file to standard output:
    //     for (SkPlainTextEditor::StringView str : editor.text()) {
    //         std::cout.write(str.data, str.size) << '\n';
    //     }
    Text text() const { return Text{fParas}; }

    TextPosition move(Editor::Movement move, Editor::TextPosition pos) const;
    TextPosition getPosition(SkIPoint);
    SkRect getLocation(TextPosition);
    TextPosition insert(TextPosition, const char* utf8Text, size_t byteLen);
    TextPosition remove(TextPosition, TextPosition);

    // If dst is nullptr, returns size of given selection.
    // Otherwise, fill dst with a copy of the selection, and return the amount copied.
    size_t copy(TextPosition pos1, TextPosition pos2, char* dst = nullptr) const;
    size_t lineCount() const { return fParas.size(); }
    StringView line(size_t i) const {
        return i < fParas.size() ? fParas[i].fText.view() : StringView{nullptr, 0};
    }

    struct PaintOpts {
        SkColor4f fForegroundColor = {1, 1, 1, 1};
        SkColor4f fSelectionColor = {0.729f, 0.827f, 0.988f, 1};
        SkColor4f fCursorColor = {1, 0, 0, 1};
        TextPosition fSelectionBegin;
        TextPosition fSelectionEnd;
        TextPosition fCursor;
    };
    void paint(SkCanvas* canvas);

private:
    struct TextParagraph {
        StringSlice fText;
        sk_sp<const SkTextBlob> fBlob;
        std::vector<SkRect> fCursorPos;
        std::vector<size_t> fLineEndOffsets;
        std::vector<bool> fWordBoundaries;
        SkIPoint fOrigin = {0, 0};
        int fHeight = 0;
        bool fShaped = false;

        TextParagraph(StringSlice t) : fText(std::move(t)) {}
        TextParagraph() {}
    };
    std::vector<TextParagraph> fParas;
    int fullTextHeight = 0;
    SkFont fFont;
    bool fNeedsReshape = false;
    const char* fLocale = "en";

    void markDirty(TextParagraph*);
    void reshapeAll();
    bool scroll(int delta, SDLSkiaWindow& window);


};
}  // namespace SkPlainTextEditor

static inline bool operator==(const SkPlainTextEditor::Editor::TextPosition& u,
                              const SkPlainTextEditor::Editor::TextPosition& v) {
    return u.fParagraphIndex == v.fParagraphIndex && u.fTextByteIndex == v.fTextByteIndex;
}
static inline bool operator!=(const SkPlainTextEditor::Editor::TextPosition& u,
                              const SkPlainTextEditor::Editor::TextPosition& v) { return !(u == v); }

static inline bool operator<(const SkPlainTextEditor::Editor::TextPosition& u,
                             const SkPlainTextEditor::Editor::TextPosition& v) {
    return u.fParagraphIndex < v.fParagraphIndex ||
           (u.fParagraphIndex == v.fParagraphIndex && u.fTextByteIndex < v.fTextByteIndex);
}
