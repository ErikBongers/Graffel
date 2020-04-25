#include "editor.h"

using namespace SkEd;

Editor::Editor()
    {
    txt.docChanged = [this]() {
        if(this->getWindow())
            this->getWindow()->setInvalid();
        };
    txt.cursorMoved = [this]() {
        this->scrollCursorInView();
        };
    }

void Editor::_resize(SDL_WindowEvent& event)
    {
    txt.setWidth(rect.width()-fMargin);
    getWindow()->setInvalid();
    }

//static void append(char** dst, size_t* count, const char* src, size_t n) {
//    if (*dst) {
//        ::memcpy(*dst, src, n);
//        *dst += n;
//        }
//    *count += n;
//    }
//
//size_t SkPlainTextEditor::Editor::copy(TextPosition pos1, TextPosition pos2, char* dst) const {
//    size_t size = 0;
//    pos1 = this->move(Editor::Movement::kNowhere, pos1);
//    pos2 = this->move(Editor::Movement::kNowhere, pos2);
//    auto cmp = [](const Editor::TextPosition& u, const Editor::TextPosition& v) { return u < v; };
//    Editor::TextPosition start = std::min(pos1, pos2, cmp);
//    Editor::TextPosition end = std::max(pos1, pos2, cmp);
//    if (start == end || start.fParagraphIndex == fParas.size()) {
//        return size;
//        }
//    if (start.fParagraphIndex == end.fParagraphIndex) {
//        SkASSERT(end.fTextByteIndex > start.fTextByteIndex);
//        auto& str = fParas[start.fParagraphIndex].fText;
//        append(&dst, &size, str.begin() + start.fTextByteIndex,
//               end.fTextByteIndex - start.fTextByteIndex);
//        return size;
//        }
//    SkASSERT(end.fParagraphIndex < fParas.size());
//    const std::vector<TextParagraph>::const_iterator firstP = fParas.begin() + start.fParagraphIndex;
//    const std::vector<TextParagraph>::const_iterator lastP = fParas.begin() + end.fParagraphIndex;
//    const auto& first = firstP->fText;
//    const auto& last = lastP->fText;
//
//    append(&dst, &size, first.begin() + start.fTextByteIndex, first.size() - start.fTextByteIndex);
//    for (auto line = firstP + 1; line < lastP; ++line) {
//        append(&dst, &size, "\n", 1);
//        append(&dst, &size, line->fText.begin(), line->fText.size());
//        }
//    append(&dst, &size, "\n", 1);
//    append(&dst, &size, last.begin(), end.fTextByteIndex);
//    return size;
//    }


// returns smallest i such that list[i] > value.  value > list[i-1]
// Use a binary search since list is monotonic
void SkEd::Editor::textInput(SDL_TextInputEvent& event)
    {
    if (!editMode)
        return;
    txt.doc.insert(event.text, strlen(event.text));
    }


void SkEd::Editor::_mouseDown(SDL_MouseButtonEvent& event)
    {
    if (event.clicks == 2)
        {
        editMode = true;
        txt.showCursor = true;
        return;
        }
    if (!editMode)
        return;
    SkPoint point = SkPoint::Make((SkScalar)event.x, (SkScalar)event.y);
    mapPixelsToPoints(&point, 1);
    auto shift = SDL_GetModState() & KMOD_SHIFT;
    setCursor(txt.getPosition({ point.fX - fMargin, point.fY + scrollPos - fMargin }), shift);
    }

void SkEd::Editor::_mouseUp(SDL_MouseButtonEvent& event)
    {
    if (!editMode)
        return;
    SkPoint point = SkPoint::Make((SkScalar)event.x, (SkScalar)event.y);
    mapPixelsToPoints(&point, 1);
    auto shift = SDL_GetModState() & KMOD_SHIFT;
    setCursor(txt.getPosition({ point.fX - fMargin, point.fY + scrollPos - fMargin }), shift);
    }

bool SkEd::Editor::scroll(SkScalar delta)
    {
    SkRect cursorRect = txt.getTextLocation(txt.doc.getCursorPos());

    SkScalar maxPos = std::max<SkScalar>(0, txt.getFullTextHeight() + 2 * fMargin - rect.height() / 2);
    SkScalar newpos = std::max<SkScalar>(0, std::min<SkScalar>(scrollPos + delta, maxPos));
    if (newpos != scrollPos) {
        SkScalar actualDelta = newpos - scrollPos;
        scrollPos = newpos;
        setCursor(txt.getPosition({ cursorRect.centerX(), cursorRect.centerY() + actualDelta}), false);
        getWindow()->setInvalid();
        }
    return true;
    }

void SkEd::Editor::keyDown(SDL_KeyboardEvent& event)
    {
    if (!editMode)
        return;

    if (event.type == SDL_KEYDOWN)
        {
        bool shift = event.keysym.mod & KMOD_SHIFT;
        switch (event.keysym.sym)
            {
            case SDLK_UP: moveCursor(Movement::kUp, shift); break;
            case SDLK_DOWN: moveCursor(Movement::kDown, shift); break;
            case SDLK_LEFT: moveCursor(Movement::kLeft, shift); break;
            case SDLK_RIGHT: moveCursor(Movement::kRight, shift); break;
            case SDLK_HOME: moveCursor(Movement::kHome, shift); break;
            case SDLK_END: moveCursor(Movement::kEnd, shift); break;
            case SDLK_PAGEDOWN:
                scroll(rect.height() * 4 / 5);
                break;
            case SDLK_PAGEUP:
                scroll(-rect.height() * 4 / 5);
                break;
            case SDLK_DELETE:
                txt.doc.remove();
                break;
            case SDLK_BACKSPACE:
                txt.doc.remove(true);
                break;
            case SDLK_RETURN:
                {
                char c = '\n';
                txt.doc.insert(&c, 1);
                break;
                }
            case SDLK_ESCAPE:
                editMode = false;
                txt.showCursor = false;
                break;
            default:
                break;
            }
        }
    }

bool SkEd::Editor::moveCursor(Movement m, bool shift) 
    {
    return setCursor(txt.getPositionMoved(m, txt.doc.getCursorPos()), shift);
    }

void SkEd::Editor::scrollCursorInView()
    {
    SkRect cursor = txt.getTextLocation(txt.doc.getCursorPos());
    if (scrollPos < cursor.bottom() - (int)rect.height() + 2 * fMargin) {
        scrollPos = cursor.bottom() - (int)rect.height() + 2 * fMargin;
        }
    else if (cursor.top() < scrollPos) {
        scrollPos = cursor.top();
        }
    getWindow()->setInvalid();
    }

bool SkEd::Editor::setCursor(TextPosition pos, bool shift)
    {
    txt.doc.setCursor(pos, shift);
    scrollCursorInView();
    return true;
    }

void SkEd::Editor::drawMe()
    {
    paint(&getWindow()->Canvas());
    }

void SkEd::Editor::onIdle()
    {
    if (txt.onIdle())
        {
        getWindow()->setInvalid();
        }
    }

void SkEd::Editor::paint(SkCanvas* c) {
    c->clipRect({ 0, 0, (float)rect.width(), (float)rect.height()});
    c->translate((SkScalar)fMargin, (float)(fMargin - scrollPos));
    txt.paint(*c);
    }

