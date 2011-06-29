//
//  CharacterTablePanel.m
//  CoreTextArcCocoa
//
//  Created by Vitaliy  Borodovsky on 12/15/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "CharacterTablePanel.h"
#import "MyDocument.h"

@implementation CharacterTablePanel

@synthesize isNumbers;
@synthesize isLatin;
@synthesize isLatinSupplement;
@synthesize isCyrilic;

NSInteger alphabeticSort(id string1, id string2, void *reverse)
{
    if (*(BOOL *)reverse == YES) {
        return [string2 localizedCaseInsensitiveCompare:string1];
    }
    return [string1 localizedCaseInsensitiveCompare:string2];
}

- (id)init
{
    self = [super init];
    if (self) 
	{
		isNumbers = YES;
		isLatin = YES;
		isLatinSupplement = YES;
		isCyrilic = YES;
		symbolTable = 0;
    }
    return self;
}

- (void)windowDidBecomeKey:(NSNotification *)notification
{
	[self updateSymbolTable];
}

- (void)windowDidResignKey:(NSNotification *)notification
{
	MyDocument * document = [[NSDocumentController sharedDocumentController] currentDocument];
	[document.arcView setSymbolTable:symbolTable];
}

- (void)updateSymbolTable
{
	int k;
    int i;
	NSMutableSet *set = [NSMutableSet setWithCapacity:500];
	
	if (isNumbers)
	{
		for (k = '0'; k <= '9'; ++k)
		{
			[set addObject:[NSNumber numberWithInt:k]];
		}
	}
	
	if (isLatin)
	{
		for (k = 0x20; k < 0x7F; ++k)
		{
			[set addObject:[NSNumber numberWithInt:k]];
		}
	}
	//80–FF
	if (isLatinSupplement)
	{
		for (k = 0xA1; k <= 0xFF; ++k)
		{
			if (k != 0xAD)
				[set addObject:[NSNumber numberWithInt:k]];
		}
	}
	
	//80–FF
	if (isCyrilic)
	{
		for (k = 0x0410; k <= 0x045F; ++k)
		{
			[set addObject:[NSNumber numberWithInt:k]];
		}
	}
    
    NSString * additionString = [additionalCharacters stringValue];
    for(i = 0; i < [additionString length]; ++i)
    {
        [set addObject:[NSNumber numberWithShort:[additionString characterAtIndex:i]]];
    }
	
	NSMutableString * string = [NSMutableString string];
	NSNumber * value;
//	NSMutableArray * arrayBeforeSo = [NSMutableArray arrayWithCapacity:500];
//	for (value in [set allObjects])
//	{
//		//unichar c = [value intValue]; 
//		[arrayOfStrings addObject:value];//[NSString stringWithCharacters:&c length:1]];
//	}
	NSArray * resultArray = [[set allObjects] sortedArrayUsingSelector:@selector(compare:)];
	
	for (value in resultArray)
	{
		unichar c = [value intValue]; 
		NSString * stringValue = [NSString stringWithCharacters:&c length:1];
		[string appendString: stringValue];
	}
    
	[outputCharacters setStringValue: string];

	[symbolTable release];
	symbolTable = [string retain];
}

- (IBAction) toggleNumbers: (id)sender
{
	isNumbers = ([sender state] == NSOnState);
	[self updateSymbolTable];
}

- (IBAction) toggleLatin: (id)sender
{
	isLatin = ([sender state] == NSOnState);
	[self updateSymbolTable];
}

- (IBAction) toggleLatinSupplement: (id)sender
{
	isLatinSupplement = ([sender state] == NSOnState);
	[self updateSymbolTable];
}

- (IBAction) toggleCyrilic: (id)sender
{
	isCyrilic = ([sender state] == NSOnState);
	[self updateSymbolTable];
}



@end
