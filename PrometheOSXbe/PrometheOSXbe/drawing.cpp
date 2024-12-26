#include "drawing.h"
#include "context.h"
#include "math.h"
#include "utils.h"
#include "meshUtility.h"
#include "stringUtility.h"
#include "theme.h"
#include "pointerMap.h"

#include <xgraphics.h>

#define SSFN_IMPLEMENTATION
#define SFFN_MAXLINES 8192
#define SSFN_memcmp memcmp
#define SSFN_memset memset
#define SSFN_realloc realloc
#define SSFN_free free
#include "ssfn.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT(x)
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace
{
	ssfn_t* mFontContext = NULL;
	//ssfn_buf_t* mFontBuffer = NULL;

	//SSFN::Font* mFont = NULL;
}

inline unsigned char lerp(unsigned  char a, unsigned char b, float t)
{
	return (unsigned char)(a + t * (b - a));
}

inline int length(int a, int b)
{
	return (a * a) + (b * b);
}

void drawing::swizzle(const void *src, const uint32_t& depth, const uint32_t& width, const uint32_t& height, void *dest)
{
  for (UINT y = 0; y < height; y++)
  {
    UINT sy = 0;
    if (y < width)
    {
      for (int bit = 0; bit < 16; bit++)
        sy |= ((y >> bit) & 1) << (2*bit);
      sy <<= 1; // y counts twice
    }
    else
    {
      UINT y_mask = y % width;
      for (int bit = 0; bit < 16; bit++)
        sy |= ((y_mask >> bit) & 1) << (2*bit);
      sy <<= 1; // y counts twice
      sy += (y / width) * width * width;
    }
    BYTE *s = (BYTE *)src + y * width * depth;
    for (UINT x = 0; x < width; x++)
    {
      UINT sx = 0;
      if (x < height * 2)
      {
        for (int bit = 0; bit < 16; bit++)
          sx |= ((x >> bit) & 1) << (2*bit);
      }
      else
      {
        int x_mask = x % (2*height);
        for (int bit = 0; bit < 16; bit++)
          sx |= ((x_mask >> bit) & 1) << (2*bit);
        sx += (x / (2 * height)) * 2 * height * height;
      }
      BYTE *d = (BYTE *)dest + (sx + sy)*depth;
      for (unsigned int i = 0; i < depth; ++i)
        *d++ = *s++;
    }
  }
}

	//uint8_t* pixels = imageData;
	//for (int i = 0; i < width * height * 4; i+=4)
	//{
	//	// swap bgra to rgba
	//	unsigned char temp = pixels[i + 0];
	//	pixels[i + 0] = pixels[i + 2];
	//	pixels[i + 2] = temp;
	//}

image* drawing::createImage(uint8_t* imageData, D3DFORMAT format, int width, int height)
{
	image* imageToAdd = new image();
	imageToAdd->width = width;
	imageToAdd->height = height;

	if (FAILED(D3DXCreateTexture(context::getD3dDevice(), imageToAdd->width, imageToAdd->height, 1, 0, format, D3DPOOL_DEFAULT, &imageToAdd->texture)))
	{
		return false;
	}

	D3DSURFACE_DESC surfaceDesc;
	imageToAdd->texture->GetLevelDesc(0, &surfaceDesc);

	imageToAdd->uvRect = math::rectF(0, 0, imageToAdd->width / (float)surfaceDesc.Width, imageToAdd->height / (float)surfaceDesc.Height);

	D3DLOCKED_RECT lockedRect;
	if (SUCCEEDED(imageToAdd->texture->LockRect(0, &lockedRect, NULL, 0)))
	{
		uint8_t* tempBuffer = (uint8_t*)malloc(surfaceDesc.Size);
		memset(tempBuffer, 0, surfaceDesc.Size);
		uint8_t* src = imageData;
		uint8_t* dst = tempBuffer;
		for (int32_t y = 0; y < imageToAdd->height; y++)
		{
			memcpy(dst, src, imageToAdd->width * 4);
			src += imageToAdd->width * 4;
			dst += surfaceDesc.Width * 4;
		}
		swizzle(tempBuffer, 4, surfaceDesc.Width, surfaceDesc.Height, lockedRect.pBits);
		free(tempBuffer);
		imageToAdd->texture->UnlockRect(0);
	}

	return imageToAdd;
}

