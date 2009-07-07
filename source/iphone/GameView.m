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

static gh_button button[] = {
	{{-170.f, -90.f, -1.f}, {0.f, 0.f, 0.f}, 70.f, 0}, /* Left stick */
	{{200.f, -90.f, -1.f}, {0.f, 0.f, 0.f}, 40.f, 0}, /* Right button */
};
static int num_buttons = sizeof(button)/sizeof(gh_button);

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
	int j;
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	r_setup_orthogonal_view(buffer.width, buffer.height);
	glRotatef(90.0f, 0.0f, 0.0f, -1.0f); // For landscape mode
	glColor4f(0.f, 0.f, 0.f, .5f);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glMatrixMode(GL_MODELVIEW);
	for (j = 0; j < num_buttons; ++j) {
		angle = atan2(button[j].rotation.y, button[j].rotation.x);
		angle = gh_rad2deg(angle);
		size = button[j].size * 2.f;
		//r_setup_orthogonal_view(buffer.width, buffer.height);
		glPushMatrix();
		glTranslatef(button[j].position.x, button[j].position.y, button[j].position.z);
		glRotatef(45.f, 0.f, 0.f, 1.f);
		glRotatef(angle, 0.f, 0.f, 1.f);
		//glScalef(size, size, size);
		r_render_circle(button[j].size);
		//r_render_quad(1);
		glPopMatrix();
	}
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
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
buttonTouched:(const gh_button *)b point:(const vec3 *)point
{
	vec3 tmp;
	
	tmp = vec3_sub(point, &b->position);
	if (vec3_length(&tmp) < b->size) {
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
buttonTouch:(gh_button *)b point:(const vec3 *)point
{
	vec3 p = *point;
	
	p = vec3_sub(&p, &b->position);
	p = vec3_normalize(&p);
	b->rotation = p;
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
touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	CGPoint location;
	NSArray *tmp;
	vec3 current;
	int i, j;
	
	tmp = [touches allObjects];
	for (i = 0; i < [tmp count]; ++i) {
		UITouch *touch = [tmp objectAtIndex:i];
		
		location = [self pointToOpenGL:[touch locationInView:self]];
		current.x = location.x;
		current.y = location.y;
		current.z = 0.f;
		
		for (j = 0; j < num_buttons; ++j) {
			if ([self buttonTouched:&button[j] point:&current] == YES) {
				++(button[j]).held;
				[self buttonTouch:&button[j] point:&current];
			}
		}
	}
}

- (void)
touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	CGPoint location;
	NSArray *tmp;
	vec3 point = {0.f, 0.f, 0.f};
	int i, j;
	
	tmp = [touches allObjects];
	for (i = 0; i < [tmp count]; ++i) {
		UITouch *touch = [tmp objectAtIndex:i];
		
		location = [self pointToOpenGL:[touch locationInView:self]];
		point.x = location.x;
		point.y = location.y;
		
		for (j = 0; j < num_buttons; ++j) {
			if ([self buttonTouched:&button[j] point:&point] == YES) {
				--(button[j]).held;
			}
		}
	}
}

- (void)
touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	CGPoint location;
	NSArray *tmp;
	vec3	current, previous;
	int		i, j;
	
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
		
		for (j = 0; j < num_buttons; ++j) {
			if ([self buttonTouched:&button[j] point:&current] == YES) {
				if ([self buttonTouched:&button[j] point:&previous] == NO) {
					++(button[j]).held;
				}
				[self buttonTouch:&button[j] point:&current];
			} else if ([self buttonTouched:&button[j] point:&previous] == YES){
				--(button[j]).held;
			}
		}
	}
}

/*
 * Main loop
 */
- (void)
update
{
	int i;
	
	for (i = 0; i < num_buttons; ++i) {
		gh_input(&button[i], i);
	}
	
	game_update();

	[EAGLContext setCurrentContext:context];
	game_render(&buffer);
	[self drawHUD];
	[context presentRenderbuffer:GL_RENDERBUFFER_OES];
}

@end

