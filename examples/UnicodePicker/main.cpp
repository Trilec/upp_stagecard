
#include <CtrlLib/CtrlLib.h>
#include <Painter/Painter.h>
#include <StageCard/StageCard.h>   // needed for demo StageCard

using namespace Upp;

// ---------------------------------------------------------------------
// Basic symbol types used across the picker/cards
// ---------------------------------------------------------------------
struct SymbolItem : Moveable<SymbolItem> {
    String charCode;     // UTF-8 glyph (e.g., "→")
    String name;         // Human-friendly name (e.g., "Right Arrow")
    String categoryKey;  // Category key (e.g., "nav1")

    SymbolItem() {}
    SymbolItem(const String& code, const String& n, const String& catKey)
        : charCode(code), name(n), categoryKey(catKey) {}
};

struct SymbolCategory : Moveable<SymbolCategory> {
    String name;             // Display name (with numeric prefix if you like)
    String key;              // Stable key used for filtering
    Color  col = Color(90,90,90);   // pastel category color
    Vector<SymbolItem> symbols;
};

// Global category list
Vector<SymbolCategory> allCategories;

// ---------------------------------------------------------------------
// InitData — populate allCategories with demo data (with pastel colors)
// ---------------------------------------------------------------------
static inline void InitData() {
    allCategories.Clear();

	const Color P1  = Color(120,170,235);  // cornflower-ish
	const Color P2  = Color(110,162,230);
	const Color P3  = Color(105,155,225);
	const Color P4  = Color(100,150,220);
	const Color P5  = Color( 95,145,215);
	const Color P6  = Color( 90,140,210);
	const Color P7  = Color( 85,135,205);
	const Color P8  = Color( 80,130,200);
	const Color P9  = Color( 76,126,196);
	const Color P10 = Color( 72,122,192);
	const Color P11 = Color( 68,118,188);
	const Color P12 = Color( 64,114,184);
	const Color P13 = Color( 60,110,180);
	const Color P14 = Color( 56,106,176);
	const Color P15 = Color( 52,102,172);
	const Color P16 = Color( 48, 98,168);
	const Color P17 = Color( 44, 94,164);
	const Color P18 = Color( 40, 90,160);
	const Color P19 = Color( 36, 86,156);
	const Color P20 = Color( 32, 82,152);

    // 1. Navigation & Arrows 1.0
    {
        SymbolCategory cat;
        cat.name = "Navigation & Arrows 1.0";
        cat.key  = "nav1";
        cat.col  = P1;
        cat.symbols.Add(SymbolItem{"→", "Right Arrow", "nav1"});
        cat.symbols.Add(SymbolItem{"←", "Left Arrow",  "nav1"});
        cat.symbols.Add(SymbolItem{"↑", "Up Arrow",    "nav1"});
        cat.symbols.Add(SymbolItem{"↓", "Down Arrow",  "nav1"});
        cat.symbols.Add(SymbolItem{"►", "Right Triangle", "nav1"});
        cat.symbols.Add(SymbolItem{"▼", "Down Triangle",  "nav1"});
        cat.symbols.Add(SymbolItem{"«", "Double Left",  "nav1"});
        cat.symbols.Add(SymbolItem{"»", "Double Right", "nav1"});
        allCategories.Add(pick(cat));
    }

    // 2. Toggle & State
    {
        SymbolCategory cat;
        cat.name = "Toggle & State";
        cat.key  = "toggle";
        cat.col  = P2;
        cat.symbols.Add(SymbolItem{"+", "Plus (Expand)",      "toggle"});
        cat.symbols.Add(SymbolItem{"−", "Minus (Collapse)",   "toggle"});
        cat.symbols.Add(SymbolItem{"✓", "Checkmark",          "toggle"});
        cat.symbols.Add(SymbolItem{"✗", "Ballot X",           "toggle"});
        cat.symbols.Add(SymbolItem{"□", "Unchecked Box",      "toggle"});
        cat.symbols.Add(SymbolItem{"☑", "Checked Box",        "toggle"});
        cat.symbols.Add(SymbolItem{"•", "Bullet Point",       "toggle"});
        cat.symbols.Add(SymbolItem{"⊛", "Circled Star",       "toggle"});
        allCategories.Add(pick(cat));
    }

    // 3. Status & Alert
    {
        SymbolCategory cat;
        cat.name = "Status & Alert";
        cat.key  = "status";
        cat.col  = P3;
        cat.symbols.Add(SymbolItem{"ℹ️", "Info Icon",        "status"});
        cat.symbols.Add(SymbolItem{"⚠️", "Warning Sign",     "status"});
        cat.symbols.Add(SymbolItem{"❌", "Error/Close",      "status"});
        cat.symbols.Add(SymbolItem{"🔒", "Locked/Secure",    "status"});
        cat.symbols.Add(SymbolItem{"🔔", "Bell/Notification","status"});
        cat.symbols.Add(SymbolItem{"⚡",  "High Priority",    "status"});
        cat.symbols.Add(SymbolItem{"💡", "Idea/Tip",         "status"});
        cat.symbols.Add(SymbolItem{"🔥", "Hot/Popular",      "status"});
        allCategories.Add(pick(cat));
    }

    // 4. Structural & Layout
    {
        SymbolCategory cat;
        cat.name = "Structural & Layout";
        cat.key  = "layout";
        cat.col  = P4;
        cat.symbols.Add(SymbolItem{"▥", "Square Blocks 1 (Panel)", "layout"});
        cat.symbols.Add(SymbolItem{"▤", "Square Blocks 2",         "layout"});
        cat.symbols.Add(SymbolItem{"▦", "Square Blocks 3 (Grid)",  "layout"});
        cat.symbols.Add(SymbolItem{"≡", "Hamburger Menu",          "layout"});
        cat.symbols.Add(SymbolItem{"⋮", "Vertical Ellipsis",       "layout"});
        cat.symbols.Add(SymbolItem{"⋯", "Horizontal Ellipsis",     "layout"});
        cat.symbols.Add(SymbolItem{"⌗", "Hash/Number",             "layout"});
        cat.symbols.Add(SymbolItem{"…", "More Dots",               "layout"});
        allCategories.Add(pick(cat));
    }

    // 5. Files & Document
    {
        SymbolCategory cat;
        cat.name = "Files & Document";
        cat.key  = "files";
        cat.col  = P5;
        cat.symbols.Add(SymbolItem{"📁", "Folder",          "files"});
        cat.symbols.Add(SymbolItem{"📄", "File/Document",   "files"});
        cat.symbols.Add(SymbolItem{"💾", "Save Disk",       "files"});
        cat.symbols.Add(SymbolItem{"🖼️", "Image/Media",     "files"});
        cat.symbols.Add(SymbolItem{"🔗", "Link/External",   "files"});
        cat.symbols.Add(SymbolItem{"🔍", "Search/Zoom",     "files"});
        cat.symbols.Add(SymbolItem{"✂️", "Scissors (Cut)",  "files"});
        cat.symbols.Add(SymbolItem{"📥", "Inbox/Receive",   "files"});
        allCategories.Add(pick(cat));
    }

    // 6. Time & Date
    {
        SymbolCategory cat;
        cat.name = "Time & Date";
        cat.key  = "time";
        cat.col  = P6;
        cat.symbols.Add(SymbolItem{"📅", "Calendar/Date", "time"});
        cat.symbols.Add(SymbolItem{"⏱️", "Stopwatch/Time","time"});
        cat.symbols.Add(SymbolItem{"⌚",  "Watch/Time",    "time"});
        cat.symbols.Add(SymbolItem{"⏳", "Hourglass",     "time"});
        cat.symbols.Add(SymbolItem{"↩️", "Undo/Back",     "time"});
        cat.symbols.Add(SymbolItem{"↪️", "Redo/Forward",  "time"});
        cat.symbols.Add(SymbolItem{"🔅", "Brightness",    "time"});
        cat.symbols.Add(SymbolItem{"🌙", "Night Mode",    "time"});
        allCategories.Add(pick(cat));
    }

    // 7. Math & Logic
    {
        SymbolCategory cat;
        cat.name = "Math & Logic";
        cat.key  = "math";
        cat.col  = P7;
        cat.symbols.Add(SymbolItem{"=",  "Equals",           "math"});
        cat.symbols.Add(SymbolItem{"≠",  "Not Equal",        "math"});
        cat.symbols.Add(SymbolItem{"≈",  "Almost Equal",     "math"});
        cat.symbols.Add(SymbolItem{"≤",  "Less or Equal",    "math"});
        cat.symbols.Add(SymbolItem{"≥",  "Greater or Equal", "math"});
        cat.symbols.Add(SymbolItem{"∑",  "Summation",        "math"});
        cat.symbols.Add(SymbolItem{"∞",  "Infinity",         "math"});
        cat.symbols.Add(SymbolItem{"÷",  "Division Sign",    "math"});
        allCategories.Add(pick(cat));
    }

    // 8. People & Contact
    {
        SymbolCategory cat;
        cat.name = "People & Contact";
        cat.key  = "people";
        cat.col  = P8;
        cat.symbols.Add(SymbolItem{"👤", "User (Single)", "people"});
        cat.symbols.Add(SymbolItem{"👥", "Users (Group)", "people"});
        cat.symbols.Add(SymbolItem{"🏠", "Home/Main",     "people"});
        cat.symbols.Add(SymbolItem{"📍", "Location Pin",  "people"});
        cat.symbols.Add(SymbolItem{"🗺️", "Map/Guide",     "people"});
        cat.symbols.Add(SymbolItem{"✍️", "Writing/Edit",  "people"});
        cat.symbols.Add(SymbolItem{"📞", "Phone/Call",    "people"});
        cat.symbols.Add(SymbolItem{"📧", "Email",         "people"});
        allCategories.Add(pick(cat));
    }

    // 9. Media & Controls
    {
        SymbolCategory cat;
        cat.name = "Media & Controls";
        cat.key  = "media";
        cat.col  = P9;
        cat.symbols.Add(SymbolItem{"▶️", "Play/Start",    "media"});
        cat.symbols.Add(SymbolItem{"⏸️", "Pause",         "media"});
        cat.symbols.Add(SymbolItem{"⏹️", "Stop",          "media"});
        cat.symbols.Add(SymbolItem{"🔇", "Mute",          "media"});
        cat.symbols.Add(SymbolItem{"🔊", "Volume Max",    "media"});
        cat.symbols.Add(SymbolItem{"🎧", "Headphones",    "media"});
        cat.symbols.Add(SymbolItem{"⬇️", "Download",      "media"});
        cat.symbols.Add(SymbolItem{"⬆️", "Upload",        "media"});
        allCategories.Add(pick(cat));
    }

    // 10. Tools & Settings
    {
        SymbolCategory cat;
        cat.name = "Tools & Settings";
        cat.key  = "tools";
        cat.col  = P10;
        cat.symbols.Add(SymbolItem{"⚙️", "Gear/Settings",   "tools"});
        cat.symbols.Add(SymbolItem{"🛠️", "Hammer/Tools",    "tools"});
        cat.symbols.Add(SymbolItem{"🔑", "Key/Access",       "tools"});
        cat.symbols.Add(SymbolItem{"✏️", "Pencil/Edit",      "tools"});
        cat.symbols.Add(SymbolItem{"🗑️", "Trash/Delete",     "tools"});
        cat.symbols.Add(SymbolItem{"🖨️", "Printer",          "tools"});
        cat.symbols.Add(SymbolItem{"🧾", "Receipt/Billing",  "tools"});
        cat.symbols.Add(SymbolItem{"📈", "Chart/Analytics",  "tools"});
        allCategories.Add(pick(cat));
    }

    // 11. Communication & Text
    {
        SymbolCategory cat;
        cat.name = "Communication & Text";
        cat.key  = "comm";
        cat.col  = P11;
        cat.symbols.Add(SymbolItem{"💬", "Chat Bubble",             "comm"});
        cat.symbols.Add(SymbolItem{"📣", "Megaphone/Announce",      "comm"});
        cat.symbols.Add(SymbolItem{"🗣️", "Speaking Head",           "comm"});
        cat.symbols.Add(SymbolItem{"📢", "Loudspeaker",             "comm"});
        cat.symbols.Add(SymbolItem{"🎙️", "Microphone",             "comm"});
        cat.symbols.Add(SymbolItem{"🔤", "Input Latin Letters",     "comm"});
        cat.symbols.Add(SymbolItem{"…",  "Ellipsis/More",           "comm"});
        cat.symbols.Add(SymbolItem{"¶",  "Pilcrow (Paragraph)",     "comm"});
        allCategories.Add(pick(cat));
    }

    // 12. Shapes & Geometry
    {
        SymbolCategory cat;
        cat.name = "Shapes & Geometry";
        cat.key  = "shapes";
        cat.col  = P12;
        cat.symbols.Add(SymbolItem{"■", "Black Square",         "shapes"});
        cat.symbols.Add(SymbolItem{"□", "White Square",         "shapes"});
        cat.symbols.Add(SymbolItem{"●", "Black Circle",         "shapes"});
        cat.symbols.Add(SymbolItem{"○", "White Circle",         "shapes"});
        cat.symbols.Add(SymbolItem{"◆", "Black Diamond",        "shapes"});
        cat.symbols.Add(SymbolItem{"◇", "White Diamond",        "shapes"});
        cat.symbols.Add(SymbolItem{"△", "White Triangle Up",    "shapes"});
        cat.symbols.Add(SymbolItem{"▽", "White Triangle Down",  "shapes"});
        allCategories.Add(pick(cat));
    }

    // 13. Currency & Finance
    {
        SymbolCategory cat;
        cat.name = "Currency & Finance";
        cat.key  = "currency";
        cat.col  = P13;
        cat.symbols.Add(SymbolItem{"$",  "Dollar Sign",  "currency"});
        cat.symbols.Add(SymbolItem{"€",  "Euro Sign",    "currency"});
        cat.symbols.Add(SymbolItem{"£",  "Pound Sign",   "currency"});
        cat.symbols.Add(SymbolItem{"¥",  "Yen Sign",     "currency"});
        cat.symbols.Add(SymbolItem{"💰", "Money Bag",    "currency"});
        cat.symbols.Add(SymbolItem{"🪙", "Coin",         "currency"});
        cat.symbols.Add(SymbolItem{"💳", "Credit Card",  "currency"});
        cat.symbols.Add(SymbolItem{"🏦", "Bank",         "currency"});
        allCategories.Add(pick(cat));
    }

    // 14. Arrows 2.0 (Curved/Move)
    {
        SymbolCategory cat;
        cat.name = "Arrows 2.0 (Curved/Move)";
        cat.key  = "nav2";
        cat.col  = P14;
        cat.symbols.Add(SymbolItem{"↱", "Up Right Arrow",       "nav2"});
        cat.symbols.Add(SymbolItem{"↲", "Up Left Arrow",        "nav2"});
        cat.symbols.Add(SymbolItem{"↶", "Curved Left",          "nav2"});
        cat.symbols.Add(SymbolItem{"↷", "Curved Right",         "nav2"});
        cat.symbols.Add(SymbolItem{"⤓", "Down Arrow with Line", "nav2"});
        cat.symbols.Add(SymbolItem{"⤒", "Up Arrow with Line",   "nav2"});
        cat.symbols.Add(SymbolItem{"⇥", "Tab Arrow Right",      "nav2"});
        cat.symbols.Add(SymbolItem{"⇤", "Tab Arrow Left",       "nav2"});
        allCategories.Add(pick(cat));
    }

    // 15. Accessibility & Gestures
    {
        SymbolCategory cat;
        cat.name = "Accessibility & Gestures";
        cat.key  = "access";
        cat.col  = P15;
        cat.symbols.Add(SymbolItem{"♿",  "Wheelchair Symbol", "access"});
        cat.symbols.Add(SymbolItem{"👁️", "Eye/Visibility",    "access"});
        cat.symbols.Add(SymbolItem{"👂",  "Ear/Hearing",       "access"});
        cat.symbols.Add(SymbolItem{"☝️", "Index Finger Up",   "access"});
        cat.symbols.Add(SymbolItem{"👌",  "OK Hand",           "access"});
        cat.symbols.Add(SymbolItem{"🤚",  "Raised Hand",       "access"});
        cat.symbols.Add(SymbolItem{"🗣️", "Speaking Head",     "access"});
        cat.symbols.Add(SymbolItem{"🫲",  "Handshake",         "access"});
        allCategories.Add(pick(cat));
    }

    // 16. Keyboard & Input
    {
        SymbolCategory cat;
        cat.name = "Keyboard & Input";
        cat.key  = "keyboard";
        cat.col  = P16;
        cat.symbols.Add(SymbolItem{"⌃", "Control Key",     "keyboard"});
        cat.symbols.Add(SymbolItem{"⌥", "Option Key",      "keyboard"});
        cat.symbols.Add(SymbolItem{"⌘", "Command Key",     "keyboard"});
        cat.symbols.Add(SymbolItem{"↩", "Return Key",      "keyboard"});
        cat.symbols.Add(SymbolItem{"⇧", "Shift Key",       "keyboard"});
        cat.symbols.Add(SymbolItem{"⇥", "Tab Key",         "keyboard"});
        cat.symbols.Add(SymbolItem{"⌦", "Delete Forward",  "keyboard"});
        cat.symbols.Add(SymbolItem{"⌫", "Delete Backward", "keyboard"});
        allCategories.Add(pick(cat));
    }

    // 17. Weather & Environment
    {
        SymbolCategory cat;
        cat.name = "Weather & Environment";
        cat.key  = "weather";
        cat.col  = P17;
        cat.symbols.Add(SymbolItem{"☀️", "Sun",          "weather"});
        cat.symbols.Add(SymbolItem{"☁️", "Cloud",        "weather"});
        cat.symbols.Add(SymbolItem{"☔",  "Rain/Umbrella","weather"});
        cat.symbols.Add(SymbolItem{"❄️", "Snowflake",    "weather"});
        cat.symbols.Add(SymbolItem{"🌡️", "Thermometer",  "weather"});
        cat.symbols.Add(SymbolItem{"🌀",  "Cyclone",      "weather"});
        cat.symbols.Add(SymbolItem{"🌍",  "Globe",        "weather"});
        cat.symbols.Add(SymbolItem{"🌿",  "Plant/Nature", "weather"});
        allCategories.Add(pick(cat));
    }

    // 18. Rating & Feedback
    {
        SymbolCategory cat;
        cat.name = "Rating & Feedback";
        cat.key  = "rating";
        cat.col  = P18;
        cat.symbols.Add(SymbolItem{"⭐",  "Star",              "rating"});
        cat.symbols.Add(SymbolItem{"♡",  "Heart (Outline)",   "rating"});
        cat.symbols.Add(SymbolItem{"♥",  "Heart (Filled)",    "rating"});
        cat.symbols.Add(SymbolItem{"👍", "Thumbs Up",         "rating"});
        cat.symbols.Add(SymbolItem{"👎", "Thumbs Down",       "rating"});
        cat.symbols.Add(SymbolItem{"💯", "Hundred Points",    "rating"});
        cat.symbols.Add(SymbolItem{"🎯", "Target/Goal",       "rating"});
        cat.symbols.Add(SymbolItem{"🏅", "Medal/Award",       "rating"});
        allCategories.Add(pick(cat));
    }

    // 19. Scientific & Technical
    {
        SymbolCategory cat;
        cat.name = "Scientific & Technical";
        cat.key  = "science";
        cat.col  = P19;
        cat.symbols.Add(SymbolItem{"°", "Degree Symbol",        "science"});
        cat.symbols.Add(SymbolItem{"µ", "Micro Sign",           "science"});
        cat.symbols.Add(SymbolItem{"Ω", "Ohm",                  "science"});
        cat.symbols.Add(SymbolItem{"∆", "Delta/Change",         "science"});
        cat.symbols.Add(SymbolItem{"λ", "Lambda",               "science"});
        cat.symbols.Add(SymbolItem{"∅", "Empty Set/Diameter",   "science"});
        cat.symbols.Add(SymbolItem{"🧪", "Test Tube",           "science"});
        cat.symbols.Add(SymbolItem{"🧬", "DNA",                 "science"});
        allCategories.Add(pick(cat));
    }

    // 20. Punctuation & Typography
    {
        SymbolCategory cat;
        cat.name = "Punctuation & Typography";
        cat.key  = "punc";
        cat.col  = P20;
        cat.symbols.Add(SymbolItem{"«", "Left Guillemet",  "punc"});
        cat.symbols.Add(SymbolItem{"»", "Right Guillemet", "punc"});
        cat.symbols.Add(SymbolItem{"—", "Em Dash",         "punc"});
        cat.symbols.Add(SymbolItem{"–", "En Dash",         "punc"});
        cat.symbols.Add(SymbolItem{"§", "Section Sign",    "punc"});
        cat.symbols.Add(SymbolItem{"†", "Dagger",          "punc"});
        cat.symbols.Add(SymbolItem{"©", "Copyright",       "punc"});
        cat.symbols.Add(SymbolItem{"®", "Registered",      "punc"});
        allCategories.Add(pick(cat));
    }
}

