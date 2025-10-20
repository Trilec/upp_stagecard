#ifndef _StageCard_StageCard_h_
#define _StageCard_StageCard_h_

// ============================================================================
// StageCard — Minimal, flexible "card" container for U++
// ----------------------------------------------------------------------------
// A StageCard renders three independent layers:
//   • Card  frame/fill  (outer container)
//   • Header frame/fill (top band)
//   • Content frame/fill (visible content pane)
//
// Below the header there is a scrollable content pane. Children are not added
// directly to the pane; they are added to an internal `contentLayer` that can
// be moved vertically to implement scrolling.
//
// Layout modes for the content layer:
//   • FIXED : honors children's own XY/WH; virtual height = bottom-most child
//   • WRAP  : flow/wrap rows with gaps/insets; optional fixed item size
//   • GRID  : N columns; fixed/auto cell; optional stretch-to-cell
//
// Smart height splitter for FIXED: `EnableContentAutoFill(true)`.
//   • If there is 1 child → it fills the content pane
//   • If there are N>=2 children → they are divided vertically into equal bands
//     (separated by content gaps) that exactly fill the content pane
//
// Scrolling:
//   • `EnableContentScroll(true)` turns on a vertical scrollbar that appears
//     only when virtual content height exceeds the visible content pane.
//
// Typical usage:
//
//     StageCard card;
//     card.SetTitle("Demo")
//         .EnableCardFrame(true)
//         .EnableContentScroll(true)
//         .ContentWrap()
//         .SetContentInset(DPI(8), DPI(8), DPI(8), DPI(8))
//         .SetContentGap(DPI(6), DPI(6));
//     card.AddContent(my_button);
//     parent.Add(card.HSizePos(8,8).VSizePos(8,8));
//
// ============================================================================

#include <CtrlLib/CtrlLib.h>
#include <Painter/Painter.h>

namespace Upp {

class StageCard : public ParentCtrl {
public:
    typedef StageCard CLASSNAME;

    // ----------- enums -----------
    enum HeaderAlign   { LEFT, RIGHT, CENTER };
    enum ContentLayout { FIXED, WRAP, GRID };

    // ----------- ctor ------------
    StageCard() {
        Add(headerPane);
        Add(contentPane);
        contentPane.Add(contentLayer);

        Add(vbar);
        vbar.Hide();
        vbar.SetTotal(0);
        vbar.SetLine(DPI(16));
        vbar.SetPage(0);
        vbar.WhenScroll = [=] {
            scroll_y = vbar.Get();
            const Rect pr = contentPane.GetRect();
            contentLayer.SetRect(0, -scroll_y, pr.GetWidth(), contentLayer.GetRect().GetHeight());
            contentLayer.Refresh();
        };
    }

    // ==============================
    // Header text & style
    // ==============================
    StageCard& SetTitle(const String& s, Font f = StdFont().Bold().Height(DPI(22)))
        { title = s; titleFont = f; Layout(); return *this; }
    StageCard& SetSubTitle(const String& s, Font f = StdFont().Height(DPI(12)))
        { subTitle = s; subTitleFont = f; Layout(); return *this; }
    StageCard& SetBadge(const String& s, Font f = StdFont().Height(DPI(12)))
        { badge = s; badgeFont = f; Layout(); return *this; }
    StageCard& SetHeaderAlign(HeaderAlign a)
        { headerAlign = a; Layout(); return *this; }
    StageCard& SetTitleUnderlineThickness(int th)
        { titleLineTh = max(0, th); Layout(); return *this; }
    StageCard& SetTitleUnderlineColor(Color c)
        { titleLineColor = c; Refresh(); return *this; }

