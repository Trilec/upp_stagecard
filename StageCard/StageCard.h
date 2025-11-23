#ifndef _StageCard_StageCard_h_
#define _StageCard_StageCard_h_

#include <CtrlLib/CtrlLib.h>
#include <Painter/Painter.h>

namespace Upp {

/*
================================================================================
StageCard — "card" chrome + simple content layout helper
================================================================================

Concept
-------

StageCard is a composite control that gives you:

  1. A HEADER BAND:
     - Title, subtitle
     - Optional badge (icon or text)
     - Underline (horizontal or vertical)
     - Optional header background and frame
     - Optional header children (buttons, toggles, etc.)

  2. A CONTENT PANE:
     - Card-like background + optional frame
     - Simple layout engine for child controls
     - Optional scrolling (auto-enabled when content overflows)
     - Optional horizontal wrapping for "chip"-like layouts

The goal is:
  - Make it easy to build "card sections" inside dashboards, forms, inspectors.
  - Avoid re-implementing header chrome and basic layout over and over.

-------------------------------------------------------------------------------
Layout Model (high level)
-------------------------------------------------------------------------------

You primarily control layout with **SetStack(StackMode)**:

  enum class StackMode { NONE, STACKV, STACKH };

  • NONE   -> "manual mode":
      - StageCard does NOT position children; you do it directly on Content().
      - StageCard only provides scrolling + clipping.
      - Use this when you already have a dedicated layout (GridLayout, etc.).

  • STACKV -> vertical stack:
      - Children are arranged top-to-bottom.
      - You add children with AddFixed(...) or AddExpand(...).
      - Scrollbar is vertical if needed.

  • STACKH -> horizontal stack:
      - Children are arranged left-to-right.
      - Scrollbar is horizontal if needed.
      - If you also call SetWrap(true), the layout becomes a wrapping "flow"
        (chips, tiles, etc.) with vertical scroll.

Supporting calls:

  - StackV()       -> SetStack(StackMode::STACKV)
  - StackH()       -> SetStack(StackMode::STACKH)
  - ContentManual()-> SetStack(StackMode::NONE)
  - SetWrap(true)  -> Enable wrapping when in STACKH mode.

-------------------------------------------------------------------------------
Stack children: AddFixed / AddExpand / AddSpacer
-------------------------------------------------------------------------------

Stack mode uses a one-dimensional "main axis" (vertical or horizontal):

  • In STACKV:
      - main axis = vertical (height)
      - cross axis = horizontal (width)

  • In STACKH:
      - main axis = horizontal (width)
      - cross axis = vertical (height)

You add content with:

  StageCard& AddFixed (Ctrl& c, int px, int py);
  StageCard& AddFixed (Ctrl& c, int px);
  StageCard& AddFixed (Ctrl& c);
  StageCard& AddExpand(Ctrl& c, int weight = 1);
  StageCard& AddSpacer(int weight = 1);

Semantics:

  • AddFixed(c, px, py)
      - In WRAP mode (STACKH + wrap):
          - px, py are the *tile size* (width/height) for that control.
          - If px <= 0 or py <= 0, StageCard uses c.GetMinSize().
          - Tiles are placed left-to-right, wrapping into new rows.
      - In non-wrap stack:
          - px is the *main-axis* fixed size (height in STACKV, width in STACKH).
          - cross-axis is always stretched to fill the available size.
          - If px <= 0, StageCard uses c.GetMinSize().cy or .cx on the main axis.

  • AddFixed(c, px)
      - Convenience: single fixed size for the main axis.
      - In WRAP mode, treated as a square tile (px x px).

  • AddFixed(c)
      - Uses c.GetMinSize() on the main axis (and both axes when wrapping).

  • AddExpand(c, weight)
      - Control participates in distributing *extra* main-axis space.
      - Its natural size comes from GetMinSize() (or px if you later use a fixed_px).
      - The "weight" is relative — e.g. weight 2 gets twice as much extra as weight 1.
      - This is how you make "buttons share height equally" in a vertical stack, etc.

  • AddSpacer(weight)
      - Like AddExpand but with no control — just flexible gap.

Important:
  - There is NO AddFit API anymore.
  - If you want a control to "fill width", just:
        • Use STACKV and AddFixed(...); StageCard always stretches cross-axis.
        • Or use AddExpand to let it also flex in the main axis.

-------------------------------------------------------------------------------
Manual mode
-------------------------------------------------------------------------------

If you call ContentManual() or SetStack(StackMode::NONE):

  • StageCard does NOT reposition children in Content().
  • You are responsible for doing SetRect() on your controls.
  • StageCard:
      - Clips content to a viewport.
      - Tracks the union of child rectangles.
      - Shows a scrollbar if content is larger than the viewport.
  • Scroll axis (vertical vs horizontal) follows an internal "direction"
    (vertical by default). For most manual use-cases you just let it be.

You can always access the scrolled layer via:
  ParentCtrl& Content()  -> add your own layout, or controls with explicit SetRect.

-------------------------------------------------------------------------------
Scrolling & insets
-------------------------------------------------------------------------------

  - EnableContentScroll(true)  -> allow scrollbars when content overflows.
  - EnableContentClampToPane(true) -> clamp the content viewport to the card.
  - SetContentInset(...)       -> margin around the content frame.
  - SetContentInnerInset(...)  -> extra inset inside the content pane applied
                                  when laying out the children.

-------------------------------------------------------------------------------
Typical usage patterns
-------------------------------------------------------------------------------

1) Vertical card with stacked rows:

    StageCard card;
    card.SetTitle("Settings")
        .StackV()
        .EnableContentScroll(true)
        .SetContentInset(DPI(8), DPI(8), DPI(8), DPI(8))
        .SetContentGap(DPI(4), DPI(4));

    Button b1, b2;
    b1.SetLabel("Apply");
    b2.SetLabel("Cancel");

    card.AddFixed(b1);                // uses b1.GetMinSize().cy as row height
    card.AddFixed(b2, DPI(40));       // force height 40px

2) "Chips" / tiles layout with wrapping:

    StageCard chips;
    chips.SetTitle("Tags")
         .SetStackH()
         .SetWrap(true)
         .EnableContentScroll(true)
         .SetContentGap(DPI(6), DPI(6));

    for (...) {
        ChipCtrl& c = ...;
        c.SetMinSize(Size(DPI(80), DPI(32)));
        chips.AddFixed(c);    // tile size taken from GetMinSize()
    }

3) Manual content with a custom layout:

    StageCard view;
    view.SetTitle("Preview")
        .SetStackNode() //Manual placement
        .EnableContentScroll(true);

    ParentCtrl& pane = view.Content();
    pane.Add(graphView.SizePos());    // you manage layout yourself

-------------------------------------------------------------------------------
Public API below: mostly styling + the stack helpers described above.
-------------------------------------------------------------------------------
*/

class StageCard : public ParentCtrl {
public:
    typedef StageCard CLASSNAME;

