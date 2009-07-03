/*-
 * License
 */

#import "Demo2AppDelegate.h"

@implementation Demo2AppDelegate

@synthesize window;
@synthesize game_view;

- (void)applicationDidFinishLaunching:(UIApplication *)application
{	
	//[application setStatusBarStyle:UIStatusBarStyleBlackTranslucent animated:NO];
	[application setStatusBarOrientation:UIInterfaceOrientationLandscapeRight animated:NO];
	[[UIApplication sharedApplication] setStatusBarHidden:YES animated:NO];
	
	// Create window
	window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    
	// Set up content view
	game_view = [[GameView alloc] initWithFrame:[[UIScreen mainScreen] applicationFrame]];
	[window addSubview:game_view];
    
	// Show window
	[window makeKeyAndVisible];
	
	// Timers
	update_timer = [NSTimer scheduledTimerWithTimeInterval:(1.0 / 60.0) target:game_view selector:@selector(update) userInfo:nil repeats:YES];
		
	[UIApplication sharedApplication].idleTimerDisabled = YES;
}


- (void)dealloc
{
    [game_view release];
	[window release];
	[super dealloc];
}


@end