// ======================================================================
// TitleDragButton — a draggable, paintable button
//  - Payload: SYMBOL (big glyph + small title) or CATEGORY (centered name)
//  - Modes:  SOURCE (starts Internal DnD) / BIN (click-to-remove)
//  - Styling knobs: radius, line thickness, dashed frame, fill toggle,
//                   normal/active face & inks, hover tint
// ======================================================================
class TitleDragButton : public Button {
public:
    typedef TitleDragButton CLASSNAME;
    enum Mode    { SOURCE, BIN };
    enum Payload { SYMBOL, CATEGORY };

    // ---------- lifecycle ----------
    TitleDragButton() {
        SetMinSize(Size(DPI(60), DPI(60)));

        // Fonts / inks
        glyphFont = StdFont().Bold().Height(DPI(18));
        nameFont  = StdFont().Height(DPI(9));
        catFont   = StdFont().Bold().Height(DPI(9));

        glyphInk  = SColorText();
        nameInk   = SColorText();
        catInk    = SColorText();

        // Defaults
        face       = Blend(SColorPaper(), SColorFace(), 20);
        border     = SColorShadow();
        activeFace = Color(22, 86, 160);       // dark blue for "selected"
        activeInk  = SColorHighlightText();    // typically white
        activeBorder = SColorShadow();

        radius        = DPI(4);
        lineThickness = 1;
        dashedFrame   = false;
        dashPattern   = "5,5";
        fillBody      = true;

        hoverTintPct  = 30;   // how much SColorHighlight to blend on hover
    }

