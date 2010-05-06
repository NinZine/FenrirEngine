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
#include "sound.h"

#import "GameView.h"

extern int luaopen_blender(lua_State* L);
extern int luaopen_event(lua_State* L);
extern int luaopen_image(lua_State* L);
extern int luaopen_mat4(lua_State* L);
extern int luaopen_net(lua_State* L);
extern int luaopen_quat(lua_State* L);
extern int luaopen_render(lua_State* L);
extern int luaopen_sound(lua_State* L);
extern int luaopen_vec3(lua_State* L);

static int do_main(int argc,char* argv[]);
static void l_message (const char *pname, const char *msg);
static int report (lua_State *L, int status);
static void quit();

static char *argv[] = {"conceptengine.app", "ludumdare/main.lua"};
static lua_State *L = 0;

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
	}
	
	return self;
}

- (void)
layoutSubviews
{
	
	assert( [EAGLContext setCurrentContext:context] != NO );

	buffer.renderbuffer = r_generate_renderbuffer();
	buffer.framebuffer = r_generate_framebuffer();
	r_bind_buffers(&buffer);
	assert( [context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(id<EAGLDrawable>)self.layer]
		!= NO );
	
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES,
									GL_RENDERBUFFER_WIDTH_OES, &buffer.width);
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES,
									GL_RENDERBUFFER_HEIGHT_OES, &buffer.height);
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES,
								 GL_RENDERBUFFER_OES, buffer.renderbuffer);
	buffer.depth = r_generate_depthbuffer(buffer.width, buffer.height);
	r_bind_buffers(&buffer);
	/* Make sure there is no error in OpenGL stuff */
	assert(glGetError() == GL_NO_ERROR);

	do_main(2, argv);
	[self update];
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
		
		/*for (j = 0; j < num_buttons; ++j) {
			if ([self buttonTouched:&button[j] point:&current] == YES) {
				++(button[j]).held;
				[self buttonTouch:&button[j] point:&current];
			}
		}*/
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
		
		/*for (j = 0; j < num_buttons; ++j) {
			if ([self buttonTouched:&button[j] point:&point] == YES) {
				--(button[j]).held;
			}
		}*/
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
		
		/*for (j = 0; j < num_buttons; ++j) {
			if ([self buttonTouched:&button[j] point:&current] == YES) {
				if ([self buttonTouched:&button[j] point:&previous] == NO) {
					++(button[j]).held;
				}
				[self buttonTouch:&button[j] point:&current];
			} else if ([self buttonTouched:&button[j] point:&previous] == YES){
				--(button[j]).held;
			}
		}*/
	}
}

/*
 * Main loop
 */
- (void)
update
{
	int i;
	
	/*for (i = 0; i < num_buttons; ++i) {
		gh_input(&button[i], i);
	}
	
	game_update();*/

	[EAGLContext setCurrentContext:context];
	
	//game_render(&buffer);
	r_bind_buffers(&buffer);
	if (0 != lua_update(L)) {
		quit();
		printf("lua> quit\n");
	}
	
	[context presentRenderbuffer:GL_RENDERBUFFER_OES];
}

@end

void
l_message (const char *pname, const char *msg)
{
	if (pname) printf("%s: ", pname);
	
	printf("%s\n", msg);
	//fflush(stderr);
}

int
report (lua_State *L, int status)
{
	if (status && !lua_isnil(L, -1)) {
		const char *msg = lua_tostring(L, -1);
		if (msg == NULL) msg = "(error object is not a string)";
		l_message(0, msg);
		lua_pop(L, 1);
	}
	return status;
}

int
do_main(int argc,char* argv[])
{
	int status;
	int *a = 0;
	const char* f;
	
	if (argc<2) {
		printf("%s: <filename.lua>\n",argv[0]);
		return 0;
	}
	
    s_init();
    printf("sound> initialized\n");
	
	L=lua_open();
	/*luaopen_base(L);*/
	luaL_openlibs(L);
	luaopen_mat4(L);
	luaopen_quat(L);
	luaopen_vec3(L);
	luaopen_event(L);
	luaopen_image(L);
	luaopen_render(L);
	luaopen_sound(L);
	luaopen_net(L);
	luaopen_blender(L);
	
	lua_getglobal(L, "package");
	if (LUA_TTABLE != lua_type(L, 1)) {
		printf("lua> 'package' is not a table\n");
		return 1;
	}
	lua_getfield(L, 1, "path");
	if (LUA_TSTRING != lua_type(L, 2)) {
		printf("lua> 'package.path' is not a string\n");
		lua_pop(L, 1);
		return 1;
	}
	lua_pop(L, 1);
	
	f = full_path();
	lua_pushlstring(L, f, strlen(f));
	lua_pushliteral(L, "/?.lua");
	lua_concat(L, 2);
	lua_setfield(L, 1, "path");
	
	f = full_path_to_file(argv[1]);
	
	printf("lua> initialized\n");
	printf("lua> loading %s\n", f);
	
	if (luaL_loadfile(L,f)==0) { // load and run the file
		lua_setglobal(L, "runme");
		lua_getglobal(L, "runme");
		status = lua_pcall(L,0,LUA_MULTRET,0);
		report(L, status);
		printf("lua> loaded\n");
	} else {
		printf("lua> unable to load %s\n",f);
		quit();
	}
	
	return status;
}

int
lua_update(lua_State *L)
{
	int status, top;
	
	lua_getglobal(L, "update");
	status = lua_pcall(L,0,LUA_MULTRET,0);
	report(L, status);
	
	return status;
}

void
quit()
{
	s_quit();
	lua_close(L);
}
