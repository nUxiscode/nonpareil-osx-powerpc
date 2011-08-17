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
//  SpiceController.h
//  nonpareil
//
//  Created by Maciej Bartosiak on 2005-10-26.
//  Copyright 2005-2010 Maciej Bartosiak.
//

#import "SpiceController.h"

@implementation SpiceController

#define JIFFY_PER_SEC 30.0

- (void)awakeFromNib
{	
	[NSApp setDelegate:self];
		
	keyQueue = [[NSMutableArray alloc] init];
	simulator = [[SpiceSimulator alloc] initWithDisplay: display];
	[display setupDisplayWith: [simulator displaySegments]
						count: [simulator displayDigits]
					  yOffset: 15.0
				  digitHeight: 16.0
				   digitWidth: 8.0 
				  digitOffset: 12.0
				   digitShare: 0.1
				  digitStroke: 1.5
					dotOffset: 6.0];
	
	timer = [[NSTimer scheduledTimerWithTimeInterval:(1.0/JIFFY_PER_SEC)
											  target:self
											selector:@selector(run:)
											userInfo:nil
											 repeats:YES] retain];
	
}

- (IBAction)buttonPressed:(id)sender
{
	[keyQueue insertObject:[NSNumber numberWithInt: [sender tag]] atIndex:0];
	[keyQueue insertObject:[NSNumber numberWithInt: -1] atIndex:0];
}

- (IBAction)modeSwitch:(id)sender
{
	int fff = [sender state];
	if (fff == NSOnState)
	{
		[simulator setFlag: 3 withBool:true];
		//NSLog(@"Run");
	} else if (fff == NSOffState) {
		[simulator setFlag: 3 withBool:false];
		//NSLog(@"Prgm");
	}
}

- (void)run:(NSTimer *)aTimer
{
	[simulator readKeysFrom: keyQueue];
	[simulator executeCycle];
}

- (void)quit
{
    [timer invalidate];
    //if (! write_ram_file (ram))
	[simulator printState];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    [self quit];
}

//--------------------------------------------------------------------------------------------------------
// NSWindow delegate methods
//--------------------------------------------------------------------------------------------------------
- (void)windowDidBecomeKey:(NSNotification *)aNotification
{
    [[aNotification object] setAlphaValue:1.0];
}

- (void)windowDidResignKey:(NSNotification *)aNotification
{
    [[aNotification object] setAlphaValue:0.85];
}

//- (void)applicationWillTerminate:(NSNotification *)aNotification {
//    [self quit];
//}


@end
