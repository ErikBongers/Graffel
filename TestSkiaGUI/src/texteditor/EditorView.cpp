#include "EditorView.h"
#include "shape.h"

using namespace SkEd;

EditorView::EditorView()
    {
    doc.cursorMoved = [this]() {
        if(this->cursorMoved)
            this->cursorMoved();
        };
    doc.paragraphChanged = [this](EditorDoc::Paragraph& para) {
        this->onParagraphChanged(para);
        };
    }

void EditorView::onParagraphChanged(EditorDoc::Paragraph& para)
    {
    if (!para.data)
        para.data = std::make_shared<ParagraphFormat>();
    auto pf = std::static_pointer_cast<ParagraphFormat>(para.data);
    //ParagraphFormat* pf = nullptr;
    pf->fBlob = nullptr;
    pf->fShaped = false;
    pf->fWordBoundaries = std::vector<bool>();
    fNeedsReshape = true;
    if (docChanged)
        docChanged();
    }

void EditorView::setFont(SkFont font) {
    if (font != fFont) {
        fFont = std::move(font);
        fNeedsReshape = true;
        for (auto& l : doc.fParas) { onParagraphChanged(l); }
        }
    }
    
static constexpr SkRect kUnsetRect{ -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX };
static SkPoint to_point(SkIPoint p) { return { (float)p.x(), (float)p.y() }; }

TextPosition EditorView::getPosition(SkPoint xy) {
    TextPosition approximatePosition;
    this->reshapeAll();
    for (size_t j = 0; j < doc.fParas.size(); ++j) {
        EditorDoc::Paragraph& para = doc.fParas[j];
        auto pf = std::static_pointer_cast<ParagraphFormat>(para.data);
        SkIRect lineRect = { 0,
                            pf->fOrigin.y(),
                            (int)width,
                            j + 1 < doc.fParas.size() ? std::static_pointer_cast<ParagraphFormat>(doc.fParas[j + 1].data)->fOrigin.y() : INT_MAX };
        if (const SkTextBlob* b = pf->fBlob.get()) {
            SkIRect r = b->bounds().roundOut();
            r.offset(pf->fOrigin);
            lineRect.join(r);
            }
        if (!lineRect.contains((int)xy.x(), (int)xy.y())) {
            continue;
            }
        SkPoint pt = xy - to_point(pf->fOrigin);
        const std::vector<SkRect>& pos = pf->fCursorPos;
        for (size_t i = 0; i < pos.size(); ++i) {
            if (pos[i] != kUnsetRect && pos[i].contains(pt.x(), pt.y())) {
                return TextPosition{ i, j };
                }
            }
        approximatePosition = { xy.x() <= pf->fOrigin.x() ? 0 : para.fText.size(), j };
        }
    return approximatePosition;
    }

void EditorView::reshapeAll() {
    if (fNeedsReshape) {
        if (doc.fParas.empty()) {
            doc.fParas.push_back(EditorDoc::Paragraph()); //todo: get rid of this -> EditorDoc
            }
        float shape_width = (float)(width);
        int i = 0;
        for (EditorDoc::Paragraph& para : doc.fParas) {
            if (!para.data)
                para.data = std::make_shared<ParagraphFormat>();
            auto pf = std::static_pointer_cast<ParagraphFormat>(para.data);
            if (!pf->fShaped) {
                ShapeResult result = Shape(para.fText.begin(), para.fText.size(),
                                            fFont, fLocale, shape_width);
                pf->fBlob = std::move(result.blob);
                pf->fLineEndOffsets = std::move(result.lineBreakOffsets);
                pf->fCursorPos = std::move(result.glyphBounds);
                pf->fWordBoundaries = std::move(result.wordBreaks);
                pf->fHeight = result.verticalAdvance;
                pf->fShaped = true;
                }
            ++i;
            }
        int y = 0;
        for (EditorDoc::Paragraph& para : doc.fParas) {
            auto pf = std::static_pointer_cast<ParagraphFormat>(para.data);
            pf->fOrigin = { 0, y };
            y += pf->fHeight;
            }
        fullTextHeight = (SkScalar)y;
        fNeedsReshape = false;
        }
    }

