#import "MTTerminalViewController.h"

@interface MTTerminalViewController ()
- (void)setupViews;
- (void)appendOutput:(NSString *)output;
- (void)sendCtrlC;
- (void)clearTerminal;
- (void)dismissKeyboard;
@end

@implementation MTTerminalViewController

- (void)loadView
{
    CGRect frame = [[UIScreen mainScreen] applicationFrame];
    UIView *rootView = [[UIView alloc] initWithFrame:frame];
    rootView.backgroundColor = [UIColor blackColor];
    self.view = rootView;
    [rootView release];

    [self setupViews];
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];

    if (_session == nil) {
        _session = [[MTTerminalSession alloc] initWithDelegate:self];
        if ([_session start]) {
            [self appendOutput:@"\nPocketDarwin MobileTerminal\n"]; 
            [self appendOutput:@"---------------------------\n"]; 
        } else {
            [self appendOutput:@"Failed to start shell session.\n"]; 
        }
    }

    [_inputField becomeFirstResponder];
    (void)animated;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return interfaceOrientation == UIInterfaceOrientationPortrait;
}

- (void)setupViews
{
    CGRect bounds = self.view.bounds;
    int screenWidth = (int)bounds.size.width;
    int screenHeight = (int)bounds.size.height;
    int toolbarHeight = 36;
    int inputHeight = 32;

    CGRect outputFrame = CGRectMake(0, 0, screenWidth, screenHeight - inputHeight - toolbarHeight);
    _terminalOutputView = [[UITextView alloc] initWithFrame:outputFrame];
    _terminalOutputView.backgroundColor = [UIColor blackColor];
    _terminalOutputView.textColor = [UIColor colorWithRed:0.39f green:0.97f blue:0.32f alpha:1.0f];
    _terminalOutputView.editable = NO;
    _terminalOutputView.font = [UIFont fontWithName:@"Courier" size:14.0f];
    _terminalOutputView.text = @"";
    [self.view addSubview:_terminalOutputView];

    CGRect inputFrame = CGRectMake(0, screenHeight - inputHeight, screenWidth, inputHeight);
    _inputField = [[UITextField alloc] initWithFrame:inputFrame];
    _inputField.backgroundColor = [UIColor colorWithWhite:0.08f alpha:1.0f];
    _inputField.textColor = [UIColor whiteColor];
    _inputField.font = [UIFont fontWithName:@"Courier" size:14.0f];
    _inputField.autocorrectionType = UITextAutocorrectionTypeNo;
    _inputField.autocapitalizationType = UITextAutocapitalizationTypeNone;
    _inputField.keyboardType = UIKeyboardTypeASCIICapable;
    _inputField.returnKeyType = UIReturnKeyGo;
    _inputField.delegate = self;
    [self.view addSubview:_inputField];

    CGRect toolbarFrame = CGRectMake(0, screenHeight - inputHeight - toolbarHeight, screenWidth, toolbarHeight);
    UIView *toolbar = [[UIView alloc] initWithFrame:toolbarFrame];
    toolbar.backgroundColor = [UIColor colorWithWhite:0.15f alpha:1.0f];

    _ctrlCButton = [UIButton buttonWithType:UIButtonTypeRoundedRect];
    _ctrlCButton.frame = CGRectMake(8.0f, 4.0f, 80.0f, 28.0f);
    [_ctrlCButton setTitle:@"Ctrl-C" forState:UIControlStateNormal];
    [_ctrlCButton addTarget:self action:@selector(sendCtrlC) forControlEvents:UIControlEventTouchUpInside];
    [toolbar addSubview:_ctrlCButton];

    _clearButton = [UIButton buttonWithType:UIButtonTypeRoundedRect];
    _clearButton.frame = CGRectMake(96.0f, 4.0f, 80.0f, 28.0f);
    [_clearButton setTitle:@"Clear" forState:UIControlStateNormal];
    [_clearButton addTarget:self action:@selector(clearTerminal) forControlEvents:UIControlEventTouchUpInside];
    [toolbar addSubview:_clearButton];

    _dismissKeyboardButton = [UIButton buttonWithType:UIButtonTypeRoundedRect];
    _dismissKeyboardButton.frame = CGRectMake(screenWidth - 104, 4, 96, 28);
    [_dismissKeyboardButton setTitle:@"Keyboard" forState:UIControlStateNormal];
    [_dismissKeyboardButton addTarget:self action:@selector(dismissKeyboard) forControlEvents:UIControlEventTouchUpInside];
    [toolbar addSubview:_dismissKeyboardButton];

    [self.view addSubview:toolbar];
    [toolbar release];
}

- (void)appendOutput:(NSString *)output
{
    if (output == nil || [output length] == 0) {
        return;
    }

    NSString *existing = _terminalOutputView.text;
    NSString *combined = [existing stringByAppendingString:output];
    _terminalOutputView.text = combined;

    NSRange tailRange = NSMakeRange([combined length], 0);
    [_terminalOutputView scrollRangeToVisible:tailRange];
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
    NSString *command = textField.text;
    if (command == nil) {
        command = @"";
    }

    [self appendOutput:[NSString stringWithFormat:@"$ %@\n", command]];

    NSString *payload = [command stringByAppendingString:@"\n"];
    [_session sendCommand:payload];

    textField.text = @"";
    return YES;
}

- (void)sendCtrlC
{
    NSString *ctrlC = [NSString stringWithFormat:@"%c", 3];
    [_session sendCommand:ctrlC];
    [self appendOutput:@"^C\n"];
}

- (void)clearTerminal
{
    _terminalOutputView.text = @"";
}

- (void)dismissKeyboard
{
    if ([_inputField isFirstResponder]) {
        [_inputField resignFirstResponder];
    } else {
        [_inputField becomeFirstResponder];
    }
}

- (void)terminalSession:(MTTerminalSession *)session didReceiveOutput:(NSString *)output
{
    [self appendOutput:output];
    (void)session;
}

- (void)terminalSessionDidExit:(MTTerminalSession *)session
{
    [self appendOutput:@"\n[Shell exited]\n"];
    (void)session;
}

- (void)dealloc
{
    [_session stop];
    [_session release];
    [_terminalOutputView release];
    [_inputField release];
    [super dealloc];
}

@end
