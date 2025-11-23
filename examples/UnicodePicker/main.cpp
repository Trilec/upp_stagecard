#include <CtrlLib/CtrlLib.h>
#include <Painter/Painter.h>
#include <StageCard/StageCard.h>

using namespace Upp;

// ============================================================================
// THEME (centralized knobs)
// ============================================================================

static inline Color ButtonFaceRed() { return Color(235, 66, 33); } 
static inline Color ButtonFaceGreen() { return Color(66, 235, 33); } 
static inline Color ButtonFaceBlue() { return Color(37, 99, 235); } 
static inline Color ButtonFaceGray() { return Color(70, 70, 70); } //

struct AppTheme {
    String name;
    Color  window_bg;

    // StageCard content paper + code paper
    Color  card_content_bg;
    Color  code_content_bg;

    // Buttons / chips
    Color  chip_face_a;
    Color  chip_face_b;
    Color  chip_border;
    Color  chip_ink;

    // Symbol tiles
    Color  tile_face;
    Color  tile_border;
    Color  tile_ink;

    // Bin dashed frame color
    Color  bin_stroke;

    // StageCard::Style factory for header/ink/underline + stateless borders
    StageCard::Style (*make_style)();
};

static StageCard::Style MakeLightStyle()
{
    StageCard::Style s = StageCard::StyleDefault();
    auto set4 = [](Color* a, Color n, Color h, Color p, Color d){ a[0]=n; a[1]=h; a[2]=p; a[3]=d; };

    const Color faceN = Color(250,250,252);
    set4(s.palette.headerFace,  faceN, Blend(faceN, White(), 8),  Blend(faceN, Black(), 10),
         Blend(SColorFace(), SColorPaper(), 60));

    const Color bordN = SColorShadow();
    set4(s.palette.headerBorder,bordN, Blend(bordN, SColorHighlight(), 20),
         Blend(bordN, Black(), 15), Blend(bordN, SColorDisabled(), 35));

    const Color t = SColorText();
    set4(s.palette.titleInk,    t, t, t, SColorDisabled());
    set4(s.palette.subTitleInk, t, t, t, SColorDisabled());
    set4(s.palette.badgeInk,    t, t, t, SColorDisabled());

    s.palette.contentBg  = SColorPaper();
    s.palette.contentInk = SColorText();
    s.palette.cardBorder = SColorShadow();
    s.palette.cardFill   = Color(240,240,240);
    s.palette.underline  = GrayColor(180);

    s.metrics.titleUnderlineTh  = 1;
    s.metrics.badgeAlignDefault = StageCard::RIGHT;
    return s;
}

static StageCard::Style MakeMidnightStyle()
{
    StageCard::Style s = StageCard::StyleDefault();
    auto set4 = [](Color* a, Color n, Color h, Color p, Color d){ a[0]=n; a[1]=h; a[2]=p; a[3]=d; };

    const Color faceN = Color(28,32,38);
    set4(s.palette.headerFace,  faceN, Blend(faceN, SColorHighlight(), 18),
         Blend(faceN, Black(), 12), Blend(SColorFace(), SColorPaper(), 70));

    const Color bordN = Color(65,72,84);
    set4(s.palette.headerBorder,bordN, Blend(bordN, SColorHighlight(), 25),
         Blend(bordN, Black(), 18), Blend(bordN, SColorDisabled(), 40));

    const Color tN   = ButtonFaceBlue();
    const Color tSub = Color(37, 99, 235);
    set4(s.palette.titleInk,    tN,   tN,   tN,   SColorDisabled());
    set4(s.palette.subTitleInk, tSub, tSub, tSub, SColorDisabled());
    set4(s.palette.badgeInk,    Color(200,220,255), Color(200,220,255),
         Color(200,220,255), SColorDisabled());

    s.palette.contentBg  = Color(28,30,34);
    s.palette.contentInk = SColorText();
    s.palette.cardBorder = Color(65,72,84);
    s.palette.cardFill   = Color(40,44,50);
    s.palette.underline  = Color(60,68,80);

    s.metrics.titleUnderlineTh  = 2;
    s.metrics.badgeAlignDefault = StageCard::LEFT;
    return s;
}

// theme array
static const AppTheme THEMES[] = {
    {
        "Light",
        SColorPaper(),                 // window_bg
        SColorPaper(),                 // card_content_bg
        Color(28,30,34),               // code_content_bg
        Blend(Color(235,239,248), SColorFace(), 20), // chip_face_a
        Blend(ButtonFaceBlue(), ButtonFaceBlue(), 40), // chip_face_b
        SColorShadow(),                // chip_border
        SColorText(),                  // chip_ink
        Color(245,245,245),            // tile_face
        GrayColor(120),                // tile_border
        Black(),                       // tile_ink
        SColorShadow(),                // bin_stroke
        &MakeLightStyle
    },
    {
        "Midnight",
        Color(18,20,23),               // window_bg
        Color(28,30,34),               // card_content_bg
        Color(28,30,34),               // code_content_bg
        Color(44,44,44),               // chip_face_a
        Color(70,70,70),               // chip_face_b
        Color(65,72,84),               // chip_border
        Color(200,220,255),            // chip_ink
        Color(54,54,54),               // tile_face
        Color(70,70,70),               // tile_border
        White(),                       // tile_ink
        Color(90,140,210),             // bin_stroke (visible dashed)
        &MakeMidnightStyle
    }
};

// percentage blend helper (0..100)
static inline Color BlendPct(Color a, Color b, int pct) {
    pct = clamp(pct, 0, 100);
    return Blend(a, b, pct);
}

// ============================================================================
// DATA (categories & symbols)
// ============================================================================
struct SymbolItem : Moveable<SymbolItem> {
    String charCode, name, categoryKey;
    SymbolItem() {}
    SymbolItem(const String& code, const String& n, const String& catKey)
        : charCode(code), name(n), categoryKey(catKey) {}
};

struct SymbolCategory : Moveable<SymbolCategory> {
    String name, key;
    Vector<SymbolItem> symbols;
};

Vector<SymbolCategory> allCategories;

