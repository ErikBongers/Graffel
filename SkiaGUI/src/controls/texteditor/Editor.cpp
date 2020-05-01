#include "editor.h"

using namespace SkEd;

Editor::Editor()
    {
    txt.docChanged = [this]() {
        if(getWindow())
            getWindow()->setInvalid();
        };
    txt.cursorMoved = [this]() {
        if(hasFocus())
            scrollCursorInView();
        };
    txt.uiElement = this;
    }

void Editor::_resizeContent(SDL_WindowEvent& event)
    {
    txt.setWidth(rect.width()-fMargin);
    getWindow()->setInvalid();
    }

void SkEd::Editor::textInput(SDL_TextInputEvent& event)
    {
    if (!editMode)
        return;
    txt.doc->insert(event.text, strlen(event.text));
    }


void SkEd::Editor::setEditMode(bool set) 
    { 
    txt.showCursor = editMode = set;
    takeFocus(set ? this: nullptr);
    }

void SkEd::Editor::_mouseDown(SDL_MouseButtonEvent& event)
    {
    if (event.clicks == 2)
        {
        setEditMode(true);
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

void SkEd::Editor::scrollCursorInView()
    {
    SkRect cursor = txt.getTextLocation(txt.doc->getCursorPos());
    if (scrollPos < cursor.bottom() - (int)rect.height() + 2 * fMargin) {
        scrollPos = cursor.bottom() - (int)rect.height() + 2 * fMargin;
        }
    else if (cursor.top() < scrollPos) {
        scrollPos = cursor.top();
        }
    getWindow()->setInvalid();
    }

bool SkEd::Editor::scroll(SkScalar delta)
    {
    SkRect cursorRect = txt.getTextLocation(txt.doc->getCursorPos());

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
    if (!editMode || !hasFocus())
        return;

    if (event.type == SDL_KEYDOWN)
        {
        bool shift = event.keysym.mod & KMOD_SHIFT;
        bool ctrl = event.keysym.mod & KMOD_CTRL;
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
                txt.doc->remove();
                break;
            case SDLK_BACKSPACE:
                txt.doc->remove(true);
                break;
            case SDLK_RETURN:
                {
                char c = '\n';
                txt.doc->insert(&c, 1);
                break;
                }
            case SDLK_ESCAPE:
                setEditMode(false);
                break;
            case SDLK_v:
                if (!ctrl)
                    break;
                if (SDL_HasClipboardText())
                    {
                    char* clipTxt = SDL_GetClipboardText();
                    txt.doc->insert(clipTxt, strlen(clipTxt));
                    }
                break;
            case SDLK_x:
            case SDLK_c:
                {
                if (!ctrl)
                    break;
                auto strClip = txt.doc->selectionToString();
                SDL_SetClipboardText(strClip.c_str());
                if (event.keysym.sym == SDLK_x)
                    txt.doc->remove();
                }
                break;
            case SDLK_z:
                if (ctrl && shift)
                    txt.doc->redo();
                else if (ctrl)
                    txt.doc->undo();
                break;
            default:
                break;
            }
        }
    }

bool SkEd::Editor::moveCursor(Movement m, bool shift) 
    {
    return setCursor(txt.getPositionMoved(m, txt.doc->getCursorPos()), shift);
    }

bool SkEd::Editor::setCursor(TextPosition pos, bool shift)
    {
    txt.doc->setCursor(pos, shift);
    if(hasFocus())
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

