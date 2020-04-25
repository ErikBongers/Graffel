#pragma once
#include "..\pch.h"
#include <climits>
#include <utility>
#include <cstddef>
#include "EditorDoc.h"

namespace SkEd {

typedef std::function<void()> PDocChanged;

class EditorView
    {
    private:
        bool fNeedsReshape = false;
        SkScalar width = 0;
        SkScalar fullTextHeight = 0;
        void resetCursorBlink();

    public:
        struct ParagraphFormat {
            sk_sp<const SkTextBlob> fBlob;
            std::vector<SkRect> fCursorPos;
            std::vector<size_t> fLineEndOffsets;
            std::vector<bool> fWordBoundaries;
            SkIPoint fOrigin = { 0, 0 };
            int fHeight = 0;
            bool fShaped = false;
            };

        EditorDoc doc;
        SkFont fFont;
        const char* fLocale = "en";
        bool showCursor = false;
        std::chrono::time_point<std::chrono::steady_clock> startCursorTime; //TODO: add cursor blinktime user defined.
        bool cursorBlinkOn = false;
        SkColor4f fForegroundColor = { 1, 1, 1, 1 };
        SkColor4f fSelectionColor = { 0.729f, 0.827f, 0.988f, 1 };
        SkColor4f fCursorColor = { 1, 0, 0, 1 };

        
        EditorView();
        
        void onParagraphChanged(EditorDoc::Paragraph& para);
        int lineHeight(size_t index) const { return std::static_pointer_cast<ParagraphFormat>(doc.fParas[index].data)->fHeight; }
        void setFont(SkFont font);
        void setWidth(SkScalar w) {
            if (width != w) {
                width = w;
                fNeedsReshape = true;
                for (auto& l : doc.fParas) { onParagraphChanged(l); }
                }
            }
        SkScalar getFullTextHeight() { return fullTextHeight; }
        TextPosition getPosition(SkPoint);
        void reshapeAll();
        SkRect getTextLocation(TextPosition);
        void paint(SkCanvas& canvas);
        bool onIdle();
        TextPosition getPositionMoved(Movement m, TextPosition pos);
        bool moveCursor(Movement m, bool expandSelection);
        PDocChanged docChanged = nullptr;
        PCursorMoved cursorMoved = nullptr;

    };
}
