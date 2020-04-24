#include "EditorText.h"
#include "shape.h"

using namespace SkEd;

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

static inline bool is_utf8_continuation(char v) {
    return ((unsigned char)v & 0b11000000) ==
        0b10000000;
    }

static const char* align_utf8(const char* p, const char* begin) {
    while (p > begin && is_utf8_continuation(*p)) {
        --p;
        }
    return p;
    }

static const char* next_utf8(const char* p, const char* end) {
    if (p < end) {
        do {
            ++p;
            } while (p < end && is_utf8_continuation(*p));
        }
    return p;
    }

static const char* prev_utf8(const char* p, const char* begin) {
    return p > begin ? align_utf8(p - 1, begin) : begin;
    }

static inline const char* begin(const StringSlice& s) { return s.begin(); }
static inline const char* end(const StringSlice& s) { return s.end(); }

static size_t align_column(const StringSlice& str, size_t p) {
    if (p >= str.size()) {
        return str.size();
        }
    return align_utf8(begin(str) + p, begin(str)) - begin(str);
    }

static const StringSlice remove_newline(const char* str, size_t len) {
    return SkASSERT((str != nullptr) || (len == 0)),
        StringSlice(str, (len > 0 && str[len - 1] == '\n') ? len - 1 : len);
    }

static size_t count_char(const StringSlice& string, char value) {
    size_t count = 0;
    for (char c : string) { if (c == value) { ++count; } }
    return count;
    }

