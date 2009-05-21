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

#include "game.h"
#include "game_helper.h"
#include "rendering.h"
#include "vec3.h"

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
	
	if ( [EAGLContext setCurrentContext:context] == NO )
		NSLog(@"Helvete");

	r_generate_renderbuffers(&buffer);
	r_bind_buffers(&buffer);
	if ( [context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(id<EAGLDrawable>)self.layer]
		== NO )
		NSLog(@"HELLVETTE");
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES,
								 GL_RENDERBUFFER_OES, buffer.render);
	r_generate_depthbuffer(&buffer);
	r_bind_buffers(&buffer);
	
	UIAlertView *hello = [[UIAlertView alloc] initWithTitle:@"Yo maddafakkas!"
		message:@"Let's rock!" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
	[hello show];
	[hello release];
}

/*
 * Main loop
 */
- (void)
update
{
	game_update();

	[EAGLContext setCurrentContext:context];
	game_render(&buffer);
	[context presentRenderbuffer:GL_RENDERBUFFER_OES];
}

@end

