#include "editor.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkExecutor.h"
#include "include/core/SkPath.h"
#include "src/utils/SkUTF.h"

#include "shape.h"

#include <algorithm>

using namespace SkPlainTextEditor;

static inline SkRect offset(SkRect r, SkIPoint p) {
    return r.makeOffset((float)p.x(), (float)p.y());
}

static constexpr SkRect kUnsetRect{-FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX};

static bool valid_utf8(const char* ptr, size_t size) { return SkUTF::CountUTF8(ptr, size) >= 0; }

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

static const StringSlice remove_newline(const char* str, size_t len) {
    return SkASSERT((str != nullptr) || (len == 0)),
           StringSlice(str, (len > 0 && str[len - 1] == '\n') ? len - 1 : len);
}

void SkPlainTextEditor::Editor::markDirty(TextParagraph* line) {
    line->fBlob = nullptr;
    line->fShaped = false;
    line->fWordBoundaries = std::vector<bool>();
}

void SkPlainTextEditor::Editor::setFont(SkFont font) {
    if (font != fFont) {
        fFont = std::move(font);
        fNeedsReshape = true;
        for (auto& l : fParas) { this->markDirty(&l); }
    }
}

void SkPlainTextEditor::Editor::setWidth(int w) {
    if (rect.width() != w) {
        rect.setXYWH(rect.fLeft, rect.fTop, w, rect.height());
        fNeedsReshape = true;
        for (auto& l : fParas) { this->markDirty(&l); }
        }
    }
static SkPoint to_point(SkIPoint p) { return { (float)p.x(), (float)p.y() }; }

Editor::TextPosition Editor::getPosition(SkIPoint xy) {
    Editor::TextPosition approximatePosition;
    this->reshapeAll();
    for (size_t j = 0; j < fParas.size(); ++j) {
        const TextParagraph& line = fParas[j];
        SkIRect lineRect = { 0,
                            line.fOrigin.y(),
                            rect.width(),
                            j + 1 < fParas.size() ? fParas[j + 1].fOrigin.y() : INT_MAX };
        if (const SkTextBlob* b = line.fBlob.get()) {
            SkIRect r = b->bounds().roundOut();
            r.offset(line.fOrigin);
            lineRect.join(r);
            }
        if (!lineRect.contains(xy.x(), xy.y())) {
            continue;
            }
        SkPoint pt = to_point(xy - line.fOrigin);
        const std::vector<SkRect>& pos = line.fCursorPos;
        for (size_t i = 0; i < pos.size(); ++i) {
            if (pos[i] != kUnsetRect && pos[i].contains(pt.x(), pt.y())) {
                return Editor::TextPosition{ i, j };
                }
            }
        approximatePosition = { xy.x() <= line.fOrigin.x() ? 0 : line.fText.size(), j };
        }
    return approximatePosition;
    }

static inline bool is_utf8_continuation(char v) {
    return ((unsigned char)v & 0b11000000) ==
        0b10000000;
    }

static const char* next_utf8(const char* p, const char* end) {
    if (p < end) {
        do {
            ++p;
            } while (p < end && is_utf8_continuation(*p));
        }
    return p;
    }

static const char* align_utf8(const char* p, const char* begin) {
    while (p > begin && is_utf8_continuation(*p)) {
        --p;
        }
    return p;
    }

static const char* prev_utf8(const char* p, const char* begin) {
    return p > begin ? align_utf8(p - 1, begin) : begin;
    }

SkRect SkPlainTextEditor::Editor::getLocation(Editor::TextPosition cursor) {
    this->reshapeAll();
    cursor = this->move(Editor::Movement::kNowhere, cursor);
    if (fParas.size() > 0) {
        const TextParagraph& cLine = fParas[cursor.fParagraphIndex];
        SkRect pos = { 0, 0, 0, 0 };
        if (cursor.fTextByteIndex < cLine.fCursorPos.size()) {
            pos = cLine.fCursorPos[cursor.fTextByteIndex];
            }
        pos.fRight = pos.fLeft + 1;
        pos.fLeft -= 1;
        return offset(pos, cLine.fOrigin);
        }
    return SkRect{ 0, 0, 0, 0 };
    }

static size_t count_char(const StringSlice& string, char value) {
    size_t count = 0;
    for (char c : string) { if (c == value) { ++count; } }
    return count;
    }

