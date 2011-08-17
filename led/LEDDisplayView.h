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
//  LEDDisplayView.h
//  nonpareil
//
//  Created by Maciej Bartosiak on 2005-10-26.
//  Copyright 2005-2010 Maciej Bartosiak.
//

#import <Cocoa/Cocoa.h>
#import "LEDDigit.h"
#import "display.h"

@interface LEDDisplayView : NSView
{
	segment_bitmap_t *ds;
	int dc;
			
	NSArray *digits;
	
	segment_bitmap_t cds [MAX_DIGIT_POSITION];
}

- (void)setupDisplayWith: (segment_bitmap_t *)disps
				   count: (int) count
				 yOffset: (float) y
			 digitHeight: (float) digitHeight
			  digitWidth: (float) digitWidth
			 digitOffset: (float) digitOffset
			  digitShare: (float) digitShare
			 digitStroke: (float) digitStroke
			   dotOffset: (float) dotOffset;
- (void)updateDisplay;
@end
