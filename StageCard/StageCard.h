#ifndef _StageCard_StageCard_h_
#define _StageCard_StageCard_h_

#include <CtrlLib/CtrlLib.h>
#include <Painter/Painter.h>

namespace Upp {

/*
===============================================================================
StageCard
-------------------------------------------------------------------------------
A self-contained “card” control with:

  • A HEADER area (title, subtitle, optional badge + optional header controls)
  • A CONTENT area (your app's controls) with an integrated vertical scrollbar

Goals
-----
- Drop-in container that looks like a modern card and “just scrolls” when the
  content is taller than the available area.
- Make common layouts trivial (absolute/owner-managed, flow/WRAP, simple GRID).
- Provide clear separation of *decor* (card/header/content frames) and *data*
  (your controls placed in the content layer).

Common Usage
------------
    StageCard card;
    card.SetTitle("Orders").SetSubTitle("Last 30 days")
        .EnableCardFrame()
        .EnableContentScroll(true)
        .ContentWrap().WrapItemSize(DPI(160), DPI(80));

    FlowBoxLayout& grid = ...; // e.g. your layout control
    card.AddContent(grid);

Key Concepts
------------
- headerPane    : a child ParentCtrl that can host *your* header widgets (filters, etc.)
- contentLayer  : where you add your actual content controls
- contentPane   : clips content; card manages a vertical ScrollBar against this pane
- autoFill (FIXED mode):
      * false -> you position children yourself; card only computes virtual height
      * true  -> card splits available height between visible children (evenly),
                 still enabling scrolling when natural height > pane height
===============================================================================
*/

class StageCard : public ParentCtrl {
public:
    typedef StageCard CLASSNAME;

    // Title/badge alignment choices inside the header text block.
    enum HeaderAlign   { LEFT, RIGHT, CENTER };

    // Built-in content layout modes (pick one):
    //   FIXED : absolute/owner-managed layout; optional autoFill to distribute height
    //   WRAP  : simple horizontal flow with wrapping (uses WrapItemSize as base tile)
    //   GRID  : columnar grid; fixed or stretchable cells
    enum ContentLayout { FIXED, WRAP, GRID };

    StageCard();

    //----------------------------------------------------------------------------
    // Header text/badge (all trigger a Layout so geometry updates immediately)
    //----------------------------------------------------------------------------
    StageCard& SetTitle(const String& s)                         { title = s;  Layout(); return *this; }
    StageCard& SetTitleFont(Font f = StdFont().Bold().Height(DPI(18)))
                                                                  { titleFont = f; Layout(); return *this; }
    StageCard& SetSubTitle(const String& s)                      { subTitle = s;  Layout(); return *this; }
    StageCard& SetSubTitleFont(Font f = StdFont().Height(DPI(10)))
                                                                  { subTitleFont = f; Layout(); return *this; }
    StageCard& SetBadge(const String& s)                         { badge = s; Layout(); return *this; }
    StageCard& SetBadgeFont(Font f = StdFont().Height(DPI(18)))  { badgeFont = f; Layout(); return *this; }

    // Where to align the header title block (also affects subtitle; badge mirrors).
    StageCard& SetHeaderAlign(HeaderAlign a)                     { headerAlign = a; Layout(); return *this; }

    // Optional title underline: thickness=0 disables. Color is independent.
    StageCard& SetTitleUnderlineThickness(int th)                { titleLineTh = max(0, th); Layout(); return *this; }
    StageCard& SetTitleUnderlineColor(Color c)                   { titleLineColor = c; Refresh(); return *this; }

    //----------------------------------------------------------------------------
    // Card (outer) frame styling
    //----------------------------------------------------------------------------
    // Enable/disable the outer card stroke/fill. Use this to blend into a parent.
    StageCard& EnableCardFrame(bool on = true)                   { cardFrameOn = on; Refresh(); return *this; }
    StageCard& EnableCardFill(bool on = true)                    { cardFillOn  = on; Refresh(); return *this; }

