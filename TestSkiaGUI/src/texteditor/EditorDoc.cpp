#include "EditorDoc.h"
#include "utf8_helpers.h"

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
    selectionPos = fCursorPos;
    return fCursorPos;
    }

void SkEd::EditorDoc::remove(bool backSpace)
    {
    if (!hasSelection())
        moveCursor(!backSpace, true);
    remove(fCursorPos, selectionPos);
    }

void EditorDoc::remove(TextPosition pos1, TextPosition pos2) {
    TextPosition start = std::min(pos1, pos2);
    TextPosition end = std::max(pos1, pos2);
    
    if (start == end || start.Para == fParas.size()) 
        return;
    
    if (start.Para == end.Para) 
        {
        fParas[start.Para].fText.remove(
            start.Byte, end.Byte - start.Byte);
        fireParagraphChanged(&fParas[start.Para]);
        }
    else 
        {
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
    fCursorPos = selectionPos = start;
    fireCursorMoved();
    }

bool EditorDoc::setCursor(TextPosition pos, bool expandSelection)
    {
    if (pos == fCursorPos)
        return false;

    if (expandSelection)
        fCursorPos = pos;
    else
        selectionPos = fCursorPos = pos;
    fireCursorMoved();
    return true;
    }


std::string EditorDoc::selectionToString()
    {
    std::string str;
    TextPosition start = std::min(fCursorPos, selectionPos);
    TextPosition end = std::max(fCursorPos, selectionPos);
    if (start == end)
        return "";
    if (start.Para == end.Para) 
        {
        auto& fText = fParas[start.Para].fText;
        str.append(fText.begin() + start.Byte, end.Byte - start.Byte);
        return str;
        }
    const std::vector<Paragraph>::const_iterator firstP = fParas.begin() + start.Para;
    const std::vector<Paragraph>::const_iterator lastP = fParas.begin() + end.Para;
    const auto& first = firstP->fText;
    const auto& last = lastP->fText;
    
    str.append(first.begin() + start.Byte, first.size() - start.Byte);
    for (auto line = firstP + 1; line < lastP; ++line) {
        str += "\n";
        str.append(line->fText.begin(), line->fText.size());
        }
    str += '\n';
    str.append(last.begin(), end.Byte);
    return str;
    }

void SkEd::EditorDoc::refitSelection()
    {
    fCursorPos = refitPosition(fCursorPos);
    selectionPos = refitPosition(selectionPos);
    }

TextPosition SkEd::EditorDoc::refitPosition(TextPosition pos)
    {
    if (pos.Para >= fParas.size()) {
        pos.Para = fParas.size() - 1;
        pos.Byte = fParas[pos.Para].fText.size();
        }
    else {
        pos.Byte = align_column(fParas[pos.Para].fText, pos.Byte);
        }
    return pos;
    }

void SkEd::EditorDoc::moveCursor(bool right, bool expandSelection)
    {
    if (expandSelection)
        if (fCursorPos == selectionPos)
            selectionPos = fCursorPos;
    fCursorPos = getPositionRelative(fCursorPos, right);
    }

TextPosition SkEd::EditorDoc::getPositionRelative(TextPosition pos, bool right)
    {
    if (right)
        {
        if (fParas[pos.Para].fText.size() == pos.Byte) {
            if (pos.Para + 1 < fParas.size()) {
                ++pos.Para;
                pos.Byte = 0;
                }
            }
        else {
            const auto& str = fParas[pos.Para].fText;
            pos.Byte =
                next_utf8(begin(str) + pos.Byte, end(str)) - begin(str);
            }
        }
    else
        {
        if (0 == pos.Byte) {
            if (pos.Para > 0) {
                --pos.Para;
                pos.Byte = fParas[pos.Para].fText.size();
                }
            }
        else {
            const auto& str = fParas[pos.Para].fText;
            pos.Byte =
                prev_utf8(begin(str) + pos.Byte, begin(str)) - begin(str);
            }
        }
    return pos;
    }
