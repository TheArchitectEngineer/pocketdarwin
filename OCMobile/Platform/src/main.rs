use cpython::*;
use mach_o::macho;
use ratatui::backend::CrosstermBackend;
use ratatui::layout::{Constraint, Direction, Layout};
use ratatui::style::{Modifier, Style};
use ratatui::text::{Line, Span, Text};
use ratatui::widgets::{Block, Borders, List, ListItem, Paragraph, Scrollbar, ScrollbarOrientation};
use ratatui::Terminal;
use std::io::{stdout, Read};
use std::fs::File;
use std::panic;
use crossterm::event::{DisableMouseCapture, EnableMouseCapture, Event, KeyCode};
use crossterm::terminal::{disable_raw_mode, enable_raw_mode, EnterAlternateScreen, LeaveAlternateScreen};

struct App {
    python_vm: Python<'_>,
    macho_data: Option<macho::MachO>,
    file_path: Option<String>,
    terminal_content: Text<'static>,
    scroll_offset: u16,
}

impl App {
    fn new() -> Result<Self, Box<dyn std::error::Error>> {
        let python_vm = Python::acquire_gil();
        Ok(App {
            python_vm,
            macho_data: None,
            file_path: None,
            terminal_content: Text::from(""),
            scroll_offset: 0,
        })
    }

    fn load_file(&mut self, file_path: &str) -> Result<(), Box<dyn std::error::Error>> {
        self.file_path = Some(file_path.to_string());
        let mut file = File::open(file_path)?;
        let mut buffer = Vec::new();
        file.read_to_end(&mut buffer)?;

        match macho::MachO::from_bytes(&buffer) {
            Ok(macho_obj) => {
                self.macho_data = Some(macho_obj);
                self.update_terminal_content();
                Ok(())
            }
            Err(e) => {
                self.terminal_content = Text::from(format!("Error loading Mach-O file: {:?}", e));
                self.macho_data = None;
                Err(Box::new(e))
            }
        }
    }

    fn update_terminal_content(&mut self) {
        let mut lines = Vec::new();
        if let Some(ref macho) = self.macho_data {
            lines.push(Line::from(Span::styled(
                format!("File: {}", self.file_path.as_deref().unwrap_or("N/A")),
                Style::default().add_modifier(Modifier::BOLD),
            )));
            lines.push(Line::from("")); // Blank line

            // Basic display of Mach-O header info
            lines.push(Line::from(Span::styled(
                "Mach-O Header:",
                Style::default().add_modifier(Modifier::BOLD),
            )));
            lines.push(Line::from(format!("  Magic: 0x{:x}", macho.header.magic)));
            lines.push(Line::from(format!("  Cpu Type: {:?}", macho.header.cputype)));
            lines.push(Line::from(format!("  Cpu Subtype: {:?}", macho.header.cpusubtype)));
            lines.push(Line::from(format!("  File Type: {:?}", macho.header.filetype)));
            lines.push(Line::from(format!("  Number of Load Commands: {}", macho.header.ncmds)));
            lines.push(Line::from(format!("  Size of Load Commands: {}", macho.header.sizeofcmds)));
            lines.push(Line::from(format!("  Flags: 0x{:x}", macho.header.flags)));
            lines.push(Line::from(""));

            // Display Load Commands (basic info)
            if !macho.load_commands.is_empty() {
                lines.push(Line::from(Span::styled(
                    "Load Commands:",
                    Style::default().add_modifier(Modifier::BOLD),
                )));
                for (i, cmd) in macho.load_commands.iter().enumerate() {
                    lines.push(Line::from(format!(
                        "  {}: Type={:?}, Size={}",
                        i, cmd.cmd, cmd.cmdsize
                    )));
                }
                lines.push(Line::from(""));
            }

            // Placeholder for more detailed section/segment display
            lines.push(Line::from(Span::styled(
                "Sections/Segments (Placeholder):",
                Style::default().add_modifier(Modifier::BOLD),
            )));
            lines.push(Line::from("  Detailed view of sections and segments would go here."));

        } else {
            lines.push(Line::from("No Mach-O file loaded. Press 'l' to load a file."));
        }
        self.terminal_content = Text::from(lines);
    }
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    // Setup terminal
    enable_raw_mode()?;
    let mut stdout = stdout();
    execute!(stdout, EnterAlternateScreen, EnableMouseCapture)?;
    let backend = CrosstermBackend::new(stdout);
    let mut terminal = Terminal::new(backend)?;

    // Create app.
    let mut app = App::new()?;

    // Initial load (example - you might want to handle this differently, e.g. prompt user)
    // For now, let's assume no file is loaded initially.

    let mut active_input_field = String::new();
    let mut show_input_field = false;

