/*
 * PSTableCell.m
 * Settings.app - PocketDarwin
 *
 * iPhoneOS 2.0 — Manual Retain/Release
 */

#import <UIKit/UIKit.h>
#import "PSTableCell.h"
#import "PSSpecifier.h"

@implementation PSTableCell

@synthesize specifier = _specifier;

- (id)initWithSpecifier:(PSSpecifier *)specifier reuseIdentifier:(NSString *)reuseID
{
    UITableViewCellStyle style = UITableViewCellStyleDefault;

    if (specifier.cellType == PSTitleValueCell) {
        style = UITableViewCellStyleValue1;
    }

    if ((self = [super initWithStyle:style reuseIdentifier:reuseID])) {
        self.specifier = specifier;
        [self _configureForSpecifier:specifier];
    }
    return self;
}

- (void)_configureForSpecifier:(PSSpecifier *)specifier
{
    self.textLabel.text = specifier.name;
    self.selectionStyle = UITableViewCellSelectionStyleBlue;

    switch (specifier.cellType) {

        case PSGroupCell:
            /* Groups are handled as section headers, not cells */
            break;

        case PSLinkCell:
            /* Navigation: show disclosure chevron */
            self.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
            break;

        case PSToggleSwitchCell: {
            /* On/Off switch in the accessory area */
            _switchControl = [[UISwitch alloc] initWithFrame:CGRectZero];
            [_switchControl addTarget:self
                               action:@selector(_switchValueChanged:)
                     forControlEvents:UIControlEventValueChanged];
            self.accessoryView = _switchControl;
            self.selectionStyle = UITableViewCellSelectionStyleNone;
            [self refreshValue];
            break;
        }

        case PSSliderCell: {
            /* Full-width slider below the label */
            _sliderControl = [[UISlider alloc] initWithFrame:
                              CGRectMake(20.0f, 22.0f, 280.0f, 24.0f)];
            _sliderControl.minimumValue = 0.0f;
            _sliderControl.maximumValue = 1.0f;
            [_sliderControl addTarget:self
                               action:@selector(_sliderValueChanged:)
                     forControlEvents:UIControlEventValueChanged];
            [self.contentView addSubview:_sliderControl];
            self.selectionStyle = UITableViewCellSelectionStyleNone;
            [self refreshValue];
            break;
        }

        case PSTitleValueCell:
            /* Value shown in the detailTextLabel */
            self.accessoryType  = UITableViewCellAccessoryNone;
            self.selectionStyle = UITableViewCellSelectionStyleNone;
            [self refreshValue];
            break;

        case PSStaticTextCell:
            self.selectionStyle = UITableViewCellSelectionStyleNone;
            break;

        case PSButtonCell:
            /* Blue centered text like a button */
            self.textLabel.textAlignment = UITextAlignmentCenter;
            self.textLabel.textColor = [UIColor colorWithRed:0.22f green:0.33f blue:0.53f alpha:1.0f];
            break;

        default:
            break;
    }
}

- (void)refreshValue
{
    if (!_specifier || !_specifier.target) return;

    SEL getter = _specifier.getter;
    if (!getter) {
        /* No getter — use defaultValue */
        id val = _specifier.defaultValue;
        [self _applyValue:val];
        return;
    }

    /* Invoke the getter on the target */
    id val = nil;
    if ([_specifier.target respondsToSelector:getter]) {
        /* iPhoneOS 2.0: use NSInvocation for safety (avoids objc_msgSend casting issues) */
        NSMethodSignature *sig = [[_specifier.target class]
                                  instanceMethodSignatureForSelector:getter];
        if (sig) {
            NSInvocation *inv = [NSInvocation invocationWithMethodSignature:sig];
            [inv setTarget:_specifier.target];
            [inv setSelector:getter];
            [inv invoke];

            /* Retrieve return value */
            const char *retType = [sig methodReturnType];
            if (strcmp(retType, @encode(BOOL)) == 0) {
                BOOL b = NO;
                [inv getReturnValue:&b];
                val = [NSNumber numberWithBool:b];
            } else if (strcmp(retType, @encode(float)) == 0) {
                float f = 0.0f;
                [inv getReturnValue:&f];
                val = [NSNumber numberWithFloat:f];
            } else {
                /* Assume object return */
                [inv getReturnValue:&val];
            }
        }
    }

    [self _applyValue:val];
}

- (void)_applyValue:(id)val
{
    switch (_specifier.cellType) {
        case PSToggleSwitchCell:
            if ([val isKindOfClass:[NSNumber class]]) {
                _switchControl.on = [val boolValue];
            }
            break;
        case PSSliderCell:
            if ([val isKindOfClass:[NSNumber class]]) {
                _sliderControl.value = [val floatValue];
            }
            break;
        case PSTitleValueCell:
            self.detailTextLabel.text = [val description];
            break;
        default:
            break;
    }
}

#pragma mark - Control callbacks

- (void)_switchValueChanged:(UISwitch *)sender
{
    if (!_specifier || !_specifier.target || !_specifier.setter) return;

    NSNumber *val = [NSNumber numberWithBool:sender.on];

    /* Invoke setter(specifier, newValue) — matches Apple's private calling convention */
    SEL setter = _specifier.setter;
    if ([_specifier.target respondsToSelector:setter]) {
        NSMethodSignature *sig = [[_specifier.target class]
                                  instanceMethodSignatureForSelector:setter];
        NSInvocation *inv = [NSInvocation invocationWithMethodSignature:sig];
        [inv setTarget:_specifier.target];
        [inv setSelector:setter];
        [inv setArgument:&val atIndex:2];
        [inv invoke];
    }
}

- (void)_sliderValueChanged:(UISlider *)sender
{
    if (!_specifier || !_specifier.target || !_specifier.setter) return;

    NSNumber *val = [NSNumber numberWithFloat:sender.value];

    SEL setter = _specifier.setter;
    if ([_specifier.target respondsToSelector:setter]) {
        NSMethodSignature *sig = [[_specifier.target class]
                                  instanceMethodSignatureForSelector:setter];
        NSInvocation *inv = [NSInvocation invocationWithMethodSignature:sig];
        [inv setTarget:_specifier.target];
        [inv setSelector:setter];
        [inv setArgument:&val atIndex:2];
        [inv invoke];
    }
}

#pragma mark - Dealloc

- (void)dealloc
{
    [_specifier     release];
    [_switchControl release];
    [_sliderControl release];
    [_valueLabel    release];
    [super dealloc];
}

@end
