#import <UIKit/UIKit.h>
#import "MTTerminalSession.h"

@interface MTTerminalViewController : UIViewController <UITextFieldDelegate, MTTerminalSessionDelegate> {
    UITextView *_terminalOutputView;
    UITextField *_inputField;
    UIButton *_ctrlCButton;
    UIButton *_clearButton;
    UIButton *_dismissKeyboardButton;
    MTTerminalSession *_session;
}
@end
