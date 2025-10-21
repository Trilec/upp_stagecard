#ifndef _StageCard_StageCard_h_
#define _StageCard_StageCard_h_

#include <CtrlLib/CtrlLib.h>
#include <Painter/Painter.h>

namespace Upp {

class StageCard : public ParentCtrl {
public:
    typedef StageCard CLASSNAME;

    enum HeaderAlign   { LEFT, RIGHT, CENTER };
    enum ContentLayout { FIXED, WRAP, GRID };

    StageCard();

	// ---- setters (inline-style definitions kept compact) ----
	StageCard& SetTitle(const String& s, Font f = StdFont().Bold().Height(DPI(22)))   { title = s; titleFont = f; Layout(); return *this; }
    StageCard& SetSubTitle(const String& s, Font f = StdFont().Height(DPI(12)))   { subTitle = s; subTitleFont = f; Layout(); return *this; }
    StageCard& SetBadge(const String& s, Font f = StdFont().Height(DPI(12)))  { badge = s; badgeFont = f; Layout(); return *this; }
    StageCard& SetHeaderAlign(HeaderAlign a)      { headerAlign = a; Layout(); return *this; }
    StageCard& SetTitleUnderlineThickness(int th) { titleLineTh = max(0, th); Layout(); return *this; }
    StageCard& SetTitleUnderlineColor(Color c)    { titleLineColor = c; Refresh(); return *this; }

	// separate font setters (clarity over any implicit "size")
	StageCard& SetTitleFont(Font f) { titleFont = f; Layout(); return *this; }
	StageCard& SetSubTitleFont(Font f) { subTitleFont = f; Layout(); return *this; }
	StageCard& SetBadgeFont(Font f) { badgeFont = f; Layout(); return *this; }	

	// Frame toggles & colors (card)
	StageCard& EnableCardFrame(bool on= true)     { cardFrameOn = on; Refresh(); return *this; }
	StageCard& EnableCardFill(bool on= true)      { cardFillOn  = on; Refresh(); return *this; }
	StageCard& SetCardCornerRadius(int px)        { cardRadius  = max(DPI(0), px); Refresh(); return *this; }
	StageCard& SetCardStrokeThickness(int px)     { cardStrokeTh= max(0, px); Refresh(); return *this; }
	StageCard& SetCardDashPattern(const String& d){ cardDash    = d; Refresh(); return *this; }
	StageCard& EnableCardDashed(bool on= true)    { cardDashed  = on; Refresh(); return *this; }
	StageCard& SetCardColors(Color fill = Color(240,240,240),
	                         Color stroke = Color(200,200,200),
                             Color tInk = SColorText(),
                             Color stInk = SColorText(),
                             Color badgeInkC = SColorDisabled())
    { cardFill = fill; cardStroke = stroke; titleInk=tInk; subTitleInk=stInk; badgeInk=badgeInkC; Refresh(); return *this; }

	
	// Header frame
	StageCard& EnableHeaderFrame(bool on= true)     { headerFrameOn = on; Refresh(); return *this; }
	StageCard& EnableHeaderFill(bool on= true)      { headerFillOn  = on; Refresh(); return *this; }
	StageCard& SetHeaderCornerRadius(int px)        { headerRadius  = max(DPI(0), px); Refresh(); return *this; }
	StageCard& SetHeaderStrokeThickness(int px)     { headerStrokeTh= max(0, px); Refresh(); return *this; }
	StageCard& SetHeaderDashPattern(const String& d){ headerDash    = d; Refresh(); return *this; }
	StageCard& EnableHeaderDashed(bool on= false)   { headerDashed  = on; Refresh(); return *this; }
	StageCard& SetHeaderFrameColors(Color fill, Color stroke) { headerFill = fill; headerStroke = stroke; Refresh(); return *this; }
	
	// Content frame
	StageCard& EnableContentFrame(bool on= false)    { contentFrameOn = on; Refresh(); return *this; }
	StageCard& EnableContentFill(bool on= true)      { contentFillOn  = on; Refresh(); return *this; }
	StageCard& SetContentCornerRadius(int px)        { contentRadius  = max(DPI(0), px); Refresh(); return *this; }
	StageCard& SetContentStrokeThickness(int px)     { contentStrokeTh= max(0, px); Refresh(); return *this; }
	StageCard& SetContentDashPattern(const String& d){ contentDash    = d; Refresh(); return *this; }
	StageCard& EnableContentDashed(bool on= false)   { contentDashed  = on; Refresh(); return *this; }
	StageCard& SetContentFrameColors(Color fill, Color stroke) { contentFill = fill; contentStroke = stroke; Refresh(); return *this; }
	
	// Insets & gaps
	StageCard& SetInset(int all)                           { SetHeaderInset(all,all,all,all); SetContentInset(all,all,all,all); return *this; }
	StageCard& SetInset(int l, int t, int r, int b)        { SetHeaderInset(l,t,r,b); SetContentInset(l,t,r,b); return *this; }
	StageCard& SetHeaderInset(int l, int t, int r, int b)  { headerInset = Rect(max(0,l), max(0,t), max(0,r), max(0,b)); Layout(); return *this; }
	StageCard& SetHeaderGap(int v)                         { headerGapV = max(0, v); Layout(); return *this; }
	StageCard& SetContentInset(int l, int t, int r, int b) { contentInset = Rect(max(0,l), max(0,t), max(0,r), max(0,b)); Layout(); return *this; }
	StageCard& SetContentGap(int gx, int gy)               { contentGap = Size(max(0,gx), max(0,gy)); Layout(); return *this; }
	StageCard& SetGap(int g)                               { headerGapV = max(0,g); contentGap = Size(max(0,g), max(0,g)); Layout(); return *this; }
	
