//
//  ViewController.m
//  Install PocketDarwin
//
//  Created by Oaky on 06/02/26.
//

#import "ViewController.h"

@interface ViewController ()

@property (nonatomic, strong) NSTextField *kernelPathField;
@property (nonatomic, strong) NSTextField *ramdiskPathField;
@property (nonatomic, strong) NSTextField *dtbPathField;
@property (nonatomic, strong) NSTextField *bootImgOutputField;
@property (nonatomic, strong) NSTextField *cmdlineField;

@property (nonatomic, strong) NSTextField *configOutputField;
@property (nonatomic, strong) NSTextField *sdRootField;
@property (nonatomic, strong) NSTextField *mobileRootField;
@property (nonatomic, strong) NSButton *allowInternalCheckbox;

@property (nonatomic, strong) NSTextView *logTextView;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [self setupUI];
}

- (void)setupUI {
    self.view.wantsLayer = YES;

    NSStackView *root = [[NSStackView alloc] init];
    root.orientation = NSUserInterfaceLayoutOrientationVertical;
    root.alignment = NSLayoutAttributeLeading;
    root.spacing = 12.0;
    root.translatesAutoresizingMaskIntoConstraints = NO;

    NSTextField *title = [self labelWithString:@"Install PocketDarwin" fontSize:20 weight:NSFontWeightSemibold];
    [root addArrangedSubview:title];

    [root addArrangedSubview:[self sectionLabel:@"Boot Image Build"]];
    [root addArrangedSubview:[self pathRowWithLabel:@"Kernel Image" field:&_kernelPathField action:@selector(browseKernel:)]];
    [root addArrangedSubview:[self pathRowWithLabel:@"Ramdisk Image" field:&_ramdiskPathField action:@selector(browseRamdisk:)]];
    [root addArrangedSubview:[self pathRowWithLabel:@"DTB (Optional)" field:&_dtbPathField action:@selector(browseDTB:)]];
    [root addArrangedSubview:[self pathRowWithLabel:@"boot.img Output" field:&_bootImgOutputField action:@selector(browseBootImgOutput:)]];

    _cmdlineField = [self textFieldWithPlaceholder:@"Kernel cmdline (optional)"];
    [root addArrangedSubview:[self labeledRow:@"Cmdline" view:_cmdlineField]];

    NSStackView *buildButtons = [self horizontalButtons:@[
        [self button:@"Build boot.img" action:@selector(buildBootImg:)],
    ]];
    [root addArrangedSubview:buildButtons];

    [root addArrangedSubview:[self sectionLabel:@"Config Generation"]];
    [root addArrangedSubview:[self pathRowWithLabel:@"OCMobileConfig.json Output" field:&_configOutputField action:@selector(browseConfigOutput:)]];

    _sdRootField = [self textFieldWithPlaceholder:@"/sdcard"];
    _mobileRootField = [self textFieldWithPlaceholder:@"/System/Mobile Enviroment"];

    [root addArrangedSubview:[self labeledRow:@"External Storage Root" view:_sdRootField]];
    [root addArrangedSubview:[self labeledRow:@"Mobile Environment Root" view:_mobileRootField]];

    _allowInternalCheckbox = [NSButton checkboxWithTitle:@"Allow Internal Storage" target:nil action:nil];
    _allowInternalCheckbox.state = NSControlStateValueOff;
    [root addArrangedSubview:_allowInternalCheckbox];

    NSStackView *configButtons = [self horizontalButtons:@[
        [self button:@"Write Config" action:@selector(writeConfig:)],
    ]];
    [root addArrangedSubview:configButtons];

    [root addArrangedSubview:[self sectionLabel:@"ADB / Fastboot"]];
    NSStackView *adbButtons = [self horizontalButtons:@[
        [self button:@"ADB Devices" action:@selector(adbDevices:)],
        [self button:@"ADB Reboot Bootloader" action:@selector(adbRebootBootloader:)],
        [self button:@"Flash boot.img" action:@selector(fastbootFlash:)],
        [self button:@"Fastboot Reboot" action:@selector(fastbootReboot:)],
    ]];
    [root addArrangedSubview:adbButtons];

    [root addArrangedSubview:[self sectionLabel:@"Log"]];
    NSScrollView *logScroll = [[NSScrollView alloc] init];
    logScroll.hasVerticalScroller = YES;
    logScroll.translatesAutoresizingMaskIntoConstraints = NO;

    _logTextView = [[NSTextView alloc] init];
    _logTextView.editable = NO;
    _logTextView.font = [NSFont userFixedPitchFontOfSize:12];
    logScroll.documentView = _logTextView;

    [root addArrangedSubview:logScroll];

    [self.view addSubview:root];

    [NSLayoutConstraint activateConstraints:@[
        [root.leadingAnchor constraintEqualToAnchor:self.view.leadingAnchor constant:20],
        [root.trailingAnchor constraintEqualToAnchor:self.view.trailingAnchor constant:-20],
        [root.topAnchor constraintEqualToAnchor:self.view.topAnchor constant:20],
        [root.bottomAnchor constraintLessThanOrEqualToAnchor:self.view.bottomAnchor constant:-20],
        [logScroll.heightAnchor constraintEqualToConstant:200],
        [_cmdlineField.widthAnchor constraintEqualToConstant:420],
        [_sdRootField.widthAnchor constraintEqualToConstant:420],
        [_mobileRootField.widthAnchor constraintEqualToConstant:420],
    ]];

    _sdRootField.stringValue = @"/sdcard";
    _mobileRootField.stringValue = @"/System/Mobile Enviroment";
}

