#include <od/units/GraphCompiler.h>
#include <hal/log.h>
#include <set>
#include <algorithm>

namespace od
{

    GraphCompiler::GraphCompiler()
    {
        // TODO Auto-generated constructor stub
    }

    GraphCompiler::~GraphCompiler()
    {
        // TODO Auto-generated destructor stub
    }

    static inline bool isExternal(Port *port, const std::vector<Object *> &graph)
    {
        return std::find(graph.begin(), graph.end(), port->owner()) == graph.end();
    }

    bool GraphCompiler::compile(const std::vector<Object *> &graph,
                                std::vector<Object *> &order)
    {
        mOriented = false;
        mRemaining.clear();

        if (graph.size() > 0)
        {
            mRemaining.reserve(graph.size());

            // First, compile each object individually.
            for (Object *o : graph)
            {
                o->compile();
            }

            // Second, sort the objects topographically
            topographicalSort(graph, order, mRemaining);
        }

        return mRemaining.size() == 0;
    }

    void GraphCompiler::topographicalSort(const std::vector<Object *> &graph,
                                          std::vector<Object *> &order,
                                          std::vector<Object *> &remaining)
    {
        std::set<Outlet *> processed;
        std::vector<Object *> delayed;
        std::vector<Object *>::iterator iter;

        // Copy graph to the list of remaining objects.
        for (Object *o : graph)
        {
            remaining.push_back(o);
        }

        while (remaining.size() > 0)
        {
            bool any = false;
            iter = remaining.begin();
            while (iter != remaining.end())
            {
                Object *o = *iter;
                // If all inputs are processed then process this object next.
                bool all = true;
                for (Inlet *port : o->mInputs)
                {
                    // connected?
                    if (port->mInwardConnection == NULL)
                        continue;
                    // connected to external object
                    if (isExternal(port->mInwardConnection, graph))
                        continue;
                    // already processed?
                    if (processed.find(port->mInwardConnection) == processed.end())
                    {
                        all = false;
                        break;
                    }
                }
                if (all)
                {
                    // put all the outputs of this object in the processed set
                    for (Outlet *port : o->mOutputs)
                    {
                        processed.insert(port);
                    }
                    // place the object in the processing order
                    order.push_back(o);
                    iter = remaining.erase(iter);
                    any = true;
                }
                else
                {
                    iter++;
                }
            }
            if (!any)
            {
                // no objects were able to be placed in the processing list
                if (!mOriented)
                {
                    mOriented = true;
                    orientGraph(graph);
                }
                iter = breakCycle(remaining);
                if (iter != remaining.end())
                {
                    Object *o = *iter;
                    for (Outlet *port : o->mOutputs)
                    {
                        processed.insert(port);
                    }
                    // place the object in the list of delayed objects
                    delayed.push_back(o);
                    remaining.erase(iter);
                }
                else
                {
                    logError("Failed to find a cycle to break.  This should never happen.");
                    break;
                }
            }
        }

        if (remaining.size() > 0)
        {
            logError("We have a problem that cannot be dealt with by recursive compiling.");
        }
        else if (delayed.size() > 0)
        {
            topographicalSort(delayed, order, remaining);
        }
    }

    void GraphCompiler::forwardWalk(Object *object, int distance,
                                    const std::vector<Object *> &graph)
    {
        object->mMarked = true;
        for (Outlet *outlet : object->mOutputs)
        {
            for (Inlet *inlet : outlet->mOutwardConnections)
            {
                Object *neighbor = getObjectFromPort(inlet, graph);
                if (neighbor)
                {
                    if (distance < neighbor->mMinDistance)
                    {
                        neighbor->mMinDistance = distance;
                    }
                    if (!neighbor->mMarked)
                    {
                        forwardWalk(neighbor, distance + 1, graph);
                    }
                }
            }
        }
    }

#define MAXDIST 999999

    void GraphCompiler::orientGraphFromInputs(const std::vector<Object *> &graph,
                                              const std::vector<Object *> &inputs)
    {
        // S(o) := minimum # of hops from any input object.
        for (Object *o : graph)
        {
            o->mMinDistance = MAXDIST;
        }

        for (Object *input : inputs)
        {
            input->mMinDistance = 0;
            for (Object *o : graph)
            {
                o->mMarked = false;
            }
            forwardWalk(input, 1, graph);
        }
    }

    Object *GraphCompiler::getObjectFromPort(Port *port,
                                             const std::vector<Object *> &graph)
    {
        auto iter = std::find(graph.begin(), graph.end(), port->owner());
        if (iter == graph.end())
        {
            return 0;
        }
        else
        {
            return *iter;
        }
    }

    std::vector<Object *>::iterator GraphCompiler::breakCycle(
        std::vector<Object *> &graph)
    {
        if (graph.size() == 0)
        {
            graph.end();
        }

        // This routine assumes the graph has already been oriented.

        // Find the object that is far from inputs but outputs to another object that is close to the inputs.
        // Distance to input goes down and distance to output goes up.
        std::vector<Object *>::iterator candidate = graph.begin();
        std::vector<Object *>::iterator iter = graph.begin();
        int maxFeedbackScore = -1;
        while (iter != graph.end())
        {
            Object *object = *iter;
            if (object->mMinDistance < MAXDIST)
            {
                for (Outlet *outlet : object->mOutputs)
                {
                    for (Inlet *inlet : outlet->mOutwardConnections)
                    {
                        Object *neighbor = getObjectFromPort(inlet, graph);
                        if (neighbor)
                        {
                            int feedbackScore = object->mMinDistance - neighbor->mMinDistance;
                            if (feedbackScore > maxFeedbackScore)
                            {
                                maxFeedbackScore = feedbackScore;
                                candidate = iter;
                            }
                        }
                    }
                }
            }
            iter++;
        }

        return candidate;
    }

    void GraphCompiler::orientGraph(const std::vector<Object *> &graph)
    {
        std::vector<Object *> inputs;
        for (Object *o : graph)
        {
            if (o->mIsInput)
            {
                inputs.push_back(o);
            }
        }

        orientGraphFromInputs(graph, inputs);
    }

} /* namespace od */
