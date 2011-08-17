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
//  ClassicSimulator.h
//  nonpareil
//
//  Created by Maciej Bartosiak on 2005-10-08.
//  Copyright 2005-2010 Maciej Bartosiak.
//

#import <Cocoa/Cocoa.h>
#import "LEDDisplayView.h"
#import "display.h"

#ifdef NONPAREIL_35

#define NNPR_OBJ	@"35"
#define NNPR_RAM	0
#define NNPR_CLOCK	196000.0
#define NNPR_WSIZE	56.0

#elif NONPAREIL_45

#define NNPR_OBJ	@"45"
#define NNPR_RAM	10
#define NNPR_CLOCK	196000.0
#define NNPR_WSIZE	56.0

#elif NONPAREIL_55

#define NNPR_OBJ	@"55"
#define NNPR_RAM	30
#define NNPR_CLOCK	196000.0
#define NNPR_WSIZE	56.0

#elif NONPAREIL_80

#define NNPR_OBJ	@"80"
#define NNPR_RAM	0
#define NNPR_CLOCK	196000.0
#define NNPR_WSIZE	56.0

#else

#error No type defined

#endif

#ifdef NONPAREIL_INTERNAL
#import "display.h"
#import "proc.h"
#import "digit_ops.h"
#import "proc_classic.h"
#endif

@interface ClassicSimulator : NSObject
{
	NSTimeInterval	lastRun;
#ifdef NONPAREIL_INTERNAL
	classic_cpu_reg_t *cpu;
	
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
