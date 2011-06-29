/*
 
 File: CoreTextArcView.m
 
 Abstract: Defines and implements the CoreTextArcView custom NSView subclass to
 draw text on a curve and illustrate best practices with CoreText.
 
 Version: 1.0
 
 Disclaimer: IMPORTANT:  This Apple software is supplied to you by 
 Apple Inc. ("Apple") in consideration of your agreement to the
 following terms, and your use, installation, modification or
 redistribution of this Apple software constitutes acceptance of these
 terms.  If you do not agree with these terms, please do not use,
 install, modify or redistribute this Apple software.
 
 In consideration of your agreement to abide by the following terms, and
 subject to these terms, Apple grants you a personal, non-exclusive
 license, under Apple's copyrights in this original Apple software (the
 "Apple Software"), to use, reproduce, modify and redistribute the Apple
 Software, with or without modifications, in source and/or binary forms;
 provided that if you redistribute the Apple Software in its entirety and
 without modifications, you must retain this notice and the following
 text and disclaimers in all such redistributions of the Apple Software. 
 Neither the name, trademarks, service marks or logos of Apple Inc. 
 may be used to endorse or promote products derived from the Apple
 Software without specific prior written permission from Apple.  Except
 as expressly stated in this notice, no other rights or licenses, express
 or implied, are granted by Apple herein, including but not limited to
 any patent rights that may be infringed by your derivative works or by
 other works in which the Apple Software may be incorporated.
 
 The Apple Software is provided by Apple on an "AS IS" basis.  APPLE
 MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION
 THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS
 FOR A PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND
 OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
 
 IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION,
 MODIFICATION AND/OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED
 AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE),
 STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 
 Copyright (C) 2008 Apple Inc. All Rights Reserved.
 
 */ 

#import "CoreTextArcView.h"
#import <AssertMacros.h>

#define ARCVIEW_DEFAULT_FONT_NAME	@"Myriad Pro"
#define ARCVIEW_DEFAULT_FONT_SIZE	24.0
#define ARCVIEW_DEFAULT_RADIUS		150.0

@implementation NSNumber (numericComparison)

- (NSComparisonResult) compare:(NSNumber *) other
{
    float myValue = [self floatValue];
    float otherValue = [other floatValue];
    if (myValue == otherValue) return NSOrderedSame;
    return (myValue < otherValue ? NSOrderedAscending : NSOrderedDescending);
}

@end

@implementation CoreTextArcView

@synthesize enableShadow;

- (id)initWithFrame:(NSRect)frame 
{
    self = [super initWithFrame:frame];
    if (self) {
		self.font = [NSFont fontWithName:ARCVIEW_DEFAULT_FONT_NAME size:ARCVIEW_DEFAULT_FONT_SIZE];
		self.string = @"Test String!@#$%^&*()~";
		self.radius = ARCVIEW_DEFAULT_RADIUS;
		self.showsGlyphBounds = NO;
		self.showsLineMetrics = NO;
		self.dimsSubstitutedGlyphs = NO;
		isPrecisionKerning = true;
		symbolTableContext = 0;
		symbolTableImage = 0;
		leftRightPadding = 5.0f;
		topBottomPadding = 5.0f;
		enableShadow = YES;
		
		//symbolTable = @" !\"#$%&\"()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_'abcdefghijklmnopqrstuvwxyz{|}©™";
		//[self extractKerningValues];
    }
    return self;
}

- (void) awakeFromNib
{
	[characterTablePanel updateSymbolTable];
	[self setSymbolTable:characterTablePanel->symbolTable];
}

typedef struct GlyphArcInfo {
	CGFloat			width;
	CGFloat			angle;	// in radians
} GlyphArcInfo;

static void PrepareGlyphArcInfo(CTLineRef line, CFIndex glyphCount, GlyphArcInfo *glyphArcInfo)
{
	NSArray *runArray = (NSArray *)CTLineGetGlyphRuns(line);
	
	// Examine each run in the line, updating glyphOffset to track how far along the run is in terms of glyphCount.
	CFIndex glyphOffset = 0;
	for (id run in runArray) {
		CFIndex runGlyphCount = CTRunGetGlyphCount((CTRunRef)run);
		
		// Ask for the width of each glyph in turn.
		CFIndex runGlyphIndex = 0;
		for (; runGlyphIndex < runGlyphCount; runGlyphIndex++) {
			glyphArcInfo[runGlyphIndex + glyphOffset].width = CTRunGetTypographicBounds((CTRunRef)run, CFRangeMake(runGlyphIndex, 1), NULL, NULL, NULL);
		}
		
		glyphOffset += runGlyphCount;
	}
	
	double lineLength = CTLineGetTypographicBounds(line, NULL, NULL, NULL);
	
	CGFloat prevHalfWidth = glyphArcInfo[0].width / 2.0;
	glyphArcInfo[0].angle = (prevHalfWidth / lineLength) * M_PI;
	
	// Divide the arc into slices such that each one covers the distance from one glyph's center to the next.
	CFIndex lineGlyphIndex = 1;
	for (; lineGlyphIndex < glyphCount; lineGlyphIndex++) {
		CGFloat halfWidth = glyphArcInfo[lineGlyphIndex].width / 2.0;
		CGFloat prevCenterToCenter = prevHalfWidth + halfWidth;
		
		glyphArcInfo[lineGlyphIndex].angle = (prevCenterToCenter / lineLength) * M_PI;
		
		prevHalfWidth = halfWidth;
	}
}

