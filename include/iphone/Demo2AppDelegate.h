//
//  Demo2AppDelegate.h
//  Demo2
//
//  Created by Andreas Kröhnke on 5/13/09.
//  Copyright BTH 2009. All rights reserved.
//

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