    enum HeaderAlign { LEFT, RIGHT, CENTER };

    // Public-facing stacking mode
    enum class StackMode { NONE, STACKV, STACKH };

    // Helpers to derive per-state colors from a base
    static inline void MakeFaceStates(Color base, Color (&dst)[4],
                                      int hot_pct=12, int press_pct=14, int dis_mix=60) {
        dst[0] = base;
        dst[1] = Blend(base, White(), hot_pct);
        dst[2] = Blend(base, Black(), press_pct);
        dst[3] = Blend(SColorFace(), SColorPaper(), dis_mix);
    }
    static inline void MakeBorderStates(Color base, Color (&dst)[4],
                                        int hot_hi=20, int press_bl=15, int dis_mix=35) {
        dst[0] = base;
        dst[1] = Blend(base, SColorHighlight(), hot_hi);
        dst[2] = Blend(base, Black(), press_bl);
        dst[3] = Blend(base, SColorDisabled(), dis_mix);
    }

    // ------- Styling types -------
    struct UiPalette {
        // Card (stateless)
        Color cardBorder = SColorShadow();
        Color cardFill   = Color(240,240,240);

        // Header (stateful)
        Color headerFace[4];     // N/H/P/D
        Color headerBorder[4];   // N/H/P/D
        Color titleInk[4];       // N/H/P/D
        Color subTitleInk[4];    // N/H/P/D
        Color badgeInk[4];       // N/H/P/D