static void InitData()
{
    allCategories.Clear();

    // Arrows
    {
        SymbolCategory c; c.name="Arrows"; c.key="arrows"; 
        c.symbols
            << SymbolItem{"→","Right Arrow","arrows"} << SymbolItem{"←","Left Arrow","arrows"}
            << SymbolItem{"↑","Up Arrow","arrows"}    << SymbolItem{"↓","Down Arrow","arrows"}
            << SymbolItem{"↔","Left-Right Arrow","arrows"} << SymbolItem{"↕","Up-Down Arrow","arrows"}
            << SymbolItem{"►","Right Triangle","arrows"}   << SymbolItem{"▼","Down Triangle","arrows"}
            << SymbolItem{"«","Double Left","arrows"}      << SymbolItem{"»","Double Right","arrows"}
            << SymbolItem{"⤓","Down Arrow with Bar","arrows"} << SymbolItem{"⤒","Up Arrow with Bar","arrows"}
            << SymbolItem{"⟵","Long Left Arrow","arrows"}  << SymbolItem{"⟶","Long Right Arrow","arrows"}
            << SymbolItem{"⟷","Long Left-Right Arrow","arrows"} << SymbolItem{"⟹","Long Right Double Arrow","arrows"}
            << SymbolItem{"⟺","Long Left-Right Double Arrow","arrows"}
            << SymbolItem{"⇤","Leftward Tab","arrows"} << SymbolItem{"⇥","Rightward Tab","arrows"}
            << SymbolItem{"↩","Return/Hook Left","arrows"} << SymbolItem{"↪","Hook Right","arrows"}
            << SymbolItem{"⤴","Arrow Right-Up","arrows"}   << SymbolItem{"⤵","Arrow Right-Down","arrows"}
            << SymbolItem{"↘","South-East Arrow","arrows"} << SymbolItem{"↙","South-West Arrow","arrows"}
            << SymbolItem{"↖","North-West Arrow","arrows"} << SymbolItem{"↗","North-East Arrow","arrows"}
            << SymbolItem{"⇀","Right Harpoon Above","arrows"} << SymbolItem{"↼","Left Harpoon Above","arrows"}
            << SymbolItem{"⇌","Equilibrium Arrows","arrows"};
        allCategories.Add(pick(c));
    }
    // Toggle & State
    {
        SymbolCategory c; c.name="Toggle & State"; c.key="toggle"; 
        c.symbols << SymbolItem{"+","Plus (Expand)","toggle"} << SymbolItem{"−","Minus (Collapse)","toggle"}
                  << SymbolItem{"✓","Checkmark","toggle"}     << SymbolItem{"✗","Ballot X","toggle"}
                  << SymbolItem{"□","Unchecked Box","toggle"} << SymbolItem{"☑","Checked Box","toggle"}
                  << SymbolItem{"•","Bullet Point","toggle"}  << SymbolItem{"⊛","Circled Star","toggle"};
        allCategories.Add(pick(c));
    }
    // Status & Alert
    {
        SymbolCategory c; c.name="Status & Alert"; c.key="status"; 
        c.symbols << SymbolItem{"ℹ️","Info Icon","status"} << SymbolItem{"⚠️","Warning Sign","status"}
                  << SymbolItem{"❌","Error/Close","status"} << SymbolItem{"🔒","Locked/Secure","status"}
                  << SymbolItem{"🔔","Bell/Notification","status"} << SymbolItem{"⚡","High Priority","status"}
                  << SymbolItem{"💡","Idea/Tip","status"} << SymbolItem{"🔥","Hot/Popular","status"};
        allCategories.Add(pick(c));
    }
    // Structural & Layout
    {
        SymbolCategory c; c.name="Structural & Layout"; c.key="layout"; 
        c.symbols << SymbolItem{"▥","Square Blocks 1 (Panel)","layout"} << SymbolItem{"▤","Square Blocks 2","layout"}
                  << SymbolItem{"▦","Square Blocks 3 (Grid)","layout"}  << SymbolItem{"≡","Hamburger Menu","layout"}
                  << SymbolItem{"⋮","Vertical Ellipsis","layout"}       << SymbolItem{"⋯","Horizontal Ellipsis","layout"}
                  << SymbolItem{"⌗","Hash/Number","layout"}             << SymbolItem{"…","More Dots","layout"};
        allCategories.Add(pick(c));
    }
    // Files & Document
    {
        SymbolCategory c; c.name="Files & Document"; c.key="files";
        c.symbols << SymbolItem{"📁","Folder","files"} << SymbolItem{"📄","File/Document","files"}
                  << SymbolItem{"💾","Save Disk","files"} << SymbolItem{"🖼️","Image/Media","files"}
                  << SymbolItem{"🔗","Link/External","files"} << SymbolItem{"🔍","Search/Zoom","files"}
                  << SymbolItem{"✂️","Scissors (Cut)","files"} << SymbolItem{"📥","Inbox/Receive","files"};
        allCategories.Add(pick(c));
    }
    // Time & Date
    {
        SymbolCategory c; c.name="Time & Date"; c.key="time";
        c.symbols << SymbolItem{"📅","Calendar/Date","time"} << SymbolItem{"⏱️","Stopwatch/Time","time"}
                  << SymbolItem{"⌚","Watch/Time","time"}    << SymbolItem{"⏳","Hourglass","time"}
                  << SymbolItem{"↩️","Undo/Back","time"}    << SymbolItem{"↪️","Redo/Forward","time"}
                  << SymbolItem{"🔅","Brightness","time"}   << SymbolItem{"🌙","Night Mode","time"};
        allCategories.Add(pick(c));
    }
    // Math & Logic
    {
        SymbolCategory c; c.name="Math & Logic"; c.key="math"; 
        c.symbols << SymbolItem{"=","Equals","math"} << SymbolItem{"≠","Not Equal","math"}
                  << SymbolItem{"≈","Almost Equal","math"} << SymbolItem{"≤","Less or Equal","math"}
                  << SymbolItem{"≥","Greater or Equal","math"} << SymbolItem{"∑","Summation","math"}
                  << SymbolItem{"∞","Infinity","math"} << SymbolItem{"÷","Division Sign","math"};
        allCategories.Add(pick(c));
    }
    // People & Contact
    {
        SymbolCategory c; c.name="People & Contact"; c.key="people";
        c.symbols << SymbolItem{"👤","User (Single)","people"} << SymbolItem{"👥","Users (Group)","people"}
                  << SymbolItem{"🏠","Home/Main","people"}     << SymbolItem{"📍","Location Pin","people"}
                  << SymbolItem{"🗺️","Map/Guide","people"}     << SymbolItem{"✍️","Writing/Edit","people"}
                  << SymbolItem{"📞","Phone/Call","people"}     << SymbolItem{"📧","Email","people"};
        allCategories.Add(pick(c));
    }
    // Media & Controls
    {
        SymbolCategory c; c.name="Media & Controls"; c.key="media"; 
        c.symbols << SymbolItem{"▶️","Play/Start","media"} << SymbolItem{"⏸️","Pause","media"}
                  << SymbolItem{"⏹️","Stop","media"}       << SymbolItem{"🔇","Mute","media"}
                  << SymbolItem{"🔊","Volume Max","media"} << SymbolItem{"🎧","Headphones","media"}
                  << SymbolItem{"⬇️","Download","media"}   << SymbolItem{"⬆️","Upload","media"};
        allCategories.Add(pick(c));
    }
    // Tools & Settings
    {
        SymbolCategory c; c.name="Tools & Settings"; c.key="tools"; 
        c.symbols << SymbolItem{"⚙️","Gear/Settings","tools"} << SymbolItem{"🛠️","Hammer/Tools","tools"}
                  << SymbolItem{"🔑","Key/Access","tools"}    << SymbolItem{"✏️","Pencil/Edit","tools"}
                  << SymbolItem{"🗑️","Trash/Delete","tools"}  << SymbolItem{"🖨️","Printer","tools"}
                  << SymbolItem{"🧾","Receipt/Billing","tools"} << SymbolItem{"📈","Chart/Analytics","tools"};
        allCategories.Add(pick(c));
    }
    // Communication & Text
    {
        SymbolCategory c; c.name="Communication & Text"; c.key="comm"; 
        c.symbols << SymbolItem{"💬","Chat Bubble","comm"} << SymbolItem{"📣","Megaphone/Announce","comm"}
                  << SymbolItem{"🗣️","Speaking Head","comm"} << SymbolItem{"📢","Loudspeaker","comm"}
                  << SymbolItem{"🎙️","Microphone","comm"}    << SymbolItem{"🔤","Input Latin Letters","comm"}
                  << SymbolItem{"…","Ellipsis/More","comm"}   << SymbolItem{"¶","Pilcrow (Paragraph)","comm"};
        allCategories.Add(pick(c));
    }
    // Shapes & Geometry
    {
        SymbolCategory c; c.name="Shapes & Geometry"; c.key="shapes";
        c.symbols << SymbolItem{"■","Black Square","shapes"} << SymbolItem{"□","White Square","shapes"}
                  << SymbolItem{"●","Black Circle","shapes"} << SymbolItem{"○","White Circle","shapes"}
                  << SymbolItem{"◆","Black Diamond","shapes"}<< SymbolItem{"◇","White Diamond","shapes"}
                  << SymbolItem{"△","White Triangle Up","shapes"} << SymbolItem{"▽","White Triangle Down","shapes"};
        allCategories.Add(pick(c));
    }
    // Currency & Finance
    {
        SymbolCategory c; c.name="Currency & Finance"; c.key="currency";
        c.symbols << SymbolItem{"$","Dollar Sign","currency"} << SymbolItem{"€","Euro Sign","currency"}
                  << SymbolItem{"£","Pound Sign","currency"}  << SymbolItem{"¥","Yen Sign","currency"}
                  << SymbolItem{"💰","Money Bag","currency"}  << SymbolItem{"🪙","Coin","currency"}
                  << SymbolItem{"💳","Credit Card","currency"}<< SymbolItem{"🏦","Bank","currency"};
        allCategories.Add(pick(c));
    }
    // Accessibility & Gestures
    {
        SymbolCategory c; c.name="Accessibility & Gestures"; c.key="access";
        c.symbols << SymbolItem{"♿","Wheelchair Symbol","access"} << SymbolItem{"👁️","Eye/Visibility","access"}
                  << SymbolItem{"👂","Ear/Hearing","access"}      << SymbolItem{"☝️","Index Finger Up","access"}
                  << SymbolItem{"👌","OK Hand","access"}          << SymbolItem{"🤚","Raised Hand","access"}
                  << SymbolItem{"🗣️","Speaking Head","access"}    << SymbolItem{"🫲","Handshake","access"};
        allCategories.Add(pick(c));
    }
    // Keyboard & Input
    {
        SymbolCategory c; c.name="Keyboard & Input"; c.key="keyboard"; 
        c.symbols << SymbolItem{"⌃","Control Key","keyboard"} << SymbolItem{"⌥","Option Key","keyboard"}
                  << SymbolItem{"⌘","Command Key","keyboard"} << SymbolItem{"↩","Return Key","keyboard"}
                  << SymbolItem{"⇧","Shift Key","keyboard"}   << SymbolItem{"⇥","Tab Key","keyboard"}
                  << SymbolItem{"⌦","Delete Forward","keyboard"} << SymbolItem{"⌫","Delete Backward","keyboard"};
        allCategories.Add(pick(c));
    }
    // Weather & Environment
    {
        SymbolCategory c; c.name="Weather & Environment"; c.key="weather";
        c.symbols << SymbolItem{"☀️","Sun","weather"} << SymbolItem{"☁️","Cloud","weather"}
                  << SymbolItem{"☔","Rain/Umbrella","weather"} << SymbolItem{"❄️","Snowflake","weather"}
                  << SymbolItem{"🌡️","Thermometer","weather"}  << SymbolItem{"🌀","Cyclone","weather"}
                  << SymbolItem{"🌍","Globe","weather"}         << SymbolItem{"🌿","Plant/Nature","weather"};
        allCategories.Add(pick(c));
    }
    // Rating & Feedback
    {
        SymbolCategory c; c.name="Rating & Feedback"; c.key="rating"; 
        c.symbols << SymbolItem{"⭐","Star","rating"} << SymbolItem{"♡","Heart (Outline)","rating"}
                  << SymbolItem{"♥","Heart (Filled)","rating"} << SymbolItem{"👍","Thumbs Up","rating"}
                  << SymbolItem{"👎","Thumbs Down","rating"}    << SymbolItem{"💯","Hundred Points","rating"}
                  << SymbolItem{"🎯","Target/Goal","rating"}    << SymbolItem{"🏅","Medal/Award","rating"};
        allCategories.Add(pick(c));
    }
    // Scientific & Technical
    {
        SymbolCategory c; c.name="Scientific & Technical"; c.key="science"; 
        c.symbols << SymbolItem{"°","Degree Symbol","science"} << SymbolItem{"µ","Micro Sign","science"}
                  << SymbolItem{"Ω","Ohm","science"}           << SymbolItem{"∆","Delta/Change","science"}
                  << SymbolItem{"λ","Lambda","science"}         << SymbolItem{"∅","Empty Set/Diameter","science"}
                  << SymbolItem{"🧪","Test Tube","science"}     << SymbolItem{"🧬","DNA","science"};
        allCategories.Add(pick(c));
    }
    // Punctuation & Typography
    {
        SymbolCategory c; c.name="Punctuation & Typography"; c.key="punc"; 
        c.symbols << SymbolItem{"«","Left Guillemet","punc"} << SymbolItem{"»","Right Guillemet","punc"}
                  << SymbolItem{"—","Em Dash","punc"}         << SymbolItem{"–","En Dash","punc"}
                  << SymbolItem{"§","Section Sign","punc"}    << SymbolItem{"†","Dagger","punc"}
                  << SymbolItem{"©","Copyright","punc"}       << SymbolItem{"®","Registered","punc"};
        allCategories.Add(pick(c));
    }
    // Box Drawing & Borders
    {
        SymbolCategory c; c.name="Box Drawing & Borders"; c.key="boxdraw"; 
        c.symbols << SymbolItem{"─","Light Horizontal","boxdraw"} << SymbolItem{"│","Light Vertical","boxdraw"}
                  << SymbolItem{"┌","Light Down & Right","boxdraw"} << SymbolItem{"┐","Light Down & Left","boxdraw"}
                  << SymbolItem{"└","Light Up & Right","boxdraw"}   << SymbolItem{"┘","Light Up & Left","boxdraw"}
                  << SymbolItem{"├","Light Vertical & Right","boxdraw"} << SymbolItem{"┤","Light Vertical & Left","boxdraw"}
                  << SymbolItem{"┬","Light Down & Horizontal","boxdraw"} << SymbolItem{"┴","Light Up & Horizontal","boxdraw"}
                  << SymbolItem{"┼","Light Vertical & Horizontal","boxdraw"}
                  << SymbolItem{"═","Double Horizontal","boxdraw"}  << SymbolItem{"║","Double Vertical","boxdraw"}
                  << SymbolItem{"╔","Double Down & Right","boxdraw"}<< SymbolItem{"╗","Double Down & Left","boxdraw"}
                  << SymbolItem{"╚","Double Up & Right","boxdraw"}  << SymbolItem{"╝","Double Up & Left","boxdraw"}
                  << SymbolItem{"╠","Double Vertical & Right","boxdraw"} << SymbolItem{"╣","Double Vertical & Left","boxdraw"}
                  << SymbolItem{"╦","Double Down & Horizontal","boxdraw"} << SymbolItem{"╩","Double Up & Horizontal","boxdraw"}
                  << SymbolItem{"╬","Double Vertical & Horizontal","boxdraw"};
        allCategories.Add(pick(c));
    }
    // Block Elements & Shading
    {
        SymbolCategory c; c.name="Block Elements & Shading"; c.key="blocks";
        c.symbols << SymbolItem{"░","Light Shade","blocks"} << SymbolItem{"▒","Medium Shade","blocks"}
                  << SymbolItem{"▓","Dark Shade","blocks"}  << SymbolItem{"█","Full Block","blocks"}
                  << SymbolItem{"▁","Lower One Eighth Block","blocks"} << SymbolItem{"▂","Lower One Quarter Block","blocks"}
                  << SymbolItem{"▃","Lower Three Eighths","blocks"}    << SymbolItem{"▄","Lower Half Block","blocks"}
                  << SymbolItem{"▅","Lower Five Eighths","blocks"}     << SymbolItem{"▆","Lower Three Quarters","blocks"}
                  << SymbolItem{"▇","Lower Seven Eighths","blocks"}    << SymbolItem{"▉","Left Seven Eighths Block","blocks"}
                  << SymbolItem{"▊","Left Three Quarters","blocks"}    << SymbolItem{"▋","Left Five Eighths","blocks"}
                  << SymbolItem{"▌","Left Half Block","blocks"}        << SymbolItem{"▍","Left Three Eighths","blocks"};
        allCategories.Add(pick(c));
    }
    // Enclosed Alphanumerics
    {
        SymbolCategory c; c.name="Enclosed Alphanumerics"; c.key="enclosed";
        c.symbols << SymbolItem{"①","Circled Digit One","enclosed"} << SymbolItem{"②","Circled Digit Two","enclosed"}
                  << SymbolItem{"③","Circled Digit Three","enclosed"} << SymbolItem{"④","Circled Digit Four","enclosed"}
                  << SymbolItem{"⑤","Circled Digit Five","enclosed"}  << SymbolItem{"❶","Dingbat One","enclosed"}
                  << SymbolItem{"❷","Dingbat Two","enclosed"}         << SymbolItem{"❸","Dingbat Three","enclosed"}
                  << SymbolItem{"Ⓐ","Circled Capital A","enclosed"}   << SymbolItem{"Ⓑ","Circled Capital B","enclosed"}
                  << SymbolItem{"Ⓒ","Circled Capital C","enclosed"}   << SymbolItem{"ⓐ","Circled Small a","enclosed"}
                  << SymbolItem{"ⓑ","Circled Small b","enclosed"}     << SymbolItem{"ⓒ","Circled Small c","enclosed"}
                  << SymbolItem{"ⓩ","Circled Small z","enclosed"}     << SymbolItem{"Ⓩ","Circled Capital Z","enclosed"};
        allCategories.Add(pick(c));
    }
    // Superscripts & Subscripts
    {
        SymbolCategory c; c.name="Superscripts & Subscripts"; c.key="super";
        c.symbols << SymbolItem{"¹","Superscript 1","super"} << SymbolItem{"²","Superscript 2","super"}
                  << SymbolItem{"³","Superscript 3","super"} << SymbolItem{"⁴","Superscript 4","super"}
                  << SymbolItem{"⁵","Superscript 5","super"} << SymbolItem{"⁶","Superscript 6","super"}
                  << SymbolItem{"⁷","Superscript 7","super"} << SymbolItem{"⁸","Superscript 8","super"}
                  << SymbolItem{"⁹","Superscript 9","super"} << SymbolItem{"⁰","Superscript 0","super"}
                  << SymbolItem{"₀","Subscript 0","super"}   << SymbolItem{"₁","Subscript 1","super"}
                  << SymbolItem{"₂","Subscript 2","super"}   << SymbolItem{"₃","Subscript 3","super"}
                  << SymbolItem{"₄","Subscript 4","super"}   << SymbolItem{"₅","Subscript 5","super"}
                  << SymbolItem{"₆","Subscript 6","super"}   << SymbolItem{"₇","Subscript 7","super"}
                  << SymbolItem{"₈","Subscript 8","super"}   << SymbolItem{"₉","Subscript 9","super"};
        allCategories.Add(pick(c));
    }
}