	// Child management
	StageCard& AddHeader(Ctrl& c)      { headerPane.Add(c); Layout(); return *this; }
	StageCard& ClearHeader()           { ClearChildren(headerPane); Layout(); return *this; }
	StageCard& AddContent(Ctrl& c)     { contentLayer.Add(c); contentDirty = true; Layout(); return *this; }
	StageCard& ClearContent()          { ClearChildren(contentLayer); contentDirty = true; Layout(); return *this; }
	StageCard& ReplaceContent(Ctrl& c) { ClearChildren(contentLayer); contentLayer.Add(c); contentDirty = true; Layout(); return *this; }
	
	// Behavior
	StageCard& EnableContentAutoFill(bool on= true)    { autoFill = on; Layout(); return *this; }
	StageCard& EnableContentScroll(bool on= true)      { scrollEnabled = on; Layout(); return *this; }
	StageCard& EnableContentClampToPane(bool on= true) { clampContentToPane = on; Layout(); return *this; }
	StageCard& SetMinContent(Size s)                   { minContent = s; Layout(); return *this; }
	StageCard& SetMaxContent(Size s)                   { maxContent = s; Layout(); return *this; }
	
	// Content layout modes
	StageCard& ContentAbsolute() { contentLayout = FIXED; Layout(); return *this; }
	StageCard& ContentWrap()     { contentLayout = WRAP;  Layout(); return *this; }
	StageCard& ContentGrid()     { contentLayout = GRID;  Layout(); return *this; }
	StageCard& WrapItemSize(int w, int h)  { wrapItemW=w;     wrapItemH=h; Layout(); return *this; }
    StageCard& WrapItemSize(Size s)        { wrapItemW=s.cx;  wrapItemH=s.cy; Layout(); return *this; }
	
	StageCard& GridCols(int n)             { gridCols = max(1, n); Layout(); return *this; }
	StageCard& GridCell(int w, int h)      { gridCellW=w; gridCellH=h; Layout(); return *this; }
	StageCard& GridStretch(bool on= false) { gridStretch=on; Layout(); return *this; }
	StageCard& GridFill(bool on= true)     { gridFill = on; Layout(); return *this; }

    // Size & layout
    Size GetMinSize() const override;
    void Layout() override;
    void Paint(Draw& w) override;
    void MouseWheel(Point p, int zdelta, dword keyflags) override; // wheel fix

private:
    // --- state ---
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

    bool    cardFrameOn  = true;
    bool    cardFillOn   = true;
    int     cardRadius   = DPI(6);
    int     cardStrokeTh = 1;
    bool    cardDashed   = false;
    String  cardDash     = "5,5";
    Color   cardFill     = Color(240,240,240);
    Color   cardStroke   = SColorShadow();

    bool    headerFrameOn  = false;
    bool    headerFillOn   = false;
    int     headerRadius   = DPI(0);
    int     headerStrokeTh = 1;
    bool    headerDashed   = false;
    String  headerDash     = "5,5";
    Color   headerFill     = Color(255,255,255);
    Color   headerStroke   = GrayColor(160);

    bool    contentFrameOn  = false;
    bool    contentFillOn   = false;
    int     contentRadius   = DPI(0);
    int     contentStrokeTh = 1;
    bool    contentDashed   = false;
    String  contentDash     = "5,5";
    Color   contentFill     = Color(255,255,255);
    Color   contentStroke   = GrayColor(160);

    bool    autoFill = false;

    int     cardPadX = DPI(0), cardPadY = DPI(0);
    Rect    headerInset  = Rect(DPI(8), DPI(8), DPI(8), DPI(8));
    int     headerGapV   = DPI(4);
    Rect    contentInset = Rect(DPI(8), DPI(8), DPI(8), DPI(8));
    Size    contentGap   = Size(DPI(6), DPI(6));

    ParentCtrl headerPane;
    ParentCtrl contentPane;
    ParentCtrl contentLayer;

    ScrollBar  vbar;
    bool       scrollEnabled = true;
    int        scroll_y = 0;

    ContentLayout contentLayout = FIXED;
    int       wrapItemW = 0, wrapItemH = 0;
    int       gridCols   = 3, gridCellW  = 0, gridCellH  = 0;
    bool      gridStretch= true;
    bool      gridFill = false;

    bool      clampContentToPane = true;
    Size      minContent = Size(0,0), maxContent = Size(0,0);

    int       cachedHeaderMin = DPI(10);
    Rect      lastHeaderRc, lastContentRc;
    int       titleX=0, titleY=0, titleW=0;
    int       subTitleX=0, subTitleY=0, subTitleW=0;
    int       badgeX=0, badgeY=0, badgeW=0;

    int       virtualH = DPI(10);
    bool      contentDirty = true;

    // helpers
    static void ClearChildren(ParentCtrl& p);
    Size WrapBaseSize(Ctrl& c) const;
    void ContentLayoutPass(int pane_w, int pane_h);

    // small paint/layout helpers (refactor points)
    void PaintCardLayer(BufferPainter& p, const Size& sz) const;
    void PaintHeaderLayer(BufferPainter& p) const;
    void PaintContentLayer(BufferPainter& p) const;
};

} // namespace Upp
#endif

