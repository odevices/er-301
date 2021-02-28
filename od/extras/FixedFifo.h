#pragma once

namespace od
{

  template <typename T, int N>
  class FixedFifo
  {

    // Construction/Destruction
  public:
    FixedFifo()
    {
      pos = 0;
      w2 = w1 + N;
    }

    void push(T s)
    {
      push(&s, 1);
    }

    void push(T *ref, int size)
    {
      int i;
      if (size > N)
      { // Way too much data
        push(&(ref[size - N]), N);
      }
      else if (size + pos <= N)
      { // Plenty of room
        for (i = 0; i < size; i++)
          w1[pos + i] = w2[pos + i] = ref[i];
        pos += size;
        if (pos == N)
          pos = 0;
      }
      else
      { // Overflow case
        int remaining = N - pos;
        for (i = 0; i < remaining; i++)
          w1[pos + i] = w2[pos + i] = ref[i];
        // Now assuming pos = 0;
        pos = size - remaining; // really ending
        for (i = 0; i < pos; i++)
          w1[i] = w2[i] = ref[i + remaining];
      }
    }

    void pop(int n = 1)
    {
      pos -= n;
      if (pos < 0)
        pos = 0;
    }

    void set(int n, T s)
    {
      w1[n] = w2[n] = s;
    }

    int size()
    {
      return N;
    }

    void clear(T s = 0)
    {
      std::memset(w1, s, N * 2 * sizeof(T));
      pos = 0;
    }

    T operator[](int i)
    {
      return w1[N + pos - i - 1];
    }

    T get()
    {
      return w1[N + pos - 1];
    }

    // n is a length not a position! get(0) is invalid.
    T *get(int n)
    {
      return &(w1[N + pos - n]);
    }

  protected:
    T w1[2 * N];
    T *w2;
    int pos;
  };
