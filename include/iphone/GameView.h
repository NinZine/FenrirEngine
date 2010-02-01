/*-
 * License
 */

#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <UIKit/UIView.h>

#include "game_helper.h"
#include "rendering.h"

@interface GameView : UIView
{
	EAGLContext 		*context;
	r_state 	buffer;
}

- (void)	drawHUD;
- (BOOL)	buttonTouched:(const gh_button *)b point:(const vec3 *)point;
- (void)	buttonTouch:(gh_button *)b point:(const vec3*)point;
- (CGPoint) pointToOpenGL:(CGPoint)point;

@end
