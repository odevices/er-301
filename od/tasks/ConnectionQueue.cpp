/*
 * ConnectionQueue.cpp
 *
 *  Created on: 1 Sep 2016
 *      Author: clarkson
 */

#include <od/tasks/ConnectionQueue.h>
#include <hal/log.h>
#include <algorithm>

namespace od
{

    ConnectionQueue::ConnectionQueue() : Task("ConnectionQueue")
    {
    }

    ConnectionQueue::~ConnectionQueue()
    {
        for (Object *object : mObjects)
        {
            object->release();
        }
    }

    void ConnectionQueue::printState()
    {
        mMutex.enter();
        logInfo("Con Q: pending=%d objects=%d", mPending.size(),
                mObjects.size());
        mMutex.leave();
    }

    void ConnectionQueue::schedule(Outlet *outlet, Inlet *inlet, Object *object)
    {
        if (outlet != nullptr)
        {
            outlet->attach();
        }
        if (inlet != nullptr)
        {
            inlet->attach();
        }
        if (object != nullptr)
        {
            object->attach();
        }
        mPending.emplace_back(outlet, inlet, object);
    }

    void ConnectionQueue::pushConnection(Outlet *outlet, Inlet *inlet,
                                         Object *object)
    {
        mMutex.enter();
        schedule(outlet, inlet, object);
        if (object)
        {
            object->setScheduledFlag(true);
        }
        mMutex.leave();
    }

    void ConnectionQueue::pushDisconnection(Outlet *outlet, Object *object)
    {
        mMutex.enter();
        schedule(outlet, nullptr, object);
        if (object)
        {
            object->setScheduledFlag(false);
        }
        mMutex.leave();
    }

    void ConnectionQueue::pushDisconnection(Inlet *inlet, Object *object)
    {
        mMutex.enter();
        schedule(nullptr, inlet, object);
        if (object)
        {
            object->setScheduledFlag(false);
        }
        mMutex.leave();
    }

    void ConnectionQueue::process(float *inputs, float *outputs)
    {
        mMutex.enter();
        for (Object *object : mObjects)
        {
            object->updateParameters();
            object->process();
        }
        for (Item &item : mPending)
        {
            if (item.outlet == nullptr && item.inlet)
            {
                item.inlet->disconnect();
                item.inlet->release();
                if (item.object)
                {
                    removeObject(item.object);
                    item.object->release();
                }
            }
            else if (item.outlet && item.inlet == nullptr)
            {
                item.outlet->disconnect();
                item.outlet->release();
                if (item.object)
                {
                    removeObject(item.object);
                    item.object->release();
                }
            }
            else if (item.inlet && item.outlet)
            {
                item.inlet->connect(item.outlet);
                item.inlet->release();
                item.outlet->release();
                if (item.object)
                {
                    addObject(item.object);
                    item.object->release();
                }
            }
        }
        mPending.clear();
        mMutex.leave();
    }

    void ConnectionQueue::addObject(Object *object)
    {
        if (object == nullptr)
            return;
        std::vector<Object *>::iterator i = std::find(mObjects.begin(),
                                                      mObjects.end(), object);
        if (i == mObjects.end())
        {
            object->attach();
            mObjects.push_back(object);
        }
    }

    void ConnectionQueue::removeObject(Object *object)
    {
        std::vector<Object *>::iterator i = std::find(mObjects.begin(),
                                                      mObjects.end(), object);
        if (i != mObjects.end())
        {
            mObjects.erase(i);
            object->release();
        }
    }

} /* namespace od */
