# YellowOffice Architecture Overview

## Design Philosophy

YellowOffice is designed with the following principles:

1. **Native Haiku Integration**: Use Haiku's native APIs, conventions, and UI patterns
2. **Modularity**: Separate concerns into reusable libraries
3. **Extensibility**: Design for future growth and new features
4. **Performance**: Optimize for responsive editing and rendering
5. **Compatibility**: Support multiple document formats through pluggable architecture

## High-Level Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    YellowDocs Application                      │
├─────────────────────────────────────────────────────────────┤
│  MainWindow       DocumentWindow      Settings                │
│  MenuBar          ToolBar             StatusBar               │
│  DocumentCanvas   FormattingPanel     Sidebar                │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                    YellowUI Library                            │
├─────────────────────────────────────────────────────────────┤
│  UI Components    Dialogs            Layout Helpers          │
│  Action Manager   Command System     Shortcut Handling       │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                 YellowDocumentLayout Library                   │
├─────────────────────────────────────────────────────────────┤
│  Layout Engine    Text Measurement   Pagination               │
│  Line Breaking    Table Layout       Image Placement          │
│  Selection        Hit Testing        Scroll Management        │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                 YellowDocumentCore Library                      │
├─────────────────────────────────────────────────────────────┤
│  Document Model   DOM Tree           Editing Commands        │
│  Undo/Redo        Styles             Document Metadata       │
│  Paragraphs       Text Runs          Lists & Tables          │
└─────────────────────────────────────────────────────────────┘
                              │
              ┌───────────────────────┼───────────────────────┐
              ▼                       ▼                       ▼
┌─────────────────────┐ ┌─────────────────┐ ┌─────────────────────┐
│ YellowImportExport   │ │  YellowCommon    │ │   Haiku APIs        │
│  Markdown Adapter    │ │  Utilities       │ │   BApplication       │
│  ODF Adapter         │ │  Logging         │ │   BWindow           │
│  YDF Serializer      │ │  Error Handling  │ │   BView             │
│  Format Registry     │ │  File Utilities  │ │   BMenuBar          │
└─────────────────────┘ └─────────────────┘ │   BToolBar          │
                                           │   BTextView          │
                                           │   BMessage           │
                                           │   BHandler           │
                                           │   BLooper            │
                                           └─────────────────────┘