    // Corner radius & stroke for the outer card.
    StageCard& SetCardCornerRadius(int px)                       { cardRadius  = max(DPI(0), px); Refresh(); return *this; }
    StageCard& SetCardStrokeThickness(int px)                    { cardStrokeTh= max(0, px);     Refresh(); return *this; }
    StageCard& SetCardDashPattern(const String& d)               { cardDash    = d; Refresh(); return *this; }
    StageCard& EnableCardDashed(bool on = true)                  { cardDashed  = on; Refresh(); return *this; }

    // Unified color setter for the outer card and header text inks.
    StageCard& SetCardColors(Color fill = Color(240,240,240),
                             Color stroke = Color(200,200,200),
                             Color tInk = SColorText(),
                             Color stInk = SColorText(),
                             Color badgeInkC = SColorDisabled())
    { cardFill = fill; cardStroke = stroke; titleInk=tInk; subTitleInk=stInk; badgeInk=badgeInkC; Refresh(); return *this; }

    //----------------------------------------------------------------------------
    // Header frame styling (rectangle behind header text/controls)
    //----------------------------------------------------------------------------
    StageCard& EnableHeaderFrame(bool on = true)                 { headerFrameOn = on; Refresh(); return *this; }
    StageCard& EnableHeaderFill(bool on = true)                  { headerFillOn  = on; Refresh(); return *this; }
    StageCard& SetHeaderCornerRadius(int px)                     { headerRadius  = max(DPI(0), px); Refresh(); return *this; }
    StageCard& SetHeaderStrokeThickness(int px)                  { headerStrokeTh= max(0, px);     Refresh(); return *this; }
    StageCard& SetHeaderDashPattern(const String& d)             { headerDash    = d; Refresh(); return *this; }
    StageCard& EnableHeaderDashed(bool on = false)               { headerDashed  = on; Refresh(); return *this; }
    StageCard& SetHeaderFrameColors(Color fill, Color stroke)    { headerFill = fill; headerStroke = stroke; Refresh(); return *this; }

    //----------------------------------------------------------------------------
    // Content frame styling (rectangle behind content area)
    //----------------------------------------------------------------------------
    StageCard& EnableContentFrame(bool on = false)               { contentFrameOn = on; Refresh(); return *this; }
    StageCard& EnableContentFill(bool on = true)                 { contentFillOn  = on; Refresh(); return *this; }
    StageCard& SetContentCornerRadius(int px)                    { contentRadius  = max(DPI(0), px); Refresh(); return *this; }
    StageCard& SetContentStrokeThickness(int px)                 { contentStrokeTh= max(0, px);     Refresh(); return *this; }
    StageCard& SetContentDashPattern(const String& d)            { contentDash    = d; Refresh(); return *this; }
    StageCard& EnableContentDashed(bool on = false)              { contentDashed  = on; Refresh(); return *this; }
    StageCard& SetContentFrameColors(Color fill, Color stroke)   { contentFill = fill; contentStroke = stroke; Refresh(); return *this; }

    //----------------------------------------------------------------------------
    // Insets & gaps
    //----------------------------------------------------------------------------
    // Convenience: set the same inset on header+content at once.
    StageCard& SetInset(int all)                                  { SetHeaderInset(all,all,all,all); SetContentInset(all,all,all,all); return *this; }
    // Convenience: set per-edge inset on both header and content.
    StageCard& SetInset(int l, int t, int r, int b)               { SetHeaderInset(l,t,r,b); SetContentInset(l,t,r,b); return *this; }

    // Header block padding and vertical gap between text lines/underline.
    StageCard& SetHeaderInset(int l, int t, int r, int b)         { headerInset = Rect(max(0,l), max(0,t), max(0,r), max(0,b)); Layout(); return *this; }
    StageCard& SetHeaderGap(int v)                                { headerGapV = max(0, v); Layout(); return *this; }

    // Content block padding and inter-control gap (used by WRAP/GRID and by
    // FIXED+autoFill when distributing height across visible children).
    StageCard& SetContentInset(int l, int t, int r, int b)        { contentInset = Rect(max(0,l), max(0,t), max(0,r), max(0,b)); Layout(); return *this; }
    StageCard& SetContentGap(int gx, int gy)                      { contentGap = Size(max(0,gx), max(0,gy)); Layout(); return *this; }

    // Convenience: sets both headerGapV and content gap uniformly.
    StageCard& SetGap(int g)                                      { headerGapV = max(0,g); contentGap = Size(max(0,g), max(0,g)); Layout(); return *this; }

