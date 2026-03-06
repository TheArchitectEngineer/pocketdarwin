/*
 * PSSpecifier.h
 * Settings.app - PocketDarwin
 *
 * A PSSpecifier describes one row in a settings table view.
 * This mirrors Apple's private PSSpecifier class from the
 * Preferences.framework (iPhoneOS 2.0).
 *
 * iPhoneOS 2.0 compatible.
 */

#import <Foundation/Foundation.h>

/* Cell types — match iPhoneOS 2.0 preference specifier types */
typedef enum {
    PSGroupCell         = 0,  /* Section header */
    PSLinkCell          = 1,  /* Navigates to a child controller */
    PSLinkListCell      = 2,  /* Selection list */
    PSToggleSwitchCell  = 3,  /* UISwitch */
    PSSliderCell        = 4,  /* UISlider */
    PSTextFieldCell     = 5,  /* Editable text */
    PSTitleValueCell    = 6,  /* key → value (read-only) */
    PSStaticTextCell    = 7,  /* Static display text */
    PSEditTextCell      = 8,  /* Single-line edit */
    PSSegmentCell       = 9,  /* UISegmentedControl */
    PSButtonCell        = 10, /* Tappable button */
    PSSpinnerCell       = 11  /* Activity indicator row */
} PSSpecifierType;

@interface PSSpecifier : NSObject {
    NSString            *_name;
    NSString            *_key;
    id                   _target;
    SEL                  _getter;
    SEL                  _setter;
    NSString            *_detailControllerClass;
    PSSpecifierType      _cellType;
    id                   _defaultValue;
    NSDictionary        *_userInfo;
    NSArray             *_values;       /* for PSLinkListCell */
    NSArray             *_titles;       /* for PSLinkListCell */
    NSString            *_footerText;
    NSString            *_placeholder;
    BOOL                 _enabled;
}

@property (nonatomic, copy)   NSString        *name;
@property (nonatomic, copy)   NSString        *key;
@property (nonatomic, assign) id               target;
@property (nonatomic, assign) SEL              getter;
@property (nonatomic, assign) SEL              setter;
@property (nonatomic, copy)   NSString        *detailControllerClass;
@property (nonatomic, assign) PSSpecifierType  cellType;
@property (nonatomic, retain) id               defaultValue;
@property (nonatomic, retain) NSDictionary    *userInfo;
@property (nonatomic, copy)   NSArray         *values;
@property (nonatomic, copy)   NSArray         *titles;
@property (nonatomic, copy)   NSString        *footerText;
@property (nonatomic, copy)   NSString        *placeholder;
@property (nonatomic, assign) BOOL             enabled;

/* Factory methods */
+ (PSSpecifier *)groupSpecifierWithName:(NSString *)name;
+ (PSSpecifier *)groupSpecifierWithName:(NSString *)name footer:(NSString *)footer;
+ (PSSpecifier *)preferenceSpecifierNamed:(NSString *)name
                                   target:(id)target
                                      set:(SEL)setter
                                      get:(SEL)getter
                                 cellType:(PSSpecifierType)cellType
                           editType:(PSSpecifierType)ignored;

+ (PSSpecifier *)switchSpecifierNamed:(NSString *)name
                                  key:(NSString *)key
                               target:(id)target
                                  set:(SEL)setter
                                  get:(SEL)getter;

+ (PSSpecifier *)linkSpecifierNamed:(NSString *)name
                    controllerClass:(NSString *)controllerClass;

+ (PSSpecifier *)titleValueSpecifierNamed:(NSString *)name value:(NSString *)value;

+ (PSSpecifier *)buttonSpecifierNamed:(NSString *)name
                               target:(id)target
                               action:(SEL)action;

@end