// ============================================================================
// DragBadgeButton (compact, solid fill; per-state)
// ============================================================================
class DragBadgeButton : public Button {
public:
    typedef DragBadgeButton CLASSNAME;
    enum Mode   { DRAGABLE, DROPPED, NORMAL };
    enum Layout { ICON_CENTER_TEXT_BOTTOM, ICON_CENTER_TEXT_TOP, TEXT_CENTER, ICON_ONLY_CENTER };
    enum { ST_NORMAL=0, ST_HOT=1, ST_PRESSED=2, ST_DISABLED=3, ST_COUNT=4 };

    struct Palette { Color face[ST_COUNT], border[ST_COUNT], ink[ST_COUNT]; };
    struct Payload { String flavor, id, group, name, text, badge; };

    DragBadgeButton() {
        Transparent();
        SetFrame(NullFrame());
        SetMinSize(Size(DPI(60), DPI(24)));
        SetFont( StdFont().Height(DPI(10)) );
        SetBadgeFont ( StdFont().Height(DPI(17)) );
        SetRadius(DPI(8)).SetStroke(1).EnableDashed(false).EnableFill(true);
        SetHoverTintPercent(40);
        SetSelected(false);
        SetSelectionColors( Color(37, 99, 235), SColorHighlightText(), White());
        SetLayout(TEXT_CENTER);
        mode = NORMAL;
        SetBaseColors(Blend(SColorPaper(), SColorFace(), 20), SColorShadow(), SColorText());
    }