    // ==============================
    // Frame toggles & colors
    // ==============================
    // Card (outer)
    StageCard& EnableCardFrame(bool on = true)   { cardFrameOn = on; Refresh(); return *this; }
    StageCard& EnableCardFill(bool on = true)    { cardFillOn  = on; Refresh(); return *this; }
    StageCard& SetCardCornerRadius(int px)       { cardRadius  = max(DPI(0), px); Refresh(); return *this; }
    StageCard& SetCardStrokeThickness(int px)    { cardStrokeTh= max(0, px); Refresh(); return *this; }
    StageCard& SetCardDashPattern(const String& d){ cardDash    = d; Refresh(); return *this; }
    StageCard& EnableCardDashed(bool on = true)  { cardDashed  = on; Refresh(); return *this; }
    StageCard& SetCardColors(Color fill, Color stroke,
                             Color tInk = SColorText(),
                             Color stInk = SColorText(),
                             Color badgeInkC = SColorDisabled())
    { cardFill = fill; cardStroke = stroke; titleInk=tInk; subTitleInk=stInk; badgeInk=badgeInkC; Refresh(); return *this; }

    // Header
    StageCard& EnableHeaderFrame(bool on = true)   { headerFrameOn = on; Refresh(); return *this; }
    StageCard& EnableHeaderFill(bool on = true)    { headerFillOn  = on; Refresh(); return *this; }
    StageCard& SetHeaderCornerRadius(int px)       { headerRadius  = max(DPI(0), px); Refresh(); return *this; }
    StageCard& SetHeaderStrokeThickness(int px)    { headerStrokeTh= max(0, px); Refresh(); return *this; }
    StageCard& SetHeaderDashPattern(const String& d){ headerDash    = d; Refresh(); return *this; }
    StageCard& EnableHeaderDashed(bool on = true)  { headerDashed  = on; Refresh(); return *this; }
    StageCard& SetHeaderFrameColors(Color fill, Color stroke)
    { headerFill = fill; headerStroke = stroke; Refresh(); return *this; }

    // Content
    StageCard& EnableContentFrame(bool on = true)   { contentFrameOn = on; Refresh(); return *this; }
    StageCard& EnableContentFill(bool on = true)    { contentFillOn  = on; Refresh(); return *this; }
    StageCard& SetContentCornerRadius(int px)       { contentRadius  = max(DPI(0), px); Refresh(); return *this; }
    StageCard& SetContentStrokeThickness(int px)    { contentStrokeTh= max(0, px); Refresh(); return *this; }
    StageCard& SetContentDashPattern(const String& d){ contentDash    = d; Refresh(); return *this; }
    StageCard& EnableContentDashed(bool on = true)  { contentDashed  = on; Refresh(); return *this; }
    StageCard& SetContentFrameColors(Color fill, Color stroke)
    { contentFill = fill; contentStroke = stroke; Refresh(); return *this; }

    // ==============================
    // Insets & gaps
    // ==============================
    StageCard& SetInset(int all)                 { SetHeaderInset(all,all,all,all); SetContentInset(all,all,all,all); return *this; }
    StageCard& SetInset(int l, int t, int r, int b) { SetHeaderInset(l,t,r,b); SetContentInset(l,t,r,b); return *this; }
    StageCard& SetHeaderInset(int l, int t, int r, int b)
        { headerInset = Rect(max(0,l), max(0,t), max(0,r), max(0,b)); Layout(); return *this; }
    StageCard& SetHeaderGap(int v)               { headerGapV = max(0, v); Layout(); return *this; }
    StageCard& SetContentInset(int l, int t, int r, int b)
        { contentInset = Rect(max(0,l), max(0,t), max(0,r), max(0,b)); Layout(); return *this; }
    StageCard& SetContentGap(int gx, int gy)     { contentGap = Size(max(0,gx), max(0,gy)); Layout(); return *this; }
    StageCard& SetGap(int g)                     { headerGapV = max(0,g); contentGap = Size(max(0,g), max(0,g)); Layout(); return *this; }

    // ==============================
    // Child management
    // ==============================
    StageCard& AddHeader(Ctrl& c)       { headerPane.Add(c); Layout(); return *this; }
    StageCard& ClearHeader()            { ClearChildren(headerPane); Layout(); return *this; }
    ParentCtrl& HeaderPane()            { return headerPane; }

