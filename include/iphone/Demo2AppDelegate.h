/*-
 * License
 */

#import <UIKit/UIKit.h>

#import "GameView.h"

@interface Demo2AppDelegate : NSObject <UIApplicationDelegate>
{
	UIWindow *window;
    GameView *game_view;
	NSTimer *update_timer;
}

@property (nonatomic, retain) UIWindow *window;
@property (nonatomic, retain) GameView *game_view;

@end

