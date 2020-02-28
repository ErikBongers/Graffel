#pragma once

namespace graffel
{

class Timeline;

class Block
    {
    private:
        SkFont font;
        SkPaint paint;
    public:
        Block();
        void draw(Timeline& timeline, SkCanvas& canvas);


    };

}