    // ---------- payload setters ----------
    TitleDragButton& SetSymbol(const SymbolItem& s) { payload = SYMBOL; sym = s; Refresh(); return *this; }
    const SymbolItem& GetSymbol() const { return sym; }

    TitleDragButton& SetCategory(const SymbolCategory& c) {
        payload = CATEGORY;
        catKey  = c.key;
        catName = c.name;
        // For categories we like a pastel derived from the category color:
        SetFace(Blend(SColorPaper(), c.col, 70));
        Refresh();
        return *this;
    }
    const String& GetCategoryKey() const { return catKey; }

    TitleDragButton& SetMode(Mode m) { mode = m; Refresh(); return *this; }

    // ---------- styling knobs ----------
    TitleDragButton& SetRadius(int px)                 { radius = max(DPI(2), px); Refresh(); return *this; }
    TitleDragButton& SetLineThickness(int th)          { lineThickness = max(0, th); Refresh(); return *this; }
    TitleDragButton& EnableDashedFrame(bool on = true) { dashedFrame = on; Refresh(); return *this; }
    TitleDragButton& SetDashPattern(const String& d)   { dashPattern = d; Refresh(); return *this; }
    TitleDragButton& EnableFill(bool on = true)        { fillBody = on; Refresh(); return *this; }

