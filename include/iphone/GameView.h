/*-
 * License
 */

#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <UIKit/UIView.h>

#include "rendering.h"

@interface GameView : UIView
{
	EAGLContext 		*context;
	struct r_gl_buffer 	buffer;
}

- (void)	drawHUD;
- (BOOL)	leftStick:(const vec3 *)point;
- (void)	leftStickTouch:(const vec3*)point;
- (void)	rightStickTouch:(const vec3*)point;
- (CGPoint) pointToOpenGL:(CGPoint)point;

@end
