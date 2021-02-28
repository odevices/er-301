/*
 * ConnectionQueue.h
 *
 *  Created on: 1 Sep 2016
 *      Author: clarkson
 */

#ifndef APP_TASKS_CONNECTIONQUEUE_H_
#define APP_TASKS_CONNECTIONQUEUE_H_

#include <od/tasks/Task.h>
#include <od/objects/Object.h>
#include <hal/concurrency/Mutex.h>
#include <utility>

namespace od
{

    class ConnectionQueue : public Task
    {
    public:
        ConnectionQueue();
        virtual ~ConnectionQueue();

#ifndef SWIGLUA
        virtual void process(float *inputs, float *outputs);
#endif

        void pushConnection(Outlet *outlet, Inlet *inlet, Object *object = nullptr);
        void pushDisconnection(Outlet *outlet, Object *object = nullptr);
        void pushDisconnection(Inlet *inlet, Object *object = nullptr);
        void printState();

    private:
        struct Item
        {
            Item(Outlet *outlet, Inlet *inlet, Object *object) : outlet(outlet), inlet(inlet), object(object)
            {
            }
            ~Item()
            {
            }
            Outlet *outlet;
            Inlet *inlet;
            Object *object;
        };

        Mutex mMutex;
        std::vector<Item> mPending;
        std::vector<Object *> mObjects; // in processing order

        void schedule(Outlet *outlet, Inlet *inlet, Object *object);
        void addObject(Object *object);
        void removeObject(Object *object);
    };

} /* namespace od */

#endif /* APP_TASKS_CONNECTIONQUEUE_H_ */