void drawing::addImage(const char* key, uint8_t* imageData, D3DFORMAT format, int width, int height)
{
	image* imageToAdd = createImage(imageData, format, width, height);
	context::getImageMap()->add(key, imageToAdd);
}

uint64_t drawing::getImageMemUse(const char* key)
{
	image* imageInfo = context::getImageMap()->get(key);
	if (imageInfo == NULL)
	{
		return 0;
	}
	return utils::roundUpToNextPowerOf2(imageInfo->width) * utils::roundUpToNextPowerOf2(imageInfo->height) * 4;
}

void drawing::removeImage(const char* key)
{
	image* imageToRemove = context::getImageMap()->get(key);
	if (imageToRemove != NULL)
	{
		context::getImageMap()->removeKey(key);
	}
}

bool drawing::loadImage(const char* buffer, uint32_t length, const char* key)
{
	int width;
	int height;
	uint8_t* imageData = (uint8_t*)stbi_load_from_memory((const stbi_uc*)buffer, length, &width, &height, NULL, STBI_rgb_alpha);
	if (imageData == NULL)
	{
		return false;
	}
	addImage(key, imageData, D3DFMT_A8B8G8R8, width, height);
	free(imageData);
	return true;
}

bool drawing::loadFont(const uint8_t* data)
{
	if (mFontContext == NULL)
	{
		mFontContext = (ssfn_t*)malloc(sizeof(ssfn_t));                         
		memset(mFontContext, 0, sizeof(ssfn_t));
	}

	int result = ssfn_load(mFontContext, data);
	return result == 0;
}

void drawing::clearBackground(uint32_t index)
{
	char* backgroundName = stringUtility::formatString("background:%i", index);

	uint32_t backgroundColor = theme::getBackgroundColor();
	context::getD3dDevice()->Clear(0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, (0xff << 24) | backgroundColor, 1.0f, 0L);

	for (int i = 0; i < 2; i++)
	{
		int index = theme::getBackgroundOverlayAsUnderlay() == true ? (1 - i) : i;
		if (index == 0 && imageExists(backgroundName) == true)
		{
			drawImage(backgroundName, theme::getBackgroundImageTint(), 0, 0, context::getBufferWidth(), context::getBufferHeight());
		}
		if (index == 1 && imageExists("background-overlay") == true)
		{
			drawImage("background-overlay", theme::getBackgroundOverlayImageTint(), 0, 0, context::getBufferWidth(), context::getBufferHeight());
		}
	}

	free(backgroundName);
}

bool drawing::imageExists(const char* key)
{
	image* result = context::getImageMap()->get(key);
    return result != NULL;
}

image* drawing::getImage(const char* key)
{
	image* result = context::getImageMap()->get(key);
    return result;
}

void drawing::setTint(unsigned int color)
{
	context::getD3dDevice()->SetRenderState(D3DRS_TEXTUREFACTOR, color);
}

void drawing::drawImage(image* image, uint32_t tint, int x, int y, int width, int height)
{
	if (image == NULL)
	{
		return;

	}
	context::getD3dDevice()->SetRenderState(D3DRS_TEXTUREFACTOR, tint);
	float newY = (float)context::getBufferHeight() - (y + height);
	context::getD3dDevice()->SetTexture(0, image->texture);
	utils::dataContainer* vertices = meshUtility::createQuadXY(math::vec3F((float)x - 0.5f, newY - 0.5f, 0), math::sizeF((float)width, (float)height), image->uvRect);
	context::getD3dDevice()->DrawPrimitiveUP(D3DPT_TRIANGLELIST, (vertices->size / sizeof(meshUtility::vertex)) / 3, vertices->data, sizeof(meshUtility::vertex));
	delete(vertices);
}

void drawing::drawImage(image* image, uint32_t tint, int x, int y)
{
	if (image == NULL)
	{
		return;

	}
	drawImage(image, tint, x, y, image->width, image->height);
}

void drawing::drawImage(const char* imageKey, uint32_t tint, int x, int y, int width, int height)
{
	image* imageToDraw = getImage(imageKey);
	if (imageToDraw == NULL)
	{
		return;
	}
	drawImage(imageToDraw, tint, x, y, width, height);
}

