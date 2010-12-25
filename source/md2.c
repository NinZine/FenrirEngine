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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ifopen.h"
#include "md2.h"

#define NUMVERTEXNORMALS        162
#define SHADEDOT_QUANT          16

static vec3 anorms_table[NUMVERTEXNORMALS] = {
	#include "anorms.h"
};

KEMD2Animation const KEMD2AnimationStand = { 0,  39,  9 };
KEMD2Animation const KEMD2AnimationRun = {  40,  46, 10 };
KEMD2Animation const KEMD2AnimationAttack = {  47,  53, 10 };
KEMD2Animation const KEMD2AnimationPainA = {  54,  57,  7 };
KEMD2Animation const KEMD2AnimationPainB = {  58,  61,  7 };
KEMD2Animation const KEMD2AnimationPainC = {  62,  65,  7 };
KEMD2Animation const KEMD2AnimationJump = {  66,  71,  7 };
KEMD2Animation const KEMD2AnimationFlip = {  72,  83,  7 };
KEMD2Animation const KEMD2AnimationSalute = {  84,  94,  7 };
KEMD2Animation const KEMD2AnimationFallBack = {  95, 111, 10 };
KEMD2Animation const KEMD2AnimationWave = { 112, 122,  7 };
KEMD2Animation const KEMD2AnimationPoint = { 123, 134,  6 };
KEMD2Animation const KEMD2AnimationCrouchStand = { 135, 153, 10 };
KEMD2Animation const KEMD2AnimationCrouchWalk = { 154, 159,  7 };
KEMD2Animation const KEMD2AnimationCrouchAttack = { 160, 168, 10 };
KEMD2Animation const KEMD2AnimationCrouchPain = { 196, 172,  7 };
KEMD2Animation const KEMD2AnimationCrouchDeath = { 173, 177,  5 };
KEMD2Animation const KEMD2AnimationDeathFallBack = { 178, 183,  7 };
KEMD2Animation const KEMD2AnimationDeathFallForward = { 184, 189,  7 };
KEMD2Animation const KEMD2AnimationDeathFallBackSlow = { 190, 197,  7 };
KEMD2Animation const KEMD2AnimationBoom = { 198, 198,  5 };


void md2_set_animation(KEMD2Object *o, KEMD2Animation animation)
{
	o->animation = animation;
	o->nextFrame = o->animation.firstFrame;
}

/*void md2_set_animation(KEMD2Object *o, int first, int last, float fps)
{
	KEMD2Animation animation = {first, last, fps};
	o->animation = animation;
	o->nextFrame = o->animation.firstFrame;
}*/

KEMD2Object* md2_open(const char *path, uint16_t texture)
{
	FILE *fp;
	KEMD2Object *o;

	fp = ifopen(path, "rb");
	o = malloc(sizeof(KEMD2Object));
	o->texture = texture;
	o->oldTime = 0;
	o->currentFrame = 0;
	o->nextFrame = 1;
	o->fpsScale = 1;
	
	fread(&o->header, sizeof(KEMD2Header), 1, fp);
	if( o->header.identifier != MD2_MAGIC_NUMBER
	|| o->header.version != MD2_VERSION) {
		free(o);
		return NULL;
	}
	
	size_t skinsSize = o->header.skinCount * sizeof(KEMD2TexturePath);
	size_t textureCoordinatesSize = o->header.textureCoordinateCount
		* sizeof(KEMD2TextureCoordinate);
	size_t trianglesSize = o->header.triangleCount * sizeof(KEMD2Triangle);
	size_t framesSize = o->header.frameCount * sizeof(KEMD2Frame);
	
	o->skins = malloc( skinsSize);
	o->textureCoordinates = malloc( textureCoordinatesSize);
	o->triangles = malloc( trianglesSize);
	o->frames = malloc( framesSize);
	KEMD2Vertex *vertexBuffer = malloc(
		o->header.vertexCount * sizeof(KEMD2Vertex));
	
	if( !o->skins || !o->textureCoordinates || !o->triangles
	|| !o->frames || !vertexBuffer) {
		if(o->skins) free(o->skins);
		if(o->textureCoordinates) free(o->textureCoordinates);
		if(o->triangles) free(o->triangles);
		if(o->frames) free(o->frames);
		if(vertexBuffer) free(vertexBuffer);
		free(o);
		printf("Couldn't allocate memory");
		return NULL;
	}
	
	fseek(fp, o->header.skinsOffset, SEEK_SET);
	fread(o->skins, skinsSize, 1, fp);
	fseek(fp, o->header.textureCoordinatesOffset, SEEK_SET);
	fread(o->textureCoordinates, textureCoordinatesSize, 1, fp);
	fseek(fp, o->header.trianglesOffset, SEEK_SET);
	fread(o->triangles, trianglesSize, 1, fp);
	
	size_t verticesSize = o->header.triangleCount * sizeof(GLVertex) * 3;
	char couldntAllocateFrame = 0;
	vec3 frameScale;
	vec3 frameTranslate;
	KEMD2Vertex *md2Vertex;
	GLVertex *glVertex;
	KEMD2TextureCoordinate *textureCoordinate;
	int i, j, k;

	fseek(fp, o->header.framesOffset, SEEK_SET);
	for(i=0; i < o->header.frameCount; ++i) {
		o->frames[i].vertices = malloc(verticesSize);
		if(!o->frames[i].vertices) {
			couldntAllocateFrame = 1;
			break;
		}
		
		fread(&frameScale, sizeof(vec3), 1, fp);
		fread(&frameTranslate, sizeof(vec3), 1, fp);
		fread(o->frames[i].name, sizeof(KEMD2FrameName), 1, fp);
		fread(vertexBuffer, sizeof(KEMD2Vertex), o->header.vertexCount, fp);

		for( j=0; j<o->header.triangleCount; ++j) {
			for( k=0; k<3; ++k) {
				md2Vertex = &vertexBuffer[o->triangles[j].vertexIndices[k]];
				textureCoordinate =  &o->textureCoordinates[
					o->triangles[j].textureCoordinateIndices[k]];
				
				glVertex = &o->frames[i].vertices[j*3+k];
				glVertex->position.x = (frameScale.x * md2Vertex->xyz[0])
					+ frameTranslate.x;
				glVertex->position.y = (frameScale.y * md2Vertex->xyz[1])
					+ frameTranslate.y;
				glVertex->position.z = (frameScale.z * md2Vertex->xyz[2])
					+ frameTranslate.z;
				glVertex->textureCoords.x = (GLfloat) textureCoordinate->s
					/ o->header.skinWidth;
				glVertex->textureCoords.y = 1.0f - ((GLfloat) textureCoordinate->t
					/ o->header.skinHeight);
				glVertex->normal.x = anorms_table[
					md2Vertex->lightnormalindex].x;
				glVertex->normal.y = anorms_table[
					md2Vertex->lightnormalindex].y;
				glVertex->normal.z = anorms_table[
					md2Vertex->lightnormalindex].z;
			}
		}
	}
	free(vertexBuffer);
	
	o->vertices = malloc(verticesSize);
	
	if(couldntAllocateFrame || !o->vertices) {
		for( i=0; i < o->header.frameCount; i++) {
			if(o->frames[i].vertices)
				free(o->frames[i].vertices);
		}

		free(o->skins);
		free(o->textureCoordinates);
		free(o->triangles);
		free(o->frames);
		free(o);
		printf("Couldn't allocate memory");
		return NULL;
	}
	
	memcpy(o->vertices, o->frames[0].vertices, verticesSize);
	o->animation = KEMD2AnimationCrouchWalk;

	return o;
}

