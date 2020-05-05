#include "StickyNote.h"

StickyNode::StickyNode(Document& doc, int id) :Visual(doc, id) {
    rect = SkRect::MakeWH(300, 300);
    txtTitle.rect = SkRect::MakeXYWH(3, 3, 300 - 3 - 3, lineHeight);
    txtDscr.rect = SkRect::MakeXYWH(3, 3 + lineHeight + 3, 300 - 3 - 3, 300 - 3 - 3 - 3 - lineHeight);
    backgroundColor = SkColorSetRGB(255, 255, 200);
    auto node= doc.nodes[id];
    node->title.maxLength = 30; //todo: put in Node?
    node->title.maxParagraphs = 1;
    node->dscr.maxLength = 200;
    node->dscr.maxParagraphs = 4;
    txtTitle.txt.attachDoc(&node->title);
    txtDscr.txt.attachDoc(&node->dscr);
    (*this) += txtTitle;
    (*this) += txtDscr;
    txtTitle.txt.fForegroundColor = { 0,0,0,1 };
    txtDscr.txt.fForegroundColor = { 0,0,0,1 };
    }

void StickyNode::_drawMe()
    {
    UIElement::_drawMe();
    SkPaint paint({ 0,0,0, 0.2f });
    paint.setStyle(SkPaint::Style::kStroke_Style);
    Canvas().drawRect(txtTitle.rect, paint);
    Canvas().drawRect(txtDscr.rect, paint);
    }

void StickyNode::update()
    {
    //what to do here???
    }
