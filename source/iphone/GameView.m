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

static vec3 left_stick_position = {-120.f, -60.f, 0.f};
static vec3 left_stick_rotation = {0.f, 0.f, 0.f};
static float left_stick_size = 60.f;

static float rad2deg (float rad)
{
	static const float PI = 3.14159265359;
	return rad * (180.f / PI);
}

@implementation GameView

+ (Class) layerClass
{
	return [CAEAGLLayer class];
}

- (id)
initWithFrame:(CGRect)frame
{
	CAEAGLLayer *gl_layer;

	self = [super initWithFrame:frame];

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
	
	return self;
}

- (void)
drawHUD
{
	float size;
	
	size = left_stick_size * 2.f;
	//r_setup_orthogonal_view(buffer.width, buffer.height);
	glPushMatrix();
	glTranslatef(left_stick_position.x, left_stick_position.y, left_stick_position.z);
	glRotatef(45.f, 0.f, 0.f, 1.f);
	glRotatef(left_stick_rotation.z, 0.f, 0.f, 1.f);
	glScalef(size, size, size);
	//r_render_circle(60);
	r_render_quad(1);
	glPopMatrix();
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
	
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES,
									GL_RENDERBUFFER_WIDTH_OES, &buffer.width);
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES,
									GL_RENDERBUFFER_HEIGHT_OES, &buffer.height);
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES,
								 GL_RENDERBUFFER_OES, buffer.render);
	r_generate_depthbuffer(&buffer);
	r_bind_buffers(&buffer);
	
	UIAlertView *hello = [[UIAlertView alloc] initWithTitle:@"Yo maddafakkas!"
		message:@"Let's rock!" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
	[hello show];
	[hello release];

	game_initialize();
}

- (void)
touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	/*
	CGRect	bounds = [self bounds];
    CGPoint	location;
	UITouch	*touch = [[event touchesForView:self] anyObject];
	
	//NSArray *tmp = [touches allObjects];
	location = [self uiViewTouchToOpenGL:touch];
	// NOTE: W/H are now swapped
	location.x -= bounds.size.height/2.0f;
	location.y -= bounds.size.width/2.0f;
	
	vec3 point = {location.x, location.y, 0.f};

	[self tryStickTouch:point];*/
	
	[self touchesMoved:touches withEvent:event];
}

- (void)
touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	game_input_touch_end();
}

- (void)
touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	CGPoint location;
	vec3	point;
	UITouch *touch;
	
	touch = [[event touchesForView:self] anyObject];
	location = [self uiViewTouchToOpenGL:touch];
	location.x -= [self bounds].size.height/2.0f;
	location.y -= [self bounds].size.width/2.0f;
	
	point.x = location.x;
	point.y = location.y;
	point.z = 0.f;
	
	[self tryStickTouch:point];
}

- (void)
tryStickTouch:(vec3)point
{
	/*static const vec3 origin	= {-120, -60, 0};*/
	
	if (point.x < left_stick_position.x + left_stick_size
		&& point.x > left_stick_position.x - left_stick_size
		&& point.y < left_stick_position.y + left_stick_size
		&& point.y > left_stick_position.y - left_stick_size) {
		
		point = vec3_sub(&point, &left_stick_position);
		point = vec3_normalize(&point);
		left_stick_rotation.z = atan2(point.y, point.x);
		left_stick_rotation.z = rad2deg(left_stick_rotation.z);
	}
}

/* Convert touch point from UIView referential to OpenGL one (upside-down flip) */
- (CGPoint)
uiViewTouchToOpenGL:(UITouch *)touch
{
	CGPoint location = [touch locationInView:self];
	//location.y = bounds.size.height - location.y;
	CGFloat tmp = location.y;
	location.y = location.x;
	location.x = tmp;
	
	return location;
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
	[self drawHUD];
	[context presentRenderbuffer:GL_RENDERBUFFER_OES];
}

@end

