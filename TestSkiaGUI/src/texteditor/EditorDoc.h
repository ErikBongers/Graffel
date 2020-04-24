#pragma once
#include "../pch.h"
#include "TextBuffer.h"
#include "TextSpan.h"

namespace SkEd
{
typedef std::function<void()> PCursorMoved;

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
    operator bool() const { return fTextByteIndex != SIZE_MAX && fParagraphIndex != SIZE_MAX; }
    bool operator==(const SkEd::TextPosition& v) { return fParagraphIndex == v.fParagraphIndex && fTextByteIndex == v.fTextByteIndex; }
    };

enum class Movement { kNowhere, kLeft, kUp, kRight, kDown, kHome, kEnd, kWordLeft, kWordRight, };

class EditorDoc
    {
    public:

        struct Paragraph {
            TextBuffer fText;
            Paragraph(TextBuffer t) : fText(std::move(t)) {}
            Paragraph() {}
            std::shared_ptr<void> data = nullptr;
            };

        typedef std::function<void(EditorDoc::Paragraph& para)> PParagraphChanged;

        std::vector<Paragraph> fParas;

        TextPosition fCursorPos{ 0, 0 };
        TextPosition fMarkPos;
        TextPosition insert(const char* utf8Text, size_t byteLen);
        TextPosition remove(TextPosition, TextPosition);
        bool setCursor(TextPosition pos, bool shift);

        size_t lineCount() const { return fParas.size(); }
        TextSpan line(size_t i) const {
            return i < fParas.size() ? fParas[i].fText.view() : TextSpan{ nullptr, 0 };
            }
        PParagraphChanged paragraphChanged = nullptr;
        PCursorMoved cursorMoved = nullptr;
        std::string toString();
    private:
        void fireParagraphChanged(Paragraph* para) { if (paragraphChanged) paragraphChanged(*para); }
        void fireCursorMoved() { if (cursorMoved) cursorMoved(); }
    };

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
