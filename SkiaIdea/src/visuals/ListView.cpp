#include "ListView.h"

void ListView::_resizeContent()
    {
    SkScalar width = 0;
    SkScalar height = 0;
    for (auto child : children)
        {
        width = std::max(width, child->rect.width());
        child->rect.offsetTo(0, height);
        height += child->rect.height();
        }
    rect.setXYWH(rect.fLeft, rect.fTop, width, height);
    }
