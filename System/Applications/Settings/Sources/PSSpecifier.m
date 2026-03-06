/*
 * PSSpecifier.m
 * Settings.app - PocketDarwin
 *
 * iPhoneOS 2.0 — Manual Retain/Release
 */

#import "PSSpecifier.h"

@implementation PSSpecifier

@synthesize name                 = _name;
@synthesize key                  = _key;
@synthesize target               = _target;
@synthesize getter               = _getter;
@synthesize setter               = _setter;
@synthesize detailControllerClass = _detailControllerClass;
@synthesize cellType             = _cellType;
@synthesize defaultValue         = _defaultValue;
@synthesize userInfo             = _userInfo;
@synthesize values               = _values;
@synthesize titles               = _titles;
@synthesize footerText           = _footerText;
@synthesize placeholder          = _placeholder;
@synthesize enabled              = _enabled;

- (id)init
{
    if ((self = [super init])) {
        _enabled = YES;
    }
    return self;
}

#pragma mark - Factory methods

+ (PSSpecifier *)groupSpecifierWithName:(NSString *)name
{
    PSSpecifier *s = [[[PSSpecifier alloc] init] autorelease];
    s.name     = name;
    s.cellType = PSGroupCell;
    return s;
}

+ (PSSpecifier *)groupSpecifierWithName:(NSString *)name footer:(NSString *)footer
{
    PSSpecifier *s = [self groupSpecifierWithName:name];
    s.footerText = footer;
    return s;
}

+ (PSSpecifier *)preferenceSpecifierNamed:(NSString *)name
                                   target:(id)target
                                      set:(SEL)setter
                                      get:(SEL)getter
                                 cellType:(PSSpecifierType)cellType
                                 editType:(PSSpecifierType)ignored
{
    PSSpecifier *s = [[[PSSpecifier alloc] init] autorelease];
    s.name     = name;
    s.target   = target;
    s.setter   = setter;
    s.getter   = getter;
    s.cellType = cellType;
    return s;
}

+ (PSSpecifier *)switchSpecifierNamed:(NSString *)name
                                  key:(NSString *)key
                               target:(id)target
                                  set:(SEL)setter
                                  get:(SEL)getter
{
    PSSpecifier *s = [[[PSSpecifier alloc] init] autorelease];
    s.name     = name;
    s.key      = key;
    s.target   = target;
    s.setter   = setter;
    s.getter   = getter;
    s.cellType = PSToggleSwitchCell;
    return s;
}

+ (PSSpecifier *)linkSpecifierNamed:(NSString *)name
                    controllerClass:(NSString *)controllerClass
{
    PSSpecifier *s = [[[PSSpecifier alloc] init] autorelease];
    s.name                  = name;
    s.cellType              = PSLinkCell;
    s.detailControllerClass = controllerClass;
    return s;
}

+ (PSSpecifier *)titleValueSpecifierNamed:(NSString *)name value:(NSString *)value
{
    PSSpecifier *s = [[[PSSpecifier alloc] init] autorelease];
    s.name         = name;
    s.defaultValue = value;
    s.cellType     = PSTitleValueCell;
    s.enabled      = NO;
    return s;
}

+ (PSSpecifier *)buttonSpecifierNamed:(NSString *)name
                               target:(id)target
                               action:(SEL)action
{
    PSSpecifier *s = [[[PSSpecifier alloc] init] autorelease];
    s.name     = name;
    s.target   = target;
    s.setter   = action;
    s.cellType = PSButtonCell;
    return s;
}

#pragma mark - Dealloc

- (void)dealloc
{
    [_name                  release];
    [_key                   release];
    [_detailControllerClass release];
    [_defaultValue          release];
    [_userInfo              release];
    [_values                release];
    [_titles                release];
    [_footerText            release];
    [_placeholder           release];
    [super dealloc];
}

@end
