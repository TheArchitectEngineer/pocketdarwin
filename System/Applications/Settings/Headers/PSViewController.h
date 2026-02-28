/*
 * PSViewController.h
 * Settings.app - PocketDarwin
 *
 * Base class for all Settings panels.
 * Manages a UITableView driven by an array of PSSpecifier objects.
 * Subclasses override -specifiers to return their row definitions.
 *
 * Mirrors Apple's private PSViewController (Preferences.framework).
 *
 * iPhoneOS 2.0 compatible.
 */

#import <UIKit/UIKit.h>
#import "PSSpecifier.h"

@interface PSViewController : UIViewController
    <UITableViewDataSource, UITableViewDelegate> {

    UITableView     *_tableView;
    NSMutableArray  *_specifiers;    /* flat list of all PSSpecifier */
    NSMutableArray  *_sections;      /* array of arrays, grouped by PSGroupCell */
    NSMutableArray  *_sectionTitles; /* NSString or NSNull for untitled sections */
    NSMutableArray  *_sectionFooters;
}

/* Subclasses return an array of PSSpecifier objects */
- (NSArray *)specifiers;

/* Reload specifiers and rebuild internal section model */
- (void)reloadSpecifiers;

/* Look up a specifier by its key */
- (PSSpecifier *)specifierForKey:(NSString *)key;

/* Get/set a preference value (reads/writes NSUserDefaults) */
- (id)readPreferenceValue:(PSSpecifier *)specifier;
- (void)setPreferenceValue:(id)value specifier:(PSSpecifier *)specifier;

/* Convenience for switch cells: called as getter/setter */
- (NSNumber *)getBoolForSpecifier:(PSSpecifier *)specifier;
- (void)setBoolForSpecifier:(PSSpecifier *)specifier value:(NSNumber *)value;

@end