void md2_tick(KEMD2Object *o, float time)
{
	KEMD2Animation anim = o->animation;
	float fps = anim.fps * o->fpsScale;
	
	if(time - o->oldTime > 1.0 / fps) {
		o->currentFrame = o->nextFrame;
		o->nextFrame++;
		o->oldTime = time;
	}
	
	if(o->currentFrame >= anim.lastFrame) {
		o->currentFrame = anim.firstFrame;
	}

	if(o->nextFrame >= anim.lastFrame) {
		o->nextFrame = anim.firstFrame;
	}
	
	float alpha = fps * (time - o->oldTime);
	GLVertex *currentVertices = o->frames[o->currentFrame].vertices;
	GLVertex *nextVertices = o->frames[o->nextFrame].vertices;
	
	int vertexCount = o->header.triangleCount * 3;
	int i;
	for(i=0; i<vertexCount; i++) {
		o->vertices[i].position.x = currentVertices[i].position.x + alpha
			* (nextVertices[i].position.x - currentVertices[i].position.x);
		o->vertices[i].position.y = currentVertices[i].position.y + alpha
			* (nextVertices[i].position.y - currentVertices[i].position.y);
		o->vertices[i].position.z = currentVertices[i].position.z + alpha
			* (nextVertices[i].position.z - currentVertices[i].position.z);
		o->vertices[i].normal.x = currentVertices[i].normal.x + alpha
			* (nextVertices[i].normal.x - currentVertices[i].normal.x);
		o->vertices[i].normal.y = currentVertices[i].normal.y + alpha
			* (nextVertices[i].normal.y - currentVertices[i].normal.y);
		o->vertices[i].normal.z = currentVertices[i].normal.z + alpha
			* (nextVertices[i].normal.z - currentVertices[i].normal.z);
	}
}

void md2_free(KEMD2Object *o)
{
	int i;

	for(i=0; i<o->header.frameCount; ++i) {
		free(o->frames[i].vertices);
	}

	free(o->skins);
	free(o->textureCoordinates);
	free(o->triangles);
	free(o->frames);
	free(o->vertices);
	r_free_texture(o->texture);
	o->texture = 0;
}

void md2_print(KEMD2Object *o)
{
	printf("MD2:\n\tskin width: %d\n\tskin height: %d\n\tframes: %d\n\tskins:\
	%d\n\tvertices: %d\n\ttex coords: %d\n\ttris: %d\n\tgl commands: %d",
		o->header.skinWidth,
		o->header.skinHeight,
		o->header.frameCount,
		o->header.skinCount,
		o->header.vertexCount,
		o->header.textureCoordinateCount,
		o->header.triangleCount,
		o->header.glCommandCount);
}

void md2_setup_render(KEMD2Object *o)
{	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, o->texture);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	//glEnableClientState( GL_COLOR_ARRAY);
}

void md2_render(KEMD2Object *o)
{
	glVertexPointer( 3, GL_FLOAT, sizeof(GLVertex), &o->vertices[0].position);
	glTexCoordPointer( 2, GL_FLOAT, sizeof(GLVertex),
		&o->vertices[0].textureCoords);
	glNormalPointer( GL_FLOAT, sizeof(GLVertex), &o->vertices[0].normal);
	//glColorPointer( 4, GL_FLOAT, sizeof( GLVertex), &_vertices[0].color);
	glDrawArrays(GL_TRIANGLES, 0, o->header.triangleCount * 3);
}

void md2_cleanup_render()
{
	glDisableClientState( GL_VERTEX_ARRAY);
	glDisableClientState( GL_TEXTURE_COORD_ARRAY);
	glDisableClientState( GL_NORMAL_ARRAY);
	//glDisableClientState( GL_COLOR_ARRAY);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable( GL_TEXTURE_2D);
}

