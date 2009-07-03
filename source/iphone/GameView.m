/*-
 * License
 */

#import <CoreGraphics/CoreGraphics.h>
#import <QuartzCore/QuartzCore.h>

#import "GameView.h"

#include "game.h"
#include "game_helper.h"
#include "rendering.h"
#include "vec3.h"

static int left_stick_held = 0;
static vec3 left_stick_position = {-170.f, -90.f, 0.f};
static vec3 left_stick_rotation = {0.f, 0.f, 0.f};
static float left_stick_size = 70.f;

@implementation GameView

+ (Class) layerClass
{
	return [CAEAGLLayer class];
}

- (void)
drawHUD
{
	float	angle,
	size;
	
	angle = atan2(left_stick_rotation.y, left_stick_rotation.x);
	angle = gh_rad2deg(angle);
	size = left_stick_size * 2.f;
	//r_setup_orthogonal_view(buffer.width, buffer.height);
	glPushMatrix();
	glTranslatef(left_stick_position.x, left_stick_position.y, left_stick_position.z);
	glRotatef(45.f, 0.f, 0.f, 1.f);
	glRotatef(angle, 0.f, 0.f, 1.f);
	//glScalef(size, size, size);
	r_render_circle(left_stick_size);
	//r_render_quad(1);
	glPopMatrix();
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
	vec3 tmp;
	
	tmp = vec3_sub(point, &left_stick_position);
	if (vec3_length(&tmp) < left_stick_size) {
		return YES;
	}
	/*
	if (point->x < left_stick_position.x + left_stick_size
		&& point->x > left_stick_position.x - left_stick_size
		&& point->y < left_stick_position.y + left_stick_size
		&& point->y > left_stick_position.y - left_stick_size) {
		return YES;
	}*/
	
	return NO;
}

- (void)
leftStickTouch:(const vec3 *)point
{
	vec3 p = *point;
	
	p = vec3_sub(&p, &left_stick_position);
	p = vec3_normalize(&p);
	left_stick_rotation = p;
}

/* Convert touch point from UIView referential to OpenGL one (upside-down flip) */
- (CGPoint)
pointToOpenGL:(CGPoint)location
{
	CGFloat tmp = location.y;
	
	location.y = location.x;
	location.x = tmp;
	location.x -= [self bounds].size.height/2.0f;
	location.y -= [self bounds].size.width/2.0f;
	
	return location;
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
	CGPoint location;
	NSArray *tmp;
	vec3 current;
	int i;
	
	tmp = [touches allObjects];
	for (i = 0; i < [tmp count]; ++i) {
		UITouch *touch = [tmp objectAtIndex:i];
		
		location = [self pointToOpenGL:[touch locationInView:self]];
		current.x = location.x;
		current.y = location.y;
		current.z = 0.f;
		
		if ([self leftStick:&current] == YES) {
			++left_stick_held;
			[self leftStickTouch:&current];
		}
	}
}

- (void)
touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	CGPoint location;
	NSArray *tmp;
	vec3 point = {0.f, 0.f, 0.f};
	int i;
	
	tmp = [touches allObjects];
	for (i = 0; i < [tmp count]; ++i) {
		UITouch *touch = [tmp objectAtIndex:i];
		
		location = [self pointToOpenGL:[touch locationInView:self]];
		point.x = location.x;
		point.y = location.y;
		
		if ([self leftStick:&point] == YES) {
			--left_stick_held;
		}
	}
}

- (void)
touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	CGPoint location;
	NSArray *tmp;
	vec3	current, previous;
	int		i;
	
	//touch = [[event touchesForView:self] anyObject];
	tmp = [touches allObjects];
	for (i = 0; i < [tmp count]; ++i) {
		UITouch *touch = [tmp objectAtIndex:i];
		
		location = [self pointToOpenGL:[touch locationInView:self]];
		current.x = location.x;
		current.y = location.y;
		current.z = 0.f;
		location = [self pointToOpenGL:[touch previousLocationInView:self]];
		previous.x = location.x;
		previous.y = location.y;
		previous.z = 0.f;
		
		if ([self leftStick:&current] == YES) {
			if ([self leftStick:&previous] == NO) {
				++left_stick_held;
			}
			[self leftStickTouch:&current];
		} else if ([self leftStick:&previous] == YES){
			--left_stick_held;
		}
	}
}

/*
 * Main loop
 */
- (void)
update
{
	if (left_stick_held > 0) {
		struct gh_input gi;
		
		gi.type = GHI_MOVE;
		gi.data = malloc(sizeof(struct gh_input));
		memcpy(gi.data, &left_stick_rotation, sizeof(vec3));
		game_input(gi);
	} else {
		struct gh_input gi;
		
		gi.type = GHI_MOVE_STOP;
		gi.data = 0;
		game_input(gi);
	}
	
	game_update();

	[EAGLContext setCurrentContext:context];
	game_render(&buffer);
	[self drawHUD];
	[context presentRenderbuffer:GL_RENDERBUFFER_OES];
}

@end

