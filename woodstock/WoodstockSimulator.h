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
//  WoodstockSimulator.h
//  nonpareil
//
//  Created by Maciej Bartosiak on 2005-12-26.
//  Copyright 2005-2010 Maciej Bartosiak
//

#import <Cocoa/Cocoa.h>
#import "display.h"
#import "LEDDisplayView.h"

#ifdef NONPAREIL_21

#define NNPR_OBJ	@"21"
#define NNPR_RAM	0
#define NNPR_CLOCK	185000
#define NNPR_WSIZE	56.0

#elif NONPAREIL_25

#define NNPR_OBJ	@"25"
#define NNPR_RAM	16
#define NNPR_CLOCK	185000
#define NNPR_WSIZE	56.0

#elif NONPAREIL_32E

#define NNPR_OBJ	@"32e"
#define NNPR_RAM	32
#define NNPR_CLOCK	140000.0
#define NNPR_WSIZE	56.0

#elif NONPAREIL_33C

#define NNPR_OBJ	@"33c"
#define NNPR_RAM	32
#define NNPR_CLOCK	140000.0
#define NNPR_WSIZE	56.0

#elif NONPAREIL_34C

#define NNPR_OBJ	@"34c"
#define NNPR_RAM	64
#define NNPR_CLOCK	140000.0
#define NNPR_WSIZE	56.0

#elif NONPAREIL_37E

#define NNPR_OBJ	@"37e"
#define NNPR_RAM	48
#define NNPR_CLOCK	140000.0
#define NNPR_WSIZE	56.0

#elif NONPAREIL_38C

#define NNPR_OBJ	@"38c"
#define NNPR_RAM	48
#define NNPR_CLOCK	140000.0
#define NNPR_WSIZE	56.0

#elif NONPAREIL_38E

#define NNPR_OBJ	@"38e"
#define NNPR_RAM	48
#define NNPR_CLOCK	140000.0
#define NNPR_WSIZE	56.0

#else

#error No type defined

#endif

#ifdef NONPAREIL_INTERNAL
#import "display.h"
#import "proc.h"
#import "digit_ops.h"
#import "proc_woodstock.h"
#endif

@interface WoodstockSimulator : NSObject
{
	NSTimeInterval	lastRun;
#ifdef NONPAREIL_INTERNAL
	act_reg_t *cpu;
	
	//segment_bitmap_t disp_buf[MAX_DIGIT_POSITION+1];
	//segment_bitmap_t disp_old[MAX_DIGIT_POSITION+1];
#endif
}
- (id)initWithDisplay: (LEDDisplayView *)display;
- (void)pressKey: (int)key;
- (void)readKeysFrom: (NSMutableArray *) keyQueue;
- (void)executeCycle;
- (void)setFlag: (int)num withBool: (BOOL)state;
- (int)displayDigits;
- (segment_bitmap_t *)displaySegments;
- (BOOL)printState;
@end