    StageCard& AddContent(Ctrl& c)      { contentLayer.Add(c); contentDirty = true; Layout(); return *this; }
    StageCard& ClearContent()           { ClearChildren(contentLayer); contentDirty = true; Layout(); return *this; }
    StageCard& ReplaceContent(Ctrl& c)  { ClearChildren(contentLayer); contentLayer.Add(c); contentDirty = true; Layout(); return *this; }

    // ==============================
    // Behavior toggles
    // ==============================
    StageCard& EnableContentAutoFill(bool on = true) { autoFill = on; Layout(); return *this; }
    StageCard& EnableContentScroll(bool on = true)   { scrollEnabled = on; Layout(); return *this; }
    StageCard& EnableContentClampToPane(bool on = true) { clampContentToPane = on; Layout(); return *this; }
    StageCard& SetMinContent(Size s) { minContent = s; Layout(); return *this; }
    StageCard& SetMaxContent(Size s) { maxContent = s; Layout(); return *this; }

    // ==============================
    // Content layout modes
    // ==============================
    StageCard& ContentAbsolute() { contentLayout = FIXED; Layout(); return *this; }
    StageCard& ContentWrap()     { contentLayout = WRAP;  Layout(); return *this; }
    StageCard& ContentGrid()     { contentLayout = GRID;  Layout(); return *this; }
    StageCard& WrapItemSize(int w, int h) { wrapItemW=w; wrapItemH=h; Layout(); return *this; }
    StageCard& WrapItemSize(Size s)       { return WrapItemSize(s.cx, s.cy); }
    StageCard& GridCols(int n)            { gridCols = max(1, n); Layout(); return *this; }
    StageCard& GridCell(int w, int h)     { gridCellW=w; gridCellH=h; Layout(); return *this; }
    StageCard& GridCell(Size s)           { return GridCell(s.cx, s.cy); }
    StageCard& GridStretch(bool on = true){ gridStretch=on; Layout(); return *this; }
    StageCard& GridFill(bool on = true) { gridFill = on; Layout(); return *this; }

    // ==============================
    // Size promise
    // ==============================
    virtual Size GetMinSize() const override {
        if(clampContentToPane)
            return Size(DPI(10), max(cachedHeaderMin, DPI(10)));
        int content_min = minContent.cy > 0 ? minContent.cy : DPI(10);
        return Size(DPI(10), max(cachedHeaderMin + content_min, DPI(10)));
    }

