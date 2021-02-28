#pragma once

#include <cstring>

namespace od
{

  template <typename T>
  class Fifo
  {

    // Construction/Destruction
  public:
    Fifo()
    {
      buffer = 0;
      depth = 0;
      pos = 0;
      w1 = 0;
      w2 = 0;
    }

    Fifo(int n)
    {
      allocate(n);
    }

    ~Fifo()
    {
      deallocate();
    }

    void allocate(int n)
    {
      deallocate();
      depth = n;
      buffer = new T[2 * n];
      w1 = buffer;
      w2 = buffer + depth;
      pos = 0;
    }

    void deallocate()
    {
      if (buffer)
      {
        delete[] buffer;
        buffer = 0;
        depth = 0;
        pos = 0;
        w1 = 0;
        w2 = 0;
      }
    }

    inline void push(T s)
    {
      push(&s, 1);
    }

#if 0
	// This memcpy version is actually slower.
	void push(T * ref, int size) {
		if (size > depth) { // Way too much data
			push(&(ref[size - depth]), depth);
		} else if (size + pos <= depth) { // Plenty of room
			memcpy(w1 + pos, ref, sizeof(T) * size);
			memcpy(w2 + pos, ref, sizeof(T) * size);
			pos += size;
			if (pos == depth)
			pos = 0;
		} else { // Overflow case
			int remaining = depth - pos;
			memcpy(w1 + pos, ref, sizeof(T) * remaining);
			memcpy(w2 + pos, ref, sizeof(T) * remaining);
			// Now assuming pos = 0;
			pos = size - remaining;// really ending
			memcpy(w1, ref + remaining, sizeof(T) * remaining);
			memcpy(w2, ref + remaining, sizeof(T) * remaining);
		}
	}
#else
    void push(T *ref, int size)
    {
      // int i;
      if (size > depth)
      { // Way too much data
        push(&(ref[size - depth]), depth);
      }
      else if (size + pos <= depth)
      { // Plenty of room
        //for (i = 0; i < size; i++) {
        //	w1[pos + i] = w2[pos + i] = ref[i];
        //}
        memcpy(w1 + pos, ref, size * sizeof(T));
        memcpy(w2 + pos, ref, size * sizeof(T));
        pos += size;
        if (pos == depth)
          pos = 0;
      }
      else
      { // Overflow case
        int remaining = depth - pos;
        //for (i = 0; i < remaining; i++) {
        //	w1[pos + i] = w2[pos + i] = ref[i];
        //}
        memcpy(w1 + pos, ref, remaining * sizeof(T));
        memcpy(w2 + pos, ref, remaining * sizeof(T));
        // Now assuming pos = 0;
        pos = size - remaining; // really ending
        //for (i = 0; i < pos; i++) {
        //	w1[i] = w2[i] = ref[i + remaining];
        //}
        memcpy(w1, ref + remaining, pos * sizeof(T));
        memcpy(w2, ref + remaining, pos * sizeof(T));
      }
    }
#endif
    // Not usually necessary because it is used to pop what you just wrote.
    void popLastWritten(int n = 1)
    {
      pos -= n;
      if (pos < 0)
        pos = 0;
    }

    void set(int n, T s)
    {
      w1[n] = w2[n] = s;
    }

    inline int size()
    {
      return depth;
    }

    void clear(T s = 0)
    {
      std::memset(buffer, s, depth * 2 * sizeof(T));
      pos = 0;
    }

    T operator[](int i)
    {
      return buffer[depth + pos - i - 1];
    }

    inline T get()
    {
      return buffer[depth + pos - 1];
    }

    // n is a length not a position! get(0) is invalid.
    inline T *get(int n)
    {
      return &(buffer[depth + pos - n]);
    }

  protected:
    T *buffer;
    T *w1;
    T *w2;
    int pos; // the write position, there is no read position
    int depth;
  };

} // namespace od
