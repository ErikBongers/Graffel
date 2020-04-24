#pragma once
#include "..\pch.h"
#include <climits>
#include <utility>
#include <cstddef>
#include "stringslice.h"

namespace SkEd {
template <typename F>
static void forEachLine(const void* data, size_t size, F f) {
    const char* start = (const char*)data;
    const char* end = start + size;
    const char* ptr = start;
    while (ptr < end) {
        while (*ptr++ != '\n' && ptr < end) {}
        size_t len = ptr - start;
        SkASSERT(len > 0);
        f(start, len);
        start = ptr;
        }
    }
static bool valid_utf8(const char* ptr, size_t size) { return SkUTF::CountUTF8(ptr, size) >= 0; }


struct TextPosition {
    size_t fTextByteIndex = SIZE_MAX;   // index into UTF-8 representation of line.
    size_t fParagraphIndex = SIZE_MAX;  // logical line, based on hard newline characters.
    };

enum class Movement { kNowhere, kLeft, kUp, kRight, kDown, kHome, kEnd, kWordLeft, kWordRight, };

class EditorText
    {
    private:
        bool fNeedsReshape = false;
        SkScalar width = 0;
        SkScalar fullTextHeight = 0;
        void resetCursorBlink();

    public:
        struct TextParagraph {
            StringSlice fText;
            sk_sp<const SkTextBlob> fBlob;
            std::vector<SkRect> fCursorPos;
            std::vector<size_t> fLineEndOffsets;
            std::vector<bool> fWordBoundaries;
            SkIPoint fOrigin = { 0, 0 };
            int fHeight = 0;
            bool fShaped = false;

            TextParagraph(StringSlice t) : fText(std::move(t)) {}
            TextParagraph() {}
            };
        std::vector<TextParagraph> fParas;
        SkFont fFont;
        const char* fLocale = "en";
        bool showCursor = false;
        std::chrono::time_point<std::chrono::steady_clock> startCursorTime; //TODO: add cursor blinktime user defined.
        bool cursorBlinkOn = false;
        SkColor4f fForegroundColor = { 1, 1, 1, 1 };
        SkColor4f fSelectionColor = { 0.729f, 0.827f, 0.988f, 1 };
        SkColor4f fCursorColor = { 1, 0, 0, 1 };
        TextPosition fSelectionBegin;
        TextPosition fSelectionEnd;
        TextPosition fCursor;
        TextPosition fTextPos{ 0, 0 };
        TextPosition fMarkPos;
        bool fShiftDown = false;

        TextPosition insert(TextPosition, const char* utf8Text, size_t byteLen);
        TextPosition remove(TextPosition, TextPosition);
        TextPosition move(Movement move, TextPosition pos) const;
        void markDirty(TextParagraph*);
        int lineHeight(size_t index) const { return fParas[index].fHeight; }
        size_t lineCount() const { return fParas.size(); }
        StringView line(size_t i) const {
            return i < fParas.size() ? fParas[i].fText.view() : StringView{ nullptr, 0 };
            }
        void setFont(SkFont font);
        void setWidth(SkScalar w) {
            if (width != w) {
                width = w;
                fNeedsReshape = true;
                for (auto& l : fParas) { this->markDirty(&l); }
                }
            }
        SkScalar getFullTextHeight() { return fullTextHeight; }
        TextPosition getPosition(SkPoint);
        void reshapeAll();
        SkRect getCursorTextLocation(TextPosition);
        void paint(SkCanvas& canvas);
        bool onIdle();
        bool moveCursor(Movement m, bool shift);
        bool moveTo(TextPosition pos, bool shift);

    };
}

static inline bool operator==(const SkEd::TextPosition& u,
                              const SkEd::TextPosition& v) {
    return u.fParagraphIndex == v.fParagraphIndex && u.fTextByteIndex == v.fTextByteIndex;
    }
static inline bool operator!=(const SkEd::TextPosition& u,
                              const SkEd::TextPosition& v) {
    return !(u == v);
    }

static inline bool operator<(const SkEd::TextPosition& u,
    const SkEd::TextPosition& v) {
    return u.fParagraphIndex < v.fParagraphIndex ||
        (u.fParagraphIndex == v.fParagraphIndex && u.fTextByteIndex < v.fTextByteIndex);
    }
