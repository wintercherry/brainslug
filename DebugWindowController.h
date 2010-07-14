//
//    Brainslug is a media center application.
//    Copyright (C) 2009 Benjamin Prucha
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

#import <Cocoa/Cocoa.h>

@class BrainslugController;

@interface DebugWindowController : NSObject {
	IBOutlet NSArrayController *_movieArrayController;
	IBOutlet NSImageView *_coverArtImageView;
	IBOutlet BrainslugController *_mainController;
	IBOutlet NSTextField* _logTextField;
	
	IBOutlet NSProgressIndicator *_coverArtProcessIndicator;
	
	NSMutableDictionary *_coverImageCache;

	NSManagedObjectContext *_managedObjectContext;
}

- (IBAction)rescanAllSources:sender;
- (IBAction)resetAllSources:sender;

@end