#pragma mark - UI Helpers

- (NSTextField *)labelWithString:(NSString *)string fontSize:(CGFloat)size weight:(NSFontWeight)weight {
    NSTextField *label = [NSTextField labelWithString:string];
    label.font = [NSFont systemFontOfSize:size weight:weight];
    return label;
}

- (NSTextField *)sectionLabel:(NSString *)title {
    return [self labelWithString:title fontSize:14 weight:NSFontWeightSemibold];
}

- (NSTextField *)textFieldWithPlaceholder:(NSString *)placeholder {
    NSTextField *field = [[NSTextField alloc] init];
    field.placeholderString = placeholder;
    field.translatesAutoresizingMaskIntoConstraints = NO;
    return field;
}

- (NSStackView *)labeledRow:(NSString *)label view:(NSView *)view {
    NSStackView *row = [[NSStackView alloc] init];
    row.orientation = NSUserInterfaceLayoutOrientationHorizontal;
    row.spacing = 8.0;

    NSTextField *labelField = [NSTextField labelWithString:label];
    labelField.alignment = NSTextAlignmentRight;
    [labelField.widthAnchor constraintEqualToConstant:180].active = YES;

    [row addArrangedSubview:labelField];
    [row addArrangedSubview:view];
    return row;
}

- (NSStackView *)pathRowWithLabel:(NSString *)label field:(NSTextField * __strong *)field action:(SEL)action {
    NSTextField *textField = [self textFieldWithPlaceholder:@"Select a file"]; 
    NSButton *button = [self button:@"Browse" action:action];
    NSStackView *row = [self labeledRow:label view:textField];
    [row addArrangedSubview:button];
    if (field) {
        *field = textField;
    }
    return row;
}

- (NSButton *)button:(NSString *)title action:(SEL)action {
    NSButton *button = [NSButton buttonWithTitle:title target:self action:action];
    button.bezelStyle = NSBezelStyleRounded;
    return button;
}

- (NSStackView *)horizontalButtons:(NSArray<NSButton *> *)buttons {
    NSStackView *row = [[NSStackView alloc] init];
    row.orientation = NSUserInterfaceLayoutOrientationHorizontal;
    row.spacing = 8.0;
    for (NSButton *button in buttons) {
        [row addArrangedSubview:button];
    }
    return row;
}

#pragma mark - Actions

- (void)browseKernel:(id)sender {
    [self openFilePanelForField:self.kernelPathField];
}

- (void)browseRamdisk:(id)sender {
    [self openFilePanelForField:self.ramdiskPathField];
}

- (void)browseDTB:(id)sender {
    [self openFilePanelForField:self.dtbPathField];
}

- (void)browseBootImgOutput:(id)sender {
    [self saveFilePanelForField:self.bootImgOutputField defaultName:@"boot.img"]; 
}

- (void)browseConfigOutput:(id)sender {
    [self saveFilePanelForField:self.configOutputField defaultName:@"OCMobileConfig.json"]; 
}

- (void)buildBootImg:(id)sender {
    NSString *kernel = self.kernelPathField.stringValue;
    NSString *ramdisk = self.ramdiskPathField.stringValue;
    NSString *dtb = self.dtbPathField.stringValue;
    NSString *output = self.bootImgOutputField.stringValue;
    NSString *cmdline = self.cmdlineField.stringValue;

    if (kernel.length == 0 || ramdisk.length == 0 || output.length == 0) {
        [self log:@"Missing kernel, ramdisk, or output path.\n"];
        return;
    }

    NSMutableArray *args = [NSMutableArray arrayWithArray:@[@"mkbootimg",
                                                             @"--kernel", kernel,
                                                             @"--ramdisk", ramdisk,
                                                             @"--output", output]];
    if (dtb.length > 0) {
        [args addObject:@"--dtb"]; 
        [args addObject:dtb];
    }
    if (cmdline.length > 0) {
        [args addObject:@"--cmdline"]; 
        [args addObject:cmdline];
    }

    [self runCommand:args label:@"mkbootimg"]; 
}

