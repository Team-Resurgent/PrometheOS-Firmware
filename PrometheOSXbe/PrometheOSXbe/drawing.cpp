#include "drawing.h"
#include "context.h"
#include "math.h"
#include "utils.h"
#include "meshUtility.h"
#include "stringUtility.h"
#include "theme.h"
#include "pointerMap.h"
#include "xmvPlayer.h"
#include "vertexContainer.h"
#include "crc32.h"

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

#define STBIW_crc32 crc32::calculate
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace {
ssfn_t* mFontContext = NULL;
}

inline unsigned char lerp(unsigned char a, unsigned char b, float t) {
    return (unsigned char)(a + t * (b - a));
}

inline int length(int a, int b) {
    return (a * a) + (b * b);
}

void drawing::swizzle(const void* src, const uint32_t& depth, const uint32_t& width, const uint32_t& height, void* dest) {
    for (UINT y = 0; y < height; y++) {
        UINT sy = 0;
        if (y < width) {
            for (int bit = 0; bit < 16; bit++)
                sy |= ((y >> bit) & 1) << (2 * bit);
            sy <<= 1; // y counts twice
        } else {
            UINT y_mask = y % width;
            for (int bit = 0; bit < 16; bit++)
                sy |= ((y_mask >> bit) & 1) << (2 * bit);
            sy <<= 1; // y counts twice
            sy += (y / width) * width * width;
        }
        BYTE* s = (BYTE*)src + y * width * depth;
        for (UINT x = 0; x < width; x++) {
            UINT sx = 0;
            if (x < height * 2) {
                for (int bit = 0; bit < 16; bit++)
                    sx |= ((x >> bit) & 1) << (2 * bit);
            } else {
                int x_mask = x % (2 * height);
                for (int bit = 0; bit < 16; bit++)
                    sx |= ((x_mask >> bit) & 1) << (2 * bit);
                sx += (x / (2 * height)) * 2 * height * height;
            }
            BYTE* d = (BYTE*)dest + (sx + sy) * depth;
            for (unsigned int i = 0; i < depth; ++i)
                *d++ = *s++;
        }
    }
}

