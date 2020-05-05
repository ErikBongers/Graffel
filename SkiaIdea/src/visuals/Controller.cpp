#include "Controller.h"

void idea::Controller::nodeChanged(idea::Node& node)
    {
    //inform all views.
    for (Visual& v : visuals)
        if (v.id == node.id)
            v.update();
    }
