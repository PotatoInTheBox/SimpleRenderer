#pragma once

#include <vector>
#include "raylib.h"

class RenderTarget {
public:
	int width, height;
	std::vector<Color> rgbBuffer;
	std::vector<float> zBuffer;

	RenderTarget(int w, int h)
		: width(w), height(h),
		rgbBuffer(w * h),
		zBuffer(w * h)
	{
		clear();
	}

	void clear() {
		std::fill(rgbBuffer.begin(), rgbBuffer.end(), BLACK);
		std::fill(zBuffer.begin(), zBuffer.end(), INFINITY);
	}

};
//	(int w, int h)
//{
//	public readonly float3[] ColourBuffer = new float3[w * h];
//	public readonly float[] DepthBuffer = new float[w * h];
//	public readonly object[] locks = new object[w * h];
//
//	public readonly int Width = w;
//	public readonly int Height = h;
//	public readonly float2 Size = new(w, h);
//
//	public void Clear(float3 bgCol)
//	{
//		for (int i = 0; i < ColourBuffer.Length; i++)
//		{
//			ColourBuffer[i] = bgCol;
//		}
//
//		for (int i = 0; i < DepthBuffer.Length; i++)
//		{
//			DepthBuffer[i] = float.PositiveInfinity;
//		}
//
//		if (locks[0] == null)
//		{
//			for (int i = 0; i < locks.Length; i++)
//			{
//				locks[i] = new object();
//			}
//		}
//	}
//}