image* drawing::createImage(uint8_t* imageData, D3DFORMAT format, int width, int height) {
    image* imageToAdd = new image();
    imageToAdd->width = width;
    imageToAdd->height = height;

    if (FAILED(D3DXCreateTexture(context::getD3dDevice(), imageToAdd->width, imageToAdd->height, 1, 0, format,
            D3DPOOL_DEFAULT, &imageToAdd->texture))) {
        delete (imageToAdd);
        return NULL;
    }

    D3DSURFACE_DESC surfaceDesc;
    imageToAdd->texture->GetLevelDesc(0, &surfaceDesc);

    imageToAdd->uvRect =
        math::rectF(0, 0, imageToAdd->width / (float)surfaceDesc.Width, imageToAdd->height / (float)surfaceDesc.Height);

    D3DLOCKED_RECT lockedRect;
    if (SUCCEEDED(imageToAdd->texture->LockRect(0, &lockedRect, NULL, 0))) {
        uint8_t* tempBuffer = (uint8_t*)malloc(surfaceDesc.Size);
        memset(tempBuffer, 0, surfaceDesc.Size);
        uint8_t* src = imageData;
        uint8_t* dst = tempBuffer;
        for (int32_t y = 0; y < imageToAdd->height; y++) {
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

void drawing::addImage(const char* key, uint8_t* imageData, D3DFORMAT format, int width, int height) {
    image* imageToAdd = createImage(imageData, format, width, height);
    context::getImageMap()->add(key, imageToAdd);
}

uint64_t drawing::getImageMemUse(const char* key) {
    image* imageInfo = context::getImageMap()->get(key);
    if (imageInfo == NULL) {
        return 0;
    }
    return utils::roundUpToNextPowerOf2(imageInfo->width) * utils::roundUpToNextPowerOf2(imageInfo->height) * 4;
}

void drawing::removeImage(const char* key) {
    image* imageToRemove = context::getImageMap()->get(key);
    if (imageToRemove != NULL) {
        context::getImageMap()->removeKey(key);
    }
}

bool drawing::loadImage(const char* buffer, uint32_t length, const char* key) {
    int width;
    int height;
    uint8_t* imageData =
        (uint8_t*)stbi_load_from_memory((const stbi_uc*)buffer, length, &width, &height, NULL, STBI_rgb_alpha);
    if (imageData == NULL) {
        return false;
    }
    addImage(key, imageData, D3DFMT_A8B8G8R8, width, height);
    free(imageData);
    return true;
}

bool drawing::loadFont(const uint8_t* data) {
    if (mFontContext == NULL) {
        mFontContext = (ssfn_t*)malloc(sizeof(ssfn_t));
        memset(mFontContext, 0, sizeof(ssfn_t));
    }

    int result = ssfn_load(mFontContext, data);
    return result == 0;
}

void drawing::clearBackground() {
    // char* backgroundName = stringUtility::formatString("background");

    uint32_t backgroundColor = theme::getBackgroundColor();
    context::getD3dDevice()->Clear(
        0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, (0xff << 24) | backgroundColor, 1.0f, 0L);

    math::rectF uvRect;
    uvRect.x = 0;
    uvRect.y = 0;
    uvRect.width = (float)xmvPlayer::getWidth();
    uvRect.height = (float)xmvPlayer::getHeight();

    int x = 0;
    int y = 0;
    int height = context::getBufferHeight();
    int width = context::getBufferWidth();

    LPDIRECT3DTEXTURE8 texture = xmvPlayer::advanceFrameForTexturing();
    if (texture != NULL) {
        vertexContainer* vertices = new vertexContainer();
        float newY = (float)context::getBufferHeight() - (y + height);
        meshUtility::appendQuadXY(vertices, math::vec3F((float)x - 0.5f, newY - 0.5f, 0),
            math::sizeF((float)width, (float)height), uvRect, 0xFFFFFFFF);

        context::getD3dDevice()->SetTexture(0, texture);
        context::getD3dDevice()->DrawPrimitiveUP(
            D3DPT_TRIANGLELIST, (vertices->size() / sizeof(vertex)) / 3, vertices->get(), sizeof(vertex));
        delete (vertices);
    } else {
        drawImage(
            "background", theme::getBackgroundImageTint(), 0, 0, context::getBufferWidth(), context::getBufferHeight());
    }

    drawImage("background-overlay", theme::getBackgroundOverlayImageTint(), 0, 0, context::getBufferWidth(),
        context::getBufferHeight());
}

bool drawing::imageExists(const char* key) {
    image* result = context::getImageMap()->get(key);
    return result != NULL;
}

image* drawing::getImage(const char* key) {
    image* result = context::getImageMap()->get(key);
    return result;
}

void drawing::drawImage(image* image, uint32_t tint, int x, int y, int width, int height) {
    if (image == NULL) {
        return;
    }
    vertexContainer* vertices = new vertexContainer();
    float newY = (float)context::getBufferHeight() - (y + height);
    meshUtility::appendQuadXY(vertices, math::vec3F((float)x - 0.5f, newY - 0.5f, 0),
        math::sizeF((float)width, (float)height), image->uvRect, tint);
    context::getD3dDevice()->SetTexture(0, image->texture);
    context::getD3dDevice()->DrawPrimitiveUP(
        D3DPT_TRIANGLELIST, (vertices->size() / sizeof(vertex)) / 3, vertices->get(), sizeof(vertex));
    delete (vertices);
}

void drawing::drawImage(image* image, uint32_t tint, int x, int y) {
    if (image == NULL) {
        return;
    }
    drawImage(image, tint, x, y, image->width, image->height);
}

void drawing::drawImage(const char* imageKey, uint32_t tint, int x, int y, int width, int height) {
    image* imageToDraw = getImage(imageKey);
    if (imageToDraw == NULL) {
        return;
    }
    drawImage(imageToDraw, tint, x, y, width, height);
}

void drawing::drawImage(const char* imageKey, uint32_t tint, int x, int y) {
    image* imageToDraw = getImage(imageKey);
    if (imageToDraw == NULL) {
        return;
    }
    drawImage(imageToDraw, tint, x, y);
}

void drawing::drawNinePatch(const char* imageKey, uint32_t tint, int x, int y, int width, int height) {
    image* imageToDraw = getImage(imageKey);
    if (imageToDraw == NULL) {
        return;
    }
    vertexContainer* vertices = new vertexContainer();
    float newY = (float)context::getBufferHeight() - (y + height);
    meshUtility::appendNinePatchXY(vertices, math::vec3F((float)x - 0.5f, newY - 0.5f, 0),
        math::sizeF((float)width, (float)height), imageToDraw->uvRect, tint);
    context::getD3dDevice()->SetTexture(0, imageToDraw->texture);
    context::getD3dDevice()->DrawPrimitiveUP(
        D3DPT_TRIANGLELIST, (vertices->size() / sizeof(vertex)) / 3, vertices->get(), sizeof(vertex));
    delete (vertices);
}

image* drawing::renderRoundedRect(const char* cacheKey, uint32_t width, uint32_t height, uint32_t cornerRadius,
    uint32_t fillColor, uint32_t strokeColor, uint32_t strokeWidth) {
    if (imageExists(cacheKey) == false) {
        uint32_t* imageData = (uint32_t*)malloc(width * height * 4);

        int widthMinus1 = width - 1;
        int heightMinus1 = height - 1;
        for (unsigned int y = 0; y < height; y++) {
            float gradient = (float)y / height;
            for (unsigned int x = 0; x < width; x++) {
                unsigned int color = 0x00000000;

                bool isInsideRect =
                    (y >= cornerRadius && y < height - cornerRadius) || (x >= cornerRadius && x < width - cornerRadius);
                int topLeftCornerLength = length(y - cornerRadius, x - cornerRadius);
                int topRightCornerLength = length(y - cornerRadius, x - (widthMinus1 - cornerRadius));
                int bottomLeftCornerLength = length(y - (heightMinus1 - cornerRadius), x - cornerRadius);
                int bottomRightCornerLength =
                    length(y - (heightMinus1 - cornerRadius), x - (widthMinus1 - cornerRadius));
                int cornerRadiusSquared = cornerRadius * cornerRadius;

                bool isInsideTopLeftCorner = topLeftCornerLength < cornerRadiusSquared;
                bool isInsideTopRightCorner = topRightCornerLength < cornerRadiusSquared;
                bool isInsideBottomLeftCorner = bottomLeftCornerLength < cornerRadiusSquared;
                bool isInsideBottomRightCorner = bottomRightCornerLength < cornerRadiusSquared;
                if (isInsideRect || isInsideTopLeftCorner || isInsideTopRightCorner || isInsideBottomLeftCorner ||
                    isInsideBottomRightCorner) {
                    color = fillColor;
                }
                imageData[(y * width) + x] = color;
            }
        }

        if (strokeWidth > 0) {
            for (uint32_t y = 0; y < height; y++) {
                int count = 0;
                for (uint32_t x = 0; x < width; x++) {
                    uint32_t color = imageData[(y * width) + x];
                    if (count == strokeWidth) {
                        break;
                    }
                    if (color > 0) {
                        imageData[(y * width) + x] = strokeColor;
                        imageData[(y * width) + (width - 1) - x] = strokeColor;
                        count++;
                    }
                }
            }

            for (uint32_t x = 0; x < width; x++) {
                int count = 0;
                for (uint32_t y = 0; y < height; y++) {
                    uint32_t color = imageData[(y * width) + x];
                    if (count == strokeWidth) {
                        break;
                    }
                    if (color > 0) {
                        imageData[(y * width) + x] = strokeColor;
                        imageData[(((height - 1) - y) * width) + x] = strokeColor;
                        count++;
                    }
                }
            }
        }
        addImage(cacheKey, (uint8_t*)imageData, D3DFMT_A8R8G8B8, width, height);
        free(imageData);
    }
    return getImage(cacheKey);
}

bitmapFont* drawing::generateBitmapFont(
    const char* fontName, int fontStyle, int fontSize, int lineHeight, int spacing, int textureDimension) {
    bitmapFont* font = new bitmapFont();
    font->charMap = new pointerMap<math::rectI*>(true);

    ssfn_select(mFontContext, SSFN_FAMILY_ANY, fontName, fontStyle, fontSize);

    int textureWidth = textureDimension;
    int textureHeight = textureDimension;

    uint32_t* imageData = (uint32_t*)malloc(textureWidth * textureHeight * 4);
    memset(imageData, 0, textureWidth * textureHeight * 4);

    int x = 2;
    int y = 2;

    char* charsToEncode = " !\"#$%&'()*+,-./"
                        "0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
                        "\xC2\xA1\xC2\xA2\xC2\xA3\xC2\xA4\xC2\xA5\xC2\xA6\xC2\xA7\xC2\xA8\xC2\xA9\xC2\xAA\xC2\xAB\xC2"
                        "\xAC\xC2\xAD\xC2\xAE\xC2\xAF\xC2\xB0\xC2\xB1\xC2\xB2\xC2\xB3\xC2\xB4\xC2\xB5\xC2\xB6";
    char* currentCharPos = charsToEncode;
    while (*currentCharPos) {
        char* nextCharPos = currentCharPos;
        uint32_t unicode = ssfn_utf8(&nextCharPos);

        int32_t length = nextCharPos - currentCharPos;
        char* currentChar = (char*)malloc(length + 1);
        memcpy(currentChar, currentCharPos, length);
        currentChar[length] = 0;

        currentCharPos = nextCharPos;

        Bounds bounds;
        int ret = ssfn_bbox(mFontContext, currentChar, &bounds.width, &bounds.height, &bounds.left, &bounds.top);
        if (ret != 0) {
            free(currentChar);
            continue;
        }

        if ((x + bounds.width + 2) > textureWidth) {
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

void drawing::measureBitmapString(bitmapFont* font, const char* message, int* width, int* height) {
    image* image = font->image;

    int xPosMax = 0;

    int xPos = 0;
    int yPos = 0;

    char* currentCharPos = (char*)message;
    while (*currentCharPos) {
        char* nextCharPos = currentCharPos;
        uint32_t unicode = ssfn_utf8(&nextCharPos);

        int32_t length = nextCharPos - currentCharPos;
        char* currentChar = (char*)malloc(length + 1);
        memcpy(currentChar, currentCharPos, length);
        currentChar[length] = 0;

        currentCharPos = nextCharPos;

        if (stringUtility::equals(currentChar, "\n", false) == true) {
            xPos = 0;
            yPos += font->lineHeight;
            continue;
        }

        char* unicodeString = stringUtility::formatString("%i", unicode);
        math::rectI* rect = font->charMap->get(unicodeString);
        free(unicodeString);
        if (rect == NULL) {
            continue;
        }

        xPos = xPos + rect->width + font->spacing;
        xPosMax = max(xPosMax, xPos);
        free(currentChar);
    }

    if (width != NULL) {
        *width = xPosMax - 2;
    }

    if (height != NULL) {
        *height = yPos + font->lineHeight;
    }
}

void drawing::measureBitmapWordWrapString(
    bitmapFont* font, const char* message, int maxWidth, int* width, int* height) {
    *height = 0;
    *width = 0;

    int textWidth;
    int textHeight;
    pointerVector<char*>* wordVector = stringUtility::split(message, " ", true);

    uint32_t index = 0;
    char* currentRow = strdup("");
    while (index < wordVector->count()) {
        char* nextRow = strlen(currentRow) > 0
            ? stringUtility::formatString("%s %s", currentRow, wordVector->get(index))
            : strdup(wordVector->get(index));
        measureBitmapString(context::getBitmapFontSmall(), nextRow, &textWidth, &textHeight);
        if (textWidth >= maxWidth) {
            if (textWidth > *width) {
                *width = textWidth;
            }
            *height += textHeight;
            utils::swapString(&currentRow, "");
            free(nextRow);
            continue;
        }
        index++;
        utils::swapString(&currentRow, nextRow);
        free(nextRow);
    }
    measureBitmapString(context::getBitmapFontSmall(), currentRow, &textWidth, &textHeight);
    if (textWidth > *width) {
        *width = textWidth;
    }
    *height += textHeight;
    free(currentRow);

    delete (wordVector);
}

pointerVector<utils::textContainer*>* drawing::wordWrapString(bitmapFont* font, const char* message, int maxWidth) {
    pointerVector<utils::textContainer*>* result = new pointerVector<utils::textContainer*>(true);

    const int MAX_LINE_WIDTH = 256;
    const int MAX_WORD_LEN = 128;

    char word_buffer[MAX_WORD_LEN];
    int word_length = 0;

    char current_line[MAX_LINE_WIDTH] = {0};
    int current_line_length = 0;

    int start_index = 0;
    int end_index = strlen(message);

    for (int index = start_index; index <= end_index; ++index) {
        char current_char = message[index];
        bool is_end_of_input = (index == end_index);
        bool is_word_delimiter = (current_char == ' ' || current_char == '\n' || is_end_of_input);

        if (!is_word_delimiter) {
            if (word_length < MAX_WORD_LEN - 1) {
                word_buffer[word_length++] = current_char;
            }
        }
        if (is_word_delimiter || is_end_of_input) {
            word_buffer[word_length] = '\0';
            if (word_length > 0) {
                char* test_line = NULL;
                if (current_line_length > 0) {
                    test_line = stringUtility::formatString("%s %s", current_line, word_buffer);
                } else {
                    test_line = strdup(word_buffer);
                }
                int textWidth;
                int textHeight;
                measureBitmapString(context::getBitmapFontSmall(), test_line, &textWidth, &textHeight);
                if (textWidth <= maxWidth) {
                    strcpy(current_line, test_line);
                    current_line_length = strlen(current_line);
                } else {
                    utils::textContainer* line = new utils::textContainer(current_line, textWidth, textHeight);
                    result->add(line);
                    strcpy(current_line, word_buffer);
                    current_line_length = strlen(current_line);
                }
                word_length = 0;
                free(test_line);
            }
            if (current_char == '\n') {
                if (current_line_length > 0) {
                    int textWidth;
                    int textHeight;
                    measureBitmapString(context::getBitmapFontSmall(), current_line, &textWidth, &textHeight);
                    utils::textContainer* line = new utils::textContainer(current_line, textWidth, textHeight);
                    result->add(line);
                    current_line[0] = '\0';
                    current_line_length = 0;
                }
                if (index + 1 < end_index && message[index + 1] == '\n') {
                    ++index;
                    utils::textContainer* line = new utils::textContainer(strdup(""), 0, font->lineHeight);
                    result->add(line);
                }
            }
            if (is_end_of_input && current_line_length > 0) {
                int textWidth;
                int textHeight;
                measureBitmapString(context::getBitmapFontSmall(), current_line, &textWidth, &textHeight);
                utils::textContainer* line = new utils::textContainer(current_line, textWidth, textHeight);
                result->add(line);
                current_line[0] = '\0';
                current_line_length = 0;
            }
        }
    }

    if (word_length > 0 && current_line_length == 0)
    {
        strcpy(current_line, word_buffer);
        current_line_length = word_length;

        int textWidth;
        int textHeight;
        measureBitmapString(context::getBitmapFontSmall(), current_line, &textWidth, &textHeight);
        utils::textContainer* line = new utils::textContainer(current_line, textWidth, textHeight);
        result->add(line);
    }
    
    return result;
}

void drawing::drawBitmapString(bitmapFont* font, const char* message, uint32_t color, int x, int y) {
    vertexContainer* vertices = new vertexContainer();
    image* image = font->image;

    int xPos = x;
    int yPos = y;

    char* currentCharPos = (char*)message;
    while (*currentCharPos) {
        char* nextCharPos = currentCharPos;
        uint32_t unicode = ssfn_utf8(&nextCharPos);

        int32_t length = nextCharPos - currentCharPos;
        char* currentChar = (char*)malloc(length + 1);
        memcpy(currentChar, currentCharPos, length);
        currentChar[length] = 0;

        currentCharPos = nextCharPos;

        if (stringUtility::equals(currentChar, "\n", false) == true) {
            free(currentChar);
            xPos = x;
            yPos += font->lineHeight;
            continue;
        }

        char* unicodeString = stringUtility::formatString("%i", unicode);
        math::rectI* rect = font->charMap->get(unicodeString);
        free(unicodeString);
        if (rect == NULL) {
            free(currentChar);
            continue;
        }

        math::rectF uvRect;
        uvRect.x = rect->x / (float)image->width;
        uvRect.y = rect->y / (float)image->height;
        uvRect.width = rect->width / (float)image->width;
        uvRect.height = rect->height / (float)image->height;

        uint32_t charColor = color;
        if (unicode == 161) {
            charColor = theme::getJoyButtonAColor();
        } else if (unicode == 162) {
            charColor = theme::getJoyButtonBColor();
        } else if (unicode == 163) {
            charColor = theme::getJoyButtonXColor();
        } else if (unicode == 164) {
            charColor = theme::getJoyButtonYColor();
        } else if (unicode == 182) {
            charColor = theme::getJoyButtonBlackColor();
        } else if (unicode == 181) {
            charColor = theme::getJoyButtonWhiteColor();
        }

        float newY = (float)context::getBufferHeight() - (yPos + rect->height);
        meshUtility::appendQuadXY(vertices, math::vec3F((float)xPos - 0.5f, newY - 0.5f, 0),
            math::sizeF((float)rect->width, (float)rect->height), uvRect, charColor);

        xPos = xPos + rect->width + font->spacing;
        free(currentChar);
    }

    if (vertices->size() > 0) {
        context::getD3dDevice()->SetTexture(0, image->texture);
        context::getD3dDevice()->DrawPrimitiveUP(
            D3DPT_TRIANGLELIST, (vertices->size() / sizeof(vertex)) / 3, vertices->get(), sizeof(vertex));
    }
    delete (vertices);
}

void drawing::drawHorizontalLine(uint32_t color, int x, int y, int width, int height)
{
    char* cacheKey = stringUtility::formatString("drawHorizontalLine-%02x-%02x", width, height);
    image* image = renderHorizontalLine(cacheKey, color, width, height);
    drawImage(image, color, x, y);
    free(cacheKey);
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

void drawing::drawBitmapStringAligned(
    bitmapFont* font, const char* message, uint32_t color, horizAlignment hAlign, int x, int y, int width) {
    int textWidth = 0;
    measureBitmapString(font, message, &textWidth, NULL);

    int xPos = x;
    if (hAlign == horizAlignmentCenter) {
        xPos = x + ((width - textWidth) / 2);
    } else if (hAlign == horizAlignmentRight) {
        xPos = x + (width - textWidth);
    }

    drawBitmapString(font, message, color, xPos, y);
}

namespace {

typedef struct JpgMemoryWriter {
    char* data;
    int size;
    int capacity;
} JpgMemoryWriter;

static void jpgMemoryWrite(void* context, void* data, int size) {
    JpgMemoryWriter* writer = (JpgMemoryWriter*)context;
    const int needed = writer->size + size;
    if (needed > writer->capacity) {
        int newCapacity = writer->capacity == 0 ? 65536 : writer->capacity * 2;
        while (newCapacity < needed) {
            newCapacity *= 2;
        }
        writer->data = (char*)realloc(writer->data, newCapacity);
        writer->capacity = newCapacity;
    }
    memcpy(writer->data + writer->size, data, size);
    writer->size += size;
}

static uint8_t* captureBackBufferRgba(uint32_t* outWidth, uint32_t* outHeight) {
    uint8_t* linear = NULL;

    IDirect3DSurface8* backBuffer = NULL;
    context::getD3dDevice()->GetBackBuffer(-1, D3DBACKBUFFER_TYPE_MONO, &backBuffer);
    D3DSURFACE_DESC desc;
    backBuffer->GetDesc(&desc);

    IDirect3DSurface8* linearSurface = NULL;
    IDirect3DSurface8* surface = backBuffer;
    if (desc.Width != 1920 || desc.Height != 1080) {
        if (context::getD3dDevice()->CreateImageSurface(desc.Width, desc.Height, D3DFMT_LIN_X8R8G8B8, &linearSurface) >= 0) {
            context::getD3dDevice()->CopyRects(backBuffer, NULL, 0, linearSurface, NULL);
            surface = linearSurface;
        }
    }

    D3DLOCKED_RECT lockedRect;
    if (surface->LockRect(&lockedRect, NULL, D3DLOCK_READONLY) >= 0) {
        const uint32_t w = desc.Width;
        const uint32_t h = desc.Height;
        linear = (uint8_t*)malloc(w * h * 4);
        if (linear != NULL) {
            for (uint32_t y = 0; y < h; ++y) {
                uint8_t* srcRow = (uint8_t*)lockedRect.pBits + y * lockedRect.Pitch;
                uint8_t* dstRow = linear + y * w * 4;
                for (uint32_t x = 0; x < w; ++x) {
                    uint8_t* src = srcRow + x * 4;
                    uint8_t* dst = dstRow + x * 4;
                    dst[0] = src[2];
                    dst[1] = src[1];
                    dst[2] = src[0];
                    dst[3] = 255;
                }
            }
        }
        *outWidth = w;
        *outHeight = h;
        surface->UnlockRect();
    }

    backBuffer->Release();
    if (linearSurface != NULL) {
        linearSurface->Release();
    }

    return linear;
}

static uint8_t* scaleRgbaToRgbHeight(
    const uint8_t* src, uint32_t srcWidth, uint32_t srcHeight, uint32_t targetHeight, uint32_t* outWidth) {
    const uint32_t targetWidth = (uint32_t)(((uint64_t)srcWidth * targetHeight) / srcHeight);
    uint8_t* rgb = (uint8_t*)malloc(targetWidth * targetHeight * 3);
    if (rgb == NULL) {
        return NULL;
    }

    for (uint32_t y = 0; y < targetHeight; ++y) {
        const uint32_t srcY = (uint32_t)(((uint64_t)y * srcHeight) / targetHeight);
        const uint8_t* srcRow = src + srcY * srcWidth * 4;
        uint8_t* dstRow = rgb + y * targetWidth * 3;
        for (uint32_t x = 0; x < targetWidth; ++x) {
            const uint32_t srcX = (uint32_t)(((uint64_t)x * srcWidth) / targetWidth);
            const uint8_t* srcPixel = srcRow + srcX * 4;
            uint8_t* dstPixel = dstRow + x * 3;
            dstPixel[0] = srcPixel[0];
            dstPixel[1] = srcPixel[1];
            dstPixel[2] = srcPixel[2];
        }
    }

    *outWidth = targetWidth;
    return rgb;
}

} // namespace

utils::dataContainer* drawing::takeScreenshot() {
    uint32_t width = 0;
    uint32_t height = 0;
    uint8_t* linear = captureBackBufferRgba(&width, &height);
    if (linear == NULL) {
        return NULL;
    }

    int fileSize = 0;
    char* pngData = (char*)stbi_write_png_to_mem(linear, width * 4, width, height, 4, &fileSize);
    free(linear);

    if (pngData == NULL) {
        return NULL;
    }

    utils::dataContainer* result = new utils::dataContainer(pngData, fileSize, fileSize);
    free(pngData);
    return result;
}

utils::dataContainer* drawing::takeRemoteViewFrame() {
    const uint32_t targetHeight = 360;
    uint32_t width = 0;
    uint32_t height = 0;
    uint8_t* linear = captureBackBufferRgba(&width, &height);
    if (linear == NULL || height == 0) {
        free(linear);
        return NULL;
    }

    uint32_t scaledWidth = 0;
    uint8_t* rgb = scaleRgbaToRgbHeight(linear, width, height, targetHeight, &scaledWidth);
    free(linear);
    if (rgb == NULL) {
        return NULL;
    }

    JpgMemoryWriter writer = {NULL, 0, 0};
    if (stbi_write_jpg_to_func(jpgMemoryWrite, &writer, scaledWidth, targetHeight, 3, rgb, 75) == 0 || writer.data == NULL) {
        free(rgb);
        free(writer.data);
        return NULL;
    }
    free(rgb);

    utils::dataContainer* result = new utils::dataContainer(writer.data, writer.size, writer.size);
    free(writer.data);
    return result;
}