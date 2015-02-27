/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#include <array>
#include <atomic>
#include <cstdint>

namespace dit
{

    template <typename T>
    class AtomicDoubleBuffer {
    public:
        AtomicDoubleBuffer() : state_(0) { }
        ~AtomicDoubleBuffer() { }

        // Never returns nullptr
        T* start_writing() {
            // Increment active users; once we do this, no one
            // can swap the active cell on us until we're done
            auto state = state_.fetch_add(0x2, std::memory_order_relaxed);
            return &buffers_[state & 1];
        }

        void end_writing() {
            // We want to swap the active cell, but only if we were the last
            // ones concurrently accessing the data (otherwise the consumer
            // will do it for us when *it's* done accessing the data)

            auto state = state_.load(std::memory_order_relaxed);
            std::uint32_t flag = (8 << (state & 1)) ^ (state & (8 << (state & 1)));
            state = state_.fetch_add(flag - 0x2, std::memory_order_release) + flag - 0x2;
            if ((state & 0x6) == 0) {
                // The consumer wasn't in the middle of a read, we should
                // swap (unless the consumer has since started a read or
                // already swapped or read a value and is about to swap).
                // If we swap, we also want to clear the full flag on what
                // will become the active cell, otherwise the consumer could
                // eventually read two values out of order (it reads a new
                // value, then swaps and reads the old value while the
                // producer is idle).
                state_.compare_exchange_strong(state, (state ^ 0x1) & ~(0x10 >> (state & 1)), std::memory_order_release);
            }
        }

        // Returns nullptr if there appears to be no more data to read yet
        T* start_reading() {
            readState_ = state_.load(std::memory_order_relaxed);
            if ((readState_ & (0x10 >> (readState_ & 1))) == 0) {
                // Nothing to read here!
                return nullptr;
            }

            // At this point, there is guaranteed to be something to
            // read, because the full flag is never turned off by the
            // producer thread once it's on; the only thing that could
            // happen is that the active cell changes, but that can
            // only happen after the producer wrote a value into it,
            // in which case there's still a value to read, just in a
            // different cell.

            readState_ = state_.fetch_add(0x2, std::memory_order_acquire) + 0x2;

            // Now that we've incremented the user count, nobody can swap until
            // we decrement it
            return &buffers_[(readState_ & 1) ^ 1];
        }

        void end_reading() {
            if ((readState_ & (0x10 >> (readState_ & 1))) == 0) {
                // There was nothing to read; shame to repeat this
                // check, but if these functions are inlined it might
                // not matter. Otherwise the API could be changed.
                // Or just don't call this method if start_reading()
                // returns nullptr -- then you could also get rid
                // of m_readState.
                return;
            }

            // Alright, at this point the active cell cannot change on
            // us, but the active cell's flag could change and the user
            // count could change. We want to release our user count
            // and remove the flag on the value we read.

            auto state = state_.load(std::memory_order_relaxed);
            std::uint32_t sub = (0x10 >> (state & 1)) | 0x2;
            state = state_.fetch_sub(sub, std::memory_order_relaxed) - sub;
            if ((state & 0x6) == 0 && (state & (0x8 << (state & 1))) == 1) {
                // Oi, we were the last ones accessing the data when we released our cell.
                // That means we should swap, but only if the producer isn't in the middle
                // of producing something, and hasn't already swapped, and hasn't already
                // set the flag we just reset (which would mean they swapped an even number
                // of times).  Note that we don't bother swapping if there's nothing to read
                // in the other cell.
                state_.compare_exchange_strong(state, state ^ 0x1, std::memory_order_relaxed);
            }
        }

    public:
        std::array<T, 2> buffers_;

        // The bottom (lowest) bit will be the active cell (the one for writing).
        // The active cell can only be switched if there's at most one concurrent
        // user. The next two bits of state will be the number of concurrent users.
        // The fourth bit indicates if there's a value available for reading
        // in m_buf[0], and the fifth bit has the same meaning but for m_buf[1].
        std::atomic<std::uint32_t> state_;

        std::uint32_t readState_;
    };

}