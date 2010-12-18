/*
 Copyright (c) 2009 Ben Hopkins
 
 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:
 
 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdint.h>

#include "rendering.h"
#include "quat.h"
#include "vec3.h"

#define MD2_MAGIC_NUMBER	844121161
#define MD2_VERSION		8

typedef struct vec2 {
	float x, y;
} vec2;

typedef struct _glvertex {
	vec3 position;
	vec3 normal;
	quat color;
	vec2 textureCoords;
} GLVertex;

// ---- MD2 Types ------------------------------//

typedef struct
{
	int identifier;
	int version;
	int skinWidth;
	int skinHeight;
	int frameSize;
	int skinCount;
	int vertexCount;
	int textureCoordinateCount;
	int triangleCount;
	int glCommandCount;
	int frameCount;
	int skinsOffset;
	int textureCoordinatesOffset;
	int trianglesOffset;
	int framesOffset;
	int glCommandsOffset;
	int eofOffset;
} KEMD2Header;

typedef char KEMD2TexturePath[64];

typedef struct
{
	unsigned char xyz[3];
	unsigned char lightnormalindex;
} KEMD2Vertex;

typedef struct
{
	short s;
	short t;
} KEMD2TextureCoordinate;

typedef struct
{
	short vertexIndices[3];
	short textureCoordinateIndices[3];
} KEMD2Triangle;

typedef char KEMD2FrameName[16];

typedef struct
{
	KEMD2FrameName name;
	GLVertex *vertices;
} KEMD2Frame;

typedef struct
{
	int firstFrame;
	int lastFrame;
	float fps;
} KEMD2Animation;

// ---- Standard Quake animations ---------------//

extern KEMD2Animation const KEMD2AnimationStand;
extern KEMD2Animation const KEMD2AnimationRun;
extern KEMD2Animation const KEMD2AnimationAttack;
extern KEMD2Animation const KEMD2AnimationPainA;
extern KEMD2Animation const KEMD2AnimationPainB;
extern KEMD2Animation const KEMD2AnimationPainC;
extern KEMD2Animation const KEMD2AnimationJump;
extern KEMD2Animation const KEMD2AnimationFlip;
extern KEMD2Animation const KEMD2AnimationSalute;
extern KEMD2Animation const KEMD2AnimationFallBack;
extern KEMD2Animation const KEMD2AnimationWave;
extern KEMD2Animation const KEMD2AnimationPoint;
extern KEMD2Animation const KEMD2AnimationCrouchStand;
extern KEMD2Animation const KEMD2AnimationCrouchWalk;
extern KEMD2Animation const KEMD2AnimationCrouchAttack;
extern KEMD2Animation const KEMD2AnimationCrouchPain;
extern KEMD2Animation const KEMD2AnimationCrouchDeath;
extern KEMD2Animation const KEMD2AnimationDeathFallBack;
extern KEMD2Animation const KEMD2AnimationDeathFallForward;
extern KEMD2Animation const KEMD2AnimationDeathFallBackSlow;
extern KEMD2Animation const KEMD2AnimationBoom;


typedef struct KEMD2Object {
	KEMD2Header header;
	KEMD2TexturePath *skins;
	KEMD2TextureCoordinate *textureCoordinates;
	KEMD2Triangle *triangles;
	KEMD2Frame *frames;
	KEMD2Animation animation;
	float oldTime;
	float fpsScale;
	int currentFrame;
	int nextFrame;
	
	GLVertex *vertices;
	uint16_t texture;
} KEMD2Object;

void 			md2_cleanup_render();
void 			md2_free(KEMD2Object *o);
KEMD2Object* 	md2_open(const char *path, uint16_t texture);
void 			md2_print(KEMD2Object *o);
void 			md2_render(KEMD2Object *o);
void 			md2_set_animation(KEMD2Object *o, KEMD2Animation animation);
//void 			md2_set_animation(KEMD2Object *o, int first, int last,
//					float fps);
void 			md2_setup_render(KEMD2Object *o);
void 			md2_tick(KEMD2Object *o, float time);

