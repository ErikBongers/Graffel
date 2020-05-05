#include "../model/Document.h"
#include "Visual.h"

namespace idea
{
class Controller
    {
    public:
        idea::Document& doc;
        Controller(idea::Document& doc) : doc(doc) {}
        void nodeChanged(idea::Node& node);
        std::vector<Visual> visuals;

    };

}