void drawing::drawImage(const char* imageKey, uint32_t tint, int x, int y)
{
	image* imageToDraw = getImage(imageKey);
	if (imageToDraw == NULL)
	{
		return;
	}
	drawImage(imageToDraw, tint, x, y);
}

void drawing::drawNinePatch(const char* imageKey, uint32_t tint, int x, int y, int width, int height)
{
	image* imageToDraw = getImage(imageKey);
	if (imageToDraw == NULL)
	{
		return;
	}
	context::getD3dDevice()->SetRenderState(D3DRS_TEXTUREFACTOR, tint);
	float newY = (float)context::getBufferHeight() - (y + height);
	context::getD3dDevice()->SetTexture(0, imageToDraw->texture);
	utils::dataContainer* vertices = meshUtility::createNinePatchXY(math::vec3F((float)x - 0.5f, newY - 0.5f, 0), math::sizeF((float)width, (float)height), imageToDraw->uvRect);
	context::getD3dDevice()->DrawPrimitiveUP(D3DPT_TRIANGLELIST, (vertices->size / sizeof(meshUtility::vertex)) / 3, vertices->data, sizeof(meshUtility::vertex));
	delete(vertices);
}

image* drawing::renderHorizontalLine(const char* cacheKey, unsigned int color, int width, int height)
{
	if (imageExists(cacheKey) == false)
	{
		uint32_t* imageData = (uint32_t*)malloc(width * height * 4);
		for (int i = 0; i < width * height; i++)
		{
			imageData[i] = color;
		}
		addImage(cacheKey, (uint8_t*)imageData, D3DFMT_A8R8G8B8, width, height);
		free(imageData);
	}
	return getImage(cacheKey);
}

image* drawing::renderRoundedRect(const char* cacheKey, uint32_t width, uint32_t height, uint32_t cornerRadius, uint32_t fillColor, uint32_t strokeColor, uint32_t strokeWidth)
{
	if (imageExists(cacheKey) == false)
	{
		uint32_t* imageData = (uint32_t*)malloc(width * height * 4);
		
		int widthMinus1 = width - 1;
		int heightMinus1 = height - 1;
		for (unsigned int y = 0; y < height; y++) 
		{
			float gradient = (float)y / height;
			for (unsigned int x = 0; x < width; x++) 
			{
				unsigned int color = 0x00000000;

				bool isInsideRect = (y >= cornerRadius && y < height - cornerRadius) || (x >= cornerRadius && x < width - cornerRadius);
				int topLeftCornerLength = length(y - cornerRadius, x - cornerRadius);
				int topRightCornerLength = length(y - cornerRadius, x - (widthMinus1 - cornerRadius));
				int bottomLeftCornerLength = length(y - (heightMinus1 - cornerRadius), x - cornerRadius);
				int bottomRightCornerLength = length(y - (heightMinus1 - cornerRadius), x - (widthMinus1 - cornerRadius));
				int cornerRadiusSquared = cornerRadius * cornerRadius;

				bool isInsideTopLeftCorner = topLeftCornerLength < cornerRadiusSquared;
				bool isInsideTopRightCorner = topRightCornerLength < cornerRadiusSquared;
				bool isInsideBottomLeftCorner = bottomLeftCornerLength < cornerRadiusSquared;
				bool isInsideBottomRightCorner = bottomRightCornerLength < cornerRadiusSquared;
				if (isInsideRect || isInsideTopLeftCorner || isInsideTopRightCorner || isInsideBottomLeftCorner || isInsideBottomRightCorner) 
				{
					color = fillColor;
				}
				imageData[(y * width) + x] = color;
			}
		}

		if (strokeWidth > 0)
		{
			for (uint32_t y = 0; y < height; y++) 
			{
				int count = 0;
				for (uint32_t x = 0; x < width; x++) 
				{
					uint32_t color = imageData[(y * width) + x];
					if (count == strokeWidth)
					{
						break;
					}
					if (color > 0)
					{
						imageData[(y * width) + x] = strokeColor;
						imageData[(y * width) + (width - 1) - x] = strokeColor;
						count++;
					}
				}
			}

			for (uint32_t x = 0; x < width; x++)  
			{
				int count = 0;
				for (uint32_t y = 0; y < height; y++)
				{
					uint32_t color = imageData[(y * width) + x];
					if (count == strokeWidth)
					{
						break;
					}
					if (color > 0)
					{
						imageData[(y * width) + x] = strokeColor;
						imageData[(((height - 1) - y) * width) + x] = strokeColor;
						count++;
					}
				}
			}
		}
	
		//stbi_write_jpg("App:\\test.jpg", width, height, 4, imageData, 100);
		addImage(cacheKey, (uint8_t*)imageData, D3DFMT_A8R8G8B8, width, height);
		free(imageData);
	}
	return getImage(cacheKey);
}