    // ==============================
    // Layout pass
    // ==============================
    virtual void Layout() override {
        const Size sz = GetSize();
        Rect outer = Rect(sz).Deflated(cardPadX, cardPadY);

        const int header_w_inner = max(0, outer.GetWidth() - headerInset.left - headerInset.right);
        int y = outer.top + headerInset.top;

        // --- title line / subtitle / badge metrics
        titleX = titleY = 0; titleW = 0;
        if(!IsNull(title)) {
            Size ts = GetTextSize(title, titleFont);
            titleW = ts.cx;
            int tx = outer.left + headerInset.left;
            if(headerAlign == RIGHT)       tx = outer.right - headerInset.right - ts.cx;
            else if(headerAlign == CENTER) tx = outer.left + headerInset.left + (header_w_inner - ts.cx)/2;
            titleX = tx; titleY = y;
            y += ts.cy;
            if(titleLineTh > 0) { y += headerGapV; titleLineX = tx; titleLineY = y; titleLineW = ts.cx; y += titleLineTh; }
            y += headerGapV;
        } else { titleLineX = titleLineY = titleLineW = 0; }

        subTitleX = subTitleY = subTitleW = 0;
        if(!IsNull(subTitle)) {
            Size ss = GetTextSize(subTitle, subTitleFont);
            int sx = outer.left + headerInset.left;
            if(headerAlign == RIGHT)       sx = outer.right - headerInset.right - ss.cx;
            else if(headerAlign == CENTER) sx = outer.left + headerInset.left + (header_w_inner - ss.cx)/2;
            subTitleX = sx; subTitleY = y; subTitleW = ss.cx;
            y += ss.cy;
            y += headerGapV;
        }

        badgeX = badgeY = 0; badgeW = 0;
        if(!IsNull(badge)) {
            Size gs = GetTextSize(badge, badgeFont);
            badgeW = gs.cx;
            badgeX = (headerAlign == RIGHT) ? (outer.left + headerInset.left)
                                            : (outer.right - headerInset.right - gs.cx);
            badgeY = outer.top + headerInset.top;
        }

        int header_h_text = y - outer.top + headerInset.bottom;

        // header children
        int header_child_bottom = 0;
        for(Ctrl *q = headerPane.GetFirstChild(); q; q = q->GetNext())
            header_child_bottom = max(header_child_bottom, q->GetRect().bottom);
        int header_h = max(header_h_text, headerInset.top + header_child_bottom + headerInset.bottom);
        cachedHeaderMin = max(header_h, DPI(10));

        lastHeaderRc = Rect(outer.left + headerInset.left,
                            outer.top  + headerInset.top,
                            outer.right - headerInset.right,
                            outer.top  + header_h - headerInset.bottom);
        headerPane.SetRect(lastHeaderRc);

        // content pane rect
        Rect pane_rc = Rect(outer.left, outer.top + header_h, outer.right, outer.bottom);
        if(!clampContentToPane) {
            if(minContent.cy > 0 && pane_rc.GetHeight() < minContent.cy)
                pane_rc.bottom = pane_rc.top + minContent.cy;
            if(maxContent.cy > 0 && pane_rc.GetHeight() > maxContent.cy)
                pane_rc.bottom = pane_rc.top + maxContent.cy;
        }

        // run content layout
        const int sbw = DPI(14);
        int pane_w_no_sb = pane_rc.GetWidth();
        int pane_h       = max(0, pane_rc.GetHeight());

        ContentLayoutPass(pane_w_no_sb, pane_h);

        bool overflow = (virtualH > pane_h);
        bool show_sb  = scrollEnabled && overflow;
        int pane_w    = show_sb ? (pane_w_no_sb - sbw) : pane_w_no_sb;

        if(show_sb && (contentLayout == WRAP || contentLayout == GRID))
            ContentLayoutPass(pane_w, pane_h);

        contentPane.SetRect(pane_rc.left, pane_rc.top, pane_w, pane_h);
        contentLayer.SetRect(0, -scroll_y, pane_w, virtualH);

        if(show_sb) {
            vbar.Show();
            vbar.LeftPos(pane_rc.right - sbw, sbw).TopPos(pane_rc.top, pane_h);
            vbar.SetTotal(virtualH);
            vbar.SetPage(pane_h);
            const int maxpos = max(0, virtualH - pane_h);
            scroll_y = clamp(scroll_y, 0, maxpos);
            vbar.Set(scroll_y);
        } else {
            vbar.Hide();
            scroll_y = 0;
        }

        lastContentRc = Rect(pane_rc.left, pane_rc.top, pane_rc.left + pane_w, pane_rc.top + pane_h);
        Refresh();
    }