static inline SkRect offset(SkRect r, SkIPoint p) { return r.makeOffset((float)p.x(), (float)p.y()); }

SkRect EditorView::getTextLocation(TextPosition cursor) {
    reshapeAll();
    cursor = this->getPositionMoved(Movement::kNowhere, cursor);
    if (doc.fParas.size() > 0) {
        const EditorDoc::Paragraph& para = doc.fParas[cursor.fParagraphIndex];
        auto pf = std::static_pointer_cast<ParagraphFormat>(para.data);
        SkRect pos = { 0, 0, 0, 0 };
        if (cursor.fTextByteIndex < pf->fCursorPos.size()) {
            pos = pf->fCursorPos[cursor.fTextByteIndex];
            }
        pos.fRight = pos.fLeft + 0.5f;
        pos.fLeft -= 1;
        return offset(pos, pf->fOrigin);
        }
    return SkRect{ 0, 0, 0, 0 };
    }

void EditorView::paint(SkCanvas& canvas)
    {
    reshapeAll();

    //paint selection
    TextPosition fSelectionBegin;
    TextPosition fSelectionEnd;
    if (doc.fMarkPos) {
        fSelectionBegin = doc.fMarkPos;
        fSelectionEnd = doc.fCursorPos;
        }
    SkPaint selection = SkPaint(fSelectionColor);
    auto cmp = [](const TextPosition& u, const TextPosition& v) { return u < v; };
    for (TextPosition pos = std::min(fSelectionBegin, fSelectionEnd, cmp),
            end = std::max(fSelectionBegin, fSelectionEnd, cmp);
            pos < end;
            pos = getPositionMoved(Movement::kRight, pos))
        {
        const EditorDoc::Paragraph& para = doc.fParas[pos.fParagraphIndex];
        auto pf = std::static_pointer_cast<ParagraphFormat>(para.data);

        canvas.drawRect(offset(pf->fCursorPos[pos.fTextByteIndex], pf->fOrigin), selection);
        }
        
    //paint cursor
    if (doc.fParas.size() > 0)//paras should never be empty???
        {
        if ((cursorBlinkOn && showCursor))
            {
            SkPaint cursorPaint(fCursorColor);
            cursorPaint.setAntiAlias(false);
            canvas.drawRect(getTextLocation(doc.fCursorPos), cursorPaint);
            }
        }
        
    //paint text
    SkPaint foreground = SkPaint(fForegroundColor);
    for (const EditorDoc::Paragraph& para : doc.fParas) {
        auto pf = std::static_pointer_cast<ParagraphFormat>(para.data);
        if (pf->fBlob) {
            canvas.drawTextBlob(pf->fBlob.get(), (SkScalar)pf->fOrigin.x(), (SkScalar)pf->fOrigin.y(), foreground);
            }
        }

    }

void EditorView::resetCursorBlink()
    {
    startCursorTime = std::chrono::steady_clock::now();
    cursorBlinkOn = true;
    }

bool EditorView::onIdle()
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

bool EditorView::moveCursor(Movement m, bool expandSelection)
    {
    return doc.setCursor(getPositionMoved(m, doc.fCursorPos), expandSelection);
    }