Editor::TextPosition SkPlainTextEditor::Editor::insert(TextPosition pos, const char* utf8Text, size_t byteLen) {
    if (!valid_utf8(utf8Text, byteLen) || 0 == byteLen) {
        return pos;
        }
    pos = this->move(Editor::Movement::kNowhere, pos);
    fNeedsReshape = true;
    if (pos.fParagraphIndex < fParas.size()) {
        fParas[pos.fParagraphIndex].fText.insert(pos.fTextByteIndex, utf8Text, byteLen);
        this->markDirty(&fParas[pos.fParagraphIndex]);
        }
    else {
        SkASSERT(pos.fParagraphIndex == fParas.size());
        SkASSERT(pos.fTextByteIndex == 0);
        fParas.push_back(Editor::TextParagraph(StringSlice(utf8Text, byteLen)));
        }
    pos = Editor::TextPosition{ pos.fTextByteIndex + byteLen, pos.fParagraphIndex };
    size_t newlinecount = count_char(fParas[pos.fParagraphIndex].fText, '\n');
    if (newlinecount > 0) {
        StringSlice src = std::move(fParas[pos.fParagraphIndex].fText);
        std::vector<TextParagraph>::const_iterator next = fParas.begin() + pos.fParagraphIndex + 1;
        fParas.insert(next, newlinecount, TextParagraph());
        TextParagraph* line = &fParas[pos.fParagraphIndex];
        forEachLine(src.begin(), src.size(), [&line](const char* str, size_t l) {
            (line++)->fText = remove_newline(str, l);
                    });
        }
    return pos;
    }

Editor::TextPosition SkPlainTextEditor::Editor::remove(TextPosition pos1, TextPosition pos2) {
    pos1 = this->move(Editor::Movement::kNowhere, pos1);
    pos2 = this->move(Editor::Movement::kNowhere, pos2);
    auto cmp = [](const Editor::TextPosition& u, const Editor::TextPosition& v) { return u < v; };
    Editor::TextPosition start = std::min(pos1, pos2, cmp);
    Editor::TextPosition end = std::max(pos1, pos2, cmp);
    if (start == end || start.fParagraphIndex == fParas.size()) {
        return start;
        }
    fNeedsReshape = true;
    if (start.fParagraphIndex == end.fParagraphIndex) {
        SkASSERT(end.fTextByteIndex > start.fTextByteIndex);
        fParas[start.fParagraphIndex].fText.remove(
            start.fTextByteIndex, end.fTextByteIndex - start.fTextByteIndex);
        this->markDirty(&fParas[start.fParagraphIndex]);
        }
    else {
        SkASSERT(end.fParagraphIndex < fParas.size());
        auto& line = fParas[start.fParagraphIndex];
        line.fText.remove(start.fTextByteIndex,
                          line.fText.size() - start.fTextByteIndex);
        line.fText.insert(start.fTextByteIndex,
                          fParas[end.fParagraphIndex].fText.begin() + end.fTextByteIndex,
                          fParas[end.fParagraphIndex].fText.size() - end.fTextByteIndex);
        this->markDirty(&line);
        fParas.erase(fParas.begin() + start.fParagraphIndex + 1,
                     fParas.begin() + end.fParagraphIndex + 1);
        }
    return start;
    }

static void append(char** dst, size_t* count, const char* src, size_t n) {
    if (*dst) {
        ::memcpy(*dst, src, n);
        *dst += n;
        }
    *count += n;
    }