-(CGContextRef) MyCreateBitmapContext: (int)pixelsWide :(int)pixelsHigh
{
    CGContextRef    context = NULL;
    CGColorSpaceRef colorSpace;
    int             bitmapByteCount;
    int             bitmapBytesPerRow;
	
    bitmapBytesPerRow   = (pixelsWide * 4);// 1
    bitmapByteCount     = (bitmapBytesPerRow * pixelsHigh);
	
    colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);// 2
    context = CGBitmapContextCreate(0,// 4
									 pixelsWide,
									 pixelsHigh,
									 8,      // bits per component
									 bitmapBytesPerRow,
									 colorSpace,
									 kCGImageAlphaPremultipliedLast);
	
	
	CGContextSetInterpolationQuality(context, kCGInterpolationHigh);
	CGContextSetShouldAntialias(context, true);
	CGContextSetAllowsAntialiasing(context, true);
	CGContextSetAllowsFontSmoothing(context, true);
	CGContextSetShouldSmoothFonts(context, true);
	CGContextSetAllowsFontSubpixelPositioning(context, true);
	CGContextSetShouldSubpixelPositionFonts(context, true);
	CGContextSetAllowsFontSubpixelQuantization(context, true);
	CGContextSetShouldSubpixelQuantizeFonts(context, true);
	CGContextSetTextDrawingMode(context, kCGTextFill);
	
	if (enableShadow)
	{
		float components[4] = {1.0f, 1.0f, 1.0f, 1.0};
		//CGColorSpaceRef colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
		CGColorRef shadowColor = CGColorCreate( colorSpace, components);
		CGContextSetShadowWithColor(context, CGSizeMake(0, 0), 1.0f, shadowColor);
	}
	
	//CGColorRelease(shadowColor);
	CGColorSpaceRelease(colorSpace);
	
	//CGContextSetShadowWithColor(context, 1.0f, 
	//NSColor * c = [NSColor redColor]; 
	//CGContextSetShadowWithColor(context, CGSizeMake(0, 0), 0.0f, NULL);
	//CGContextSetShadow(context, CGSizeMake(1, 1), 1.0f);
	
	//float flatness = CGContextGetFlatness(context);
	//CGContextSetFlatness(context, 0.01f);
	
	CGContextSetTextMatrix(context, CGAffineTransformIdentity);
	
	CGContextSetRGBFillColor(context, 0.0f, 0.0f, 0.0f, 0.0f);
	CGContextFillRect(context, CGRectMake(0, 0, pixelsWide,
										  pixelsHigh));

	//CGLayerRef layer = CGLayerCreateWithContext(context, CGSizeMake(pixelsWide, pixelsHigh), NULL);
	//context = CGLayerGetContext(layer);

    if (context== NULL)
    {
        return NULL;
    }
    CGColorSpaceRelease( colorSpace );// 6
    return context;// 7
}

void ExportImage(CGImageRef image, NSString * filepath)
{
	//CFURLRef url = CFURLCreateWithString( NULL, CFSTR("/Sources/dava.framework/ExportedFont.png"), NULL);
	//[[NSBundle mainBundle] documentsPath]
		
	NSURL * url = [NSURL fileURLWithPath: filepath];
	
	CFStringRef type = kUTTypePNG;  //public.png
	size_t count = 1; 
	CFDictionaryRef options = NULL;
	CGImageDestinationRef dest = CGImageDestinationCreateWithURL((CFURLRef)url, type, count, options);
	
	CGImageDestinationAddImage(dest, image, NULL);
	
	CGImageDestinationFinalize(dest);
	CFRelease(dest);
}

NSInteger floatSort(id num1, id num2, void *context)
{
    float v1 = [num1 floatValue];
    float v2 = [num2 floatValue];
    if (v1 < v2)
        return NSOrderedAscending;
    else if (v1 > v2)
        return NSOrderedDescending;
    else
        return NSOrderedSame;
}



/*
 NSArray *allKeys = [aDictionary allKeys];
 NSArray *sortedKeys = [allKeys sortedArrayUsingFunction:floatSort context:NULL];
 for (id key in sortedKeys)
 id val = [aDictionary objectForKey:key];
 
 */
- (void) findMaxInLine: (int)s1 
{
	NSMutableDictionary * counts = [NSMutableDictionary dictionaryWithCapacity: 2500];
	int s2;
	for (s2 = 0; s2 < [symbolTable length]; ++s2)
	{
		NSNumber * key = [NSNumber numberWithFloat:kerningTable[s1][s2]];
		NSNumber * count = [counts objectForKey: key];
		if (count == nil)
		{
			[counts setObject:[NSNumber numberWithInteger:1]  forKey:key];
		}else 
		{
			[counts setObject:[NSNumber numberWithInteger:([count intValue] + 1)]  forKey:key];
		}	
	}
	float maxValue = 0.0f;
	int maxCount = 0;
	NSNumber *key;
	for(key in counts)
	{
		//NSLog(@"Key: %@, Value %@", key, [counts objectForKey: key]);
		if ([[counts objectForKey: key] intValue] > maxCount)
		{
			maxCount = [[counts objectForKey: key] intValue];
			maxValue = [key floatValue];
		}
	}
	kerningBaseShift[s1] = maxValue;
	for (s2 = 0; s2 < [symbolTable length]; ++s2)
	{
		kerningTable[s1][s2] -= kerningBaseShift[s1];
	}
}
- (void) setSymbolTable: (NSString*)newSymbolTable
{
	symbolTable = newSymbolTable;
	[self extractKerningValues];
	[self needsDisplay];
}