template <typename T>
static size_t find_first_larger(const std::vector<T>& list, T value) {
    return (size_t)(std::upper_bound(list.begin(), list.end(), value) - list.begin());
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
static inline const char* begin(const TextBuffer& s) { return s.begin(); }
static inline const char* end(const TextBuffer& s) { return s.end(); }

static const char* align_utf8(const char* p, const char* begin) {
    while (p > begin && is_utf8_continuation(*p)) {
        --p;
        }
    return p;
    }

static size_t align_column(const TextBuffer& str, size_t p) {
    if (p >= str.size()) {
        return str.size();
        }
    return align_utf8(begin(str) + p, begin(str)) - begin(str);
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

static const char* prev_utf8(const char* p, const char* begin) {
    return p > begin ? align_utf8(p - 1, begin) : begin;
    }

TextPosition EditorView::getPositionMoved(Movement m, TextPosition pos)
    {
    if (doc.fParas.empty()) {
        return { 0, 0 };
        }
    // First thing: fix possible bad input values.
    if (pos.fParagraphIndex >= doc.fParas.size()) {
        pos.fParagraphIndex = doc.fParas.size() - 1;
        pos.fTextByteIndex = doc.fParas[pos.fParagraphIndex].fText.size();
        }
    else {
        pos.fTextByteIndex = align_column(doc.fParas[pos.fParagraphIndex].fText, pos.fTextByteIndex);
        }

    switch (m) {
        case Movement::kNowhere:
            break;
        case Movement::kLeft:
            if (0 == pos.fTextByteIndex) {
                if (pos.fParagraphIndex > 0) {
                    --pos.fParagraphIndex;
                    pos.fTextByteIndex = doc.fParas[pos.fParagraphIndex].fText.size();
                    }
                }
            else {
                const auto& str = doc.fParas[pos.fParagraphIndex].fText;
                pos.fTextByteIndex =
                    prev_utf8(begin(str) + pos.fTextByteIndex, begin(str)) - begin(str);
                }
            break;
        case Movement::kRight:
            if (doc.fParas[pos.fParagraphIndex].fText.size() == pos.fTextByteIndex) {
                if (pos.fParagraphIndex + 1 < doc.fParas.size()) {
                    ++pos.fParagraphIndex;
                    pos.fTextByteIndex = 0;
                    }
                }
            else {
                const auto& str = doc.fParas[pos.fParagraphIndex].fText;
                pos.fTextByteIndex =
                    next_utf8(begin(str) + pos.fTextByteIndex, end(str)) - begin(str);
                }
            break;
        case Movement::kHome:
            {
            const std::vector<size_t>& list = std::static_pointer_cast<ParagraphFormat>(doc.fParas[pos.fParagraphIndex].data)->fLineEndOffsets;
            size_t f = find_first_larger(list, pos.fTextByteIndex);
            pos.fTextByteIndex = f > 0 ? list[f - 1] : 0;
            }
            break;
        case Movement::kEnd:
            {
            const std::vector<size_t>& list = std::static_pointer_cast<ParagraphFormat>(doc.fParas[pos.fParagraphIndex].data)->fLineEndOffsets;
            size_t f = find_first_larger(list, pos.fTextByteIndex);
            if (f < list.size()) {
                pos.fTextByteIndex = list[f] > 0 ? list[f] - 1 : 0;
                }
            else {
                pos.fTextByteIndex = doc.fParas[pos.fParagraphIndex].fText.size();
                }
            }
            break;
        case Movement::kUp:
            {
            SkASSERT(pos.fTextByteIndex < std::static_pointer_cast<ParagraphFormat>(doc.fParas[pos.fParagraphIndex].data)->fCursorPos.size());
            float x = std::static_pointer_cast<ParagraphFormat>(doc.fParas[pos.fParagraphIndex].data)->fCursorPos[pos.fTextByteIndex].left();
            const std::vector<size_t>& list = std::static_pointer_cast<ParagraphFormat>(doc.fParas[pos.fParagraphIndex].data)->fLineEndOffsets;
            size_t f = find_first_larger(list, pos.fTextByteIndex);
            // list[f] > value.  value > list[f-1]
            if (f > 0) {
                // not the first line in paragraph.
                pos.fTextByteIndex = find_closest_x(std::static_pointer_cast<ParagraphFormat>(doc.fParas[pos.fParagraphIndex].data)->fCursorPos, x,
                                                    (f == 1) ? 0 : list[f - 2],
                                                    list[f - 1]);
                }
            else if (pos.fParagraphIndex > 0) {
                --pos.fParagraphIndex;
                const auto& newPara = doc.fParas[pos.fParagraphIndex];
                auto pf = std::static_pointer_cast<ParagraphFormat>(newPara.data);
                size_t r = pf->fLineEndOffsets.size();
                if (r > 0) {
                    pos.fTextByteIndex = find_closest_x(pf->fCursorPos, x,
                                                        pf->fLineEndOffsets[r - 1],
                                                        pf->fCursorPos.size());
                    }
                else {
                    pos.fTextByteIndex = find_closest_x(pf->fCursorPos, x, 0,
                                                        pf->fCursorPos.size());
                    }
                }
            pos.fTextByteIndex =
                align_column(doc.fParas[pos.fParagraphIndex].fText, pos.fTextByteIndex);
            }
            break;
        case Movement::kDown:
            {
            const std::vector<size_t>& list = std::static_pointer_cast<ParagraphFormat>(doc.fParas[pos.fParagraphIndex].data)->fLineEndOffsets;
            float x = std::static_pointer_cast<ParagraphFormat>(doc.fParas[pos.fParagraphIndex].data)->fCursorPos[pos.fTextByteIndex].left();

            size_t f = find_first_larger(list, pos.fTextByteIndex);
            if (f < list.size()) {
                const auto& bounds = std::static_pointer_cast<ParagraphFormat>(doc.fParas[pos.fParagraphIndex].data)->fCursorPos;
                pos.fTextByteIndex = find_closest_x(bounds, x, list[f],
                                                    f + 1 < list.size() ? list[f + 1]
                                                    : bounds.size());
                }
            else if (pos.fParagraphIndex + 1 < doc.fParas.size()) {
                ++pos.fParagraphIndex;
                const auto& bounds = std::static_pointer_cast<ParagraphFormat>(doc.fParas[pos.fParagraphIndex].data)->fCursorPos;
                const std::vector<size_t>& l2 = std::static_pointer_cast<ParagraphFormat>(doc.fParas[pos.fParagraphIndex].data)->fLineEndOffsets;
                pos.fTextByteIndex = find_closest_x(bounds, x, 0,
                                                    l2.size() > 0 ? l2[0] : bounds.size());
                }
            else {
                pos.fTextByteIndex = doc.fParas[pos.fParagraphIndex].fText.size();
                }
            pos.fTextByteIndex =
                align_column(doc.fParas[pos.fParagraphIndex].fText, pos.fTextByteIndex);
            }
            break;
        case Movement::kWordLeft:
            {
            if (pos.fTextByteIndex == 0) {
                pos = getPositionMoved(Movement::kLeft, pos);
                break;
                }
            const std::vector<bool>& words = std::static_pointer_cast<ParagraphFormat>(doc.fParas[pos.fParagraphIndex].data)->fWordBoundaries;
            SkASSERT(words.size() == doc.fParas[pos.fParagraphIndex].fText.size());
            do {
                --pos.fTextByteIndex;
                } while (pos.fTextByteIndex > 0 && !words[pos.fTextByteIndex]);
            }
            break;
        case Movement::kWordRight:
            {
            const TextBuffer& text = doc.fParas[pos.fParagraphIndex].fText;
            if (pos.fTextByteIndex == text.size()) {
                pos = getPositionMoved(Movement::kRight, pos);
                break;
                }
            const std::vector<bool>& words = std::static_pointer_cast<ParagraphFormat>(doc.fParas[pos.fParagraphIndex].data)->fWordBoundaries;
            SkASSERT(words.size() == text.size());
            do {
                ++pos.fTextByteIndex;
                } while (pos.fTextByteIndex < text.size() && !words[pos.fTextByteIndex]);
            }
            break;

        }
    return pos;
    }