    //----------------------------------------------------------------------------
    // Child management
    //----------------------------------------------------------------------------
    // Add/clear your own controls inside the header (e.g., buttons, filters).
    StageCard& AddHeader(Ctrl& c)                                 { headerPane.Add(c); Layout(); return *this; }
    StageCard& ClearHeader()                                      { ClearChildren(headerPane); Layout(); return *this; }

    // Add/clear/replace content controls. Add to *contentLayer* (not contentPane).
    StageCard& AddContent(Ctrl& c)                                { contentLayer.Add(c); contentDirty = true; Layout(); return *this; }
    StageCard& ClearContent()                                     { ClearChildren(contentLayer); contentDirty = true; Layout(); return *this; }
    StageCard& ReplaceContent(Ctrl& c)                            { ClearChildren(contentLayer); contentLayer.Add(c); contentDirty = true; Layout(); return *this; }

    //----------------------------------------------------------------------------
    // Behavior toggles that influence scrolling and sizing
    //----------------------------------------------------------------------------
    // FIXED mode helper: when true, the card evenly distributes available height
    // across visible children; if children need more, the scrollbar appears.
    StageCard& EnableContentAutoFill(bool on = true)              { autoFill = on; Layout(); return *this; }

    // Turn the built-in vertical scrollbar on/off.
    StageCard& EnableContentScroll(bool on = true)                { scrollEnabled = on; Layout(); return *this; }

    // If true, the content pane’s height is clamped to the visible area (usual).
    // If false, min/max content sizes below can grow/shrink the pane before
    // scrolling is considered (useful for “tall” details cards).
    StageCard& EnableContentClampToPane(bool on = true)           { clampContentToPane = on; Layout(); return *this; }

    // Optional vertical size constraints for the content pane (effective when
    // clamp is disabled). Leave at (0,0) to ignore.
    StageCard& SetMinContent(Size s)                              { minContent = s; Layout(); return *this; }
    StageCard& SetMaxContent(Size s)                              { maxContent = s; Layout(); return *this; }

    //----------------------------------------------------------------------------
    // Content layout modes (pick one)
    //----------------------------------------------------------------------------
    StageCard& ContentAbsolute()                                   { contentLayout = FIXED; Layout(); return *this; } // owner manages child rects (or use autoFill)
    StageCard& ContentWrap()                                       { contentLayout = WRAP;  Layout(); return *this; } // simple horizontal flow with wrapping
    StageCard& ContentGrid()                                       { contentLayout = GRID;  Layout(); return *this; } // columnar grid

    // WRAP mode base item size (tile width/height). If not set, child min-size is used.
    StageCard& WrapItemSize(int w, int h)                          { wrapItemW=w; wrapItemH=h; Layout(); return *this; }
    StageCard& WrapItemSize(Size s)                                { wrapItemW=s.cx; wrapItemH=s.cy; Layout(); return *this; }

    // GRID mode knobs.
    StageCard& GridCols(int n)                                     { gridCols = max(1, n); Layout(); return *this; }
    StageCard& GridCell(int w, int h)                              { gridCellW=w; gridCellH=h; Layout(); return *this; }
    // Stretch: child rect == cell rect; otherwise child is centered within the cell.
    StageCard& GridStretch(bool on = false)                        { gridStretch=on; Layout(); return *this; }
    // Fill: compute cell size from pane to fill all columns/rows; otherwise use fixed cell size.
    StageCard& GridFill(bool on = true)                            { gridFill = on; Layout(); return *this; }

    //----------------------------------------------------------------------------
    // Size & layout hooks
    //----------------------------------------------------------------------------
    Size GetMinSize() const override;      // conservative height; ensures header is visible
    void Layout() override;                // header metrics, content/scroll negotiation
    void Paint(Draw& w) override;          // three layers: card, header, content