    // payload & content
    DragBadgeButton& SetPayload(const Payload& p) { payload = p; Refresh(); return *this; }
    const Payload&   GetPayload() const           { return payload; }
    DragBadgeButton& SetText(const String& t)     { label = t; Refresh(); return *this; }
    DragBadgeButton& SetBadgeText(const String& b){ badgeText = b; Refresh(); return *this; }
    DragBadgeButton& ClearBadge()                 { badgeText.Clear(); Refresh(); return *this; }

    // layout / fonts
    DragBadgeButton& SetLayout(Layout l)          { layout = l; Refresh(); return *this; }
    DragBadgeButton& SetFont(Font f)              { textFont = f; Refresh(); return *this; }
    DragBadgeButton& SetBadgeFont(Font f)         { badgeFont = f; Refresh(); return *this; }
    DragBadgeButton& SetHoverColor(Color c)       { hoverColor = c; return *this; }
    
    // modes / selection
    DragBadgeButton& SetMode(Mode m)              { mode = m; Refresh(); return *this; }
    DragBadgeButton& SetSelected(bool on=true)    { selected = on; Refresh(); return *this; }
    bool             IsSelected() const           { return selected; }
    DragBadgeButton& SetSelectionColors(Color faceC, Color borderC, Color inkC = SColorHighlightText())
        { selFace=faceC; selBorder=borderC; selInk=inkC; return *this; }

    // palette
    DragBadgeButton& SetPalette(const Palette& p) { pal = p; Refresh(); return *this; }
    Palette          GetPalette() const           { return pal; }
    DragBadgeButton& SetBaseColors(Color face, Color border, Color ink, int hot_pct=12, int press_pct=14) {
        pal.face[ST_NORMAL]   = face;
        pal.face[ST_HOT]      = Blend(face, White(), hot_pct);
        pal.face[ST_PRESSED]  = Blend(face, Black(), press_pct);
        pal.face[ST_DISABLED] = Blend(SColorFace(), SColorPaper(), 60);

        pal.border[ST_NORMAL]   = border;
        pal.border[ST_HOT]      = Blend(border, SColorHighlight(), 20);
        pal.border[ST_PRESSED]  = Blend(border, Black(), 15);
        pal.border[ST_DISABLED] = Blend(border, SColorDisabled(), 35);

        pal.ink[ST_NORMAL]   = ink;
        pal.ink[ST_HOT]      = ink;
        pal.ink[ST_PRESSED]  = ink;
        pal.ink[ST_DISABLED] = SColorDisabled();
        Refresh();
        return *this;
    }