```

## Library Breakdown

### 1. YellowDocumentCore

**Purpose**: Core document model and editing logic

**Components**:
- `DocumentNode` hierarchy (base class for all document elements)
- `YellowDocument` - Root document container
- `SectionNode` - Document sections
- `ParagraphNode` - Paragraphs with formatting
- `TextRunNode` - Formatted text spans
- `ListNode` / `ListItemNode` - List structures
- `TableNode` / `TableRowNode` / `TableCellNode` - Table structures
- `ImageNode` - Embedded images
- `PageBreakNode` - Page breaks
- `HeaderFooterNode` - Headers and footers
- `StyleDefinition` - Style system
- `DocumentMetadata` - Author, timestamps, etc.

**Key Features**:
- Tree-based document structure
- Command-based editing with undo/redo
- Style inheritance system
- Document validation
- Change notification system

### 2. YellowDocumentLayout

**Purpose**: Document layout and rendering

**Components**:
- `LayoutEngine` - Coordinates layout passes
- `TextLayouter` - Measures and positions text
- `ParagraphLayouter` - Handles paragraph formatting
- `TableLayouter` - Table layout calculations
- `PageLayouter` - Pagination and page management
- `SelectionManager` - Handles text selection
- `CaretManager` - Manages cursor positioning
- `HitTester` - Maps screen coordinates to document positions
- `RenderContext` - Drawing context for document rendering

**Key Features**:
- Incremental layout (avoid full re-layout on every change)
- Multi-page document support
- Zoom level support
- Scrollable canvas
- Selection painting

### 3. YellowImportExport

**Purpose**: Document format import and export

**Components**:
- `FormatRegistry` - Manages available formats
- `Importer` base class and implementations:
  - `MarkdownImporter`
  - `ODFImporter`
  - `YDFImporter`
- `Exporter` base class and implementations:
  - `MarkdownExporter`
  - `ODFExporter`
  - `YDFExporter`
- `FormatCapabilities` - Describes what each format supports

**Key Features**:
- Pluggable architecture for new formats
- Format capability detection
- Error handling for malformed documents
- Progress reporting for large documents

### 4. YellowUI

**Purpose**: Shared UI components and patterns

**Components**:
- `ActionManager` - Manages actions and keyboard shortcuts
- `CommandSystem` - Command pattern implementation
- `DialogFactory` - Creates standard dialogs
- `FormattingPanel` - Text and paragraph formatting controls
- `StylePanel` - Style management UI
- `DocumentNavigator` - Document structure navigation
- `StatusBar` - Application status display
- `ToolBar` - Customizable toolbar

**Key Features**:
- Consistent look and feel across applications
- Keyboard shortcut management
- Localization support
- Accessibility considerations

### 5. YellowCommon

**Purpose**: Shared utilities and helpers

**Components**:
- `Logger` - Logging framework
- `ErrorHandler` - Error reporting and handling
- `FileUtils` - File system operations
- `StringUtils` - String manipulation
- `MemoryUtils` - Memory management helpers
- `PlatformUtils` - Haiku-specific utilities
- `Settings` - Application settings management

## Document Model Details

### Node Hierarchy

```
DocumentNode (abstract)
├── SectionNode
│   ├── ParagraphNode
│   │   └── TextRunNode
│   ├── ListNode
│   │   └── ListItemNode
│   │       └── ParagraphNode
│   ├── TableNode
│   │   └── TableRowNode
│   │       └── TableCellNode
│   │           └── ParagraphNode
│   ├── ImageNode
│   ├── PageBreakNode
│   └── HeaderFooterNode
└── DocumentMetadata
```

### Document Tree Example

```
YellowDocument
├── DocumentMetadata
│   ├── title: "My Document"
│   ├── author: "John Doe"
│   └── created: 2024-01-01
├── SectionNode (default)
│   ├── ParagraphNode (Heading 1)
│   │   └── TextRunNode("Introduction", bold, 24pt)
│   ├── ParagraphNode (Body)
│   │   └── TextRunNode("Welcome to YellowDocs...")
│   ├── ListNode (bulleted)
│   │   ├── ListItemNode
│   │   │   └── ParagraphNode
│   │   │       └── TextRunNode("First item")
│   │   └── ListItemNode
│   │       └── ParagraphNode
│   │           └── TextRunNode("Second item")
│   └── TableNode (3x3)
│       └── TableRowNode
│           ├── TableCellNode
│           │   └── ParagraphNode
│           │       └── TextRunNode("Cell 1")
│           └── TableCellNode
│               └── ParagraphNode
│                   └── TextRunNode("Cell 2")
└── SectionNode (page break)
    └── ParagraphNode
        └── TextRunNode("Next page content...")
```

## Editing Architecture

### Command Pattern

All edits are performed through command objects that support undo/redo:

```cpp
class EditCommand {
public:
    virtual ~EditCommand();
    virtual void Execute(YellowDocument* document) = 0;
    virtual void Undo(YellowDocument* document) = 0;
    virtual void Redo(YellowDocument* document) = 0;
    virtual const char* GetName() const = 0;
};

// Example: Insert text command
class InsertTextCommand : public EditCommand {
public:
    InsertTextCommand(const DocumentPosition& position, 
                      const BString& text, 
                      const TextStyle& style);
    
    void Execute(YellowDocument* document) override;
    void Undo(YellowDocument* document) override;
    void Redo(YellowDocument* document) override;
    const char* GetName() const override { return "Insert Text"; }
};
```

### Command History

```cpp
class CommandHistory {
public:
    void AddCommand(std::unique_ptr<EditCommand> command);
    void Undo();
    void Redo();
    bool CanUndo() const;
    bool CanRedo() const;
    void Clear();
    
    // For document save/restore
    void MarkSavedState();
    bool IsModifiedSinceSave() const;
};
```

## Layout Architecture

### Layout Process

1. **Document Change**: Edit command modifies document tree
2. **Invalidation**: Mark affected regions as dirty
3. **Layout Pass**: 
   - Measure text runs
   - Calculate line breaks
   - Position paragraphs
   - Handle pagination
4. **Render**: Draw document to view

### Incremental Layout Strategy

- Track which parts of the document are affected by changes
- Only re-layout affected paragraphs and their dependencies
- Cache layout results for unchanged content
- Handle cascading changes (e.g., font size change affects line breaks)

## File Format Architecture

### Format Registry

```cpp
class FormatRegistry {
public:
    void RegisterFormat(std::unique_ptr<DocumentFormat> format);
    DocumentFormat* FindFormatByExtension(const BString& extension);
    std::vector<DocumentFormat*> GetAllFormats() const;
    std::vector<DocumentFormat*> GetImporters() const;
    std::vector<DocumentFormat*> GetExporters() const;
};