size_t SkPlainTextEditor::Editor::copy(TextPosition pos1, TextPosition pos2, char* dst) const {
    size_t size = 0;
    pos1 = this->move(Editor::Movement::kNowhere, pos1);
    pos2 = this->move(Editor::Movement::kNowhere, pos2);
    auto cmp = [](const Editor::TextPosition& u, const Editor::TextPosition& v) { return u < v; };
    Editor::TextPosition start = std::min(pos1, pos2, cmp);
    Editor::TextPosition end = std::max(pos1, pos2, cmp);
    if (start == end || start.fParagraphIndex == fParas.size()) {
        return size;
        }
    if (start.fParagraphIndex == end.fParagraphIndex) {
        SkASSERT(end.fTextByteIndex > start.fTextByteIndex);
        auto& str = fParas[start.fParagraphIndex].fText;
        append(&dst, &size, str.begin() + start.fTextByteIndex,
               end.fTextByteIndex - start.fTextByteIndex);
        return size;
        }
    SkASSERT(end.fParagraphIndex < fParas.size());
    const std::vector<TextParagraph>::const_iterator firstP = fParas.begin() + start.fParagraphIndex;
    const std::vector<TextParagraph>::const_iterator lastP = fParas.begin() + end.fParagraphIndex;
    const auto& first = firstP->fText;
    const auto& last = lastP->fText;

    append(&dst, &size, first.begin() + start.fTextByteIndex, first.size() - start.fTextByteIndex);
    for (auto line = firstP + 1; line < lastP; ++line) {
        append(&dst, &size, "\n", 1);
        append(&dst, &size, line->fText.begin(), line->fText.size());
        }
    append(&dst, &size, "\n", 1);
    append(&dst, &size, last.begin(), end.fTextByteIndex);
    return size;
    }

static inline const char* begin(const StringSlice& s) { return s.begin(); }

static inline const char* end(const StringSlice& s) { return s.end(); }

static size_t align_column(const StringSlice& str, size_t p) {
    if (p >= str.size()) {
        return str.size();
        }
    return align_utf8(begin(str) + p, begin(str)) - begin(str);
    }

// returns smallest i such that list[i] > value.  value > list[i-1]
// Use a binary search since list is monotonic
template <typename T>
static size_t find_first_larger(const std::vector<T>& list, T value) {
    return (size_t)(std::upper_bound(list.begin(), list.end(), value) - list.begin());
    }

static size_t find_closest_x(const std::vector<SkRect>& bounds, float x, size_t b, size_t e) {
    if (b >= e) {
        return b;
        }
    SkASSERT(e <= bounds.size());
    size_t best_index = b;
    float best_diff = ::fabsf(bounds[best_index].x() - x);
    for (size_t i = b + 1; i < e; ++i) {
        float d = ::fabsf(bounds[i].x() - x);
        if (d < best_diff) {
            best_diff = d;
            best_index = i;
            }
        }
    return best_index;
    }