    // geometry
    DragBadgeButton& SetRadius(int px){ radius = max(0,px); Refresh(); return *this; }
    DragBadgeButton& SetStroke(int th){ stroke = max(0,th); Refresh(); return *this; }
    DragBadgeButton& EnableDashed(bool on=true){ dashed=on; Refresh(); return *this; }
    DragBadgeButton& SetDashPattern(const String& d){ dash=d; Refresh(); return *this; }
    DragBadgeButton& EnableFill(bool on=true){ fill=on; Refresh(); return *this; }
    DragBadgeButton& SetHoverTintPercent(int pct){ hoverPct=clamp(pct,0,100); return *this; }
    DragBadgeButton& SetTileSize(Size s)       { SetMinSize(s); return *this; }
    DragBadgeButton& SetTileSize(int w,int h)  { return SetTileSize(Size(w,h)); }

    // signal used by bin tiles
    Callback WhenRemove;

    // DnD
    void LeftDrag(Point, dword) override {
        if(mode != DRAGABLE) return;
        if(IsEmpty(payload.flavor)) return;
        Image sample = MakeDragSample();
        DoDragAndDrop(InternalClip(*this, payload.flavor), sample, DND_COPY);
    }
    void LeftDown(Point p, dword k) override {
        if(mode == DROPPED && WhenRemove) WhenRemove();
        Button::LeftDown(p, k);
    }

    // paint
    void Paint(Draw& w) override {
        const int st = StateIndex();
        Color faceC   = pal.face[st];
        Color borderC = pal.border[st];
        Color inkC    = pal.ink[st];

		if(HasMouse() && st==ST_HOT && !selected) {
		    Color target = IsNull(hoverColor) ? White() : hoverColor; // default to simple lighten
		    faceC = Blend(faceC, target, hoverPct);
		}

        if(selected) {
            if(!IsNull(selFace))   faceC = selFace;
            if(!IsNull(selBorder)) borderC = selBorder;
            if(!IsNull(selInk))    inkC = selInk;
        }

        Size sz = GetSize();
        ImageBuffer ib(sz);
        Fill(~ib, RGBAZero(), ib.GetLength());
        {
            BufferPainter p(ib, MODE_ANTIALIASED);
            const double inset = 0.5;
            double x=inset, y=inset, wdt=sz.cx-2*inset, hgt=sz.cy-2*inset;

            p.Begin();
            if(radius>0) p.RoundedRectangle(x,y,wdt,hgt,radius);
            else         p.Rectangle(x,y,wdt,hgt);
            if(fill) {
                p.Fill(faceC);
            }
            if(stroke>0) {
                if(dashed) p.Dash(dash, 0.0);
                p.Stroke(stroke, borderC);
            }
            p.End();
        }
        w.DrawImage(0,0,ib);

        // content
        Rect r = Rect(sz).Deflated(DPI(6), DPI(4));
        const int pad = DPI(4);

        if(layout == ICON_CENTER_TEXT_BOTTOM) {
            bool hasBadge = !badgeText.IsEmpty();
            if(!hasBadge) {
                Size ts = GetTextSize(label, textFont);
                w.DrawText(r.left + (r.GetWidth()-ts.cx)/2, r.top+(r.GetHeight()-ts.cy)/2, label, textFont, inkC);
            } else {
                Size gsz = GetTextSize(badgeText, badgeFont);
                Size ts  = GetTextSize(label,     textFont);
                int ty   = r.bottom - ts.cy;
                int gx   = r.left + (r.GetWidth() - gsz.cx)/2;
                int gy   = r.top + (r.GetHeight() - (gsz.cy + pad + ts.cy))/2;
                w.DrawText(gx, gy, badgeText, badgeFont, inkC);
                w.DrawText(r.left + (r.GetWidth()-ts.cx)/2, ty, label, textFont, inkC);
            }
        }
        else if(layout == ICON_CENTER_TEXT_TOP) {
            bool hasBadge = !badgeText.IsEmpty();
            Size ts = GetTextSize(label, textFont);
            int tx = r.left + (r.GetWidth()-ts.cx)/2;
            int ty = r.top;
            w.DrawText(tx, ty, label, textFont, inkC);
            if(hasBadge) {
                Size gsz = GetTextSize(badgeText, badgeFont);
                int gx   = r.left + (r.GetWidth()-gsz.cx)/2;
                int gy   = r.top + (r.GetHeight() - gsz.cy) / 2;
                w.DrawText(gx, gy, badgeText, badgeFont, inkC);
            }
        }
        else if(layout == TEXT_CENTER) {
            Size ts = GetTextSize(label, textFont);
            w.DrawText(r.left + (r.GetWidth()-ts.cx)/2, r.top+(r.GetHeight()-ts.cy)/2, label, textFont, inkC);
        }
        else { // ICON_ONLY_CENTER
            Size gsz = GetTextSize(badgeText, badgeFont);
            w.DrawText(r.left + (r.GetWidth()-gsz.cx)/2, r.top+(r.GetHeight()-gsz.cy)/2, badgeText, badgeFont, inkC);
        }
    }

private:
    int StateIndex() const {
        if(!IsShowEnabled())         return ST_DISABLED;
        if(IsPush() || HasCapture()) return ST_PRESSED;
        return HasMouse() ? ST_HOT : ST_NORMAL;
    }

    Image MakeDragSample() const {
        Size sz = GetSize();
        if(sz.cx<=0 || sz.cy<=0) return Image();
        ImageBuffer ib(sz);
        Fill(~ib, RGBAZero(), ib.GetLength());
        { BufferPainter p(ib); const_cast<DragBadgeButton*>(this)->Paint(p); }
        return Image(ib);
    }

    // payload/content/look
    Payload    payload;
    String     label, badgeText;
    Layout     layout = TEXT_CENTER;
    Palette    pal;
    Font       textFont, badgeFont;
    int        radius = DPI(8), stroke = 1;
    bool       dashed = false, fill = true;
    String     dash   = "5,5";
    int        hoverPct = 22;
    bool       selected = false;
    Color      selFace=Null, selBorder=Null, selInk=Null;
    Mode       mode = NORMAL;
    Color      hoverColor = Null;
};

// ============================================================================
// Styled DropList (rounded background; self-painted text & arrow)
// ============================================================================
struct DropListStyled : DropList {
    // look
    int   radius = DPI(8);
    Color face_normal   = Color(36, 99, 235);
    Color face_hot      = Blend( Color(36, 99, 235), White(), 15);
    Color face_pressed  = Blend( Color(37, 99, 235), Black(), 100);
    Color face_disabled = Blend(SColorFace(), SColorPaper(), 60);
    Color ink_normal    = White();
    Color ink_disabled  = GrayColor(180);
    Color border_color  = SColorShadow();

    // geometry / stroke (button-like)
    int    stroke  = 0;
    bool   dashed  = false;
    String dash    = "5,5";
    bool   fill    = true;

    // key -> label map so we can show human text while storing an int key
    VectorMap<Value, String> key_to_label;

