#include "EditorDoc.h"
#include "utf8_helpers.h"

using namespace SkEd;

void EditorDoc::insert(const char* utf8Text, size_t byteLen)
    {
    CmdInsert* cmdInsert = new CmdInsert(*this);
    cmdInsert->str.append(utf8Text, byteLen);
    undoRedo.execute(cmdInsert);
    }

void SkEd::CmdInsert::execute()
    {
    cursorPosBefore = doc.getCursorPos();
    selectPosBefore = doc.getSelectionPos();
    if (doc.hasSelection())
        {
        strBefore = doc.selectionToString();
        doc._remove();
        }
    doc._insert(str.c_str(), str.size());
    cursorPosAfter = doc.getCursorPos();
    }

void SkEd::CmdInsert::undo()
    {
    doc.setCursor(std::min(selectPosBefore, cursorPosBefore));
    doc.setCursor(cursorPosAfter, true);
    doc._remove();
    if (!strBefore.empty())
        doc._insert(strBefore.c_str(), strBefore.length());
    doc.setCursor(selectPosBefore);
    doc.setCursor(cursorPosBefore, true);
    }

void SkEd::EditorDoc::remove(bool backSpace)
    {
    CmdRemove* cmdRemove = new CmdRemove(*this);
    cmdRemove->backspace = backSpace;
    undoRedo.execute(cmdRemove);
    }

void SkEd::CmdRemove::execute()
    {
    cursorPosBefore = doc.getCursorPos();
    selectPosBefore = doc.getSelectionPos();
    if (!doc.hasSelection())
        doc.moveCursor(!backspace, true);
    strBefore = doc.selectionToString();
    doc._remove();
    cursorPosAfter = doc.getCursorPos();
    }

void SkEd::CmdRemove::undo()
    {
    doc.setCursor(cursorPosAfter);
    doc._insert(strBefore.c_str(), strBefore.length());
    doc.setCursor(selectPosBefore);
    doc.setCursor(cursorPosBefore, true);
    }

void EditorDoc::_insert(const char* utf8Text, size_t byteLen) {
    if (!valid_utf8(utf8Text, byteLen) || 0 == byteLen)
        return;

    TextBuffer txt(utf8Text, byteLen);
    size_t newlinecount = txt.count_char('\n');
    Paragraph* para = &fParas[fCursorPos.Para];
    forEachLine(txt.begin(), txt.size(), [&para, this](const char* str, size_t lineLen, bool isNewLine) {
        if (isNewLine)
            {
            Paragraph newPara;
            //split current line at current cursorLoc.
            const char* strAfterSplitPart = fParas[fCursorPos.Para].fText.begin() + fCursorPos.Byte;
            newPara.fText.insert(0, strAfterSplitPart, fParas[fCursorPos.Para].fText.end() - strAfterSplitPart);
            fParas.insert(this->fParas.begin() + fCursorPos.Para + 1, newPara);
            fParas[fCursorPos.Para].fText.trim(fCursorPos.Byte);
            fireParagraphChanged(&fParas[fCursorPos.Para]);
            fCursorPos.Para++;
            fCursorPos.Byte = 0;
            }
        fParas[fCursorPos.Para].fText.insert(fCursorPos.Byte, str, lineLen);
        fireParagraphChanged(&this->fParas[fCursorPos.Para]);
        fCursorPos.Byte += lineLen;
        });
    selectionPos = fCursorPos;
    return;
    }

void EditorDoc::_remove()
    {
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
    for (auto para = firstP + 1; para < lastP; ++para) {
        str += "\n";
        str.append(para->fText.begin(), para->fText.size());
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