- (void) extractKerningValues
{
	// get font 
	CGContextRef tempContext = [self MyCreateBitmapContext:1024 :64];
	
	// Create our attributes
	NSDictionary *attributes = [NSDictionary dictionaryWithObjectsAndKeys:self.font, NSFontAttributeName, [NSNumber numberWithInteger:0], NSLigatureAttributeName, nil];
	assert(attributes != nil);
	fontWidth = 0.0f;
	fontHeight = 0.0f;
	maxSymbolWidth = 0.0f;
	memset(distanceTable, 0, sizeof(distanceTable));
	int s1, s2;
	fontAscent = CTFontGetAscent((CTFontRef)self.font);
	fontDescent = CTFontGetDescent((CTFontRef)self.font);
	fontLeading = CTFontGetLeading((CTFontRef)self.font);
	fontHeight = fontAscent + fontDescent + fontLeading;
	fontXHeight = CTFontGetXHeight((CTFontRef)self.font);
	
	for (s1 = 0; s1 < [symbolTable length]; ++s1)
		for (s2 = 0; s2 < [symbolTable length]; ++s2)
			{
				// Create the attributed string
				unichar tsarr[] = {[symbolTable characterAtIndex:s1], [symbolTable characterAtIndex:s2], 0};
				NSString * ts = [NSString stringWithCharacters:tsarr length: 2];
				NSAttributedString *attrString = [[[NSAttributedString alloc] initWithString:ts attributes:attributes] autorelease];
				
				CTLineRef line = CTLineCreateWithAttributedString((CFAttributedStringRef)attrString);
				assert(line != NULL);
				
				
				CFArrayRef runArray = CTLineGetGlyphRuns(line);
				CFIndex runCount = CFArrayGetCount(runArray);
								
				
				//assert(runCount == 1);
				//assert(runGlyphCount == 2);
				
				if (runCount == 1)
				{
					CTRunRef run = (CTRunRef)CFArrayGetValueAtIndex(runArray, 0);
					CFIndex	runGlyphCount = CTRunGetGlyphCount(run);
					//CTFontRef runFont = CFDictionaryGetValue(CTRunGetAttributes(run), kCTFontAttributeName);
					assert(runGlyphCount == 2);
					CFIndex runGlyphIndex = 0;
					for (; runGlyphIndex < runGlyphCount; runGlyphIndex++) 
					{
						CFRange glyphRange = CFRangeMake(runGlyphIndex, 1);
						//CGContextRotateCTM(context, -(glyphArcInfo[runGlyphIndex + glyphOffset].angle));
						
						// Center this glyph by moving left by half its width.
						CGFloat glyphWidth = //glyphArcInfo[runGlyphIndex + glyphOffset].width;
						CTRunGetTypographicBounds((CTRunRef)run, glyphRange, NULL, NULL, NULL);
						
						if (runGlyphIndex == 0)
						{
							CGRect lineMetrics;
							CGFloat ascent, descent;
							CTRunGetTypographicBounds(run, glyphRange, &ascent, &descent, NULL);
							lineMetrics.origin.x = 0;
							lineMetrics.origin.y = - descent;
							lineMetrics.size.width = glyphWidth; 
							lineMetrics.size.height = ascent + descent;
							fontWidth += glyphWidth + 3;
														
							//NSLog(@"%c asc:%f desc: %f", [symbolTable characterAtIndex:s1], ascent, descent);
							
							// wrong glyph size calculation
							//maxSymbolWidth = MAX(glyphWidth, maxSymbolWidth);
							
							symbolTypoBounds[s1] = lineMetrics;
							
							CGRect glyphBounds = CTRunGetImageBounds(run, tempContext, glyphRange);
							symbolBounds[s1] = glyphBounds;
							maxSymbolWidth = MAX(glyphBounds.size.width, maxSymbolWidth);
							
							if (s2 == 0)
							{
							//	NSLog(@"char: %c %f", [symbolTable characterAtIndex:s1], symbolBounds[s1].origin.x);
							}
						}
						
						if (runGlyphIndex == 0)
							distanceTable[s1][s2] = glyphWidth;
					}
				}else if (runCount == 2)
				{
					int runIndex;
					for (runIndex = 0; runIndex < runCount; ++runIndex)
					{
						CTRunRef run = (CTRunRef)CFArrayGetValueAtIndex(runArray, runIndex);
						CFIndex	runGlyphCount = CTRunGetGlyphCount(run);
						assert(runGlyphCount == 1);
						CFRange glyphRange = CFRangeMake(0, 1);
											
						// Center this glyph by moving left by half its width.
						CGFloat glyphWidth = //glyphArcInfo[runGlyphIndex + glyphOffset].width;
						CTRunGetTypographicBounds((CTRunRef)run, glyphRange, NULL, NULL, NULL);
						
						if (runIndex == 0)
						{
							CGRect lineMetrics;
							CGFloat ascent, descent;
							CTRunGetTypographicBounds(run, glyphRange, &ascent, &descent, NULL);
							lineMetrics.origin.x = 0;
							lineMetrics.origin.y = - descent;
							lineMetrics.size.width = glyphWidth; 
							lineMetrics.size.height = ascent + descent;
							fontHeight = ascent + descent;
							fontWidth += glyphWidth + 3;
							
							
							//NSLog(@"%c asc:%f desc: %f", [symbolTable characterAtIndex:s1], ascent, descent);
							
							// wrong glyph size calculation
							//maxSymbolWidth = MAX(glyphWidth, maxSymbolWidth);
							
							symbolTypoBounds[s1] = lineMetrics;
							
							CGRect glyphBounds = CTRunGetImageBounds(run, tempContext, glyphRange);
							symbolBounds[s1] = glyphBounds;
							maxSymbolWidth = MAX(glyphBounds.size.width, maxSymbolWidth);
							
							if (s2 == 0)
							{
								//	NSLog(@"char: %c %f", [symbolTable characterAtIndex:s1], symbolBounds[s1].origin.x);
							}
						}
						
						if (runIndex == 0)
							distanceTable[s1][s2] = glyphWidth;
					}
				}else {
					assert("runCount != 1 & runCount != 2" && NULL);
				}

				
				CFRelease(line);
			}
	for (s1 = 0; s1 < [symbolTable length]; ++s1)
		for (s2 = 0; s2 < [symbolTable length]; ++s2)
		{
			distanceTable[s1][s2] -= symbolBounds[s1].size.width;
		}
	
	NSMutableDictionary * counts = [NSMutableDictionary dictionaryWithCapacity: 25000];
	for (s1 = 0; s1 < [symbolTable length]; ++s1)
		for (s2 = 0; s2 < [symbolTable length]; ++s2)
		{	
			float distance = distanceTable[s1][s2];
			NSNumber * key = [NSNumber numberWithFloat: distance];
			NSNumber * count = [counts objectForKey: key];
			if (count == nil)
			{
				[counts setObject:[NSNumber numberWithInteger:1]  forKey:key];
			}else 
			{
				[counts setObject:[NSNumber numberWithInteger:([count intValue] + 1)]  forKey:key];
			}
		}
	
//	//printf("Distance table:\n");
//	for (s1 = 0; s1 < [symbolTable length]; ++s1)
//	{
//		for (s2 = 0; s2 < [symbolTable length]; ++s2)
//		{
//			float distance = distanceTable[s1][s2];
//			//printf("%0.4f ", distance);
//			//NSLog(@"%d, %d = %f", s1, s2, distance);
//		}
//		//printf("\n");
//	}
	float maxValue = 0.0f;
	int maxCount = 0;
	NSNumber *key;
	for(key in counts)
	{
		//NSLog(@"Key: %@, Value %@", key, [counts objectForKey: key]);
		if ([[counts objectForKey: key] intValue] > maxCount)
		{
			maxCount = [[counts objectForKey: key] intValue];
			maxValue = [key floatValue];
		}
	}
	
	
	NSArray *sortedKeys = [counts keysSortedByValueUsingSelector:@selector(compare:)];
	for (id key in sortedKeys)
	{
		id val = [counts objectForKey:key];
		//NSLog(@"Key: %@, Value %@", key, val);
	}
	
	
	defaultShiftValue = maxValue;
	
	FILE * fp = fopen("kerning.txt", "wt");
	for (s1 = 0; s1 < [symbolTable length]; ++s1)	
	{
		unichar c = [symbolTable characterAtIndex:s1];
		if (c <= 255)
		{
			fprintf(fp, "sym: %c (%f,%f,%f,%f)\n", (char)c, symbolBounds[s1].origin.x, symbolBounds[s1].origin.y, symbolBounds[s1].size.width, symbolBounds[s1].size.height);
		}
	}
	
	NSMutableDictionary * kerningCounts = [NSMutableDictionary dictionaryWithCapacity: 25000];
	fprintf(fp, "Kerning table: %f\n", defaultShiftValue);
	
	kerningPairCount = 0;
	for (s1 = 0; s1 < [symbolTable length]; ++s1)
	{
		for (s2 = 0; s2 < [symbolTable length]; ++s2)
		{
			kerningTable[s1][s2] = distanceTable[s1][s2] - defaultShiftValue;
						
			NSNumber * key = [NSNumber numberWithFloat: kerningTable[s1][s2]];
			NSNumber * count = [kerningCounts objectForKey: key];
			if (count == nil)
			{
				[kerningCounts setObject:[NSNumber numberWithInteger:1]  forKey:key];
			}else 
			{
				[kerningCounts setObject:[NSNumber numberWithInteger:([count intValue] + 1)]  forKey:key];
			}
		}
		
		[self findMaxInLine:s1];
		fprintf(fp, "base: %4.4f custom: ", kerningBaseShift[s1]);
		const float FONT_KERN_EPSILON = 1.0f;
		for (s2 = 0; s2 < [symbolTable length]; ++s2)
		{
			fprintf(fp, "%4.4f ", kerningTable[s1][s2]);

			if (kerningTable[s1][s2] != 0.0f)
				kerningPairCount++;
			
//			if (fabs(kerningTable[s1][s2]) >= FONT_KERN_EPSILON)
//				kerningPairCount++;
//			else 
//				kerningTable[s1][s2] = 0.0f;
			
//			fixedKerningTable[s1][s2] = (signed char)floor(kerningTable[s1][s2]);
			fixedKerningTable[s1][s2] = (signed char)(kerningTable[s1][s2] + 0.5f);
		}
		fprintf(fp, "\n");
	}
	fprintf(fp, "Non-zero count: %d\n", kerningPairCount);
	fclose(fp);
	
	sortedKeys = [kerningCounts keysSortedByValueUsingSelector:@selector(compare:)];
	for (id key in sortedKeys)
	{
		id val = [kerningCounts objectForKey:key];
		//NSLog(@"Key: %@, Value %@", key, val);
	}
	
	CGContextRelease(tempContext);
	
	if (symbolTableContext)
	{
		CGContextRelease(symbolTableContext);
	}
	if (symbolTableImage)
	{
		CGImageRelease(symbolTableImage);
	}
	
	exportWidth = (int)(maxSymbolWidth + 2 * leftRightPadding);
	exportHeight = (int)(fontHeight + 2 * topBottomPadding);
	
	symbolTableContext = [self MyCreateBitmapContext:exportWidth * [symbolTable length] :exportHeight];
//	CGFloat color[] = {1.0f, 1.0f, 1.0f, 1.0f};
//	CGContextSetFillColor(symbolTableContext, color);
//	CGContextFillRect(symbolTableContext, CGRectMake(0, 0, exportWidth * [symbolTable length], exportHeight));
	[self exportStringToContext:symbolTable context: symbolTableContext export: true];	
	symbolTableImage = CGBitmapContextCreateImage(symbolTableContext);
}

