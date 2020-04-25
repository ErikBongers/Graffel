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
    size_t Para = SIZE_MAX;  // logical line, based on hard newline characters.
    size_t Byte = SIZE_MAX;   // index into UTF-8 representation of line.
    operator bool() const { return Byte != SIZE_MAX && Para != SIZE_MAX; }
    bool operator==(const SkEd::TextPosition& v) const { return Para == v.Para && Byte == v.Byte; }
    bool operator<(const SkEd::TextPosition& v) const { 
        bool test1 = Para < v.Para;
        return test1 || (Para == v.Para && Byte < v.Byte); 
        }
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

