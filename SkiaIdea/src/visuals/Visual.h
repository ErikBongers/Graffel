#pragma once
#include "../model/Document.h"

class Visual
    {
    public:
        idea::Document doc;
        int id;
        Visual(idea::Document& doc, int id) : doc(doc), id(id) {}
        virtual void update() = 0;
    };