void drawing::drawHorizontalLine(uint32_t color, int x, int y, int width, int height)
{
	char* cacheKey = stringUtility::formatString("drawHorizontalLine-%02x-%02x", width, height);
	image* image = renderHorizontalLine(cacheKey, color, width, height);
	drawImage(image, color, x, y);
	free(cacheKey);
}
//
//void drawing::measureString(const char* fontName, int fontStyle, int fontSize, const char* message, int* width, int* height)
//{
//	ssfn_select(mFontContext, SSFN_FAMILY_ANY, fontName, fontStyle, fontSize);
//	ssfn_bbox(mFontContext, message, width, height, NULL, NULL);
//}
//
//image* drawing::renderString(const char* cacheKey, Bounds bounds, const char* message, uint32_t color)
//{
//	if (imageExists(cacheKey) == false)
//	{
//		int ret;
//
//		uint32_t* imageData = (uint32_t*)malloc(bounds.width * bounds.height * 4);
//		memset(imageData, 0, bounds.width * bounds.height * 4);
//
//		ssfn_buf_t buffer; 
//		memset(&buffer, 0, sizeof(buffer));
//		buffer.ptr = (uint8_t*)imageData;       
//		buffer.x = bounds.left;
//		buffer.y = bounds.top;
//		buffer.w = bounds.width;                        
//		buffer.h = bounds.height;                     
//		buffer.p = bounds.width * 4;                          
//		buffer.bg = color;
//		buffer.fg = color;           
//
//		const char *text = message;
//
//		while((ret = ssfn_render(mFontContext, &buffer, text)) > 0)
//		{
//			text += ret;
//		}
//
//		addImage(cacheKey, (uint8_t*)imageData, D3DFMT_A8R8G8B8, bounds.width, bounds.height);
//		free(imageData);
//	}
//	return getImage(cacheKey);
//}
//
//void drawing::drawString(const char* fontName, int fontStyle, int fontSize, const char* message, uint32_t color, int x, int y)
//{
//	ssfn_select(mFontContext, SSFN_FAMILY_ANY, fontName, fontStyle, fontSize);
//
//	Bounds bounds;
//	ssfn_bbox(mFontContext, message, &bounds.width, &bounds.height, &bounds.left, &bounds.top);
//
//	char* cacheKey = stringUtility::formatString("drawString-%s-%02x-%02x-%s-%08x-%08x", fontName, fontStyle, fontSize, message, color);
//	image* image = renderString(cacheKey, bounds, message, color);
//	drawImage(image, 0xffffffff, x, y);
//	free(cacheKey);
//}
//
//void drawing::drawStringAligned(const char* fontName, int fontStyle, int fontSize, const char*  message, uint32_t color, horizAlignment hAlign, int x, int y, int width)
//{
//	ssfn_select(mFontContext, SSFN_FAMILY_ANY, fontName, fontStyle, fontSize);
//
//	int textWidth = 0;
//	measureString(fontName, fontStyle, fontSize, message, &textWidth, NULL);
//
//	int xPos = x;
//	if (hAlign == horizAlignmentCenter)
//	{
//		xPos = x + ((width - textWidth) / 2);
//	}
//	else if (hAlign == horizAlignmentRight)
//	{
//		xPos = x + (width - textWidth);
//	}
//
//	drawString(fontName, fontStyle, fontSize, message, color, xPos, y);
//}