        // Content (stateless)
        Color contentBg  = SColorPaper();
        Color contentInk = SColorText();

        // Misc
        Color underline  = GrayColor(160);
    };

    struct UiMetrics {
        Font  titleFont        = StdFont().Bold().Height(DPI(18));
        Font  subTitleFont     = StdFont().Height(DPI(9));
        Font  badgeFont        = StdFont().Height(DPI(18));
        int   padX             = DPI(4); // internal pad used by header text block
        int   padY             = DPI(4);
        int   headerGap        = DPI(4); // single line-spacing inside header
        int   titleUnderlineTh = 1;
        HeaderAlign badgeAlignDefault = RIGHT;
    };

    struct GlyphLook {
        Image  img;
        String text;
        Font   font;
        Color  ink = SColorText();
        bool IsEmpty() const { return img.IsEmpty() && text.IsEmpty(); }
    };

    struct Style {
        UiPalette palette;
        UiMetrics metrics;
        GlyphLook badgeGlyph; // default badge if not set on control

        // Optional painters (called when corresponding layer is enabled)
        Callback2<const StageCard&, Draw&> paintCardBg;
        Callback2<const StageCard&, Draw&> paintHeaderBg;
        Callback2<const StageCard&, Draw&> paintContentBg;
        Callback2<const StageCard&, Draw&> paintHeaderFrame;
        Callback2<const StageCard&, Draw&> paintContentFrame;
        Callback2<const StageCard&, Draw&> paintCardFrame;
        Callback2<const StageCard&, Draw&> paintTitles; // replaces default title/subtitle/underline
    };

    // ------- API: style -------
    StageCard& SetStyle(const Style& s);      // references external style (not owned)
    StageCard& SetStyleOwned(const Style& s); // stores a copy (owned)
    StageCard& SetPalette(const UiPalette& p);
    StageCard& SetMetrics(const UiMetrics& m);
    static const Style& StyleDefault();

    // Header dynamic state on/off
    StageCard& EnableHeaderState(bool on = true) { headerStateOn_ = on; Refresh(); return *this; }

    // ------- Lifecycle -------
    StageCard();

    // ---- Header text ----
    StageCard& SetTitle(const String& s)            { title = s; Layout(); return *this; }
    StageCard& SetTitleFont(Font f)                 { metrics_.titleFont = f; Layout(); return *this; }
    StageCard& SetSubTitle(const String& s)         { subTitle = s; Layout(); return *this; }
    StageCard& SetSubTitleFont(Font f)              { metrics_.subTitleFont = f; Layout(); return *this; }

    // ---- Badge (icon or centered text) ----
    StageCard& SetBadge(const String& s)            { badge = s; hasBadgeText = !IsNull(s); Layout(); return *this; }
    StageCard& SetBadgeFont(Font f)                 { metrics_.badgeFont = f; Layout(); return *this; }
    StageCard& SetBadgeIcon(const Image& img, Size pref = Size(0,0))
                                                    { badgeIcon = img; badgeIconPref = pref; hasBadgeIcon = !img.IsEmpty(); Layout(); return *this; }
    StageCard& SetBadgeAlignment(HeaderAlign a)     { badgeAlign = a; badgeAlignExplicit = true; Layout(); return *this; }

    // Title/subtitle horizontal alignment (does not move centered icon)
    StageCard& SetHeaderAlign(HeaderAlign a)        { headerAlign = a; Layout(); return *this; }

    // Underline
    StageCard& SetTitleUnderlineThickness(int th)   { metrics_.titleUnderlineTh = max(0, th); Layout(); return *this; }
    StageCard& SetTitleUnderlineColor(Color c)      { palette_.underline = c; Refresh(); return *this; }
    StageCard& SetTitleUnderlineVertical(bool on = true) { underlineVertical = on; Layout(); return *this; }