    DropListStyled() {
        Transparent();
        SetFrame(NullFrame());
        SetDropLines(16);
        // Do NOT "using DropList::Add" (we want to hide the base overload)
    }

    // ---- API parity with our buttons ----
    DropListStyled& SetBgColor(Color base, int hot_pct=15, int press_pct=20) {
        face_normal   = base;
        face_hot      = Blend(base, White(), hot_pct);
        face_pressed  = Blend(base, Black(), press_pct);
        face_disabled = Blend(SColorFace(), SColorPaper(), 60);
        Refresh();
        return *this;
    }
    DropListStyled& SetTextColor(Color ink, Color dis = GrayColor(180)) {
        ink_normal = ink; ink_disabled = dis; Refresh(); return *this;
    }
    DropListStyled& SetBorderColor(Color c) { border_color = c; Refresh(); return *this; }
    DropListStyled& SetRadius(int px)       { radius = max(0,px); Refresh(); return *this; }
    DropListStyled& SetStroke(int th)       { stroke = max(0, th); Refresh(); return *this; }
    DropListStyled& EnableDashed(bool on=true) { dashed = on; Refresh(); return *this; }
    DropListStyled& SetDashPattern(const String& d) { dash = d; Refresh(); return *this; }
    DropListStyled& EnableFill(bool on=true) { fill = on; Refresh(); return *this; }
    DropListStyled& SetTileSize(Size s)      { SetMinSize(s); return *this; }
    DropListStyled& SetTileSize(int w,int h) { return SetTileSize(Size(w,h)); }

    // HIDE base overload by providing the same signature (3 args)
    DropListStyled& Add(const Value& key, const Value& label, bool enable = true) {
        key_to_label.GetAdd(key) = AsString(label);
        DropList::Add(key, label, enable);   // forward to base
        return *this;
    }

    void Paint(Draw& w) override {
        Size sz = GetSize();

        // background color by state
        const int st = VisualState();
        Color bg  = (st == CTRL_DISABLED) ? face_disabled
                 : (st == CTRL_PRESSED)  ? face_pressed
                 : (st == CTRL_HOT)      ? face_hot
                                          : face_normal;
        Color ink = (st == CTRL_DISABLED) ? ink_disabled : ink_normal;

        ImageBuffer ib(sz);
        ib.SetKind(IMAGE_ALPHA);
        Fill(~ib, RGBAZero(), ib.GetLength());
        {
            BufferPainter p(ib, MODE_ANTIALIASED);
            const double inset = 0.5;
            const double x = inset, y = inset;
            const double wdt = sz.cx - 2*inset;
            const double hgt = sz.cy - 2*inset;
            const int r = min(radius, min(sz.cx, sz.cy) / 2);

            p.Begin();
            p.RoundedRectangle(x, y, wdt, hgt, r);
            if(fill) p.Fill(bg);
            if(stroke > 0) {
                if(dashed) p.Dash(dash, 0.0);
                p.Stroke(stroke, border_color);
            }
            p.End();
        }
        w.DrawImage(0, 0, ib);

        // label text for the current key
        Value key = Get(); // current selected key
        String text = key_to_label.Get(key, AsString(key));
        Font f = StdFont().Height(DPI(11));
        Size ts = GetTextSize(text, f);

        const int padL = DPI(8);
        const int padR = DPI(8);

        // chevron
        Image arrow = CtrlImg::SortDown();
        const int aw = DPI(12), ah = DPI(12);
        int ax = sz.cx - padR - aw;
        int ay = (sz.cy - ah) / 2;

        // clip text so it doesn't run under the arrow
        Rect tr = RectC(padL, 0, ax - padL - DPI(4), sz.cy);
        w.Clip(tr);
        w.DrawText(padL, (sz.cy - ts.cy)/2, text, f, ink);
        w.End(); // end clip

        w.DrawImage(ax, ay, aw, ah, arrow);
    }

private:
    int VisualState() const {
        if(!IsShowEnabled()) return CTRL_DISABLED;
        if(IsPopUp())        return CTRL_PRESSED;
        return HasMouse() ? CTRL_HOT : CTRL_NORMAL;
    }
};


// ============================================================================
// Helpers to mount data onto DragBadgeButton
// ============================================================================
static inline DragBadgeButton& SetupSymbolTile(DragBadgeButton& b, const SymbolItem& s) {
    DragBadgeButton::Payload p;
    p.flavor="symbol"; p.id=s.charCode; p.group=s.categoryKey; p.name=s.name; p.text=s.name; p.badge=s.charCode;
    return b.SetText(s.name).SetBadgeText(s.charCode)
            .SetLayout(DragBadgeButton::ICON_CENTER_TEXT_BOTTOM)
            .SetPayload(p);
}

static inline DragBadgeButton& SetupCategoryChip(DragBadgeButton& b, const SymbolCategory& c) {
    DragBadgeButton::Payload p;
    p.flavor="category"; p.id=c.key; p.group=c.key; p.name=c.name; p.text=c.name;
    return b.SetText(c.name).ClearBadge()
            .SetLayout(DragBadgeButton::TEXT_CENTER)
            .SetPayload(p);
}

static inline DragBadgeButton& SetupPillButton(DragBadgeButton& b, const String& label) {
    DragBadgeButton::Payload p; p.text = label;
    return b.SetText(label).ClearBadge()
            .SetLayout(DragBadgeButton::TEXT_CENTER)
            .SetPayload(p);
}

// ============================================================================
// DropBinCard — StageCard subclass; content-only rounded, dashed on drop
// ============================================================================
class DropBinCard : public StageCard {
public:
    typedef DropBinCard CLASSNAME;

    Callback1<const SymbolItem&> WhenAdded;
    Callback1<const SymbolItem&> WhenRemoved;
    Callback                     WhenListChanged;

    Size tileSizes = Size(70, 50);

    DropBinCard() {
        SetTitle("Selection Bin (Drag & Drop Here)")
            .SetHeaderAlign(StageCard::LEFT)
            .EnableHeaderFill(false)
            .EnableCardFill(false)
            .EnableCardFrame(false)
            .EnableContentFill(true)
            .EnableContentFrame(true)
            .EnableContentDash(true)
            .SetContentCornerRadius(DPI(10))
            .SetContentFrameThickness(2);

        StackH(); SetWrap(); EnableContentScroll(true); EnableContentClampToPane(true);
        SetContentInset(DPI(8), DPI(8), DPI(8), DPI(8));
        SetContentGap(DPI(4), DPI(4));
    }

    DropBinCard& SetTileSize(Size s)      { tileSizes = s;  Reflow(); return *this; }
    DropBinCard& SetTileSize(int w,int h) { tileSizes = Size(w,h); Reflow(); return *this; }

    const Vector<SymbolItem>& GetSymbols() const { return items; }

    void AddSymbolUnique(const SymbolItem& s) {
        for(const auto& x : items) if(x.charCode == s.charCode) return;
        items.Add(s);
        if(WhenAdded) WhenAdded(s);
        RebuildTiles(); FireListChanged();
    }
    void RemoveIndex(int i) {
        if(i < 0 || i >= items.GetCount()) return;
        SymbolItem removed = items[i];
        items.Remove(i);
        if(WhenRemoved) WhenRemoved(removed);
        RebuildTiles(); FireListChanged();
    }
    void ClearAll() { items.Clear(); RebuildTiles(); FireListChanged(); }