Editor::TextPosition SkPlainTextEditor::Editor::move(Editor::Movement move, Editor::TextPosition pos) const {
    if (fParas.empty()) {
        return { 0, 0 };
        }
    // First thing: fix possible bad input values.
    if (pos.fParagraphIndex >= fParas.size()) {
        pos.fParagraphIndex = fParas.size() - 1;
        pos.fTextByteIndex = fParas[pos.fParagraphIndex].fText.size();
        }
    else {
        pos.fTextByteIndex = align_column(fParas[pos.fParagraphIndex].fText, pos.fTextByteIndex);
        }

    switch (move) {
        case Editor::Movement::kNowhere:
            break;
        case Editor::Movement::kLeft:
            if (0 == pos.fTextByteIndex) {
                if (pos.fParagraphIndex > 0) {
                    --pos.fParagraphIndex;
                    pos.fTextByteIndex = fParas[pos.fParagraphIndex].fText.size();
                    }
                }
            else {
                const auto& str = fParas[pos.fParagraphIndex].fText;
                pos.fTextByteIndex =
                    prev_utf8(begin(str) + pos.fTextByteIndex, begin(str)) - begin(str);
                }
            break;
        case Editor::Movement::kRight:
            if (fParas[pos.fParagraphIndex].fText.size() == pos.fTextByteIndex) {
                if (pos.fParagraphIndex + 1 < fParas.size()) {
                    ++pos.fParagraphIndex;
                    pos.fTextByteIndex = 0;
                    }
                }
            else {
                const auto& str = fParas[pos.fParagraphIndex].fText;
                pos.fTextByteIndex =
                    next_utf8(begin(str) + pos.fTextByteIndex, end(str)) - begin(str);
                }
            break;
        case Editor::Movement::kHome:
            {
            const std::vector<size_t>& list = fParas[pos.fParagraphIndex].fLineEndOffsets;
            size_t f = find_first_larger(list, pos.fTextByteIndex);
            pos.fTextByteIndex = f > 0 ? list[f - 1] : 0;
            }
            break;
        case Editor::Movement::kEnd:
            {
            const std::vector<size_t>& list = fParas[pos.fParagraphIndex].fLineEndOffsets;
            size_t f = find_first_larger(list, pos.fTextByteIndex);
            if (f < list.size()) {
                pos.fTextByteIndex = list[f] > 0 ? list[f] - 1 : 0;
                }
            else {
                pos.fTextByteIndex = fParas[pos.fParagraphIndex].fText.size();
                }
            }
            break;
        case Editor::Movement::kUp:
            {
            SkASSERT(pos.fTextByteIndex < fParas[pos.fParagraphIndex].fCursorPos.size());
            float x = fParas[pos.fParagraphIndex].fCursorPos[pos.fTextByteIndex].left();
            const std::vector<size_t>& list = fParas[pos.fParagraphIndex].fLineEndOffsets;
            size_t f = find_first_larger(list, pos.fTextByteIndex);
            // list[f] > value.  value > list[f-1]
            if (f > 0) {
                // not the first line in paragraph.
                pos.fTextByteIndex = find_closest_x(fParas[pos.fParagraphIndex].fCursorPos, x,
                                                    (f == 1) ? 0 : list[f - 2],
                                                    list[f - 1]);
                }
            else if (pos.fParagraphIndex > 0) {
                --pos.fParagraphIndex;
                const auto& newLine = fParas[pos.fParagraphIndex];
                size_t r = newLine.fLineEndOffsets.size();
                if (r > 0) {
                    pos.fTextByteIndex = find_closest_x(newLine.fCursorPos, x,
                                                        newLine.fLineEndOffsets[r - 1],
                                                        newLine.fCursorPos.size());
                    }
                else {
                    pos.fTextByteIndex = find_closest_x(newLine.fCursorPos, x, 0,
                                                        newLine.fCursorPos.size());
                    }
                }
            pos.fTextByteIndex =
                align_column(fParas[pos.fParagraphIndex].fText, pos.fTextByteIndex);
            }
            break;
        case Editor::Movement::kDown:
            {
            const std::vector<size_t>& list = fParas[pos.fParagraphIndex].fLineEndOffsets;
            float x = fParas[pos.fParagraphIndex].fCursorPos[pos.fTextByteIndex].left();

            size_t f = find_first_larger(list, pos.fTextByteIndex);
            if (f < list.size()) {
                const auto& bounds = fParas[pos.fParagraphIndex].fCursorPos;
                pos.fTextByteIndex = find_closest_x(bounds, x, list[f],
                                                    f + 1 < list.size() ? list[f + 1]
                                                    : bounds.size());
                }
            else if (pos.fParagraphIndex + 1 < fParas.size()) {
                ++pos.fParagraphIndex;
                const auto& bounds = fParas[pos.fParagraphIndex].fCursorPos;
                const std::vector<size_t>& l2 = fParas[pos.fParagraphIndex].fLineEndOffsets;
                pos.fTextByteIndex = find_closest_x(bounds, x, 0,
                                                    l2.size() > 0 ? l2[0] : bounds.size());
                }
            else {
                pos.fTextByteIndex = fParas[pos.fParagraphIndex].fText.size();
                }
            pos.fTextByteIndex =
                align_column(fParas[pos.fParagraphIndex].fText, pos.fTextByteIndex);
            }
            break;
        case Editor::Movement::kWordLeft:
            {
            if (pos.fTextByteIndex == 0) {
                pos = this->move(Editor::Movement::kLeft, pos);
                break;
                }
            const std::vector<bool>& words = fParas[pos.fParagraphIndex].fWordBoundaries;
            SkASSERT(words.size() == fParas[pos.fParagraphIndex].fText.size());
            do {
                --pos.fTextByteIndex;
                } while (pos.fTextByteIndex > 0 && !words[pos.fTextByteIndex]);
            }
            break;
        case Editor::Movement::kWordRight:
            {
            const StringSlice& text = fParas[pos.fParagraphIndex].fText;
            if (pos.fTextByteIndex == text.size()) {
                pos = this->move(Editor::Movement::kRight, pos);
                break;
                }
            const std::vector<bool>& words = fParas[pos.fParagraphIndex].fWordBoundaries;
            SkASSERT(words.size() == text.size());
            do {
                ++pos.fTextByteIndex;
                } while (pos.fTextByteIndex < text.size() && !words[pos.fTextByteIndex]);
            }
            break;

        }
    return pos;
    }