- (int) getIndexForChar: (unichar)c
{
	int k;
	for (k = 0; k < [symbolTable length]; ++k)
		if (c == [symbolTable characterAtIndex:k])
		{
			return k;
		}
	return 0;
}

- (void) drawChar: (unichar)c atPosition: (CGPoint)position toContext: (CGContextRef)context
{
	int frame = [self getIndexForChar: c];
	
	if ((self.showsGlyphBounds) != 0) 
	{
		//int s1Index = [self getIndexForChar: [exportString characterAtIndex:s1]];;
		CGRect symbolImageBounds = symbolBounds[frame];
		symbolImageBounds.origin.x = position.x;
		symbolImageBounds.origin.y += position.y + fontDescent + topBottomPadding;
		CGContextSetRGBStrokeColor(context, 0.0, 0.0, 1.0, 1.0);
		CGContextStrokeRect(context, symbolImageBounds);
	}
	
	
	position.x = (int) (position.x + 0.5f);
	position.y = (int) (position.y + 0.5f);
	
	CGRect clipRect =  CGRectMake(position.x - leftRightPadding, position.y, exportWidth, exportHeight);
	CGContextSaveGState(context);
	
	CGContextSetRGBFillColor(context, 1.0f, 0.0f, 0.0f, 1.0f);
	CGContextFillRect(context, CGRectMake(position.x, position.y, 1, 1));

	
	//CGFloat color[] = {1.0f, 1.0f, 1.0f, 1.0f};
	//CGContextSetStrokeColor(context, color);
	CGContextClipToRect(context, clipRect);
	CGContextDrawImage(context, CGRectMake(position.x - frame * exportWidth - leftRightPadding, position.y, exportWidth * [symbolTable length], exportHeight), symbolTableImage);
	//CGContextDrawImage(context, CGRectMake(0, 0, exportWidth * [symbolTable length], exportHeight), symbolTableImage);
	CGContextRestoreGState(context);
	
	

}

- (void) drawString: (NSString*) str atPosition: (CGPoint)position toContext: (CGContextRef)context
{
	int s1;
	for (s1 = 0; s1 < [str length]; ++s1)
	{
		int s1Index = [self getIndexForChar: [str characterAtIndex:s1]];;

		CGPoint drawPosition = position;
		//if (s1 != 0)
		drawPosition.x += symbolBounds[s1Index].origin.x;
		
		[self drawChar: [str characterAtIndex:s1] atPosition: drawPosition toContext: context];
		
		
		int s2Index = -1;
		if (s1 + 1 < [str length])
			s2Index = [self getIndexForChar: [str characterAtIndex:s1 + 1]];

		position.x += symbolBounds[s1Index].size.width;
		NSLog(@"%c w:%f pos: %f\n", [str characterAtIndex:s1], symbolBounds[s1Index].size.width, position.x); 

		position.x += defaultShiftValue;

		if (s2Index != -1)
		{
			position.x += kerningBaseShift[s1Index] + kerningTable[s1Index][s2Index];

			NSLog(@"kern: %c-%c = %f", [str characterAtIndex:s1], [str characterAtIndex:s1 + 1], defaultShiftValue + kerningBaseShift[s1Index] + kerningTable[s1Index][s2Index]);
		}
	}
}

