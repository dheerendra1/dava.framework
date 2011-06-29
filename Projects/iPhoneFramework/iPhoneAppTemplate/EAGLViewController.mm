//
//  EAGLViewController.m
//  MahJongQuest
//
//  Created by Vitaliy  Borodovsky on 10/8/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "EAGLViewController.h"


@implementation EAGLViewController

/*
 // The designated initializer.  Override if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        // Custom initialization
    }
    return self;
}
*/

/*
// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
}
*/

/*
// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad];
}
*/

// Override to allow orientations other than the default portrait orientation.
/*- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation 
{
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationLandscapeLeft);
}*/


- (void)didReceiveMemoryWarning 
{
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload 
{
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}

- (void)dealloc 
{
    [super dealloc];
}

- (void) viewWillAppear: (BOOL)animating
{
	NSLog(@"EAGLViewController viewWillAppear (startAnimation)");
	[glView setCurrentContext];
	[glView startAnimation];
}

- (void) viewDidDisappear: (BOOL)animating
{
	NSLog(@"EAGLViewController viewDidDisappear (stopAnimation)");
	[glView stopAnimation];
}


@end