void SkPlainTextEditor::Editor::setSize(int width, int height, SDLSkiaWindow& window)
    {
    setWidth(rect.width());
    window.setInvalid();
    }

void SkPlainTextEditor::Editor::textInput(SDL_TextInputEvent& event, SDLSkiaWindow& window)
    {
    insert(fTextPos, event.text, strlen(event.text));
    moveCursor(Editor::Movement::kRight, false, window);
    }


void SkPlainTextEditor::Editor::_mouseDown(SDL_MouseButtonEvent& event, SDLSkiaWindow& window)
    {
    SkPoint point = SkPoint::Make(event.x, event.y);
    mapPixelsToPoints(&point, 1);
    moveTo(getPosition({ ((int)point.fX) - fMargin, ((int)point.fY) + fPos - fMargin }), true, window);
    }

void SkPlainTextEditor::Editor::_mouseUp(SDL_MouseButtonEvent& event, SDLSkiaWindow& window)
    {
    SkPoint point = SkPoint::Make(event.x, event.y);
    mapPixelsToPoints(&point, 1);
    moveTo(getPosition({ ((int)point.fX) - fMargin, ((int)point.fY) + fPos - fMargin }), false, window);
    }

bool SkPlainTextEditor::Editor::scroll(int delta, SDLSkiaWindow& window)
    {
    int maxPos = std::max<int>(0, fullTextHeight + 2 * fMargin - rect.height() / 2);
    int newpos = std::max<int>(0, std::min(fPos + delta, maxPos));
    if (newpos != fPos) {
        //moveCursor(Movement::)
        fPos = newpos;
        window.setInvalid();
        }
    return true;
    }

void SkPlainTextEditor::Editor::keyDown(SDL_KeyboardEvent& event, SDLSkiaWindow& window)
    {
    if (event.type == SDL_KEYDOWN)
        {
        bool shift = event.keysym.mod & KMOD_SHIFT;
        switch (event.keysym.sym)
            {
            case SDLK_UP: moveCursor(Editor::Movement::kUp, shift, window); break;
            case SDLK_DOWN: moveCursor(Editor::Movement::kDown, shift, window); break;
            case SDLK_LEFT: moveCursor(Editor::Movement::kLeft, shift, window); break;
            case SDLK_RIGHT: moveCursor(Editor::Movement::kRight, shift, window); break;
            case SDLK_HOME: moveCursor(Editor::Movement::kHome, shift, window); break;
            case SDLK_END: moveCursor(Editor::Movement::kEnd, shift, window); break;
            case SDLK_PAGEDOWN:
                scroll(rect.height() * 4 / 5, window);
                break;
            case SDLK_PAGEUP:
                scroll(-rect.height() * 4 / 5, window);
                break;
            case SDLK_DELETE:
                if (fMarkPos != Editor::TextPosition()) {
                    moveTo(remove(fMarkPos, fTextPos), false, window);
                    }
                else {
                    auto pos = move(Editor::Movement::kRight, fTextPos);
                    moveTo(remove(fTextPos, pos), false, window);
                    }
                window.setInvalid();
                break;
            case SDLK_BACKSPACE:
                if (fMarkPos != Editor::TextPosition()) {
                    moveTo(remove(fMarkPos, fTextPos), false, window);
                    }
                else {
                    auto pos = move(Editor::Movement::kLeft, fTextPos);
                    moveTo(remove(fTextPos, pos), false, window);
                    }
                window.setInvalid();
                break;
            case SDLK_RETURN:
                {
                char c = '\n';
                insert(fTextPos, &c, 1);
                moveCursor(Editor::Movement::kRight, false, window);
                break;
                }
            default:
                break;
            }
        }
    }

bool SkPlainTextEditor::Editor::moveCursor(SkPlainTextEditor::Editor::Movement m, bool shift, SDLSkiaWindow& window) 
    {
    return moveTo(move(m, fTextPos), shift, window);
    }