    // Colors (normal)
    TitleDragButton& SetFace(Color c)                  { face = c; Refresh(); return *this; }
    TitleDragButton& SetBorder(Color c)                { border = c; Refresh(); return *this; }
    TitleDragButton& SetTextInks(Color mainInk, Color subInk) { glyphInk = mainInk; nameInk = subInk; Refresh(); return *this; }

    // Colors (active/selected)
    TitleDragButton& SetActiveColors(Color faceC, Color borderC, Color textInk = SColorHighlightText()) {
        activeFace = faceC; activeBorder = borderC; activeInk = textInk; Refresh(); return *this;
    }

    // Selection state (e.g. current category)
    TitleDragButton& SetSelected(bool on = true) { selected = on; Refresh(); return *this; }
    bool IsSelected() const { return selected; }

    // Hover emphasis amount (0..100 = stronger)
    TitleDragButton& SetHoverTintPct(int pct) { hoverTintPct = ::clamp(pct, 0, 100); Refresh(); return *this; }

	// Chainable size helpers (wrap the void-returning SetMinSize)
	TitleDragButton& SetTileSize(Size s)          { Button::SetMinSize(s); return *this; }
	TitleDragButton& SetTileSize(int w, int h)    { return SetTileSize(Size(w, h)); }

    // ---------- DnD ----------
    Image MakeDragSample() const {
        Size sz = GetSize();
        if(sz.cx <= 0 || sz.cy <= 0) return Image();
        ImageBuffer ib(sz);
        Fill(~ib, RGBAZero(), ib.GetLength());
        { BufferPainter p(ib); const_cast<TitleDragButton*>(this)->Paint(p); }
        return Image(ib);
    }