TextPosition EditorText::insert(TextPosition pos, const char* utf8Text, size_t byteLen) {
    if (!valid_utf8(utf8Text, byteLen) || 0 == byteLen) {
        return pos;
        }
    pos = this->move(Movement::kNowhere, pos);
    fNeedsReshape = true;
    if (pos.fParagraphIndex < fParas.size()) {
        fParas[pos.fParagraphIndex].fText.insert(pos.fTextByteIndex, utf8Text, byteLen);
        this->markDirty(&fParas[pos.fParagraphIndex]);
        }
    else {
        SkASSERT(pos.fParagraphIndex == fParas.size());
        SkASSERT(pos.fTextByteIndex == 0);
        fParas.push_back(EditorText::TextParagraph(StringSlice(utf8Text, byteLen)));
        }
    pos = TextPosition{ pos.fTextByteIndex + byteLen, pos.fParagraphIndex };
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

::TextPosition EditorText::remove(TextPosition pos1, TextPosition pos2) {
    pos1 = this->move(Movement::kNowhere, pos1);
    pos2 = this->move(Movement::kNowhere, pos2);
    auto cmp = [](const TextPosition& u, const TextPosition& v) { return u < v; };
    TextPosition start = std::min(pos1, pos2, cmp);
    TextPosition end = std::max(pos1, pos2, cmp);
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

TextPosition EditorText::move(Movement move, TextPosition pos) const {
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
        case Movement::kNowhere:
            break;
        case Movement::kLeft:
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
        case Movement::kRight:
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
        case Movement::kHome:
            {
            const std::vector<size_t>& list = fParas[pos.fParagraphIndex].fLineEndOffsets;
            size_t f = find_first_larger(list, pos.fTextByteIndex);
            pos.fTextByteIndex = f > 0 ? list[f - 1] : 0;
            }
            break;
        case Movement::kEnd:
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
        case Movement::kUp:
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
        case Movement::kDown:
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
        case Movement::kWordLeft:
            {
            if (pos.fTextByteIndex == 0) {
                pos = this->move(Movement::kLeft, pos);
                break;
                }
            const std::vector<bool>& words = fParas[pos.fParagraphIndex].fWordBoundaries;
            SkASSERT(words.size() == fParas[pos.fParagraphIndex].fText.size());
            do {
                --pos.fTextByteIndex;
                } while (pos.fTextByteIndex > 0 && !words[pos.fTextByteIndex]);
            }
            break;
        case Movement::kWordRight:
            {
            const StringSlice& text = fParas[pos.fParagraphIndex].fText;
            if (pos.fTextByteIndex == text.size()) {
                pos = this->move(Movement::kRight, pos);
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

    void EditorText::markDirty(TextParagraph* line) {
        line->fBlob = nullptr;
        line->fShaped = false;
        line->fWordBoundaries = std::vector<bool>();
        }

    void EditorText::setFont(SkFont font) {
        if (font != fFont) {
            fFont = std::move(font);
            fNeedsReshape = true;
            for (auto& l : fParas) { this->markDirty(&l); }
            }
        }
    
    static constexpr SkRect kUnsetRect{ -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX };
    static SkPoint to_point(SkIPoint p) { return { (float)p.x(), (float)p.y() }; }

    TextPosition EditorText::getPosition(SkPoint xy) {
        TextPosition approximatePosition;
        this->reshapeAll();
        for (size_t j = 0; j < fParas.size(); ++j) {
            const TextParagraph& line = fParas[j];
            SkIRect lineRect = { 0,
                                line.fOrigin.y(),
                                (int)width,
                                j + 1 < fParas.size() ? fParas[j + 1].fOrigin.y() : INT_MAX };
            if (const SkTextBlob* b = line.fBlob.get()) {
                SkIRect r = b->bounds().roundOut();
                r.offset(line.fOrigin);
                lineRect.join(r);
                }
            if (!lineRect.contains((int)xy.x(), (int)xy.y())) {
                continue;
                }
            SkPoint pt = xy - to_point(line.fOrigin);
            const std::vector<SkRect>& pos = line.fCursorPos;
            for (size_t i = 0; i < pos.size(); ++i) {
                if (pos[i] != kUnsetRect && pos[i].contains(pt.x(), pt.y())) {
                    return TextPosition{ i, j };
                    }
                }
            approximatePosition = { xy.x() <= line.fOrigin.x() ? 0 : line.fText.size(), j };
            }
        return approximatePosition;
        }

    void EditorText::reshapeAll() {
        if (fNeedsReshape) {
            if (fParas.empty()) {
                fParas.push_back(TextParagraph());
                }
            float shape_width = (float)(width);
            int i = 0;
            for (TextParagraph& line : fParas) {
                if (!line.fShaped) {
                    ShapeResult result = Shape(line.fText.begin(), line.fText.size(),
                                               fFont, fLocale, shape_width);
                    line.fBlob = std::move(result.blob);
                    line.fLineEndOffsets = std::move(result.lineBreakOffsets);
                    line.fCursorPos = std::move(result.glyphBounds);
                    line.fWordBoundaries = std::move(result.wordBreaks);
                    line.fHeight = result.verticalAdvance;
                    line.fShaped = true;
                    }
                ++i;
                }
            int y = 0;
            for (TextParagraph& line : fParas) {
                line.fOrigin = { 0, y };
                y += line.fHeight;
                }
            fullTextHeight = (SkScalar)y;
            fNeedsReshape = false;
            }
        }

    static inline SkRect offset(SkRect r, SkIPoint p) { return r.makeOffset((float)p.x(), (float)p.y()); }

    SkRect EditorText::getCursorTextLocation(TextPosition cursor) {
        reshapeAll();
        cursor = this->move(Movement::kNowhere, cursor);
        if (fParas.size() > 0) {
            const TextParagraph& cLine = fParas[cursor.fParagraphIndex];
            SkRect pos = { 0, 0, 0, 0 };
            if (cursor.fTextByteIndex < cLine.fCursorPos.size()) {
                pos = cLine.fCursorPos[cursor.fTextByteIndex];
                }
            pos.fRight = pos.fLeft + 0.5f;
            pos.fLeft -= 1;
            return offset(pos, cLine.fOrigin);
            }
        return SkRect{ 0, 0, 0, 0 };
        }

    void EditorText::paint(SkCanvas& canvas)
        {
        reshapeAll();
        fCursor = fTextPos;

        //paint selection
        if (fMarkPos != TextPosition()) {
            fSelectionBegin = fMarkPos;
            fSelectionEnd = fTextPos;
            }
        SkPaint selection = SkPaint(fSelectionColor);
        auto cmp = [](const TextPosition& u, const TextPosition& v) { return u < v; };
        for (TextPosition pos = std::min(fSelectionBegin, fSelectionEnd, cmp),
             end = std::max(fSelectionBegin, fSelectionEnd, cmp);
             pos < end;
             pos = move(Movement::kRight, pos))
            {
            const TextParagraph& l = fParas[pos.fParagraphIndex];
            canvas.drawRect(offset(l.fCursorPos[pos.fTextByteIndex], l.fOrigin), selection);
            }
        
        //paint cursor
        if (fParas.size() > 0)//paras should never be empty???
            {
            if ((cursorBlinkOn && showCursor))
                {
                SkPaint cursorPaint(fCursorColor);
                cursorPaint.setAntiAlias(false);
                canvas.drawRect(getCursorTextLocation(fCursor), cursorPaint);
                }
            }
        
        //paint text
        SkPaint foreground = SkPaint(fForegroundColor);
        for (const TextParagraph& line : fParas) {
            if (line.fBlob) {
                canvas.drawTextBlob(line.fBlob.get(), (SkScalar)line.fOrigin.x(), (SkScalar)line.fOrigin.y(), foreground);
                }
            }

        }

    void EditorText::resetCursorBlink()
        {
        startCursorTime = std::chrono::steady_clock::now();
        cursorBlinkOn = true;
        }

    bool EditorText::onIdle()
        {
        auto ellapsedTime = std::chrono::steady_clock::now() - startCursorTime;
        if (ellapsedTime > std::chrono::milliseconds(500)) //TODO: make user defined
            {
            cursorBlinkOn = !cursorBlinkOn;
            startCursorTime = std::chrono::steady_clock::now();
            return true;
            }
        return false;
        }

    bool SkEd::EditorText::moveCursor(Movement m, bool shift)
        {
        return moveTo(move(m, fTextPos), shift);
        }

    bool SkEd::EditorText::moveTo(TextPosition pos, bool shift)
        {
        if (pos == fTextPos || pos == TextPosition()) {
            if (!shift) {
                fMarkPos = TextPosition();
                }
            return false;
            }
        if (shift != fShiftDown) {
            fMarkPos = shift ? fTextPos : TextPosition();
            fShiftDown = shift;
            }
        fTextPos = pos;
        resetCursorBlink();
        return true;
        }

