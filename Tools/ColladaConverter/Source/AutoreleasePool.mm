/*
 *  AutoreleasePool.cpp
 *  ColladaConverter
 *
 *  Created by Vitaliy  Borodovsky on 5/5/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "AutoreleasePool.h"
#include <Cocoa/Cocoa.h>

static NSAutoreleasePool * pool = nil;

void AutoreleasePool_Init()
{
	pool = [[NSAutoreleasePool alloc] init];
}

void AutoreleasePool_Release()
{
	[pool release];
}