    virtual void LeftDrag(Point, dword) override {
        if(mode != SOURCE) return;
        if(payload == SYMBOL)
            DoDragAndDrop(InternalClip(*this, "symbol"),   MakeDragSample(), DND_COPY);
        else
            DoDragAndDrop(InternalClip(*this, "category"), MakeDragSample(), DND_COPY);
    }

	virtual void LeftDown(Point p, dword k) override {
	    if(mode == BIN) {
	        if(WhenRemove)
	            PostCallback(WhenRemove);   // defer to end of event loop
	        return;                          // do NOT call Button::LeftDown
	    }
	    Button::LeftDown(p, k);
	}


    // ---------- paint ----------
    virtual void Paint(Draw& w) override {
        // Compute current face/border/ink according to state
        bool over  = HasMouse();

        Color curFace   = selected ? activeFace : face;
        Color curBorder = selected ? activeBorder : border;
        Color curGlyph  = selected ? activeInk  : glyphInk;
        Color curName   = selected ? activeInk  : nameInk;
        Color curCat    = selected ? activeInk  : catInk;

        if(over && !selected) {
            curFace = Blend(curFace, SColorHighlight(), hoverTintPct);
        }

        const Size sz = GetSize();
        const int  x = 0, y = 0, wdt = sz.cx, hgt = sz.cy;
        const int  r  = radius;
        const int  bw = lineThickness;

        // Draw body + optional frame with Painter for crisp corners
        {
            ImageBuffer ib(sz);
            Fill(~ib, RGBAZero(), ib.GetLength());
            BufferPainter p(ib);

            p.Begin();
            if(fillBody) {
                (r ? p.RoundedRectangle(x + 0.5, y + 0.5, wdt - 1.0, hgt - 1.0, r)
                   : p.Rectangle(x + 0.5, y + 0.5, wdt - 1.0, hgt - 1.0));
                p.Fill(curFace);
            }
            if(bw > 0) {
                (r ? p.RoundedRectangle(x + 0.5, y + 0.5, wdt - 1.0, hgt - 1.0, r)
                   : p.Rectangle(x + 0.5, y + 0.5, wdt - 1.0, hgt - 1.0));
                if(dashedFrame) p.Dash(dashPattern, 0.0);
                p.Stroke(bw, curBorder);
            }
            p.End();

            w.DrawImage(0, 0, ib);
        }

        // Inner content area
        Rect rct = Rect(sz).Deflated(DPI(6), DPI(4));

        if(payload == SYMBOL) {
            // big glyph + small name centered
            Size gsz = GetTextSize(sym.charCode, glyphFont);
            Size nsz = GetTextSize(sym.name,     nameFont);
            int total_h = gsz.cy + DPI(4) + nsz.cy;
            int y0 = rct.top + (rct.GetHeight() - total_h)/2;

            int gx = rct.left + (rct.GetWidth() - gsz.cx)/2;
            w.DrawText(gx, y0, sym.charCode, glyphFont, curGlyph);

            int nx = rct.left + (rct.GetWidth() - nsz.cx)/2;
            w.DrawText(nx, y0 + gsz.cy + DPI(4), sym.name, nameFont, curName);
        }
        else {
            // centered category name
            Size ts = GetTextSize(catName, catFont);
            int x0 = rct.left + (rct.GetWidth() - ts.cx)/2;
            int y0 = rct.top  + (rct.GetHeight() - ts.cy)/2;
            w.DrawText(x0, y0, catName, catFont, curCat);
        }
    }

    // ---------- signals ----------
    Callback WhenRemove;

private:
    // payload
    Payload   payload = SYMBOL;
    SymbolItem sym;
    String     catKey, catName;

