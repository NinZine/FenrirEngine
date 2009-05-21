//
//  Demo2AppDelegate.m
//  Demo2
//
//  Created by Andreas Kröhnke on 5/13/09.
//  Copyright BTH 2009. All rights reserved.
//

#import "Demo2AppDelegate.h"

@implementation Demo2AppDelegate

@synthesize window;
@synthesize game_view;

- (void)applicationDidFinishLaunching:(UIApplication *)application
{	
	// Create window
	window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    
	// Set up content view
	game_view = [[GameView alloc] initWithFrame:[[UIScreen mainScreen] applicationFrame]];
	[window addSubview:game_view];
    
	// Show window
	[window makeKeyAndVisible];
	[application setStatusBarOrientation:UIInterfaceOrientationPortrait animated:YES];
	[application setStatusBarHidden:YES animated:YES];
	
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