- (void)writeConfig:(id)sender {
    NSString *output = self.configOutputField.stringValue;
    if (output.length == 0) {
        [self log:@"Please choose an output path for OCMobileConfig.json.\n"];
        return;
    }

    NSString *kernelPath = self.kernelPathField.stringValue;
    if (kernelPath.length == 0) {
        [self log:@"Kernel path is empty; config will still be written.\n"];
    }

    NSDictionary *platform = @{
        @"ExternalStorageRoot": self.sdRootField.stringValue.length ? self.sdRootField.stringValue : @"/sdcard",
        @"MobileEnvironmentRoot": self.mobileRootField.stringValue.length ? self.mobileRootField.stringValue : @"/System/Mobile Enviroment",
        @"InternalStorageAllowed": @(self.allowInternalCheckbox.state == NSControlStateValueOn)
    };

    NSDictionary *config = @{
        @"darwinKernelPath": kernelPath.length ? kernelPath : @"",
        @"Platform": platform
    };

    NSError *error = nil;
    NSData *data = [NSJSONSerialization dataWithJSONObject:config options:NSJSONWritingPrettyPrinted error:&error];
    if (!data) {
        [self log:[NSString stringWithFormat:@"Failed to serialize JSON: %@\n", error.localizedDescription]];
        return;
    }

    if (![data writeToFile:output options:NSDataWritingAtomic error:&error]) {
        [self log:[NSString stringWithFormat:@"Failed to write config: %@\n", error.localizedDescription]];
        return;
    }

    [self log:[NSString stringWithFormat:@"Wrote config: %@\n", output]];
}

- (void)adbDevices:(id)sender {
    [self runCommand:@[@"adb", @"devices", @"-l"] label:@"adb devices"]; 
}

- (void)adbRebootBootloader:(id)sender {
    [self runCommand:@[@"adb", @"reboot", @"bootloader"] label:@"adb reboot bootloader"]; 
}

- (void)fastbootFlash:(id)sender {
    NSString *bootImg = self.bootImgOutputField.stringValue;
    if (bootImg.length == 0) {
        [self log:@"Please set boot.img output path before flashing.\n"];
        return;
    }
    [self runCommand:@[@"fastboot", @"flash", @"boot", bootImg] label:@"fastboot flash boot"]; 
}

- (void)fastbootReboot:(id)sender {
    [self runCommand:@[@"fastboot", @"reboot"] label:@"fastboot reboot"]; 
}

#pragma mark - Helpers

- (void)openFilePanelForField:(NSTextField *)field {
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    panel.canChooseDirectories = NO;
    panel.canChooseFiles = YES;
    panel.allowsMultipleSelection = NO;
    [panel beginWithCompletionHandler:^(NSModalResponse result) {
        if (result == NSModalResponseOK) {
            field.stringValue = panel.URL.path ?: @"";
        }
    }];
}

- (void)saveFilePanelForField:(NSTextField *)field defaultName:(NSString *)name {
    NSSavePanel *panel = [NSSavePanel savePanel];
    panel.nameFieldStringValue = name;
    [panel beginWithCompletionHandler:^(NSModalResponse result) {
        if (result == NSModalResponseOK) {
            field.stringValue = panel.URL.path ?: @"";
        }
    }];
}

- (void)runCommand:(NSArray<NSString *> *)args label:(NSString *)label {
    if (args.count == 0) {
        return;
    }

    [self log:[NSString stringWithFormat:@"\n$ %@\n", [args componentsJoinedByString:@" "]]];

    NSTask *task = [[NSTask alloc] init];
    task.launchPath = @"/usr/bin/env";
    task.arguments = args;

    NSPipe *pipe = [NSPipe pipe];
    task.standardOutput = pipe;
    task.standardError = pipe;

    NSFileHandle *handle = [pipe fileHandleForReading];
    handle.readabilityHandler = ^(NSFileHandle * _Nonnull fh) {
        NSData *data = fh.availableData;
        if (data.length == 0) {
            return;
        }
        NSString *output = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
        if (!output) {
            return;
        }
        dispatch_async(dispatch_get_main_queue(), ^{
            [self log:output];
        });
    };

    task.terminationHandler = ^(NSTask * _Nonnull t) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [self log:[NSString stringWithFormat:@"\n[%@] exited with code %d\n", label, t.terminationStatus]];
        });
    };

    @try {
        [task launch];
    } @catch (NSException *exception) {
        [self log:[NSString stringWithFormat:@"Failed to run %@: %@\n", label, exception.reason]];
    }
}

- (void)log:(NSString *)text {
    if (!text.length) {
        return;
    }
    NSTextStorage *storage = self.logTextView.textStorage;
    [storage appendAttributedString:[[NSAttributedString alloc] initWithString:text]];
    [self.logTextView scrollRangeToVisible:NSMakeRange(storage.length, 0)];
}

@end