class DocumentFormat {
public:
    virtual ~DocumentFormat();
    
    virtual const char* GetName() const = 0;
    virtual const char* GetDescription() const = 0;
    virtual const char* const* GetExtensions() const = 0;
    virtual const char* GetMIMEType() const = 0;
    
    virtual bool CanImport() const = 0;
    virtual bool CanExport() const = 0;
    
    virtual status_t Import(YellowDocument* document, BFile& file) = 0;
    virtual status_t Export(const YellowDocument* document, BFile& file) = 0;
    
    virtual FormatCapabilities GetCapabilities() const = 0;
};
```

### YDF Format Structure

```
.ydf file (ZIP archive)
├── manifest.json          # Format version, metadata
├── document.xml           # Document structure
├── styles.xml            # Style definitions
├── resources/            # Embedded assets
│   ├── image1.png
│   └── image2.jpg
└── meta/                 # Additional metadata
    ├── author.info
    └── timestamps
```

## UI Architecture

### Main Window Structure

```
YellowDocsWindow (BWindow)
├── fMenuBar (BMenuBar)
│   ├── File menu
│   ├── Edit menu
│   ├── View menu
│   ├── Insert menu
│   ├── Format menu
│   ├── Tools menu
│   └── Help menu
├── fToolBar (BToolBar)
│   ├── New, Open, Save buttons
│   ├── Undo, Redo buttons
│   ├── Format buttons (Bold, Italic, etc.)
│   └── Zoom controls
├── fMainView (BView)
│   ├── fDocumentCanvas (DocumentCanvas)
│   │   └── Document rendering
│   ├── fSidebar (BSplitView)
│   │   ├── fNavigator (DocumentNavigator)
│   │   └── fInspector (PropertyInspector)
│   └── fStatusBar (StatusBar)
└── fFormattingPanel (FormattingPanel)
```

### Document Canvas

The `DocumentCanvas` is a custom `BView` that:
- Renders the document using the layout engine
- Handles mouse events for selection and editing
- Manages keyboard input
- Handles scrolling
- Supports zoom levels
- Draws selection and caret

## Communication Patterns

### Message Passing

Haiku uses message passing between objects. YellowDocs uses:

```cpp
// Message constants
const uint32 kMsgNewDocument = 'YDnew';
const uint32 kMsgOpenDocument = 'YDopen';
const uint32 kMsgSaveDocument = 'YDsave';
const uint32 kMsgUndo = 'YDundo';
const uint32 kMsgRedo = 'YDredo';
const uint32 kMsgTextChanged = 'YDtxtc';
const uint32 kMsgSelectionChanged = 'YDselc';
const uint32 kMsgDocumentLoaded = 'YDload';

// Messages often include additional data
BMessage msg(kMsgTextChanged);
msg.AddInt32("position", position);
msg.AddString("text", text);
```

### Notification System

```cpp
class DocumentObserver {
public:
    virtual void DocumentChanged(const DocumentChange& change) = 0;
    virtual void SelectionChanged(const Selection& selection) = 0;
    virtual void LayoutInvalidated(const LayoutRegion& region) = 0;
};

class YellowDocument {
public:
    void AddObserver(DocumentObserver* observer);
    void RemoveObserver(DocumentObserver* observer);
    // ...
};
```

## Build System Integration

### Library Dependencies

```
YellowDocs
├── YellowUI
│   ├── YellowDocumentLayout
│   │   └── YellowDocumentCore
│   └── YellowCommon
├── YellowImportExport
│   └── YellowDocumentCore
└── YellowCommon
```

### Build Order

1. YellowCommon
2. YellowDocumentCore
3. YellowDocumentLayout
4. YellowImportExport
5. YellowUI
6. YellowDocs

## Future Considerations

1. **Multi-document support**: Multiple documents in separate windows
2. **Collaboration**: Real-time collaborative editing
3. **Plugins**: Extensible through plugin architecture
4. **Scripting**: Automation through scripting interface
5. **Accessibility**: Full accessibility support
6. **Internationalization**: Multi-language support
7. **Performance**: Optimizations for large documents

## Conclusion

This architecture provides a solid foundation for YellowDocs while allowing for future expansion into a full office suite. The modular design ensures that components can be reused across different applications in the YellowOffice suite.
