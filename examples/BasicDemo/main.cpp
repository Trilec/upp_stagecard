#include <CtrlLib/CtrlLib.h>
#include <Painter/Painter.h>
#include <StageCard/StageCard.h>

using namespace Upp;

// tiny “chip” button used in the flow demo
class ChipBtn : public Button {
public:
    ChipBtn(const String& s = Null) { SetLabel(s); SetMinSize(Size(DPI(96), DPI(28))); NoWantFocus(); }
};

class DemoApp : public TopWindow {
public:
    typedef DemoApp CLASSNAME;

    // outer split: left | right (only one splitter)
    Splitter mainSplit;

    // each side is a StageCard that stacks two child cards via AutoFill
    StageCard leftStack;   // [Flow] over [AutoFill buttons]
    StageCard rightStack;  // [Static Grid] over [Responsive Grid/Buttons]

    // child cards (left side)
    StageCard flowCard;
    StageCard autofillBtns;

    // child cards (right side)
    StageCard staticGrid;
    StageCard responsiveArea;   // toggles between responsive grid and full-size buttons

    // header controls
    Button add10, add40, clearFlow;
    Button themeToggle;             // for left lower card
    Button modeToggle;              // for right lower card (Grid <-> Buttons)
    EditIntSpin staticCols;         // change columns of the static grid

    // state
    bool responsiveIsGrid = true;

    DemoApp() {
        Title("StageCard demo — one splitter, stacked via AutoFill");
        Sizeable().Zoomable();
        SetMinSize(Size(DPI(900), DPI(600)));
        SetRect(0, 0, DPI(1120), DPI(720));

        // =============== LEFT STACK CONTAINER =================
        leftStack
            .SetTitle("Left stack (AutoFill container)")
            .EnableCardFill(false).EnableCardFrame(false)
            .EnableHeaderFill(true).SetHeaderFrameColors(Color(228,238,252), GrayColor(200))
            .SetHeaderAlign(StageCard::LEFT)
            .ContentAbsolute().EnableContentAutoFill(true)   // <<< stacks its children equally
            .EnableContentFill(false).EnableContentFrame(false)
            .SetContentInset(DPI(8), DPI(8), DPI(8), DPI(8))
            .SetContentGap(DPI(8), DPI(8));

        // ---- Flow (top-left) ----
        flowCard
            .SetTitle("Flow layout (WRAP)")
            .SetSubTitle("Chips wrap; scroll on overflow")
            .SetBadge("WRAP")
            .SetHeaderAlign(StageCard::LEFT)
            .SetTitleUnderlineThickness(DPI(2))
            .SetTitleUnderlineColor(Color(22,86,160))
            .EnableCardFill(false).EnableCardFrame(false)
            .EnableContentFill(true).EnableContentFrame(true)
            .SetContentCornerRadius(DPI(8))
            .SetContentStrokeThickness(1)
            .SetContentFrameColors(Blend(SColorPaper(), SColorFace(), 10), GrayColor(200))
            .ContentWrap()
            .SetContentInset(DPI(10), DPI(10), DPI(10), DPI(10))
            .SetContentGap(DPI(8), DPI(8))
            .WrapItemSize(DPI(96), DPI(28))
            .EnableContentScroll(true);

        add10.SetLabel("Add 10");
        add40.SetLabel("Add 40");
        clearFlow.SetLabel("Clear");
        add10.WhenAction   = [=]{ AddChips(flowCard, 10); };
        add40.WhenAction   = [=]{ AddChips(flowCard, 40); };
        clearFlow.WhenAction = [=]{ flowCard.ClearContent(); flowCard.Layout(); };
        flowCard.AddHeader(add10).AddHeader(add40).AddHeader(clearFlow);

        AddChips(flowCard, 18);

        // ---- AutoFill buttons (bottom-left) ----
        autofillBtns
            .SetTitle("Auto-fill (two buttons)")
            .SetSubTitle("Children share height equally")
            .SetBadge("AUTO")
            .SetHeaderAlign(StageCard::LEFT)
            .EnableHeaderFill(true).SetHeaderFrameColors(Color(210,226,245), GrayColor(180))
            .EnableCardFill(false).EnableCardFrame(false)
            .EnableContentFill(true).EnableContentFrame(true)
            .SetContentCornerRadius(DPI(10))
            .SetContentStrokeThickness(0)
            .SetContentFrameColors(Color(32,36,42), Color(0,0,0)) // dark panel
            .SetContentInset(DPI(10), DPI(10), DPI(10), DPI(10))
            .ContentAbsolute().EnableContentAutoFill(true)        // <<< splits children
            .EnableContentScroll(false);

        Button& a = *new Button(); a.SetLabel("Primary Action");  a.SetFont(StdFont().Bold());
        Button& b = *new Button(); b.SetLabel("Secondary Action");
        autofillBtns.AddContent(a);
        autofillBtns.AddContent(b);

        themeToggle.SetLabel("Dark/Light");
        themeToggle.WhenAction = [=]{ ToggleAutoFillTheme(); };
        autofillBtns.AddHeader(themeToggle);

        // mount left children into leftStack
        leftStack.AddContent(flowCard);
        leftStack.AddContent(autofillBtns);

        // =============== RIGHT STACK CONTAINER =================
        rightStack
            .SetTitle("Right stack (AutoFill container)")
            .EnableCardFill(false).EnableCardFrame(false)
            .EnableHeaderFill(true).SetHeaderFrameColors(Color(226,244,230), GrayColor(200))
            .SetHeaderAlign(StageCard::LEFT)
            .ContentAbsolute().EnableContentAutoFill(true)
            .EnableContentFill(false).EnableContentFrame(false)
            .SetContentInset(DPI(8), DPI(8), DPI(8), DPI(8))
            .SetContentGap(DPI(8), DPI(8));

        // ---- Static grid (top-right) ----
        staticGrid
            .SetTitle("Static Grid (uniform cells)")
            .SetSubTitle("Fixed cell size; scroll on overflow")
            .SetBadge("GRID")
            .SetHeaderAlign(StageCard::LEFT)
            .SetTitleUnderlineThickness(DPI(2))
            .SetTitleUnderlineColor(Color(22,86,160))
            .EnableCardFill(false).EnableCardFrame(false)
            .EnableContentFill(true).EnableContentFrame(true)
            .SetContentCornerRadius(DPI(8))
            .SetContentStrokeThickness(1)
            .SetContentFrameColors(Blend(SColorPaper(), SColorFace(), 10), GrayColor(200))
            .ContentGrid()
            .GridCols(4)
            .GridCell(DPI(120), DPI(72))         // fixed cells
            .GridStretch(true)
            .SetContentInset(DPI(10), DPI(10), DPI(10), DPI(10))
            .SetContentGap(DPI(10), DPI(10))
            .EnableContentScroll(true);

        for(int i = 1; i <= 16; ++i) {
            Button& c = *new Button();
            c.SetLabel(Format("Cell %d", i));
            c.SetMinSize(Size(DPI(120), DPI(72)));
            staticGrid.AddContent(c);
        }

        staticCols.MinMax(1, 8).SetData(4);
        staticCols.WhenAction = [=]{ staticGrid.GridCols((int)staticCols.GetData()); staticGrid.Layout(); };
        staticGrid.AddHeader(staticCols);

        // ---- Responsive area (bottom-right) ----
        responsiveArea
            .SetTitle("Responsive area")
            .SetSubTitle("Header button toggles: 3×3 Grid  ↔  3×3 Full Buttons")
            .SetBadge("FILL")
            .SetHeaderAlign(StageCard::LEFT)
            // header: soft green to highlight styling
            .EnableHeaderFill(true).SetHeaderFrameColors(Color(210,244,218), GrayColor(180))
            .EnableCardFill(false).EnableCardFrame(false)
            // body: dashed frame, NO fill so dashes are visible
            .EnableContentFill(false).EnableContentFrame(true).EnableContentDashed(true)
            .SetContentCornerRadius(DPI(0))
            .SetContentStrokeThickness(1)
            .SetContentFrameColors(SColorPaper(), GrayColor(140))
            .ContentGrid()
            .GridCols(3)
            .GridStretch(true)
            .GridFill(true)                       // <<< responsive: cells fill the pane
            .SetContentInset(DPI(10), DPI(10), DPI(10), DPI(10))
            .SetContentGap(DPI(10), DPI(10))
            .EnableContentScroll(false);

        modeToggle.SetLabel("Switch Mode");
        modeToggle.WhenAction = [=]{ ToggleResponsiveMode(); };
        responsiveArea.AddHeader(modeToggle);

        BuildResponsiveGrid(); // start in grid mode

        // mount right children into rightStack
        rightStack.AddContent(staticGrid);
        rightStack.AddContent(responsiveArea);

        // =============== single splitter =================
        mainSplit.Horz(leftStack, rightStack);
        mainSplit.SetPos(5200); // ~52% left / 48% right
        Add(mainSplit.HSizePos(DPI(10), DPI(10)).VSizePos(DPI(10), DPI(10)));
    }

private:
    // add a bunch of chips to the flow card
    void AddChips(StageCard& card, int n) {
        int start = Random(1000);
        for(int i = 0; i < n; ++i) {
            ChipBtn& c = *new ChipBtn(Format("Chip %d", start + i));
            card.AddContent(c);
        }
        card.Layout();
    }

