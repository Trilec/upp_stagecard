#include "StageCard.h"

namespace Upp {

// -------------------------- Default style --------------------------
const StageCard::Style& StageCard::StyleDefault() {
    static Style s;

    Color faceN   = Color(255,255,255);
    Color faceHot = Blend(faceN, SColorHighlight(), 20);
    Color facePr  = Blend(faceN, Black(), 6);
    Color faceDis = Blend(SColorFace(), SColorPaper(), 40);

    Color bordN   = SColorShadow();
    Color bordHot = Blend(bordN, SColorHighlight(), 20);
    Color bordPr  = Blend(bordN, Black(), 15);
    Color bordDis = Blend(bordN, SColorDisabled(), 35);

    Color textN   = SColorText();
    Color textDis = SColorDisabled();

    s.palette.headerFace [StageCard::ST_NORMAL]   = faceN;
    s.palette.headerFace [StageCard::ST_HOT]      = faceHot;
    s.palette.headerFace [StageCard::ST_PRESSED]  = facePr;
    s.palette.headerFace [StageCard::ST_DISABLED] = faceDis;

    s.palette.headerBorder[StageCard::ST_NORMAL]   = bordN;
    s.palette.headerBorder[StageCard::ST_HOT]      = bordHot;
    s.palette.headerBorder[StageCard::ST_PRESSED]  = bordPr;
    s.palette.headerBorder[StageCard::ST_DISABLED] = bordDis;

    s.palette.titleInk   [StageCard::ST_NORMAL]   = textN;
    s.palette.titleInk   [StageCard::ST_HOT]      = textN;
    s.palette.titleInk   [StageCard::ST_PRESSED]  = textN;
    s.palette.titleInk   [StageCard::ST_DISABLED] = textDis;

    s.palette.subTitleInk[StageCard::ST_NORMAL]   = textN;
    s.palette.subTitleInk[StageCard::ST_HOT]      = textN;
    s.palette.subTitleInk[StageCard::ST_PRESSED]  = textN;
    s.palette.subTitleInk[StageCard::ST_DISABLED] = textDis;

    s.palette.badgeInk   [StageCard::ST_NORMAL]   = textN;
    s.palette.badgeInk   [StageCard::ST_HOT]      = textN;
    s.palette.badgeInk   [StageCard::ST_PRESSED]  = textN;
    s.palette.badgeInk   [StageCard::ST_DISABLED] = textDis;

    s.palette.contentBg   = SColorPaper();
    s.palette.contentInk  = SColorText();
    s.palette.cardBorder  = SColorShadow();
    s.palette.cardFill    = Color(240,240,240);
    s.palette.underline   = GrayColor(160);

    s.metrics.titleFont        = StdFont().Bold().Height(DPI(18));
    s.metrics.subTitleFont     = StdFont().Height(DPI(9));
    s.metrics.badgeFont        = StdFont().Height(DPI(18));
    s.metrics.padX             = DPI(4);
    s.metrics.padY             = DPI(4);
    s.metrics.headerGap        = DPI(4);
    s.metrics.titleUnderlineTh = 1;
    s.metrics.badgeAlignDefault= StageCard::RIGHT;

    s.badgeGlyph = StageCard::GlyphLook{};
    return s;
}

// -------------------------- Style setters --------------------------
StageCard& StageCard::SetStyle(const Style& s) {
    style_ref_ = &s;
    palette_   = s.palette;
    metrics_   = s.metrics;
    Layout();
    Refresh();
    return *this;
}

StageCard& StageCard::SetStyleOwned(const Style& s) {
    owned_style_.Create() = s;
    style_ref_ = ~owned_style_;
    palette_   = s.palette;
    metrics_   = s.metrics;
    Layout();
    Refresh();
    return *this;
}

StageCard& StageCard::SetPalette(const UiPalette& p) {
    palette_ = p;
    Refresh();
    return *this;
}

StageCard& StageCard::SetMetrics(const UiMetrics& m) {
    metrics_ = m;
    Layout();
    return *this;
}


// -------------------------- Constructor --------------------------
StageCard::StageCard() {
    Add(headerPane);
    Add(contentPane);
    contentPane.Add(contentLayer);
    Add(vbar);

    headerPane.Transparent();
    contentPane.Transparent();
    contentLayer.Transparent();
    vbar.Transparent();

    vbar.Hide();
    vbar.SetTotal(0);
    vbar.SetLine(DPI(16));
    vbar.SetPage(0);

    vbar.WhenScroll = [this] {
        scroll_y = vbar.Get();
        const Rect pr = contentPane.GetRect();
        if (IsVerticalScroll())
            contentLayer.SetRect(0, -scroll_y, pr.GetWidth(), contentLayer.GetRect().GetHeight());
        else
            contentLayer.SetRect(-scroll_y, 0, contentLayer.GetRect().GetWidth(), pr.GetHeight());
        contentLayer.Refresh();
    };

    lastVBarRc = RectC(0,0,0,0);
    SetStyle(StyleDefault());
}

// -------------------------- Header mouse -> state -------------------
int StageCard::HeaderStateIndex() const {
    if(!IsEnabled())    return ST_DISABLED;
    if(!headerStateOn_) return ST_NORMAL;
    if(headerDown_)     return ST_PRESSED;
    if(headerHot_)      return ST_HOT;
    return ST_NORMAL;
}
void StageCard::OnHeaderMouseEnter(Point, dword) { if(!headerStateOn_) return; headerHot_ = true;  Refresh(); }
void StageCard::OnHeaderMouseMove (Point, dword) { if(!headerStateOn_) return; Refresh(); }
void StageCard::OnHeaderMouseLeave()             { if(!headerStateOn_) return; headerHot_ = false; headerDown_ = false; ReleaseCapture(); Refresh(); }
void StageCard::OnHeaderLeftDown   (Point, dword){ if(!IsEnabled() || !headerStateOn_) return; headerDown_ = true; SetCapture(); Refresh(); }
void StageCard::OnHeaderLeftUp     (Point, dword){ if(!headerStateOn_) return; headerDown_ = false; ReleaseCapture(); Refresh(); }

// -------------------------- High-level layout mode ------------------
StageCard& StageCard::SetStack(StackMode m) {
    switch(m) {
    case StackMode::NONE:
        // Manual layout, keep current scroll axis (dir)
        mode = ContentMode::MANUAL;
        break;
    case StackMode::STACKV:
        mode = ContentMode::STACK;
        dir  = Direction::V;
        break;
    case StackMode::STACKH:
        mode = ContentMode::STACK;
        dir  = Direction::H;
        break;
    }
    Layout();
    return *this;
}

// -------------------------- stack API --------------------------
StageCard& StageCard::ReplaceExpand(Ctrl& c, int w) {
    ClearChildren(contentLayer);
    items.Clear();
    return AddExpand(c, w);
}

StageCard& StageCard::ReplaceFixed (Ctrl& c, int px, int py) {
    ClearChildren(contentLayer);
    items.Clear();
    return AddFixed(c, px, py);
}

StageCard& StageCard::ReplaceFixed (Ctrl& c) {
    ClearChildren(contentLayer);
    items.Clear();
    return AddFixed(c);
}

StageCard& StageCard::ClearContent() {
    ClearChildren(contentLayer);
    items.Clear();
    contentDirty = true;
    Layout();
    return *this;
}

StageCard& StageCard::AddFixed(Ctrl& c, int px, int py) {
    contentLayer.Add(c);
    Item it;
    it.kind     = ItemKind::CtrlItem;
    it.c        = &c;
    it.expand_w = 0;

    if(wrap) {
        // Wrap mode: px/py define explicit tile size; <= 0 -> use GetMinSize()
        if(px <= 0 || py <= 0) {
            it.fixed_w  = -1;
            it.fixed_h  = -1;
            it.fixed_px = -1;
        } else {
            it.fixed_w  = max(1, px);
            it.fixed_h  = max(1, py);
            it.fixed_px = (dir == Direction::V ? it.fixed_h : it.fixed_w);
        }
    } else {
        // Non-wrap stack: px = main-axis fixed size; <= 0 -> use GetMinSize()
        if(px <= 0)
            it.fixed_px = -1;
        else
            it.fixed_px = max(1, px);
        it.fixed_w = it.fixed_h = -1;
    }

    items.Add(it);
    contentDirty = true;
    Layout();
    return *this;
}

StageCard& StageCard::AddFixed(Ctrl& c, int px) {
    if(wrap)
        return AddFixed(c, px, px); // symmetric in wrap mode

    contentLayer.Add(c);
    Item it;
    it.kind     = ItemKind::CtrlItem;
    it.c        = &c;
    it.expand_w = 0;

    if(px <= 0)
        it.fixed_px = -1;
    else
        it.fixed_px = max(1, px);

    it.fixed_w = it.fixed_h = -1;

    items.Add(it);
    contentDirty = true;
    Layout();
    return *this;
}

StageCard& StageCard::AddFixed(Ctrl& c) {
    if(wrap) {
        Size ms = c.GetMinSize();
        return AddFixed(c, ms.cx, ms.cy);
    }
    return AddFixed(c, -1);
}

StageCard& StageCard::AddExpand(Ctrl& c, int w) {
    contentLayer.Add(c);
    Item it;
    it.kind     = ItemKind::CtrlItem;
    it.c        = &c;
    it.fixed_px = -1;
    it.fixed_w  = -1;
    it.fixed_h  = -1;
    it.expand_w = max(1, w);

    items.Add(it);
    contentDirty = true;
    Layout();
    return *this;
}

StageCard& StageCard::AddSpacer(int weight) {
    Item it;
    it.kind     = ItemKind::Spacer;
    it.c        = nullptr;
    it.fixed_px = -1;
    it.fixed_w  = -1;
    it.fixed_h  = -1;
    it.expand_w = max(1, weight);

    items.Add(it);
    contentDirty = true;
    Layout();
    return *this;
}

StageCard& StageCard::ClearHeader() {
    ClearChildren(headerPane);
    Layout();
    return *this;
}

// -------------------------- Sizing helpers --------------------------
int StageCard::HeaderHeight() const {
    int h = headerInset.top;

    if(!title.IsEmpty()) {
        h += metrics_.titleFont.GetCy();
        if(metrics_.titleUnderlineTh > 0 && !underlineVertical)
            h += metrics_.headerGap + metrics_.titleUnderlineTh;
        h += metrics_.headerGap;
    }
    if(!subTitle.IsEmpty()) {
        h += metrics_.subTitleFont.GetCy();
        h += metrics_.headerGap;
    }
    h += headerInset.bottom;
    return max(h, DPI(12));
}

Size StageCard::GetMinSize() const {
    const int minw = DPI(10);
    const int header_min = max(cachedHeaderMin, HeaderHeight());
    if (clampContentToPane || scrollEnabled)
        return (dir == Direction::V) ? Size(minw, header_min) : Size(header_min, minw);
    const int cm = (dir == Direction::V ? (minContent.cy>0?minContent.cy:DPI(10))
                                        : (minContent.cx>0?minContent.cx:DPI(10)));
    return (dir == Direction::V) ? Size(minw, header_min + cm)
                                 : Size(header_min + cm, minw);
}

void StageCard::MouseWheel(Point, int zdelta, dword) {
    if(!scrollEnabled || !vbar.IsShown()) return;
    vbar.Wheel(zdelta);
}

void StageCard::ClearChildren(ParentCtrl& p) {
    for(Ctrl *q = p.GetFirstChild(); q; ) {
        Ctrl* n = q->GetNext();
        q->Remove();
        q = n;
    }
}

void StageCard::RebuildItemsFromChildrenIfNeeded() {
    int child_count = 0;
    for(Ctrl* q = contentLayer.GetFirstChild(); q; q = q->GetNext())
        ++child_count;

    int known_ctrls = 0;
    for(const Item& it : items)
        if(it.kind == ItemKind::CtrlItem)
            ++known_ctrls;

    if(known_ctrls == child_count)
        return;

    Vector<Item> rebuilt;
    rebuilt.Reserve(child_count);
    for(Ctrl* q = contentLayer.GetFirstChild(); q; q = q->GetNext()) {
        int idx = -1;
        for(int i = 0; i < items.GetCount(); ++i)
            if(items[i].kind == ItemKind::CtrlItem && items[i].c == q) { idx = i; break; }
        if(idx >= 0) {
            rebuilt.Add(items[idx]);
        } else {
            Item it;
            it.kind     = ItemKind::CtrlItem;
            it.c        = q;
            it.fixed_px = -1;
            it.fixed_w  = -1;
            it.fixed_h  = -1;
            it.expand_w = 0;
            rebuilt.Add(it);
        }
    }
    for(const Item& it : items)
        if(it.kind == ItemKind::Spacer)
            rebuilt.Add(it);

    items = pick(rebuilt);
}

// Effective content inset = user inset only
Rect StageCard::EffectiveContentInset() const {
    // card-level inset is applied to the content area,
    // this is the child layout control now
    return contentInnerInset;
}

int StageCard::MeasureNaturalPrimaryForWidth(int) const {
    Rect eff = EffectiveContentInset();
    int total = eff.top + eff.bottom;
    int gaps  = 0;
    bool first = true;
    for(const Item& it : items) {
        if(it.kind == ItemKind::Spacer) continue;
        if(!it.c || !it.c->IsShown()) continue;
        Size ms = it.c->GetMinSize();
        int h = (it.fixed_px >= 0) ? it.fixed_px : ms.cy;
        if(!first) gaps += contentGap.cy;
        total += h;
        first = false;
    }
    return max(total + gaps, DPI(10));
}

int StageCard::MeasureNaturalPrimaryForHeight(int) const {
    Rect eff = EffectiveContentInset();
    int total = eff.left + eff.right;
    int gaps  = 0;
    bool first = true;
    for(const Item& it : items) {
        if(it.kind == ItemKind::Spacer) continue;
        if(!it.c || !it.c->IsShown()) continue;
        Size ms = it.c->GetMinSize();
        int w = (it.fixed_px >= 0) ? it.fixed_px : ms.cx;
        if(!first) gaps += contentGap.cx;
        total += w;
        first = false;
    }
    return max(total + gaps, DPI(10));
}

void StageCard::LayoutStackV(const Rect& inner) {
    Rect eff = EffectiveContentInset();
    int insetL = eff.left;
    int insetR = eff.right;
    int insetT = eff.top;
    int insetB = eff.bottom;

    struct Row {
        bool spacer;
        bool is_expand;
        int  expand;
        Ctrl* c;
        int  nat_h;   // natural height of the child (header, etc.)
    };
    Vector<Row> rows; rows.Reserve(items.GetCount());

    const int inner_w = max(0, inner.GetWidth()  - insetL - insetR);
    const int inner_h = max(0, inner.GetHeight() - insetT - insetB);

    int natural_fixed = 0;    // sum of non-expand row naturals
    int expand_weight = 0;    // total expand weight
    int expand_min    = 0;    // max natural among expand rows
    int expand_count  = 0;    // number of expand rows

    // Collect rows and measure
    for(const Item& it : items) {
        if(it.kind == ItemKind::Spacer) {
            bool is_exp = (it.expand_w > 0);
            rows.Add({true, is_exp, max(1, it.expand_w), nullptr, 0});
            if(is_exp) {
                expand_weight += max(1, it.expand_w);
                expand_count++;
            }
            continue;
        }

        if(!it.c || !it.c->IsShown())
            continue;

        Size ms = it.c->GetMinSize();
        int h = (it.fixed_px >= 0) ? it.fixed_px : ms.cy;
        bool is_exp = (it.expand_w > 0);

        rows.Add({false, is_exp, it.expand_w, it.c, h});

        if(is_exp) {
            expand_weight += max(1, it.expand_w);
            expand_min    = max(expand_min, h);
            expand_count++;
        } else {
            natural_fixed += h;
        }
    }

    const int cnt = rows.GetCount();
    const int total_gaps = (cnt > 0 ? (cnt - 1) * contentGap.cy : 0);

    // Baseline "needed" height:
    //  - all non-expand rows at their natural height
    //  - all expand rows at a common baseline expand_min
    int needed = insetT + natural_fixed + insetB + total_gaps;
    if(expand_count > 0)
        needed += expand_min * expand_count;

    const int avail = inner_h;
    const int extra = max(0, avail - needed);

    int y = insetT;
    const int x = insetL;
    const int w = inner_w;

    virtualLen = max(inner_h, needed);

    for(int i = 0; i < cnt; i++) {
        int h;

        if(rows[i].is_expand && expand_weight > 0) {
            // Split the remaining "extra" purely by weight,
            // each expand row starts from the same baseline expand_min.
            int share = (extra * rows[i].expand) / expand_weight;
            h = expand_min + share;
        } else {
            // Fixed / non-expand rows keep their natural height
            h = rows[i].nat_h;
        }

        if(!rows[i].spacer && rows[i].c)
            rows[i].c->SetRect(Rect(x, y, x + w, y + h));

        y += h;
        if(i + 1 < cnt)
            y += contentGap.cy;
    }

    contentLayer.SetRect(0, -scroll_y, inner.GetWidth(), virtualLen);
}


void StageCard::LayoutStackH(const Rect& inner) {
    Rect eff = EffectiveContentInset();
    int insetL = eff.left;
    int insetR = eff.right;
    int insetT = eff.top;
    int insetB = eff.bottom;

    struct Col {
        bool spacer;
        bool is_expand;
        int  expand;
        Ctrl* c;
        int  nat_w;   // natural width of the child
    };
    Vector<Col> cols; cols.Reserve(items.GetCount());

    const int inner_w = max(0, inner.GetWidth()  - insetL - insetR);
    const int inner_h = max(0, inner.GetHeight() - insetT - insetB);

    int natural_fixed = 0;
    int expand_weight = 0;
    int expand_min    = 0;
    int expand_count  = 0;

    // Collect columns and measure
    for(const Item& it : items) {
        if(it.kind == ItemKind::Spacer) {
            bool is_exp = (it.expand_w > 0);
            cols.Add({true, is_exp, max(1, it.expand_w), nullptr, 0});
            if(is_exp) {
                expand_weight += max(1, it.expand_w);
                expand_count++;
            }
            continue;
        }

        if(!it.c || !it.c->IsShown())
            continue;

        Size ms = it.c->GetMinSize();
        int w = (it.fixed_px >= 0) ? it.fixed_px : ms.cx;
        bool is_exp = (it.expand_w > 0);

        cols.Add({false, is_exp, it.expand_w, it.c, w});

        if(is_exp) {
            expand_weight += max(1, it.expand_w);
            expand_min    = max(expand_min, w);
            expand_count++;
        } else {
            natural_fixed += w;
        }
    }

    const int cnt = cols.GetCount();
    const int total_gaps = (cnt > 0 ? (cnt - 1) * contentGap.cx : 0);

    int needed = insetL + natural_fixed + insetR + total_gaps;
    if(expand_count > 0)
        needed += expand_min * expand_count;

    const int avail = inner_w;
    const int extra = max(0, avail - needed);

    int x = insetL;
    const int y = insetT;
    const int h = inner_h;

    virtualLen = max(inner_w, needed);

    for(int i = 0; i < cnt; i++) {
        int w;

        if(cols[i].is_expand && expand_weight > 0) {
            int share = (extra * cols[i].expand) / expand_weight;
            w = expand_min + share;
        } else {
            w = cols[i].nat_w;
        }

        if(!cols[i].spacer && cols[i].c)
            cols[i].c->SetRect(Rect(x, y, x + w, y + h));

        x += w;
        if(i + 1 < cnt)
            x += contentGap.cx;
    }

    contentLayer.SetRect(-scroll_y, 0, virtualLen, inner.GetHeight());
}


void StageCard::LayoutWrapH(const Rect& inner) {
    Rect eff = EffectiveContentInset();
    int insetL = eff.left;
    int insetR = eff.right;
    int insetT = eff.top;
    int insetB = eff.bottom;

    const int avail_w = max(0, inner.GetWidth()  - insetL - insetR);
    const int inner_h = max(0, inner.GetHeight() - insetT - insetB);

    int x = insetL;
    int y = insetT;
    int line_h = 0;

    for(Item& it : items) {
        if(it.kind == ItemKind::Spacer) continue;
        if(!it.c || !it.c->IsShown())   continue;

        const Size ms = it.c->GetMinSize();
        const int cw  = (it.fixed_w >= 0 ? it.fixed_w : ms.cx);
        const int ch  = (it.fixed_h >= 0 ? it.fixed_h : ms.cy);

        int nx = (x == insetL) ? x : x + contentGap.cx;

        if(nx + cw > insetL + avail_w && x != insetL) {
            x = insetL;
            y += line_h + contentGap.cy;
            line_h = 0;
            nx = x;
        }

        it.c->SetRect(Rect(nx, y, nx + cw, y + ch));
        x = nx + cw;
        line_h = max(line_h, ch);
    }

    const int used_h = (x == insetL ? y : y + line_h);
    const int needed = used_h + insetB;

    virtualLen = max(inner_h, needed);
    contentLayer.SetRect(0, -scroll_y, inner.GetWidth(), virtualLen);
}

// -------------------------- Layout (header + content + scrollbars) --------------------------
StageCard& StageCard::SetHeaderColor(Color face_base, Color border_base) {
    MakeFaceStates(face_base,   palette_.headerFace);
    MakeBorderStates(border_base, palette_.headerBorder);
    Refresh();
    return *this;
}

StageCard& StageCard::SetHeaderColorState(Color fN, Color fH, Color fP, Color fD,
                                          Color bN, Color bH, Color bP, Color bD) {
    palette_.headerFace[0]=fN; palette_.headerFace[1]=fH; palette_.headerFace[2]=fP; palette_.headerFace[3]=fD;
    palette_.headerBorder[0]=bN; palette_.headerBorder[1]=bH; palette_.headerBorder[2]=bP; palette_.headerBorder[3]=bD;
    Refresh();
    return *this;
}

StageCard& StageCard::SetTitleColor(Color base, Color disabled) {
    palette_.titleInk[0]=base; palette_.titleInk[1]=base; palette_.titleInk[2]=base; palette_.titleInk[3]=disabled;
    Refresh();
    return *this;
}
StageCard& StageCard::SetSubTitleColor(Color base, Color disabled) {
    palette_.subTitleInk[0]=base; palette_.subTitleInk[1]=base; palette_.subTitleInk[2]=base; palette_.subTitleInk[3]=disabled;
    Refresh();
    return *this;
}
StageCard& StageCard::SetBadgeColor(Color base, Color disabled) {
    palette_.badgeInk[0]=base; palette_.badgeInk[1]=base; palette_.badgeInk[2]=base; palette_.badgeInk[3]=disabled;
    Refresh();
    return *this;
}

StageCard& StageCard::SetTitleColorState(Color n, Color h, Color p, Color d) {
    palette_.titleInk[0]=n; palette_.titleInk[1]=h; palette_.titleInk[2]=p; palette_.titleInk[3]=d;
    Refresh(); return *this;
}
StageCard& StageCard::SetSubTitleColorState(Color n, Color h, Color p, Color d) {
    palette_.subTitleInk[0]=n; palette_.subTitleInk[1]=h; palette_.subTitleInk[2]=p; palette_.subTitleInk[3]=d;
    Refresh(); return *this;
}
StageCard& StageCard::SetBadgeColorState(Color n, Color h, Color p, Color d) {
    palette_.badgeInk[0]=n; palette_.badgeInk[1]=h; palette_.badgeInk[2]=p; palette_.badgeInk[3]=d;
    Refresh(); return *this;
}

StageCard& StageCard::SetContentColor(Color bg, Color ink) {
    palette_.contentBg = bg;
    palette_.contentInk = ink;
    Refresh();
    return *this;
}

StageCard& StageCard::SetCardColors(Color fill, Color stroke) {
    palette_.cardFill = fill;
    palette_.cardBorder = stroke;
    Refresh();
    return *this;
}

StageCard& StageCard::SetHeaderInset(int l, int t, int r, int b) {
    headerInset = Rect(max(0,l), max(0,t), max(0,r), max(0,b));
    Layout();
    return *this;
}
StageCard& StageCard::SetHeaderGap(int px) {
    metrics_.headerGap = max(0, px);
    Layout();
    return *this;
}
StageCard& StageCard::SetCardGap(int px) {
    cardGap = max(0, px);
    Layout();
    return *this;
}

StageCard& StageCard::SetContentInset(int l, int t, int r, int b) {
    contentInset = Rect(max(0,l), max(0,t), max(0,r), max(0,b));
    Layout();
    return *this;
}
StageCard& StageCard::SetContentInnerInset(int l, int t, int r, int b) {
    contentInnerInset = Rect(max(0,l), max(0,t), max(0,r), max(0,b));
    Layout();
    return *this;
}
StageCard& StageCard::SetContentGap(int gx, int gy) {
    contentGap = Size(max(0,gx), max(0,gy));
    Layout();
    return *this;
}

void StageCard::Layout() {
    const Size sz = GetSize();

    // Outer card rect (inside card frame)
    Rect outer = Rect(sz);
    if(cardFrameOn && cardStrokeTh > 0) {
        int pad = (cardStrokeTh + 1) / 2;
        if(cardRadius > 0)
            pad += max(1, cardRadius / 4);
        outer = outer.Deflated(pad, pad);
        if(outer.right < outer.left)  outer.right  = outer.left;
        if(outer.bottom < outer.top)  outer.bottom = outer.top;
    }

    // --- Badge band ---
    const int inner_left   = outer.left  + headerInset.left;
    const int inner_right  = outer.right - headerInset.right;
    const int inner_width  = max(0, inner_right - inner_left);
    const int icon_band_h  = max(0, headerInset.top - metrics_.headerGap);
    const int cx           = inner_left + inner_width / 2;

    badgeIconRc = RectC(0,0,0,0);

    const HeaderAlign effBadgeAlign = EffectiveBadgeAlign();
    const bool styleHasGlyph = (style_ref_ && !style_ref_->badgeGlyph.IsEmpty());
    if(icon_band_h > 0 && (hasBadgeIcon || styleHasGlyph)) {
        Size nat  = hasBadgeIcon ? badgeIcon.GetSize()
                                 : (style_ref_->badgeGlyph.img.IsEmpty() ? Size(0,0)
                                                                          : style_ref_->badgeGlyph.img.GetSize());
        Size pref = (badgeIconPref.cx > 0 || badgeIconPref.cy > 0) ? badgeIconPref : nat;
        if(pref.cx <= 0) pref.cx = nat.cx > 0 ? nat.cx : DPI(24);
        if(pref.cy <= 0) pref.cy = nat.cy > 0 ? nat.cy : DPI(24);

        int max_w_side = max(DPI(28), min(icon_band_h, inner_width / 4));
        int avail_w    = (effBadgeAlign == CENTER ? max(DPI(36), min(inner_width / 3, inner_width))
                                                  : max_w_side);
        int avail_h    = icon_band_h;

        double sx = double(avail_w) / double(pref.cx);
        double sy = double(avail_h) / double(pref.cy);
        double s  = min(1.0, min(sx, sy));

        int iw = max(1, int(pref.cx * s));
        int ih = max(1, int(pref.cy * s));

        int ix = inner_left;
        if(effBadgeAlign == CENTER)      ix = cx - iw / 2;
        else if(effBadgeAlign == RIGHT)  ix = inner_right - iw;

        int iy = outer.top + (icon_band_h - ih) / 2;
        badgeIconRc = RectC(ix, iy, iw, ih);
    }

    // --- Title / underline / subtitle block ---
    int reserve_left  = 0;
    int reserve_right = 0;
    if(badgeIconRc.GetWidth() > 0) {
        if(effBadgeAlign == LEFT)  reserve_left  = badgeIconRc.GetWidth() + badgeGapX;
        if(effBadgeAlign == RIGHT) reserve_right = badgeIconRc.GetWidth() + badgeGapX;
    }

    const int textL = inner_left  + reserve_left;
    const int textR = inner_right - reserve_right;
    const int textW = max(0, textR - textL);

    int y = outer.top + headerInset.top;
    titleX = titleY = 0; titleW = 0;
    line1X = line1W = line2X = line2W = 0;
    titleLineY = 0;
    vLineX = vLineY = vLineH = 0;

    int textTop = 0;
    int textBottom = 0;
    int titleH = 0;
    int subTitleH = 0;

    if(!IsNull(title)) {
        Size ts = GetTextSize(title, metrics_.titleFont);
        titleW = ts.cx;
        titleH = ts.cy;

        if(headerAlign == LEFT)       titleX = textL;
        else if(headerAlign == RIGHT) titleX = max(textL, textR - ts.cx);
        else                          titleX = textL + max(0, (textW - ts.cx) / 2);

        titleY = y;
        textTop = titleY;
        y += ts.cy;

        if(metrics_.titleUnderlineTh > 0 && !underlineVertical) {
            y += metrics_.headerGap;
            const int lineY = y;
            if(effBadgeAlign == CENTER && badgeIconRc.GetWidth() > 0) {
                int left_end  = min(textR, badgeIconRc.left - badgeGapX/2);
                int right_beg = max(textL, badgeIconRc.right + badgeGapX/2);
                if(left_end > textL)  { line1X = textL; line1W = left_end - textL; }
                if(textR > right_beg) { line2X = right_beg; line2W = textR - right_beg; }
            } else {
                line1X = textL; line1W = textW; line2X = line2W = 0;
            }
            titleLineY = lineY;
            y += metrics_.titleUnderlineTh;
        }

        y += metrics_.headerGap;
    }

    subTitleX = subTitleY = subTitleW = 0;
    if(!IsNull(subTitle)) {
        Size ss = GetTextSize(subTitle, metrics_.subTitleFont);
        subTitleH = ss.cy;
        if(headerAlign == LEFT)       subTitleX = textL;
        else if(headerAlign == RIGHT) subTitleX = max(textL, textR - ss.cx);
        else                          subTitleX = textL + max(0, (textW - ss.cx) / 2);
        subTitleY = y;
        subTitleW = ss.cx;
        y += ss.cy;
        y += metrics_.headerGap;
    }

    if(!IsNull(title)) {
        if(!IsNull(subTitle))
            textBottom = subTitleY + subTitleH;
        else
            textBottom = titleY + titleH;

        if(underlineVertical && metrics_.titleUnderlineTh > 0) {
            int gap = metrics_.padX;
            int baseX;

            if(headerAlign == RIGHT)
                baseX = titleX - gap;
            else
                baseX = titleX + titleW + gap;

            if(baseX < textL) baseX = textL;
            if(baseX > textR) baseX = textR;

            vLineX = baseX;
            vLineY = textTop;
            vLineH = max(0, textBottom - textTop);
        }
    }

    int header_child_bottom = 0;
    for(Ctrl *q = headerPane.GetFirstChild(); q; q = q->GetNext())
        header_child_bottom = max(header_child_bottom, q->GetRect().bottom);

    int header_h_text = y - outer.top + headerInset.bottom;
    const int header_h = max(header_h_text,
                             headerInset.top + header_child_bottom + headerInset.bottom);
    cachedHeaderMin = max(header_h, DPI(10));

    lastHeaderRc = Rect(outer.left, outer.top, outer.right, outer.top + header_h);
    headerPane.SetRect(lastHeaderRc);

    // --- Content viewport (between header and bottom of card) ---
    Rect pane_rc = Rect(outer.left,
                        lastHeaderRc.bottom + cardGap,
                        outer.right,
                        outer.bottom);

    // Apply content inset as “outer margin” for the content region
    if(contentInset.left || contentInset.top || contentInset.right || contentInset.bottom) {
        pane_rc = pane_rc.Deflated(contentInset.left, contentInset.top,
                                   contentInset.right, contentInset.bottom);
        if(pane_rc.right < pane_rc.left)   pane_rc.right  = pane_rc.left;
        if(pane_rc.bottom < pane_rc.top)   pane_rc.bottom = pane_rc.top;
    }

    // clamp logic (pane_rc is still "frame" rect)
    if(!clampContentToPane) {
        if(dir == Direction::V) {
            if(minContent.cy > 0 && pane_rc.GetHeight() < minContent.cy)
                pane_rc.bottom = pane_rc.top + minContent.cy;
            if(maxContent.cy > 0 && pane_rc.GetHeight() > maxContent.cy)
                pane_rc.bottom = pane_rc.top + maxContent.cy;
        } else {
            if(minContent.cx > 0 && pane_rc.GetWidth()  < minContent.cx)
                pane_rc.right = pane_rc.left + minContent.cx;
            if(maxContent.cx > 0 && pane_rc.GetWidth()  > maxContent.cx)
                pane_rc.right = pane_rc.left + maxContent.cx;
        }
    }

    // Split: frame rect vs inner content rect
    Rect frame_rc = pane_rc;           // used for content frame drawing
    Rect inner    = frame_rc;          // used for contentPane / children

    if(contentFrameOn && contentStrokeTh > 0) {
        int pad = (contentStrokeTh + 1) / 2;
        if(contentRadius > 0)
            pad += max(1, contentRadius / 4);
        inner = inner.Deflated(pad, pad);
        if(inner.right < inner.left)   inner.right  = inner.left;
        if(inner.bottom < inner.top)   inner.bottom = inner.top;
    }

    const int sbw = DPI(14);
    contentPane.SetRect(inner);
    RebuildItemsFromChildrenIfNeeded();

    lastVBarRc = RectC(0,0,0,0);

    if(mode == ContentMode::STACK) {
        if(dir == Direction::V) {
            LayoutStackV(inner);
            bool overflow = (virtualLen > inner.GetHeight());
            bool show_sb  = scrollEnabled && overflow;
            if(show_sb) {
                contentPane.SetRect(inner.left, inner.top,
                                    inner.GetWidth()-sbw, inner.GetHeight());
                inner = contentPane.GetRect();
                LayoutStackV(inner);
            }
            if(scrollEnabled && (virtualLen > inner.GetHeight())) {
                vbar.Show();
                vbar.LeftPos(inner.right, sbw).TopPos(inner.top, inner.GetHeight());
                vbar.SetTotal(virtualLen);
                vbar.SetPage(inner.GetHeight());
                const int maxpos = max(0, virtualLen - inner.GetHeight());
                scroll_y = clamp(scroll_y, 0, maxpos);
                vbar.Set(scroll_y);
                lastVBarRc = Rect(inner.right, inner.top,
                                  inner.right + sbw, inner.top + inner.GetHeight());
            } else {
                vbar.Hide(); scroll_y = 0; lastVBarRc = RectC(0,0,0,0);
            }
        } else {
            if(wrap) {
                LayoutWrapH(inner);
                bool overflow = (virtualLen > inner.GetHeight());
                bool show_sb  = scrollEnabled && overflow;
                if(show_sb) {
                    contentPane.SetRect(inner.left, inner.top,
                                        inner.GetWidth()-sbw, inner.GetHeight());
                    inner = contentPane.GetRect();
                    LayoutWrapH(inner);
                }
                if(scrollEnabled && (virtualLen > inner.GetHeight())) {
                    vbar.Show();
                    vbar.LeftPos(inner.right, sbw).TopPos(inner.top, inner.GetHeight());
                    vbar.SetTotal(virtualLen);
                    vbar.SetPage(inner.GetHeight());
                    const int maxpos = max(0, virtualLen - inner.GetHeight());
                    scroll_y = clamp(scroll_y, 0, maxpos);
                    vbar.Set(scroll_y);
                    lastVBarRc = Rect(inner.right, inner.top,
                                      inner.right + sbw, inner.top + inner.GetHeight());
                } else {
                    vbar.Hide(); scroll_y = 0; lastVBarRc = RectC(0,0,0,0);
                }
            } else {
                LayoutStackH(inner);
                bool overflow = (virtualLen > inner.GetWidth());
                bool show_sb  = scrollEnabled && overflow;
                if(show_sb) {
                    contentPane.SetRect(inner.left, inner.top,
                                        inner.GetWidth()-sbw, inner.GetHeight());
                    inner = contentPane.GetRect();
                    LayoutStackH(inner);
                }
                if(scrollEnabled && (virtualLen > inner.GetWidth())) {
                    vbar.Show();
                    vbar.LeftPos(inner.right, sbw).TopPos(inner.top, inner.GetHeight());
                    vbar.SetTotal(virtualLen);
                    vbar.SetPage(inner.GetWidth());
                    const int maxpos = max(0, virtualLen - inner.GetWidth());
                    scroll_y = clamp(scroll_y, 0, maxpos);
                    vbar.Set(scroll_y);
                    lastVBarRc = Rect(inner.right, inner.top,
                                      inner.right + sbw, inner.top + inner.GetHeight());
                } else {
                    vbar.Hide(); scroll_y = 0; lastVBarRc = RectC(0,0,0,0);
                }
            }
        }
    } else { // MANUAL
        Rect bounds(0,0,0,0);
        for(Ctrl* q = contentLayer.GetFirstChild(); q; q = q->GetNext())
            if(q->IsShown()) bounds |= q->GetRect();

        Rect eff = EffectiveContentInset();
        int insetB = eff.bottom;
        int insetR = eff.right;

        if(dir == Direction::V) {
            virtualLen = max(inner.GetHeight(), bounds.bottom + insetB);
            contentLayer.SetRect(0, -scroll_y, inner.GetWidth(), virtualLen);
            bool overflow = (virtualLen > inner.GetHeight());
            bool show_sb  = scrollEnabled && overflow;
            if(show_sb) {
                contentPane.SetRect(inner.left, inner.top,
                                    inner.GetWidth()-sbw, inner.GetHeight());
                inner = contentPane.GetRect();
                contentLayer.SetRect(0, -scroll_y, inner.GetWidth(), virtualLen);
                vbar.Show();
                vbar.LeftPos(inner.right, sbw).TopPos(inner.top, inner.GetHeight());
                vbar.SetTotal(virtualLen);
                vbar.SetPage(inner.GetHeight());
                const int maxpos = max(0, virtualLen - inner.GetHeight());
                scroll_y = clamp(scroll_y, 0, maxpos);
                vbar.Set(scroll_y);
                lastVBarRc = Rect(inner.right, inner.top,
                                  inner.right + sbw, inner.top + inner.GetHeight());
            } else {
                vbar.Hide(); scroll_y = 0; lastVBarRc = RectC(0,0,0,0);
            }
        } else {
            virtualLen = max(inner.GetWidth(), bounds.right + insetR);
            contentLayer.SetRect(-scroll_y, 0, virtualLen, inner.GetHeight());
            bool overflow = (virtualLen > inner.GetWidth());
            bool show_sb  = scrollEnabled && overflow;
            if(show_sb) {
                contentPane.SetRect(inner.left, inner.top,
                                    inner.GetWidth()-sbw, inner.GetHeight());
                inner = contentPane.GetRect();
                contentLayer.SetRect(-scroll_y, 0, virtualLen, inner.GetHeight());
                vbar.Show();
                vbar.LeftPos(inner.right, sbw).TopPos(inner.top, inner.GetHeight());
                vbar.SetTotal(virtualLen);
                vbar.SetPage(inner.GetWidth());
                const int maxpos = max(0, virtualLen - inner.GetWidth());
                scroll_y = clamp(scroll_y, 0, maxpos);
                vbar.Set(scroll_y);
                lastVBarRc = Rect(inner.right, inner.top,
                                  inner.right + sbw, inner.top + inner.GetHeight());
            } else {
                vbar.Hide(); scroll_y = 0; lastVBarRc = RectC(0,0,0,0);
            }
        }
    }

    // Important: content frame rect (for Paint) is the *outer* frame_rc
    lastContentRc = frame_rc;
    Refresh();
}

// -------------------------- Paint --------------------------
StageCard::HeaderAlign StageCard::EffectiveBadgeAlign() const {
    if(badgeAlignExplicit) return badgeAlign;
    if(style_ref_)         return style_ref_->metrics.badgeAlignDefault;
    return RIGHT;
}

void StageCard::DrawBadgeGlyph(Draw& w, const Rect& rc) const {
    if(hasBadgeIcon && !badgeIcon.IsEmpty()) {
        w.DrawImage(rc.left, rc.top, rc.GetWidth(), rc.GetHeight(), badgeIcon);
        return;
    }
    if(hasBadgeText && !badge.IsEmpty()) {
        Size ts = GetTextSize(badge, metrics_.badgeFont);
        int x = rc.left + (rc.GetWidth()  - ts.cx)/2;
        int y = rc.top  + (rc.GetHeight() - ts.cy)/2;
        w.DrawText(x, y, badge, metrics_.badgeFont,
                   palette_.badgeInk[HeaderStateIndex()]);
        return;
    }
    if(style_ref_ && !style_ref_->badgeGlyph.IsEmpty()) {
        const GlyphLook& g = style_ref_->badgeGlyph;
        if(!g.img.IsEmpty()) {
            w.DrawImage(rc.left, rc.top, rc.GetWidth(), rc.GetHeight(), g.img);
            return;
        }
        if(!g.text.IsEmpty()) {
            Font f = (g.font.GetHeight() > 0) ? g.font : metrics_.badgeFont;
            Size ts = GetTextSize(g.text, f);
            int x = rc.left + (rc.GetWidth()  - ts.cx)/2;
            int y = rc.top  + (rc.GetHeight() - ts.cy)/2;
            w.DrawText(x, y, g.text, f, g.ink);
        }
    }
}

void StageCard::Paint(Draw& w) {
    Size sz = GetSize();
    if(sz.cx <= 0 || sz.cy <= 0) return;

    ImageBuffer ib(sz);
    Fill(~ib, RGBAZero(), ib.GetLength());
    BufferPainter p(ib, MODE_ANTIALIASED);

    auto RR = [&](double x, double y, double w0, double h0, double r) {
        if(r > 0) p.RoundedRectangle(x, y, w0, h0, r);
        else      p.Rectangle(x, y, w0, h0);
    };
    auto FillRectR = [&](Rect rc, int radius, Color fill, int def = 0) {
        if(fill.IsNullInstance() || rc.IsEmpty()) return;
        rc = rc.Deflated(def, def, def, def);
        if(rc.GetWidth() <= 0 || rc.GetHeight() <= 0) return;
        p.Begin();
        RR(rc.left, rc.top, rc.GetWidth(), rc.GetHeight(), max(0, radius - def));
        p.Fill(fill);
        p.End();
    };
    auto StrokeRectR = [&](Rect rc, int radius, Color stroke, int th,
                           bool dashed, const String& dash) {
        if(th <= 0 || stroke.IsNullInstance() || rc.IsEmpty()) return;
        const double inset = 0.5 * th;
        const double x = rc.left + inset;
        const double y = rc.top  + inset;
        const double w0 = max(0.0, rc.GetWidth()  - 2*inset);
        const double h0 = max(0.0, rc.GetHeight() - 2*inset);
        p.Begin();
        RR(x, y, w0, h0, max(0.0, radius - inset));
        if(dashed) p.Dash(dash, 0.0);
        p.Stroke(th, stroke);
        p.End();
    };

    const int headerInsetPx  = (headerFrameOn  && headerStrokeTh  > 0) ? (headerStrokeTh  + 1)/2 : 0;
    const int contentInsetPx = (contentFrameOn && contentStrokeTh > 0) ? (contentStrokeTh + 1)/2 : 0;

    const int hs = HeaderStateIndex();

    // LAYER 1: Backgrounds
    if(cardFillOn)
        FillRectR(Rect(sz), cardRadius, palette_.cardFill, 0);
    if(headerFillOn && !lastHeaderRc.IsEmpty())
        FillRectR(lastHeaderRc, headerRadius, palette_.headerFace[hs], headerInsetPx);
    if(contentFillOn && !lastContentRc.IsEmpty())
        FillRectR(lastContentRc, contentRadius, palette_.contentBg, contentInsetPx);

    // LAYER 2: Titles / badge
    if(!lastHeaderRc.IsEmpty()) {
        if(badgeIconRc.GetWidth() > 0 && badgeIconRc.GetHeight() > 0)
            DrawBadgeGlyph(p, badgeIconRc);

        if(!IsNull(title))
            p.DrawText(titleX, titleY, title, metrics_.titleFont, palette_.titleInk[hs]);
        if(!IsNull(subTitle))
            p.DrawText(subTitleX, subTitleY, subTitle, metrics_.subTitleFont,
                       palette_.subTitleInk[hs]);

        if(metrics_.titleUnderlineTh > 0 && !IsNull(title)) {
            if(!underlineVertical) {
                if(line1W > 0)
                    p.DrawRect(line1X, titleLineY, line1W,
                               metrics_.titleUnderlineTh, palette_.underline);
                if(line2W > 0)
                    p.DrawRect(line2X, titleLineY, line2W,
                               metrics_.titleUnderlineTh, palette_.underline);
            } else {
                if(vLineH > 0)
                    p.DrawRect(vLineX, vLineY,
                               metrics_.titleUnderlineTh, vLineH, palette_.underline);
            }
        }
    }

    // LAYER 3: Frames
    if(headerFrameOn && headerStrokeTh > 0 && !lastHeaderRc.IsEmpty())
        StrokeRectR(lastHeaderRc, headerRadius, palette_.headerBorder[hs],
                    headerStrokeTh, headerDashed, headerDash);
    if(contentFrameOn && contentStrokeTh > 0 && !lastContentRc.IsEmpty())
        StrokeRectR(lastContentRc, contentRadius, palette_.cardBorder,
                    contentStrokeTh, contentDashed, contentDash);
    if(cardFrameOn && cardStrokeTh > 0)
        StrokeRectR(Rect(sz), cardRadius, palette_.cardBorder,
                    cardStrokeTh, cardDashed, cardDash);

    w.DrawImage(0, 0, ib);
}

} // namespace Upp
