/*-
 * License
 */

#include <stdbool.h>
#include <stdio.h>

#import <CoreGraphics/CoreGraphics.h>
#import <QuartzCore/QuartzCore.h>
#import <UIKit/UIKit.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "event.h"
#include "ifopen.h"
#include "system.h"

#import "GameView.h"

static char *argv[] = {"conceptengine.app", "LD19/main.lua"};

@implementation GameView

+ (Class) layerClass
{
	return [CAEAGLLayer class];
}

- (id)
initWithFrame:(CGRect)frame
{
	CAEAGLLayer *gl_layer;

	if (self = [super initWithFrame:frame]) {
		/* Init OpenGL */
		gl_layer = (CAEAGLLayer*)self.layer;
		gl_layer.opaque = YES;
		gl_layer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
		
		context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
		assert(context);
		
		/* Enable multiple touches */
		[self setMultipleTouchEnabled:YES];
		
		/* Nullify buffer info */
		bzero(&buffer, sizeof(buffer));
		
		quit = NO;
	}
	
	return self;
}

- (void)
layoutSubviews
{
	
	assert( [EAGLContext setCurrentContext:context] == YES );

	buffer.renderbuffer = r_generate_renderbuffer();
	buffer.framebuffer = r_generate_framebuffer();
	r_bind_buffers(&buffer);
	assert( [context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(id<EAGLDrawable>)self.layer]
		== YES );
	
	/*glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES,
									GL_RENDERBUFFER_WIDTH_OES, &buffer.width);
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES,
									GL_RENDERBUFFER_HEIGHT_OES, &buffer.height);
	*/
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES,
								 GL_RENDERBUFFER_OES, buffer.renderbuffer);
	/*buffer.depth = r_generate_depthbuffer(buffer.width, buffer.height);
	r_bind_buffers(&buffer);*/
	/* Make sure there is no error in OpenGL stuff */
	assert(glGetError() == GL_NO_ERROR);

	/* All systems go! */
	sys_start(2, argv);
	[self update];
}

/* Convert touch point from UIView referential to OpenGL one (upside-down flip) */
- (CGPoint)
pointToOpenGL:(CGPoint)location
{
	CGFloat tmp = location.y;
	
	location.y = location.x;
	location.x = tmp;
	/*location.x -= [self bounds].size.height/2.0f;
	location.y -= [self bounds].size.width/2.0f;*/
	
	return location;
}

- (void)
touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	CGPoint location;
	NSArray *tmp;
	int i, j;
	union event ev;
	
	ev.type = TOUCHDOWN;
	tmp = [touches allObjects];
	for (i = 0; i < [tmp count]; ++i) {
		UITouch *touch = [tmp objectAtIndex:i];
		
		location = [self pointToOpenGL:[touch locationInView:self]];
		ev.touch.dx = location.x;
		ev.touch.dy = location.y;
		event_push(ev);
	}
}

- (void)
touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	CGPoint location;
	NSArray *tmp;
	union event ev;
	int i, j;
	
	ev.type = TOUCHUP;
	tmp = [touches allObjects];
	for (i = 0; i < [tmp count]; ++i) {
		UITouch *touch = [tmp objectAtIndex:i];
		
		location = [self pointToOpenGL:[touch locationInView:self]];
		ev.touch.dx = location.x;
		ev.touch.dy = location.y;
		event_push(ev);
	}
}

- (void)
touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	CGPoint location;
	NSArray *tmp;
	int		i, j;
	union event ev;
	
	//touch = [[event touchesForView:self] anyObject];
	ev.type = TOUCHMOVE;
	tmp = [touches allObjects];
	for (i = 0; i < [tmp count]; ++i) {
		UITouch *touch = [tmp objectAtIndex:i];
		
		location = [self pointToOpenGL:[touch locationInView:self]];
		ev.touch.dx = location.x;
		ev.touch.dy = location.y;
		location = [self pointToOpenGL:[touch previousLocationInView:self]];
		ev.touch.sx = location.x;
		ev.touch.sy = location.y;
	}
}

/*
 * Main loop
 */
- (void)
update
{
	assert([EAGLContext setCurrentContext:context] == YES);
	
	//r_bind_buffers(&buffer);
	if (NO == quit && 0 != sys_update()) {
		sys_quit();
		printf("lua> quit\n");
		quit = YES;
	}
	
	assert([context presentRenderbuffer:GL_RENDERBUFFER_OES] == YES);
}

@end