    // ==============================
    // Paint
    // ==============================
    virtual void Paint(Draw& w) override {
        const Size sz = GetSize();
        if(sz.cx <= 0 || sz.cy <= 0) return;

        ImageBuffer ib(sz);
        Fill(~ib, RGBAZero(), ib.GetLength());
        BufferPainter p(ib);

        // Card frame/fill
        if(cardFillOn || (cardFrameOn && cardStrokeTh > 0)) {
            p.Begin();
            (cardRadius > 0 ? p.RoundedRectangle(0.5, 0.5, sz.cx - 1.0, sz.cy - 1.0, cardRadius)
                            : p.Rectangle(0.5, 0.5, sz.cx - 1.0, sz.cy - 1.0));
            if(cardFillOn) p.Fill(cardFill);
            if(cardFrameOn && cardStrokeTh > 0) {
                if(cardDashed) p.Dash(cardDash, 0.0);
                p.Stroke(cardStrokeTh, cardStroke);
            }
            p.End();
        }

        // Header frame/fill
        if(!lastHeaderRc.IsEmpty() && (headerFillOn || (headerFrameOn && headerStrokeTh > 0))) {
            p.Begin();
            if(headerRadius > 0)
                p.RoundedRectangle(lastHeaderRc.left, lastHeaderRc.top,
                                   lastHeaderRc.GetWidth(), lastHeaderRc.GetHeight(), headerRadius);
            else
                p.Rectangle(lastHeaderRc.left, lastHeaderRc.top,
                            lastHeaderRc.GetWidth(), lastHeaderRc.GetHeight());
            if(headerFillOn) p.Fill(headerFill);
            if(headerFrameOn && headerStrokeTh > 0) {
                if(headerDashed) p.Dash(headerDash, 0.0);
                p.Stroke(headerStrokeTh, headerStroke);
            }
            p.End();
        }

        // Content frame/fill
        if(!lastContentRc.IsEmpty() && (contentFillOn || (contentFrameOn && contentStrokeTh > 0))) {
            p.Begin();
            if(contentRadius > 0)
                p.RoundedRectangle(lastContentRc.left, lastContentRc.top,
                                   lastContentRc.GetWidth(), lastContentRc.GetHeight(), contentRadius);
            else
                p.Rectangle(lastContentRc.left, lastContentRc.top,
                            lastContentRc.GetWidth(), lastContentRc.GetHeight());
            if(contentFillOn) p.Fill(contentFill);
            if(contentFrameOn && contentStrokeTh > 0) {
                if(contentDashed) p.Dash(contentDash, 0.0);
                p.Stroke(contentStrokeTh, contentStroke);
            }
            p.End();
        }

        // Header text
        if(!IsNull(title))    p.DrawText(titleX, titleY, title, titleFont, titleInk);
        if(titleLineTh > 0 && !IsNull(title))
            p.DrawRect(titleLineX, titleLineY, titleLineW, titleLineTh, titleLineColor);
        if(!IsNull(subTitle)) p.DrawText(subTitleX, subTitleY, subTitle, subTitleFont, subTitleInk);
        if(!IsNull(badge))    p.DrawText(badgeX,    badgeY,    badge,    badgeFont,    badgeInk);

        w.DrawImage(0, 0, ib);
    }

private:
    // ==============================
    // State (defaults)
    // ==============================
    // header text
    String  title, subTitle, badge;
    Font    titleFont   = StdFont().Bold().Height(DPI(18));
    Font    subTitleFont= StdFont().Height(DPI(10));
    Font    badgeFont   = StdFont().Height(DPI(10));
    HeaderAlign headerAlign = LEFT;
    Color   titleInk = SColorText();
    Color   subTitleInk = SColorText();
    Color   badgeInk = SColorDisabled();
    int     titleLineTh = 0;
    int     titleLineX=0, titleLineY=0, titleLineW=0;
    Color   titleLineColor = GrayColor(160);

    // card frame defaults
    bool    cardFrameOn  = true;
    bool    cardFillOn   = true;
    int     cardRadius   = DPI(6);
    int     cardStrokeTh = 1;
    bool    cardDashed   = false;
    String  cardDash     = "5,5";
    Color   cardFill     = Color(245,245,245);
    Color   cardStroke   = SColorShadow();

    // header frame defaults
    bool    headerFrameOn  = false;
    bool    headerFillOn   = false;
    int     headerRadius   = DPI(0);
    int     headerStrokeTh = 1;
    bool    headerDashed   = false;
    String  headerDash     = "5,5";
    Color   headerFill     = Color(255,255,255);
    Color   headerStroke   = GrayColor(160);

    // content frame defaults
    bool    contentFrameOn  = false;
    bool    contentFillOn   = false;
    int     contentRadius   = DPI(0);
    int     contentStrokeTh = 1;
    bool    contentDashed   = false;
    String  contentDash     = "5,5";
    Color   contentFill     = Color(255,255,255);
    Color   contentStroke   = GrayColor(160);