    // ---- Header colors (palette wiring) ----
    StageCard& SetHeaderColor(Color face_base, Color border_base);
    StageCard& SetHeaderColorState(Color fN, Color fH, Color fP, Color fD,
                                   Color bN, Color bH, Color bP, Color bD);

    // ---- Title / Subtitle / Badge inks ----
    StageCard& SetTitleColor(Color base, Color disabled = SColorDisabled());
    StageCard& SetSubTitleColor(Color base, Color disabled = SColorDisabled());
    StageCard& SetBadgeColor(Color base, Color disabled = SColorDisabled());

    StageCard& SetTitleColorState(Color n, Color h, Color p, Color d);
    StageCard& SetSubTitleColorState(Color n, Color h, Color p, Color d);
    StageCard& SetBadgeColorState(Color n, Color h, Color p, Color d);

    // ---- Content/Card colors (stateless) ----
    StageCard& SetContentColor(Color bg, Color ink = SColorText());
    StageCard& SetCardColors(Color fill = Color(240,240,240), Color stroke = Color(200,200,200));

    // ---- Frames & toggles ----
    StageCard& EnableCardFrame(bool on = true)           { cardFrameOn = on; Refresh(); return *this; }
    StageCard& EnableCardFill(bool on = true)            { cardFillOn  = on; Refresh(); return *this; }
    StageCard& SetCardCornerRadius(int px)               { cardRadius  = max(DPI(0), px); Refresh(); return *this; }
    StageCard& SetCardFrameThickness(int px)             { cardStrokeTh= max(0, px);     Refresh(); return *this; }
    StageCard& SetCardDashPattern(const String& d)       { cardDash    = d; Refresh(); return *this; }
    StageCard& EnableCardDash(bool on = false)           { cardDashed  = on; Refresh(); return *this; }

    StageCard& EnableHeaderFrame(bool on = false)        { headerFrameOn = on; Refresh(); return *this; }
    StageCard& EnableHeaderFill(bool on = true)          { headerFillOn  = on; Refresh(); return *this; }
    StageCard& SetHeaderCornerRadius(int px)             { headerRadius  = max(DPI(0), px); Refresh(); return *this; }
    StageCard& SetHeaderFrameThickness(int px)           { headerStrokeTh= max(0, px);     Refresh(); return *this; }
    StageCard& SetHeaderDashPattern(const String& d)     { headerDash    = d; Refresh(); return *this; }
    StageCard& EnableHeaderDash(bool on = false)         { headerDashed  = on; Refresh(); return *this; }

    StageCard& EnableContentFrame(bool on = false)       { contentFrameOn = on; Refresh(); return *this; }
    StageCard& EnableContentFill(bool on = true)         { contentFillOn  = on; Refresh(); return *this; }
    StageCard& SetContentCornerRadius(int px)            { contentRadius  = max(DPI(0), px); Refresh(); return *this; }
    StageCard& SetContentFrameThickness(int px)          { contentStrokeTh= max(0, px);     Refresh(); return *this; }
    StageCard& SetContentDashPattern(const String& d)    { contentDash    = d; Refresh(); return *this; }
    StageCard& EnableContentDash(bool on = false)        { contentDashed  = on; Refresh(); return *this; }

    // ---- Insets & gaps (Qt-like) ----
    StageCard& SetHeaderInset(int l, int t, int r, int b);
    StageCard& SetHeaderGap(int px);
    StageCard& SetCardGap(int px);

    // Content insets & gaps
    StageCard& SetContentInset(int l, int t, int r, int b);
    StageCard& SetContentInnerInset(int l, int t, int r, int b);
    StageCard& SetContentGap(int gx, int gy);

    // ---- Content behavior / sizing ----
    StageCard& EnableContentScroll(bool on = true)        { scrollEnabled = on; Layout(); return *this; }
    StageCard& EnableContentClampToPane(bool on = true)   { clampContentToPane = on; Layout(); return *this; }
    StageCard& SetMinContent(Size s)                      { minContent = s; Layout(); return *this; }
    StageCard& SetMaxContent(Size s)                      { maxContent = s; Layout(); return *this; }

