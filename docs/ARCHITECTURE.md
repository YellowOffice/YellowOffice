# YellowOffice Architecture

## Design principles

1. **Native Haiku integration** — Haiku API for UI, storage, and app model.
2. **Modularity** — one library per concern; the app is a thin shell.
3. **Testability off-platform** — everything above the UI layer builds and
   runs unit tests on a normal Unix host against `build/haiku-stub` headers.

## Layering

```
apps/YellowDocs            BApplication, BWindow, canvas view, menus
        │
        ▼
libraries/YellowUI         ToolBar, ActionManager
        │
        ▼
libraries/YellowDocumentLayout
                           LayoutEngine; text/paragraph/page layouters;
                           selection, caret, hit testing, render context
        │
        ▼
libraries/YellowDocumentCore
                           DocumentNode tree (Section, Paragraph, TextRun,
                           List, Table, Image, …), YellowDocument, Command
                           stack (undo/redo), styles, statistics
        │
        ├──────────────────────────────┐
        ▼                              ▼
libraries/YellowImportExport    libraries/YellowCommon
        JSON/XML parsers,                Logger, Json value,
        ZIP container (zlib),            Error, shared defines
        YDF/Markdown/Text/ODF
        importers + exporters,
        FormatRegistry
```

## Document model (YellowDocumentCore)

- `DocumentNode` is the tree base (`GetType()`, children, parent). Concrete
  nodes: `SectionNode`, `ParagraphNode`, `TextRunNode`, `ListNode` /
  `ListItemNode`, `TableNode` / `TableRowNode` / `TableCellNode`, `ImageNode`,
  `PageBreakNode`.
- `YellowDocument` owns the root node, the command stack, and metadata.
  `Clear()` reseeds the root with one empty paragraph — importers must empty
  the root themselves before appending parsed content.
- Edits go through `Command` objects (`CommandInsertNode`,
  `CommandDeleteRange`, `CommandSplitParagraph`, …) so undo/redo works.
- `TextStyle` (bold/italic/underline/color/size/family, paragraph alignment)
  lives on runs and paragraphs; statistics (`word/character/paragraph counts`)
  are computed on demand, counting the paragraph mark as a character.

## Layout (YellowDocumentLayout)

`LayoutEngine` measures the document into a list of positioned text lines,
caret/selection geometry, and page breaks. `DocumentCanvas` in the app owns
one engine, feeds it edited content, and paints the output; hit testing maps
mouse positions back to document positions.

## Import/export (YellowImportExport)

- `FormatRegistry` maps names/extensions/MIME types to `Importer` /
  `Exporter` instances (YDF, Markdown, plain text, ODF).
- **YDF** (native): a ZIP (zlib via `ZipArchive`) containing `document.json`
  (the node tree, base64 for embedded binary) plus `mimetype` stored first,
  uncompressed, per ZIP convention. Serializer: `DocumentSerializer`.
- **ODF**: same ZIP container; `content.xml` / `styles.xml` parsed by the
  small hand-written XML parser (`Xml.h`). Paragraph/character properties
  map to/from `TextStyle`; automatic styles for run formatting.
- **Markdown**: ATX headings, bold/italic/underline, lists; import maps
  heading levels to paragraph styles.
- **Plain text**: paragraphs split on blank lines.

## UI (apps/YellowDocs, YellowUI)

- `YellowDocsApp` — BApplication; single-launch; handles refs/open/quit.
- `YellowDocsWindow` — BWindow with a menu bar, `ToolBar` (custom BView
  button bar posting `BMessage`s with command constants), the document
  canvas in a BScrollView, and a BStatusBar. Owns BFilePanel open/save and
  dispatches import/export by extension.
- `DocumentCanvas` — BView; keyboard editing (typing, navigation, selection,
  clipboard, undo, zoom), drag selection, delegates mutations to the
  document's command stack, posts `kMsgDocumentEdited` to the window.
- `YellowUI::ToolBar` / `ActionManager` — shared button bar and
  listener/dispatch helpers.

## Testing and off-Haiku builds

- `build/haiku-stub/` contains minimal but signature-accurate Haiku API
  headers (BString, BMessage, support/storage/interface kit subsets) so the
  non-UI layer compiles anywhere. Not linked — only `-fsyntax-only`.
- `scripts/syntax-check.sh` syntax-checks every `.cpp` in the repo against
  the stubs with the project include paths.
- `tests/` build with CMake off-Haiku (core + import/export + common) and
  exercise JSON, the document model, commands, and format round-trips
  (serializer, ZIP, XML, Markdown, plain text, YDF, ODF, registry).

## Packaging

`scripts/package.sh` (Haiku only): Release build → `rc`/`xres` for
signature, version, file types, and the HVIF icon → staged tree with
`.PackageInfo` → `package create`. See `packaging/hpkg/YellowDocs.PackageInfo`
for the package metadata.
