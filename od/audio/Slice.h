/*
 * Slice.h
 *
 *  Created on: 4 Jul 2016
 *      Author: clarkson
 */

#ifndef APP_AUDIO_SLICE_H_
#define APP_AUDIO_SLICE_H_

namespace od
{

        class Slice
        {
        public:
                Slice()
                {
                }

                Slice(int start) : mStart(start)
                {
                }

                Slice(int start, int loopStart) : mStart(start), mLoopStart(loopStart)
                {
                }

                Slice(int start, int loopStart, int loopEnd) : mStart(start), mLoopStart(loopStart), mLoopEnd(loopEnd)
                {
                }

                Slice(int start, int loopStart, int loopEnd, float speed) : mStart(start), mLoopStart(loopStart), mLoopEnd(loopEnd), mSpeed(
                                                                                                                                         speed)
                {
                }

                bool hasNonTrivialLoop()
                {
                        return mLoopStart > 0 || mLoopEnd > 0;
                }

                int mStart = 0;
                // mLoopStart==0 means begin of slice (i.e. slice mStart).
                int mLoopStart = 0;
                // mLoopEnd==0 means end of slice (i.e. next slice mStart).
                int mLoopEnd = 0;
                float mSpeed = 1.0f;
        };

        static inline bool operator<(const Slice &a, const Slice &b)
        {
                return a.mStart < b.mStart;
        }

} /* namespace od */

#endif /* APP_AUDIO_SLICE_H_ */
