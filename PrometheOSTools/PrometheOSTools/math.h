#pragma once

class math
{
public:

	typedef struct vec2F
	{
		union {
			struct {
				float x;
				float y;
			};
			float values[2];
		};
		vec2F() : x(0), y(0) {}
		vec2F(const float x, const float y) : x(x), y(y) {}
	} vec2F;

	typedef struct vec3F
	{
		union {
			struct {
				float x;
				float y;
				float z;
			};
			float values[3];
		};
		vec3F() : x(0), y(0), z(0) {}
		vec3F(const float x, const float y, const float z) : x(x), y(y), z(z) {}
	} vec3F;

	typedef struct sizeF
	{
		union {
			struct {
				float width;
				float height;
			};
			float values[2];
		};
		sizeF() : width(0), height(0) {}
		sizeF(const float width, const float height) : width(width), height(height) {}
	} sizeF;

	typedef struct rectF
	{
		union {
			struct {
				float x;
				float y;
				float width;
				float height;
			};
			float values[4];
		};
		rectF() : x(0), y(0), width(0), height(0) {}
		rectF(const float x, const float y, const float width, const float height) : x(x), y(y), width(width), height(height) {}
	} rectF;

	typedef struct rectI
	{
		union {
			struct {
				int x;
				int y;
				int width;
				int height;
			};
			int values[4];
		};
		rectI() : x(0), y(0), width(0), height(0) {}
		rectI(const int x, const int y, const int width, const int height) : x(x), y(y), width(width), height(height) {}
	} rectI;

	static float calcNinePatchPosition(int index, float length);
	static float calcNinePatchUV(int index, float length);
};
