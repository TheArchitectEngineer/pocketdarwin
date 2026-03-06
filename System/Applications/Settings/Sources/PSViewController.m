/*
 * PSViewController.m
 * Settings.app - PocketDarwin
 *
 * iPhoneOS 2.0 — Manual Retain/Release
 */

#import <UIKit/UIKit.h>
#import "PSViewController.h"
#import "PSTableCell.h"
#import "PSSpecifier.h"

static NSString * const kPSDefaultsAppID = @"com.pocketdarwin.settings";

@implementation PSViewController

#pragma mark - View lifecycle

- (void)loadView
{
    /* iPhoneOS 2.0: create table view manually — no UITableViewController */
    CGRect screenBounds = [[UIScreen mainScreen] bounds];
    CGRect tableFrame   = CGRectMake(0, 0,
                                     CGRectGetWidth(screenBounds),
                                     CGRectGetHeight(screenBounds) - 44.0f); /* minus nav bar */

    _tableView = [[UITableView alloc] initWithFrame:tableFrame
                                              style:UITableViewStyleGrouped];
    _tableView.dataSource      = self;
    _tableView.delegate        = self;
    _tableView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;

    self.view = _tableView;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    [self reloadSpecifiers];
}

#pragma mark - Specifiers

- (NSArray *)specifiers
{
    /* Subclasses must override this method */
    return [NSArray array];
}

- (void)reloadSpecifiers
{
    [_specifiers     release];
    [_sections       release];
    [_sectionTitles  release];
    [_sectionFooters release];

    _specifiers     = [[NSMutableArray alloc] init];
    _sections       = [[NSMutableArray alloc] init];
    _sectionTitles  = [[NSMutableArray alloc] init];
    _sectionFooters = [[NSMutableArray alloc] init];

    [_specifiers addObjectsFromArray:[self specifiers]];
    [self _buildSectionModel];
    [_tableView reloadData];
}

- (void)_buildSectionModel
{
    NSMutableArray *currentSection = nil;

    for (PSSpecifier *spec in _specifiers) {
        if (spec.cellType == PSGroupCell) {
            /* Start a new section */
            currentSection = [NSMutableArray array];
            [_sections addObject:currentSection];

            NSString *title = spec.name ? spec.name : (NSString *)[NSNull null];
            [_sectionTitles addObject:title];

            NSString *footer = spec.footerText ? spec.footerText : (NSString *)[NSNull null];
            [_sectionFooters addObject:footer];
        } else {
            /* Ensure there's at least one section */
            if (!currentSection) {
                currentSection = [NSMutableArray array];
                [_sections addObject:currentSection];
                [_sectionTitles addObject:[NSNull null]];
                [_sectionFooters addObject:[NSNull null]];
            }
            [currentSection addObject:spec];
        }
    }
}

- (PSSpecifier *)specifierForKey:(NSString *)key
{
    for (PSSpecifier *spec in _specifiers) {
        if ([spec.key isEqualToString:key]) return spec;
    }
    return nil;
}

#pragma mark - Preference storage

- (id)readPreferenceValue:(PSSpecifier *)specifier
{
    if (!specifier.key) return specifier.defaultValue;

    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    id value = [defaults objectForKey:specifier.key];
    return value ? value : specifier.defaultValue;
}

- (void)setPreferenceValue:(id)value specifier:(PSSpecifier *)specifier
{
    if (!specifier.key) return;

    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    if (value) {
        [defaults setObject:value forKey:specifier.key];
    } else {
        [defaults removeObjectForKey:specifier.key];
    }
    [defaults synchronize];

    /* Notify other interested parties via Darwin notification */
    NSString *notifName = [NSString stringWithFormat:@"%@.%@.changed",
                           kPSDefaultsAppID, specifier.key];
    [[NSNotificationCenter defaultCenter] postNotificationName:notifName
                                                        object:self];
}

- (NSNumber *)getBoolForSpecifier:(PSSpecifier *)specifier
{
    id val = [self readPreferenceValue:specifier];
    if ([val isKindOfClass:[NSNumber class]]) return val;
    return [NSNumber numberWithBool:NO];
}

- (void)setBoolForSpecifier:(PSSpecifier *)specifier value:(NSNumber *)value
{
    [self setPreferenceValue:value specifier:specifier];
}

#pragma mark - UITableViewDataSource

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return (NSInteger)[_sections count];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return (NSInteger)[[_sections objectAtIndex:section] count];
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    id title = [_sectionTitles objectAtIndex:section];
    if ([title isKindOfClass:[NSNull class]]) return nil;
    NSString *s = (NSString *)title;
    return ([s length] > 0) ? s : nil;
}

- (NSString *)tableView:(UITableView *)tableView titleForFooterInSection:(NSInteger)section
{
    id footer = [_sectionFooters objectAtIndex:section];
    if ([footer isKindOfClass:[NSNull class]]) return nil;
    NSString *s = (NSString *)footer;
    return ([s length] > 0) ? s : nil;
}

- (UITableViewCell *)tableView:(UITableView *)tableView
         cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    PSSpecifier *spec = [[_sections objectAtIndex:indexPath.section]
                         objectAtIndex:indexPath.row];

    NSString *reuseID = [NSString stringWithFormat:@"PSCell-%d", (int)spec.cellType];
    PSTableCell *cell = (PSTableCell *)[tableView dequeueReusableCellWithIdentifier:reuseID];

    if (!cell) {
        cell = [[[PSTableCell alloc] initWithSpecifier:spec
                                       reuseIdentifier:reuseID] autorelease];
    } else {
        cell.specifier = spec;
        [cell refreshValue];
    }

    return cell;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    PSSpecifier *spec = [[_sections objectAtIndex:indexPath.section]
                         objectAtIndex:indexPath.row];
    if (spec.cellType == PSSliderCell) return 54.0f;
    return 44.0f;  /* standard iPhoneOS cell height */
}

#pragma mark - UITableViewDelegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    [tableView deselectRowAtIndexPath:indexPath animated:YES];

    PSSpecifier *spec = [[_sections objectAtIndex:indexPath.section]
                         objectAtIndex:indexPath.row];

    switch (spec.cellType) {

        case PSLinkCell: {
            /* Instantiate the detail controller and push it */
            NSString *className = spec.detailControllerClass;
            if (!className) break;

            Class cls = NSClassFromString(className);
            if (!cls) {
                NSLog(@"PSViewController: Unknown controller class %@", className);
                break;
            }

            PSViewController *detail = [[[cls alloc] init] autorelease];
            detail.title = spec.name;
            [self.navigationController pushViewController:detail animated:YES];
            break;
        }

        case PSButtonCell: {
            /* Invoke setter as the action */
            SEL action = spec.setter;
            if (action && [spec.target respondsToSelector:action]) {
                [spec.target performSelector:action withObject:spec];
            }
            break;
        }

        default:
            break;
    }
}

#pragma mark - Dealloc

- (void)dealloc
{
    [_tableView      release];
    [_specifiers     release];
    [_sections       release];
    [_sectionTitles  release];
    [_sectionFooters release];
    [super dealloc];
}

@end
