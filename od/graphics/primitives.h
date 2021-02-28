#pragma once

#include <od/constants.h>
#include <hal/ops.h>

namespace od
{

	typedef int Color;

	struct Pixel
	{
		Color color;
		int x, y;
	};

	struct Point
	{
		int x, y;
	};

	struct Size
	{
		int width, height;
	};

	struct Rect
	{
		int left;
		int bottom;
		int right;
		int top;
	};

	static inline bool empty(const Rect &R)
	{
		return R.left == R.right || R.top == R.bottom;
	}

	static inline bool intersects(const Rect &A, const Rect &B)
	{
		return !(empty(A) || empty(B) || B.left > A.right || B.right < A.left || B.top < A.bottom || B.bottom > A.top);
	}

	// Does A completely contain B?
	static inline bool contains(const Rect &A, const Rect &B)
	{
		return !empty(A) && !empty(B) && A.left <= B.left && A.right >= B.right && A.bottom <= B.bottom && A.top >= B.top;
	}

	static inline bool operator<(const Rect &A, const Rect &B)
	{
		return contains(B, A);
	}

	static inline Rect merge(const Rect &A, const Rect &B)
	{
		Rect result;
		if (empty(A))
		{
			result = B;
		}
		else if (empty(B))
		{
			result = A;
		}
		else
		{
			result.left = MIN(A.left, B.left);
			result.right = MAX(A.right, B.right);
			result.bottom = MIN(A.bottom, B.bottom);
			result.top = MAX(A.top, B.top);
		}
		return result;
	}

	static inline Rect operator+(const Rect &A, const Rect &B)
	{
		return merge(A, B);
	}

} // namespace od