    // Main loop
    loop {
        terminal.draw(|f| {
            let size = f.size();
            let chunks = Layout::default()
                .direction(Direction::Vertical)
                .constraints(
                    [
                        Constraint::Length(3), // Header
                        Constraint::Min(0),    // Content Area
                        Constraint::Length(3), // Footer/Input Area
                    ]
                    .as_ref(),
                )
                .split(size);

            // Header
            let header = Paragraph::new(vec![
                Line::from(Span::styled("Kextdyld Mach-O Parser/Interpreter", Style::default().add_modifier(Modifier::BOLD))),
            ]).block(Block::default().title("Header").borders(Borders::ALL));
            f.render_widget(header, chunks[0]);

            // Content Area
            let content_area_constraints = Layout::default()
                .direction(Direction::Horizontal)
                .constraints([Constraint::Percentage(70), Constraint::Percentage(30)].as_ref())
                .split(chunks[1]);

            let mut content_lines = app.terminal_content.lines.clone();
            let scrollbar = Scrollbar::default()
                .orientation(ScrollbarOrientation::Vertical)
                .tracks_mouse(true)
                .begin_symbol(Some("↑"))
                .end_symbol(Some("↓"));

            let mut list_items = Vec::new();
            for line in content_lines.iter().skip(app.scroll_offset as usize).take(content_area_constraints[0].height as usize) {
                list_items.push(ListItem::new(line.clone()));
            }

            let content_list = List::new(list_items)
                .block(Block::default().title("Mach-O Structure").borders(Borders::ALL))
                .highlight_style(Style::default().add_modifier(Modifier::REVERSED))
                .highlight_symbol(">");

            // Render the list and its scrollbar
            let list_height = content_area_constraints[0].height as usize;
            let list_content_height = content_lines.len();
            let scrollbar_state = scrollbar.scroll_state.clone()
                .content_height(list_content_height)
                .position(app.scroll_offset as u128);

            f.render_widget(content_list, content_area_constraints[0]);
            f.render_widget(
                scrollbar.clone().scroll_state(scrollbar_state),
                content_area_constraints[0].right(1),
            );


            // Placeholder for Python interpreter or other info
            let mut python_content = vec![Line::from(Span::styled(
                "Python Interpreter",
                Style::default().add_modifier(Modifier::BOLD),
            ))];
            python_content.push(Line::from("  (Python integration goes here)"));
            let python_widget = Paragraph::new(python_content)
                .block(Block::default().title("Python").borders(Borders::ALL));
            f.render_widget(python_widget, content_area_constraints[1]);


            // Input field area
            let input_paragraph = if show_input_field {
                Paragraph::new(vec![
                    Line::from(Span::styled(
                        format!("> {}", active_input_field),
                        Style::default(),
                    )),
                ])
                .block(Block::default().title("Input").borders(Borders::ALL))
            } else {
                Paragraph::new(vec![
                    Line::from("Press 'l' to load file, 'q' to quit.")
                ])
                .block(Block::default().title("Footer").borders(Borders::ALL))
            };
            f.render_widget(input_paragraph, chunks[2]);
        })?;

        // Event handling
        match crossterm::event::read()? {
            Event::Key(key_event) => {
                match key_event.code {
                    KeyCode::Char('q') => break, // Quit
                    KeyCode::Char('l') => {
                        show_input_field = true;
                        active_input_field.clear();
                    }
                    KeyCode::Enter => {
                        if show_input_field {
                            if !active_input_field.is_empty() {
                                // Attempt to load the file
                                match app.load_file(&active_input_field) {
                                    Ok(_) => {
                                        // File loaded, update content and reset input
                                        app.update_terminal_content();
                                    }
                                    Err(e) => {
                                        // Error already displayed in terminal_content
                                        eprintln!("File load error: {}", e);
                                    }
                                }
                            }
                            show_input_field = false;
                            active_input_field.clear();
                        }
                    }
                    KeyCode::Esc => {
                        show_input_field = false;
                        active_input_field.clear();
                    }
                    KeyCode::Backspace => {
                        if show_input_field {
                            active_input_field.pop();
                        }
                    }
                    KeyCode::Char(c) => {
                        if show_input_field {
                            active_input_field.push(c);
                        }
                    }
                    KeyCode::Down => {
                        if app.scroll_offset < app.terminal_content.lines.len() as u16 - 1 {
                            app.scroll_offset += 1;
                        }
                    }
                    KeyCode::Up => {
                        if app.scroll_offset > 0 {
                            app.scroll_offset -= 1;
                        }
                    }
                    _ => {}
                }
            }
            _ => {}
        }
    }

    // Restore terminal
    disable_raw_mode()?;
    execute!(terminal.backend_mut(), LeaveAlternateScreen, DisableMouseCapture)?;
    terminal.show_cursor()?;

    Ok(())
}

// Helper to execute crossterm commands
use crossterm::{execute, terminal::{EnterAlternateScreen, LeaveAlternateScreen}};
use std::io::Write;