- (void) exportStringToContext:(NSString *) exportString context: (CGContextRef)context export: (bool) export
{
	NSDictionary *attributes = [NSDictionary dictionaryWithObjectsAndKeys:self.font, NSFontAttributeName, [NSNumber numberWithInteger:0], NSLigatureAttributeName, nil];
	assert(attributes != nil);

	CGPoint textPosition = CGPointMake(0, 0);
	
	int s1;
	for (s1 = 0; s1 < [exportString length]; ++s1)
	{
		// Create the attributed string
		unichar tsarr[] = {[exportString characterAtIndex:s1], 0};
		NSString * ts = [NSString stringWithCharacters:tsarr length: 1];
		NSAttributedString *attrString = [[[NSAttributedString alloc] initWithString:ts attributes:attributes] autorelease];
	
		CTLineRef line = CTLineCreateWithAttributedString((CFAttributedStringRef)attrString);
		assert(line != NULL);

		CFArrayRef runArray = CTLineGetGlyphRuns(line);
		CFIndex runCount = CFArrayGetCount(runArray);
		assert(runCount == 1);
		
		CTRunRef run = (CTRunRef)CFArrayGetValueAtIndex(runArray, 0);
		CFIndex	runGlyphCount = CTRunGetGlyphCount(run);
		CTFontRef runFont = CFDictionaryGetValue(CTRunGetAttributes(run), kCTFontAttributeName);
		assert(runGlyphCount == 1);
		
		CFRange wholeLineRange = CFRangeMake(0, runGlyphCount);

		CGFloat runAscent, runDescent, runLeading;
		CTRunGetTypographicBounds(run, wholeLineRange, &runAscent, &runDescent, &runLeading);
		
		
		CFIndex runGlyphIndex = 0;
		for (; runGlyphIndex < runGlyphCount; runGlyphIndex++) 
		{
			CFRange glyphRange = CFRangeMake(runGlyphIndex, 1);
			CGFontRef cgFont = CTFontCopyGraphicsFont(runFont, NULL);
			CGGlyph glyph;
			CGPoint position;
			
			CTRunGetGlyphs(run, glyphRange, &glyph);
			CTRunGetPositions(run, glyphRange, &position);
			
			CGContextSetFont(context, cgFont);
			CGContextSetFontSize(context, CTFontGetSize(runFont));
			
			CGContextSetRGBFillColor(context, 1.0f, 1.0f, 1.0f, 1.0f);
			CGContextSetRGBStrokeColor(context, 1.0f, 1.0f, 1.0f, 1.0f);
			position.x = textPosition.x;
			position.y = textPosition.y + runDescent;
			if (!isPrecisionKerning)
			{
				position.y += runAscent + runDescent;
			}
			if (export)
			{
				int s1Index = [self getIndexForChar: [exportString characterAtIndex:s1]];;
				position.x += leftRightPadding - symbolBounds[s1Index].origin.x;
				position.y += topBottomPadding;
			}
			CGContextShowGlyphsAtPositions(context, &glyph, &position, 1);
			
			if ((self.showsGlyphBounds) != 0) 
			{
				int s1Index = [self getIndexForChar: [exportString characterAtIndex:s1]];;

				CGRect symbolImageBounds = symbolBounds[s1Index];
				symbolImageBounds.origin.x += textPosition.x;
				symbolImageBounds.origin.y += textPosition.y + runDescent;
				if (!isPrecisionKerning)
				{
					symbolImageBounds.origin.y += runAscent + runDescent;
				}
				CGContextSetRGBStrokeColor(context, 0.0, 0.0, 1.0, 1.0);
				CGContextStrokeRect(context, symbolImageBounds);
			}
			
			if ((self.showsLineMetrics) != 0) 
			{
				int s1Index = [self getIndexForChar: [exportString characterAtIndex:s1]];;
				CGRect lineMetrics;
				// The glyph is centered around the y-axis
				lineMetrics.origin.x = textPosition.x;// - halfGlyphWidth;
				lineMetrics.origin.y = textPosition.y;
				if (!isPrecisionKerning)
				{
					lineMetrics.origin.y += runAscent + runDescent;
				}
				lineMetrics.size.width = symbolTypoBounds[s1Index].size.width; 
				lineMetrics.size.height = fontHeight;
				

				CGContextSetRGBStrokeColor(context, 1.0, 0.0, 0.0, 1.0);
				CGContextStrokeRect(context, lineMetrics);
			}
			
			CFRelease(cgFont);
		}
		
		CFRelease(line);
		
		if (!export)
		{
			int s1Index = [self getIndexForChar: [exportString characterAtIndex:s1]];;
			int s2Index = -1;
			if (s1 + 1 < [exportString length])
				s2Index = [self getIndexForChar: [exportString characterAtIndex:s1 + 1]];
			
			textPosition.x += symbolBounds[s1Index].size.width;

			textPosition.x += defaultShiftValue;
			if (s2Index != -1)
			{
				if (isPrecisionKerning)
					textPosition.x += kerningBaseShift[s1Index] + kerningTable[s1Index][s2Index];
				else {
					textPosition.x += kerningBaseShift[s1Index] + fixedKerningTable[s1Index][s2Index];
				}
			}
		}else {
			textPosition.x += exportWidth;
		}
	}		
}

