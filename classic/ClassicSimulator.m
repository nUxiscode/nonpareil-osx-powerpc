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
//  ClassicSimulator.m
//  nonpareil
//
//  Created by Maciej Bartosiak on 2005-10-08.
//  Copyright 2005-2010 Maciej Bartosiak
//

#define NONPAREIL_INTERNAL
#import "ClassicSimulator.h"
#import "LEDDisplayView.h"
#import <math.h>

@implementation ClassicSimulator

- (id)initWithDisplay: (LEDDisplayView *)display 
{
    self = [super init];
	
	NSBundle *nonpareilBundle = [NSBundle mainBundle];
	NSString *objFile = [nonpareilBundle pathForResource: NNPR_OBJ ofType:@"obj"];
	
	cpu = classic_new_processor (NNPR_RAM, (void *)display);
	classic_read_object_file (cpu, [objFile UTF8String]);
	
	lastRun = [NSDate timeIntervalSinceReferenceDate];
	
	return self;
}

- (void)pressKey: (int)key
{
	if (key == -1)
	{
		classic_release_key(cpu);
	} else {
		classic_press_key(cpu, key);
	}
}

- (void)readKeysFrom: (NSMutableArray *) keyQueue
{
	static int delay = 0;
	int key;
	
	if (delay)
		delay--;
	else
	{
		if([keyQueue lastObject])
		{
			key = [[keyQueue lastObject] intValue];
			[keyQueue removeLastObject];
			[self pressKey: key];
#ifdef NONPAREIL_55
			delay = 1;
#endif
		}		
	}	
}

- (void)executeCycle
{
	NSTimeInterval now = [NSDate timeIntervalSinceReferenceDate];
	int i = (int)round((now - lastRun) * (NNPR_CLOCK / NNPR_WSIZE));
	lastRun = now;
	
	if (i > 5000) i = 5000;
	
	while (i--)
	{
		classic_execute_instruction(cpu);
	}
}

- (void)setFlag: (int)num withBool: (BOOL)state
{
	classic_set_ext_flag(cpu, num, (bool)state);
}

- (int)displayDigits
{
	return cpu->display_digits;
}

- (segment_bitmap_t *)displaySegments
{
	return cpu->display_segments;
}

- (BOOL)printState
{
	//return (BOOL)classic_print_state(cpu);
	return NO;
}

@end

void display_callback(struct classic_cpu_reg_t *cr)
{
	static segment_bitmap_t o_display_segments [MAX_DIGIT_POSITION];
	
	if (memcmp( o_display_segments, cr->display_segments, MAX_DIGIT_POSITION * sizeof(segment_bitmap_t)))
	{
		memcpy( o_display_segments, cr->display_segments, MAX_DIGIT_POSITION * sizeof(segment_bitmap_t));
		[(LEDDisplayView *)cr->display updateDisplay];
	}
}

