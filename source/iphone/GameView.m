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

static vec3 left_stick_position = {-160.f, -80.f, 0.f};
static vec3 left_stick_rotation = {0.f, 0.f, 0.f};
static float left_stick_size = 60.f;
static UITouch *left_touch = 0;

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
	float	angle,
			size;
	
	angle = atan2(left_stick_rotation.y, left_stick_rotation.x);
	angle = rad2deg(angle);
	size = left_stick_size * 2.f;
	//r_setup_orthogonal_view(buffer.width, buffer.height);
	glPushMatrix();
	glTranslatef(left_stick_position.x, left_stick_position.y, left_stick_position.z);
	glRotatef(45.f, 0.f, 0.f, 1.f);
	glRotatef(angle, 0.f, 0.f, 1.f);
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
	
	/*
	UIAlertView *hello = [[UIAlertView alloc] initWithTitle:@"Yo maddafakkas!"
		message:@"Let's rock!" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
	[hello show];
	[hello release];
	*/

	game_initialize();
}

- (BOOL)
leftStick:(const vec3 *)point
{
	if (point->x < left_stick_position.x + left_stick_size
		&& point->x > left_stick_position.x - left_stick_size
		&& point->y < left_stick_position.y + left_stick_size
		&& point->y > left_stick_position.y - left_stick_size) {
		return YES;
	}
	
	return NO;
}

- (void)
leftStickTouch:(const vec3 *)point
{
	struct gh_input gi;
	vec3 p = *point;
	
	p = vec3_sub(&p, &left_stick_position);
	p = vec3_normalize(&p);
	left_stick_rotation = p;
		
	gi.type = GHI_MOVE;
	gi.data = malloc(sizeof(struct gh_input));
	memcpy(gi.data, &p, sizeof(vec3));
	game_input(gi);
}

- (void)
rightStickTouch:(const vec3 *)point
{
	struct gh_input gi;
	vec3 p = *point;
	
	p = vec3_sub(&p, &left_stick_position);
	p = vec3_normalize(&p);
	left_stick_rotation = p;
	
	gi.type = GHI_MOVE;
	gi.data = malloc(sizeof(struct gh_input));
	memcpy(gi.data, &p, sizeof(vec3));
	game_input(gi);
}

- (void)
touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	[self touchesMoved:touches withEvent:event];
}

- (void)
touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	//CGPoint location;
	NSArray *tmp;
	struct	gh_input gi;
	int i;
	//vec3	point = {0.f, 0.f, 0.f};
	
	tmp = [touches allObjects];
	for (i = 0; i < [tmp count]; ++i) {
		UITouch *touch = [tmp objectAtIndex:i];
		
		if (left_touch == touch) {
		
			left_touch = 0;
			gi.type = GHI_MOVE_STOP;
			gi.data = 0;
			game_input(gi);
		}
	}
}

- (void)
touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	CGPoint location;
	NSArray *tmp;
	vec3	point;
	int i;
	
	//touch = [[event touchesForView:self] anyObject];
	tmp = [touches allObjects];
	for (i = 0; i < [tmp count]; ++i) {
		UITouch *touch = [tmp objectAtIndex:i];
		
		location = [self uiViewTouchToOpenGL:touch];
		point.x = location.x;
		point.y = location.y;
		point.z = 0.f;
	
		if ([self leftStick:&point] == YES) {
			if (0 == left_touch) {
				left_touch = touch;
			}
			
			if (touch == left_touch) {
				[self leftStickTouch:&point];
			}
		}
	}
}

/* Convert touch point from UIView referential to OpenGL one (upside-down flip) */
- (CGPoint)
uiViewTouchToOpenGL:(UITouch *)touch
{
	CGPoint location = [touch locationInView:self];
	CGFloat tmp = location.y;
	
	location.y = location.x;
	location.x = tmp;
	location.x -= [self bounds].size.height/2.0f;
	location.y -= [self bounds].size.width/2.0f;
	
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

