#include <CtrlLib/CtrlLib.h>
#include <Painter/Painter.h>
#include <StageCard/StageCard.h>

using namespace Upp;

class ChipBtn : public Button {
public:
    ChipBtn(const String& s = Null) { SetLabel(s); SetMinSize(Size(DPI(96), DPI(28))); NoWantFocus(); }
};

class DemoApp : public TopWindow {
public:
    typedef DemoApp CLASSNAME;

    Splitter mainSplit;

    StageCard leftStack;
    StageCard rightStack;

    StageCard flowCard;
    StageCard autofillBtns;

    StageCard staticGrid;
    StageCard responsiveArea;

    Button add10, add40, clearFlow;
    Button themeToggle;
    Button modeToggle;
    EditIntSpin staticCols;

    bool responsiveIsGrid = true;

    Array<ChipBtn> chips;              // for flowCard
    Button primaryAction, secondaryAction; // for autofillBtns
    Array<Button> staticCells;         // for staticGrid
    Array<Button> respCells;           // for responsiveArea

    DemoApp() {
        Title("StageCard demo — one splitter, stacked via AutoFill");
        Sizeable().Zoomable();
        SetMinSize(Size(DPI(900), DPI(600)));
        SetRect(0, 0, DPI(1120), DPI(720));

        // LEFT STACK
        leftStack
            .SetTitle("Left stack (AutoFill container)")
            .EnableCardFill(false).EnableCardFrame(false)
            .EnableHeaderFill(true).SetHeaderFrameColors(Color(228,238,252), GrayColor(200))
            .SetHeaderAlign(StageCard::LEFT)
            .ContentAbsolute().EnableContentAutoFill(true)
            .EnableContentFill(false).EnableContentFrame(false)
            .SetContentInset(DPI(8), DPI(8), DPI(8), DPI(8))
            .SetContentGap(DPI(8), DPI(8));

        flowCard
            .SetTitle("Flow layout (WRAP)")
            .SetSubTitle("Chips wrap; scroll on overflow")
            .SetBadge("↩")
            .SetBadgeFont(StdFont().Height(DPI(20)))
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
        add10.WhenAction   = [=]{ AddChips(10); };
        add40.WhenAction   = [=]{ AddChips(40); };
        clearFlow.WhenAction = [=]{ flowCard.ClearContent(); chips.Clear(); flowCard.Layout(); };
        flowCard.AddHeader(add10).AddHeader(add40).AddHeader(clearFlow);
        AddChips(18);

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
            .SetContentFrameColors(Color(32,36,42), Color(0,0,0))
            .SetContentInset(DPI(10), DPI(10), DPI(10), DPI(10))
            .ContentAbsolute().EnableContentAutoFill(true)
            .EnableContentScroll(false);

        primaryAction.SetLabel("Primary Action"); primaryAction.SetFont(StdFont().Bold());
        secondaryAction.SetLabel("Secondary Action");
        autofillBtns.AddContent(primaryAction);
        autofillBtns.AddContent(secondaryAction);

        themeToggle.SetLabel("Dark/Light");
        themeToggle.WhenAction = [=]{ ToggleAutoFillTheme(); };
        autofillBtns.AddHeader(themeToggle);

        leftStack.AddContent(flowCard);
        leftStack.AddContent(autofillBtns);

        // RIGHT STACK
        rightStack
            .SetTitle("Right stack (AutoFill container)")
            .EnableCardFill(false).EnableCardFrame(false)
            .EnableHeaderFill(true).SetHeaderFrameColors(Color(226,244,230), GrayColor(200))
            .SetHeaderAlign(StageCard::LEFT)
            .ContentAbsolute().EnableContentAutoFill(true)
            .EnableContentFill(false).EnableContentFrame(false)
            .SetContentInset(DPI(8), DPI(8), DPI(8), DPI(8))
            .SetContentGap(DPI(8), DPI(8));

        staticGrid
            .SetTitle("Static Grid (uniform cells)")
            .SetSubTitle("Fixed cell size; scroll on overflow")
            .SetBadge("▦")
            .SetBadgeFont(StdFont().Height(DPI(20)))
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
            .GridCell(DPI(120), DPI(72))
            .GridStretch(true)
            .SetContentInset(DPI(10), DPI(10), DPI(10), DPI(10))
            .SetContentGap(DPI(10), DPI(10))
            .EnableContentScroll(true);

        for(int i = 1; i <= 16; ++i) {
            Button& c = staticCells.Add();
            c.SetLabel(Format("Cell %d", i));
            c.SetMinSize(Size(DPI(120), DPI(72)));
            staticGrid.AddContent(c);
        }

        staticCols.MinMax(1, 8).SetData(4);
        staticCols.WhenAction = [=]{ staticGrid.GridCols((int)staticCols.GetData()); staticGrid.Layout(); };
        staticGrid.AddHeader(staticCols);

        responsiveArea
            .SetTitle("Responsive area")
            .SetSubTitle("Header button toggles: 3×3 Grid  ↔  3×3 Full Buttons")
            .SetBadge("FILL")
            .SetHeaderAlign(StageCard::LEFT)
            .EnableHeaderFill(true).SetHeaderFrameColors(Color(210,244,218), GrayColor(180))
            .EnableCardFill(false).EnableCardFrame(false)
            .EnableContentFill(false).EnableContentFrame(true).EnableContentDashed(true)
            .SetContentCornerRadius(DPI(0))
            .SetContentStrokeThickness(1)
            .SetContentFrameColors(SColorPaper(), GrayColor(140))
            .ContentGrid()
            .GridCols(3)
            .GridStretch(true)
            .GridFill(true)
            .SetContentInset(DPI(10), DPI(10), DPI(10), DPI(10))
            .SetContentGap(DPI(10), DPI(10))
            .EnableContentScroll(false);

        modeToggle.SetLabel("Switch Mode");
        modeToggle.WhenAction = [=]{ ToggleResponsiveMode(); };
        responsiveArea.AddHeader(modeToggle);

        BuildResponsiveGrid();

        rightStack.AddContent(staticGrid);
        rightStack.AddContent(responsiveArea);

        mainSplit.Horz(leftStack, rightStack);
        mainSplit.SetPos(5200);
        Add(mainSplit.HSizePos(DPI(10), DPI(10)).VSizePos(DPI(10), DPI(10)));
    }

private:
    void AddChips(int n) {
        int start = Random(1000);
        for(int i = 0; i < n; ++i) {
            ChipBtn& c = chips.Add();
            c.SetLabel(Format("Chip %d", start + i));
            flowCard.AddContent(c);
        }
        flowCard.Layout();
    }

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

    void ToggleResponsiveMode() {
        responsiveIsGrid = !responsiveIsGrid;
        responsiveArea.ClearContent();
        respCells.Clear();
        if(responsiveIsGrid) BuildResponsiveGrid();
        else                 BuildResponsiveButtons();
        responsiveArea.Layout();
    }

    void BuildResponsiveGrid() {
        for(int i = 1; i <= 9; ++i) {
            Button& c = respCells.Add();
            c.SetLabel(Format("Cell %d", i));
            responsiveArea.AddContent(c);
        }
    }

    void BuildResponsiveButtons() {
        for(int i = 1; i <= 9; ++i) {
            Button& b = respCells.Add();
            b.SetLabel(Format("Action %d", i));
            b.SetFont(StdFont().Bold());
            responsiveArea.AddContent(b);
        }
    }
};

GUI_APP_MAIN
{
    DemoApp().Run();
}