    // behavior
    bool    autoFill = false;          // smart vertical splitter for FIXED

    // insets & gaps
    int     cardPadX = DPI(0), cardPadY = DPI(0);
    Rect    headerInset  = Rect(DPI(8), DPI(8), DPI(8), DPI(8));
    int     headerGapV   = DPI(4);
    Rect    contentInset = Rect(DPI(8), DPI(8), DPI(8), DPI(8));
    Size    contentGap   = Size(DPI(6), DPI(6));

    // panes
    ParentCtrl headerPane;
    ParentCtrl contentPane;
    ParentCtrl contentLayer;

    // scroll
    ScrollBar  vbar;
    bool       scrollEnabled = true;
    int        scroll_y = 0;

    // content layout config
    ContentLayout contentLayout = FIXED;
    int       wrapItemW = 0, wrapItemH = 0;
    int       gridCols   = 3, gridCellW  = 0, gridCellH  = 0;
    bool      gridStretch= true;
    bool gridFill = false;

    // clamp & size promises
    bool      clampContentToPane = true;
    Size      minContent = Size(0,0), maxContent = Size(0,0);

    // cached layout info
    int       cachedHeaderMin = DPI(10);
    Rect      lastHeaderRc, lastContentRc;
    int       titleX=0, titleY=0, titleW=0;
    int       subTitleX=0, subTitleY=0, subTitleW=0;
    int       badgeX=0, badgeY=0, badgeW=0;

    // virtual content height (for scroll)
    int       virtualH = DPI(10);
    bool      contentDirty = true;

    // ==============================
    // Helpers
    // ==============================
    static void ClearChildren(ParentCtrl& p) {
        for(Ctrl *q = p.GetFirstChild(); q; ) { Ctrl* n = q->GetNext(); q->Remove(); q = n; }
    }

    Size WrapBaseSize(Ctrl& c) const {
        Size ms = c.GetMinSize();
        int w = wrapItemW > 0 ? wrapItemW : ms.cx;
        int h = wrapItemH > 0 ? wrapItemH : ms.cy;
        return Size(max(1, w), max(1, h));
    }

