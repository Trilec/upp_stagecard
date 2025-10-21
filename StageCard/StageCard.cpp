#include "StageCard.h"

namespace Upp {

StageCard::StageCard() {
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


Size StageCard::GetMinSize() const {
    if(clampContentToPane)
        return Size(DPI(10), max(cachedHeaderMin, DPI(10)));
    int content_min = minContent.cy > 0 ? minContent.cy : DPI(10);
    return Size(DPI(10), max(cachedHeaderMin + content_min, DPI(10)));
}

void StageCard::MouseWheel(Point, int zdelta, dword) {
    if(!scrollEnabled || !vbar.IsShown()) return;
    vbar.Wheel(zdelta); // ScrollBar::Wheel handles accumulation/lines
}

void StageCard::Layout() {
    const Size sz = GetSize();
    Rect outer = Rect(sz).Deflated(cardPadX, cardPadY);

    const int header_w_inner = max(0, outer.GetWidth() - headerInset.left - headerInset.right);
    int y = outer.top + headerInset.top;

    // title / subtitle / badge metrics
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
        y += ss.cy; y += headerGapV;
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

    // content layout / scrollbar negotiation
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

void StageCard::Paint(Draw& w) {
    const Size sz = GetSize();
    if(sz.cx <= 0 || sz.cy <= 0) return;

    ImageBuffer ib(sz);
    Fill(~ib, RGBAZero(), ib.GetLength());
    BufferPainter p(ib);

    PaintCardLayer(p, sz);
    PaintHeaderLayer(p);
    PaintContentLayer(p);

    w.DrawImage(0, 0, ib);
}

void StageCard::PaintCardLayer(BufferPainter &p, const Size &sz) const	{
	    if (!(cardFillOn || (cardFrameOn && cardStrokeTh > 0))) return;
	    p.Begin();
	    (cardRadius > 0 ? p.RoundedRectangle(0.5, 0.5, sz.cx - 1.0, sz.cy - 1.0, cardRadius)
	                    : p.Rectangle(0.5, 0.5, sz.cx - 1.0, sz.cy - 1.0));
	    if (cardFillOn) p.Fill(cardFill);
	    if (cardFrameOn && cardStrokeTh > 0)
	    {
	        if (cardDashed) p.Dash(cardDash, 0.0);
	        p.Stroke(cardStrokeTh, cardStroke);
	    }
	    p.End();
	}
	
void StageCard::PaintHeaderLayer(BufferPainter& p) const {
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

    // header text
    if(!IsNull(title))    p.DrawText(titleX, titleY, title, titleFont, titleInk);
    if(titleLineTh > 0 && !IsNull(title))
        p.DrawRect(titleLineX, titleLineY, titleLineW, titleLineTh, titleLineColor);
    if(!IsNull(subTitle)) p.DrawText(subTitleX, subTitleY, subTitle, subTitleFont, subTitleInk);
    if(!IsNull(badge))    p.DrawText(badgeX,    badgeY,    badge,    badgeFont,    badgeInk);
}

void StageCard::PaintContentLayer(BufferPainter& p) const {
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
}

// ---- helpers ----
void StageCard::ClearChildren(ParentCtrl& p) {
    for(Ctrl *q = p.GetFirstChild(); q; ) { Ctrl* n = q->GetNext(); q->Remove(); q = n; }
}

Size StageCard::WrapBaseSize(Ctrl& c) const {
    Size ms = c.GetMinSize();
    int w = wrapItemW > 0 ? wrapItemW : ms.cx;
    int h = wrapItemH > 0 ? wrapItemH : ms.cy;
    return Size(max(1, w), max(1, h));
}

void StageCard::ContentLayoutPass(int pane_w, int pane_h) {
    virtualH = pane_h;

    if(contentLayout == FIXED) {
        const int inner_w = max(0, pane_w - contentInset.left - contentInset.right);
        const int inner_h = max(0, pane_h - contentInset.top  - contentInset.bottom);

        if(autoFill) {
            Vector<Ctrl*> vis;
            for(Ctrl* q = contentLayer.GetFirstChild(); q; q = q->GetNext())
                if(q->IsShown()) vis.Add(q);
            const int n = vis.GetCount();
            if(n == 0) { virtualH = pane_h; return; }
            if(n == 1) { vis[0]->SetRect(contentInset.left, contentInset.top, inner_w, inner_h); virtualH = pane_h; return; }
            const int gaps_h   = (n - 1) * contentGap.cy;
            const int avail_h  = max(0, inner_h - gaps_h);
            const int per_h    = n > 0 ? max(0, avail_h / n) : 0;
            int y = contentInset.top;
            for(int i = 0; i < n; ++i) {
                int h = (i == n - 1) ? (contentInset.top + inner_h - y) : per_h;
                h = max(0, h);
                vis[i]->SetRect(contentInset.left, y, inner_w, h);
                y += h;
                if(i + 1 < n) y += contentGap.cy;
            }
            virtualH = pane_h; return;
        }
        int max_bottom = contentInset.top;
        for(Ctrl *q = contentLayer.GetFirstChild(); q; q = q->GetNext())
            if(q->IsShown()) max_bottom = max(max_bottom, q->GetRect().bottom);
        virtualH = max(pane_h, max_bottom + contentInset.bottom); return;
    }

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
        virtualH = max(pane_h, content_h); return;
    }

    if(contentLayout == GRID) {
        const int inner_w = max(0, pane_w - contentInset.left - contentInset.right);
        const int inner_h = max(0, pane_h - contentInset.top  - contentInset.bottom);
        int nvis = 0; for(Ctrl *q = contentLayer.GetFirstChild(); q; q = q->GetNext()) if(q->IsShown()) ++nvis;
        const int cols = max(1, gridCols);
        const int rows = max(1, (nvis + cols - 1) / cols);
        int cell_w = 0, cell_h = 0;
        if(gridFill) {
            const int total_gap_x = (cols - 1) * contentGap.cx;
            const int total_gap_y = (rows - 1) * contentGap.cy;
            cell_w = cols > 0 ? max(1, (inner_w - total_gap_x) / cols) : inner_w;
            cell_h = rows > 0 ? max(1, (inner_h - total_gap_y) / rows) : inner_h;
            virtualH = pane_h;
        } else {
            cell_w = gridCellW > 0 ? gridCellW
                                   : max(1, (inner_w - (cols - 1) * contentGap.cx) / cols);
            int auto_h = 0;
            if(gridCellH <= 0) {
                for(Ctrl *q = contentLayer.GetFirstChild(); q; q = q->GetNext())
                    if(q->IsShown()) auto_h = max(auto_h, q->GetMinSize().cy);
                if(auto_h <= 0) auto_h = DPI(32);
            }
            cell_h = gridCellH > 0 ? gridCellH : auto_h;
            int content_h = contentInset.top + (rows > 0 ? rows * cell_h + (rows - 1) * contentGap.cy : 0) + contentInset.bottom;
            virtualH = max(pane_h, content_h);
        }
        int idx = 0;
        for(Ctrl *q = contentLayer.GetFirstChild(); q; q = q->GetNext()) {
            if(!q->IsShown()) continue;
            int col = idx % cols, row = idx / cols;
            int x = contentInset.left + col * (cell_w + contentGap.cx);
            int y = contentInset.top  + row * (cell_h + contentGap.cy);
            if(gridStretch || gridFill) q->SetRect(x, y, cell_w, cell_h);
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

} // namespace Upp

