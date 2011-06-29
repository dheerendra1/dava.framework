//
//  CharacterTablePanel.h
//  CoreTextArcCocoa
//
//  Created by Vitaliy  Borodovsky on 12/15/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface CharacterTablePanel : NSObject<NSWindowDelegate>
{
@public
	NSString * symbolTable;
	
	BOOL isNumbers;
	BOOL isLatin;
	BOOL isLatinSupplement;
	BOOL isCyrilic;
	
	
	IBOutlet NSTextField * additionalCharacters;
	IBOutlet NSTextField * outputCharacters;
}

- (void)updateSymbolTable;
- (void)windowDidBecomeKey:(NSNotification *)notification;
- (void)windowDidResignKey:(NSNotification *)notification;

@property(nonatomic) BOOL isNumbers;
@property(nonatomic) BOOL isLatin;
@property(nonatomic) BOOL isLatinSupplement;
@property(nonatomic) BOOL isCyrilic;

- (IBAction) toggleNumbers: (id)sender;
- (IBAction) toggleLatin: (id)sender;
- (IBAction) toggleLatinSupplement: (id)sender;
- (IBAction) toggleCyrilic: (id)sender;

- (void)updateSymbolTable;

@end
