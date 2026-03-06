/*
 * PSTableCell.h
 * Settings.app - PocketDarwin
 *
 * Custom UITableViewCell subclass that renders the various
 * PSSpecifier cell types: toggle switch, disclosure arrow,
 * title/value, slider, etc.
 *
 * iPhoneOS 2.0 compatible.
 */

#import <UIKit/UIKit.h>
#import "PSSpecifier.h"

@interface PSTableCell : UITableViewCell {
    PSSpecifier     *_specifier;
    UISwitch        *_switchControl;
    UISlider        *_sliderControl;
    UILabel         *_valueLabel;
}

@property (nonatomic, retain) PSSpecifier *specifier;

- (id)initWithSpecifier:(PSSpecifier *)specifier reuseIdentifier:(NSString *)reuseID;

/* Update the cell's control to reflect the current value */
- (void)refreshValue;

/* UISwitch action */
- (void)_switchValueChanged:(UISwitch *)sender;

/* UISlider action */
- (void)_sliderValueChanged:(UISlider *)sender;

@end
