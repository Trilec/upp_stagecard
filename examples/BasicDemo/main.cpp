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

    Button add10, clearFlow;
    Button themeToggle;
    Button modeToggle;

    //bool responsiveIsGrid = true;
	
	bool expandWeighted = true; 
    
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
            .SetTitle("Left stack ( Vertical )")
            .SetHeaderColor( Color(228,238,252), GrayColor(200 ) )
            .SetHeaderAlign(StageCard::LEFT)
            .SetStackV()
            .EnableCardFill(false)
            .EnableCardFrame(false)
            .EnableHeaderFill(true)
            .EnableContentFill(false)
            .EnableContentFrame(false)
            .SetContentInset(DPI(8), DPI(8), DPI(8), DPI(8))
            .SetContentGap(DPI(8), DPI(8));

        flowCard
            .SetTitle("Horizontal Wrapping")
            .SetSubTitle("Chips wrap; scroll on overflow")
            .SetContentColor( Blend(SColorPaper(), SColorFace(), 10), GrayColor(200))
            .SetHeaderAlign(StageCard::LEFT)
            .SetWrap(true)
            .SetBadge("↩")
            
            .SetBadgeFont(StdFont().Height(DPI(20)))
            .SetTitleUnderlineThickness(DPI(2))
            .SetTitleUnderlineColor(Color(22,86,160))
            .EnableCardFill(false).EnableCardFrame(false)
            .EnableContentFill(true).EnableContentFrame(true)
            .SetContentCornerRadius(DPI(8))
            .SetContentFrameThickness(1)
            .SetContentInset(DPI(10), DPI(10), DPI(10), DPI(10))
            .SetContentGap(DPI(8), DPI(8))
            .EnableContentScroll(true);

        add10.SetLabel("Add 10");
        clearFlow.SetLabel("Clear");
        add10.WhenAction   = [=]{ AddChips( flowCard,20,100,30); };
        
        clearFlow.WhenAction = [=]{ flowCard.ClearContent(); chips.Clear(); flowCard.Layout(); };
        flowCard.AddHeader(add10.RightPos(DPI(8), DPI(80)).TopPos(DPI(12), DPI(28)) );
        flowCard.AddHeader(clearFlow.RightPos(DPI(98), DPI(80)).TopPos(DPI(12), DPI(28)) );
        AddChips(flowCard,20,100,30);

        autofillBtns
            .SetTitle("Two buttons Expanded (Vertical)")
            .SetSubTitle("Children share height equally")
            .SetHeaderColor(Color(210,226,245), GrayColor(180))
            .SetBadge("AUTO")
            .SetStackV()
            .SetHeaderAlign(StageCard::LEFT)
            .EnableHeaderFill(true)
            .EnableCardFill(false).EnableCardFrame(false)
            .EnableContentFill(true).EnableContentFrame(true)
            .SetContentCornerRadius(DPI(10))
            .SetContentFrameThickness(0)
            .SetContentInset(DPI(10), DPI(10), DPI(10), DPI(10))
            .EnableContentScroll(false);

        primaryAction.SetLabel("Primary Action"); primaryAction.SetFont(StdFont().Bold());
        secondaryAction.SetLabel("Secondary Action");
        autofillBtns.AddExpand(primaryAction);
        autofillBtns.AddExpand(secondaryAction);

        themeToggle.SetLabel("Dark/Light");
        themeToggle.WhenAction = [=]{ ToggleAutoFillTheme(); };
        autofillBtns.AddHeader(themeToggle);

        leftStack.AddExpand(flowCard);
        leftStack.AddExpand(autofillBtns);

        // RIGHT STACK
        rightStack
            .SetTitle("Right stack")
            .EnableHeaderFill(true).SetHeaderColor(Color(226,244,230), GrayColor(200))
            .SetHeaderAlign(StageCard::RIGHT)
            .SetStackV()
            .EnableCardFill(false)
            .EnableCardFrame(false)
            .EnableHeaderFill(true)
            .EnableContentFill(false)
            .EnableContentFrame(false)
            .SetContentInset(DPI(8), DPI(8), DPI(8), DPI(8))
            .SetContentGap(DPI(8), DPI(8));

        staticGrid
		    .SetTitle("Static Grid (No Stack)")
		    .SetSubTitle("Manual placement of cells, no Stack or wrap")
		    .SetBadge("▦")
		    .SetBadgeFont(StdFont().Height(DPI(20)))
		    .SetHeaderAlign(StageCard::RIGHT)
		    .SetTitleUnderlineThickness(DPI(2))
		    .SetTitleUnderlineColor(Color(22,86,160))
		    .EnableCardFill(false).EnableCardFrame(false)
		    .EnableContentFill(true).EnableContentFrame(true)
		    .SetContentCornerRadius(DPI(8))
		    .SetContentFrameThickness(1)
		    .SetContentColor(Blend(SColorPaper(), SColorFace(), 10), GrayColor(200))
		    // Choose vertical axis, then disable stacking -> manual + vertical scroll
		    .SetStackV()
		    .SetStackNone()
		    .SetContentInset(DPI(10), DPI(10), DPI(10), DPI(10))
		    .EnableContentScroll(true);

			// We are in MANUAL mode: use staticGrid.Content() and SetRect().
			ParentCtrl& grid = staticGrid.Content();
			
			// Layout parameters
			const int startX  = DPI(10);
			const int startY  = DPI(10);
			const int gapX    = DPI(8);
			const int gapY    = DPI(8);
			
			const int smallW  = DPI(90);
			const int smallH  = DPI(30);
			const int bigW1   = DPI(110);
			const int bigW2   = DPI(140);
			const int bigH1   = DPI(36);
			const int bigH2   = DPI(44);
			
			// ----- First block: 3 across, several rows of small buttons -----
			int x = startX;
			int y = startY;
			int col = 0;
			
			for(int i = 0; i < 12; ++i) {        // 12 buttons: 3 x 4 grid-ish
			    Button& c = staticCells.Add();
			    c.SetLabel(Format("Cell %d", i + 1));
			
			    grid.Add(c);
			    c.SetRect(x, y, smallW, smallH);
			
			    ++col;
			    if(col == 3) {
			        col = 0;
			        x = startX;
			        y += smallH + gapY;
			    } else {
			        x += smallW + gapX;
			    }
			}
			
			// ----- Second block: 1 column of bigger buttons below -----
			y += DPI(20);   // extra gap between groups
			x = startX;
			
			for(int i = 12; i < 20; ++i) {      // 8 more buttons, taller / wider
			    Button& c = staticCells.Add();
			    c.SetLabel(Format("Cell %d", i + 1));
			
			    grid.Add(c);
			
			    // Alternate sizes a bit to show variety
			    int w = (i % 2 == 0) ? bigW2 : bigW1;
			    int h = (i % 2 == 0) ? bigH2 : bigH1;
			
			    c.SetRect(x, y, w, h);
			    y += h + gapY;
			}


		responsiveArea
		    .SetTitle("Expand Area")
		    .SetSubTitle("3 panels using AddExpand; toggle equal vs weighted (1:2:1)")
		    .SetBadge("FILL")
		    .SetHeaderAlign(StageCard::RIGHT)
		    .EnableHeaderFill(true).SetHeaderColor(Color(210,244,218), GrayColor(180))
		    .EnableCardFill(false).EnableCardFrame(false)
		    .EnableContentFill(true).EnableContentFrame(true).EnableContentDash(false)
		    .SetContentCornerRadius(DPI(6))
		    .SetContentFrameThickness(1)
		    .SetContentColor(Blend(SColorPaper(), SColorFace(), 10), GrayColor(200))
		    .SetStackH()                  // <--- horizontal stack
		    .SetWrap(false)               // <--- no wrap; overflow = horizontal scroll
		    .SetContentInset(DPI(10), DPI(10), DPI(10), DPI(10))
		    .SetContentGap(DPI(8), DPI(0))// horizontal gaps only
		    .EnableContentScroll(true);   // <--- important: show scroll when needed
		
		modeToggle.SetLabel("Toggle weights");
		modeToggle.WhenAction = [=]{ ToggleResponsiveMode(); };
		responsiveArea.AddHeader(modeToggle);

        BuildResponsiveGrid();

        rightStack.AddExpand(staticGrid);
        rightStack.AddExpand(responsiveArea);

        mainSplit.Horz(leftStack, rightStack);
        mainSplit.SetPos(5200);
        Add(mainSplit.HSizePos(DPI(10), DPI(10)).VSizePos(DPI(10), DPI(10)));
    }

