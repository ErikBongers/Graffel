#pragma once
#include "../pch.h"
#include "core/UIElement.h"
#include "../model/Document.h"
#include "controls/texteditor/Editor.h"

using namespace idea;

class StickyNode : public UIElement
    {
    private:
        SkEd::Editor txtTitle;
        SkEd::Editor txtDscr;
        SkEd::EditorDoc docTitle;
        SkEd::EditorDoc docDscr;
        const int lineHeight = 30;
    public:
        Node* node;
        StickyNode() {
            rect = SkRect::MakeWH(300, 300);
            txtTitle.rect = SkRect::MakeXYWH(3, 3, 300 - 3 - 3, lineHeight);
            txtDscr.rect = SkRect::MakeXYWH(3, 3 + lineHeight + 3, 300 - 3 - 3, 300 - 3 - 3 - 3 - lineHeight);
            backgroundColor = SkColorSetRGB(255, 255, 200);
            docTitle.insert(u8"d€n titel...");
            docTitle.maxLength = 30;
            docTitle.maxParagraphs = 1;
            docDscr.insert("dscr.......\n........!");
            docDscr.maxLength = 200;
            docDscr.maxParagraphs = 4;
            txtTitle.txt.attachDoc(&docTitle);
            txtDscr.txt.attachDoc(&docDscr);
            (*this) += txtTitle;
            (*this) += txtDscr;
            txtTitle.txt.fForegroundColor = {0,0,0,1};
            txtDscr.txt.fForegroundColor = {0,0,0,1};
            }
        virtual void _drawMe() override
            {
            UIElement::_drawMe();
            SkPaint paint({ 0,0,0, 0.2f });
            paint.setStyle(SkPaint::Style::kStroke_Style);
            Canvas().drawRect(txtTitle.rect, paint);
            Canvas().drawRect(txtDscr.rect, paint);
            }

    };