#pragma once
#include "..\..\pch.h"
#include <climits>
#include <utility>
#include <cstddef>
#include "EditorDoc.h"
#include "core/UIElement.h"

namespace SkEd {

typedef std::function<void()> PDocChanged;

class EditorView;
class EditorUserData
    {
    private:
        int viewerCount = 0;
        std::vector<EditorView*> views;
    public:
        int getNewId() { return viewerCount++; }
        void registerViewWithDoc(EditorView* view, int userDataIndex);

        EditorUserData(EditorDoc& doc);
    };

class EditorView
    {
    private:
        bool fNeedsReshape = false;
        SkScalar width = 0;
        SkScalar fullTextHeight = 0;
        void resetCursorBlink();
        int userDataIndex = -1;
        UIElement* uiElement = nullptr;
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

        class ParaData
            {
            private:
                std::vector<std::shared_ptr<ParagraphFormat>> formats;
            public:
                ParagraphFormat& operator[](std::size_t i) 
                    { 
                    if (i >= formats.size())
                        {
                        formats.resize(i + 1);
                        formats[i] = std::make_shared<ParagraphFormat>();
                        }
                    return *formats[i]; 
                    }
            };

        EditorDoc* doc;
        SkFont fFont;
        const char* fLocale = "en"; //TODO: should this not be in Doc?
        bool showCursor = false;
        std::chrono::time_point<std::chrono::steady_clock> startCursorTime; //TODO: add cursor blinktime user defined.
        bool cursorBlinkOn = false;
        SkColor4f fForegroundColor = { 1, 1, 1, 1 };
        SkColor4f fSelectionColor = { 0.729f, 0.827f, 0.988f, 1 };
        SkColor4f fCursorColor = { 1, 0, 0, 1 };
        
        void attachDoc(EditorDoc* doc);
        ParaData& formats(EditorDoc::Paragraph& para) 
            { 
            if (!para.data)
                para.data = std::make_shared<ParaData>();
            return *std::static_pointer_cast<ParaData>(para.data); 
            }
        ParagraphFormat& format(EditorDoc::Paragraph& para) 
            { 
            return formats(para)[userDataIndex]; 
            }
        void onParagraphChanged(EditorDoc::Paragraph& para);
        //int lineHeight(size_t index) const { return std::static_pointer_cast<ParagraphFormat>(doc.fParas[index].data)->fHeight; }
        void setFont(SkFont font);
        void setWidth(SkScalar w) {
            if (width != w) {
                width = w;
                fNeedsReshape = true;
                if(doc)
                    for (auto& para : doc->fParas) { format(para).fShaped = false; }
                }
            }
        SkScalar getFullTextHeight() { return fullTextHeight; }
        TextPosition getPosition(SkPoint);
        void reshapeAll();
        SkRect getTextLocation(TextPosition);
        void paint(SkCanvas& canvas);
        bool onIdle();
        TextPosition getPositionMoved(Movement m, TextPosition pos);
        PDocChanged docChanged = nullptr;
        PCursorMoved cursorMoved = nullptr;
        friend class EditorUserData;
        friend class Editor; //is this ok?
    };
}