bitmapFont* drawing::generateBitmapFont(const char* fontName, int fontStyle, int fontSize, int lineHeight, int spacing, int textureDimension)
{
	bitmapFont* font = new bitmapFont();
	font->charMap = new pointerMap<math::rectI*>(true);

	ssfn_select(mFontContext, SSFN_FAMILY_ANY, fontName, fontStyle, fontSize);

	int textureWidth = textureDimension;
	int textureHeight = textureDimension; 

	uint32_t* imageData = (uint32_t*)malloc(textureWidth * textureHeight * 4);
	memset(imageData, 0, textureWidth * textureHeight * 4);  

	int x = 2;
	int y = 2;

	char* charsToEncode = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\xC2\xA1\xC2\xA2\xC2\xA3\xC2\xA4\xC2\xA5\xC2\xA6\xC2\xA7\xC2\xA8\xC2\xA9\xC2\xAA\xC2\xAB\xC2\xAC\xC2\xAD\xC2\xAE\xC2\xAF\xC2\xB0\xC2\xB1\xC2\xB2\xC2\xB3\xC2\xB4\xC2\xB5\xC2\xB6";
	char* currentCharPos = charsToEncode;
	while(*currentCharPos)
	{	
		char* nextCharPos = currentCharPos;
		uint32_t unicode = ssfn_utf8(&nextCharPos);

		int32_t length = nextCharPos - currentCharPos;
		char* currentChar = (char*)malloc(length + 1);
		memcpy(currentChar, currentCharPos, length);
		currentChar[length] = 0;

		currentCharPos = nextCharPos;

		Bounds bounds;
		int ret = ssfn_bbox(mFontContext, currentChar, &bounds.width, &bounds.height, &bounds.left, &bounds.top);
		if (ret != 0)
		{
			continue;
		}

		if ((x + bounds.width + 2) > textureWidth)
		{
			x = 2;
			y = y + bounds.height + 2;
		}

		math::rectI* rect = new math::rectI();
		rect->x = x;
		rect->y = y;
		rect->width = bounds.width;
		rect->height = bounds.height;

		char* unicodeString = stringUtility::formatString("%i", unicode);
		font->charMap->add(unicodeString, rect);
		free(unicodeString);

		ssfn_buf_t buffer; 
		memset(&buffer, 0, sizeof(buffer));
		buffer.ptr = (uint8_t*)imageData;       
		buffer.x = x + bounds.left;
		buffer.y = y + bounds.top;
		buffer.w = textureWidth;                        
		buffer.h = textureHeight;                     
		buffer.p = textureWidth * 4;                          
		buffer.bg = 0xffffffff;
		buffer.fg = 0xffffffff;   

		ssfn_render(mFontContext, &buffer, currentChar);

		x = x + bounds.width + 2;   
		free(currentChar);
	}

	font->image = createImage((uint8_t*)imageData, D3DFMT_A8R8G8B8, textureWidth, textureHeight);
	font->lineHeight = lineHeight;
	font->spacing = spacing;
	free(imageData);

	return font;
}

void drawing::measureBitmapString(bitmapFont* font, const char* message, int* width, int* height)
{
	image* image = font->image;

	int xPosMax = 0;

	int xPos = 0;
	int yPos = 0;

	char* currentCharPos = (char*)message;
	while(*currentCharPos)
	{	
		char* nextCharPos = currentCharPos;
		uint32_t unicode = ssfn_utf8(&nextCharPos);

		int32_t length = nextCharPos - currentCharPos;
		char* currentChar = (char*)malloc(length + 1);
		memcpy(currentChar, currentCharPos, length);
		currentChar[length] = 0;

		currentCharPos = nextCharPos;

		if (stringUtility::equals(currentChar, "\n", false) == true)
		{
			xPos = 0;
			yPos += font->lineHeight;
			continue;
		}

		char* unicodeString = stringUtility::formatString("%i", unicode);
		math::rectI* rect = font->charMap->get(unicodeString);
		free(unicodeString);
		if (rect == NULL)
		{
			continue;
		}

		xPos = xPos + rect->width + font->spacing;
		xPosMax = max(xPosMax, xPos);
		free(currentChar);
	}

	if (width != NULL)
	{
		*width = xPosMax - 2;
	}

	if (height != NULL)
	{
		*height = yPos + font->lineHeight;
	}
}

