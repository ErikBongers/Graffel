#include "EditorDoc.h"

using namespace SkEd;

static const StringSlice remove_newline(const char* str, size_t len) {
    return SkASSERT((str != nullptr) || (len == 0)),
        StringSlice(str, (len > 0 && str[len - 1] == '\n') ? len - 1 : len);
    }
static size_t count_char(const StringSlice& string, char value) {
    size_t count = 0;
    for (char c : string) { if (c == value) { ++count; } }
    return count;
    }

TextPosition EditorDoc::insert(const char* utf8Text, size_t byteLen) {
    if (!valid_utf8(utf8Text, byteLen) || 0 == byteLen) {
        return fCursorPos;
        }
    if (fCursorPos.fParagraphIndex < fParas.size()) {
        fParas[fCursorPos.fParagraphIndex].fText.insert(fCursorPos.fTextByteIndex, utf8Text, byteLen);
        fireParagraphChanged(&fParas[fCursorPos.fParagraphIndex]);
        }
    else {
        fParas.push_back(EditorDoc::Paragraph(StringSlice(utf8Text, byteLen)));
        }
    fCursorPos = TextPosition{ fCursorPos.fTextByteIndex + byteLen, fCursorPos.fParagraphIndex };
    size_t newlinecount = count_char(fParas[fCursorPos.fParagraphIndex].fText, '\n');
    if (newlinecount > 0) {
        StringSlice src = std::move(fParas[fCursorPos.fParagraphIndex].fText);
        std::vector<Paragraph>::const_iterator next = fParas.begin() + fCursorPos.fParagraphIndex + 1;
        fParas.insert(next, newlinecount, Paragraph());
        Paragraph* para = &fParas[fCursorPos.fParagraphIndex];
        forEachLine(src.begin(), src.size(), [&para, this](const char* str, size_t l) {
            this->fireParagraphChanged(para);
            (para++)->fText = remove_newline(str, l);
                    });
        }
    return fCursorPos;
    }

::TextPosition EditorDoc::remove(TextPosition pos1, TextPosition pos2) {
    auto cmp = [](const TextPosition& u, const TextPosition& v) { return u < v; };
    TextPosition start = std::min(pos1, pos2, cmp);
    TextPosition end = std::max(pos1, pos2, cmp);
    if (start == end || start.fParagraphIndex == fParas.size()) {
        return start;
        }
    if (start.fParagraphIndex == end.fParagraphIndex) {
        fParas[start.fParagraphIndex].fText.remove(
            start.fTextByteIndex, end.fTextByteIndex - start.fTextByteIndex);
        fireParagraphChanged(&fParas[start.fParagraphIndex]);
        }
    else {
        auto& line = fParas[start.fParagraphIndex];
        line.fText.remove(start.fTextByteIndex,
                          line.fText.size() - start.fTextByteIndex);
        line.fText.insert(start.fTextByteIndex,
                          fParas[end.fParagraphIndex].fText.begin() + end.fTextByteIndex,
                          fParas[end.fParagraphIndex].fText.size() - end.fTextByteIndex);
        fireParagraphChanged(&line);
        fParas.erase(fParas.begin() + start.fParagraphIndex + 1,
                     fParas.begin() + end.fParagraphIndex + 1);
        }
    return start;
    }

bool EditorDoc::setCursor(TextPosition pos, bool expandSelection)
    {
    if (pos == fCursorPos)
        return false;

    if (expandSelection)
        {
        if (!fMarkPos)
            fMarkPos = fCursorPos;
        }
    else
        fMarkPos = TextPosition();
    fCursorPos = pos;
    fireCursorMoved();
    return true;
    }