    void DragAndDrop(Point, PasteClip& d) override {
        const bool ok_symbol   = AcceptInternal<DragBadgeButton>(d, "symbol");
        const bool ok_category = AcceptInternal<DragBadgeButton>(d, "category");
        if(!(ok_symbol || ok_category)) return;

        d.SetAction(DND_COPY);
        d.Accept();
        if(!d.IsPaste()) return;

        const DragBadgeButton& src = GetInternal<DragBadgeButton>(d);
        const auto& P = src.GetPayload();

        if(ok_symbol) {
            SymbolItem s; s.charCode=P.badge; s.name=IsEmpty(P.name) ? P.text : P.name; s.categoryKey=P.group;
            AddSymbolUnique(s);
        } else {
            for(const auto& cat : allCategories)
                if(cat.key == P.group)
                    for(const auto& it : cat.symbols) AddSymbolUnique(it);
        }
    }

    void Paint(Draw& w) override {
        StageCard::Paint(w);
        if(items.IsEmpty()) {
            Size sz = GetSize();
            String t = "Drag symbols or a whole category here. Click a tile to remove.";
            Font f = StdFont().Italic().Height(DPI(10));
            Size ts = GetTextSize(t, f);
            w.DrawText((sz.cx - ts.cx)/2, (sz.cy - ts.cy)/2, t, f, SColorDisabled());
        }
    }

private:
    Vector<SymbolItem>        items;
    Array<DragBadgeButton>    tiles;

    void FireListChanged() { if(WhenListChanged) WhenListChanged(); }

    void RebuildTiles() {
        for (int i = 0; i < tiles.GetCount(); ++i) tiles[i].Remove();
        tiles.Clear();

        for (int i = 0; i < items.GetCount(); ++i) {
            DragBadgeButton& t = tiles.Create();
            SetupSymbolTile(t, items[i])
                .SetMode(DragBadgeButton::DROPPED)
                .SetRadius(DPI(5))
                .SetStroke(1)
                .EnableDashed(false)
                .EnableFill(true);
            t.WhenRemove = THISBACK1(RemoveIndex, i);
            AddFixed(t, tileSizes.cx, tileSizes.cy);
        }
        Layout(); Refresh();
    }

    void Reflow() {
        for(int i = 0; i < tiles.GetCount(); ++i) tiles[i].SetMinSize(tileSizes);
        Layout(); Refresh();
    }
};

// ============================================================================
// Main app
// ============================================================================
class SymbolPickerApp : public TopWindow {
public:
    typedef SymbolPickerApp CLASSNAME;

    // Cards
    StageCard      headerCard;
    StageCard      categoryCard;
    StageCard      itemsCard;
    DropBinCard    binCard;
    StageCard      codeCard;

    // Header controls
    DragBadgeButton btnExit,btnCopy;

    // Theme selector
    DropListStyled styleDrop;

    // Left/Right split
    Splitter       mainSplitter;
    Splitter       leftSplit;

    // Content controls
    Array<DragBadgeButton> symbolTiles;
    Array<DragBadgeButton> categoryButtons;
    DocEdit        codeOutput;
    Option         hexMode;

    // State
    int            theme_id = 1; // Midnight default
    String         activeCategoryKey;
    Color          appBg     = SColorPaper();
    Size           tileSizes = Size(70, 50);

    // small layout cursor for header row
    int colStart = DPI(8), colPad = DPI(4), colX = DPI(0);
    int ColPos(int width, bool reset=false) { if(reset) colX=colStart; int cur=colX; colX += DPI(width) + colPad; return cur; }

    SymbolPickerApp() {
        Title("U++ Symbol Picker");
        Sizeable().Zoomable();
        SetMinSize(Size(DPI(800), DPI(500)));
        SetRect(Size(DPI(1200), DPI(800)));

        InitData();
        activeCategoryKey = allCategories.IsEmpty() ? String() : allCategories[0].key;

        // Header card (no fill; text only)
        headerCard
            .SetTitle("U++ Symbol Picker")
            .SetSubTitle("Filter by category, then drag symbols or an entire category into the Selection Bin. Export/Generate U++ code.")
            .SetHeaderAlign(StageCard::LEFT)
            .SetHeaderGap(DPI(6))
            .SetHeaderInset(0, DPI(8), 0, 0)
            .EnableCardFrame(false).EnableCardFill(false).EnableHeaderFill(false);
        Add(headerCard.HSizePos(DPI(8), DPI(8)).TopPos(DPI(8), DPI(60)));

        // Header buttons
        btnExit.SetText("Exit").SetRadius(DPI(10)).SetStroke(0).EnableFill(true).SetTileSize(90, 28);
        btnExit.WhenAction = [=]{ Close(); };

        btnCopy.SetText("Copy").SetRadius(DPI(10)).SetStroke(0).EnableFill(true).SetTileSize(90, 28);
        btnCopy.WhenAction = THISBACK(CopyToClipboard);
       
        Add(btnExit.RightPos(ColPos(80, true),  DPI(80)).TopPos(DPI(16), DPI(28)));
        Add(btnCopy.RightPos(ColPos(100),       DPI(80)).TopPos(DPI(16), DPI(28)));

        // DropListStyled for theme
        styleDrop.Add(0, "Light");
        styleDrop.Add(1, "Midnight");
        styleDrop.SetRadius(DPI(10)).SetStroke(0);
        styleDrop <<= theme_id;
        styleDrop.WhenAction = [=]{ ApplyStyleId((int)~styleDrop); };
        Add(styleDrop.RightPos(ColPos(100), DPI(100)).TopPos(DPI(15), DPI(28)));

        // Category bar
        categoryCard.EnableHeaderFill(false).EnableCardFill(false).EnableCardFrame(false)
                    .EnableContentFill(false).EnableContentFrame(false)
                    .EnableContentClampToPane(true).EnableContentScroll(true)
                    .StackH().SetWrap()
                    .SetContentInset(DPI(4), DPI(4), DPI(4), DPI(4))
                    .SetContentGap(DPI(6), DPI(6));
        Add(categoryCard.HSizePos(DPI(8), DPI(8)).TopPos(DPI(72), DPI(130)));

        for (int i = 0; i < allCategories.GetCount(); ++i) {
            const auto& cat = allCategories[i];
            DragBadgeButton& b = categoryButtons.Create();
            SetupCategoryChip(b, cat)
                .SetMode(DragBadgeButton::DRAGABLE)
                .SetTileSize(Size(150, 25))
                .SetRadius(DPI(10))
                .SetStroke(0)
                .EnableDashed(false)
                .EnableFill(true)
                .SetHoverTintPercent(22)

                .WhenAction = THISBACK1(FilterCategory, cat.key);
            categoryCard.AddFixed(b, 150, 25);
        }

        // Items card
        itemsCard.SetTitle("Items").SetHeaderAlign(StageCard::LEFT)
                .EnableHeaderFill(false).EnableCardFill(false).EnableCardFrame(false)
                .EnableContentFill(true).EnableContentFrame(false)
                .SetContentCornerRadius(DPI(10)).SetContentFrameThickness(0)
                .EnableContentScroll(true).EnableContentClampToPane(true)
                .StackH().SetWrap()
                .SetContentInset(DPI(6), DPI(6), DPI(6), DPI(6))
                .SetContentGap(DPI(6), DPI(6));

        // Bin card (dashed frame enabled inside ApplyStyleId via a clone style)
        binCard.SetTileSize(tileSizes).WhenListChanged = THISBACK(UpdateCodeOutput);

        // Code card
        codeCard.SetTitle("U++ Code Output").SetHeaderAlign(StageCard::LEFT)
                .EnableHeaderFill(true).EnableCardFill(false).EnableCardFrame(false)
                .EnableContentFill(true).EnableContentFrame(false)
                .SetContentCornerRadius(DPI(10)).SetContentFrameThickness(0)
                .SetContentInset(DPI(6), DPI(6), DPI(6), DPI(6))
                .EnableContentScroll(false);

        codeOutput.SetReadOnly();
        codeOutput.Transparent();
        codeOutput.SetFrame(NullFrame());
        codeOutput.SetFont(Monospace(10));
        codeOutput.SetColor(DocEdit::INK_NORMAL,   Color(170,255,170));
        codeOutput.SetColor(DocEdit::PAPER_NORMAL, Null);
        codeOutput.SetColor(DocEdit::PAPER_READONLY, Null);
        codeOutput.SetColor(DocEdit::PAPER_SELECTED, Blend(Color(32,36,42), SColorHighlight(), 50));
        codeCard.ReplaceExpand(codeOutput);
        codeOutput.HSizePos(0, 0).VSizePos(0, 0);

        // Splitters
        leftSplit.Vert(itemsCard, binCard).SetPos(7000);
        mainSplitter.Horz(leftSplit, codeCard).SetPos(6000);
        leftSplit.Transparent(); mainSplitter.Transparent();
        Add(mainSplitter.HSizePos(DPI(8), DPI(8)).VSizePos(DPI(210), DPI(8)));

        // Initial
        if(!activeCategoryKey.IsEmpty()) FilterCategory(activeCategoryKey);
        UpdateCodeOutput();
        ApplyStyleId(theme_id);
    }