    // mode/state
    Mode   mode     = SOURCE;
    bool   selected = false;

    // look
    Font   glyphFont, nameFont, catFont;
    Color  glyphInk,  nameInk,  catInk;

    // body/frame
    int     radius        = 0;
    int     lineThickness = 1;
    bool    dashedFrame   = false;
    String  dashPattern   = "5,5";
    bool    fillBody      = true;

    // colors
    Color face, border;
    Color activeFace, activeBorder, activeInk;

    int   hoverTintPct = 30; // 0..100 blend amount
};


// ======================================================================
// DropBinCard — StageCard subclass; whole card is a drop target
//  - Accepts both "symbol" and "category" drags
// ======================================================================
class DropBinCard : public StageCard {
public:
    typedef DropBinCard CLASSNAME;

    Callback1<const SymbolItem&> WhenAdded;
    Callback1<const SymbolItem&> WhenRemoved;
    Callback                     WhenListChanged;
    int index = -1; // optional container index (BIN uses it for remove)
    
    DropBinCard() {
        SetTitle("Selection Bin (Drag & Drop Here)")
            .SetHeaderAlign(StageCard::LEFT)
            .EnableCardFill(false)
            .EnableCardFrame(false)
            .EnableContentFill(true)
            .EnableContentFrame(true)
            .EnableContentDashed(true)
            .SetContentCornerRadius(6)
            .SetContentStrokeThickness(2)
            .SetCardColors(Color(245,245,245), SColorShadow())
            .SetContentFrameColors(SColorPaper(), GrayColor(160));
        baseContentFill   = SColorPaper();
        baseContentStroke = GrayColor(160);

        ContentWrap();
        EnableContentScroll(true);
        EnableContentClampToPane(true);
        SetContentInset(DPI(8), DPI(8), DPI(8), DPI(8));
        SetContentGap(DPI(4), DPI(4));
        WrapItemSize(tileSize);
        SetMinContent(Size(0, DPI(72)));
    }

    // ----------------------- Knobs -----------------------
    DropBinCard& SetTileSize(Size s)        { tileSize = s; WrapItemSize(s); Reflow(); return *this; }
    DropBinCard& SetTileSize(int w,int h)   { tileSize = Size(w,h); WrapItemSize(w,h); Reflow(); return *this; }
    DropBinCard& SetTileGap(int g)          { SetContentGap(g, g); Reflow(); return *this; }
    DropBinCard& SetTileGap(int gx,int gy)  { SetContentGap(gx, gy); Reflow(); return *this; }
    DropBinCard& SetContentInsets(int l,int t,int r,int b) { SetContentInset(l,t,r,b); Reflow(); return *this; }
    DropBinCard& HighlightOnDrop(bool on)   { highlightOnDrop = on; return *this; }

    // ------------------------ API ------------------------
    const Vector<SymbolItem>& GetSymbols() const { return items; }

    void AddSymbolUnique(const SymbolItem& s) {
        for(const auto& x : items)
            if(x.charCode == s.charCode)
                return;
        items.Add(s);
        if(WhenAdded) WhenAdded(s);
        RebuildTiles();
        FireListChanged();
    }

    void RemoveIndex(int i) {
        if(i < 0 || i >= items.GetCount()) return;
        SymbolItem removed = items[i];
        items.Remove(i);
        if(WhenRemoved) WhenRemoved(removed);
        RebuildTiles();
        FireListChanged();
    }

    void ClearAll() {
        items.Clear();
        RebuildTiles();
        FireListChanged();
    }

    // ------------------ DnD (whole card) ------------------
    virtual void DragAndDrop(Point, PasteClip& d) override {
        bool ok_symbol   = AcceptInternal<TitleDragButton>(d, "symbol");
        bool ok_category = AcceptInternal<TitleDragButton>(d, "category");
        bool ok = ok_symbol || ok_category;

        if(ok && highlightOnDrop) {
            SetContentFrameColors(baseContentFill, SColorHighlight());
            Refresh();
        }

        if(!ok) return;

        d.SetAction(DND_COPY);
        d.Accept();

        if(!d.IsPaste()) return;

        const TitleDragButton& src = GetInternal<TitleDragButton>(d);
        if(ok_symbol) {
            AddSymbolUnique(src.GetSymbol());
        } else {
            // category drop: add all items of that category
            const String key = src.GetCategoryKey();
            for(const auto& cat : allCategories)
                if(cat.key == key)
                    for(const auto& it : cat.symbols)
                        AddSymbolUnique(it);
        }
    }

    virtual void DragLeave() override {
        if(highlightOnDrop) {
            SetContentFrameColors(baseContentFill, baseContentStroke);
            Refresh();
        }
    }

    virtual void Paint(Draw& w) override {
        StageCard::Paint(w);
        if(items.IsEmpty()) {
            Size sz = GetSize();
            String t = "Drag symbols or a whole category here. Click a tile to remove.";
            Font f = StdFont().Italic().Height(DPI(10));
            Size ts = GetTextSize(t, f);
            int tx = (sz.cx - ts.cx) / 2;
            int ty = (sz.cy - ts.cy) / 2;
            w.DrawText(tx, ty, t, f, SColorDisabled());
        }
    }

private:
    Vector<SymbolItem>       items;
    Array<TitleDragButton>   tiles;

    Color  baseContentFill, baseContentStroke;
    bool   highlightOnDrop = true;

    Size   tileSize = Size(DPI(60), DPI(60));

