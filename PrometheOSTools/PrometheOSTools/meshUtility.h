#pragma once

#include "math.h"
#include "utils.h"

class meshUtility
{
public:

	typedef struct vertex
	{
		math::vec3F position;
		math::vec2F texcoord;

		vertex()
		{
			position = math::vec3F();
			texcoord = math::vec2F();
		}

		vertex(const float& positionX, const float& positionY, const float& positionZ, 
			   const float& texcoordX, const float& texcoordY) 
		{
			position = math::vec3F(positionX, positionY, positionZ);
			texcoord = math::vec2F(texcoordX, texcoordY);
		}

		vertex(const math::vec3F& position, const math::vec2F& texcoord) : position(position), texcoord(texcoord) {}

	} vertex;	

	static utils::dataContainer* createQuadXY(const math::vec3F& position, const math::sizeF& size, const math::rectF& uvRect);
	static utils::dataContainer* createNinePatchXY(const math::vec3F& position, const math::sizeF& size, const math::rectF& uvRect);
};