    // Mouse wheel is forwarded to the integrated scrollbar (when shown).
    void MouseWheel(Point p, int zdelta, dword keyflags) override;

private:
    // --- Public state (text/inks/metrics) ------------------------------------
    String  title, subTitle, badge;
    Font    titleFont   = StdFont().Bold().Height(DPI(18));
    Font    subTitleFont= StdFont().Height(DPI(10));
    Font    badgeFont   = StdFont().Height(DPI(18));
    HeaderAlign headerAlign = LEFT;
    Color   titleInk = SColorText();
    Color   subTitleInk = SColorText();
    Color   badgeInk = SColorDisabled();
    int     titleLineTh = 1;                 // 0 => no underline
    int     titleLineX=0, titleLineY=0, titleLineW=0;
    Color   titleLineColor = GrayColor(160);

    // --- Card frame styling ---------------------------------------------------
    bool    cardFrameOn  = true;
    bool    cardFillOn   = true;
    int     cardRadius   = DPI(6);
    int     cardStrokeTh = 1;
    bool    cardDashed   = false;
    String  cardDash     = "5,5";
    Color   cardFill     = Color(240,240,240);
    Color   cardStroke   = SColorShadow();

    // --- Header frame styling -------------------------------------------------
    bool    headerFrameOn  = false;
    bool    headerFillOn   = false;
    int     headerRadius   = DPI(0);
    int     headerStrokeTh = 1;
    bool    headerDashed   = false;
    String  headerDash     = "5,5";
    Color   headerFill     = Color(255,255,255);
    Color   headerStroke   = GrayColor(160);

    // --- Content frame styling ------------------------------------------------
    bool    contentFrameOn  = false;
    bool    contentFillOn   = false;
    int     contentRadius   = DPI(0);
    int     contentStrokeTh = 1;
    bool    contentDashed   = false;
    String  contentDash     = "5,5";
    Color   contentFill     = Color(255,255,255);
    Color   contentStroke   = GrayColor(160);

    // --- Behavior toggles -----------------------------------------------------
    bool    autoFill = false;               // FIXED mode helper
    int     cardPadX = DPI(0), cardPadY = DPI(0);

    // Insets and gaps
    Rect    headerInset  = Rect(DPI(6), DPI(6), DPI(6), DPI(6));
    int     headerGapV   = DPI(6);
    Rect    contentInset = Rect(DPI(6), DPI(6), DPI(6), DPI(6));
    Size    contentGap   = Size(DPI(6), DPI(6));

    // --- Internal panes -------------------------------------------------------
    ParentCtrl headerPane;                  // user header widgets live here
    ParentCtrl contentPane;                 // viewport that clips content
    ParentCtrl contentLayer;                // user content lives here (scrolls)

    // --- Scroll state ---------------------------------------------------------
    ScrollBar  vbar;
    bool       scrollEnabled = true;
    int        scroll_y = 0;                // current top offset in contentLayer

    // --- Content layout mode + params ----------------------------------------
    ContentLayout contentLayout = FIXED;
    int       wrapItemW = 0, wrapItemH = 0; // WRAP base size (fallback to child min)
    int       gridCols   = 3, gridCellW  = 0, gridCellH  = 0; // GRID params
    bool      gridStretch= true;
    bool      gridFill = false;

    // --- Pane size rules ------------------------------------------------------
    bool      clampContentToPane = true;    // true: pane_h is viewport height
    Size      minContent = Size(0,0), maxContent = Size(0,0);

    // --- Cached layout metrics ------------------------------------------------
    int       cachedHeaderMin = DPI(10);
    Rect      lastHeaderRc, lastContentRc;
    int       titleX=0, titleY=0, titleW=0;
    int       subTitleX=0, subTitleY=0, subTitleW=0;
    int       badgeX=0, badgeY=0, badgeW=0;

    int       virtualH = DPI(10);           // total natural height of contentLayer
    bool      contentDirty = true;

    // --- helpers --------------------------------------------------------------
    static void ClearChildren(ParentCtrl& p);              // remove all children
    Size WrapBaseSize(Ctrl& c) const;                      // WRAP base tile size
    void ContentLayoutPass(int pane_w, int pane_h);        // computes virtualH & child rects

    // Painting helpers (card/header/content layers)
    void PaintCardLayer(BufferPainter& p, const Size& sz) const;
    void PaintHeaderLayer(BufferPainter& p) const;
    void PaintContentLayer(BufferPainter& p) const;
};

} // namespace Upp
#endif