- (void) exportFontToFile
{	
	// Create the File Open Dialog class.
	NSSavePanel* openDlg = [NSSavePanel savePanel];
	
	// Enable the selection of files in the dialog.
	//[openDlg setCanChooseFiles:YES];
	
	// Enable the selection of directories in the dialog.
	//[openDlg setCanChooseDirectories:YES];
	
	[openDlg setAllowedFileTypes: [NSArray arrayWithObject: @"def"]];
	
	// Display the dialog.  If the OK button was pressed,
	// process the files.
	
	NSString * fileDefName = @"font.def";
	if ( [openDlg runModalForDirectory:nil file:nil] == NSOKButton )
	{
		// Get an array containing the full filenames of all
		// files and directories selected.
		fileDefName = [openDlg filename];
		NSLog(@"filename: %@", fileDefName);
	}else 
	{
		return;
	}

	
	NSString * withoutExt = [fileDefName stringByDeletingPathExtension];
	
	exportWidth = (int)(maxSymbolWidth + 2 * leftRightPadding);
	exportHeight = (int)(fontHeight + 2 * topBottomPadding);
	
	CGContextRef bitmapContext = [self MyCreateBitmapContext:exportWidth * [symbolTable length] :exportHeight];
	[self exportStringToContext:symbolTable context: bitmapContext export: true];

	CGImageRef imageRef = CGBitmapContextCreateImage(bitmapContext);
	ExportImage(imageRef, [withoutExt stringByAppendingString:@".png"]); 
	CGImageRelease(imageRef);
	CGContextRelease(bitmapContext);
	
	FILE * fontFP = fopen([[withoutExt stringByAppendingString:@".def"] UTF8String], "wb");
	
	char header[5] = "FDEF";
	
	fwrite(header, 4, 1, fontFP);
	uint32 version = 1;
	fwrite(&version, 4, 1, fontFP);
	
	fwrite(&fontAscent, 4, 1, fontFP);
	fwrite(&fontDescent, 4, 1, fontFP);
	fwrite(&fontLeading, 4, 1, fontFP);
	fwrite(&fontXHeight, 4, 1, fontFP);
	fwrite(&leftRightPadding, 4, 1, fontFP);
	fwrite(&topBottomPadding, 4, 1, fontFP);
	
	
	int tableLength = [symbolTable length];
	fwrite(&tableLength, 4, 1, fontFP);
	
	int t;
	for (t = 0; t < [symbolTable length]; ++t)
	{
		unichar c = [symbolTable characterAtIndex:t];
		fwrite(&c, 2, 1, fontFP); 
		fwrite(&symbolBounds[t].origin.x, 4, 1, fontFP);
		fwrite(&symbolBounds[t].size.width, 4, 1, fontFP);
	}
	
	fwrite(&defaultShiftValue, 4, 1, fontFP);

	for (t = 0; t < [symbolTable length]; ++t)
	{
		fwrite(&kerningBaseShift[t], 4, 1, fontFP);
	}
	
	fwrite(&kerningPairCount, 4, 1, fontFP);
	int s1, s2;
	for (s1 = 0; s1 < [symbolTable length]; ++s1)
		for (s2 = 0; s2 < [symbolTable length]; ++s2)
			if (kerningTable[s1][s2] != 0.0f)
			{
				unsigned short s1short = s1;
				fwrite(&s1short, 2, 1, fontFP); 
				unsigned short s2short = s2;
				fwrite(&s2short, 2, 1, fontFP); 
				
				fwrite(&kerningTable[s1][s2], 4, 1, fontFP);
			}
	
	fclose(fontFP);
	
	FILE * fontPngDefFP = fopen([[withoutExt stringByAppendingString:@".pngdef"] UTF8String], "wt");
	fprintf(fontPngDefFP, "%d", tableLength);
	fclose(fontPngDefFP);

	
	/*CTLineRef line = CTLineCreateWithAttributedString((CFAttributedStringRef)self.attributedString);
	assert(line != NULL);

	CFIndex glyphCount = CTLineGetGlyphCount(line);
	if (glyphCount == 0) 
	{
		CFRelease(line);
		return;
	}
	
	CFArrayRef runArray = CTLineGetGlyphRuns(line);
	CFIndex runCount = CFArrayGetCount(runArray);
	
	if (runCount != 1)
	{
		CFRelease(line);
		return;
	}
	
	CTRunRef run = (CTRunRef)CFArrayGetValueAtIndex(runArray, 0);
	CFIndex	runGlyphCount = CTRunGetGlyphCount(run);
	CTFontRef runFont = CFDictionaryGetValue(CTRunGetAttributes(run), kCTFontAttributeName);
	
	
	CFRange wholeLineRange = CFRangeMake(0, runGlyphCount);
	
	CGFloat fontAscent, fontDescent, fontLeading;
	CTRunGetTypographicBounds(run, wholeLineRange, &fontAscent, &fontDescent, &fontLeading);
	
//	CGContextRef screenContext = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
//	CGFloat length = 0.0f;
//	CFIndex runGlyphIndex = 0;
//	for (; runGlyphIndex < runGlyphCount; runGlyphIndex++) 
//	{
//		CFRange glyphRange = CFRangeMake(runGlyphIndex, 1);
//		CGRect glyphBounds = CTRunGetImageBounds(run, screenContext, glyphRange);
//		length += glyphBounds.size.width;
//	}

	
	// get font 
	CGContextRef bitmapContext = MyCreateBitmapContext(1024, fontAscent + fontDescent);
	CGPoint textPosition = CGPointMake(0.0, 0.0f);

	
	CFIndex runGlyphIndex = 0;
	for (; runGlyphIndex < runGlyphCount; runGlyphIndex++) 
	{
		CFRange glyphRange = CFRangeMake(runGlyphIndex, 1);
		//CGContextRotateCTM(context, -(glyphArcInfo[runGlyphIndex + glyphOffset].angle));
		
		// Center this glyph by moving left by half its width.
		CGFloat glyphWidth = //glyphArcInfo[runGlyphIndex + glyphOffset].width;
		CTRunGetTypographicBounds((CTRunRef)run, glyphRange, NULL, NULL, NULL);
		
		CGFloat halfGlyphWidth = glyphWidth / 2.0;
		CGPoint positionForThisGlyph = CGPointMake(textPosition.x - halfGlyphWidth, textPosition.y);
		

			// We need to draw the glyphs manually in this case because we are effectively applying a graphics operation by setting the context fill color. Normally we would use kCTForegroundColorAttributeName, but this does not apply as we don't know the ranges for the colors in advance, and we wanted demonstrate how to manually draw.
		{
			CGFontRef cgFont = CTFontCopyGraphicsFont(runFont, NULL);
			CGGlyph glyph;
			CGPoint position;
			
			CTRunGetGlyphs(run, glyphRange, &glyph);
			CTRunGetPositions(run, glyphRange, &position);
			
			CGContextSetFont(bitmapContext, cgFont);
			CGContextSetFontSize(bitmapContext, CTFontGetSize(runFont));
			//				CGContextSetRGBFillColor(context, 0.25, 0.25, 0.25, 0.5);
			CGContextSetRGBFillColor(bitmapContext, 0.0, 0.0f, 0.0f, 1.0f);
			
			position.x = textPosition.x;
			position.y = textPosition.y + fontDescent;
			CGContextShowGlyphsAtPositions(bitmapContext, &glyph, &position, 1);
			
			CFRelease(cgFont);
		}
		
		// Draw the glyph bounds 
		//if ((self.showsGlyphBounds) != 0) 
		{
			CGRect glyphBounds = CTRunGetImageBounds(run, bitmapContext, glyphRange);
			glyphBounds.origin.x = textPosition.x; 
			CGContextSetRGBStrokeColor(bitmapContext, 0.0, 0.0, 1.0, 1.0);
			CGContextStrokeRect(bitmapContext, glyphBounds);
		}
		// Draw the bounding boxes defined by the line metrics
		if ((self.showsLineMetrics) != 0) 
		{
			CGRect lineMetrics;
			CGFloat ascent, descent;
			
			CTRunGetTypographicBounds(run, glyphRange, &ascent, &descent, NULL);
			
			// The glyph is centered around the y-axis
			lineMetrics.origin.x = textPosition.x;// - halfGlyphWidth;
			lineMetrics.origin.y = positionForThisGlyph.y - descent;
			lineMetrics.size.width = glyphWidth; 
			lineMetrics.size.height = ascent + descent;
			
			CGContextSetRGBStrokeColor(bitmapContext, 0.0, 1.0, 0.0, 0.3);
			CGContextStrokeRect(bitmapContext, lineMetrics);
			
			CGRect rect1 = {textPosition.x, textPosition.y, 1, 1};
			CGContextSetRGBStrokeColor(bitmapContext, 1.0, 0.0, 0.0, 1.0);
			CGContextStrokeRect(bitmapContext, rect1);
			
			//				CGRect rect1 = {0, lineMetrics.origin.y - ascent, 10000, 1};
			//				CGContextSetRGBStrokeColor(context, 0.0, 1.0, 0.0, 1.0);
			//				CGContextStrokeRect(context, rect1);
		}
		CGRect glyphBounds = CTRunGetImageBounds(run, bitmapContext, glyphRange);
		textPosition.x += 80;
		//textPosition.x += glyphWidth;
	}
	*/
	
//	CGImageRef imageRef = CGBitmapContextCreateImage(bitmapContext);
//	ExportImage(imageRef); 
//	
//	
//	CFRelease(line);	
}

