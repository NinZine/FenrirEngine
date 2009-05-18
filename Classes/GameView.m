//
//  GameView.m
//  Demo2
//
//  Created by Andreas Kröhnke on 5/13/09.
//  Copyright 2009 BTH. All rights reserved.
//

#import <CoreGraphics/CoreGraphics.h>
#import <QuartzCore/QuartzCore.h>

#import "GameView.h"

#include "GameHelper.h"
#include "MathHelper.h"
#include "RenderHelper.h"

/* TEMP */
GLuint my_fbo, texture;
/* !TEMP */

@implementation GameView

+ (Class) layerClass
{
	return [CAEAGLLayer class];
}

- (id)
initWithFrame:(CGRect)frame
{
	CAEAGLLayer *gl_layer;

	[super initWithFrame:frame];

	/* Init OpenGL */
	gl_layer = (CAEAGLLayer*)self.layer;
	gl_layer.opaque = YES;
	gl_layer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
	
	context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
	assert(context);
	
	bzero(&buffer, sizeof(buffer));
	
	return self;
}

- (void)
layoutSubviews
{
	Vector pos = {25.0f, 100.0f, 25.0f};
	struct color color = {1.0f, 0.5f, 0.5f, 0.5f};
	
	if ( [EAGLContext setCurrentContext:context] == NO )
		NSLog(@"Helvete");

	generate_renderbuffers(&buffer);
	bind_buffers(&buffer);
	if ( [context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(id<EAGLDrawable>)self.layer]
		== NO )
		NSLog(@"HELLVETTE");
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES,
								 GL_RENDERBUFFER_OES, buffer.render);
	generate_depthbuffer(&buffer);
	bind_buffers(&buffer);
	
	UIAlertView *hello = [[UIAlertView alloc] initWithTitle:@"Yo maddafakkas!"
		message:@"Let's rock!" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
	[hello show];
	[hello release];
	
	enable_lighting();
	setup_ambient_light(pos, color);
	
	/*
	glGenFramebuffersOES(1, &my_fbo);
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, my_fbo);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8_OES, buffer.width, buffer.height, 0,
				 GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2DOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES,
							  GL_TEXTURE_2D, texture, 0);*/
}

/*
 * Main loop
 */
- (void)
update
{
	static Vector prev = {0,0,0};
	static Vector curr = {0,0,0};
	static Time game_time = {0, 0, 0, 0, 0, 1.f / 2.f};
	static clock_t time_prev = 0;
	clock_t time_now = clock();
	
	game_time.now = time_elapsed();
	game_time.delta = game_time.now - game_time.absolute;
	
	if (game_time.delta <= 0.0f)
		return;
	
	game_time.absolute = game_time.now;
	game_time.accumulator += game_time.delta;
	
	while (game_time.accumulator >= game_time.timestep) {
		
		prev = curr;
		curr.x += 0.f;
		curr.y += 5.f;
		curr.z += 0.f;
		
		time_prev = time_now;
		game_time.accumulator -= game_time.timestep;
	}
	
	/*
	 * Rendering part
	 */
	[EAGLContext setCurrentContext:context];
	
	bind_buffers(&buffer);
	glViewport(0, 0, buffer.width, buffer.height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	setup_orthogonal(buffer.width, buffer.height);
	//glRotatef(90.0f, 0.0f, 0.0f, -1.0f); // For landscape mode
	
	// Clear buffers
	glEnable(GL_DEPTH_TEST);
	glClearColor(.14f, .14f, .14f, 1.f);
	glScissor(140, 0, 40, buffer.height);
	glEnable(GL_SCISSOR_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	float_t interpolate = 1.0f - game_time.accumulator/game_time.timestep;
	Vector w = vector_lerp(&curr, &prev, interpolate);
	
	glColor4f(1.f, 1.f, 1.f, 1.f);
	glPushMatrix();
	glTranslatef(w.x, w.y, w.z);
	render_sphere(200);
	glPopMatrix();
	
	glRotatef(180.f, 0.f, 0.f, 1.f);
	glTranslatef(w.x, w.y, w.z);
	render_circle(20);
	
	[context presentRenderbuffer:GL_RENDERBUFFER_OES];
}

@end