    void ContentLayoutPass(int pane_w, int pane_h) {
        virtualH = pane_h;

        // -------- FIXED (with optional AutoFill) --------
        if(contentLayout == FIXED) {
            const int inner_w = max(0, pane_w - contentInset.left - contentInset.right);
            const int inner_h = max(0, pane_h - contentInset.top  - contentInset.bottom);

            if(autoFill) {
                Vector<Ctrl*> vis;
                for(Ctrl* q = contentLayer.GetFirstChild(); q; q = q->GetNext())
                    if(q->IsShown())
                        vis.Add(q);

                const int n = vis.GetCount();

                if(n == 0) {
                    virtualH = pane_h;
                    return;
                }
                else if(n == 1) {
                    vis[0]->SetRect(contentInset.left, contentInset.top, inner_w, inner_h);
                    virtualH = pane_h;
                    return;
                }
                else {
                    const int gaps_h   = (n - 1) * contentGap.cy;
                    const int avail_h  = max(0, inner_h - gaps_h);
                    const int per_h    = n > 0 ? max(0, avail_h / n) : 0;

                    int y = contentInset.top;
                    for(int i = 0; i < n; ++i) {
                        int h = (i == n - 1) ? (contentInset.top + inner_h - y) : per_h; // remainder to last
                        h = max(0, h);
                        vis[i]->SetRect(contentInset.left, y, inner_w, h);
                        y += h;
                        if(i + 1 < n) y += contentGap.cy;
                    }
                    virtualH = pane_h;
                    return;
                }
            }

            // classic FIXED behavior (no AutoFill)
            int max_bottom = contentInset.top;
            for(Ctrl *q = contentLayer.GetFirstChild(); q; q = q->GetNext())
                if(q->IsShown())
                    max_bottom = max(max_bottom, q->GetRect().bottom);
            virtualH = max(pane_h, max_bottom + contentInset.bottom);
            return;
        }

        // -------- WRAP --------
        if(contentLayout == WRAP) {
            const int inner_w = max(0, pane_w - contentInset.left - contentInset.right);
            int x = contentInset.left, y = contentInset.top, line_h = 0;
            for(Ctrl *q = contentLayer.GetFirstChild(); q; q = q->GetNext()) {
                if(!q->IsShown()) continue;
                Size s = WrapBaseSize(*q);
                if(x > contentInset.left && x + s.cx > contentInset.left + inner_w) {
                    x = contentInset.left; y += line_h + contentGap.cy; line_h = 0;
                }
                q->SetRect(x, y, s.cx, s.cy);
                x += s.cx + contentGap.cx;
                line_h = max(line_h, s.cy);
            }
            int content_h = y + (line_h > 0 ? line_h : 0) + contentInset.bottom;
            virtualH = max(pane_h, content_h);
            return;
        }

        // -------- GRID --------
		if(contentLayout == GRID) {
		    const int inner_w = max(0, pane_w - contentInset.left - contentInset.right);
		    const int inner_h = max(0, pane_h - contentInset.top  - contentInset.bottom);
		
		    // count visible children
		    int nvis = 0;
		    for(Ctrl *q = contentLayer.GetFirstChild(); q; q = q->GetNext())
		        if(q->IsShown()) ++nvis;
		
		    const int cols = max(1, gridCols);
		    const int rows = max(1, (nvis + cols - 1) / cols);
		
		    int cell_w = 0, cell_h = 0;
		
		    if(gridFill) {
		        // Fill the available space: compute cells from pane size
		        const int total_gap_x = (cols - 1) * contentGap.cx;
		        const int total_gap_y = (rows - 1) * contentGap.cy;
		        cell_w = cols > 0 ? max(1, (inner_w - total_gap_x) / cols) : inner_w;
		        cell_h = rows > 0 ? max(1, (inner_h - total_gap_y) / rows) : inner_h;
		
		        // Grid fills the pane, so the card itself won't scroll
		        virtualH = pane_h;
		    } else {
		        // Original behavior
		        cell_w = gridCellW > 0 ? gridCellW
		                               : max(1, (inner_w - (cols - 1) * contentGap.cx) / cols);
		        int auto_h = 0;
		        if(gridCellH <= 0) {
		            for(Ctrl *q = contentLayer.GetFirstChild(); q; q = q->GetNext())
		                if(q->IsShown()) auto_h = max(auto_h, q->GetMinSize().cy);
		            if(auto_h <= 0) auto_h = DPI(32);
		        }
		        cell_h = gridCellH > 0 ? gridCellH : auto_h;
		
		        int content_h = contentInset.top
		                      + (rows > 0 ? rows * cell_h + (rows - 1) * contentGap.cy : 0)
		                      + contentInset.bottom;
		        virtualH = max(pane_h, content_h);
		    }
		
		    int idx = 0;
		    for(Ctrl *q = contentLayer.GetFirstChild(); q; q = q->GetNext()) {
		        if(!q->IsShown()) continue;
		        int col = idx % cols, row = idx / cols;
		        int x = contentInset.left + col * (cell_w + contentGap.cx);
		        int y = contentInset.top  + row * (cell_h + contentGap.cy);
		
		        if(gridStretch || gridFill) // in fill mode stretching usually makes sense
		            q->SetRect(x, y, cell_w, cell_h);
		        else {
		            Size ms = q->GetMinSize();
		            int w = min(ms.cx, cell_w), h = min(ms.cy, cell_h);
		            int cx = x + (cell_w - w) / 2, cy = y + (cell_h - h) / 2;
		            q->SetRect(cx, cy, w, h);
		        }
		        ++idx;
		    }
		    return;
		}

    }
};

} // namespace Upp
#endif //_StageCard_StageCard_h_