#include <bjorklund.h>

/* 
Length of output buffer must be at least n.
Length of scratch buffer must be at least 2n. 
*/
bool bjorklund(int k, int n, float *output, float *scratch)
{
  float *left = scratch;
  float *right = scratch + n;
  int nleft, nright;
  int i, j, m;

  // valid values for k and n
  if (k > n || n > 100)
    return false;

  if (k == 0)
  {
    for (i = 0; i < n; i++)
      output[i] = 0;
    return true;
  }

  left[0] = 1;
  nleft = 1;
  right[0] = 0;
  nright = 1;
  m = n - k;

  while (k + m > 3 && m > 0)
  {
    if (k < m)
    {
      m -= k;
      // append right to left
      for (i = 0, j = nleft; i < nright; i++, j++)
        left[j] = right[i];
      nleft += nright;
    }
    else if (m < k)
    {
      int tmp;
      // append right to left
      for (i = 0, j = nleft; i < nright; i++, j++)
        left[j] = right[i];
      // copy original left onto right
      for (i = 0; i < nleft; i++)
        right[i] = left[i];
      // adjust lengths
      tmp = nleft;
      nleft += nright;
      nright = tmp;
      // adjust state
      tmp = k;
      k = m;
      m = tmp - m;
    }
    else
    {
      // append right to left
      for (i = 0, j = nleft; i < nright; i++, j++)
        left[j] = right[i];
      nleft += nright;
      // clear right
      nright = 0;
      m = 0;
    }
  }

  // construct the output array

  // append left k times
  j = 0; // length of output
  while (k > 0)
  {
    for (i = 0; i < nleft; i++)
      output[j++] = left[i];
    k--;
  }

  // append right m times
  while (m > 0)
  {
    for (i = 0; i < nright; i++)
      output[j++] = right[i];
    m--;
  }

  return true;
}
