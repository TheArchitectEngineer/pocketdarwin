# pdUIKit - PocketDarwin UIKit Framework

pdUIKit is a touch-oriented adaptation of GNUStep AppKit/Cocoa that identifies itself as UIKit for mobile devices (phones, tablets, etc.). It provides the familiar UIKit API while leveraging GNUStep's underlying implementation.

## Overview

pdUIKit bridges the gap between desktop GNUStep/AppKit and mobile UIKit, allowing developers to write UIKit-style applications that run on PocketDarwin systems with touch input support.

## Key Features

### **Touch-Oriented Design**
- **Touch Events**: Full UITouch and UIEvent support adapted from NSEvent
- **Gesture Recognition**: Complete gesture recognizer system
- **Multi-touch**: Support for multiple simultaneous touch points
- **Touch Adaptation**: Seamless conversion from mouse events to touch events

### **UIKit API Compatibility**
- **Familiar Classes**: UIView, UIViewController, UIApplication, etc.
- **Standard Patterns**: View controllers, navigation, tab bars
- **Touch Methods**: Standard UIKit touch event handling
- **Animation Support**: UIView animation methods

### **GNUStep Integration**
- **Underlying AppKit**: Uses NSView, NSWindow, NSApplication
- **Desktop Compatibility**: Works with mouse input as touch simulation
- **Performance**: Leverages mature GNUStep rendering system
- **Platform Bridge**: Automatic conversion between coordinate systems

## Architecture

```
pdUIKit Framework
├── Core Classes
│   ├── UIApplication (NSApplication)
│   ├── UIView (NSView)
│   └── UIViewController (NSViewController)
├── Touch System
│   ├── UITouch (NSEvent adaptation)
│   ├── UIEvent (NSEvent wrapper)
│   └── UIGestureRecognizer (gesture handling)
├── Geometry & Utilities
│   ├── CGPoint (NSPoint compatibility)
│   ├── CGSize (NSSize compatibility)
│   ├── CGRect (NSRect compatibility)
│   └── UIColor (NSColor adaptation)
└── UI Controls
    ├── UIButton (NSButton adaptation)
    ├── UILabel (NSTextField adaptation)
    └── UITextField (NSTextField adaptation)
```

## Usage Example

```objc
#import <pdUIKit/UIKit.h>

@interface MyViewController : UIViewController
@end

@implementation MyViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    // Create a button using UIKit API
    UIButton *button = [UIButton buttonWithType:UIButtonTypeSystem];
    [button setTitle:@"Tap Me!" forState:UIControlStateNormal];
    button.frame = CGRectMake(100, 100, 200, 50);
    [button addTarget:self action:@selector(buttonTapped:) forControlEvents:UIControlEventTouchUpInside];
    
    [self.view addSubview:button];
    
    // Set background color using UIKit API
    self.view.backgroundColor = [UIColor systemBlueColor];
}

- (void)buttonTapped:(UIButton *)sender {
    UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"Button Tapped"
                                                                   message:@"pdUIKit is working!"
                                                            preferredStyle:UIAlertControllerStyleAlert];
    
    [alert addAction:[UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:nil]];
    [self presentViewController:alert animated:YES completion:nil];
}

@end

int main(int argc, char * argv[]) {
    @autoreleasepool {
        // Create application using UIKit API
        UIApplication *app = [UIApplication sharedApplication];
        
        MyViewController *rootVC = [[MyViewController alloc] init];
        UIWindow *window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
        window.rootViewController = rootVC;
        [window makeKeyAndVisible];
        
        app.window = window;
        
        // Run the application
        [app run];
    }
    return 0;
}
```

## Touch Adaptation

pdUIKit automatically converts GNUStep mouse events to UIKit touch events:

```objc
// Mouse down -> Touch began
- (void)mouseDown:(NSEvent *)event {
    UITouch *touch = [UITouch touchWithNSEvent:event];
    UIEvent *uiEvent = [UIEvent eventWithNSEvent:event];
    [self touchesBegan:[NSSet setWithObject:touch] withEvent:uiEvent];
}

// Mouse drag -> Touch moved
- (void)mouseDragged:(NSEvent *)event {
    UITouch *touch = [UITouch touchWithNSEvent:event];
    UIEvent *uiEvent = [UIEvent eventWithNSEvent:event];
    [self touchesMoved:[NSSet setWithObject:touch] withEvent:uiEvent];
}

// Mouse up -> Touch ended
- (void)mouseUp:(NSEvent *)event {
    UITouch *touch = [UITouch touchWithNSEvent:event];
    UIEvent *uiEvent = [UIEvent eventWithNSEvent:event];
    [self touchesEnded:[NSSet setWithObject:touch] withEvent:uiEvent];
}
```

## Platform Identification

pdUIKit identifies itself as UIKit to enable conditional compilation:

```objc
#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
    // UIKit code path
#else
    // Desktop fallback
#endif

// pdUIKit specific detection
#ifdef PD_UKIT
    // pdUIKit-specific code
#endif
```

## Building

```bash
# Build pdUIKit framework
cd /System/Frameworks/pdUIKit
xcodebuild -project pdUIKit.xcodeproj -scheme pdUIKit -configuration Release

# Build application with pdUIKit
gcc -framework pdUIKit -framework Foundation -framework AppKit myapp.m -o myapp
```

## Compatibility

### **Supported Platforms**
- PocketDarwin (primary target)
- macOS with GNUStep (development)
- Linux with GNUStep (experimental)

### **Input Methods**
- Touch screens (native)
- Mouse input (adapted)
- Trackpad input (adapted)
- Stylus input (adapted)

### **Rendering**
- Core Graphics (via GNUStep)
- OpenGL ES (future)
- Metal (future)

## Development Status

### **Completed**
- ✅ Core geometry classes (CGPoint, CGSize, CGRect)
- ✅ UIColor system
- ✅ UIView basic functionality
- ✅ Touch event adaptation
- ✅ UIApplication lifecycle

### **In Progress**
- 🔄 UIViewController navigation
- 🔄 UI controls (UIButton, UILabel)
- 🔄 Gesture recognizers
- 🔄 Animation system

### **Planned**
- 📋 Auto Layout constraints
- 📋 Collection views
- 📋 Navigation controllers
- 📋 Tab bar controllers
- 📋 Table views

## Contributing

pdUIKit is open source and welcomes contributions. The framework is designed to be a drop-in replacement for UIKit on PocketDarwin systems.

## License

pdUIKit follows the same licensing as PocketDarwin project.