    // ---- Content / layout modes ----

    // High-level API: main way to choose layout.
    StageCard& SetStack(StackMode m);

    // Convenience helpers
    StageCard& SetStackV()    { return SetStack(StackMode::STACKV); }
    StageCard& SetStackH()    { return SetStack(StackMode::STACKH); }
    StageCard& SetStackNone() { return SetStack(StackMode::NONE);  }

    // Wrapping only affects horizontal stack mode (STACKH).
    StageCard& SetWrap(bool on = true)                    { wrap = on; Layout(); return *this; }

    // ---- Stack API (Fixed / Expand / Spacer) ----
    StageCard& ReplaceExpand(Ctrl& c, int w = 1);
    StageCard& ReplaceFixed (Ctrl& c, int px, int py);
    StageCard& ReplaceFixed (Ctrl& c);

    StageCard& ClearContent();

    // px/py: fixed cell size; <=0 means "use control's natural size"
    StageCard& AddFixed (Ctrl& c, int px, int py);
    StageCard& AddFixed (Ctrl& c, int px);   // main-axis size
    StageCard& AddFixed (Ctrl& c);           // natural main-axis size
    StageCard& AddExpand(Ctrl& c, int w=1);  // participates in extra space distribution
    StageCard& AddSpacer(int weight=1);

    // Header children
    StageCard& AddHeader(Ctrl& c) { headerPane.Add(c); Layout(); return *this; }
    StageCard& ClearHeader();

    // ---- Hooks ----
    ParentCtrl& Header()  { return headerPane; }   // header band
    ParentCtrl& Content() { return contentLayer; } // scrolled layer (use this)

    Size GetMinSize() const override;
    void Layout() override;
    void Paint(Draw& w) override;
    void MouseWheel(Point p, int zdelta, dword keyflags) override;

    bool IsVerticalScroll() const { return (dir == Direction::V) || (mode == ContentMode::STACK && wrap); }
    bool IsWrap() const           { return mode == ContentMode::STACK && wrap && dir == Direction::H; }

private:
    // ---- Internal layout enums ----
    enum class Direction   { V, H };
    enum class ContentMode { STACK, MANUAL };

    // ---- Internal state index ----
    enum { ST_NORMAL = 0, ST_HOT = 1, ST_PRESSED = 2, ST_DISABLED = 3, ST_COUNT = 4 };

    // Header mouse sensor
    struct HeaderHitCtrl : ParentCtrl {
        HeaderHitCtrl(StageCard& o) : owner(o) { Transparent(); }
        void MouseMove(Point p, dword k) override     { owner.OnHeaderMouseMove(p, k); }
        void MouseEnter(Point p, dword k) override    { owner.OnHeaderMouseEnter(p, k); }
        void MouseLeave() override                    { owner.OnHeaderMouseLeave(); }
        void LeftDown(Point p, dword k) override      { owner.OnHeaderLeftDown(p, k); }
        void LeftUp(Point p, dword k) override        { owner.OnHeaderLeftUp(p, k); }
        void MouseWheel(Point p, int z, dword k) override { owner.MouseWheel(p, z, k); }
        StageCard& owner;
    };

    int  HeaderStateIndex() const;
    void OnHeaderMouseEnter(Point, dword);
    void OnHeaderMouseMove(Point, dword);
    void OnHeaderMouseLeave();
    void OnHeaderLeftDown(Point, dword);
    void OnHeaderLeftUp(Point, dword);

    // ---- Style state ----
    UiPalette   palette_;
    UiMetrics   metrics_;
    One<Style>  owned_style_;
    const Style* style_ref_ = nullptr;

    // header interaction
    bool headerStateOn_ = true;
    bool headerHot_     = false;
    bool headerDown_    = false;

    Rect  lastVBarRc;

    // text
    String  title, subTitle, badge;

