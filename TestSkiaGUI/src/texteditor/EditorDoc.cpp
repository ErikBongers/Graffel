#include "EditorDoc.h"

using namespace SkEd;

static const TextBuffer remove_newline(const char* str, size_t len) {
    return SkASSERT((str != nullptr) || (len == 0)),
        TextBuffer(str, (len > 0 && str[len - 1] == '\n') ? len - 1 : len);
    }
static size_t count_char(const TextBuffer& string, char value) {
    size_t count = 0;
    for (char c : string) { if (c == value) { ++count; } }
    return count;
    }

TextPosition EditorDoc::insert(const char* utf8Text, size_t byteLen) {
    if (!valid_utf8(utf8Text, byteLen) || 0 == byteLen) {
        return fCursorPos;
        }
    if (fCursorPos.Para < fParas.size()) {
        fParas[fCursorPos.Para].fText.insert(fCursorPos.Byte, utf8Text, byteLen);
        fireParagraphChanged(&fParas[fCursorPos.Para]);
        }
    else {
        fParas.push_back(EditorDoc::Paragraph(TextBuffer(utf8Text, byteLen)));
        }
    fCursorPos = TextPosition{ fCursorPos.Para, fCursorPos.Byte + byteLen };
    size_t newlinecount = count_char(fParas[fCursorPos.Para].fText, '\n');
    if (newlinecount > 0) {
        TextBuffer src = std::move(fParas[fCursorPos.Para].fText);
        std::vector<Paragraph>::const_iterator next = fParas.begin() + fCursorPos.Para + 1;
        fParas.insert(next, newlinecount, Paragraph());
        Paragraph* para = &fParas[fCursorPos.Para];
        forEachLine(src.begin(), src.size(), [&para, this](const char* str, size_t l) {
            this->fireParagraphChanged(para);
            (para++)->fText = remove_newline(str, l);
                    });
        }
    return fCursorPos;
    }

::TextPosition EditorDoc::remove(TextPosition pos1, TextPosition pos2) {
    TextPosition start = std::min(pos1, pos2);
    TextPosition end = std::max(pos1, pos2);
    if (start == end || start.Para == fParas.size()) {
        return start;
        }
    if (start.Para == end.Para) {
        fParas[start.Para].fText.remove(
            start.Byte, end.Byte - start.Byte);
        fireParagraphChanged(&fParas[start.Para]);
        }
    else {
        auto& line = fParas[start.Para];
        line.fText.remove(start.Byte,
                          line.fText.size() - start.Byte);
        line.fText.insert(start.Byte,
                          fParas[end.Para].fText.begin() + end.Byte,
                          fParas[end.Para].fText.size() - end.Byte);
        fireParagraphChanged(&line);
        fParas.erase(fParas.begin() + start.Para + 1,
                     fParas.begin() + end.Para + 1);
        }
    fCursorPos = fMarkPos = start;
    fireCursorMoved();
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


std::string EditorDoc::toString()
    {
    size_t totalSize = 0;
    for (auto& para : fParas)
        {
        totalSize += para.fText.size() + 1;// +1 for '\n'
        }
    std::string str;
    str.reserve(totalSize);
    for (Paragraph& para : fParas)
        {
        TextSpan span = para.fText.view();
        str.append(span.data, span.size);
        str += '\n';
        }
    return str;
    }