void drawing::drawBitmapString(bitmapFont* font, const char* message, uint32_t color, int x, int y)
{
	image* image = font->image;

	int xPos = x;
	int yPos = y;

	char* currentCharPos = (char*)message;
	while(*currentCharPos)
	{	
		char* nextCharPos = currentCharPos;
		uint32_t unicode = ssfn_utf8(&nextCharPos);

		int32_t length = nextCharPos - currentCharPos;
		char* currentChar = (char*)malloc(length + 1);
		memcpy(currentChar, currentCharPos, length);
		currentChar[length] = 0;

		currentCharPos = nextCharPos;

		if (stringUtility::equals(currentChar, "\n", false) == true)
		{
			xPos = x;
			yPos += font->lineHeight;
			continue;
		}

		char* unicodeString = stringUtility::formatString("%i", unicode);
		math::rectI* rect = font->charMap->get(unicodeString);
		free(unicodeString);
		if (rect == NULL)
		{
			continue;
		}

		math::rectF uvRect;
		uvRect.x = rect->x / (float)image->width;
		uvRect.y = rect->y / (float)image->height;
		uvRect.width = rect->width / (float)image->width;
		uvRect.height = rect->height / (float)image->height;

		uint32_t charColor = color;
		if (unicode == 161)
		{
			charColor = theme::getJoyButtonAColor();
		}
		else if (unicode == 162)
		{
			charColor = theme::getJoyButtonBColor();
		}
		else if (unicode == 163)
		{
			charColor = theme::getJoyButtonXColor();
		}
		else if (unicode == 164)
		{
			charColor = theme::getJoyButtonYColor();
		}
		else if (unicode == 182)
		{
			charColor = theme::getJoyButtonBlackColor();
		}
			else if (unicode == 181)
		{
			charColor = theme::getJoyButtonWhiteColor();
		}

		context::getD3dDevice()->SetRenderState(D3DRS_TEXTUREFACTOR, charColor);
		float newY = (float)context::getBufferHeight() - (yPos + rect->height);
		context::getD3dDevice()->SetTexture(0, image->texture);
		utils::dataContainer* vertices = meshUtility::createQuadXY(math::vec3F((float)xPos - 0.5f, newY - 0.5f, 0), math::sizeF((float)rect->width, (float)rect->height), uvRect);
		context::getD3dDevice()->DrawPrimitiveUP(D3DPT_TRIANGLELIST, (vertices->size / sizeof(meshUtility::vertex)) / 3, vertices->data, sizeof(meshUtility::vertex));
		delete(vertices);

		xPos = xPos + rect->width + font->spacing;
		free(currentChar);
	}
}

void drawing::drawBitmapStringAligned(bitmapFont* font, const char*  message, uint32_t color, horizAlignment hAlign, int x, int y, int width)
{
	int textWidth = 0;
	measureBitmapString(font, message, &textWidth, NULL);

	int xPos = x;
	if (hAlign == horizAlignmentCenter)
	{
		xPos = x + ((width - textWidth) / 2);
	}
	else if (hAlign == horizAlignmentRight)
	{
		xPos = x + (width - textWidth);
	}

	drawBitmapString(font, message, color, xPos, y);
}

utils::dataContainer* drawing::takeScreenshot()
{
	utils::dataContainer* result = NULL;

	IDirect3DSurface8* surface;
	context::getD3dDevice()->GetBackBuffer(-1, D3DBACKBUFFER_TYPE_MONO, &surface);

	D3DLOCKED_RECT lockedRect;
	if (surface->LockRect(&lockedRect, NULL, D3DLOCK_READONLY) >= 0)
	{
		uint8_t* pBits = (uint8_t*)lockedRect.pBits;

		D3DSURFACE_DESC desc;
		surface->GetDesc(&desc);

		uint8_t* buffer = (uint8_t*)malloc(desc.Width * desc.Height * 4);
		uint8_t* bufferOffset = buffer;

		for (uint32_t y = 0; y < desc.Height; ++y) {
			uint8_t* row = pBits + y * lockedRect.Pitch;
			for (uint32_t x = 0; x < desc.Width; ++x) {
				uint8_t* pixel = row + x * 4; 
				bufferOffset[0] = pixel[2];
				bufferOffset[1] = pixel[1];
				bufferOffset[2] = pixel[0];
				bufferOffset[3] = 0xff;
				bufferOffset += 4;
			}
		}
		
		int fileSize = 0;
		char *pngData = (char*)stbi_write_png_to_mem(buffer, lockedRect.Pitch, desc.Width, desc.Height, 4, &fileSize);
		result = new utils::dataContainer(pngData, fileSize, fileSize);
		free(buffer);
		free(pngData);

		surface->UnlockRect();
	}
	surface->Release();

	return result;
}