bool SkPlainTextEditor::Editor::moveTo(SkPlainTextEditor::Editor::TextPosition pos, bool shift, SDLSkiaWindow& window)
    {
    if (pos == fTextPos || pos == Editor::TextPosition()) {
        if (!shift) {
            fMarkPos = Editor::TextPosition();
            }
        return false;
        }
    if (shift != fShiftDown) {
        fMarkPos = shift ? fTextPos : Editor::TextPosition();
        fShiftDown = shift;
        }
    fTextPos = pos;

    // scroll if needed.
    SkIRect cursor = getLocation(fTextPos).roundOut();
    if (fPos < cursor.bottom() - rect.height() + 2 * fMargin) {
        fPos = cursor.bottom() - rect.height() + 2 * fMargin;
        }
    else if (cursor.top() < fPos) {
        fPos = cursor.top();
        }
    window.setInvalid();
    resetCursorBlink(window);
    return true;
    }

void SkPlainTextEditor::Editor::resetCursorBlink(SDLSkiaWindow& window)
    {
    startCursorTime = std::chrono::steady_clock::now();
    showCursor = true;
    }

void SkPlainTextEditor::Editor::drawMe(SDLSkiaWindow& window)
    {
    paint(&window.Canvas());
    }

void SkPlainTextEditor::Editor::onIdle(SDLSkiaWindow& window)
    {
    auto ellapsedTime = std::chrono::steady_clock::now() - startCursorTime;
    if (ellapsedTime > std::chrono::milliseconds(500))
        {
        showCursor = !showCursor;
        startCursorTime = std::chrono::steady_clock::now();
        window.setInvalid();
        }
    }

void SkPlainTextEditor::Editor::paint(SkCanvas* c) {
//from editorApp:
    c->clipRect({ 0, 0, (float)rect.width(), (float)rect.height()}); //TDDO: also set width
    c->translate(fMargin, (float)(fMargin - fPos));
    Editor::PaintOpts options;
    options.fCursor = fTextPos;
    if (fMarkPos != Editor::TextPosition()) {
        options.fSelectionBegin = fMarkPos;
        options.fSelectionEnd = fTextPos;
        }
#ifdef SK_EDITOR_DEBUG_OUT
    {
    Timer timer("shaping");
    fEditor.paint(nullptr, options);
    }
    Timer timer("painting");
#endif  // SK_EDITOR_DEBUG_OUT

//end from editorApp

    this->reshapeAll();
    if (!c) {
        return;
        }

    SkPaint selection = SkPaint(options.fSelectionColor);
    auto cmp = [](const Editor::TextPosition& u, const Editor::TextPosition& v) { return u < v; };
    for (TextPosition pos = std::min(options.fSelectionBegin, options.fSelectionEnd, cmp),
         end = std::max(options.fSelectionBegin, options.fSelectionEnd, cmp);
         pos < end;
         pos = this->move(Editor::Movement::kRight, pos))
        {
        const TextParagraph& l = fParas[pos.fParagraphIndex];
        c->drawRect(offset(l.fCursorPos[pos.fTextByteIndex], l.fOrigin), selection);
        }

    if (fParas.size() > 0) {
        c->drawRect(Editor::getLocation(options.fCursor), SkPaint(showCursor ? options.fCursorColor : SkColor4f::FromBytes_RGBA(SK_ColorTRANSPARENT)));
        }

    SkPaint foreground = SkPaint(options.fForegroundColor);
    for (const TextParagraph& line : fParas) {
        if (line.fBlob) {
            c->drawTextBlob(line.fBlob.get(), (SkScalar)line.fOrigin.x(), (SkScalar)line.fOrigin.y(), foreground);
            }
        }
    }

void SkPlainTextEditor::Editor::reshapeAll() {
    if (fNeedsReshape) {
        if (fParas.empty()) {
            fParas.push_back(TextParagraph());
        }
        float shape_width = (float)(rect.width()-fMargin);
        int i = 0;
        for (TextParagraph& line : fParas) {
            if (!line.fShaped) {
                ShapeResult result = Shape(line.fText.begin(), line.fText.size(),
                                           fFont, fLocale, shape_width);
                line.fBlob           = std::move(result.blob);
                line.fLineEndOffsets = std::move(result.lineBreakOffsets);
                line.fCursorPos      = std::move(result.glyphBounds);
                line.fWordBoundaries = std::move(result.wordBreaks);
                line.fHeight         = result.verticalAdvance;
                line.fShaped = true;
            }
            ++i;
        }
        int y = 0;
        for (TextParagraph& line : fParas) {
            line.fOrigin = {0, y};
            y += line.fHeight;
        }
        fullTextHeight = y;
        fNeedsReshape = false;
    }
}