- (void)drawRect:(NSRect)rect 
{
	// Don't draw if we don't have a font or string
	if (self.font == NULL || self.string == NULL) 
		return;

	
	// Initialize the text matrix to a known value
	CGContextRef context = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
	CGContextSetTextMatrix(context, CGAffineTransformIdentity);
	
	// Draw a white background
	//[[NSColor clearColor] set];
	//[[NSColor redColor] set];
	//NSRectFill(rect);

	CGContextSetRGBFillColor(context, 0.0f, 0.0f, 0.0f, 1.0f);
	CGContextFillRect(context, CGRectMake(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height));
	
	
	CGContextSaveGState(context);
	
//	CGContextSetInterpolationQuality(context, kCGInterpolationHigh);
//	CGContextSetShouldAntialias(context, true);
//	CGContextSetAllowsAntialiasing(context, true);
//	CGContextSetAllowsFontSmoothing(context, true);
//	CGContextSetShouldSmoothFonts(context, true);
//	CGContextSetAllowsFontSubpixelPositioning(context, true);
//	CGContextSetShouldSubpixelPositionFonts(context, true);
//	CGContextSetAllowsFontSubpixelQuantization(context, true);
//	CGContextSetShouldSubpixelQuantizeFonts(context, true);
//	CGContextSetTextDrawingMode(context, kCGTextFill);
	
	
	isPrecisionKerning = true;
	[self exportStringToContext:self.string context: context export:false];
//	isPrecisionKerning = false;
//	if (enableShadow)
//	{
//		float components[4] = {1.0f, 1.0f, 1.0f, 1.0};
//		CGColorSpaceRef colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
//		CGColorRef shadowColor = CGColorCreate( colorSpace, components);
//		CGContextSetShadowWithColor(context, CGSizeMake(0, 0), 1.0f, shadowColor);
//		
//		CGColorRelease(shadowColor);
//		CGColorSpaceRelease(colorSpace);
//	}
//	CGContextSetShadow(context, CGSizeMake(0, 0), 0.0f);*/
//	[self exportStringToContext:self.string context: context export:false];
//	isPrecisionKerning = true;

	CGContextRestoreGState(context);
	
	
	//CGContextDrawLine(
	
	float yPos = 2 * (fontAscent + fontDescent);
	
	CGRect rect1 = {0, yPos, 10000, 0.0f};
	CGContextSetRGBStrokeColor(context, 0.0, 1.0, 0.0, 1.0);
	CGContextStrokeRect(context, rect1);

	
	CGRect rect2 = {0, yPos + fontDescent + topBottomPadding, 10000, 0.0f};
	CGContextSetRGBStrokeColor(context, 1.0, 0.0, 0.0, 0.5);
	CGContextStrokeRect(context, rect2);
	
	CGRect rect3 = {0, yPos + fontDescent + fontAscent + topBottomPadding, 10000, 0.0f};
	CGContextSetRGBStrokeColor(context, 1.0, 0.0, 0.0, 0.5);
	CGContextStrokeRect(context, rect3);

	[self drawString:self.string atPosition:CGPointMake(0, yPos) toContext: context]; 

	
	
//	CGRect r = CGRectMake(0, 3 * (fontAscent + fontDescent), exportWidth * [symbolTable length], exportHeight);
//	CGContextDrawImage(context, r , symbolTableImage);

/*	CTLineRef line = CTLineCreateWithAttributedString((CFAttributedStringRef)self.attributedString);
	assert(line != NULL);
	
	CFIndex glyphCount = CTLineGetGlyphCount(line);
	if (glyphCount == 0) {
		CFRelease(line);
		return;
	}
	
	//GlyphArcInfo *	glyphArcInfo = (GlyphArcInfo*)calloc(glyphCount, sizeof(GlyphArcInfo));
	//PrepareGlyphArcInfo(line, glyphCount, glyphArcInfo);
	
	// Move the origin from the lower left of the view nearer to its center.
	CGContextSaveGState(context);
//	CGContextTranslateCTM(context, CGRectGetMidX(NSRectToCGRect(rect)), CGRectGetMidY(NSRectToCGRect(rect)) - self.radius / 2.0);
//	
//	// Stroke the arc in red for verification.
//	CGContextBeginPath(context);
//	CGContextAddArc(context, 0.0, 0.0, self.radius, M_PI, 0.0, 1);
//	CGContextSetRGBStrokeColor(context, 1.0, 0.0, 0.0, 1.0);
//	CGContextStrokePath(context);
//	
//	// Rotate the context 90 degrees counterclockwise.
//	CGContextRotateCTM(context, M_PI_2);
	
	// Now for the actual drawing. The angle offset for each glyph relative to the previous glyph has already been calculated; with that information in hand, draw those glyphs overstruck and centered over one another, making sure to rotate the context after each glyph so the glyphs are spread along a semicircular path.
	
	CGFloat trailingWhitespaceWidth = CTLineGetTrailingWhitespaceWidth(line);
	
	
	CGPoint initialTextPosition = CGPointMake(0.0, 100.0f);
	CGPoint textPosition = initialTextPosition;
	CGPoint virtRealTextPosition = initialTextPosition;
	
	CFArrayRef runArray = CTLineGetGlyphRuns(line);
	CFIndex runCount = CFArrayGetCount(runArray);
	
	CFIndex glyphOffset = 0;
	CFIndex runIndex = 0;
	for (; runIndex < runCount; runIndex++) {
		CTRunRef run = (CTRunRef)CFArrayGetValueAtIndex(runArray, runIndex);
		CFIndex runGlyphCount = CTRunGetGlyphCount(run);
		Boolean	drawSubstitutedGlyphsManually = false;
		CTFontRef runFont = CFDictionaryGetValue(CTRunGetAttributes(run), kCTFontAttributeName);
		
		// Determine if we need to draw substituted glyphs manually. Do so if the runFont is not the same as the overall font.
		if (self.dimsSubstitutedGlyphs && ![self.font isEqual:(NSFont *)runFont]) {
			drawSubstitutedGlyphsManually = true;
		}
		
		CFIndex runGlyphIndex = 0;
		for (; runGlyphIndex < runGlyphCount; runGlyphIndex++) {
			CFRange glyphRange = CFRangeMake(runGlyphIndex, 1);
			//CGContextRotateCTM(context, -(glyphArcInfo[runGlyphIndex + glyphOffset].angle));
			
			// Center this glyph by moving left by half its width.
			CGFloat glyphWidth = //glyphArcInfo[runGlyphIndex + glyphOffset].width;
				CTRunGetTypographicBounds((CTRunRef)run, glyphRange, NULL, NULL, NULL);
			
			CGFloat halfGlyphWidth = glyphWidth / 2.0;
			CGPoint positionForThisGlyph = CGPointMake(textPosition.x - halfGlyphWidth, textPosition.y);
			
			// Glyphs are positioned relative to the text position for the line, so offset text position leftwards by this glyph's width in preparation for the next glyph.
			
			
//			CGAffineTransform textMatrix = CTRunGetTextMatrix(run);
//			textMatrix.tx = positionForThisGlyph.x;
//			textMatrix.ty = positionForThisGlyph.y;
//			CGContextSetTextMatrix(context, textMatrix);
			
//			if (!drawSubstitutedGlyphsManually) {
//				CTRunDraw(run, context, glyphRange);
//			} 
//			else 
			{
				// We need to draw the glyphs manually in this case because we are effectively applying a graphics operation by setting the context fill color. Normally we would use kCTForegroundColorAttributeName, but this does not apply as we don't know the ranges for the colors in advance, and we wanted demonstrate how to manually draw.
				CGFontRef cgFont = CTFontCopyGraphicsFont(runFont, NULL);
				CGGlyph glyph;
				CGPoint position;
				
				CTRunGetGlyphs(run, glyphRange, &glyph);
				CTRunGetPositions(run, glyphRange, &position);
				
				CGContextSetFont(context, cgFont);
				CGContextSetFontSize(context, CTFontGetSize(runFont));
//				CGContextSetRGBFillColor(context, 0.25, 0.25, 0.25, 0.5);
				CGContextSetRGBFillColor(context, 0.0, 0.0f, 0.0f, 1.0f);
				
				position.x = textPosition.x;
				position.y = textPosition.y;
 				CGContextShowGlyphsAtPositions(context, &glyph, &position, 1);
				
				CFRelease(cgFont);
			}
			
			// Draw the glyph bounds 
			if ((self.showsGlyphBounds) != 0) 
			{
				CGRect glyphBounds = CTRunGetImageBounds(run, context, glyphRange);
				
				glyphBounds.origin.x += - virtRealTextPosition.x + textPosition.x;
				glyphBounds.origin.y += initialTextPosition.y;
				CGContextSetRGBStrokeColor(context, 0.0, 0.0, 1.0, 1.0);
				CGContextStrokeRect(context, glyphBounds);
			}
			// Draw the bounding boxes defined by the line metrics
			if ((self.showsLineMetrics) != 0) 
			{
				CGRect lineMetrics;
				CGFloat ascent, descent;
				
				CTRunGetTypographicBounds(run, glyphRange, &ascent, &descent, NULL);
				
				// The glyph is centered around the y-axis
				lineMetrics.origin.x = textPosition.x;// - halfGlyphWidth;
				lineMetrics.origin.y = positionForThisGlyph.y - descent;
				lineMetrics.size.width = glyphWidth; 
				lineMetrics.size.height = ascent + descent;
				
				CGContextSetRGBStrokeColor(context, 0.0, 1.0, 0.0, 0.3);
				CGContextStrokeRect(context, lineMetrics);
				
				CGRect rect1 = {textPosition.x, textPosition.y, 1, 1};
				CGContextSetRGBStrokeColor(context, 1.0, 0.0, 0.0, 1.0);
				CGContextStrokeRect(context, rect1);

//				CGRect rect1 = {0, lineMetrics.origin.y - ascent, 10000, 1};
//				CGContextSetRGBStrokeColor(context, 0.0, 1.0, 0.0, 1.0);
//				CGContextStrokeRect(context, rect1);
			}
			//CGRect glyphBounds = CTRunGetImageBounds(run, context, glyphRange);
			textPosition.x += 60;
//			textPosition.x += glyphBounds.size.width;
			virtRealTextPosition.x += glyphWidth;
			NSLog(@"gl: %d %f", (int)runGlyphIndex, (float)glyphWidth);
		}
		
		glyphOffset += runGlyphCount;
	}
	
	CFRelease(line);	

	CGContextRestoreGState(context);
*/	
	
	//[self extractKerningValues];
	
}

