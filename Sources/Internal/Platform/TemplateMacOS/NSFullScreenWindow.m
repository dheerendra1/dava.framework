//
//  NSFullScreenWindow.m
//  Framework
//
//  Created by Dmitry Shpakov on 11/16/11.
//  Copyright 2011 DAVA Consulting. All rights reserved.
//

#import "NSFullScreenWindow.h"


@implementation NSFullScreenWindow

- (id)init
{
    self = [super init];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

- (void)dealloc
{
    [super dealloc];
}

- (BOOL)canBecomeKeyWindow
{
    return YES;
}

@end
