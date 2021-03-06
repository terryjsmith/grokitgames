#ifndef defines_h
#define defines_h

// Color types
extern int COLOR_RED;
extern int COLOR_RED16F;
extern int COLOR_RGB;
extern int COLOR_RGBA;
extern int COLOR_RGB16F;
extern int COLOR_RED32F;
extern int COLOR_DEPTH_COMPONENT24;
extern int COLOR_DEPTH_COMPONENT32F;
extern int COLOR_DEPTH_COMPONENT;

extern int TEXTURE_TYPE_BYTE;
extern int TEXTURE_TYPE_FLOAT;

// Value tests
extern int TEST_LESS;
extern int TEST_EQUAL;
extern int TEST_GREATER;
extern int TEST_LEQUAL;
extern int TEST_GEQUAL;
extern int TEST_ALWAYS;

// Filters
extern int FILTER_NEAREST;
extern int FILTER_LINEAR;
extern int FILTER_MIPMAP;

// Clear bits
extern int COLOR_BUFFER_BIT;
extern int DEPTH_BUFFER_BIT;
extern int STENCIL_BUFFER_BIT;

// Drawing types
extern int DRAW_TRIANGLES;
extern int DRAW_TRIANGLE_STRIP;

// Rendering destinations
extern int DRAW_BUFFER_NONE;
extern int DRAW_BUFFER_FRONT;
extern int DRAW_BUFFER_BACK;

// Blend types
extern int BLEND_ZERO;
extern int BLEND_ONE;
extern int BLEND_SRC_COLOR;
extern int BLEND_ONE_MINUS_SRC_COLOR;
extern int BLEND_SRC_ALPHA;
extern int BLEND_ONE_MINUS_SRC_ALPHA;
extern int BLEND_DST_ALPHA;
extern int BLEND_ONE_MINUS_DST_ALPHA;

// Texture repeat modes
extern int TEXTURE_REPEAT_X;
extern int TEXTURE_REPEAT_Y;

extern int TEXTURE_REPEAT_MODE_REPEAT;
extern int TEXTURE_REPEAT_MODE_CLAMP;

extern int CUBEMAP_FACE_RIGHT;
extern int CUBEMAP_FACE_LEFT;
extern int CUBEMAP_FACE_TOP;
extern int CUBEMAP_FACE_BOTTOM;
extern int CUBEMAP_FACE_BACK;
extern int CUBEMAP_FACE_FRONT;

extern int CULLMODE_FRONT;
extern int CULLMODE_BACK;

#endif