    // left-bottom theme toggle
    void ToggleAutoFillTheme() {
        static bool dark = true;
        dark = !dark;
        if(dark) {
            autofillBtns
                .EnableContentFill(true).EnableContentFrame(true)
                .SetContentCornerRadius(DPI(10))
                .SetContentStrokeThickness(0)
                .SetContentFrameColors(Color(32,36,42), Color(0,0,0));
        } else {
            autofillBtns
                .EnableContentFill(true).EnableContentFrame(true)
                .SetContentCornerRadius(DPI(8))
                .SetContentStrokeThickness(1)
                .SetContentFrameColors(Blend(SColorPaper(), SColorFace(), 10), GrayColor(200));
        }
        autofillBtns.Layout();
        autofillBtns.Refresh();
    }

    // right-bottom mode toggle
    void ToggleResponsiveMode() {
        responsiveIsGrid = !responsiveIsGrid;
        responsiveArea.ClearContent();
        if(responsiveIsGrid) BuildResponsiveGrid();
        else                 BuildResponsiveButtons();
        responsiveArea.Layout();
    }

    void BuildResponsiveGrid() {
        for(int i = 1; i <= 9; ++i) {
            Button& c = *new Button();
            c.SetLabel(Format("Cell %d", i));
            responsiveArea.AddContent(c); // size comes from GridFill + GridCols(3)
        }
    }

    void BuildResponsiveButtons() {
        for(int i = 1; i <= 9; ++i) {
            Button& b = *new Button();
            b.SetLabel(Format("Action %d", i));
            b.SetFont(StdFont().Bold());
            responsiveArea.AddContent(b); // still fills via GridFill
        }
    }
};

GUI_APP_MAIN
{
    DemoApp().Run();
}
