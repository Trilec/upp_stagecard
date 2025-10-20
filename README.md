# StageCard (U++)

**StageCard** is a tiny, flexible card container for U++ that pairs a compact **header** with a highly configurable **content** area. It supports WRAP/GRID/FIXED layouts, per-card vertical scrolling, independent frame/fill for card/header/content, and a smart **AutoFill** mode that splits one or multiple children neatly to fill the available height.

- Header: title / subtitle / badge (optional)
- Content: WRAP, GRID, or FIXED layout modes
- Per-card scrollbar (no global scroll required)
- Frames & fills for Card / Header / Content areas (independent)
- Simple, U++-style API: `AddContent`, `ClearContent`, `ReplaceContent`

## Why StageCard?
In demo apps and tooling UIs you often need repeatable “cards” with a consistent header and an adaptable body (lists, grids, editors, previews). StageCard gives you that pattern with minimal code and no heavy layout engine.

---

## Package Layout

This repository follows a simple U++ package layout:

```

StageCard/            # U++ package
├─ StageCard.upp
└─ StageCard.h      # header-only (inline)
examples/
└─ UniCodePicker/
├─ UniCodePicker.upp
└─ main.cpp      # example using StageCard

````

---

## Install

1. Copy `StageCard/` into your U++ assembly or add this repository as an assembly.
2. Add `StageCard` to your package `uses:` list.
3. `#include <StageCard/StageCard.h>` in your code.
4. Paste the code in as its header only based (how I use it)

`StageCard.upp` declares dependencies on `CtrlLib` and `Painter`.

---

## Quick Start

```cpp
#include <CtrlLib/CtrlLib.h>
#include <StageCard/StageCard.h>
using namespace Upp;

struct App : TopWindow {
    StageCard card;
    Button    btn;

    App() {
        Title("StageCard demo").Sizeable().Zoomable();
        btn.SetLabel("Hello");
        btn.SetMinSize(Size(DPI(120), DPI(36)));

        card.SetTitle("My Card")
            .EnableContentScroll(true)
            .ContentWrap()
            .SetContentInset(DPI(8), DPI(8), DPI(8), DPI(8))
            .SetContentGap(DPI(6), DPI(6));

        card.AddContent(btn);
        Add(card.HSizePos(DPI(8), DPI(8)).VSizePos(DPI(8), DPI(8)));
    }
};

GUI_APP_MAIN() { App().Run(); }
````

---

## Common Recipes

### 1) Rounded dark content panel with transparent `DocEdit`

```cpp
StageCard code;
DocEdit   editor;

code.EnableCardFrame(false)
    .EnableCardFill(false)
    .EnableContentFill(true)
    .EnableContentFrame(true)
    .SetContentCornerRadius(DPI(10))
    .SetContentStrokeThickness(0)
    .SetContentFrameColors(Color(32,36,42), Black())
    .SetContentInset(DPI(12), DPI(12), DPI(12), DPI(12))
    .EnableContentScroll(true);

editor.Transparent();
editor.SetFrame(NullFrame());
editor.SetFont(Monospace(12));
editor.SetColor(DocEdit::INK_NORMAL, Color(170,255,170)); // green code

code.ReplaceContent(editor);
```

### 2) AutoFill (smart vertical split)

* **One child**: fills the content pane.
* **Multiple children**: equally split vertically with gaps, respecting content insets.

```cpp
StageCard flex;
Button a, b, c;
flex.ContentAbsolute()
    .EnableContentAutoFill(true)     // <—
    .SetContentInset(DPI(8),DPI(8),DPI(8),DPI(8))
    .SetContentGap(DPI(6), DPI(6));
flex.AddContent(a);
flex.AddContent(b);
flex.AddContent(c);
```

### 3) WRAP grid of tiles

```cpp
StageCard grid;
grid.ContentWrap()
    .EnableContentScroll(true)
    .SetContentInset(DPI(6), DPI(6), DPI(6), DPI(6))
    .SetContentGap(DPI(6), DPI(6));
// Add N buttons/tiles with SetMinSize; StageCard will wrap them.
```

### 4) Strict GRID (N columns, fixed cell size)

```cpp
StageCard g;
g.ContentGrid()
 .GridCols(4)
 .GridCell(DPI(120), DPI(90))
 .GridStretch(true)
 .SetContentInset(DPI(6),DPI(6),DPI(6),DPI(6))
 .SetContentGap(DPI(6),DPI(6));
```

---

## Public API (short)

**Header**

* `SetTitle(const String&, Font = StdFont().Bold().Height(DPI(22)))`
* `SetSubTitle(const String&, Font = StdFont().Height(DPI(12)))`
* `SetBadge(const String&, Font = StdFont().Height(DPI(12)))`
* `SetHeaderAlign(LEFT|RIGHT|CENTER)`
* `SetTitleUnderlineThickness(int)`, `SetTitleUnderlineColor(Color)`
* `AddHeader(Ctrl&)`, `ClearHeader()`, `HeaderPane()`

**Frames & Colors**

* Card: `EnableCardFrame(bool)`, `EnableCardFill(bool)`, `SetCardCornerRadius(int)`,
  `SetCardStrokeThickness(int)`, `EnableCardDashed(bool)`, `SetCardDashPattern(String)`,
  `SetCardColors(Color fill, Color stroke, Color titleInk, Color subInk, Color badgeInk)`
* Header / Content: same pattern with `Enable…Frame/Fill`, `Set…CornerRadius`, etc.

**Content**

* Child mgmt: `AddContent(Ctrl&)`, `ClearContent()`, `ReplaceContent(Ctrl&)`
* Insets/gaps: `SetContentInset(int l,t,r,b)`, `SetContentGap(int gx, int gy)`
* Layouts: `ContentAbsolute()`, `ContentWrap()`, `ContentGrid()`
* WRAP sizing: `WrapItemSize(int w,int h)`
* GRID sizing: `GridCols(int)`, `GridCell(int w,int h)`, `GridStretch(bool)`
* Scrolling: `EnableContentScroll(bool)` (per-card vertical scrollbar)
* Clamp: `EnableContentClampToPane(bool)`
* Smart fill: `EnableContentAutoFill(bool)` (FIXED mode only)

---

## Notes

* StageCard is **header-only**. Keep method bodies inline.
* Avoid `using namespace Upp;` in the header. Use it in examples/apps instead.
* Per-card scrolling kicks in only when the content height exceeds the visible pane.
  (You don’t need to size the content layer manually; the card manages it.)

---

## License

MIT

## Contributing

Issues and PRs welcome. Please keep API U++-idiomatic: small, composable methods; minimal magic.