	void RebuildTiles() {
	    // Remove previous tile ctrls from UI
	    for (int i = 0; i < tiles.GetCount(); ++i)
	        tiles[i].Remove();
	    tiles.Clear();
	
	    // Recreate BIN tiles from current items
	    for (int i = 0; i < items.GetCount(); ++i) {
	        TitleDragButton& t = tiles.Create();
	        t.SetSymbol(items[i])
	         .SetMode(TitleDragButton::BIN);
	
	        t.SetMinSize(tileSize);      //silly this is not chainable
	
	        // Optional: subtle bin styling (thin line, rounded)
	        t.SetRadius(DPI(5))
	         .SetLineThickness(1)
	         .EnableDashedFrame(false)
	         .EnableFill(true);
	
	        // Click in BIN removes the tile
	        t.WhenRemove = THISBACK1(RemoveIndex, i);
	
	        AddContent(t);                       // WRAP layout will position it
	    }
	
	    Layout();   // recompute flow + scrollbar
	    Refresh();
	}


    void Reflow() {
        WrapItemSize(tileSize);
        for(int i = 0; i < tiles.GetCount(); ++i)
            tiles[i].SetMinSize(tileSize);
        Layout();
        Refresh();
    }

    void FireListChanged() { if(WhenListChanged) WhenListChanged(); }
};


/* ======================================================================
   SymbolPickerApp — main window
   ====================================================================== */

class SymbolPickerApp : public TopWindow {
public:
    typedef SymbolPickerApp CLASSNAME;

    // Header + controls
    StageCard   headerCard;
    Button         copyBtn, exitBtn;

    // Category bar (now uses TitleDragButton tiles)
    StageCard           categoryCard;
    Array<TitleDragButton> categoryButtons;

    // Splitters
    Splitter       mainSplitter;  // Left (items+bin) | Right (code)
    Splitter       leftSplit;     // Top (items) | Bottom (bin)

    // Left-top items card (wrap layout of draggable tiles)
    StageCard             itemsCard;
    Array<TitleDragButton>   symbolTiles;

    // Left-bottom bin card
    DropBinCard              binCard;

    // Right code pane
    StageCard   codeCard;
    Option         hexMode;       // toggle format
    DocEdit        codeOutput;

    // Data/state
    String         activeCategoryKey;

    // ctor + behavior
    SymbolPickerApp();

    void FilterCategory(const String& key);
    void UpdateSymbolGrid();
    void UpdateCodeOutput();
    void AddSymbolToBin(const SymbolItem& item);
    void CopyToClipboard();
};


void SymbolPickerApp::FilterCategory(const String& key) {
    activeCategoryKey = key;

    for (int i = 0; i < categoryButtons.GetCount(); ++i) {
        bool isActive = (allCategories[i].key == key);
        categoryButtons[i].SetSelected(isActive);
    }

    for (const auto& cat : allCategories)
        if(cat.key == key) { itemsCard.SetTitle(cat.name); break; }

    UpdateSymbolGrid();
}

void SymbolPickerApp::UpdateSymbolGrid() {
    for (int i = 0; i < symbolTiles.GetCount(); ++i)
        symbolTiles[i].Remove();
    symbolTiles.Clear();

    const SymbolCategory* activeCat = nullptr;
    for (const auto& cat : allCategories)
        if (cat.key == activeCategoryKey) { activeCat = &cat; break; }
    if (!activeCat) return;

    const Size tileSz = Size(DPI(60), DPI(60));

    for (int i = 0; i < activeCat->symbols.GetCount(); ++i) {
        const auto& item = activeCat->symbols[i];
        TitleDragButton& tile = symbolTiles.Create();
        tile.SetSymbol(item).SetMode(TitleDragButton::SOURCE);
        tile.SetMinSize(tileSz);
        tile.SetStyle(Button::StyleNormal());

        itemsCard.AddContent(tile);
    }

    itemsCard.Layout();
}


static dword FirstCodepoint(const String& utf8) {
    if(utf8.IsEmpty()) return 0;
    WString ws = ToUnicode(utf8, CHARSET_UTF8);
    return ws.IsEmpty() ? 0 : (dword)ws[0];
}

void SymbolPickerApp::UpdateCodeOutput() {
    String out;
    out << "// Unicode Picker Selections v0.9\n\n";

    const Vector<SymbolItem>& selected = binCard.GetSymbols();

    if (selected.IsEmpty()) {
        out << "// Drag a symbol or a whole category into the Selection Bin...\n";
        codeOutput.Set(out.ToWString());
        return;
    }

    VectorMap<String, Vector<SymbolItem>> grouped;
    for (const auto& item : selected) {
        String fullCategory;
        for (const auto& cat : allCategories)
            if (cat.key == item.categoryKey) { fullCategory = cat.name; break; }
        grouped.GetAdd(fullCategory).Add(item);
    }

    Vector<String> keys;
    for(int i = 0; i < grouped.GetCount(); i++)
        keys.Add(grouped.GetKey(i));
    Sort(keys);

    bool useHex = (bool)hexMode;

    for(const String& catName : keys) {
        out << "// " << catName << "\n";
        for (const auto& item : grouped.Get(catName)) {
            if(useHex) {
                dword cp = FirstCodepoint(item.charCode);
                out << Format("myControl.SetLabel(FromUtf32(0x%X)); // %s\n", (int)cp, ~item.name);
            } else {
                out << Format("myControl.SetLabel(\"%s\"); // %s\n", ~item.charCode, ~item.name);
            }
        }
        out << "\n";
    }
    codeOutput.Set(out.ToWString());
}

void SymbolPickerApp::AddSymbolToBin(const SymbolItem& item) {
    binCard.AddSymbolUnique(item);
    UpdateCodeOutput();
}

void SymbolPickerApp::CopyToClipboard() {
    WriteClipboardText(codeOutput.Get()); // DocEdit::Get returns WString
}