@synthesize font = _font;
@synthesize string = _string;
@synthesize radius = _radius;

@dynamic attributedString;
- (NSAttributedString *)attributedString {
	// Create an attributed string with the current font and string.
	assert(self.font != nil);
	assert(self.string != nil);
	
	// Create our attributes
	NSDictionary *attributes = [NSDictionary dictionaryWithObjectsAndKeys:self.font, NSFontAttributeName, [NSNumber numberWithInteger:0], NSLigatureAttributeName, nil];
	assert(attributes != nil);
	
	// Create the attributed string
	NSAttributedString *attrString = [[NSAttributedString alloc] initWithString:self.string attributes:attributes];
	return [attrString autorelease];
}

@dynamic showsGlyphBounds;
- (BOOL)showsGlyphBounds {
	return _flags.showsGlyphBounds;
}

- (void)setShowsGlyphBounds:(BOOL)show {
	_flags.showsGlyphBounds = show ? 1 : 0;
}

@dynamic showsLineMetrics;
- (BOOL)showsLineMetrics {
	return _flags.showsLineMetrics;
}

- (void)setShowsLineMetrics:(BOOL)show {
	_flags.showsLineMetrics = show ? 1 : 0;
}

@dynamic dimsSubstitutedGlyphs;
- (BOOL)dimsSubstitutedGlyphs {
	return _flags.dimsSubstitutedGlyphs;
}

- (void)setDimsSubstitutedGlyphs:(BOOL)dim {
	_flags.dimsSubstitutedGlyphs = dim ? 1 : 0;
}

@end