private:
    void AddChips(StageCard &card, int n,int w, int h) {
        int start = Random(1000);
        for(int i = 0; i < n; ++i) {
            ChipBtn& c = chips.Add();
            c.SetLabel(Format("Chip %d", start + i));
            card.AddFixed(c,w,h);
        }
        card.Layout();
    }

    void ToggleAutoFillTheme() {
        static bool dark = true;
        dark = !dark;
        if(dark) {
            autofillBtns
                .EnableContentFill(true).EnableContentFrame(true)
                .SetContentCornerRadius(DPI(10))
                .SetContentFrameThickness(0)
                .SetContentColor(Color(32,36,42), Color(0,0,0));
        } else {
            autofillBtns
                .EnableContentFill(true).EnableContentFrame(true)
                .SetContentCornerRadius(DPI(8))
                .SetContentFrameThickness(1)
                .SetContentColor(Blend(SColorPaper(), SColorFace(), 10), GrayColor(200));
        }
        autofillBtns.Layout();
        autofillBtns.Refresh();
    }

	void ToggleResponsiveMode() {
	    expandWeighted = !expandWeighted;
	    BuildResponsiveGrid();
	}
	
	// This now means: build the Expand demo
	void BuildResponsiveGrid() {
	    responsiveArea.ClearContent();
	    respCells.Clear();
	
	    // We’ll always build 3 panels
	    static const char* baseLabels[3] = {
	        "Left panel",
	        "Center panel",
	        "Right panel"
	    };
	
	    int weights[3];
	
	    if(expandWeighted) {
	        // Weighted: middle = 2×
	        weights[0] = 1;
	        weights[1] = 2;
	        weights[2] = 1;
	    } else {
	        // Equal: all 1×
	        weights[0] = weights[1] = weights[2] = 1;
	    }
	
	    for(int i = 0; i < 3; ++i) {
	        Button& c = respCells.Add();
	
	        c.SetLabel(
	            Format("%s (%d weight)", baseLabels[i], weights[i])
	        );
	
	        // Big enough so we get horizontal scroll on narrow windows,
	        // but Expand will stretch them nicely on wide windows.
	        c.SetMinSize(Size(DPI(260), DPI(120)));
	
	        responsiveArea.AddExpand(c, weights[i]);
	    }
	
	    responsiveArea.Layout();
	}


};

GUI_APP_MAIN
{
    DemoApp().Run();
}