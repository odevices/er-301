#pragma once

#include <od/objects/Object.h>
#include <vector>

namespace od
{

    class GraphCompiler
    {
    public:
        GraphCompiler();
        virtual ~GraphCompiler();

        bool compile(const std::vector<Object *> &graph,
                     std::vector<Object *> &order);
        std::vector<Object *> mRemaining;

    private:
        void topographicalSort(const std::vector<Object *> &graph,
                               std::vector<Object *> &order,
                               std::vector<Object *> &remaining);

        void orientGraph(const std::vector<Object *> &graph);
        void orientGraphFromInputs(const std::vector<Object *> &graph,
                                   const std::vector<Object *> &inputs);
        std::vector<Object *>::iterator breakCycle(std::vector<Object *> &graph);
        Object *getObjectFromPort(Port *port, const std::vector<Object *> &graph);
        void forwardWalk(Object *object, int distance,
                         const std::vector<Object *> &graph);

        bool mOriented = false;
    };

} /* namespace od */