    void Paint(Draw& w) override { w.DrawRect(GetSize(), appBg); }

    void ApplyStyleId(int id)
    {
        theme_id = id;
        const int theme_count = (int)(sizeof(THEMES)/sizeof(THEMES[0]));
        const AppTheme& T = THEMES[clamp(theme_id,0,theme_count-1)];
        appBg = T.window_bg;

        StageCard::Style s = T.make_style();

        // header
        headerCard.SetStyleOwned(s)
                  .EnableHeaderFill(false).EnableCardFill(false).EnableCardFrame(false)
                  .SetTitleFont(StdFont().Bold().Height(DPI(25)))
                  .SetHeaderGap(DPI(6)).SetHeaderInset(0, DPI(8), 0, 0);

        // category strip (no backgrounds)
        categoryCard.SetStyleOwned(s)
                    .EnableHeaderFill(false).EnableCardFill(false).EnableCardFrame(false)
                    .EnableContentFill(false).EnableContentFrame(false)
                    .SetContentFrameThickness(0);

        // items paper
        itemsCard.SetStyleOwned(s)
                 .EnableHeaderFill(false).EnableCardFill(false).EnableCardFrame(false)
                 .EnableContentFill(true).EnableContentFrame(false)
                 .SetContentCornerRadius(DPI(10)).SetContentFrameThickness(0)
                 .SetContentColor(T.card_content_bg);

        // bin paper + dashed, with stronger border color
        {
            StageCard::Style sb = s;
            sb.palette.cardBorder = T.bin_stroke;
            binCard.SetStyleOwned(sb)
                   .EnableHeaderFill(false).EnableCardFill(false).EnableCardFrame(false)
                   .EnableContentFill(true).EnableContentFrame(true).EnableContentDash(true)
                   .SetContentCornerRadius(DPI(10)).SetContentFrameThickness(2);
        }

        // code paper
        codeCard.SetStyleOwned(s)
                .EnableHeaderFill(false).EnableCardFill(false).EnableCardFrame(false)
                .EnableContentFill(true).EnableContentFrame(false)
                .SetContentCornerRadius(DPI(10)).SetContentFrameThickness(0)
                .SetContentColor(T.code_content_bg);

        // Style header buttons
        btnExit.SetBaseColors(ButtonFaceRed(), T.chip_border, White());
        btnCopy.SetBaseColors(ButtonFaceBlue(), T.chip_border, White());


        // Category chips gradient across row
        int n = categoryButtons.GetCount();
        for(int i=0;i<n;++i) {
            int t = (n>1) ? i*100/(n-1) : 0;
            Color face = BlendPct(T.chip_face_a, T.chip_face_b, t);
            categoryButtons[i].SetBaseColors(face, T.chip_border, T.chip_ink);
        }

        // Symbol tiles
        for(int i=0;i<symbolTiles.GetCount();++i) {
            symbolTiles[i].SetBaseColors(T.tile_face, T.tile_border, T.tile_ink);
            symbolTiles[i].SetFont( StdFont().Height(DPI(8)) );
        }
        // Style selector itself
        if(theme_id == 1) { // Midnight
            styleDrop.SetBgColor(Gray()).SetTextColor(Black());
        } else {            // Light
            styleDrop.SetBgColor(Color(36,99,235)).SetTextColor(White());
        }

        Layout(); Refresh();
    }

    void FilterCategory(const String& key) {
        activeCategoryKey = key;
        for (int i=0;i<categoryButtons.GetCount();++i)
            categoryButtons[i].SetSelected(allCategories[i].key == key);
        for (const auto& cat : allCategories)
            if(cat.key == key) { itemsCard.SetTitle(cat.name); break; }
        UpdateSymbolGrid();
    }

    void UpdateSymbolGrid() {
        for (int i = 0; i < symbolTiles.GetCount(); ++i) symbolTiles[i].Remove();
        symbolTiles.Clear();

        const SymbolCategory* activeCat = nullptr;
        for (const auto& cat : allCategories) if(cat.key == activeCategoryKey) { activeCat = &cat; break; }
        if(!activeCat) return;

        for (int i = 0; i < activeCat->symbols.GetCount(); ++i) {
            const auto& item = activeCat->symbols[i];
            DragBadgeButton& tile = symbolTiles.Create();
            SetupSymbolTile(tile, item).SetMode(DragBadgeButton::DRAGABLE);
            itemsCard.AddFixed(tile, tileSizes.cx, tileSizes.cy);
        }
        itemsCard.Layout();
        ApplyStyleId(theme_id); // restyle new tiles
    }

    void UpdateCodeOutput() {
        String out; out << "// Picker Selections v1.0\n\n";
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
        for(int i = 0; i < grouped.GetCount(); i++) keys.Add(grouped.GetKey(i));
        Sort(keys);

        const bool useHex = (bool)hexMode;

        for(const String& catName : keys) {
            out << "// " << catName << "\n";
            for (const auto& item : grouped.Get(catName)) {
                if(useHex) {
                    WString ws = ToUnicode(item.charCode, CHARSET_UTF8);
                    dword cp = ws.IsEmpty() ? 0u : (dword)ws[0];
                    out << Format("myControl.SetLabel(FromUtf32(0x%X)); // %s\n", (int)cp, ~item.name);
                } else {
                    out << Format("myControl.SetLabel(\"%s\"); // %s\n", ~item.charCode, ~item.name);
                }
            }
            out << "\n";
        }
        codeOutput.Set(out.ToWString());
    }

    void CopyToClipboard() { WriteClipboardText(codeOutput.Get()); }
};

GUI_APP_MAIN
{
    SymbolPickerApp().Run();
}
