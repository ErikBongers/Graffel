#pragma once
#include "../pch.h"
#include "core/UIElement.h"
#include "controls/texteditor/Editor.h"
#include "Visual.h"

using namespace idea;

class StickyNode : public UIElement, Visual
    {
    private:
        SkEd::Editor txtTitle;
        SkEd::Editor txtDscr;
        const int lineHeight = 30;
    public:
        Node* node;
        StickyNode(Document& doc, int id);
        virtual void _drawMe() override;


        // Inherited via Visual
        virtual void update() override;

    };