// =====================================================
// App impl
// =====================================================
SymbolPickerApp::SymbolPickerApp()
{
    Title("Unicode Symbol Picker (U++ Badge Generator)");
    Sizeable().Zoomable();
    SetMinSize(Size(DPI(800), DPI(500)));
    SetRect( Size(DPI(1000), DPI(700)));  //window size
    
    // --- Data ---
    InitData();
    activeCategoryKey = allCategories.IsEmpty() ? String() : allCategories[0].key;

    // --- Top header strip ---
    headerCard
        .SetTitle("Unicode Symbol References for U++")
        .SetSubTitle("Filter by category, then drag symbols or an entire category into the Selection Bin to generate U++ code")
        .SetHeaderAlign(StageCard::LEFT)
        .EnableCardFrame(false)                // no outer frame
        .EnableCardFill(false)                 // let window bg show
        .SetCardColors(Color(245,245,245), SColorShadow());
    this->Add(headerCard.HSizePos(DPI(8), DPI(8)).TopPos(DPI(8), DPI(56)));

    // Header actions
    copyBtn.SetLabel("Copy");
    copyBtn.WhenAction = THISBACK(CopyToClipboard);
    Add(copyBtn.RightPos(DPI(120), DPI(110)).TopPos(DPI(16), DPI(28)));

    exitBtn.SetLabel("Exit");
    exitBtn.WhenAction = [=]{ this->Close(); };
    Add(exitBtn.RightPos(DPI(8), DPI(100)).TopPos(DPI(16), DPI(28)));

    // --- Category bar (TitleDragButton tiles, draggable, pastel bg) ---
    categoryCard
        .SetCardColors(Color(245,245,245), SColorShadow())
        .EnableCardFrame(false)
        .EnableCardFill(false)
        .EnableContentClampToPane(true)   // fixed band height
        .EnableContentScroll(true)
        .ContentWrap()
        .SetContentInset(DPI(4), DPI(4), DPI(4), DPI(4))
        .SetContentGap(DPI(6), DPI(6))
        .WrapItemSize(DPI(130), DPI(20)); // uniform category tile size
    Add(categoryCard.HSizePos(DPI(8), DPI(8)).TopPos(DPI(72), DPI(100)));

    // Add category tiles (TitleDragButton), draggable + clickable
    for (int i = 0; i < allCategories.GetCount(); ++i) {
        const auto& cat = allCategories[i];
        TitleDragButton& b = categoryButtons.Create();
        b.SetCategory(cat)                      // pastel face from cat.col (with darker mix in SetCategory)
         .SetMode(TitleDragButton::SOURCE)
         .SetTileSize(Size(DPI(160), DPI(30)))  // changeable via our chainable setter
         .SetRadius(DPI(10))                     // slightly rounded
         .SetLineThickness(0)                   // no line for category chips
         .EnableDashedFrame(false)
         .EnableFill(true)
         .SetHoverTintPct(22)                   // gentle hover
         .SetActiveColors(Color(22, 86, 160),   // dark blue when selected
                          SColorShadow(),
                          SColorHighlightText());
        b.WhenAction = THISBACK1(FilterCategory, cat.key); // click == filter
        categoryCard.AddContent(b);
    }

    // --- Left items card (wrap grid of draggable tiles) ---
    itemsCard
        .SetTitle("Items")
        .SetHeaderAlign(StageCard::LEFT)
        .EnableCardFill(true)
        .EnableCardFrame(true)
        .SetCardCornerRadius(DPI(10))
        .SetCardStrokeThickness(1)
        .SetCardColors(Color(245,245,245), SColorShadow())
        .EnableContentScroll(true)
        .EnableContentClampToPane(true)
        .ContentWrap()
        .SetContentInset(DPI(6), DPI(6), DPI(6), DPI(6))
        .SetContentGap(DPI(6), DPI(6))
        .WrapItemSize(DPI(60), DPI(60));

    // --- Selection bin (drop area) ---
    binCard
        .SetTileSize(DPI(60), DPI(60))
        .SetTileGap(DPI(4))
        .EnableCardFrame(false)
        .EnableCardFill(false);
    binCard.WhenListChanged = THISBACK(UpdateCodeOutput);

    // --- Right code card (rounded dark content with green code) ---
    // Hack: using the StageCard content frame to draw the rounded dark panel,
    // and put DocEdit inside with transparent background and custom inks.
	codeCard
	    .SetTitle("U++ Code Output")
	    .SetHeaderAlign(StageCard::LEFT)
	    .EnableCardFill(false)
	    .EnableCardFrame(false)
	    .SetCardColors(Color(245,245,245), SColorShadow())
	    .EnableContentFill(true)
	    .EnableContentFrame(true)
	    .SetContentCornerRadius(DPI(10))  //rounded
	    .SetContentStrokeThickness(0)
	    .SetContentFrameColors(Color(32,36,42), Color(0,0,0))
	    .SetContentInset(DPI(12), DPI(12), DPI(12), DPI(12))
	    .EnableContentScroll(false)      // card itself won’t scroll
	    .ContentAbsolute()
	    .EnableContentAutoFill(true);    // smart autofill
	
	codeOutput.SetReadOnly();
	codeOutput.Transparent();
	codeOutput.SetFrame(NullFrame());
	codeOutput.SetFont(Monospace(10));
	codeOutput.SetColor(DocEdit::INK_NORMAL,   Color(170,255,170));
	codeOutput.SetColor(DocEdit::PAPER_NORMAL,   Color(Null));
    codeOutput.SetColor(DocEdit::PAPER_READONLY, Color(Null));
    codeOutput.SetColor(DocEdit::PAPER_SELECTED, Blend(Color(32,36,42), SColorHighlight(), 50));

	codeCard.ReplaceContent(codeOutput);

    
    codeOutput.HSizePos(0, 0).VSizePos(0, 0);
    
    // --- Splitters: items | bin  ||  code ---
    leftSplit.Vert(itemsCard, binCard);
    leftSplit.SetPos(7000);

    mainSplitter.Horz(leftSplit, codeCard);
    mainSplitter.SetPos(6000);

    Add(mainSplitter.HSizePos(DPI(8), DPI(8)).VSizePos(DPI(178), DPI(8)));

    // --- Initial content ---
    if(!activeCategoryKey.IsEmpty())
        FilterCategory(activeCategoryKey);
    UpdateCodeOutput();
}


GUI_APP_MAIN
{
    SymbolPickerApp().Run();
}