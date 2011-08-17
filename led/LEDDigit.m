/*
 $Id$
 Copyright 1995, 2003, 2004, 2005 Eric L. Smith <eric@brouhaha.com>
 
 Nonpareil is free software; you can redistribute it and/or modify it
 under the terms of the GNU General Public License version 2 as
 published by the Free Software Foundation.  Note that I am not
 granting permission to redistribute or modify Nonpareil under the
 terms of any later version of the General Public License.
 
 Nonpareil is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program (in the file "COPYING"); if not, write to the
 Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 MA 02111, USA.
 */

//
//  LEDDigit.m
//  nonpareil
//
//  Created by Maciej Bartosiak on 2005-10-26.
//  Copyright 2005 Maciej Bartosiak.
//

#import "LEDDigit.h"

@implementation LEDDigit

- (id)initWithDigitHeight: (float) digitH 
					width: (float) digitW 
					share: (float) share
				   stroke: (float) stroke
				dotOffset: (float) dotOff
						x: (float) x
						y: (float) y
{
	NSAffineTransform *tmp;
	
	float digith = digitH / 2.0;
	//float digitoff = (digitW + digitS * 2.0);
	
    self = [super init];
	
	[NSBezierPath setDefaultLineWidth: stroke];
	//[NSBezierPath setDefaultLineCapStyle: NSRoundLineCapStyle];
	
	a = [[NSBezierPath bezierPath] retain];
	[a moveToPoint:			NSMakePoint(    0.0, digitH)];
	[a relativeLineToPoint:	NSMakePoint( digitW,    0.0)];
	
	b = [[NSBezierPath bezierPath] retain];
	[b moveToPoint:			NSMakePoint( digitW, digitH)];
	[b relativeLineToPoint:	NSMakePoint(    0.0,-digith)];
	
	c = [b copy];
	tmp = [NSAffineTransform transform];
	[tmp translateXBy: 0.0 yBy: digitH];
	[tmp scaleXBy: 1.0 yBy: -1.0];
	[c transformUsingAffineTransform: tmp];
	
	d = [a copy];
	tmp = [NSAffineTransform transform];
	[tmp translateXBy: 0.0 yBy: digitH];
	[tmp scaleXBy: 1.0 yBy: -1.0];
	[d transformUsingAffineTransform: tmp];
	
	e = [b copy];
	tmp = [NSAffineTransform transform];
	[tmp translateXBy: digitW yBy: digitH];
	[tmp scaleXBy: -1.0 yBy: -1.0];
	[e transformUsingAffineTransform: tmp];
	
	f = [c copy];
	tmp = [NSAffineTransform transform];
	[tmp translateXBy: digitW yBy: digitH];
	[tmp scaleXBy: -1.0 yBy: -1.0];
	[f transformUsingAffineTransform: tmp];
	
	g = [[NSBezierPath bezierPath] retain];
	[g moveToPoint:			NSMakePoint(   0.0, digith)];
	[g relativeLineToPoint:	NSMakePoint(digitW,    0.0)];
	
	// "dot" segment
	h = [[NSBezierPath bezierPath] retain];
	[h moveToPoint:			NSMakePoint(digitW+dotOff, -1.0)];
	[h relativeLineToPoint:	NSMakePoint(           0.0, 2.0)];
	[h relativeMoveToPoint:	NSMakePoint(          -1.0, 0.0)];
	[h relativeLineToPoint:	NSMakePoint(           0.0,-2.0)];
	//[h closePath];
	
	// "," segment
	
	i = [[NSBezierPath bezierPath] retain];
	[i moveToPoint:			NSMakePoint(digitW + dotOff, 0.0 /*-DIGIT_o*/)];
	[i relativeLineToPoint:	NSMakePoint(    -dotOff/3.0,      -dotOff/2.0)];
	
	tmp = [NSAffineTransform transform];
	[tmp shearXBy: share]; 
	[tmp translateXBy: x + 0.5 yBy: y + 0.5];
	[a transformUsingAffineTransform: tmp];
	[b transformUsingAffineTransform: tmp];
	[c transformUsingAffineTransform: tmp];
	[d transformUsingAffineTransform: tmp];
	[e transformUsingAffineTransform: tmp];
	[f transformUsingAffineTransform: tmp];
	[g transformUsingAffineTransform: tmp];
	[h transformUsingAffineTransform: tmp];
	[i transformUsingAffineTransform: tmp];
	
	return self;
}

- (id)init
{
	return [self initWithDigitHeight: (float) 16.0 
							   width: (float) 8.0
							   share: (float) 0.1
							  stroke: (float) 1.5
						   dotOffset: (float) 6.0
								   x: (float) 0.0
								   y: (float) 0.0];
}

- (void) drawDigit: (segment_bitmap_t)dig
{
#if 0
	[[NSColor blackColor] set];
	[a stroke];
	[b stroke];
	[c stroke];
	[d stroke];
	[e stroke];
	[f stroke];
	[g stroke];
	[h stroke];
	[i stroke];
	[[NSColor redColor] set];
#endif
	if((dig >> 0) & 1) [a stroke];
	if((dig >> 1) & 1) [b stroke];
	if((dig >> 2) & 1) [c stroke];
	if((dig >> 3) & 1) [d stroke];
	if((dig >> 4) & 1) [e stroke];
	if((dig >> 5) & 1) [f stroke];
	if((dig >> 6) & 1) [g stroke];
	if((dig >> 7) & 1) [h stroke];
	if((dig >> 8) & 1) [i stroke];
}

@end
