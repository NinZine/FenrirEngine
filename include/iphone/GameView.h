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
	r_state 	buffer;
}

- (CGPoint) pointToOpenGL:(CGPoint)point;

@end