    // computed header geometry
    int  titleX = 0, titleY = 0, titleW = 0;
    int  subTitleX = 0, subTitleY = 0, subTitleW = 0;
    int  titleLineY = 0;
    int  line1X = 0, line1W = 0;
    int  line2X = 0, line2W = 0;
    int  vLineX = 0, vLineY = 0, vLineH = 0;   // vertical underline geometry
    bool underlineVertical = false;            // false = horizontal (default), true = vertical

    // badge (icon+text overlay)
    Image   badgeIcon;
    Size    badgeIconPref = Size(0,0);
    bool    hasBadgeIcon  = false;
    bool    hasBadgeText  = false;
    HeaderAlign badgeAlign = RIGHT;
    bool    badgeAlignExplicit = false;
    Rect    badgeIconRc;

    // frames/toggles
    bool    cardFrameOn  = true;
    bool    cardFillOn   = true;
    int     cardRadius   = DPI(6);
    int     cardStrokeTh = 1;
    bool    cardDashed   = false;
    String  cardDash     = "5,5";

    bool    headerFrameOn  = false;
    bool    headerFillOn   = true;
    int     headerRadius   = DPI(0);
    int     headerStrokeTh = 1;
    bool    headerDashed   = false;
    String  headerDash     = "5,5";

    bool    contentFrameOn  = false;
    bool    contentFillOn   = false;
    int     contentRadius   = 0;
    int     contentStrokeTh = 1;
    bool    contentDashed   = false;
    String  contentDash     = "5,5";

    // behavior / sizing
    Rect    headerInset  = Rect(DPI(0), DPI(0), DPI(0), DPI(0)); // padding around header block
    int     cardGap      = DPI(6);                               // gap between header and content
    Rect    contentInset = Rect(DPI(0), DPI(0), DPI(0), DPI(0));
    Rect    contentInnerInset = Rect(DPI(0), DPI(0), DPI(0), DPI(0));
    Size    contentGap   = Size(DPI(6), DPI(6));
    HeaderAlign headerAlign = LEFT;
    int     badgeGapX    = DPI(8);

    // panes & scroll
    HeaderHitCtrl headerPane { *this };
    ParentCtrl    contentPane;
    ParentCtrl    contentLayer;
    ScrollBar     vbar;
    bool          scrollEnabled = true;
    int           scroll_y = 0;

    // modes
    ContentMode mode = ContentMode::STACK;
    Direction   dir  = Direction::H;
    bool        wrap = false;

    // size rules
    bool      clampContentToPane = true;
    Size      minContent = Size(0,0), maxContent = Size(0,0);

    // cached
    int       cachedHeaderMin = DPI(10);
    Rect      lastHeaderRc, lastContentRc;
    int       virtualLen = DPI(10);
    bool      contentDirty = true;

    // ---- Stack bookkeeping ----
    enum class ItemKind { CtrlItem, Spacer };
    struct Item : Moveable<Item> {
        ItemKind kind = ItemKind::CtrlItem;
        Ctrl*    c    = nullptr;
        int      fixed_px = -1; // main-axis fixed size (non-wrap)
        int      fixed_w  = -1; // explicit width  (wrap)
        int      fixed_h  = -1; // explicit height (wrap)
        int      expand_w = 0;  // >0 == participates in expand distribution
        Rect     rect;
    };
    Vector<Item> items;

    // helpers
    static void ClearChildren(ParentCtrl& p);
    void RebuildItemsFromChildrenIfNeeded();
    int  MeasureNaturalPrimaryForWidth(int pane_w) const;
    int  MeasureNaturalPrimaryForHeight(int pane_h) const;
    void LayoutStackV(const Rect& inner);
    void LayoutStackH(const Rect& inner);
    void LayoutWrapH (const Rect& inner);
    int  HeaderHeight() const;

    // style helpers
    HeaderAlign EffectiveBadgeAlign() const;
    Rect        EffectiveContentInset() const;
    void        DrawBadgeGlyph(Draw& w, const Rect& rc) const;
};

} // namespace Upp
#endif
