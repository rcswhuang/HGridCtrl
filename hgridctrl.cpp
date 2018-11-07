﻿// GridCtrl.cpp : implementation file
//
// MFC Grid Control v2.24
//
//
/////////////////////////////////////////////////////////////////////////////

#include "hgridctrl.h"
uint getMouseScrollLines()
{
    uint nScrollLines = 3;            // reasonable default
/*#ifdef WIN32 //如果Windows系统就用API函数，其他系统默认
    SystemParametersInfo (SPI_GETWHEELSCROLLLINES, 0, &nScrollLines, 0);
#endif*/
    return nScrollLines;
}

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl
HGridCtrl::HGridCtrl(int nRows, int nCols, int nFixedRows, int nFixedCols,QWidget *parent)
    :QAbstractScrollArea(parent)
{
    // Store the system colours in case they change. The gridctrl uses
    // these colours, and in OnSysColorChange we can check to see if
    // the gridctrl colours have been changed from the system colours.
    // If they have, then leave them, otherwise change them to reflect
    // the new system colours.
    QWidget* widget = new QWidget;
    setViewport(widget); 
    setMouseTracking(true);

    m_crWindowText        = QColor(QCOLOR_WINDOWTEXT);
    m_crWindowColour      = QColor(QCOLOR_WINDOW);
    m_cr3DFace            = QColor(QCOLOR_3DFACE);
    m_crShadow            = QColor(QCOLOR_3DSHADOW);
    m_crGridLineColour    = QColor(192,192,192);

    m_nRows               = 0;
    m_nCols               = 0;
    m_nFixedRows          = 0;
    m_nFixedCols          = 0;

    m_bVirtualMode        = false;
    //m_pfnCallback         = NULL;

    m_nVScrollMax         = 0;          // Scroll position
    m_nHScrollMax         = 0;
    m_nRowsPerWheelNotch  = getMouseScrollLines(); // Get the number of lines
                                                   // per mouse wheel notch to scroll
    m_nBarState           = GVL_NONE;
    m_MouseMode           = MOUSE_NOTHING;
    m_nGridLines          = GVL_BOTH;
    m_bEditable           = true;
    m_bListMode           = false;
    m_bSingleRowSelection = false;
    m_bSingleColSelection = false;
    m_bLMouseButtonDown   = false;
    m_bRMouseButtonDown   = false;
    m_bAllowDraw          = true;       // allow draw updates
    m_bEnableSelection    = true;
    m_bFixedColumnSelection = true;
    m_bFixedRowSelection  = true;
    m_bAllowRowResize     = true;
    m_bAllowColumnResize  = true;
    m_bSortOnClick        = false;      // Sort on header row click
    m_bHandleTabKey       = true;
    m_bDoubleBuffer       = false;
    /*
#ifdef _WIN32_WCE
    m_bDoubleBuffer       = false;      // Use double buffering to avoid flicker?
#else
    m_bDoubleBuffer       = true;       // Use double buffering to avoid flicker?
#endif*/

    m_bTitleTips          = true;       // show cell title tips

    m_bWysiwygPrinting    = false;      // use size-to-width printing

    m_bHiddenColUnhide    = true;       // 0-width columns can be expanded via mouse
    m_bHiddenRowUnhide    = true;       // 0-Height rows can be expanded via mouse

    m_bAllowColHide       = true;       // Columns can be contracted to 0-width via mouse
    m_bAllowRowHide       = true;       // Rows can be contracted to 0-height via mouse

    m_bAscending          = true;       // sorting stuff
    m_nSortColumn         = -1;
	m_pfnCompare		  = NULL;

    m_nAutoSizeColumnStyle = GVS_BOTH;  // Autosize grid using header and data info

    m_nTimerID            = 0;          // For drag-selection
    m_nTimerInterval      = 25;         // (in milliseconds)
    m_nResizeCaptureRange = 3;          // When resizing columns/row, the cursor has to be
                                        // within +/-3 pixels of the dividing line for
                                        // resizing to be possible
    m_pImageList          = NULL;       // Images in the grid
    m_bAllowDragAndDrop   = false;      // for drag and drop - EFW - off by default
    m_bTrackFocusCell     = true;       // Track Focus cell?
    m_bFrameFocus         = true;       // Frame the selected cell?
    m_bMulSelection       = false;

    //m_pRtcDefault = RUNTIME_CLASS(CGridCell);

    setupDefaultCells();
    setGridBkColor(m_crShadow);

    //初始化单元格行列
    setRowCount(nRows);
    setColumnCount(nCols);
    setFixedRowCount(nFixedRows);
    setFixedColumnCount(nFixedCols);

    setTitleTipTextClr(QColor(QCLR_DEFAULT));  //FNA
    setTitleTipBackClr(QColor(QCLR_DEFAULT));

    // set initial selection range (ie. none)
    m_SelectedCellMap.clear();
    m_PrevSelectedCellMap.clear();

    /*
#if !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)
    // EFW - Added to support shaded/unshaded printout and
    // user-definable margins.
    m_bShadedPrintOut = true;
    setPrintMarginInfo(2, 2, 4, 4, 1, 1, 1);
#endif*/

    //try
    {
        m_arRowHeights.resize(m_nRows);    // initialize row heights
        m_arColWidths.resize(m_nCols);     // initialize column widths
    }
    //catch (exception &e)
    {
     //   e.what();
      //  return;
    }

    int i;
    for (i = 0; i < m_nRows; i++)
        m_arRowHeights[i] = m_cellDefault.height();
    for (i = 0; i < m_nCols; i++)
        m_arColWidths[i] = m_cellDefault.width();

    connect(horizontalScrollBar(),&QScrollBar::valueChanged,this,&HGridCtrl::onHorizontalScrollBarChanged);
    connect(verticalScrollBar(),&QScrollBar::valueChanged,this,&HGridCtrl::onVerticalScrollBarChanged);
}

//no
HGridCtrl::~HGridCtrl()
{
    //deleteAllItems();

    /*
#ifndef GRIDCONTROL_NO_TITLETIPS
    if (m_bTitleTips && ::IsWindow(m_TitleTip.GetSafeHwnd())) 
        m_TitleTip.DestroyWindow();
#endif

    DestroyWindow();

#if !defined(GRIDCONTROL_NO_DRAGDROP) || !defined(QT_NO_CLIPBOARD)
    // BUG FIX - EFW
    COleDataSource *pSource = COleDataSource::GetClipboardOwner();
    if(pSource)
        COleDataSource::FlushClipboard();
#endif
*/
}

//ok
void HGridCtrl::setupDefaultCells()
{
    m_cellDefault.setGrid(this);            // Normal editable cell
    m_cellFixedColDef.setGrid(this);        // Cell for fixed columns
    m_cellFixedRowDef.setGrid(this);        // Cell for fixed rows
    m_cellFixedRowColDef.setGrid(this);     // Cell for area overlapped by fixed columns/rows

    m_cellDefault.setTextClr(m_crWindowText);
    m_cellDefault.setBackClr(m_crWindowColour);
    m_cellFixedColDef.setTextClr(m_crWindowText);
    m_cellFixedColDef.setBackClr(m_cr3DFace);
    m_cellFixedRowDef.setTextClr(m_crWindowText);
    m_cellFixedRowDef.setBackClr(m_cr3DFace);
    m_cellFixedRowColDef.setTextClr(m_crWindowText);
    m_cellFixedRowColDef.setBackClr(m_cr3DFace);
}


/////////////////////////////////////////////////////////////////////////////
// CGridCtrl message handlers

void HGridCtrl::paintEvent(QPaintEvent* event)
{
    QPainter painter(viewport());      // device context for painting
    m_painter = &painter;

    if (m_bDoubleBuffer)    // Use a memory DC to remove flicker
    {
        //CMemDC MemDC(&dc);
        //OnDraw(&MemDC);
    }
    else                    // Draw raw - this helps in debugging vis problems.
        onDraw(&painter);
}



// Custom background erasure. This gets called from within the OnDraw function,
// since we will (most likely) be using a memory DC to stop flicker. If we just
// erase the background normally through OnEraseBkgnd, and didn't fill the memDC's
// selected bitmap with colour, then all sorts of vis problems would occur
void HGridCtrl::eraseBkgnd(QPainter* pDC)
{
    if(NULL == pDC)
        return;
    QRect  VisRect, ClipRect, rect;
    QBrush FixedRowColBack(defaultCell(true, true)->backClr()),
           FixedRowBack(defaultCell(true, false)->backClr()),
           FixedColBack(defaultCell(false, true)->backClr()),
           TextBack(defaultCell(false, false)->backClr());
    QBrush Back(gridBkColor());
    //CBrush Back(GetTextBkColor());

    if (NULL == viewport())
        return;
    ClipRect = viewport()->rect();
    visibleNonFixedCellRange(VisRect);

    int nFixedColumnWidth = fixedColumnWidth();
    int nFixedRowHeight = fixedRowHeight();

    // Draw Fixed row/column background
    if (ClipRect.left() < nFixedColumnWidth && ClipRect.top() < nFixedRowHeight)
        pDC->fillRect(QRect(ClipRect.left(), ClipRect.top(),
                      nFixedColumnWidth, nFixedRowHeight),
                      FixedRowColBack);

    // Draw Fixed columns background
    if (ClipRect.left() < nFixedColumnWidth && ClipRect.top() < VisRect.bottom())
        pDC->fillRect(QRect(ClipRect.left(), ClipRect.top(),
                      nFixedColumnWidth, VisRect.bottom()),
                      FixedColBack);
        
    // Draw Fixed rows background
    if (ClipRect.top() < nFixedRowHeight &&
        ClipRect.right() > nFixedColumnWidth && ClipRect.left() < VisRect.right())
        pDC->fillRect(QRect(nFixedColumnWidth-1, ClipRect.top(),
                      VisRect.right(), nFixedRowHeight),
                      FixedRowBack);

    // Draw non-fixed cell background
    if ((rect = VisRect.intersected(ClipRect)).isValid())
    {
        QRect CellRect(max(nFixedColumnWidth, rect.left()),
                       max(nFixedRowHeight, rect.top()),
                       rect.right(), rect.bottom());
        pDC->fillRect(CellRect, TextBack);
    }

    //note:注意bottom+1,right+1,如果不+1,右下有两条白线
    // Draw right hand side of window outside grid
    if (VisRect.right() < ClipRect.right())
        pDC->fillRect(QRect(VisRect.right(), ClipRect.top(),
                      ClipRect.right(), ClipRect.bottom()+1),
                      Back);

    // Draw bottom of window below grid
    if (VisRect.bottom() < ClipRect.bottom() && ClipRect.left() < VisRect.right())
        pDC->fillRect(QRect(ClipRect.left(), VisRect.bottom(),
                      VisRect.right()+1, ClipRect.bottom()),
                      Back);

}

HCellID HGridCtrl::setFocusCell(HCellID cell)
{
    if (cell == m_idCurrentCell)
        return m_idCurrentCell;

    HCellID idPrev = m_idCurrentCell;

    // EFW - Bug Fix - Force focus to be in a non-fixed cell
    if (cell.row != -1 && cell.row < fixedRowCount())
        cell.row = fixedRowCount();
    if (cell.col != -1 && cell.col < fixedColumnCount())
        cell.col = fixedColumnCount();

    m_idCurrentCell = cell;


    if (isValid(idPrev))
    {
        setItemState(idPrev.row, idPrev.col,
            itemState(idPrev.row, idPrev.col) & ~GVIS_FOCUSED);
        redrawCell(idPrev); // comment to reduce flicker

        if (isTrackFocusCell() && idPrev.col != m_idCurrentCell.col)
            for (int row = 0; row < m_nFixedRows; row++)
                redrawCell(row, idPrev.col);
        if (isTrackFocusCell() && idPrev.row != m_idCurrentCell.row)
            for (int col = 0; col < m_nFixedCols; col++)
                redrawCell(idPrev.row, col);
    }

    if (isValid(m_idCurrentCell))
    {
        setItemState(m_idCurrentCell.row, m_idCurrentCell.col,
            itemState(m_idCurrentCell.row, m_idCurrentCell.col) | GVIS_FOCUSED);

        /*redrawCell(m_idCurrentCell); // comment to reduce flicker

        if (isTrackFocusCell() && idPrev.col != m_idCurrentCell.col)
            for (int row = 0; row < m_nFixedRows; row++)
                redrawCell(row, m_idCurrentCell.col);
        if (isTrackFocusCell() && idPrev.row != m_idCurrentCell.row)
            for (int col = 0; col < m_nFixedCols; col++)
                redrawCell(m_idCurrentCell.row, col);*/

        // EFW - New addition.  If in list mode, make sure the selected
        // row highlight follows the cursor.
        // Removed by C Maunder 27 May
        //if (m_bListMode)
        //{
        //    m_PrevSelectedCellMap.RemoveAll();
        //    m_MouseMode = MOUSE_SELECT_ROW;
        //    onSelecting(m_idCurrentCell);

            // Leave this off so that you can still drag the highlight around
            // without selecting rows.
            // m_MouseMode = MOUSE_NOTHING;
        //}

    }

    return idPrev;
}

// Sets the currently selected cell, returning the previous current cell
HCellID HGridCtrl::setFocusCell(int nRow, int nCol)
{
    return setFocusCell(HCellID(nRow, nCol));
}

/*
void HGridCtrl::OnSize(UINT nType, int cx, int cy)
{  
    static bool bAlreadyInsideThisProcedure = false;
    if (bAlreadyInsideThisProcedure)
        return;

    if (!::IsWindow(m_hWnd))
        return;

	// This is not the ideal place to register the droptarget
#ifndef GRIDCONTROL_NO_DRAGDROP
	m_DropTarget.Register(this);
#endif

    // Start re-entry blocking
    bAlreadyInsideThisProcedure = true;

    EndEditing();        // destroy any InPlaceEdit's
    CWnd::OnSize(nType, cx, cy);
    ResetScrollBars();

    // End re-entry blocking
    bAlreadyInsideThisProcedure = false;
}

UINT HGridCtrl::OnGetDlgCode()
{
    UINT nCode = DLGC_WANTARROWS | DLGC_WANTCHARS; // DLGC_WANTALLKEYS; //

    if (m_bHandleTabKey && !IsCTRLpressed())
        nCode |= DLGC_WANTTAB;

    return nCode;
}

#ifndef _WIN32_WCE
// If system colours change, then redo colours
void HGridCtrl::OnSysColorChange()
{
    CWnd::OnSysColorChange();

    if (GetDefaultCell(false, false)->GetTextClr() == m_crWindowText)                   // Still using system colours
        GetDefaultCell(false, false)->SetTextClr(::GetSysColor(COLOR_WINDOWTEXT));      // set to new system colour
    if (GetDefaultCell(false, false)->GetBackClr() == m_crWindowColour)
        GetDefaultCell(false, false)->SetBackClr(::GetSysColor(COLOR_WINDOW));

    if (GetDefaultCell(true, false)->GetTextClr() == m_crWindowText)                   // Still using system colours
        GetDefaultCell(true, false)->SetTextClr(::GetSysColor(COLOR_WINDOWTEXT));      // set to new system colour
    if (GetDefaultCell(true, false)->GetBackClr() == m_crWindowColour)
        GetDefaultCell(true, false)->SetBackClr(::GetSysColor(COLOR_WINDOW));

    if (GetDefaultCell(false, true)->GetTextClr() == m_crWindowText)                   // Still using system colours
        GetDefaultCell(false, true)->SetTextClr(::GetSysColor(COLOR_WINDOWTEXT));      // set to new system colour
    if (GetDefaultCell(false, true)->GetBackClr() == m_crWindowColour)
        GetDefaultCell(false, true)->SetBackClr(::GetSysColor(COLOR_WINDOW));

    if (GetDefaultCell(true, true)->GetTextClr() == m_crWindowText)                   // Still using system colours
        GetDefaultCell(true, true)->SetTextClr(::GetSysColor(COLOR_WINDOWTEXT));      // set to new system colour
    if (GetDefaultCell(true, true)->GetBackClr() == m_crWindowColour)
        GetDefaultCell(true, true)->SetBackClr(::GetSysColor(COLOR_WINDOW));

    if (GetGridBkColor() == m_crShadow)
        SetGridBkColor(::GetSysColor(COLOR_3DSHADOW));

    m_crWindowText   = ::GetSysColor(COLOR_WINDOWTEXT);
    m_crWindowColour = ::GetSysColor(COLOR_WINDOW);
    m_cr3DFace       = ::GetSysColor(COLOR_3DFACE);
    m_crShadow       = ::GetSysColor(COLOR_3DSHADOW);
}
#endif

#ifndef _WIN32_WCE_NO_CURSOR
// If we are drag-selecting cells, or drag and dropping, stop now
void HGridCtrl::OnCaptureChanged(CWnd *pWnd)
{
    if (pWnd->GetSafeHwnd() == GetSafeHwnd())
        return;

    // kill timer if active
    if (m_nTimerID != 0)
    {
        KillTimer(m_nTimerID);
        m_nTimerID = 0;
    }

#ifndef GRIDCONTROL_NO_DRAGDROP
    // Kill drag and drop if active
    if (m_MouseMode == MOUSE_DRAGGING)
        m_MouseMode = MOUSE_NOTHING;
#endif
}
#endif

#if (_MFC_VER >= 0x0421) || (_WIN32_WCE >= 210)
// If system settings change, then redo colours
void HGridCtrl::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
    CWnd::OnSettingChange(uFlags, lpszSection);

    if (GetDefaultCell(false, false)->GetTextClr() == m_crWindowText)                   // Still using system colours
        GetDefaultCell(false, false)->SetTextClr(::GetSysColor(COLOR_WINDOWTEXT));      // set to new system colour
    if (GetDefaultCell(false, false)->GetBackClr() == m_crWindowColour)
        GetDefaultCell(false, false)->SetBackClr(::GetSysColor(COLOR_WINDOW));

    if (GetDefaultCell(true, false)->GetTextClr() == m_crWindowText)                   // Still using system colours
        GetDefaultCell(true, false)->SetTextClr(::GetSysColor(COLOR_WINDOWTEXT));      // set to new system colour
    if (GetDefaultCell(true, false)->GetBackClr() == m_crWindowColour)
        GetDefaultCell(true, false)->SetBackClr(::GetSysColor(COLOR_WINDOW));

    if (GetDefaultCell(false, true)->GetTextClr() == m_crWindowText)                   // Still using system colours
        GetDefaultCell(false, true)->SetTextClr(::GetSysColor(COLOR_WINDOWTEXT));      // set to new system colour
    if (GetDefaultCell(false, true)->GetBackClr() == m_crWindowColour)
        GetDefaultCell(false, true)->SetBackClr(::GetSysColor(COLOR_WINDOW));

    if (GetDefaultCell(true, true)->GetTextClr() == m_crWindowText)                   // Still using system colours
        GetDefaultCell(true, true)->SetTextClr(::GetSysColor(COLOR_WINDOWTEXT));      // set to new system colour
    if (GetDefaultCell(true, true)->GetBackClr() == m_crWindowColour)
        GetDefaultCell(true, true)->SetBackClr(::GetSysColor(COLOR_WINDOW));

    if (GetGridBkColor() == m_crShadow)
        SetGridBkColor(::GetSysColor(COLOR_3DSHADOW));

    m_crWindowText   = ::GetSysColor(COLOR_WINDOWTEXT);
    m_crWindowColour = ::GetSysColor(COLOR_WINDOW);
    m_cr3DFace       = ::GetSysColor(COLOR_3DFACE);
    m_crShadow       = ::GetSysColor(COLOR_3DSHADOW);

    m_nRowsPerWheelNotch = GetMouseScrollLines(); // Get the number of lines
}
#endif

// For drag-selection. Scrolls hidden cells into view
// TODO: decrease timer interval over time to speed up selection over time
void HGridCtrl::OnTimer(UINT nIDEvent)
{
    Q_ASSERT(nIDEvent == WM_LBUTTONDOWN);
    if (nIDEvent != WM_LBUTTONDOWN)
        return;

    CPoint pt, origPt;

#ifdef _WIN32_WCE
    if (m_MouseMode == MOUSE_NOTHING)
        return;
    origPt = GetMessagePos();
#else
    if (!GetCursorPos(&origPt))
        return;
#endif

    ScreenToClient(&origPt);

    CRect rect;
    GetClientRect(rect);

    int nFixedRowHeight = GetFixedRowHeight();
    int nFixedColWidth = GetFixedColumnWidth();

    pt = origPt;
    if (pt.y > rect.bottom)
    {
        //SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);
        SendMessage(WM_KEYDOWN, VK_DOWN, 0);

        if (pt.x < rect.left)
            pt.x = rect.left;
        if (pt.x > rect.right)
            pt.x = rect.right;
        pt.y = rect.bottom;
        onSelecting(cellFromPt(pt));
    }
    else if (pt.y < nFixedRowHeight)
    {
        //SendMessage(WM_VSCROLL, SB_LINEUP, 0);
        SendMessage(WM_KEYDOWN, VK_UP, 0);

        if (pt.x < rect.left)
            pt.x = rect.left;
        if (pt.x > rect.right)
            pt.x = rect.right;
        pt.y = nFixedRowHeight + 1;
        onSelecting(cellFromPt(pt));
    }

    pt = origPt;
    if (pt.x > rect.right)
    {
        // SendMessage(WM_HSCROLL, SB_LINERIGHT, 0);
        SendMessage(WM_KEYDOWN, VK_RIGHT, 0);

        if (pt.y < rect.top)
            pt.y = rect.top;
        if (pt.y > rect.bottom)
            pt.y = rect.bottom;
        pt.x = rect.right;
        onSelecting(cellFromPt(pt));
    }
    else if (pt.x < nFixedColWidth)
    {
        //SendMessage(WM_HSCROLL, SB_LINELEFT, 0);
        SendMessage(WM_KEYDOWN, VK_LEFT, 0);

        if (pt.y < rect.top)
            pt.y = rect.top;
        if (pt.y > rect.bottom)
            pt.y = rect.bottom;
        pt.x = nFixedColWidth + 1;
        onSelecting(cellFromPt(pt));
    }
}
*/
// move about with keyboard
void HGridCtrl::keyReleaseEvent(QKeyEvent *event)
{
/*
 * Qt滚动条是有相关自动实现的部分，但除了这部分其他的如ctrl键等是没有的需要自己修改
注意对于里面特殊的按键处理没有调用QWidget类的keyReleaseEvent来处理
而是通过自己调整滚动工作条即可。
*/
    if (!isValid(m_idCurrentCell))
    {
        QWidget::keyReleaseEvent(event);
        return;
    }

    HCellID next = m_idCurrentCell;
    bool bChangeLine = false;
    bool bHorzScrollAction = false;
    bool bVertScrollAction = false;

    if (Qt::CTRL == event->modifiers())
    {
        switch (event->key())
        {
        case Qt::Key_A:
            onEditSelectAll();
            break;
#ifndef QT_NO_CLIPBOARD
        case Qt::Key_X:
            onEditCut();
            break;
        case Qt::Key_C:
            onEditCopy();
            break;
        case Qt::Key_V:
            onEditPaste();
            break;
#endif
        }
    }

    bool bFoundVisible;
    int iOrig;

    if (Qt::Key_Delete == event->key())
    {
        cutSelectedText();
    }
    else if (Qt::Key_Down == event->key())
    {
        // don't let user go to a hidden row
        bFoundVisible = false;
        iOrig = next.row;
        next.row++;
        //不能移动到隐藏行列中
        while( next.row < rowCount())
        {
            if( rowHeight( next.row) > 0)
            {
                bFoundVisible = true;
                break;
            }
            next.row++;
        }
        if( !bFoundVisible)
            next.row = iOrig;
    }
    else if (Qt::Key_Up == event->key())
    {
        // don't let user go to a hidden row
        bFoundVisible = false;
        iOrig = next.row;
        next.row--;
        while( next.row >= m_nFixedRows)
        {
            if( rowHeight( next.row) > 0)
            {
                bFoundVisible = true;
                break;
            }
            next.row--;
        }
        if( !bFoundVisible)
            next.row = iOrig;
    }
    else if (event->key() == Qt::Key_Right || (event->key() == Qt::Key_Tab && !(Qt::SHIFT == event->modifiers())) )
    {
        // don't let user go to a hidden column
        bFoundVisible = false;
        iOrig = next.col;
        next.col++;

        if (Qt::Key_Tab == event->key())
        {
			// If we're at the end of a row, go down a row till we find a non-hidden row
            if (next.col == (columnCount()) && next.row < (rowCount() - 1))
            {
				next.row++;
                while( next.row < rowCount())
				{
                    if( rowHeight(next.row) > 0)
					{
                        bFoundVisible = true;
						break;
					}
					next.row++; 
				}

                next.col = m_nFixedCols;	// Place focus on first non-fixed column
                bChangeLine = true;
            }
            else
                QWidget::keyReleaseEvent(event);
        }

		// We're on a non-hidden row, so look across for the next non-hidden column
        while( next.col < columnCount())
        {
            if( columnWidth( next.col) > 0)
            {
                bFoundVisible = true;
                break;
            }
            next.col++;
        }

		// If nothing worked then don't bother
        if( !bFoundVisible)
            next.col = iOrig;
    }
    else if (Qt::Key_Left == event->key() || (event->key() == Qt::Key_Tab && (Qt::SHIFT == event->modifiers())) )
    {
        // don't let user go to a hidden column
        bFoundVisible = false;
        iOrig = next.col;
        next.col--;

        if (Qt::Key_Tab == event->key())
        {
            if (next.col == (fixedColumnCount()-1) && next.row > fixedRowCount())
            {
                next.row--;
                while( next.row > fixedRowCount())
				{
                    if( rowHeight(next.row) > 0)
					{
                        bFoundVisible = true;
						break;
					}
					next.row--; 
				}

                next.col = columnCount() - 1;
                bChangeLine = true;
            }
            else
                QWidget::keyReleaseEvent(event);
        }

        while( next.col >= m_nFixedCols)
        {
            if( columnWidth( next.col) > 0)
            {
                bFoundVisible = true;
                break;
            }
            next.col--;
        }
        if( !bFoundVisible)
            next.col = iOrig;
    }
    else if (Qt::Key_PageDown == event->key())
    {
        HCellID idOldTopLeft = topleftNonFixedCell();//huangw
        setScrollBarValue(QWM_VSCROLL,QSB_PAGEDOWN,0);
        bVertScrollAction = true;
        HCellID idNewTopLeft = topleftNonFixedCell();

        int increment = idNewTopLeft.row - idOldTopLeft.row;
        if (increment)
        {
            next.row += increment;
            if (next.row >(rowCount() - 1))
                next.row = rowCount() - 1;
        }
        else
            next.row = rowCount() - 1;
    }
    else if (Qt::Key_PageUp == event->key())
    {
        HCellID idOldTopLeft = topleftNonFixedCell();
        setScrollBarValue(QWM_VSCROLL,QSB_PAGEUP,0);
        bVertScrollAction = true;
        HCellID idNewTopLeft = topleftNonFixedCell();
            
        int increment = idNewTopLeft.row - idOldTopLeft.row;
        if (increment) 
        {
            next.row += increment;
            if (next.row < m_nFixedRows) 
                next.row = m_nFixedRows;
        }
        else
            next.row = m_nFixedRows;
    }
    else if (Qt::Key_Home == event->key())
    {
        // Home and Ctrl-Home work more like Excel
        //  and don't let user go to a hidden cell
        if (Qt::SHIFT == event->modifiers())
        {
            //调整滚动条的位置
            setScrollBarValue(QWM_VSCROLL,QSB_TOP,0);
            setScrollBarValue(QWM_HSCROLL,QSB_LEFT,0);
            bVertScrollAction = true;
            bHorzScrollAction = true;
            next.row = m_nFixedRows;
            next.col = m_nFixedCols;
        }
        else
        {
            setScrollBarValue(QWM_HSCROLL,QSB_LEFT,0);
            bHorzScrollAction = true;
            next.col = m_nFixedCols;
        }
        // adjust column to avoid hidden columns and rows
        while( next.col < columnCount() - 1)
        {
            if( columnWidth( next.col) > 0)
                break;
            next.col++;
        }
        while( next.row < rowCount() - 1)
        {
            if( rowHeight( next.row) > 0)
                break;
            next.row++;
        }
    }
    else if (Qt::Key_End == event->key())
    {
        // End and Ctrl-End work more like Excel
        //  and don't let user go to a hidden cell
        if (Qt::CTRL == event->modifiers())
        {
            setScrollBarValue(QWM_VSCROLL,QSB_BOTTOM,0);
            setScrollBarValue(QWM_HSCROLL,QSB_RIGHT,0);
            bHorzScrollAction = true;
            bVertScrollAction = true;
            next.row = rowCount() - 1;
            next.col = columnCount() - 1;
        }
        else
        {
            setScrollBarValue(QWM_HSCROLL,QSB_RIGHT,0);
            bHorzScrollAction = true;
            next.col = columnCount() - 1;
        }
        // adjust column to avoid hidden columns and rows
        while( next.col > m_nFixedCols + 1)
        {
            if( columnWidth( next.col) > 0)
                break;
            next.col--;
        }
        while( next.row > m_nFixedRows + 1)
        {
            if( rowHeight( next.row) > 0)
                break;
            next.row--;
        }
    }
    else if (Qt::Key_F2 == event->key())
    {
        onEditCell(m_idCurrentCell.row, m_idCurrentCell.col, QPoint( -1, -1));
    }
    else
    {
        QAbstractScrollArea::keyReleaseEvent(event);
        return;
    }

    if (next != m_idCurrentCell)
    {
        // While moving with the Cursorkeys the current ROW/CELL will get selected
        // OR Selection will get expanded when SHIFT is pressed
        // Cut n paste from OnLButtonDown - Franco Bez
        // Added check for NULL mouse mode - Chris Maunder.
        if (m_MouseMode == MOUSE_NOTHING)
        {
            m_PrevSelectedCellMap.clear();
            m_MouseMode = m_bListMode? MOUSE_SELECT_ROW : MOUSE_SELECT_CELLS;
            if (!(Qt::SHIFT == event->modifiers()) || Qt::Key_Tab == event->key())
                m_SelectionStartCell = next;

			// Notify parent that selection is changing - Arthur Westerman/Scot Brennecke 
            //huangw
            //SendMessageToParent(next.row, next.col, GVN_SELCHANGING);
            onSelecting(next);
            //SendMessageToParent(next.row, next.col, GVN_SELCHANGED);


            m_MouseMode = MOUSE_NOTHING;
        }

        setFocusCell(next);

        if (!isCellVisible(next))
        {

            switch (event->key())
            {
            case Qt::Key_Right:
                setScrollBarValue(QWM_HSCROLL,QSB_LINERIGHT,0);
                bHorzScrollAction = true;
                break;
                
            case Qt::Key_Left:

                setScrollBarValue(QWM_HSCROLL,QSB_LINELEFT,0);
                bHorzScrollAction = true;
                break;
                
            case Qt::Key_Down:
                setScrollBarValue(QWM_VSCROLL,QSB_LINEDOWN,0);
                bVertScrollAction = true;
                break;
                
            case Qt::Key_Up:
                setScrollBarValue(QWM_VSCROLL,QSB_LINEUP,0);
                bVertScrollAction = true;
                break;                
                
            case Qt::Key_Tab:
                if (Qt::SHIFT == event->modifiers())
                {
                    if (bChangeLine) 
                    {
                        setScrollBarValue(QWM_VSCROLL,QSB_LINEUP,0);
                        bVertScrollAction = true;
                        setScrollPos32(QSB_HORZ, m_nHScrollMax);
                        break;
                    }
                    else 
					{
                        setScrollBarValue(QWM_HSCROLL,QSB_LINELEFT,0);
                        bHorzScrollAction = true;
					}
                }
                else
                {
                    if (bChangeLine) 
                    {
                        setScrollBarValue(QWM_VSCROLL,QSB_LINEDOWN,0);
                        bVertScrollAction = true;
                        setScrollPos32(QSB_HORZ, 0);
                        break;
                    }
                    else 
					{
                        setScrollBarValue(QWM_HSCROLL,QSB_LINERIGHT,0);
                        bHorzScrollAction = true;
					}
                }
                break;
            }
            //EnsureVisible(next); // Make sure cell is visible  --huangw
            update();
        }
        //EnsureVisible(next); // Make sure cell is visible --huangw

		if (bHorzScrollAction)
        {
            setScrollBarValue(QWM_HSCROLL,QSB_ENDSCROLL,0);
        }
		if (bVertScrollAction)
        {
            setScrollBarValue(QWM_VSCROLL,QSB_ENDSCROLL,0);
        }
    }
}

void HGridCtrl::keyPressEvent(QKeyEvent* event)
{
    QAbstractScrollArea::keyPressEvent(event);
}

/*
void HGridCtrl::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
#ifdef GRIDCONTROL_USE_TITLETIPS
    m_TitleTip.Hide();  // hide any titletips
#endif

    CWnd::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

// Instant editing of cells when keys are pressed
void HGridCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // EFW - BUG FIX
    if (!IsCTRLpressed() && m_MouseMode == MOUSE_NOTHING && nChar != VK_ESCAPE)
    {
        if (!m_bHandleTabKey || (m_bHandleTabKey && nChar != VK_TAB))
            OnEditCell(m_idCurrentCell.row, m_idCurrentCell.col, CPoint( -1, -1), nChar);
    }

    CWnd::OnChar(nChar, nRepCnt, nFlags);
}

// Added by KiteFly
LRESULT HGridCtrl::OnImeChar(WPARAM wCharCode, LPARAM)
{
    // EFW - BUG FIX
    if (!IsCTRLpressed() && m_MouseMode == MOUSE_NOTHING && wCharCode != VK_ESCAPE) 
        OnEditCell(m_idCurrentCell.row, m_idCurrentCell.col, CPoint( -1, -1), wCharCode);
    return 0;
}

// Callback from any CInPlaceEdits that ended. This just calls OnEndEditCell,
// refreshes the edited cell and moves onto next cell if the return character
// from the edit says we should.
void HGridCtrl::OnEndInPlaceEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
    GV_DISPINFO *pgvDispInfo = (GV_DISPINFO *)pNMHDR;
    GV_ITEM     *pgvItem = &pgvDispInfo->item;

    // In case OnEndInPlaceEdit called as window is being destroyed
    if (!IsWindow(GetSafeHwnd()))
        return;

    OnEndEditCell(pgvItem->row, pgvItem->col, pgvItem->strText);
    //invalidateCellRect(HCellID(pgvItem->row, pgvItem->col));

    switch (pgvItem->lParam)
    {
    case VK_TAB:
    case VK_DOWN:
    case VK_UP:
    case VK_RIGHT:
    case VK_LEFT:
    case VK_NEXT:
    case VK_PRIOR:
    case VK_HOME:
    case VK_END:
        OnKeyDown(pgvItem->lParam, 0, 0);
        OnEditCell(m_idCurrentCell.row, m_idCurrentCell.col, CPoint( -1, -1), pgvItem->lParam);
    }

    *pResult = 0;
}
*/
// Handle horz scrollbar notifications 水平滚动条
void HGridCtrl::onHScroll(uint nSBCode, uint nPos, QScrollBar* pScrollBar)
{

    endEditing();//huangw

    int scrollPos = scrollPos32(QSB_HORZ);

    HCellID idTopLeft = topleftNonFixedCell();

    QRect rect;
    rect = viewport()->rect();

    switch (nSBCode)
    {
    case QSB_LINERIGHT: //向右滚动一行
        if (scrollPos < m_nHScrollMax)
        {
            // may have contiguous hidden columns.  Blow by them
            while (idTopLeft.col < (columnCount()-1)
                    && columnWidth( idTopLeft.col) < 1 )
            {
                idTopLeft.col++;
            }
            int xScroll = columnWidth(idTopLeft.col);
            setScrollPos32(QSB_HORZ, scrollPos + xScroll);//就是设置滚动条的位置
            if (scrollPos32(QSB_HORZ) == scrollPos)
                break;          // didn't work

            //rect.setLeft(fixedColumnWidth());
            //update();
        }
        break;

    case QSB_LINELEFT: //向左滚动一行
        if (scrollPos > 0 && idTopLeft.col > fixedColumnCount())
        {
            int iColToUse = idTopLeft.col-1;
            // may have contiguous hidden columns.  Blow by them
            while(  iColToUse > fixedColumnCount()
                    && columnWidth(iColToUse) < 1 )
            {
                iColToUse--;
            }

            int xScroll = columnWidth(iColToUse);
            setScrollPos32(QSB_HORZ, max(0, scrollPos - xScroll));
            //rect.setLeft(fixedColumnWidth());
            //update();
        }
        break;

    case QSB_PAGERIGHT: //向右滚动一页
        if (scrollPos < m_nHScrollMax)
        {
            rect.setLeft(fixedColumnWidth());
            int offset = rect.width();
            int pos = min(m_nHScrollMax, scrollPos + offset);
            setScrollPos32(QSB_HORZ, pos);
            //rect.setLeft(fixedColumnWidth());
            //update();
        }
        break;
        
    case QSB_PAGELEFT: //向左滚动一页
        if (scrollPos > 0)
        {
            rect.setLeft(fixedColumnWidth());
            int offset = -rect.width();
            int pos = max(0, scrollPos + offset);
            setScrollPos32(QSB_HORZ, pos);
            //rect.setLeft(fixedColumnWidth());
            //update();
        }
        break;
        
    case QSB_THUMBPOSITION://滚动框到指定位置
    case QSB_THUMBTRACK: //滚动框被拖动
        {
            setScrollPos32(QSB_HORZ, scrollPos32(QSB_HORZ, true));
            m_idTopLeftCell.row = -1;
            HCellID idNewTopLeft = topleftNonFixedCell();
            if (idNewTopLeft != idTopLeft)
            {
                rect.setLeft(fixedColumnWidth());
                update();
            }
        }
        break;
        
    case QSB_LEFT://到左端
        if (scrollPos > 0)
        {
            setScrollPos32(QSB_HORZ, 0);
            //update();
        }
        break;
        
    case QSB_RIGHT: //到右端
        if (scrollPos < m_nHScrollMax)
        {
            setScrollPos32(QSB_HORZ, m_nHScrollMax);
            //update();
        }
        break;

    default: 
        break;
    }
}

// Handle vert scrollbar notifications
void HGridCtrl::onVScroll(uint nSBCode, uint nPos, QScrollBar* pScrollBar)
{

    endEditing();
    // Get the scroll position ourselves to ensure we get a 32 bit value
    int scrollPos = scrollPos32(QSB_VERT);

    HCellID idTopLeft = topleftNonFixedCell();

    QRect rect;
    //GetClientRect(rect);

    switch (nSBCode)
    {
    case QSB_LINEDOWN: //向下一行
        if (scrollPos < m_nVScrollMax)
        {
            // may have contiguous hidden rows.  Blow by them
            while(  idTopLeft.row < (rowCount()-1)
                    && rowHeight( idTopLeft.row) < 1 )
            {
                idTopLeft.row++;
            }

            int yScroll = rowHeight(idTopLeft.row);
            setScrollPos32(QSB_VERT, scrollPos + yScroll);
            if (scrollPos32(QSB_VERT) == scrollPos)
                break;          // didn't work

            rect.setTop(fixedRowHeight());
            update();
        }
        break;
        
    case QSB_LINEUP: //向上一行
        if (scrollPos > 0 && idTopLeft.row > fixedRowCount())
        {
            int iRowToUse = idTopLeft.row-1;
            // may have contiguous hidden rows.  Blow by them
            while(  iRowToUse > fixedRowCount()
                    && rowHeight( iRowToUse) < 1 )
            {
                iRowToUse--;
            }

            int yScroll = rowHeight( iRowToUse);
            setScrollPos32(QSB_VERT, max(0, scrollPos - yScroll));
            rect.setTop(fixedRowHeight());
            update();
        }
        break;
        
    case QSB_PAGEDOWN: //向下一页
        if (scrollPos < m_nVScrollMax)
        {
            rect.setTop(fixedRowHeight());
            scrollPos = min(m_nVScrollMax, scrollPos + rect.height());
            setScrollPos32(QSB_VERT, scrollPos);
            rect.setTop(fixedRowHeight());
            update();
        }
        break;
        
    case QSB_PAGEUP: // 向上一页
        if (scrollPos > 0)
        {
            rect.setTop(fixedRowHeight());
            int offset = -rect.height();
            int pos = max(0, scrollPos + offset);
            setScrollPos32(QSB_VERT, pos);
            rect.setTop(fixedRowHeight());
            update();
        }
        break;
        
    case QSB_THUMBPOSITION: //滚动到指定位置
    case QSB_THUMBTRACK: //拖动滚动框
        {
            setScrollPos32(QSB_VERT, scrollPos32(QSB_VERT, true));
            m_idTopLeftCell.row = -1;
            HCellID idNewTopLeft = topleftNonFixedCell();
            if (idNewTopLeft != idTopLeft)
            {
                rect.setTop(fixedRowHeight());
                update();
            }
        }
        break;
        
    case QSB_TOP: //到顶段
        if (scrollPos > 0)
        {
            setScrollPos32(QSB_VERT, 0);
            update();
        }
        break;
        
    case QSB_BOTTOM: //到底端
        if (scrollPos < m_nVScrollMax)
        {
            setScrollPos32(QSB_VERT, m_nVScrollMax);
            update();
        }
        
    default: 
        break;
    }
}

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl implementation functions
//最核心函数
void HGridCtrl::onDraw(QPainter* painter)
{
    if(!m_bAllowDraw)
        return;

    //如果滚动区域为空
    if(NULL == viewport())
        return;
    QRect clipRect = viewport()->rect();

    eraseBkgnd(painter);            // OnEraseBkgnd does nothing, so erase bkgnd here.
    // This necessary since we may be using a Memory DC.
/*
#ifdef _DEBUG
	LARGE_INTEGER iStartCount;
	QueryPerformanceCounter(&iStartCount);
#endif*/

    QRect rect;
    int row, col;
    HGridCellBase* pCell;

    int nFixedRowHeight = fixedRowHeight();
    int nFixedColWidth  = fixedColumnWidth();

    HCellID idTopLeft = topleftNonFixedCell();
    int minVisibleRow = idTopLeft.row,
        minVisibleCol = idTopLeft.col;

    QRect VisRect;
    HCellRange VisCellRange = visibleNonFixedCellRange(VisRect);//huangw
    int maxVisibleRow = VisCellRange.maxRow(),
        maxVisibleCol = VisCellRange.maxCol();

    //if (isVirtualMode())  --huangw
    //    SendCacheHintToParent(VisCellRange);

    painter->save();
    QPen pen(m_crGridLineColour);
    pen.setWidth(0);
    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);


    //1.先画垂直水平线条，每个格子画一个
    //绘制垂直线条
    if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_VERT)
    {
        int x = nFixedColWidth;
        for (col = minVisibleCol; col <= maxVisibleCol; col++)
        {
            if (columnWidth(col) <= 0) continue;

            x += columnWidth(col);
            painter->drawLine(QPoint(x-1,nFixedRowHeight),QPoint(x-1,VisRect.bottom()));
        }
    }

    //绘制水平线条
    if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_HORZ)
    {
        int y = nFixedRowHeight;
        for (row = minVisibleRow; row <= maxVisibleRow; row++)
        {
            if (rowHeight(row) <= 0) continue;

            y += rowHeight(row);
            painter->drawLine(QPoint(nFixedColWidth,y-1),QPoint(VisRect.right(),y-1));
        }
    }

    painter->setPen(Qt::NoPen);
    //2.绘制单元格里面的内容
    rect.setBottom(nFixedRowHeight-1);
    for (row = minVisibleRow; row <= maxVisibleRow; row++)
    {
        if (rowHeight(row) <= 0) continue;

        rect.setTop(rect.bottom()+1);
        rect.setBottom(rect.top() + rowHeight(row)-1);

        // rect.bottom = bottom pixel of previous row
        if (rect.top() > clipRect.bottom())
            break;                // Gone past cliprect
        if (rect.bottom() < clipRect.top())
            continue;

        rect.setRight(nFixedColWidth-1);
        for (col = minVisibleCol; col <= maxVisibleCol; col++)
        {
            if (columnWidth(col) <= 0) continue;

            rect.setLeft(rect.right()+1);
            rect.setRight(rect.left() + columnWidth(col)-1);

            if (rect.left() > clipRect.right())
                break;        // gone past cliprect
            if (rect.right() < clipRect.left())
                continue;     // Reached cliprect yet?

            pCell = getCell(row, col);
            if (pCell)
			{
                /*if(!pCell->IsMerged())
				{
					if(!pCell->IsMergeWithOthers())
					{
						pCell->SetCoords(row,col);
                        pCell->Draw(pDC, row, col, rect, false);
					}
					else
					{
						CGridCellBase* pMergedCell=GetCell(pCell->GetMergeCellID());
						CRect mergerect=rect;
                        if(cellRangeRect(pMergedCell->m_MergeRange,&mergerect))
						{
							mergerect.DeflateRect(0,0,1,1);
							pMergedCell->SetCoords(pCell->GetMergeCellID().row,pCell->GetMergeCellID().col);
                            pMergedCell->Draw(pDC, pCell->GetMergeCellID().row,pCell->GetMergeCellID().col, mergerect, true);
						}
					}
				}
				else
				{
					CRect mergerect=rect;

                    if(cellRangeRect(pCell->m_MergeRange,&mergerect))
					{
						mergerect.DeflateRect(0,0,1,1);
						pCell->SetCoords(row,col);
                        pCell->Draw(pDC, row, col, mergerect, true);
					}
                }*/
                //huangw
                pCell->setCoords(row,col);
                pCell->draw(painter, row, col, rect, false);
			}
        }
    }

    //注意：表头行列此处没有实现合并部分，有点类似excel
    // draw fixed column cells:  m_nFixedRows..n, 0..m_nFixedCols-1
    //3.绘制列表头部分
    rect.setBottom(nFixedRowHeight-1);
    for (row = minVisibleRow; row <= maxVisibleRow; row++)
    {
        if (rowHeight(row) <= 0) continue;

        rect.setTop(rect.bottom()+1);
        rect.setBottom(rect.top() + rowHeight(row)-1);

        //huangw
        /*
        // rect.bottom = bottom pixel of previous row
        if (rect.top > clipRect.bottom)
            break;                // Gone past cliprect
        if (rect.bottom < clipRect.top)
            continue;             // Reached cliprect yet?
            */
        rect.setRight(-1);
        for (col = 0; col < m_nFixedCols; col++)
        {
            if (columnWidth(col) <= 0) continue;

            rect.setLeft(rect.right()+1);
            rect.setRight(rect.left() + columnWidth(col)-1);

            //huangw
            /*
            if (rect.left > clipRect.right)
                break;            // gone past cliprect
            if (rect.right < clipRect.left)
                continue;         // Reached cliprect yet?
                */

            pCell = getCell(row, col);
            if (pCell)
            {
                pCell->setCoords(row,col);
                pCell->draw(painter, row, col, rect, false);
            }
            /* huangw
            if (pCell)
			{
				//Used for merge cells
				//by Huang Wei

				if(!pCell->IsMerged())
				{
					if(!pCell->IsMergeWithOthers())
					{
						pCell->SetCoords(row,col);
                        pCell->Draw(pDC, row, col, rect, false);
					}
					else
					{
						CGridCellBase* pMergedCell=GetCell(pCell->GetMergeCellID());
						CRect mergerect=rect;
                        if(cellRangeRect(pMergedCell->m_MergeRange,&mergerect))
						{
							mergerect.DeflateRect(0,0,1,1);
							pMergedCell->SetCoords(pCell->GetMergeCellID().row,pCell->GetMergeCellID().col);
                            pMergedCell->Draw(pDC, pCell->GetMergeCellID().row,pCell->GetMergeCellID().col, mergerect, true);
						}
					}
				}
				else
				{
					CRect mergerect=rect;

                    if(cellRangeRect(pCell->m_MergeRange,&mergerect))
					{
						mergerect.DeflateRect(0,0,1,1);
						pCell->SetCoords(row,col);
                        pCell->Draw(pDC, row, col, mergerect, true);
					}
				}
            }*/
        }
    }

    // draw fixed row cells  0..m_nFixedRows, m_nFixedCols..n
    //4.绘制行表头问题
    rect.setBottom(-1);
    for (row = 0; row < m_nFixedRows; row++)
    {
        if (rowHeight(row) <= 0) continue;

        rect.setTop(rect.bottom()+1);
        rect.setBottom(rect.top() + rowHeight(row)-1);

        /* huangw
        // rect.bottom = bottom pixel of previous row
        if (rect.top > clipRect.bottom)
            break;                // Gone past cliprect
        if (rect.bottom < clipRect.top)
            continue;             // Reached cliprect yet?
            */

        rect.setRight(nFixedColWidth-1);
        for (col = minVisibleCol; col <= maxVisibleCol; col++)
        {
            if (columnWidth(col) <= 0) continue;

            rect.setLeft(rect.right()+1);
            rect.setRight(rect.left() + columnWidth(col)-1);

            /* huangw
            if (rect.left > clipRect.right)
                break;        // gone past cliprect
            if (rect.right < clipRect.left)
                continue;     // Reached cliprect yet?
                */

            pCell = getCell(row, col);
            if (pCell)
            {
                pCell->setCoords(row,col);
                pCell->draw(painter, row, col, rect, false);
            }
            /*
            if (pCell)
			{
				//Used for merge cells
				//by Huang Wei

				if(!pCell->IsMerged())
				{
					if(!pCell->IsMergeWithOthers())
					{
						pCell->SetCoords(row,col);
                        pCell->Draw(pDC, row, col, rect, false);
					}
					else
					{
						CGridCellBase* pMergedCell=GetCell(pCell->GetMergeCellID());
						CRect mergerect=rect;
                        if(cellRangeRect(pMergedCell->m_MergeRange,&mergerect))
						{
							mergerect.DeflateRect(0,0,1,1);
							pMergedCell->SetCoords(pCell->GetMergeCellID().row,pCell->GetMergeCellID().col);
                            pMergedCell->Draw(pDC, pCell->GetMergeCellID().row,pCell->GetMergeCellID().col, mergerect, true);
						}
					}
				}
				else
				{
					CRect mergerect=rect;

                    if(cellRangeRect(pCell->m_MergeRange,&mergerect))
					{
						mergerect.DeflateRect(0,0,1,1);
						pCell->SetCoords(row,col);
                        pCell->Draw(pDC, row, col, mergerect, true);
					}
				}
            }*/
        }
    }
/*  already
    // draw top-left cells 0..m_nFixedRows-1, 0..m_nFixedCols-1
    rect.bottom = -1;
    for (row = 0; row < m_nFixedRows; row++)
    {
        if (GetRowHeight(row) <= 0) continue;

        rect.top = rect.bottom+1;
        rect.bottom = rect.top + GetRowHeight(row)-1;
        rect.right = -1;

        for (col = 0; col < m_nFixedCols; col++)
        {
            if (GetColumnWidth(col) <= 0) continue;

            rect.left = rect.right+1;
            rect.right = rect.left + GetColumnWidth(col)-1;

            pCell = GetCell(row, col);
            if (pCell)
			{
				pCell->SetCoords(row,col);
                pCell->Draw(pDC, row, col, rect, false);
			}
        }
    }
*/

	// draw top-left cells 0..m_nFixedRows-1, 0..m_nFixedCols-1
    rect.setBottom(-1);
	for (row = 0; row < m_nFixedRows; row++)
	{
        if (rowHeight(row) <= 0) continue;

        rect.setTop(rect.bottom()+1);
        rect.setBottom(rect.top() + rowHeight(row)-1);
        rect.setRight(-1);

		for (col = 0; col < m_nFixedCols; col++)
		{
            if (columnWidth(col) <= 0) continue;

            rect.setLeft(rect.right()+1);
            rect.setRight(rect.left() + columnWidth(col)-1);

            pCell = getCell(row, col);
            if (pCell)
			{
            pCell->setCoords(row,col);
            pCell->draw(painter, row, col, rect, false);
            }
            /*
			if (pCell)
			{
				//Used for merge cells by Huang Wei
				//bugfix by Luther Bruck

				if(!pCell->IsMerged())
				{
					if(!pCell->IsMergeWithOthers())
					{
						pCell->SetCoords(row,col);
                        pCell->Draw(pDC, row, col, rect, false);
					}
					else
					{
						CGridCellBase* pMergedCell=GetCell(pCell->GetMergeCellID());
						CRect mergerect=rect;
                        if(cellRangeRect(pMergedCell->m_MergeRange,&mergerect))
						{
							mergerect.DeflateRect(0,0,1,1);
							pMergedCell->SetCoords(pCell->GetMergeCellID().row,pCell->GetMergeCellID().col);
                            pMergedCell->Draw(pDC, pCell->GetMergeCellID().row,pCell->GetMergeCellID().col, mergerect, true);
						}
					}
				}
				else
				{
					CRect mergerect=rect;

                    if(cellRangeRect(pCell->m_MergeRange,&mergerect))
					{
						mergerect.DeflateRect(0,0,1,1);
						pCell->SetCoords(row,col);
                        pCell->Draw(pDC, row, col, mergerect, true);
					}
				}
            }*/
		}
	}


    //本来放在mouseMoveEvent里面的，但QPainter指针只能在paintEvent里面，所以放在此处
    if (m_bLMouseButtonDown)
    {
        switch (m_MouseMode){
        case MOUSE_SIZING_COL:{
                QRect newInvertedRect(QPoint(m_CurMousePoint.x(), clipRect.top()),QPoint(m_CurMousePoint.x() + 1, clipRect.bottom()));
                painter->fillRect(newInvertedRect,Qt::black);
            }
            break;
        case MOUSE_SIZING_ROW:{
            QRect newInvertedRect(QPoint(clipRect.left(), m_CurMousePoint.y()),QPoint(clipRect.right(), m_CurMousePoint.y() +1 ));
            painter->fillRect(newInvertedRect,Qt::darkBlue);
            break;
        }
        }
    }


    painter->restore();
    // Let parent know it can discard it's data if it needs to.
    //if (isVirtualMode())
    //   SendCacheHintToParent(HCellRange(-1,-1,-1,-1));
/*
#ifdef _DEBUG
	LARGE_INTEGER iEndCount;
	QueryPerformanceCounter(&iEndCount);
	TRACE1("Draw counter ticks: %d\n", iEndCount.LowPart-iStartCount.LowPart);
#endif*/

}

////////////////////////////////////////////////////////////////////////////////////////
// CGridCtrl Cell selection stuff

// Is a given cell designation valid (ie within the bounds of our number
// of columns/rows)?
bool HGridCtrl::isValid(int nRow, int nCol) const
{
    bool bIsValid = (nRow >= 0 && nRow < m_nRows && nCol >= 0 && nCol < m_nCols);
    return bIsValid;
}

bool HGridCtrl::isValid(const HCellID& cell) const
{
    return isValid(cell.row, cell.col);
}

// Is a given cell range valid (ie within the bounds of our number
// of columns/rows)?
bool HGridCtrl::isValid(const HCellRange& range) const
{
    return (range.minRow() >= 0 && range.minCol() >= 0 &&
        range.maxRow() >= 0 && range.maxCol() >= 0 &&
        range.maxRow() < m_nRows && range.maxCol() < m_nCols &&
        range.minRow() <= range.maxRow() && range.minCol() <= range.maxCol());
}


// Enables/Disables redraw for certain operations like columns auto-sizing etc,
// but not for user caused things such as selection changes.
void HGridCtrl::setRedraw(bool bAllowDraw, bool bResetScrollBars  )
{
    if (bAllowDraw && !m_bAllowDraw)
    {
        m_bAllowDraw = true;
        refresh();
    }

    m_bAllowDraw = bAllowDraw;
    if (bResetScrollBars)
        resetScrollBars();
}

// Forces a redraw of a cell immediately (using a direct DC construction,
// or the supplied dc)
bool HGridCtrl::redrawCell(const HCellID& cell, QPainter* pDC )
{
    return redrawCell(cell.row, cell.col, pDC);
}

bool HGridCtrl::redrawCell(int nRow, int nCol, QPainter* pDC )
{
    bool bResult = true;
    if (!m_bAllowDraw || !isCellVisible(nRow, nCol))
        return false;

    QRect rect;
    if (!cellRect(nRow, nCol, rect))
        return false;


    //redrawCell函数好像没什么用 待确定
    if(NULL == pDC)
    {
        //pDC = new QPainter(viewport());
    }

    if (pDC)
    {
        // Redraw cells directly
       /* if (nRow < m_nFixedRows || nCol < m_nFixedCols)
        {
            HGridCellBase* pCell = getCell(nRow, nCol);
            if (pCell)
                bResult = pCell->draw(pDC, nRow, nCol, rect, true);
        }
        else
        {
            HGridCellBase* pCell = getCell(nRow, nCol);
            if (pCell)
                bResult = pCell->draw(pDC, nRow, nCol, rect, true);

            // Since we have erased the background, we will need to redraw the gridlines
            pDC->save();
            QPen pen(m_crGridLineColour,0,Qt::SolidLine);
            //pen.CreatePen(PS_SOLID, 0, m_crGridLineColour);
            //CPen* pOldPen = (CPen*) pDC->SelectObject(&pen);
            pDC->setPen(pen);
            if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_HORZ)
            {
                pDC->drawLine(QPoint(rect.left(),rect.bottom()),QPoint(rect.right() + 1, rect.bottom()));
                //pDC->LineTo(rect.right + 1, rect.bottom);
            }
            if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_VERT)
            {
                pDC->drawLine(QPoint(rect.right(), rect.top()),QPoint(rect.right(), rect.bottom() + 1));
                //pDC->LineTo(rect.right, rect.bottom + 1);
            }
            pDC->restore();
            //pDC->SelectObject(pOldPen);

        }*/
    } else
        viewport()->update(rect);     // Could not get a DC - invalidate it anyway
    // and hope that OnPaint manages to get one
    return bResult;
}

// redraw a complete row
bool HGridCtrl::redrawRow(int row)
{
    bool bResult = true;

    QPainter *pDC = m_painter;
    for (int col = 0; col < columnCount(); col++)
        bResult = redrawCell(row, col, pDC) && bResult;
    return bResult;
}

// redraw a complete column
bool HGridCtrl::redrawColumn(int col)
{
    bool bResult = true;
    QPainter *pDC = m_painter;
    for (int row = 0; row < rowCount(); row++)
        bResult = redrawCell(row, col, pDC) && bResult;
    return bResult;
}

// Sets the range of currently selected cells
void HGridCtrl::setSelectedRange(const HCellRange& Range,
                                 bool bForceRepaint , bool bselectCells)
{
    setSelectedRange(Range.minRow(), Range.minCol(),
                     Range.maxRow(), Range.maxCol(),
                     bForceRepaint, bselectCells);
}

void HGridCtrl::setSelectedRange(int nMinRow, int nMinCol, int nMaxRow, int nMaxCol,
                                 bool bForceRepaint , bool bselectCells)
{
    if (!m_bEnableSelection)
        return;

    //CWaitCursor wait; // Thomas Haase

    QPainter* pDC = NULL;
    if (bForceRepaint)
        pDC = m_painter;

	// Only redraw visible cells
    HCellRange VisCellRange;
    VisCellRange = visibleNonFixedCellRange();
   
    // EFW - Bug fix - Don't allow selection of fixed rows
    if(nMinRow >= 0 && nMinRow < fixedRowCount())
        nMinRow = fixedRowCount();
    if(nMaxRow >= 0 && nMaxRow < fixedRowCount())
        nMaxRow = fixedRowCount();
    if(nMinCol >= 0 && nMinCol < fixedColumnCount())
        nMinCol = fixedColumnCount();
    if(nMaxCol >= 0 && nMaxCol < fixedColumnCount())
        nMaxCol = fixedColumnCount();

    // If we are selecting cells, then first clear out the list of currently selected cells, then
    if (bselectCells)
    {
         QMapIterator<quint32,HCellID> it(m_SelectedCellMap);
        // Unselect all previously selected cells
        while (it.hasNext())
        {
            it.next();
            quint32 key = it.key();
            HCellID cell;
            cell = m_SelectedCellMap.value(key);

            // Reset the selection flag on the cell
            if (isValid(cell))
            {
                // This will remove the cell from the m_SelectedCellMap map
                setItemState(cell.row, cell.col,
                    itemState(cell.row, cell.col) & ~GVIS_SELECTED);

                // If this is to be reselected, continue on past the redraw
                if (nMinRow <= cell.row && cell.row <= nMaxRow &&
                    nMinCol <= cell.col && cell.col <= nMaxCol)
                    continue;

                if ( VisCellRange.isValid() && VisCellRange.inRange( cell ) )
				{
					if (bForceRepaint && pDC)                    // Redraw NOW
                        redrawCell(cell.row, cell.col, pDC);
					else
                        invalidateCellRect(cell);                // Redraw at leisure
				}
            }
            else
            {
                m_SelectedCellMap.remove(key);  // if it's not valid, get rid of it!
            }
        }

        // if we are selecting cells, and there are previous selected cells to be retained 
        // (eg Ctrl is being held down) then copy them to the newly created list, and mark 
        // all these cells as selected
        // Note that if we are list mode, single row selection, then we won't be adding 
        // the previous cells. Only the current row of cells will be added (see below)
        if (!isSingleRowSelection() &&
            nMinRow >= 0 && nMinCol >= 0 && nMaxRow >= 0 && nMaxCol >= 0)
        {
            QMap<quint32,HCellID>::iterator it = m_PrevSelectedCellMap.begin();
            for (;it != m_PrevSelectedCellMap.end();it++ )
            {
                quint32 key = it.key();
                HCellID cell;
                cell = m_PrevSelectedCellMap.value(key);

                if (!isValid(cell))
                    continue;

                int nState = itemState(cell.row, cell.col);

                // Set state as Selected. This will add the cell to m_SelectedCellMap
                setItemState(cell.row, cell.col, nState | GVIS_SELECTED);

                if ( VisCellRange.isValid() && VisCellRange.inRange( cell ) )
				{
					// Redraw (immediately or at leisure)
					if (bForceRepaint && pDC)
                        redrawCell(cell.row, cell.col, pDC);
					else
                        invalidateCellRect(cell);
				}
            }
        }
    }

    // Now select/deselect all cells in the cell range specified. If selecting, and the cell 
    // has already been marked as selected (above) then ignore it. If we are deselecting and
    // the cell isn't selected, then ignore
    if (nMinRow >= 0 && nMinCol >= 0 && nMaxRow >= 0 && nMaxCol >= 0 &&
        nMaxRow < m_nRows && nMaxCol < m_nCols &&
        nMinRow <= nMaxRow && nMinCol <= nMaxCol)
    {
        for (int row = nMinRow; row <= nMaxRow; row++)
            for (int col = nMinCol; col <= nMaxCol; col++)
            {
                bool bCellSelected = isCellSelected(row, col);
                if (bselectCells == bCellSelected)
                    continue;    // Already selected or deselected - ignore

                // Set the selected state. This will add/remove the cell to m_SelectedCellMap
                if (bselectCells)
                    setItemState(row, col, itemState(row, col) | GVIS_SELECTED);
                else
                    setItemState(row, col, itemState(row, col) & ~GVIS_SELECTED);

                if ( VisCellRange.isValid() && VisCellRange.inRange(row, col) )
				{
	                // Redraw (immediately or at leisure)
	                if (bForceRepaint && pDC)
                        redrawCell(row, col, pDC);
	                else
                        invalidateCellRect(row, col);
				}
            }
    }
    //    TRACE(_T("%d cells selected.\n"), m_SelectedCellMap.GetCount());

    if (pDC != NULL)
    {
        delete pDC;
        pDC = NULL;
    }
}


// selects all cells
void HGridCtrl::selectAllCells()
{
    if (!m_bEnableSelection)
        return;

    setSelectedRange(m_nFixedRows, m_nFixedCols, rowCount()-1, columnCount()-1);
}

// selects columns
void HGridCtrl::selectColumns(HCellID currentCell,
                              bool bForceRedraw , bool bselectCells)
{
    if (!m_bEnableSelection)
        return;

    //if (currentCell.col == m_idCurrentCell.col) return;
    if (currentCell.col < m_nFixedCols)
        return;
    if (!isValid(currentCell))
        return;

    if (isSingleColSelection())
        setSelectedRange(fixedRowCount(), currentCell.col,
                         rowCount()-1,    currentCell.col,
                         bForceRedraw, bselectCells);
    else
        setSelectedRange(fixedRowCount(),
                         min(m_SelectionStartCell.col, currentCell.col),
                         rowCount()-1,
                         max(m_SelectionStartCell.col, currentCell.col),
                         bForceRedraw, bselectCells);
}

// selects rows
void HGridCtrl::selectRows(HCellID currentCell,
                           bool bForceRedraw , bool bselectCells )
{
    if (!m_bEnableSelection)
        return;

    //if (currentCell.row; == m_idCurrentCell.row) return;
    if (currentCell.row < m_nFixedRows)
        return;
    if (!isValid(currentCell))
        return;

    if (isSingleRowSelection())
        setSelectedRange(currentCell.row, fixedColumnCount(),
                         currentCell.row, columnCount()-1,
                         bForceRedraw, bselectCells);
    else
        setSelectedRange(min(m_SelectionStartCell.row, currentCell.row),
                         fixedColumnCount(),
                         max(m_SelectionStartCell.row, currentCell.row),
                         columnCount()-1,
                         bForceRedraw, bselectCells);
}

// selects cells
void HGridCtrl::selectCells(HCellID currentCell,
                            bool bForceRedraw , bool bselectCells )
{
    if (!m_bEnableSelection)
        return;

    int row = currentCell.row;
    int col = currentCell.col;
    if (row < m_nFixedRows || col < m_nFixedCols)
        return;
    if (!isValid(currentCell))
        return;

    // Prevent unnecessary redraws
    //if (currentCell == m_LeftClickDownCell)  return;
    //else if (currentCell == m_idCurrentCell) return;

    setSelectedRange(min(m_SelectionStartCell.row, row),
                     min(m_SelectionStartCell.col, col),
                     max(m_SelectionStartCell.row, row),
                     max(m_SelectionStartCell.col, col),
                     bForceRedraw, bselectCells);
}

// Called when mouse/keyboard selection is a-happening.
void HGridCtrl::onSelecting(const HCellID& currentCell)
{
    if (!m_bEnableSelection)
        return;

    switch (m_MouseMode)
    {
    case MOUSE_SELECT_ALL:
        selectAllCells();
        break;
    case MOUSE_SELECT_COL:
        selectColumns(currentCell, false);
        break;
    case MOUSE_SELECT_ROW:
        selectRows(currentCell, false);
        break;
    case MOUSE_SELECT_CELLS:
        selectCells(currentCell, false);
        break;
    }

    // EFW - Bug fix [REMOVED CJM: this will cause infinite loop in list mode]
    setFocusCell(max(currentCell.row, m_nFixedRows), max(currentCell.col, m_nFixedCols));
}

void HGridCtrl::validateAndModifyCellContents(int nRow, int nCol, const QString& strText)
{
    if (!isCellEditable(nRow, nCol))
        return;
    //huangw 发送消息
/*
    if (SendMessageToParent(nRow, nCol, GVN_BEGINLABELEDIT) >= 0)
    {
        CString strCurrentText = itemText(nRow, nCol);
        if (strCurrentText != strText)
        {
            SetItemText(nRow, nCol, strText);
            if (ValidateEdit(nRow, nCol, strText) && 
                SendMessageToParent(nRow, nCol, GVN_ENDLABELEDIT) >= 0)
            {
                setModified(true, nRow, nCol);
                RedrawCell(nRow, nCol);
            }
            else
            {
                SetItemText(nRow, nCol, strCurrentText);
            }
        }
    }
    */
}

void HGridCtrl::clearCells(HCellRange Selection)
{
    for (int row = Selection.minRow(); row <= Selection.maxRow(); row++)
    {
        for (int col = Selection.minCol(); col <= Selection.maxCol(); col++)
        {
            // don't clear hidden cells
            if ( m_arRowHeights[row] > 0 && m_arColWidths[col] > 0 )
            {
                validateAndModifyCellContents(row, col, (""));
            }
		}
	}
    refresh();
}

#ifndef QT_NO_CLIPBOARD
////////////////////////////////////////////////////////////////////////////////////////
// Clipboard functions

// Deletes the contents from the selected cells
void HGridCtrl::cutSelectedText()
{
    if (!isEditable())
        return;
    QMap<quint32,HCellID>::iterator it;
    for(it = m_SelectedCellMap.begin();it != m_SelectedCellMap.end();++it)
    {
        quint32 key1 = it.key();
        HCellID cell;
        cell = m_SelectedCellMap.value(key1);
        validateAndModifyCellContents(cell.row, cell.col, (""));
    }

}

// Copies text from the selected cells to the clipboard
QString HGridCtrl::copyTextFromGrid()
{
    HCellRange Selection = selectedCellRange();
    if (!isValid(Selection))
        return NULL;

    //if (isVirtualMode())
    //    SendCacheHintToParent(Selection);

    //获取选择文字 返回
    // Get a tab delimited string to copy to cache
    QString str;
    HGridCellBase *pCell;
    for (int row = Selection.minRow(); row <= Selection.maxRow(); row++)
    {
        // don't copy hidden cells
        if( m_arRowHeights[row] <= 0 )
            continue;

        str = "";
        for (int col = Selection.minCol(); col <= Selection.maxCol(); col++)
        {
            // don't copy hidden cells
            if( m_arColWidths[col] <= 0 )
                continue;

            pCell = getCell(row, col);
            if (pCell &&(pCell->state() & GVIS_SELECTED))
            {
                str += pCell->text();
            }
            if (col != Selection.maxCol())
                str += tr("\t");
        }
        if (row != Selection.maxRow())
            str += tr("\n");
    }

    //if (GetVirtualMode())
    //    SendCacheHintToParent(HCellRange(-1,-1,-1,-1));
    return str;
}

// Pastes text from the clipboard to the selected cells
bool HGridCtrl::pasteTextToGrid(HCellID& cell, const QString& strCopyText, bool bSelectPastedCells)
{
    if (!isValid(cell) || !isCellEditable(cell))
        return false;

    QString strText = strCopyText;

    // Parse text data and set in cells...
    //strText.LockBuffer();
    QString strLine = strText;
    int nLine = 0;

    // Find the end of the first line
    HCellRange PasteRange(cell.row, cell.col,-1,-1);
    int nIndex;
    do
    {
        int nColumn = 0;
        nIndex = strLine.indexOf(tr("\n"));

        // Store the remaining chars after the newline
        QString strNext = (nIndex < 0)? ("")  : strLine.mid(nIndex + 1);

        // Remove all chars after the newline
        if (nIndex >= 0)
            strLine = strLine.left(nIndex);

        int nLineIndex = strLine.indexOf(tr("\t,"));
        QString strCellText = (nLineIndex >= 0)? strLine.left(nLineIndex) : strLine;

        // skip hidden rows
        int iRowVis = cell.row + nLine;
        while( iRowVis < rowCount())
        {
            if( rowHeight(iRowVis) > 0)
                break;
            nLine++;
            iRowVis++;
        }

        while (!strLine.isEmpty())
        {
            // skip hidden columns
            int iColVis = cell.col + nColumn;
            while( iColVis < columnCount())
            {
                if( columnWidth( iColVis) > 0)
                    break;
                nColumn++;
                iColVis++;
            }

            HCellID TargetCell(iRowVis, iColVis);
            if (isValid(TargetCell))
            {
                strCellText.trimmed();
                //strCellText.TrimRight();

                validateAndModifyCellContents(TargetCell.row, TargetCell.col, strCellText);

                // Make sure cell is not selected to avoid data loss
                setItemState(TargetCell.row, TargetCell.col,
                    itemState(TargetCell.row, TargetCell.col) & ~GVIS_SELECTED);

                if (iRowVis > PasteRange.maxRow()) PasteRange.setMaxRow(iRowVis);
                if (iColVis > PasteRange.maxCol()) PasteRange.setMaxCol(iColVis);
            }

            strLine = (nLineIndex >= 0)? strLine.mid(nLineIndex + 1) : tr("");
            nLineIndex = strLine.indexOf(tr("\t,"));
            strCellText = (nLineIndex >= 0)? strLine.left(nLineIndex) : strLine;

            nColumn++;
        }

        strLine = strNext;
        nLine++;
    } while (nIndex >= 0);

    //strText.UnlockBuffer();

	if (bSelectPastedCells)
        setSelectedRange(PasteRange, true);
	else
	{
        resetSelectedRange();
        refresh();
	}

    return true;
}
#endif

/*
#ifndef GRIDCONTROL_NO_DRAGDROP

// Start drag n drop
void HGridCtrl::OnBeginDrag()
{
    if (!m_bAllowDragAndDrop)
        return;

    COleDataSource* pSource = copyTextFromGrid();
    if (pSource)
    {
        SendMessageToParent(selectedCellRange().GetTopLeft().row,
            selectedCellRange().GetTopLeft().col,
            GVN_BEGINDRAG);

        m_MouseMode = MOUSE_DRAGGING;
        m_bLMouseButtonDown = false;

        DROPEFFECT dropEffect = pSource->DoDragDrop(DROPEFFECT_COPY | DROPEFFECT_MOVE);

        if (dropEffect & DROPEFFECT_MOVE)
            cutSelectedText();

        if (pSource)
            delete pSource;    // Did not pass source to clipboard, so must delete
    }
}

// Handle drag over grid
DROPEFFECT HGridCtrl::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState,
                                 CPoint point)
{
    // Any text data available for us?
    if (!m_bAllowDragAndDrop || !IsEditable() || !pDataObject->IsDataAvailable(CF_TEXT))
        return DROPEFFECT_NONE;

    // Find which cell we are over and drop-highlight it
    HCellID cell = cellFromPt(point, false);

    // If not valid, set the previously drop-highlighted cell as no longer drop-highlighted
    if (!isValid(cell))
    {
        OnDragLeave();
        m_LastDragOverCell = HCellID(-1,-1);
        return DROPEFFECT_NONE;
    }

    if (!IsCellEditable(cell))
        return DROPEFFECT_NONE;

    // Have we moved over a different cell than last time?
    if (cell != m_LastDragOverCell)
    {
        // Set the previously drop-highlighted cell as no longer drop-highlighted
        if (isValid(m_LastDragOverCell))
        {
            UINT nState = GetItemState(m_LastDragOverCell.row, m_LastDragOverCell.col);
            SetItemState(m_LastDragOverCell.row, m_LastDragOverCell.col,
                nState & ~GVIS_DROPHILITED);
            RedrawCell(m_LastDragOverCell);
        }

        m_LastDragOverCell = cell;

        // Set the new cell as drop-highlighted
        if (isValid(m_LastDragOverCell))
        {
            UINT nState = GetItemState(m_LastDragOverCell.row, m_LastDragOverCell.col);
            SetItemState(m_LastDragOverCell.row, m_LastDragOverCell.col,
                nState | GVIS_DROPHILITED);
            RedrawCell(m_LastDragOverCell);
        }
    }

    // Return an appropraite value of DROPEFFECT so mouse cursor is set properly
    if (dwKeyState & MK_CONTROL)
        return DROPEFFECT_COPY;
    else
        return DROPEFFECT_MOVE;
}

// Something has just been dragged onto the grid
DROPEFFECT HGridCtrl::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState,
                                  CPoint point)
{
    // Any text data available for us?
    if (!m_bAllowDragAndDrop || !pDataObject->IsDataAvailable(CF_TEXT))
        return DROPEFFECT_NONE;

    // Find which cell we are over and drop-highlight it
    m_LastDragOverCell = cellFromPt(point, false);
    if (!isValid(m_LastDragOverCell))
        return DROPEFFECT_NONE;

    if (!IsCellEditable(m_LastDragOverCell))
        return DROPEFFECT_NONE;

    if (isValid(m_LastDragOverCell))
    {
        UINT nState = GetItemState(m_LastDragOverCell.row, m_LastDragOverCell.col);
        SetItemState(m_LastDragOverCell.row, m_LastDragOverCell.col,
            nState | GVIS_DROPHILITED);
        RedrawCell(m_LastDragOverCell);
    }

    // Return an appropraite value of DROPEFFECT so mouse cursor is set properly
    if (dwKeyState & MK_CONTROL)
        return DROPEFFECT_COPY;
    else
        return DROPEFFECT_MOVE;
}

// Something has just been dragged away from the grid
void HGridCtrl::OnDragLeave()
{
    // Set the previously drop-highlighted cell as no longer drop-highlighted
    if (isValid(m_LastDragOverCell))
    {
        UINT nState = GetItemState(m_LastDragOverCell.row, m_LastDragOverCell.col);
        SetItemState(m_LastDragOverCell.row, m_LastDragOverCell.col,
            nState & ~GVIS_DROPHILITED);
        RedrawCell(m_LastDragOverCell);
    }
}

// Something has just been dropped onto the grid
bool HGridCtrl::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect,
                       CPoint  point )
{
    m_MouseMode = MOUSE_NOTHING;
    
    if (!m_bAllowDragAndDrop || !IsCellEditable(m_LastDragOverCell))
        return false;

    OnDragLeave();

    return pasteTextToGrid(m_LastDragOverCell, pDataObject, false);
}
#endif
*/
//注意：函数只能对文字进行操作
#ifndef QT_NO_CLIPBOARD
void HGridCtrl::onEditCut()
{
    if (!isEditable())
        return;
    QString strCopy = copyTextFromGrid();
    if (!strCopy.isEmpty())
        return;
    QApplication::clipboard()->setText(strCopy);
    cutSelectedText();
}

void HGridCtrl::onEditCopy()
{
    QString strCopy = copyTextFromGrid();
    if (!strCopy.isEmpty())
        return;
    QApplication::clipboard()->setText(strCopy);
}

void HGridCtrl::onEditPaste()
{
    if (!isEditable())
        return;

    HCellRange cellRange = selectedCellRange();

	// Get the top-left selected cell, or the Focus cell, or the topleft (non-fixed) cell
    HCellID cell;
    if (cellRange.isValid())
	{
        cell.row = cellRange.minRow();
        cell.col = cellRange.minCol();
	}
	else
	{
        cell = focusCell();
        if (!isValid(cell))
            cell = topleftNonFixedCell();
        if (!isValid(cell))
			return;
	}

	// If a cell is being edited, then call it's edit window paste function.
    if ( isItemEditing(cell.row, cell.col) )
    {
        HGridCellBase* pCell = getCell(cell.row, cell.col);
        Q_ASSERT(pCell);
        if (!pCell) return;

        /*CWnd* pEditWnd = pCell->GetEditWnd();
		if ( pEditWnd && pEditWnd->IsKindOf(RUNTIME_CLASS(CEdit)) )
		{
			((CEdit*)pEditWnd)->Paste();
			return;
        }*/
    }

    // 从选择的单元格开始进行复制
    /*COleDataObject obj;
    if (obj.AttachClipboard())
        pasteTextToGrid(cell, &obj);*/
}
#endif

void HGridCtrl::onEditSelectAll()
{
    //SendMessageToParent(m_LeftClickDownCell.row, m_LeftClickDownCell.col, GVN_SELCHANGING);
    selectAllCells();
    //SendMessageToParent(m_idCurrentCell.row, m_idCurrentCell.col, GVN_SELCHANGED);
}

/*
#ifndef QT_NO_CLIPBOARD
void HGridCtrl::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetSelectedCount() > 0); // - Thomas Haase 
    //HCellRange Selection = selectedCellRange();
    //pCmdUI->Enable(Selection.Count() && isValid(Selection));
}

void HGridCtrl::OnUpdateEditCut(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(IsEditable() && GetSelectedCount() > 0); // - Thomas Haase 
    //HCellRange Selection = selectedCellRange();
    //pCmdUI->Enable(IsEditable() && Selection.Count() && isValid(Selection));
}

void HGridCtrl::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
    HCellID cell = GetFocusCell();

    bool bCanPaste = isValid(cell) && IsCellEditable(cell) &&
        ::IsClipboardFormatAvailable(CF_TEXT);

    pCmdUI->Enable(bCanPaste);
}
#endif

void HGridCtrl::OnUpdateEditSelectAll(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_bEnableSelection);
}
*/
////////////////////////////////////////////////////////////////////////////////////////
// hittest-like functions

// true if the mouse is over a row resize area
//鼠标只移动到表头行位置处调整
bool HGridCtrl::mouseOverRowResizeArea(QPoint& point)
{
    if (point.x() >= fixedColumnWidth())
        return false;

    HCellID idCurrentCell = cellFromPt(point);
    QPoint start;
    if (!cellOrigin(idCurrentCell, start))
        return false;

    int endy = start.y() + rowHeight(idCurrentCell.row);

    if ((point.y() - start.y() < m_nResizeCaptureRange && idCurrentCell.row != 0) ||
        endy - point.y() < m_nResizeCaptureRange)
    {
        return true;
    }
    else
        return false;
}

// true if the mouse is over a column resize area. point is in Client coords
//鼠标只移动到表头列位置处调整
bool HGridCtrl::mouseOverColumnResizeArea(QPoint& point)
{
    if (point.y() >= fixedRowHeight())
        return false;

    HCellID idCurrentCell = cellFromPt(point);
    QPoint start;
    if (!cellOrigin(idCurrentCell, start))
        return false;

    int endx = start.x() + columnWidth(idCurrentCell.col);
    if ((point.x() - start.x() < m_nResizeCaptureRange && idCurrentCell.col != 0) ||
        endx - point.x() < m_nResizeCaptureRange)
    {
        return true;
    }
    else
        return false;
}


// Get cell from point.
// point - client coordinates
// bAllowFixedCellCheck - if true then fixed cells are checked
HCellID HGridCtrl::cellFromPt(const QPoint& point, bool bAllowFixedCellCheck )
{
    HCellID cellID; // return value

    //1.判断如果取不包括固定行列同时非固定行左上角的无效
    HCellID idTopLeft = topleftNonFixedCell();
    if (!bAllowFixedCellCheck && !isValid(idTopLeft))
		return cellID;

    // calculate column index
    //2
    int fixedColWidth = fixedColumnWidth();//固定列宽

    if (point.x() < 0 || (!bAllowFixedCellCheck && point.x() < fixedColWidth)) // 点位置不在固定常规表格中，同时又不包括固定行列
        cellID.col = -1;
    else if (point.x() < fixedColWidth) // in fixed col 在固定行列之中
    {
        int xpos = 0;
        int col = 0;
        while (col < m_nFixedCols)
        {
            xpos += columnWidth(col);
            if (xpos > point.x())
                break;
			col++;
        }
        cellID.col = col;
    }
    else    // in non-fixed col 计算常规列的数目：只要加的列总宽超过点的x轴位置，就是列的数目
    {
        int xpos = fixedColWidth;
		int col = idTopLeft.col; //m_nFixedCols;
        while ( col < columnCount())
        {
            xpos += columnWidth(col);
            if (xpos > point.x())
                break;
			col++;
        }

        if (col >= columnCount())
            cellID.col = -1;
        else
            cellID.col = col;
    }

    // calculate row index 行同列一样计算
    int nFixedRowHeight = fixedRowHeight();
    if (point.y() < 0 || (!bAllowFixedCellCheck && point.y() < nFixedRowHeight)) // not in window
        cellID.row = -1;
    else if (point.y() < nFixedRowHeight) // in fixed col
    {
        int ypos = 0;
        int row = 0;
        while (row < m_nFixedRows) 
        {
            ypos += rowHeight(row);
            if (ypos > point.y())
                break;
			row++;
        }
        cellID.row = row;
    }
    else
    {
        int ypos = nFixedRowHeight;
		int row = idTopLeft.row; //m_nFixedRows;
        while ( row < rowCount() )
        {
            ypos += rowHeight(row);
            if (ypos > point.y())
                break;
			row++;
        }

        if (row >= rowCount())
            cellID.row = -1;
        else
            cellID.row = row;
    }
    //cellID=GetMergeCellID(cellID); // note：合并单元格时候用到的
    return cellID;
}

////////////////////////////////////////////////////////////////////////////////
// CGridCtrl cellrange functions

// Gets the first non-fixed cell ID
HCellID HGridCtrl::topleftNonFixedCell(bool bForceRecalculation )
{
    // Used cached value if possible
    if (m_idTopLeftCell.isValid() && !bForceRecalculation)
        return m_idTopLeftCell;

    //
    int nHorzScroll = horizontalScrollBar()->value();
    int nVertScroll = verticalScrollBar()->value();

    m_idTopLeftCell.col = m_nFixedCols;
    //有何用？ --huangw
    int nRight = 0;
    while (nRight < nHorzScroll && m_idTopLeftCell.col < (columnCount()-1))
        nRight += columnWidth(m_idTopLeftCell.col++);

    m_idTopLeftCell.row = m_nFixedRows;
    //有何用 --huangw
    int nTop = 0;
    while (nTop < nVertScroll && m_idTopLeftCell.row < (rowCount()-1))
        nTop += rowHeight(m_idTopLeftCell.row++);

    //TRACE2("TopLeft cell is row %d, col %d\n",m_idTopLeftCell.row, m_idTopLeftCell.col);
    return m_idTopLeftCell;
}


// This gets even partially visible cells
HCellRange HGridCtrl::visibleNonFixedCellRange(QRect& pRect,
                                                  bool bForceRecalculation )
{
    QRect rect = viewport()->rect();
    //GetClientRect(rect);

    HCellID idTopLeft = topleftNonFixedCell(bForceRecalculation);

    // calc bottom
    int bottom = fixedRowHeight();
	int i;
    for (i = idTopLeft.row; i < rowCount(); i++)
    {
        bottom += rowHeight(i);
        if (bottom >= rect.bottom())
        {
            bottom = rect.bottom();
            break;
        }
    }
    int maxVisibleRow = min(i, rowCount() - 1);

    // calc right
    int right = fixedColumnWidth();
    for (i = idTopLeft.col; i < columnCount(); i++)
    {
        right += columnWidth(i);
        if (right >= rect.right())
        {
            right = rect.right();
            break;
        }
    }
    int maxVisibleCol = min(i, columnCount() - 1);
    if (!pRect.isValid())
    {
        pRect.setTop(0);
        pRect.setLeft(0);
        pRect.setRight(right);
        pRect.setBottom(bottom);
    }

    return HCellRange(idTopLeft.row, idTopLeft.col, maxVisibleRow, maxVisibleCol);
}

/*
// used by ResetScrollBars() - This gets only fully visible cells
HCellRange HGridCtrl::GetUnobstructedNonFixedCellRange(bool bForceRecalculation )
{
    CRect rect;
    GetClientRect(rect);

    HCellID idTopLeft = GetTopleftNonFixedCell(bForceRecalculation);

    // calc bottom
    int bottom = GetFixedRowHeight();
	int i;
    for (i = idTopLeft.row; i < GetRowCount(); i++)
    {
        bottom += GetRowHeight(i);
        if (bottom >= rect.bottom)
            break;
    }
    int maxVisibleRow = min(i, GetRowCount() - 1);
    if (maxVisibleRow > 0 && bottom > rect.bottom)
        maxVisibleRow--;

    // calc right
    int right = GetFixedColumnWidth();
    for (i = idTopLeft.col; i < GetColumnCount(); i++)
    {
        right += GetColumnWidth(i);
        if (right >= rect.right)
            break;
    }
    int maxVisibleCol = min(i, GetColumnCount() - 1);
    if (maxVisibleCol > 0 && right > rect.right)
        maxVisibleCol--;


    return HCellRange(idTopLeft.row, idTopLeft.col, maxVisibleRow, maxVisibleCol);
}
*/
// Returns the minimum bounding range of the current selection
// If no selection, then the returned HCellRange will be invalid
HCellRange HGridCtrl::selectedCellRange()
{
    HCellRange Selection(rowCount(), columnCount(), -1,-1);

    QMap<quint32,HCellID>::iterator i = m_SelectedCellMap.begin();
    for(;i != m_SelectedCellMap.end();++i)
    {
        quint32 key = i.key();
        HCellID cell;
        cell = m_SelectedCellMap.value(key);

        Selection.setMinRow( min(Selection.minRow(), cell.row) );
        Selection.setMinCol( min(Selection.minCol(), cell.col) );
        Selection.setMaxRow( max(Selection.maxRow(), cell.row) );
        Selection.setMaxCol( max(Selection.maxCol(), cell.col) );
    }

    return Selection;
}

// Returns ALL the cells in the grid
HCellRange HGridCtrl::cellRange() const
{
    return HCellRange(0, 0, rowCount() - 1,columnCount() - 1);
}

// Resets the selected cell range to the empty set.
void HGridCtrl::resetSelectedRange()
{
    m_PrevSelectedCellMap.clear();
    setSelectedRange(-1,-1,-1,-1);
    setFocusCell(-1,-1);
}

//设置滚动条参数
void HGridCtrl::setScrollBarValue(uint Msg,HWPARAM wParam,HLPARAM IParam )
{
    QScrollBar *pScrollBar = NULL;
    if(QWM_HSCROLL == Msg)
    {
        pScrollBar = horizontalScrollBar();
        onHScroll(wParam,0,pScrollBar);
    }
    else if(QWM_VSCROLL == Msg)
    {
        pScrollBar = verticalScrollBar();
        onVScroll(wParam,0,pScrollBar);
    }
    else
        return;

}

// Get/Set scroll position using 32 bit functions
//和win32 有区别
int HGridCtrl::scrollPos32(int nBar, bool bGetTrackPos )
{
    if(QSB_HORZ == nBar)
    {
        return horizontalScrollBar()->value();
    }
    else if(QSB_VERT == nBar)
    {
        return verticalScrollBar()->value();
    }

    return 0;
}

bool HGridCtrl::setScrollPos32(int nBar, int nPos, bool bRedraw )
{
    //为了保持与原来代码一致，未做大的变动，此处代码只作为设置滚动条的位置，但不会触发HGridCtrl的槽函数
    //其实完全改动为Qt滚动条的方法可能会简单一点
    if(QSB_HORZ == nBar)
    {
        horizontalScrollBar()->setValue(nPos);
    }
    else if(QSB_VERT == nBar)
    {
        verticalScrollBar()->setValue(nPos);
     }

    return true;
}
/*
void HGridCtrl::enableScrollBars(int nBar, bool bEnable )
{
    if (bEnable)
    {
        if (!isVisibleHScroll() && (nBar == SB_HORZ || nBar == SB_BOTH))
        {
            m_nBarState |= GVL_HORZ;
            CWnd::EnableScrollBarCtrl(SB_HORZ, bEnable);
        }
        
        if (!isVisibleVScroll() && (nBar == SB_VERT || nBar == SB_BOTH))
        {
            m_nBarState |= GVL_VERT;
            CWnd::EnableScrollBarCtrl(SB_VERT, bEnable);
        }
    }
    else
    {
        if ( isVisibleHScroll() && (nBar == SB_HORZ || nBar == SB_BOTH))
        {
            m_nBarState &= ~GVL_HORZ; 
            CWnd::EnableScrollBarCtrl(SB_HORZ, bEnable);
        }
        
        if ( isVisibleVScroll() && (nBar == SB_VERT || nBar == SB_BOTH))
        {
            m_nBarState &= ~GVL_VERT;
            CWnd::EnableScrollBarCtrl(SB_VERT, bEnable);
        }
    }
}
*/
// If resizing or cell counts/sizes change, call this - it'll fix up the scroll bars
void HGridCtrl::resetScrollBars()
{
    // Force a refresh.
    m_idTopLeftCell.row = -1;

    if (!m_bAllowDraw)
        return;

    QRect rect;
    rect = viewport()->rect();
    // This would have caused OnSize event - Brian 
    //EnableScrollBars(SB_BOTH, false);
    
    if (rect.left() == rect.right() || rect.top() == rect.bottom())
        return;
    //把垂直滚动条的宽度要加到rect里面
    if (isVisibleVScroll())
        rect.setRight(rect.right() + verticalScrollBar()->width());
    
    if (isVisibleHScroll())
        rect.setBottom(rect.bottom() + horizontalScrollBar()->height());
    
    rect.setLeft(rect.left() + fixedColumnWidth());
    rect.setTop(rect.top() + fixedRowHeight());
    
    
    if (rect.left() >= rect.right() || rect.top() >= rect.bottom())
    {
        //EnableScrollBarCtrl(SB_BOTH, false);
        return;
    }
    
    QRect VisibleRect(fixedColumnWidth(), fixedRowHeight(),
                      rect.right(), rect.bottom());
    QRect VirtualRect(fixedColumnWidth(), fixedRowHeight(),
                      virtualWidth(), virtualHeight());
    
    // Removed to fix single row scrollbar problem (Pontus Goffe)
    // If vertical scroll bar, horizontal space is reduced
    if (VisibleRect.height() < VirtualRect.height())
        VisibleRect.setRight(VisibleRect.right() - verticalScrollBar()->width());

    // If horz scroll bar, vert space is reduced
    if (VisibleRect.width() < VirtualRect.width())
        VisibleRect.setBottom(VisibleRect.bottom() - horizontalScrollBar()->height());
    
    if (VisibleRect.height() < VirtualRect.height())
    {
        //EnableScrollBars(SB_VERT, true);
        m_nVScrollMax = VirtualRect.height() - 1;
    }
    else
    {
        //EnableScrollBars(SB_VERT, false);
        m_nVScrollMax = 0;
    }

    if (VisibleRect.width() < VirtualRect.width())
    {
        //EnableScrollBars(SB_HORZ, true);
        m_nHScrollMax = VirtualRect.width() - 1;
    }
    else
    {
        //EnableScrollBars(SB_HORZ, false);
        m_nHScrollMax = 0;
    }

    Q_ASSERT(m_nVScrollMax < INT_MAX && m_nHScrollMax < INT_MAX); // This should be fine

    horizontalScrollBar()->setPageStep(VisibleRect.width());
    horizontalScrollBar()->setRange(0,m_nHScrollMax);
    verticalScrollBar()->setPageStep(VisibleRect.height());
    verticalScrollBar()->setRange(0,m_nVScrollMax);

   /* SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_PAGE | SIF_RANGE;
    si.nPage = (m_nHScrollMax>0)? VisibleRect.Width() : 0;
    si.nMin = 0;
    si.nMax = m_nHScrollMax;
    SetScrollInfo(SB_HORZ, &si, true);

    si.fMask |= SIF_DISABLENOSCROLL;
    si.nPage = (m_nVScrollMax>0)? VisibleRect.Height() : 0;
    si.nMin = 0;
    si.nMax = m_nVScrollMax;
    SetScrollInfo(SB_VERT, &si, true);*/
}

////////////////////////////////////////////////////////////////////////////////////
// Row/Column position functions

// returns the top left point of the cell. Returns false if cell not visible.
// consider cell's merge

/*
// returns the top left point of the cell. Returns false if cell not visible.
// don't consider cell's merge
bool HGridCtrl::GetCellOriginNoMerge(int nRow, int nCol, LPPOINT p)
{
    int i;

    if (!isValid(nRow, nCol))
        return false;

    HCellID idTopLeft;
    if (nCol >= m_nFixedCols || nRow >= m_nFixedRows)
        idTopLeft = GetTopleftNonFixedCell();

    if ((nRow >= m_nFixedRows && nRow < idTopLeft.row) ||
        (nCol>= m_nFixedCols && nCol < idTopLeft.col))
        return false;

    p->x = 0;
    if (nCol < m_nFixedCols)                      // is a fixed column
        for (i = 0; i < nCol; i++)
            p->x += GetColumnWidth(i);
        else 
        {                                        // is a scrollable data column
            for (i = 0; i < m_nFixedCols; i++)
                p->x += GetColumnWidth(i);
            for (i = idTopLeft.col; i < nCol; i++)
                p->x += GetColumnWidth(i);
        }
        
        p->y = 0;
        if (nRow < m_nFixedRows)                      // is a fixed row
            for (i = 0; i < nRow; i++)
                p->y += GetRowHeight(i);
            else 
            {                                        // is a scrollable data row
                for (i = 0; i < m_nFixedRows; i++)
                    p->y += GetRowHeight(i);
                for (i = idTopLeft.row; i < nRow; i++)
                    p->y += GetRowHeight(i);
            }
            
            return true;
}
*/

bool HGridCtrl::cellOrigin(int nRow, int nCol, QPoint& p)
{
    int i;
    if (!isValid(nRow, nCol))
        return false;

    HCellID idTopLeft;
    if (nCol >= m_nFixedCols || nRow >= m_nFixedRows)
        idTopLeft = topleftNonFixedCell();

    //Merge the selected cells --huangw

     //note:源代码是有的，后来为了合并修改了
    //if ((nRow >= m_nFixedRows && nRow < idTopLeft.row) ||
    //    (nCol>= m_nFixedCols && nCol < idTopLeft.col))
    //    return false;

    p.setX(0);
    //如果是在固定列，就是从0开始计算到nCol的宽度之和
    if (nCol < m_nFixedCols)                      // is a fixed column
    {
        for (i = 0; i < nCol; i++)
            p.setX(p.x() + columnWidth(i));
    }
    else
    {                                        // is a scrollable data column
        //如果是正常列，就把固定列的位置和左上角列到正常列的位置相加
        for (i = 0; i < m_nFixedCols; i++)
            p.setX(p.x() + columnWidth(i));
        //note:此处也修改过 合并
        if(nCol>idTopLeft.col)
        {
            for (i = idTopLeft.col; i < nCol; i++)
                p.setX(p.x() + columnWidth(i));
        }
        else
        {
            for (i = nCol; i <idTopLeft.col ; i++)
                p.setX(p.x() - columnWidth(i));
        }
    }
        
    p.setY(0);
    if (nRow < m_nFixedRows)                      // is a fixed row
    {
        for (i = 0; i < nRow; i++)
            p.setY(p.y() + rowHeight(i));
    }
    else
    {                                        // is a scrollable data row
        for (i = 0; i < m_nFixedRows; i++)
            p.setY(p.y() + rowHeight(i));
        //note:此处也修改过 合并
        if(nRow>idTopLeft.row)
        {
            for (i = idTopLeft.row; i < nRow; i++)
                p.setY(p.y() + rowHeight(i));
        }
        else
        {
            for (i = nRow; i <idTopLeft.row; i++)
                p.setY(p.y() - rowHeight(i));
        }
    }
    return true;
}


bool HGridCtrl::cellOrigin(const HCellID& cell, QPoint& p)
{
    return cellOrigin(cell.row, cell.col, p);
}

/*
bool HGridCtrl::GetCellOriginNoMerge(const HCellID& cell, LPPOINT p)
{
    return GetCellOriginNoMerge(cell.row, cell.col, p);
}
*/
// Returns the bounding box of the cell
bool HGridCtrl::cellRect(const HCellID& cell, QRect& pRect)
{
    return cellRect(cell.row, cell.col, pRect);
}

bool HGridCtrl::cellRect(int nRow, int nCol, QRect& pRect)
{
    QPoint CellOrigin;
    if (!cellOrigin(nRow, nCol, CellOrigin))
        return false;

    //Merge the selected cells 
    HGridCellBase *pCell = (HGridCellBase*) getCell(nRow,nCol);
	
    /* //huangw
	if(!pCell->IsMerged())
	{
		pRect->left   = CellOrigin.x;
		pRect->top    = CellOrigin.y;
		pRect->right  = CellOrigin.x + GetColumnWidth(nCol)-1;
		pRect->bottom = CellOrigin.y + GetRowHeight(nRow)-1;
	}
	else
	{
        cellRangeRect(pCell->m_MergeRange,pRect);
    }*/

    pRect.setLeft(CellOrigin.x());
    pRect.setTop(CellOrigin.y());
    pRect.setRight(CellOrigin.x() + columnWidth(nCol)-1);
    pRect.setBottom(CellOrigin.y() + rowHeight(nRow)-1);
    return true;
}

bool HGridCtrl::textRect(const HCellID& cell, QRect& pRect)
{
    return textRect(cell.row, cell.col, pRect);
}

bool HGridCtrl::textRect(int nRow, int nCol, QRect& pRect)
{
    HGridCellBase* pCell = getCell( nRow, nCol);
    if( pCell == NULL)
        return false;
    
    if( !cellRect( nRow, nCol, pRect) )
        return false;

    return pCell->textRect( pRect);
}

// Returns the bounding box of a range of cells
//返回表格的外部box
bool HGridCtrl::cellRangeRect(const HCellRange& cellRange, QRect& lpRect)
{
    QPoint MinOrigin,MaxOrigin;

    if (!cellOrigin(cellRange.minRow(), cellRange.minCol(), MinOrigin))
        return false;
    if (!cellOrigin(cellRange.maxRow(), cellRange.maxCol(), MaxOrigin))
        return false;

    lpRect.setLeft(MinOrigin.x());
    lpRect.setTop(MinOrigin.y());
    lpRect.setRight(MaxOrigin.x() + columnWidth(cellRange.maxCol()) - 1);
    lpRect.setBottom(MaxOrigin.y() + rowHeight(cellRange.maxRow()) - 1);

    return true;
}

////////////////////////////////////////////////////////////////////////////////////
// Grid attribute functions
/*
LRESULT HGridCtrl::OnSetFont(WPARAM hFont, LPARAM lParam )
{
    LRESULT result = Default();

    // Get the logical font
    LOGFONT lf;
    if (!GetObject((HFONT) hFont, sizeof(LOGFONT), &lf))
        return result;

    m_cellDefault.SetFont(&lf);
    m_cellFixedColDef.SetFont(&lf);
    m_cellFixedRowDef.SetFont(&lf);
    m_cellFixedRowColDef.SetFont(&lf);

    refresh();

    return result;
}

LRESULT HGridCtrl::OnGetFont(WPARAM wParam, LPARAM lParam)
{
    //LOGFONT    lf;
    //m_cellDefault.GetFontObject()->GetLogFont(&lf);

    return (LRESULT) m_cellDefault.GetFontObject()->GetSafeHandle();
}

#ifndef _WIN32_WCE_NO_CURSOR
bool HGridCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    if (nHitTest == HTCLIENT)
    {
        switch (m_MouseMode)
        {
        case MOUSE_OVER_COL_DIVIDE:
            SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
            break;
        case MOUSE_OVER_ROW_DIVIDE:
            SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));
            break;
#ifndef GRIDCONTROL_NO_DRAGDROP
        case MOUSE_DRAGGING:
            break;
#endif
        default:
            if (!GetVirtualMode())
            {
                CPoint pt(GetMessagePos());
                ScreenToClient(&pt);
                HCellID cell = cellFromPt(pt);
                if (isValid(cell))
                {
                    CGridCellBase* pCell = GetCell(cell.row, cell.col);
                    if (pCell)
                        return pCell->OnSetCursor();
                }
            }

            SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
        }
        return true;
    }

    return CWnd::OnSetCursor(pWnd, nHitTest, message);
}
#endif
*/
////////////////////////////////////////////////////////////////////////////////////
// Row/Column count functions
bool HGridCtrl::setRowCount(int nRows)
{
    bool bResult = true;

    Q_ASSERT(nRows >= 0);
    if (nRows == rowCount())
        return bResult;

    // Force recalculation
    m_idTopLeftCell.col = -1;

    if (nRows < m_nFixedRows)
        m_nFixedRows = nRows;

    if (m_idCurrentCell.row >= nRows)
        setFocusCell(-1, - 1);

    int addedRows = nRows - rowCount();

    //如果现在设置的行数要小于原来的行数，必须要删除一些行。
    if (addedRows < 0)
    {
        if (!isVirtualMode())
        {
            //从nRows到最后一行都要删除，删除行，对应每一列都要删除一次
            for (int row = nRows; row < m_nRows; row++)
            {
                // Delete cells
                for (int col = 0; col < m_nCols; col++)
                    destroyCell(row, col);//---huangw

                // Delete rows
                GRID_ROW* pRow = m_RowData.takeAt(row);
                if (pRow)
                    delete pRow;
            }
        }
        m_nRows = nRows;
    }

    try
    {
        m_arRowHeights.resize(nRows);

        //如果是打印模式
        if (isVirtualMode())
        {
            m_nRows = nRows;
            if (addedRows > 0)
            {
                int startRow = nRows - addedRows;
                for (int row = startRow; row < nRows; row++)
                    m_arRowHeights[row] = m_cellDefault.height();
            }
        }
        else
        {
            // 如果是增加行,就需要创建增加的行列，添加到存储列表后面
            if (addedRows > 0)
            {
                // initialize row heights and data
                int startRow = nRows - addedRows;
                for (int row = startRow; row < nRows; row++)
                {
                    m_arRowHeights[row] = m_cellDefault.height();

                    GRID_ROW *gridRow = new GRID_ROW;
                    m_RowData.insert(row,gridRow);
                    //m_RowData[row]->SetSize(m_nCols); //不要设置列
                    for (int col = 0; col < m_nCols; col++)
                    {
                        GRID_ROW* pRow = m_RowData.at(col);
                        if (pRow && !isVirtualMode())
                            pRow->insert(col, createCell(row, col));
                    }
                    m_nRows++;
                }
            }
        }
    }
    catch (exception& e)
    {
        e.what();
        bResult = false;
    }

    setModified();
    resetScrollBars();
    refresh();

    return bResult;
}

bool HGridCtrl::setFixedRowCount(int nFixedRows)
{
    if (m_nFixedRows == nFixedRows)
        return true;

    Q_ASSERT(nFixedRows >= 0);

    //resetSelectedRange();  --huangw

    // Force recalculation
    m_idTopLeftCell.col = -1;

    //如果固定行超过已有行，那么已有行设成固定行数。一般来说表格就只有1行1列是固定的
    if (nFixedRows > rowCount())
    {
        if (!setRowCount(nFixedRows))
            return false;
    }

    if (m_idCurrentCell.row < nFixedRows)
        setFocusCell(-1, - 1);

    if (!isVirtualMode())
    {
        if (nFixedRows > m_nFixedRows)
        {
            for (int i = m_nFixedRows; i < nFixedRows; i++)
                for (int j = 0; j < columnCount(); j++)
                {
                    setItemState(i, j, itemState(i, j) | GVIS_FIXED | GVIS_FIXEDROW);
                    setItemBkColour(i, j, QColor(QCLR_DEFAULT) );
                    setItemFgColour(i, j, QColor(QCLR_DEFAULT) );
                }
        }
        else
        {
            for (int i = nFixedRows; i < m_nFixedRows; i++)
            {
                int j;
                for (j = 0; j < fixedColumnCount(); j++)
                    setItemState(i, j, itemState(i, j) & ~GVIS_FIXEDROW );

                for (j = fixedColumnCount(); j < columnCount(); j++)
                {
                    setItemState(i, j, itemState(i, j) & ~(GVIS_FIXED | GVIS_FIXEDROW) );
                    setItemBkColour(i, j, QColor(QCLR_DEFAULT) );
                    setItemFgColour(i, j, QColor(QCLR_DEFAULT) );
                }
            }
        }
    }

    m_nFixedRows = nFixedRows;

    refresh();
    return true;
}

bool HGridCtrl::setFixedColumnCount(int nFixedCols)
{
    if (m_nFixedCols == nFixedCols)
        return true;

    Q_ASSERT(nFixedCols >= 0);

    if (nFixedCols > columnCount())
        if (!setColumnCount(nFixedCols))
            return false;

    if (m_idCurrentCell.col < nFixedCols)
        setFocusCell(-1, - 1);

    resetSelectedRange();
    // Force recalculation
    m_idTopLeftCell.col = -1;

    //实际上将表格的行列中抽取fixedrow,fixedcol为固定行列
    if (!isVirtualMode())
    {
        if (nFixedCols > m_nFixedCols)
        {
            for (int i = 0; i < rowCount(); i++)
                for (int j = m_nFixedCols; j < nFixedCols; j++)
                {
                    setItemState(i, j, itemState(i, j) | GVIS_FIXED | GVIS_FIXEDCOL);
                    setItemBkColour(i, j, QColor(QCLR_DEFAULT) );
                    setItemFgColour(i, j, QColor(QCLR_DEFAULT) );
                }
        }
        else
        {
			{ // Scope limit i,j
                for (int i = 0; i < fixedRowCount(); i++)
		            for (int j = nFixedCols; j < m_nFixedCols; j++)
                        setItemState(i, j, itemState(i, j) & ~GVIS_FIXEDCOL );
			}
			{// Scope limit i,j
                for (int i = fixedRowCount(); i < rowCount(); i++)
		            for (int j = nFixedCols; j < m_nFixedCols; j++)
			        {
                        setItemState(i, j, itemState(i, j) & ~(GVIS_FIXED | GVIS_FIXEDCOL) );
                        setItemBkColour(i, j, QColor(QCLR_DEFAULT) );
                        setItemFgColour(i, j, QColor(QCLR_DEFAULT) );
	                }
			}
        }
    }
        
    m_nFixedCols = nFixedCols;
        
    refresh();
    return true;
}

bool HGridCtrl::setColumnCount(int nCols)
{
    bool bResult = true;

    Q_ASSERT(nCols >= 0);

    if (nCols == columnCount())
        return bResult;

    // Force recalculation
    m_idTopLeftCell.col = -1;

    if (nCols < m_nFixedCols)
        m_nFixedCols = nCols;

    if (m_idCurrentCell.col >= nCols)
        setFocusCell(-1, - 1);

    int addedCols = nCols - columnCount();

    // 同行一样 如果列数减少，需要删除多余的部分
    if (addedCols < 0 && !isVirtualMode())
    {
        for (int row = 0; row < m_nRows; row++)
            for (int col = nCols; col < columnCount(); col++)
                destroyCell(row, col);
    }

    try
    { 
        // 增加列
        m_arColWidths.resize(nCols);
        if (addedCols > 0)
        {
            //列宽
            int startCol = nCols - addedCols;
            for (int col = startCol; col < nCols; col++)
                m_arColWidths[col] = m_cellFixedColDef.width();
        
            // 创建列
            if (!isVirtualMode())
            {
                for (int row = 0; row < m_nRows; row++)
                    for (int col = startCol; col < nCols; col++)
                    {
                        GRID_ROW* pRow = m_RowData[row];
                        if (pRow)
                            pRow->insert(col, createCell(row, col));
                    }
            }
        }
    }
    catch (exception &e)
    {
        e.what();
        bResult = false;
    }

    m_nCols = nCols;

    setModified();
    resetScrollBars();
    refresh();
    return bResult;
}


// Insert a column at a given position, or add to end of columns (if nColumn = -1)
int HGridCtrl::insertColumn(const QString& strHeading, uint nFormat,int nColumn  )
{
    if (nColumn >= 0 && nColumn < m_nFixedCols)
    {
        Q_ASSERT(false);
        return -1;
    }

    // If the insertion is for a specific column, check it's within range.
    if (nColumn >= 0 && nColumn > columnCount())
        return -1;

    // Force recalculation
    m_idTopLeftCell.col = -1;

    resetSelectedRange();

    // Gotta be able to at least _see_ some of the column.
    if (m_nRows < 1)
        setRowCount(1);

    // Allow the user to insert after the last of the columns, but process it as a
    // "-1" column, meaning it gets flaged as being the last column, and not a regular
    // "insert" routine.
    if (nColumn == columnCount())
        nColumn = -1;

    try
    {
        if (nColumn < 0)
        {
            nColumn = m_nCols;
            m_arColWidths.append(0);
            if (!isVirtualMode())
            {
                for (int row = 0; row < m_nRows; row++)
                {
                    GRID_ROW* pRow = m_RowData[row];
                    if (!pRow)
                        return -1;
                    pRow->append(createCell(row, nColumn));
                }
            }
        } 
        else
        {
            m_arColWidths.insert(nColumn, (int)0);
            if (!isVirtualMode())
            {
                for (int row = 0; row < m_nRows; row++) 
                {
                    GRID_ROW* pRow = m_RowData[row];
                    if (!pRow)
                        return -1;
                    pRow->insert(nColumn, createCell(row, nColumn));
                }
            }
        }
    }
    catch (exception e)
    {
        e.what();
        return false;
    }

    m_nCols++;
    
    // Initialise column data
    setItemText(0, nColumn, strHeading);
    for (int row = 0; row < m_nRows; row++) 
        setItemFormat(row, nColumn, nFormat);
    
    // initialized column width
    m_arColWidths[nColumn] = textExtent(0, nColumn, strHeading).width();
    
    if (m_idCurrentCell.col != -1 && nColumn < m_idCurrentCell.col)
        m_idCurrentCell.col++;
    
    //ResetScrollBars();//刷新滚动条
    setModified();

    return nColumn;
}

// Insert a row at a given position, or add to end of rows (if nRow = -1)
int HGridCtrl::insertRow(const QString& strHeading, int nRow )
{
    if (nRow >= 0 && nRow < m_nFixedRows)
    {
        Q_ASSERT(false);
        return -1;
    }

    // If the insertion is for a specific row, check it's within range.
    if (nRow >= 0 && nRow >= rowCount())
        return -1;

    // Force recalculation
    m_idTopLeftCell.col = -1;

    resetSelectedRange();

    // Gotta be able to at least _see_ some of the row.
    if (m_nCols < 1)
        setColumnCount(1);

    try
    {
        // Adding a row to the bottom
        if (nRow < 0)
        {
            nRow = m_nRows;
            m_arRowHeights.append(0);
            if (!isVirtualMode())
                m_RowData.append(new GRID_ROW);
        }
        else
        {
            m_arRowHeights.insert(nRow, (int)0);
            if (!isVirtualMode())
                m_RowData.insert(nRow, new GRID_ROW);
        }

        //if (!isVirtualMode())
        //    m_RowData[nRow]->(m_nCols);
    }
    catch (exception e)
    {
        e.what();
        return false;
    }

    m_nRows++;

    // Initialise cell data
    if (!isVirtualMode())
    {
        for (int col = 0; col < m_nCols; col++)
        {
            GRID_ROW* pRow = m_RowData[nRow];
            if (!pRow)
                return -1;
            pRow->insert(col, createCell(nRow, col));
        }
    }

    // Set row title
    setItemText(nRow, 0, strHeading);

    // initialized row height
    if (!strHeading.isEmpty())
        m_arRowHeights[nRow] = textExtent(nRow, 0, strHeading).height();
    else
        m_arRowHeights[nRow] = m_cellFixedRowDef.height();

    if (m_idCurrentCell.row != -1 && nRow < m_idCurrentCell.row)
        m_idCurrentCell.row++;

    //ResetScrollBars(); //刷新滚动条

    setModified();

    return nRow;
}
/*
///////////////////////////////////////////////////////////////////////////////
// Cell creation stuff

bool HGridCtrl::SetCellType(int nRow, int nCol, CRuntimeClass* pRuntimeClass)
{
    if (GetVirtualMode())
        return false;

    Q_ASSERT(isValid(nRow, nCol));
    if (!isValid(nRow, nCol))
        return false;

    if (!pRuntimeClass->IsDerivedFrom(RUNTIME_CLASS(CGridCellBase)))
    {
        Q_ASSERT( false);
        return false;
    }

    CGridCellBase* pNewCell = (CGridCellBase*) pRuntimeClass->CreateObject();

    CGridCellBase* pCurrCell = GetCell(nRow, nCol);
    if (pCurrCell)
        *pNewCell = *pCurrCell;

    SetCell(nRow, nCol, pNewCell);
    delete pCurrCell;

    return true;
}

bool HGridCtrl::SetDefaultCellType( CRuntimeClass* pRuntimeClass)
{
    Q_ASSERT( pRuntimeClass != NULL );
    if (!pRuntimeClass->IsDerivedFrom(RUNTIME_CLASS(CGridCellBase)))
    {
        Q_ASSERT( false);
        return false;
    }
    m_pRtcDefault = pRuntimeClass;
    return true;
}
*/
// Creates a new grid cell and performs any necessary initialisation
 HGridCellBase* HGridCtrl::createCell(int nRow, int nCol)
{
    Q_ASSERT(!isVirtualMode());

    //if (!m_pRtcDefault || !m_pRtcDefault->IsDerivedFrom(RUNTIME_CLASS(CGridCellBase)))
    //{
     //   Q_ASSERT( false);
     //   return NULL;
    //}
    //CGridCellBase* pCell = (CGridCellBase*) m_pRtcDefault->CreateObject();
    HGridCellBase* pCell = new HGridCell();
    if (!pCell)
        return NULL;

    pCell->setGrid(this);
    pCell->setCoords(nRow, nCol);

    if (nCol < m_nFixedCols)
        pCell->setState(pCell->state() | GVIS_FIXED | GVIS_FIXEDCOL);
    if (nRow < m_nFixedRows)
        pCell->setState(pCell->state() | GVIS_FIXED | GVIS_FIXEDROW);
    
    pCell->setFormat(pCell->defaultCell()->format());
    return pCell;
}

// Performs any cell cleanup necessary to maintain grid integrity
void HGridCtrl::destroyCell(int nRow, int nCol)
{
    // Should NEVER get here in virtual mode.
    Q_ASSERT(!isVirtualMode());

    // Set the cells state to 0. If the cell is selected, this
    // will remove the cell from the selected list.
    setItemState(nRow, nCol, 0);
    delete getCell(nRow, nCol);
}

bool HGridCtrl::deleteColumn(int nColumn)
{
    if (nColumn < 0 || nColumn >= columnCount())
        return false;

    resetSelectedRange();

    if (!isVirtualMode())
    {
        for (int row = 0; row < rowCount(); row++)
        {
            GRID_ROW* pRow = m_RowData[row];
            if (!pRow)
                return false;

            //pRow->removeAt(nColumn);
            destroyCell(row, nColumn);
        
            pRow->removeAt(nColumn);
            //注意以上做法如果不对就将removeAt移到上面
        }
    }
    m_arColWidths.removeAt(nColumn);
    m_nCols--;
    if (nColumn < m_nFixedCols)
        m_nFixedCols--;
    
    if (nColumn == m_idCurrentCell.col)
        m_idCurrentCell.row = m_idCurrentCell.col = -1;
    else if (nColumn < m_idCurrentCell.col)
        m_idCurrentCell.col--;
    
    //ResetScrollBars();//刷新滚动条 --huangw

    setModified();

    return true;
}

bool HGridCtrl::deleteRow(int nRow)
{
    if (nRow < 0 || nRow >= rowCount())
        return false;

    resetSelectedRange();

    if (!isVirtualMode())
    {
        GRID_ROW* pRow = m_RowData[nRow];
        if (!pRow)
            return false;

        for (int col = 0; col < columnCount(); col++)
            destroyCell(nRow, col);

        delete pRow;
        m_RowData.removeAt(nRow);
        //注意 如果以上不对将removeAt移到上面
    }

    m_arRowHeights.removeAt(nRow);

    m_nRows--;
    if (nRow < m_nFixedRows)
        m_nFixedRows--;
    
    if (nRow == m_idCurrentCell.row)
        m_idCurrentCell.row = m_idCurrentCell.col = -1;
    else if (nRow < m_idCurrentCell.row)
        m_idCurrentCell.row--;
    
    //ResetScrollBars();//刷新滚动条 huangw

    setModified();
    
    return true;
}

// Handy function that removes all non-fixed rows
bool HGridCtrl::deleteNonFixedRows()
{
    resetSelectedRange();

    int nFixed = fixedRowCount();
    int nCount = rowCount();

    // Delete all data rows
    for (int nRow = nCount; nRow >= nFixed; nRow--)
        deleteRow(nRow);

    return true;
}

// Removes all rows, columns and data from the grid.
bool HGridCtrl::deleteAllItems()
{
    resetSelectedRange();

    m_arColWidths.clear();
    m_arRowHeights.clear();

    // Delete all cells in the grid
    if (!isVirtualMode())
    {
        for (int row = 0; row < m_nRows; row++)
        {
            for (int col = 0; col < m_nCols; col++)
                destroyCell(row, col);

            GRID_ROW* pRow = m_RowData[row];
            delete pRow;
        }

        // Remove all rows
        m_RowData.clear();
    }

    m_idCurrentCell.row = m_idCurrentCell.col = -1;
    m_nRows = m_nFixedRows = m_nCols = m_nFixedCols = 0;

    //ResetScrollBars();//刷新滚动条 huangw

    setModified();

    return true;
}
/*
void HGridCtrl::AutoFill()
{
    if (!::IsWindow(m_hWnd))
        return;

    CRect rect;
    GetClientRect(rect);

    setColumnCount(rect.Width() / m_cellDefault.GetWidth() + 1);
    SetRowCount(rect.Height() / m_cellDefault.GetHeight() + 1);
    SetFixedRowCount(1);
    SetFixedColumnCount(1);
    ExpandToFit();
}
*/
/////////////////////////////////////////////////////////////////////////////
// CGridCtrl data functions
/*
// Set CListCtrl::GetNextItem for details
HCellID HGridCtrl::GetNextItem(HCellID& cell, int nFlags) const
{
    if ((nFlags & GVNI_ALL) == GVNI_ALL)
    {    // GVNI_ALL Search whole Grid beginning from cell
        //          First row (cell.row) -- ONLY Columns to the right of cell
        //          following rows       -- ALL  Columns
        int row = cell.row , col = cell.col + 1;
        if (row <= 0)
            row = GetFixedRowCount();
        for (; row < GetRowCount(); row++)
        {
            if (col <= 0)
                col = GetFixedColumnCount();
            for (; col < GetColumnCount(); col++)
            {
                int nState = GetItemState(row, col);
                if ((nFlags & GVNI_DROPHILITED && nState & GVIS_DROPHILITED) || 
                    (nFlags & GVNI_FOCUSED     && nState & GVIS_FOCUSED)     ||
                    (nFlags & GVNI_SELECTED    && nState & GVIS_SELECTED)    ||
                    (nFlags & GVNI_READONLY    && nState & GVIS_READONLY)    ||
                    (nFlags & GVNI_FIXED       && nState & GVIS_FIXED)       ||
                    (nFlags & GVNI_MODIFIED    && nState & GVIS_MODIFIED))
                    return HCellID(row, col);
            }
            // go to First Column
            col = GetFixedColumnCount();
        }
    }
    else if ((nFlags & GVNI_BELOW) == GVNI_BELOW && 
             (nFlags & GVNI_TORIGHT) == GVNI_TORIGHT)
    {   // GVNI_AREA Search Grid beginning from cell to Lower-Right of Grid
        //           Only rows starting with  cell.row and below
        //           All rows   -- ONLY Columns to the right of cell
        int row = cell.row;
        if (row <= 0)
            row = GetFixedRowCount();
        for (; row < GetRowCount(); row++)
        {
            int col = cell.col + 1;
            if (col <= 0)
                col = GetFixedColumnCount();
            for (; col < GetColumnCount(); col++) 
            {
                int nState = GetItemState(row, col);
                if ((nFlags & GVNI_DROPHILITED && nState & GVIS_DROPHILITED) || 
                    (nFlags & GVNI_FOCUSED     && nState & GVIS_FOCUSED)     ||
                    (nFlags & GVNI_SELECTED    && nState & GVIS_SELECTED)    ||
                    (nFlags & GVNI_READONLY    && nState & GVIS_READONLY)    ||
                    (nFlags & GVNI_FIXED       && nState & GVIS_FIXED)       ||
                    (nFlags & GVNI_MODIFIED    && nState & GVIS_MODIFIED))
                    return HCellID(row, col);
            }
        }
    }
    else if ((nFlags & GVNI_ABOVE) == GVNI_ABOVE) 
    {
        for (int row = cell.row - 1; row >= GetFixedRowCount(); row--) 
        {
            int nState = GetItemState(row, cell.col);
            if ((nFlags & GVNI_DROPHILITED && nState & GVIS_DROPHILITED) || 
                (nFlags & GVNI_FOCUSED     && nState & GVIS_FOCUSED)     ||
                (nFlags & GVNI_SELECTED    && nState & GVIS_SELECTED)    ||
                (nFlags & GVNI_READONLY    && nState & GVIS_READONLY)    ||
                (nFlags & GVNI_FIXED       && nState & GVIS_FIXED)       ||
                (nFlags & GVNI_MODIFIED    && nState & GVIS_MODIFIED))
                return HCellID(row, cell.col);
        }
    }
    else if ((nFlags & GVNI_BELOW) == GVNI_BELOW)
    {
        for (int row = cell.row + 1; row < GetRowCount(); row++) 
        {
            int nState = GetItemState(row, cell.col);
            if ((nFlags & GVNI_DROPHILITED && nState & GVIS_DROPHILITED) || 
                (nFlags & GVNI_FOCUSED     && nState & GVIS_FOCUSED)     ||
                (nFlags & GVNI_SELECTED    && nState & GVIS_SELECTED)    ||
                (nFlags & GVNI_READONLY    && nState & GVIS_READONLY)    ||
                (nFlags & GVNI_FIXED       && nState & GVIS_FIXED)       ||
                (nFlags & GVNI_MODIFIED    && nState & GVIS_MODIFIED))
                return HCellID(row, cell.col);
        }
    } 
    else if ((nFlags & GVNI_TOLEFT) == GVNI_TOLEFT)
    {
        for (int col = cell.col - 1; col >= GetFixedColumnCount(); col--) 
        {
            int nState = GetItemState(cell.row, col);
            if ((nFlags & GVNI_DROPHILITED && nState & GVIS_DROPHILITED) || 
                (nFlags & GVNI_FOCUSED     && nState & GVIS_FOCUSED)     ||
                (nFlags & GVNI_SELECTED    && nState & GVIS_SELECTED)    ||
                (nFlags & GVNI_READONLY    && nState & GVIS_READONLY)    ||
                (nFlags & GVNI_FIXED       && nState & GVIS_FIXED)       ||
                (nFlags & GVNI_MODIFIED    && nState & GVIS_MODIFIED))
                return HCellID(cell.row, col);
        }
    }
    else if ((nFlags & GVNI_TORIGHT) == GVNI_TORIGHT)
    {
        for (int col = cell.col + 1; col < GetColumnCount(); col++) 
        {
            int nState = GetItemState(cell.row, col);
            if ((nFlags & GVNI_DROPHILITED && nState & GVIS_DROPHILITED) || 
                (nFlags & GVNI_FOCUSED     && nState & GVIS_FOCUSED)     ||
                (nFlags & GVNI_SELECTED    && nState & GVIS_SELECTED)    ||
                (nFlags & GVNI_READONLY    && nState & GVIS_READONLY)    ||
                (nFlags & GVNI_FIXED       && nState & GVIS_FIXED)       ||
                (nFlags & GVNI_MODIFIED    && nState & GVIS_MODIFIED))
                return HCellID(cell.row, col);
        }
    }
    
    return HCellID(-1, -1);
}

// Sorts on a given column using the cell text
bool HGridCtrl::SortTextItems(int nCol, bool bAscending, LPARAM data  )
{
    setSortColumn(nCol);
    SetSortAscending(bAscending);
    resetSelectedRange();
    SetFocusCell(-1, - 1);
    return HGridCtrl::SortItems(pfnCellTextCompare, nCol, bAscending, data);
}

void HGridCtrl::SetCompareFunction(PFNLVCOMPARE pfnCompare)
{
	m_pfnCompare = pfnCompare;
}

// Sorts on a given column using the cell text and using the specified comparison
// function
bool HGridCtrl::SortItems(int nCol, bool bAscending, LPARAM data )
{
    setSortColumn(nCol);
    SetSortAscending(bAscending);
    resetSelectedRange();
    SetFocusCell(-1, - 1);

	if (m_pfnCompare == NULL)
        return HGridCtrl::SortItems(pfnCellTextCompare, nCol, bAscending, data);
	else
        return HGridCtrl::SortItems(m_pfnCompare, nCol, bAscending, data);
}

// Sorts on a given column using the supplied compare function (see CListCtrl::SortItems)
bool HGridCtrl::SortItems(PFNLVCOMPARE pfnCompare, int nCol, bool bAscending,
                          LPARAM data )
{
    setSortColumn(nCol);
    SetSortAscending(bAscending);
    resetSelectedRange();
    SetFocusCell(-1, -1);
    return SortItems(pfnCompare, nCol, bAscending, data, GetFixedRowCount(), -1);
}

int CALLBACK HGridCtrl::pfnCellTextCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	UNUSED_ALWAYS(lParamSort);

	CGridCellBase* pCell1 = (CGridCellBase*) lParam1;
	CGridCellBase* pCell2 = (CGridCellBase*) lParam2;
	if (!pCell1 || !pCell2) return 0;

	return _tcscmp(pCell1->GetText(), pCell2->GetText());
}

int CALLBACK HGridCtrl::pfnCellNumericCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	UNUSED_ALWAYS(lParamSort);

	CGridCellBase* pCell1 = (CGridCellBase*) lParam1;
	CGridCellBase* pCell2 = (CGridCellBase*) lParam2;
	if (!pCell1 || !pCell2) return 0;

	int nValue1 = _ttol(pCell1->GetText());
	int nValue2 = _ttol(pCell2->GetText());

	if (nValue1 < nValue2)
		return -1;
	else if (nValue1 == nValue2)
		return 0;
	else
		return 1;
}

// private recursive sort implementation
bool HGridCtrl::SortItems(PFNLVCOMPARE pfnCompare, int nCol, bool bAscending, LPARAM data,
                          int low, int high)
{
    if (nCol >= GetColumnCount())
        return false;

    if (high == -1)
        high = GetRowCount() - 1;

    int lo = low;
    int hi = high;
    
    if (hi <= lo)
        return false;
    
    //LPARAM midItem = GetItemData((lo + hi)/2, nCol);
	LPARAM pMidCell = (LPARAM) GetCell((lo + hi)/2, nCol);
    
    // loop through the list until indices cross
    while (lo <= hi)
    {
        // Find the first element that is greater than or equal to the partition 
        // element starting from the left Index.
        if (bAscending)
            while (lo < high  && pfnCompare( (LPARAM)GetCell(lo, nCol), (LPARAM) pMidCell, data) < 0)
                ++lo;
        else
            while (lo < high && pfnCompare((LPARAM)GetCell(lo, nCol), pMidCell, data) > 0)
				++lo;
                
		// Find an element that is smaller than or equal to  the partition 
		// element starting from the right Index.
		if (bAscending)
			while (hi > low && pfnCompare((LPARAM)GetCell(hi, nCol), pMidCell, data) > 0)
				--hi;
		else
			while (hi > low && pfnCompare((LPARAM)GetCell(hi, nCol), pMidCell, data) < 0)
				--hi;
                        
        // If the indexes have not crossed, swap if the items are not equal
        if (lo <= hi)
        {
			// swap only if the items are not equal
            if (pfnCompare((LPARAM)GetCell(lo, nCol), (LPARAM)GetCell(hi, nCol), data) != 0)
            {
				for (int col = 0; col < GetColumnCount(); col++)
                {
					CGridCellBase *pCell = GetCell(lo, col);
                    SetCell(lo, col, GetCell(hi, col));
                    SetCell(hi, col, pCell);
                }
                UINT nRowHeight = m_arRowHeights[lo];
                m_arRowHeights[lo] = m_arRowHeights[hi];
                m_arRowHeights[hi] = nRowHeight;
            }
                            
            ++lo;
            --hi;
         }
    }
    
    // If the right index has not reached the left side of array
    // must now sort the left partition.
    if (low < hi)
        SortItems(pfnCompare, nCol, bAscending, data, low, hi);
    
    // If the left index has not reached the right side of array
    // must now sort the right partition.
    if (lo < high)
        SortItems(pfnCompare, nCol, bAscending, data, lo, high);
    
    return true;
}
*/
/////////////////////////////////////////////////////////////////////////////
// CGridCtrl data functions

bool HGridCtrl::setItem(const GV_ITEM* pItem)
{
    if (!pItem || isVirtualMode())
        return false;

    HGridCellBase* pCell = getCell(pItem->row, pItem->col);
    if (!pCell)
        return false;

    setModified(true, pItem->row, pItem->col);

    if (pItem->mask & GVIF_TEXT)
        pCell->setText(pItem->strText);
    if (pItem->mask & GVIF_PARAM)
        pCell->setData(pItem->lParam);
    if (pItem->mask & GVIF_IMAGE)
        pCell->setImage(pItem->iImage);
    if (pItem->mask & GVIF_STATE)
        setItemState(pItem->row, pItem->col, pItem->nState);
    if (pItem->mask & GVIF_FORMAT)
        pCell->setFormat(pItem->nFormat);
    if (pItem->mask & GVIF_BKCLR)
        pCell->setBackClr(pItem->crBkClr);
    if (pItem->mask & GVIF_FGCLR)
        pCell->setTextClr(pItem->crFgClr);
    if (pItem->mask & GVIF_FONT)
        pCell->setFont(pItem->lfFont);
    if( pItem->mask & GVIF_MARGIN)
        pCell->setMargin( pItem->nMargin);
    
    return true;
}

bool HGridCtrl::item(GV_ITEM* pItem)
{
    if (!pItem)
        return false;
    HGridCellBase* pCell = getCell(pItem->row, pItem->col);
    if (!pCell)
        return false;

    if (pItem->mask & GVIF_TEXT)
        pItem->strText = itemText(pItem->row, pItem->col);
    if (pItem->mask & GVIF_PARAM)
        pItem->lParam  = pCell->data();
    if (pItem->mask & GVIF_IMAGE)
        pItem->iImage  = pCell->image();
    if (pItem->mask & GVIF_STATE)
        pItem->nState  = pCell->state();
    if (pItem->mask & GVIF_FORMAT)
        pItem->nFormat = pCell->format();
    if (pItem->mask & GVIF_BKCLR)
        pItem->crBkClr = pCell->backClr();
    if (pItem->mask & GVIF_FGCLR)
        pItem->crFgClr = pCell->textClr();
    if (pItem->mask & GVIF_FONT)
        pItem->lfFont  = pCell->font();
    if( pItem->mask & GVIF_MARGIN)
        pItem->nMargin = pCell->margin();

    return true;
}

bool HGridCtrl::setItemText(int nRow, int nCol, const QString& str)
{
    if (isVirtualMode())
        return false;

    HGridCellBase* pCell = getCell(nRow, nCol);
    if (!pCell)
        return false;

    pCell->setText(str);

    setModified(true, nRow, nCol);
    return true;
}

// virtual
QString HGridCtrl::itemText(int nRow, int nCol) const
{
    if (nRow < 0 || nRow >= m_nRows || nCol < 0 || nCol >= m_nCols)
        return ("");

    HGridCellBase* pCell = getCell(nRow, nCol);
    Q_ASSERT(pCell);
    if (!pCell)
        return ("");

    return pCell->text();
}
/*
#if !defined(_WIN32_WCE) || (_WIN32_WCE >= 210)
// EFW - 06/13/99 - Added to support printf-style formatting codes
bool HGridCtrl::SetItemTextFmt(int nRow, int nCol, LPCTSTR szFmt, ...)
{
    if (GetVirtualMode())
        return false;

    CString strText;

    va_list argptr;

    CGridCellBase* pCell = GetCell(nRow, nCol);
    if (!pCell)
        return false;

    // Format the message text
    va_start(argptr, szFmt);
    strText.FormatV(szFmt, argptr);
    va_end(argptr);

    pCell->SetText(strText);

    setModified(true, nRow, nCol);
    return true;
}

// EFW - 06/13/99 - Added to support string resource ID.  Supports
// a variable argument list too.
bool HGridCtrl::SetItemTextFmtID(int nRow, int nCol, UINT nID, ...)
{
    if (GetVirtualMode())
        return false;

    CString strFmt, strText;
    va_list argptr;

    CGridCellBase* pCell = GetCell(nRow, nCol);
    if (!pCell)
        return false;

    // Format the message text
    va_start(argptr, nID);
    VERIFY(strFmt.LoadString(nID));
    strText.FormatV(strFmt, argptr);
    va_end(argptr);

    pCell->SetText(strText);

    setModified(true, nRow, nCol);
    return true;
}
#endif
*/
bool HGridCtrl::setItemData(int nRow, int nCol, const QVariant& lParam)
{
    if (isVirtualMode())
        return false;

    HGridCellBase* pCell = getCell(nRow, nCol);
    if (!pCell)
        return false;

    pCell->setData(lParam);
    setModified(true, nRow, nCol);
    return true;
}

QVariant HGridCtrl::itemData(int nRow, int nCol) const
{
    HGridCellBase* pCell = getCell(nRow, nCol);
    if (!pCell)
        return QVariant();

    return pCell->data();
}

bool HGridCtrl::setItemImage(int nRow, int nCol, int iImage)
{
    if (isVirtualMode())
        return false;

    HGridCellBase* pCell = getCell(nRow, nCol);
    if (!pCell)
        return false;

    pCell->setImage(iImage);
    setModified(true, nRow, nCol);
    return true;
}

int HGridCtrl::itemImage(int nRow, int nCol) const
{
    HGridCellBase* pCell = getCell(nRow, nCol);
    Q_ASSERT(pCell);
    if (!pCell)
        return -1;

    return pCell->image();
}

bool HGridCtrl::setItemState(int nRow, int nCol, uint state)
{
    bool bSelected = isCellSelected(nRow, nCol);

    // If the cell is being unselected, remove it from the selected list
    if (bSelected && !(state & GVIS_SELECTED))
    {
        HCellID cell;
        quint32 key = QMAKELONG(nRow, nCol);
        cell = m_SelectedCellMap.value(key);
        if (cell.isValid())
            m_SelectedCellMap.remove(key);
    }

    // If cell is being selected, add it to the list of selected cells
    //如果单元格未被选中同时正在选中此单元格，就加入到选择单元格列表中
    else if (!bSelected && (state & GVIS_SELECTED))
    {
        HCellID cell(nRow, nCol);
        m_SelectedCellMap.insert(QMAKELONG(nRow, nCol), cell);
    }

    if (isVirtualMode())
        return false;

    HGridCellBase* pCell = getCell(nRow, nCol);
    Q_ASSERT(pCell);
    if (!pCell)
        return false;

    // Set the cell's state
    //单元格的state就是存储单元格的状态信息
    pCell->setState(state);
    return true;
}

uint HGridCtrl::itemState(int nRow, int nCol) const
{
    HGridCellBase* pCell = getCell(nRow, nCol);
    Q_ASSERT(pCell);
    if (!pCell)
        return 0;

    return pCell->state();
}

bool HGridCtrl::setItemFormat(int nRow, int nCol, uint nFormat)
{
    if (isVirtualMode())
        return false;

    HGridCellBase* pCell = getCell(nRow, nCol);
    Q_ASSERT(pCell);
    if (!pCell)
        return false;

    pCell->setFormat(nFormat);
    return true;
}

uint HGridCtrl::itemFormat(int nRow, int nCol) const
{
    HGridCellBase* pCell = getCell(nRow, nCol);
    Q_ASSERT(pCell);
    if (!pCell)
        return 0;

    return pCell->format();
}

bool HGridCtrl::setItemBkColour(int nRow, int nCol, QColor& cr )
{
    if (isVirtualMode())
        return false;

    HGridCellBase* pCell = getCell(nRow, nCol);
    Q_ASSERT(pCell);
    if (!pCell)
        return false;

    pCell->setBackClr(cr);
    return true;
}

QColor HGridCtrl::itemBkColour(int nRow, int nCol) const
{
    HGridCellBase* pCell = getCell(nRow, nCol);
    Q_ASSERT(pCell);
    if (!pCell)
        return QColor();

    return pCell->backClr();
}

bool HGridCtrl::setItemFgColour(int nRow, int nCol, QColor& cr )
{
    if (isVirtualMode())
        return false;

    HGridCellBase* pCell = getCell(nRow, nCol);
    Q_ASSERT(pCell);
    if (!pCell)
        return false;
    
    pCell->setTextClr(cr);
    return true;
}

QColor HGridCtrl::itemFgColour(int nRow, int nCol) const
{
    HGridCellBase* pCell = getCell(nRow, nCol);
    Q_ASSERT(pCell);
    if (!pCell)
        return QColor();
    
    return pCell->textClr();
}


bool HGridCtrl::setItemFont(int nRow, int nCol, const QFont& lf)
{
    if (isVirtualMode())
        return false;

    HGridCellBase* pCell = getCell(nRow, nCol);
    Q_ASSERT(pCell);
    if (!pCell)
        return false;
    
    pCell->setFont(lf);
    
    return true;
}

QFont HGridCtrl::itemFont(int nRow, int nCol)
{
    HGridCellBase* pCell = getCell(nRow, nCol);
    Q_ASSERT(pCell);
    if (!pCell) 
        return defaultCell(nRow < fixedRowCount(), nCol < fixedColumnCount())->font();
    
    return pCell->font();
}

bool HGridCtrl::isItemEditing(int nRow, int nCol)
{
    HGridCellBase* pCell = getCell(nRow, nCol);
    Q_ASSERT(pCell);
    if (!pCell)
        return false;

    return pCell->isEditing();
}

////////////////////////////////////////////////////////////////////////////////////
// Row/Column size functions
//好像是所有列的宽度之和
long HGridCtrl::virtualWidth() const
{
    long lVirtualWidth = 0;
    int iColCount = columnCount();
    for (int i = 0; i < iColCount; i++)
        lVirtualWidth += m_arColWidths[i];

    return lVirtualWidth;
}

//所有行的高度之和
long HGridCtrl::virtualHeight() const
{
    long lVirtualHeight = 0;
    int iRowCount = rowCount();
    for (int i = 0; i < iRowCount; i++)
        lVirtualHeight += m_arRowHeights[i];

    return lVirtualHeight;
}

int HGridCtrl::rowHeight(int nRow) const
{
    Q_ASSERT(nRow >= 0 && nRow < m_nRows);
    if (nRow < 0 || nRow >= m_nRows)
        return -1;

    return m_arRowHeights[nRow];
}

int HGridCtrl::columnWidth(int nCol) const
{
    Q_ASSERT(nCol >= 0 && nCol < m_nCols);
    if (nCol < 0 || nCol >= m_nCols)
        return -1;

    return m_arColWidths[nCol];
}

bool HGridCtrl::setRowHeight(int nRow, int height)
{
    Q_ASSERT(nRow >= 0 && nRow < m_nRows && height >= 0);
    if (nRow < 0 || nRow >= m_nRows || height < 0)
        return false;

    m_arRowHeights[nRow] = height;
    //ResetScrollBars(); //--huangw

    return true;
}

bool HGridCtrl::setColumnWidth(int nCol, int width)
{
    Q_ASSERT(nCol >= 0 && nCol < m_nCols && width >= 0);
    if (nCol < 0 || nCol >= m_nCols || width < 0)
        return false;

    m_arColWidths[nCol] = width;
    //ResetScrollBars(); //--huangw

    return true;
}

//固定行列的高度和virtualWidth,virtualHeight的差不多都是和
int HGridCtrl::fixedRowHeight() const
{
    int nHeight = 0;
    for (int i = 0; i < m_nFixedRows; i++)
        nHeight += rowHeight(i);

    return nHeight;
}

int HGridCtrl::fixedColumnWidth() const
{
    int nWidth = 0;
    for (int i = 0; i < m_nFixedCols; i++)
        nWidth += columnWidth(i);

    return nWidth;
}

/*
bool HGridCtrl::AutoSizeColumn(int nCol, UINT nAutoSizeStyle ,
                               bool bResetScroll )
{
    Q_ASSERT(nCol >= 0 && nCol < m_nCols);
    if (nCol < 0 || nCol >= m_nCols)
        return false;

    //  Skip hidden columns when autosizing
    if( GetColumnWidth( nCol) <=0 )
        return false;

    CSize size;
    CDC* pDC = GetDC();
    if (!pDC)
        return false;

    int nWidth = 0;

    Q_ASSERT(GVS_DEFAULT <= nAutoSizeStyle && nAutoSizeStyle <= GVS_BOTH);
    if (nAutoSizeStyle == GVS_DEFAULT)
        nAutoSizeStyle = GetAutoSizeStyle();

    int nStartRow = (nAutoSizeStyle & GVS_HEADER)? 0 : GetFixedRowCount();
    int nEndRow   = (nAutoSizeStyle & GVS_DATA)? GetRowCount()-1 : GetFixedRowCount()-1;

    if (GetVirtualMode())
        SendCacheHintToParent(HCellRange(nStartRow, nCol, nEndRow, nCol));

    for (int nRow = nStartRow; nRow <= nEndRow; nRow++)
    {
        CGridCellBase* pCell = GetCell(nRow, nCol);
        if (pCell)
            size = pCell->GetCellExtent(pDC);
        if (size.cx > nWidth)
            nWidth = size.cx;
    }

    if (GetVirtualMode())
        SendCacheHintToParent(HCellRange(-1,-1,-1,-1));

    m_arColWidths[nCol] = nWidth;

    ReleaseDC(pDC);
    if (bResetScroll)
        ResetScrollBars();

    return true;
}

bool HGridCtrl::AutoSizeRow(int nRow, bool bResetScroll )
{
    Q_ASSERT(nRow >= 0 && nRow < m_nRows);
    if (nRow < 0 || nRow >= m_nRows)
        return false;

    //  Skip hidden rows when autosizing
    if( GetRowHeight( nRow) <=0 )
        return false;

    CSize size;
    CDC* pDC = GetDC();
    if (!pDC)
        return false;

    int nHeight = 0;
    int nNumColumns = GetColumnCount();

    if (GetVirtualMode())
        SendCacheHintToParent(HCellRange(nRow, 0, nRow, nNumColumns));

    for (int nCol = 0; nCol < nNumColumns; nCol++)
    {
        CGridCellBase* pCell = GetCell(nRow, nCol);
        if (pCell)
            size = pCell->GetCellExtent(pDC);
        if (size.cy > nHeight)
            nHeight = size.cy;
    }
    m_arRowHeights[nRow] = nHeight;

    if (GetVirtualMode())
        SendCacheHintToParent(HCellRange(-1,-1,-1,-1));

    ReleaseDC(pDC);
    if (bResetScroll)
        ResetScrollBars();

    return true;
}

void HGridCtrl::AutoSizeColumns(UINT nAutoSizeStyle )
{
    int nNumColumns = GetColumnCount();
    for (int nCol = 0; nCol < nNumColumns; nCol++)
    {
        //  Skip hidden columns when autosizing
        if( GetColumnWidth( nCol) > 0 )
            AutoSizeColumn(nCol, nAutoSizeStyle, false);
    }
    ResetScrollBars();
}

void HGridCtrl::AutoSizeRows()
{
    int nNumRows = GetRowCount();
    for (int nRow = 0; nRow < nNumRows; nRow++)
    {
        //  Skip hidden rows when autosizing
        if( GetRowHeight( nRow) > 0 )
            AutoSizeRow(nRow, false);
    }
    ResetScrollBars();
}

// sizes all rows and columns
// faster than calling both AutoSizeColumns() and AutoSizeRows()
void HGridCtrl::AutoSize(UINT nAutoSizeStyle )
{
    CDC* pDC = GetDC();
    if (!pDC)
        return;

    int nNumColumns = GetColumnCount();

    int nCol, nRow;

    Q_ASSERT(GVS_DEFAULT <= nAutoSizeStyle && nAutoSizeStyle <= GVS_BOTH);
    if (nAutoSizeStyle == GVS_DEFAULT)
        nAutoSizeStyle = GetAutoSizeStyle();

    int nStartRow = (nAutoSizeStyle & GVS_HEADER)? 0 : GetFixedRowCount();
    int nEndRow   = (nAutoSizeStyle & GVS_DATA)? GetRowCount()-1 : GetFixedRowCount()-1;

    if (GetVirtualMode())
        SendCacheHintToParent(HCellRange(nStartRow, 0, nEndRow, nNumColumns));

    // Row initialisation - only work on rows whose height is > 0
    for (nRow = nStartRow; nRow <= nEndRow; nRow++)
    {
        if( GetRowHeight( nRow) > 0 )
            m_arRowHeights[nRow] = 1;
    }

    CSize size;
    for (nCol = 0; nCol < nNumColumns; nCol++)
    {
        //  Don't size hidden columns or rows
        if( GetColumnWidth( nCol) > 0 )
        {
            // Skip columns that are hidden, but now initialize
            m_arColWidths[nCol] = 0;
            for (nRow = nStartRow; nRow <= nEndRow; nRow++)
            {
                if( GetRowHeight( nRow) > 0 )
                {
                    CGridCellBase* pCell = GetCell(nRow, nCol);
                    if (pCell)
                        size = pCell->GetCellExtent(pDC);
                    if (size.cx >(int) m_arColWidths[nCol])
                        m_arColWidths[nCol] = size.cx;
                    if (size.cy >(int) m_arRowHeights[nRow])
                        m_arRowHeights[nRow] = size.cy;
                }
            }
        }
    }

    if (GetVirtualMode())
        SendCacheHintToParent(HCellRange(-1,-1,-1,-1));

    ReleaseDC(pDC);

    ResetScrollBars();
    refresh();
}

// Expands the columns to fit the screen space. If bExpandFixed is false then fixed
// columns will not be affected
void HGridCtrl::ExpandColumnsToFit(bool bExpandFixed )
{
    if (bExpandFixed)
	{
		if (GetColumnCount() <= 0) return;
	} 
	else
	{
		if (GetColumnCount() <= GetFixedColumnCount()) return;
	}

    EnableScrollBars(SB_HORZ, false);

    CRect rect;
    GetClientRect(rect);

    int nFirstColumn = (bExpandFixed)? 0 : GetFixedColumnCount();

    int nNumColumnsAffected = 0;
    for (int col = nFirstColumn; col < GetColumnCount(); col++)
    {
        if (m_arColWidths[col] > 0)
            nNumColumnsAffected++;
    }

    if (nNumColumnsAffected <= 0)
        return;

    long virtualWidth = GetVirtualWidth();
    int nDifference = rect.Width() -(int) virtualWidth;
    int nColumnAdjustment = nDifference / nNumColumnsAffected;

    for (int col = nFirstColumn; col < GetColumnCount(); col++)
    {
        if (m_arColWidths[col] > 0)
            m_arColWidths[col] += nColumnAdjustment;    
    }

    if (nDifference > 0)
    {
        int leftOver = nDifference % nNumColumnsAffected;
        for (int nCount = 0, col = nFirstColumn; 
             (col < GetColumnCount()) && (nCount < leftOver); col++, nCount++)
        {
            if (m_arColWidths[col] > 0)
                m_arColWidths[col] += 1;
        }
    }
    else 
    {
        int leftOver = (-nDifference) % nNumColumnsAffected;
        for (int nCount = 0, col = nFirstColumn; 
             (col < GetColumnCount()) && (nCount < leftOver); col++, nCount++)
        {
            if (m_arColWidths[col] > 0)
                m_arColWidths[col] -= 1;
        }
    }

    refresh();

    ResetScrollBars();
}

void HGridCtrl::ExpandLastColumn()
{
    if (GetColumnCount() <= 0)
        return;

    // Search for last non-hidden column
    int nLastColumn = GetColumnCount()-1;
    while (m_arColWidths[nLastColumn] <= 0)
        nLastColumn--;

    if (nLastColumn <= 0)
        return;

    EnableScrollBars(SB_HORZ, false);

    CRect rect;
    GetClientRect(rect);

    long virtualWidth = GetVirtualWidth();
    int nDifference = rect.Width() -(int) virtualWidth;

    if (nDifference > 0)
    {
        //if (GetVirtualHeight() > rect.Height())
        //    nDifference -= GetSystemMetrics(SM_CXVSCROLL);

        m_arColWidths[ nLastColumn ] += nDifference;
        refresh();
    }

    ResetScrollBars();
}

// Expands the rows to fit the screen space. If bExpandFixed is false then fixed
// rows will not be affected
void HGridCtrl::ExpandRowsToFit(bool bExpandFixed )
{
    if (bExpandFixed)
	{
		if (GetRowCount() <= 0) return;
	} 
	else
	{
		if (GetRowCount() <= GetFixedRowCount()) return;
	}

    EnableScrollBars(SB_VERT, false);

    CRect rect;
    GetClientRect(rect);
    
    int nFirstRow = (bExpandFixed)? 0 : GetFixedRowCount();

    int nNumRowsAffected = 0;
    for (int row = nFirstRow; row < GetRowCount(); row++)
    {
        if (m_arRowHeights[row] > 0)
            nNumRowsAffected++;
    }

    if (nNumRowsAffected <= 0)
        return;

    long virtualHeight = GetVirtualHeight();
    int nDifference = rect.Height() -(int) virtualHeight;
    int nRowAdjustment = nDifference / nNumRowsAffected;
    
    for (int row = nFirstRow; row < GetRowCount(); row++)
    {
        if (m_arRowHeights[row] > 0)
            m_arRowHeights[row] += nRowAdjustment;    
    }
    
    if (nDifference > 0)
    {
        int leftOver = nDifference % nNumRowsAffected;
        for (int nCount = 0, row = nFirstRow; 
             (row < GetRowCount()) && (nCount < leftOver); row++, nCount++)
        {
            if (m_arRowHeights[row] > 0)
                m_arRowHeights[row] += 1;
        }
    } 
    else 
    {
        int leftOver = (-nDifference) % nNumRowsAffected;
        for (int nCount = 0, row = nFirstRow; 
             (row < GetRowCount()) && (nCount < leftOver); row++, nCount++)
        {
            if (m_arRowHeights[row] > 0)
                m_arRowHeights[row] -= 1;
        }
    }

    refresh();

    ResetScrollBars();
}

// Expands the cells to fit the screen space. If bExpandFixed is false then fixed
// cells  will not be affected
void HGridCtrl::ExpandToFit(bool bExpandFixed )
{
    ExpandColumnsToFit(bExpandFixed);   // This will remove any existing horz scrollbar
    ExpandRowsToFit(bExpandFixed);      // This will remove any existing vert scrollbar
    ExpandColumnsToFit(bExpandFixed);   // Just in case the first adjustment was with a vert
                                        // scrollbar in place
    refresh();
}

/////////////////////////////////////////////////////////////////////////////////////
// Attributes
*/
void HGridCtrl::setVirtualMode(bool bVirtual)
{
    deleteAllItems();
    m_bVirtualMode = bVirtual;

    // Force some defaults here.
    if (m_bVirtualMode)
    {
        setEditable(false);
        setHeaderSort(false);
        setAutoSizeStyle(GVS_HEADER);
        setFixedColumnSelection(false);
        setFixedRowSelection(false);
    }
}


void HGridCtrl::setGridLines(int nWhichLines )
{
    m_nGridLines = nWhichLines;
    refresh();
}

void HGridCtrl::setListMode(bool bEnableListMode )
{
    resetSelectedRange();
    setSortColumn(-1);
    m_bListMode = bEnableListMode;
    setFixedRowSelection(false);
    refresh();
}

//---huangw
void HGridCtrl::setSortColumn(int nCol)
{
    /*
    if (m_nSortColumn >= 0)
        invalidateCellRect(0, m_nSortColumn);
    m_nSortColumn = nCol;
    if (nCol >= 0)
        invalidateCellRect(0, nCol);
        */
}


bool HGridCtrl::isCellFixed(int nRow, int nCol)
{
    return (nRow < fixedRowCount() || nCol < fixedColumnCount());
}

void HGridCtrl::setModified(bool bModified , int nRow , int nCol )
{
    // Cannot guarantee sorting anymore...  sortColumn --huangw
    if (nCol < 0 || nCol == sortColumn())
        setSortColumn(-1);

    if (nRow >= 0 && nCol >= 0)
    {
        if (bModified)
        {
            setItemState(nRow, nCol, itemState(nRow, nCol) | GVIS_MODIFIED);
            m_bModified = true;
        }
        else
            setItemState(nRow, nCol, itemState(nRow, nCol) & ~GVIS_MODIFIED);
    }
    else
        m_bModified = bModified;

    if (!m_bModified)
    {
        for (int row = 0; row < rowCount(); row++)
            for (int col = 0; col < columnCount(); col++)
                setItemState(row, col, itemState(row, col) & ~GVIS_MODIFIED);
    }
}

bool HGridCtrl::isModified(int nRow , int nCol )
{
    if (nRow >= 0 && nCol >= 0)
        return ( (itemState(nRow, nCol) & GVIS_MODIFIED) == GVIS_MODIFIED );
    else
        return m_bModified;
}

/////////////////////////////////////////////////////////////////////////////////////
// GridCtrl cell visibility tests and invalidation/redraw functions

void HGridCtrl::refresh()
{
    if (m_bAllowDraw)
        update();
}
/*
// EnsureVisible supplied by Roelf Werkman
void HGridCtrl::EnsureVisible(int nRow, int nCol)
{
    if (!m_bAllowDraw)
        return;

    CRect rectWindow;*/
    /*   already ----huangw
    // set the scroll to the approximate position of row (Nigel Page-Jones)
    int nPos = (int)((float)nRow / GetRowCount() * 1000);
    float fPos = (float)nPos / 1000;
    SCROLLINFO scrollInfo;
    GetScrollInfo(SB_VERT, &scrollInfo);
    scrollInfo.nPos = (int)(scrollInfo.nMax * fPos);
    SetScrollInfo(SB_VERT, &scrollInfo, false);
    
    GetClientRect(rectWindow);

    // redraw cells    if necessary (Nigel Page-Jones)
    HCellID idTopLeft = GetTopleftNonFixedCell(false);
    HCellID idNewTopLeft = GetTopleftNonFixedCell(true);
    if (idNewTopLeft != idTopLeft)
    {
        rectWindow.top = GetFixedRowHeight();
        InvalidateRect(rectWindow);
    }
    */

    // We are gonna send some scroll messages, which will steal the focus 
    // from it's rightful owner. Squirrel it away ourselves so we can give
    // it back. (Damir)
    /*CWnd* pFocusWnd = GetFocus();

    HCellRange VisibleCells = visibleNonFixedCellRange();

    int right = nCol - VisibleCells.GetMaxCol();
    int left  = VisibleCells.GetMinCol() - nCol;
    int down  = nRow - VisibleCells.GetMaxRow();
    int up    = VisibleCells.GetMinRow() - nRow;

    int iColumnStart;
    int iRowStart;

    iColumnStart = VisibleCells.GetMaxCol() + 1;
    while( right > 0 )
    {
        if( GetColumnWidth( iColumnStart ) > 0 )
            SendMessage( WM_HSCROLL, SB_LINERIGHT, 0 );

        right--;
        iColumnStart++;
    }

    iColumnStart = VisibleCells.GetMinCol() - 1;
    while( left > 0 )
    {
        if( GetColumnWidth( iColumnStart ) > 0 )
            SendMessage( WM_HSCROLL, SB_LINELEFT, 0 );
        left--;
        iColumnStart--;
    }

    iRowStart = VisibleCells.GetMaxRow() + 1;
    while( down > 0 )
    {
        if( GetRowHeight( iRowStart ) > 0 )
            SendMessage( WM_VSCROLL, SB_LINEDOWN, 0 );
        down--;
        iRowStart++;
    }

    iRowStart = VisibleCells.GetMinRow() - 1;
    while( up > 0 )
    {
        if( GetRowHeight( iRowStart ) > 0 )
            SendMessage( WM_VSCROLL, SB_LINEUP, 0 );
        up--;
        iRowStart--;
    }

    // Move one more if we only see a snall bit of the cell
    CRect rectCell;
    if (!GetCellRect(nRow, nCol, rectCell))
    {
		if (pFocusWnd && ::IsWindow(pFocusWnd->GetSafeHwnd()))
			pFocusWnd->SetFocus(); 
        return;
    }

    GetClientRect(rectWindow);

    // The previous fix was fixed properly by Martin Richter <martin.richter@grutzeck.de>
    while (rectCell.right > rectWindow.right
           && rectCell.left > GetFixedColumnWidth())
    {
        SendMessage(WM_HSCROLL, SB_LINERIGHT, 0);
        if (!GetCellRect(nRow, nCol, rectCell))
        {
			if (pFocusWnd && ::IsWindow(pFocusWnd->GetSafeHwnd()))
				pFocusWnd->SetFocus(); 
            return;
        }
    }

    while (rectCell.bottom > rectWindow.bottom
           && rectCell.top > GetFixedRowHeight())
    {
        SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);
        if (!GetCellRect(nRow, nCol, rectCell))
        {
			if (pFocusWnd && ::IsWindow(pFocusWnd->GetSafeHwnd()))
				pFocusWnd->SetFocus(); 
            return;
        }
    }

    // restore focus to whoever owned it
	if (pFocusWnd && ::IsWindow(pFocusWnd->GetSafeHwnd()))
		pFocusWnd->SetFocus(); 
}
*/
bool HGridCtrl::isCellEditable(HCellID &cell) const
{
    return isCellEditable(cell.row, cell.col);
}

bool HGridCtrl::isCellEditable(int nRow, int nCol) const
{
    return isEditable() && ((itemState(nRow, nCol) & GVIS_READONLY) != GVIS_READONLY);
}

bool HGridCtrl::isCellSelected(HCellID &cell) const
{
    return isCellSelected(cell.row, cell.col);
}

bool HGridCtrl::isCellSelected(int nRow, int nCol) const
{
    if (isVirtualMode())
    {   
        if (!isSelectable())
            return false;

        HCellID cell;
        quint32 key = QMAKELONG(nRow, nCol);
        cell = m_SelectedCellMap.value(key);
        return (cell.isValid());
    }
    else
        return isSelectable() && ((itemState(nRow, nCol) & GVIS_SELECTED) == GVIS_SELECTED);
}

bool HGridCtrl::isCellVisible(const HCellID& cell)
{
    return isCellVisible(cell.row, cell.col);
}

bool HGridCtrl::isCellVisible(int nRow, int nCol)
{
    //question --huangw 计算x,y有什么用？
    int x, y;

    HCellID TopLeft;
    if (nCol >= fixedColumnCount() || nRow >= fixedRowCount())
    {
        TopLeft = topleftNonFixedCell();
        if (nCol >= fixedColumnCount() && nCol < TopLeft.col)
            return false;
        if (nRow >= fixedRowCount() && nRow < TopLeft.row)
            return false;
    }
    
    QRect rect = this->rect();//huangw rect()不包含框架的矩形
    //GetClientRect(rect);
    if (nCol < fixedColumnCount())
    {
        x = 0;
        for (int i = 0; i <= nCol; i++) 
        {
            if (x >= rect.right())
                return false;
            x += columnWidth(i);
        }
    } 
    else 
    {
        x = fixedColumnWidth();
        for (int i = TopLeft.col; i <= nCol; i++) 
        {
            if (x >= rect.right())
                return false;
            x += columnWidth(i);
        }
    }
    
    if (nRow < fixedRowCount())
    {
        y = 0;
        for (int i = 0; i <= nRow; i++) 
        {
            if (y >= rect.bottom())
                return false;
            y += rowHeight(i);
        }
    } 
    else 
    {
        if (nRow < TopLeft.row)
            return false;
        y = fixedRowHeight();
        for (int i = TopLeft.row; i <= nRow; i++) 
        {
            if (y >= rect.bottom())
                return false;
            y += rowHeight(i);
        }
    }
    
    return true;
}

bool HGridCtrl::invalidateCellRect(const HCellID& cell)
{
    return invalidateCellRect(cell.row, cell.col);
}

bool HGridCtrl::invalidateCellRect(const int row, const int col)
{
    if (!m_bAllowDraw)
        return false;

    if (!isValid(row, col))
        return false;

    if (!isCellVisible(row, col))
        return false;

    QRect rect;
    if (!cellRect(row, col, rect))
        return false;
    rect.setRight(rect.right()+5);
    rect.setBottom(rect.bottom()+5);
    //repaint(rect); //重绘 huangw
    update();
    return true;
}

bool HGridCtrl::invalidateCellRect(const HCellRange& cellRange)
{
    Q_ASSERT(isValid(cellRange));
    if (!m_bAllowDraw)
        return false;

    /*HCellRange visibleCellRange = visibleNonFixedCellRange().Intersect(cellRange);

    QRect rect;
    if (!cellRangeRect(visibleCellRange, rect))
        return false;

    rect.right++;
    rect.bottom++;*/
    update(); //重绘 huangw

    return true;
}


/////////////////////////////////////////////////////////////////////////////
#include <QDebug>
void  HGridCtrl::onHorizontalScrollBarChanged(int value)
{
    setScrollBarValue(QWM_HSCROLL,QSB_THUMBPOSITION,value);
}

void  HGridCtrl::onVerticalScrollBarChanged(int value)
{
    setScrollBarValue(QWM_VSCROLL,QSB_THUMBPOSITION,value);
}


void HGridCtrl::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta() / 8;//滚动的角度，*8就是鼠标滚动的距离
    int numSteps = numDegrees / 15;//滚动的步数，*15就是鼠标滚动的角度
    if (event->orientation() == Qt::Horizontal)
    {
        qDebug()<<"Horizontal";
    }
    else
    {
        qDebug()<<"Hor left:"<<horizontalScrollBar()->rect().left() << "Hor top:"<<horizontalScrollBar()->rect().top();
        qDebug()<<"Hor bottom:"<<horizontalScrollBar()->rect().bottom() << "Hor right:"<<horizontalScrollBar()->rect().right();
    }
    event->accept(); //接收该事件
}

void HGridCtrl::resizeEvent(QResizeEvent *event)
{
    static bool bAlreadyInsideThisProcedure = false;
    if (bAlreadyInsideThisProcedure)
        return;

    // Start re-entry blocking
    bAlreadyInsideThisProcedure = true;

    endEditing();        // destroy any InPlaceEdit's
    QAbstractScrollArea::resizeEvent(event);
    resetScrollBars();

    // End re-entry blocking
    bAlreadyInsideThisProcedure = false;
}

// CGridCtrl Mouse stuff
void HGridCtrl::mousePressEvent(QMouseEvent *event)
{
    qDebug()<<"mousePressEvent";

#ifdef GRIDCONTROL_USE_TITLETIPS
    // EFW - Bug Fix
    //m_TitleTip.Hide();  // hide any titletips
#endif

    // TRACE0("HGridCtrl::OnLButtonDown\n");
    // CWnd::OnLButtonDown(nFlags, point);
    QAbstractScrollArea::mousePressEvent(event);
    //SetFocus();

    m_bLMouseButtonDown   = true;
    m_LeftClickDownPoint = event->pos(); //注意了
    m_LeftClickDownCell  = cellFromPt(m_LeftClickDownPoint);
    if (!isValid(m_LeftClickDownCell))
        return;

    // If the SHIFT key is not down, then the start of the selection area should be the
    // cell just clicked. Otherwise, keep the previous selection-start-cell so the user
    // can add to their previous cell selections in an intuitive way. If no selection-
    // start-cell has been specified, then set it's value here and now.
    if(Qt::ShiftModifier != event->modifiers())
        m_SelectionStartCell = m_LeftClickDownCell;
    else
    {
        if (!isValid(m_SelectionStartCell))
            m_SelectionStartCell = m_idCurrentCell;
    }

    endEditing();//huangw

    // tell the cell about it
    HGridCellBase* pCell = getCell(m_LeftClickDownCell.row, m_LeftClickDownCell.col);
    if (pCell)
        pCell->onClickDown(pointClicked( m_LeftClickDownCell.row, m_LeftClickDownCell.col, m_LeftClickDownPoint));

    // Clicked in the text area? Only then will cell selection work
    bool bInTextArea = false;
    if (pCell)
    {
        QRect rectCell;
        if (cellRect(m_LeftClickDownCell.row, m_LeftClickDownCell.col, rectCell) &&
            pCell->textRect(rectCell))
        {
            bInTextArea = rectCell.contains(m_LeftClickDownPoint);
        }
    }

    //如果第一选择cell，第二次又点了同样cell 此处返回，进入cell编辑状态
    if (m_LeftClickDownCell == m_idCurrentCell &&  !(Qt::ControlModifier == event->modifiers()) &&
                                bInTextArea &&isCellEditable(m_LeftClickDownCell))
    {
        m_MouseMode = MOUSE_PREPARE_EDIT;
        return;
    }
    // If the user clicks on a selected cell, then prepare to drag it.
    // (If the user moves the mouse, then dragging occurs)
    else if (isCellSelected(m_LeftClickDownCell))
    {
        // If control is pressed then unselect the cell or row (depending on the list mode)
        if (Qt::ControlModifier == event->modifiers())
        {
            setFocusCell(m_LeftClickDownCell);
            if (isListMode())
                selectRows(m_LeftClickDownCell, true, false);
            else
                selectCells(m_LeftClickDownCell, true, false);
            return;
        }
        else
            m_MouseMode = MOUSE_PREPARE_DRAG;
    }
    else if (m_MouseMode != MOUSE_OVER_COL_DIVIDE &&
             m_MouseMode != MOUSE_OVER_ROW_DIVIDE)
    {
        if (m_LeftClickDownCell.row >= fixedRowCount() &&
            m_LeftClickDownCell.col >= fixedColumnCount())
        {
            setFocusCell(m_LeftClickDownCell.row, m_LeftClickDownCell.col);
        }
        else
            setFocusCell(-1, -1);
    }

    //SetCapture();

    if (m_MouseMode == MOUSE_NOTHING)
    {
        if (m_bAllowColumnResize && mouseOverColumnResizeArea(m_LeftClickDownPoint))
        {
            if (m_MouseMode != MOUSE_OVER_COL_DIVIDE)
            {
                setCursor(Qt::SizeHorCursor);
                m_MouseMode = MOUSE_OVER_COL_DIVIDE;
            }
        }
        else if (m_bAllowRowResize && mouseOverRowResizeArea(m_LeftClickDownPoint))
        {
            if (m_MouseMode != MOUSE_OVER_ROW_DIVIDE)
            {
                setCursor(Qt::SizeVerCursor);
                m_MouseMode = MOUSE_OVER_ROW_DIVIDE;
            }
        }
    }

    if (m_MouseMode == MOUSE_OVER_COL_DIVIDE) // sizing column
    {
        m_MouseMode = MOUSE_SIZING_COL;

        // Kludge for if we are over the last column...
        if (columnWidth(columnCount()-1) < m_nResizeCaptureRange)
        {
            QRect VisRect;
            visibleNonFixedCellRange(VisRect);
            if (abs(m_LeftClickDownPoint.x() - VisRect.right()) < m_nResizeCaptureRange)
                m_LeftClickDownCell.col = columnCount()-1;
        }

        QPoint start;
        if (!cellOrigin(0, m_LeftClickDownCell.col, start))
            return;

        if( !m_bHiddenColUnhide)
        {
            //  ignore columns that are hidden and look left towards first visible column
            bool bLookForVisible = true;
            bool bIsCellRightBorder = m_LeftClickDownPoint.x() - start.x() >= m_nResizeCaptureRange;

            if( bIsCellRightBorder && m_LeftClickDownCell.col + 1 >= columnCount() )
            {
                // clicked on last column's right border

                // if last column is visible, don't do anything
                if( m_LeftClickDownCell.col >= 0)
                    bLookForVisible = false;
            }

            if( bLookForVisible)
            {
                // clicked on column divider other than last right border
                bool bFoundVisible = false;
                int iOffset = 1;

                if( bIsCellRightBorder)
                    iOffset = 0;

                while( m_LeftClickDownCell.col - iOffset >= 0)
                {
                    if( columnWidth( m_LeftClickDownCell.col - iOffset) > 0)
                    {
                        bFoundVisible = true;
                        break;
                    }
                    m_LeftClickDownCell.col--;
                }
                if( !bFoundVisible)
                    return;
            }
        }
        // If we clicked to the right of the colimn divide, then reset the click-down cell
        // as the cell to the left of the column divide - UNLESS we clicked on the last column
        // and the last column is teensy (kludge fix)
        if (m_LeftClickDownPoint.x() - start.x() < m_nResizeCaptureRange)
        {
            if (m_LeftClickDownCell.col < columnCount()-1 ||
                columnWidth(columnCount()-1) >= m_nResizeCaptureRange)
            {
                if (!cellOrigin(0, --m_LeftClickDownCell.col, start))
                    return;
            }
        }
    }
    else if (m_MouseMode == MOUSE_OVER_ROW_DIVIDE) // sizing row
    {
        m_MouseMode = MOUSE_SIZING_ROW;

        // Kludge for if we are over the last column...
        if (rowHeight(rowCount()-1) < m_nResizeCaptureRange)
        {
            QRect VisRect;
            visibleNonFixedCellRange(VisRect);
            if (abs(m_LeftClickDownPoint.y() - VisRect.bottom()) < m_nResizeCaptureRange)
                m_LeftClickDownCell.row = rowCount()-1;
        }

        QPoint start;
        if (!cellOrigin(m_LeftClickDownCell, start))
            return;

        if( !m_bHiddenRowUnhide)
        {
            //  ignore rows that are hidden and look up towards first visible row
            bool bLookForVisible = true;
            bool bIsCellBottomBorder = m_LeftClickDownPoint.y() - start.y() >= m_nResizeCaptureRange;

            if( bIsCellBottomBorder
                && m_LeftClickDownCell.row + 1 >= rowCount() )
            {
                // clicked on last row's bottom border

                // if last row is visible, don't do anything
                if( m_LeftClickDownCell.row >= 0)
                    bLookForVisible = false;
            }

            if( bLookForVisible)
            {
                // clicked on row divider other than last bottom border
                bool bFoundVisible = false;
                int iOffset = 1;

                if( bIsCellBottomBorder)
                    iOffset = 0;

                while( m_LeftClickDownCell.row - iOffset >= 0)
                {
                    if( rowHeight( m_LeftClickDownCell.row - iOffset) > 0)
                    {
                        bFoundVisible = true;
                        break;
                    }
                    m_LeftClickDownCell.row--;
                }
                if( !bFoundVisible)
                    return;
            }
        }

        if (m_LeftClickDownPoint.y() - start.y() < m_nResizeCaptureRange)            // clicked below border
        {
            if (m_LeftClickDownCell.row < rowCount()-1 ||
                rowHeight(rowCount()-1) >= m_nResizeCaptureRange)
            {
                if (!cellOrigin(--m_LeftClickDownCell.row, 0, start))
                    return;
            }
        }
    }
    else if (m_MouseMode != MOUSE_PREPARE_DRAG) // not sizing or editing -- selecting
    {
        //SendMessageToParent(m_LeftClickDownCell.row, m_LeftClickDownCell.col, GVN_SELCHANGING);

        // If Ctrl pressed, save the current cell selection. This will get added
        // to the new cell selection at the end of the cell selection process
        m_PrevSelectedCellMap.clear();
        if (Qt::ControlModifier == event->modifiers())
        {
            QMap<quint32,HCellID>::iterator it = m_SelectedCellMap.begin();
            for (; it != m_SelectedCellMap.end(); ++it)
            {
                quint32 key = it.key();
                HCellID cell;
                cell = m_SelectedCellMap.value(key);
                m_PrevSelectedCellMap.insert(key, cell);
            }
        }

        if (m_LeftClickDownCell.row < fixedRowCount())
        {
            onFixedRowClick(m_LeftClickDownCell);
        }
        else if (m_LeftClickDownCell.col < fixedColumnCount())
        {
            onFixedColumnClick(m_LeftClickDownCell);
        }
        else
        {
            m_MouseMode = m_bListMode? MOUSE_SELECT_ROW : MOUSE_SELECT_CELLS;
            onSelecting(m_LeftClickDownCell);

            //m_nTimerID = SetTimer(WM_LBUTTONDOWN, m_nTimerInterval, 0);
        }
    }
    m_LastMousePoint = event->pos();
}

#include <QDebug>
void HGridCtrl::mouseReleaseEvent(QMouseEvent *event)
{
    // TRACE0("HGridCtrl::OnLButtonUp\n");

    QAbstractScrollArea::mouseReleaseEvent(event);
    QPoint point = event->pos();
    m_bLMouseButtonDown = false;

    /*if (GetCapture()->GetSafeHwnd() == GetSafeHwnd())
    {
        ReleaseCapture();
        KillTimer(m_nTimerID);
        m_nTimerID = 0;
    }*/

    QPoint pointClickedRel;
    pointClickedRel = pointClicked( m_idCurrentCell.row, m_idCurrentCell.col, point);

    // m_MouseMode == MOUSE_PREPARE_EDIT only if user clicked down on current cell
    // and then didn't move mouse before clicking up (releasing button)
    if (m_MouseMode == MOUSE_PREPARE_EDIT)
    {
        onEditCell(m_idCurrentCell.row, m_idCurrentCell.col, pointClickedRel);
    }
    else if (m_MouseMode == MOUSE_PREPARE_DRAG)
    {
        HGridCellBase* pCell = getCell(m_idCurrentCell.row, m_idCurrentCell.col);
        if (pCell)
            pCell->onClick( pointClicked( m_idCurrentCell.row, m_idCurrentCell.col, point) );
        resetSelectedRange();
    }
    else if (m_MouseMode == MOUSE_SIZING_COL)
    {
        QRect rect;
        rect = viewport()->rect();
        if (m_LeftClickDownPoint != point && (point.x() != 0 || point.y() != 0)) // 0 pt fix by email1@bierling.net
        {
            QPoint start;
            //Used for merge cells
            //by Huang Wei
            //m_LeftClickDownCell=GetMergeCellID(m_LeftClickDownCell);
            if (!cellOrigin(m_LeftClickDownCell, start))
                return;

            int nColumnWidth = max(point.x() - start.x(), m_bAllowColHide? 0 : 1);
            //Used for merge cells
            //by Huang Wei
            //int mergewidth=GetMergeCellWidth(m_LeftClickDownCell)-GetColumnWidth(m_LeftClickDownCell.col);
            setColumnWidth(m_LeftClickDownCell.col, nColumnWidth);

            resetScrollBars();
            update();
        }
    }
    else if (m_MouseMode == MOUSE_SIZING_ROW)
    {
        QRect rect;
        rect = viewport()->rect();
        if (m_LeftClickDownPoint != point  && (point.x() != 0 || point.y() != 0)) // 0 pt fix by email1@bierling.net
        {
            QPoint start;
            //Used for merge cells
            //by Huang Wei
            //m_LeftClickDownCell=GetMergeCellID(m_LeftClickDownCell);
            if (!cellOrigin(m_LeftClickDownCell, start))
                return;

            int nRowHeight = max(point.y() - start.y(), m_bAllowRowHide? 0 : 1);
            //Used for merge cells
            //by Huang Wei
            //int mergeheight=GetMergeCellHeight(m_LeftClickDownCell)-GetRowHeight(m_LeftClickDownCell.row);

            setRowHeight(m_LeftClickDownCell.row, nRowHeight);
            resetScrollBars();
            update();
        }
    }
    else
    {
        HGridCellBase* pCell = getCell(m_idCurrentCell.row, m_idCurrentCell.col);
        if (pCell)
            pCell->onClick( pointClicked( m_idCurrentCell.row, m_idCurrentCell.col, point) );
    }

    m_MouseMode = MOUSE_NOTHING;
    setCursor(Qt::ArrowCursor);

    if (!isValid(m_LeftClickDownCell))
        return;

}


void HGridCtrl::mouseMoveEvent(QMouseEvent *event)
{
    QPoint point = event->pos();
    QRect rect;
    rect = viewport()->rect();

#ifndef GRIDCONTROL_NO_DRAGDROP
    // If outside client area, return (unless we are drag n dropping)
    if (m_MouseMode != MOUSE_DRAGGING && !rect.contains(point))
        return;
#endif

    // Sometimes a MOUSEMOVE message can come after the left buttons
    // has been let go, but before the BUTTONUP message hs been processed.
    // We'll keep track of mouse buttons manually to avoid this.
    // All bMouseButtonDown's have been replaced with the member m_bLMouseButtonDown
    // BOOL bMouseButtonDown = ((nFlags & MK_LBUTTON) == MK_LBUTTON);

    // If the left mouse button is up, then test to see if row/column sizing is imminent
    if (!m_bLMouseButtonDown || (m_bLMouseButtonDown && m_MouseMode == MOUSE_NOTHING))
    {
        if (m_bAllowColumnResize && mouseOverColumnResizeArea(point))
        {
            if (m_MouseMode != MOUSE_OVER_COL_DIVIDE)
            {
                setCursor(Qt::SizeHorCursor);
                m_MouseMode = MOUSE_OVER_COL_DIVIDE;
            }
        }
        else if (m_bAllowRowResize && mouseOverRowResizeArea(point))
        {
            if (m_MouseMode != MOUSE_OVER_ROW_DIVIDE)
            {
                setCursor(Qt::SizeVerCursor);
                m_MouseMode = MOUSE_OVER_ROW_DIVIDE;
            }
        }
        else if (m_MouseMode != MOUSE_NOTHING)
        {
            setCursor(Qt::ArrowCursor);
            m_MouseMode = MOUSE_NOTHING;
        }

        if (m_MouseMode == MOUSE_NOTHING)
        {
            HGridCellBase* pCell = NULL;
            HCellID idCurrentCell;
            if (!isVirtualMode() || m_bTitleTips)
            {
                // Let the cell know that a big fat cursor is currently hovering
                // over it.
                idCurrentCell = cellFromPt(point);
                pCell = getCell(idCurrentCell.row, idCurrentCell.col);
                if (pCell)
                    pCell->onMouseOver();
            }
        }
        m_LastMousePoint = point;
        return;
    }

    if (!isValid(m_LeftClickDownCell))
    {
        m_LastMousePoint = point;
        return;
    }

    // If the left mouse button is down, then process appropriately
    if (m_bLMouseButtonDown)
    {
        switch (m_MouseMode)
        {
        case MOUSE_SELECT_ALL:
            break;

        case MOUSE_SELECT_COL:
        case MOUSE_SELECT_ROW:
        case MOUSE_SELECT_CELLS:
            {
                HCellID idCurrentCell = cellFromPt(point);
                if (!isValid(idCurrentCell))
                    return;
                if (idCurrentCell != focusCell())
                {
                    onSelecting(idCurrentCell);
                    if((idCurrentCell.row >= m_nFixedRows &&
                      idCurrentCell.col >= m_nFixedCols) ||
                      m_MouseMode==MOUSE_SELECT_COL ||
                      m_MouseMode==MOUSE_SELECT_ROW)
                    {
                        setFocusCell(idCurrentCell);
                    }
                }
                break;
            }

        case MOUSE_SIZING_COL:
        case MOUSE_SIZING_ROW:
        {
            m_CurMousePoint = point;
            update();
        }
            break;
        }
    }

    m_LastMousePoint = point;
}

/*
// Handles mouse wheel notifications
// Note - if this doesn't work for win95 then use OnRegisteredMouseWheel instead
#if !defined(_WIN32_WCE) && (_MFC_VER >= 0x0421)
bool HGridCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // A m_nRowsPerWheelNotch value less than 0 indicates that the mouse
    // wheel scrolls whole pages, not just lines.
    if (m_nRowsPerWheelNotch == -1)
    {
        int nPagesScrolled = zDelta / 120;

        if (nPagesScrolled > 0)
		{
            for (int i = 0; i < nPagesScrolled; i++)
			{
                PostMessage(WM_VSCROLL, SB_PAGEUP, 0);
			}
		}
        else
		{
            for (int i = 0; i > nPagesScrolled; i--)
			{
                PostMessage(WM_VSCROLL, SB_PAGEDOWN, 0);
			}
		}
    }
    else
    {
        int nRowsScrolled = m_nRowsPerWheelNotch * zDelta / 120;

        if (nRowsScrolled > 0)
		{
            for (int i = 0; i < nRowsScrolled; i++)
			{
                PostMessage(WM_VSCROLL, SB_LINEUP, 0);
			}
		}
        else
		{
            for (int i = 0; i > nRowsScrolled; i--)
			{
                PostMessage(WM_VSCROLL, SB_LINEDOWN, 0);
			}
		}
    }

    return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}
#endif // !defined(_WIN32_WCE) && (_MFC_VER >= 0x0421)

void HGridCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
    CRect rect;
    GetClientRect(rect);

#ifndef GRIDCONTROL_NO_DRAGDROP
    // If outside client area, return (unless we are drag n dropping)
    if (m_MouseMode != MOUSE_DRAGGING && !rect.PtInRect(point))
        return;
#endif

    // Sometimes a MOUSEMOVE message can come after the left buttons
    // has been let go, but before the BUTTONUP message hs been processed.
    // We'll keep track of mouse buttons manually to avoid this.
    // All bMouseButtonDown's have been replaced with the member m_bLMouseButtonDown
    // bool bMouseButtonDown = ((nFlags & MK_LBUTTON) == MK_LBUTTON);

    // If the left mouse button is up, then test to see if row/column sizing is imminent
    if (!m_bLMouseButtonDown ||
        (m_bLMouseButtonDown && m_MouseMode == MOUSE_NOTHING))
    {
        if (m_bAllowColumnResize && mouseOverColumnResizeArea(point))
        {
            if (m_MouseMode != MOUSE_OVER_COL_DIVIDE)
            {
#ifndef _WIN32_WCE_NO_CURSOR
                SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
#endif
                m_MouseMode = MOUSE_OVER_COL_DIVIDE;
            }
        }
        else if (m_bAllowRowResize && mouseOverRowResizeArea(point))
        {
            if (m_MouseMode != MOUSE_OVER_ROW_DIVIDE)
            {
#ifndef _WIN32_WCE_NO_CURSOR
                SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));
#endif
                m_MouseMode = MOUSE_OVER_ROW_DIVIDE;
            }
        }
        else if (m_MouseMode != MOUSE_NOTHING)
        {
#ifndef _WIN32_WCE_NO_CURSOR
            SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
#endif
            m_MouseMode = MOUSE_NOTHING;
        }

        if (m_MouseMode == MOUSE_NOTHING)
        {
            CGridCellBase* pCell = NULL;
            HCellID idCurrentCell;
            if (!GetVirtualMode() || m_bTitleTips)
            {
                // Let the cell know that a big fat cursor is currently hovering
                // over it.
                idCurrentCell = cellFromPt(point);
                pCell = GetCell(idCurrentCell.row, idCurrentCell.col);
                if (pCell)
                    pCell->OnMouseOver();
            }

#ifndef GRIDCONTROL_NO_TITLETIPS
            // Titletips anyone? anyone?
            if (m_bTitleTips)
            {
                CRect TextRect, CellRect;
                if (pCell)
                {
                    LPCTSTR szTipText = pCell->GetTipText();
                    if (!m_bRMouseButtonDown
						&& szTipText && szTipText[0]
                        && !pCell->IsEditing()
                        && GetCellRect( idCurrentCell.row, idCurrentCell.col, &TextRect)
                        && pCell->GetTipTextRect( &TextRect)
                        && GetCellRect(idCurrentCell.row, idCurrentCell.col, CellRect) )
                    {
						TRACE0("Showing TitleTip\n");
						m_TitleTip.Show(TextRect, pCell->GetTipText(),  0, CellRect,
                                        pCell->GetFont(),  GetTitleTipTextClr(), GetTitleTipBackClr());
                    }
                }
            }
#endif
        }

        m_LastMousePoint = point;
        return;
    }

    if (!isValid(m_LeftClickDownCell))
    {
        m_LastMousePoint = point;
        return;
    }

    // If the left mouse button is down, then process appropriately
    if (m_bLMouseButtonDown)
    {
        switch (m_MouseMode)
        {
        case MOUSE_SELECT_ALL:
            break;

        case MOUSE_SELECT_COL:
        case MOUSE_SELECT_ROW:
        case MOUSE_SELECT_CELLS:
            {
                HCellID idCurrentCell = cellFromPt(point);
                if (!isValid(idCurrentCell))
                    return;

                if (idCurrentCell != GetFocusCell())
                {
                    onSelecting(idCurrentCell);

                    // EFW - BUG FIX - Keep the appropriate cell row and/or
                    // column focused.  A fix in SetFocusCell() will place
                    // the cursor in a non-fixed cell as needed.
                    if((idCurrentCell.row >= m_nFixedRows &&
                      idCurrentCell.col >= m_nFixedCols) ||
                      m_MouseMode==MOUSE_SELECT_COL ||
                      m_MouseMode==MOUSE_SELECT_ROW)
                    {
                        SetFocusCell(idCurrentCell);
                    }
                }
                break;
            }

        case MOUSE_SIZING_COL:
            {
                CDC* pDC = GetDC();
                if (!pDC)
                    break;

                CRect oldInvertedRect(m_LastMousePoint.x, rect.top,
                    m_LastMousePoint.x + 2, rect.bottom);
                pDC->InvertRect(&oldInvertedRect);
                CRect newInvertedRect(point.x, rect.top, 
                    point.x + 2, rect.bottom);
                pDC->InvertRect(&newInvertedRect);
                ReleaseDC(pDC);
            }
            break;
            
        case MOUSE_SIZING_ROW:        
            {
                CDC* pDC = GetDC();
                if (!pDC)
                    break;
                
                CRect oldInvertedRect(rect.left, m_LastMousePoint.y,
                    rect.right, m_LastMousePoint.y + 2);
                pDC->InvertRect(&oldInvertedRect);
                CRect newInvertedRect(rect.left, point.y, 
                    rect.right, point.y + 2);
                pDC->InvertRect(&newInvertedRect);
                ReleaseDC(pDC);
            }
            break;
            
#ifndef GRIDCONTROL_NO_DRAGDROP
        case MOUSE_PREPARE_EDIT:
        case MOUSE_PREPARE_DRAG:
            m_MouseMode = MOUSE_PREPARE_DRAG;
            OnBeginDrag();    
            break;
#endif
        }    
    }

    m_LastMousePoint = point;
}
*/
// Returns the point inside the cell that was clicked (coords relative to cell top left)
QPoint HGridCtrl::pointClicked(int nRow, int nCol, const QPoint& point)
{
    QPoint PointCellOrigin;
    if( !cellOrigin( nRow, nCol, PointCellOrigin)  )
        return QPoint( 0, 0);

    QPoint PointClickedCellRelative( point);
    PointClickedCellRelative -= PointCellOrigin;
    return PointClickedCellRelative;
}

/*
void HGridCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    TRACE0("HGridCtrl::OnLButtonDblClk\n");

    HCellID cell = cellFromPt(point);
    if( !isValid( cell) )
    {
        //Q_ASSERT(false);
        return;
    }

#ifdef _WIN32_WCE
    if (mouseOverColumnResizeArea(point))
#else
    if (m_MouseMode == MOUSE_OVER_COL_DIVIDE)
#endif
    {
        CPoint start;
        if (!GetCellOrigin(0, cell.col, &start))
            return;

        if (point.x - start.x < m_nResizeCaptureRange)     // Clicked right of border
            cell.col--;

        //  ignore columns that are hidden and look left towards first visible column
        bool bFoundVisible = false;
        while( cell.col >= 0)
        {
            if( GetColumnWidth( cell.col) > 0)
            {
                bFoundVisible = true;
                break;
            }
            cell.col--;
        }
        if( !bFoundVisible)
            return;

        AutoSizeColumn(cell.col, GetAutoSizeStyle());
        Invalidate();
    }
#ifdef _WIN32_WCE
    else if (mouseOverRowResizeArea(point))
#else
    else if (m_MouseMode == MOUSE_OVER_ROW_DIVIDE)
#endif
    {
        CPoint start;
        if (!GetCellOrigin(0, cell.col, &start))
            return;

        if (point.y - start.y < m_nResizeCaptureRange)     // Clicked below border
            cell.row--;

        //  ignore rows that are hidden and look up towards first visible row
        bool bFoundVisible = false;
        while( cell.row >= 0)
        {
            if( GetRowHeight( cell.row) > 0)
            {
                bFoundVisible = true;
                break;
            }
            cell.row--;
        }
        if( !bFoundVisible)
            return;

        AutoSizeRow(cell.row);
        Invalidate();
    }
    else if (m_MouseMode == MOUSE_NOTHING)
    {
        CPoint pointClickedRel;
        pointClickedRel = pointClicked( cell.row, cell.col, point);

        CGridCellBase* pCell = NULL;
        if (isValid(cell))
            pCell = GetCell(cell.row, cell.col);

        // Clicked in the text area? Only then will cell selection work
        bool bInTextArea = false;
        if (pCell)
        {
            CRect rectCell;
            if (GetCellRect(cell.row, cell.col, rectCell) && pCell->GetTextRect(rectCell))
                bInTextArea = rectCell.PtInRect(point);
        }

        if (cell.row >= m_nFixedRows && isValid(m_LeftClickDownCell) &&
            cell.col >= m_nFixedCols && bInTextArea)
        {
            OnEditCell(cell.row, cell.col, pointClickedRel, VK_LBUTTON);
        }
        else if (m_bListMode)
        {
            if (!isValid(cell))
                return;
            if (cell.row >= m_nFixedRows && cell.col >= m_nFixedCols && bInTextArea)
                OnEditCell(cell.row, cell.col, pointClickedRel, VK_LBUTTON);
        }

        if (isValid(cell))
        {
            CGridCellBase* pCell = GetCell(cell.row, cell.col);
            if (pCell)
                pCell->OnDblClick(pointClickedRel);
            SendMessageToParent(cell.row, cell.col, NM_DBLCLK);
        }
    }

    CWnd::OnLButtonDblClk(nFlags, point);
}
*/
/*
void HGridCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
#ifdef GRIDCONTROL_USE_TITLETIPS
    // EFW - Bug Fix
    m_TitleTip.Hide();  // hide any titletips
#endif

    // TRACE0("HGridCtrl::OnLButtonDown\n");
    // CWnd::OnLButtonDown(nFlags, point);

    SetFocus();

    m_bLMouseButtonDown   = true;
    m_LeftClickDownPoint = point;
    m_LeftClickDownCell  = cellFromPt(point);
    if (!isValid(m_LeftClickDownCell))
        return;

    // If the SHIFT key is not down, then the start of the selection area should be the 
    // cell just clicked. Otherwise, keep the previous selection-start-cell so the user
    // can add to their previous cell selections in an intuitive way. If no selection-
    // start-cell has been specified, then set it's value here and now.
    if ((nFlags & MK_SHIFT) != MK_SHIFT)
        m_SelectionStartCell = m_LeftClickDownCell;
    else
    {
        if (!isValid(m_SelectionStartCell))
            m_SelectionStartCell = m_idCurrentCell;
    }

    EndEditing();

    // tell the cell about it 
    CGridCellBase* pCell = GetCell(m_LeftClickDownCell.row, m_LeftClickDownCell.col);
    if (pCell)
        pCell->OnClickDown(pointClicked( m_LeftClickDownCell.row, m_LeftClickDownCell.col, point));

    // Clicked in the text area? Only then will cell selection work
    bool bInTextArea = false;
    if (pCell)
    {
        CRect rectCell;
        if (GetCellRect(m_LeftClickDownCell.row, m_LeftClickDownCell.col, rectCell) &&
            pCell->GetTextRect(rectCell))
        {
            bInTextArea = rectCell.PtInRect(point);
        }
    }

    // If the user clicks on the current cell, then prepare to edit it.
    // (If the user moves the mouse, then dragging occurs)
    if (m_LeftClickDownCell == m_idCurrentCell && 
        !(nFlags & MK_CONTROL) && bInTextArea &&
        IsCellEditable(m_LeftClickDownCell))
    {
        m_MouseMode = MOUSE_PREPARE_EDIT;
        return;
    }
    // If the user clicks on a selected cell, then prepare to drag it.
    // (If the user moves the mouse, then dragging occurs)
    else if (IsCellSelected(m_LeftClickDownCell))
    {
        // If control is pressed then unselect the cell or row (depending on the list mode)
        if (nFlags & MK_CONTROL)
        {
            SetFocusCell(m_LeftClickDownCell);
            if (GetListMode())
                selectRows(m_LeftClickDownCell, true, false);
            else
                selectCells(m_LeftClickDownCell, true, false);
            return;
        }
#ifndef GRIDCONTROL_NO_DRAGDROP
        else if (m_bAllowDragAndDrop)
            m_MouseMode = MOUSE_PREPARE_DRAG;
#endif
    }
    else if (m_MouseMode != MOUSE_OVER_COL_DIVIDE &&
             m_MouseMode != MOUSE_OVER_ROW_DIVIDE)        
    {
		if (m_LeftClickDownCell.row >= GetFixedRowCount() && 
			m_LeftClickDownCell.col >= GetFixedColumnCount())
		{
            SetFocusCell(m_LeftClickDownCell.row, m_LeftClickDownCell.col);
		}
		else
			SetFocusCell(-1, -1);

		//Don't set focus on any cell if the user clicked on a fixed cell - David Pritchard
        //if (GetRowCount() > GetFixedRowCount() && 
        //    GetColumnCount() > GetFixedColumnCount())
        //    SetFocusCell(max(m_LeftClickDownCell.row, m_nFixedRows),
        //                 max(m_LeftClickDownCell.col, m_nFixedCols));
    }
    
    SetCapture();
    
    if (m_MouseMode == MOUSE_NOTHING)
    {
        if (m_bAllowColumnResize && mouseOverColumnResizeArea(point))
        {
            if (m_MouseMode != MOUSE_OVER_COL_DIVIDE)
            {
#ifndef _WIN32_WCE_NO_CURSOR
                SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
#endif
                m_MouseMode = MOUSE_OVER_COL_DIVIDE;
            }
        }
        else if (m_bAllowRowResize && mouseOverRowResizeArea(point))
        {
            if (m_MouseMode != MOUSE_OVER_ROW_DIVIDE)
            {
#ifndef _WIN32_WCE_NO_CURSOR
                SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));
#endif
                m_MouseMode = MOUSE_OVER_ROW_DIVIDE;
            }
        }
        // else if (m_MouseMode != MOUSE_NOTHING)
        //{
        //    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
        //    m_MouseMode = MOUSE_NOTHING;
        //}
    }
    
    if (m_MouseMode == MOUSE_OVER_COL_DIVIDE) // sizing column
    {
        m_MouseMode = MOUSE_SIZING_COL;

        // Kludge for if we are over the last column...
        if (GetColumnWidth(GetColumnCount()-1) < m_nResizeCaptureRange)
        {
            CRect VisRect;
            visibleNonFixedCellRange(VisRect);
            if (abs(point.x - VisRect.right) < m_nResizeCaptureRange)
                m_LeftClickDownCell.col = GetColumnCount()-1;
        }

        CPoint start;
        if (!GetCellOrigin(0, m_LeftClickDownCell.col, &start))
            return;

        if( !m_bHiddenColUnhide)
        {
            //  ignore columns that are hidden and look left towards first visible column
            bool bLookForVisible = true;
            bool bIsCellRightBorder = point.x - start.x >= m_nResizeCaptureRange;

            if( bIsCellRightBorder
                && m_LeftClickDownCell.col + 1 >= GetColumnCount() )
            {
                // clicked on last column's right border

                // if last column is visible, don't do anything
                if( m_LeftClickDownCell.col >= 0)
                    bLookForVisible = false;
            }

            if( bLookForVisible)
            {
                // clicked on column divider other than last right border
                bool bFoundVisible = false;
                int iOffset = 1;

                if( bIsCellRightBorder)
                    iOffset = 0;

                while( m_LeftClickDownCell.col - iOffset >= 0)
                {
                    if( GetColumnWidth( m_LeftClickDownCell.col - iOffset) > 0)
                    {
                        bFoundVisible = true;
                        break;
                    }
                    m_LeftClickDownCell.col--;
                }
                if( !bFoundVisible)
                    return;
            }
        }


        CRect rect;
        GetClientRect(rect);
        CRect invertedRect(point.x, rect.top, point.x + 2, rect.bottom);

        CDC* pDC = GetDC();
        if (pDC)
        {
            pDC->InvertRect(&invertedRect);
            ReleaseDC(pDC);
        }

        // If we clicked to the right of the colimn divide, then reset the click-down cell
        // as the cell to the left of the column divide - UNLESS we clicked on the last column
        // and the last column is teensy (kludge fix)
        if (point.x - start.x < m_nResizeCaptureRange)
        {
            if (m_LeftClickDownCell.col < GetColumnCount()-1 || 
                GetColumnWidth(GetColumnCount()-1) >= m_nResizeCaptureRange)
            {
                if (!GetCellOrigin(0, --m_LeftClickDownCell.col, &start))
                    return;
            }
        }

		// Allow a cell resize width no greater than that which can be viewed within
		// the grid itself
		int nMaxCellWidth = rect.Width()-GetFixedColumnWidth();
        rect.left  = start.x + 1;
		rect.right = rect.left + nMaxCellWidth;

        ClientToScreen(rect);
#ifndef _WIN32_WCE_NO_CURSOR
        ClipCursor(rect);
#endif
    }
    else if (m_MouseMode == MOUSE_OVER_ROW_DIVIDE) // sizing row
    {
        m_MouseMode = MOUSE_SIZING_ROW;

        // Kludge for if we are over the last column...
        if (GetRowHeight(GetRowCount()-1) < m_nResizeCaptureRange)
        {
            CRect VisRect;
            visibleNonFixedCellRange(VisRect);
            if (abs(point.y - VisRect.bottom) < m_nResizeCaptureRange)
                m_LeftClickDownCell.row = GetRowCount()-1;
        }

        CPoint start;
        if (!GetCellOrigin(m_LeftClickDownCell, &start))
            return;

        if( !m_bHiddenRowUnhide)
        {
            //  ignore rows that are hidden and look up towards first visible row
            bool bLookForVisible = true;
            bool bIsCellBottomBorder = point.y - start.y >= m_nResizeCaptureRange;

            if( bIsCellBottomBorder
                && m_LeftClickDownCell.row + 1 >= GetRowCount() )
            {
                // clicked on last row's bottom border

                // if last row is visible, don't do anything
                if( m_LeftClickDownCell.row >= 0)
                    bLookForVisible = false;
            }

            if( bLookForVisible)
            {
                // clicked on row divider other than last bottom border
                bool bFoundVisible = false;
                int iOffset = 1;

                if( bIsCellBottomBorder)
                    iOffset = 0;

                while( m_LeftClickDownCell.row - iOffset >= 0)
                {
                    if( GetRowHeight( m_LeftClickDownCell.row - iOffset) > 0)
                    {
                        bFoundVisible = true;
                        break;
                    }
                    m_LeftClickDownCell.row--;
                }
                if( !bFoundVisible)
                    return;
            }
        }

        CRect rect;
        GetClientRect(rect);
        CRect invertedRect(rect.left, point.y, rect.right, point.y + 2);

        CDC* pDC = GetDC();
        if (pDC)
        {
            pDC->InvertRect(&invertedRect);
            ReleaseDC(pDC);
        }

        // If we clicked below the row divide, then reset the click-down cell
        // as the cell above the row divide - UNLESS we clicked on the last row
        // and the last row is teensy (kludge fix)
        if (point.y - start.y < m_nResizeCaptureRange)            // clicked below border
        {
            if (m_LeftClickDownCell.row < GetRowCount()-1 || 
                GetRowHeight(GetRowCount()-1) >= m_nResizeCaptureRange)
            {
                if (!GetCellOrigin(--m_LeftClickDownCell.row, 0, &start))
                    return;
            }
        }

		int nMaxCellHeight = rect.Height()-GetFixedRowHeight();
        rect.top = start.y + 1;
		rect.bottom = rect.top + nMaxCellHeight;

        ClientToScreen(rect);

#ifndef _WIN32_WCE_NO_CURSOR
        ClipCursor(rect);
#endif
    }
    else
#ifndef GRIDCONTROL_NO_DRAGDROP
    if (m_MouseMode != MOUSE_PREPARE_DRAG) // not sizing or editing -- selecting
#endif
    {
	    SendMessageToParent(m_LeftClickDownCell.row, m_LeftClickDownCell.col, GVN_SELCHANGING);

        // If Ctrl pressed, save the current cell selection. This will get added
        // to the new cell selection at the end of the cell selection process
        m_PrevSelectedCellMap.RemoveAll();
        if (nFlags & MK_CONTROL)
        {
            for (POSITION pos = m_SelectedCellMap.GetStartPosition(); pos != NULL; )
            {
                DWORD key;
                HCellID cell;
                m_SelectedCellMap.GetNextAssoc(pos, key, (HCellID&)cell);
                m_PrevSelectedCellMap.SetAt(key, cell);
            }
        }
        
        if (m_LeftClickDownCell.row < GetFixedRowCount())
            OnFixedRowClick(m_LeftClickDownCell);
        else if (m_LeftClickDownCell.col < GetFixedColumnCount())
            OnFixedColumnClick(m_LeftClickDownCell);
        else
        {
            m_MouseMode = m_bListMode? MOUSE_SELECT_ROW : MOUSE_SELECT_CELLS;
            onSelecting(m_LeftClickDownCell);

            m_nTimerID = SetTimer(WM_LBUTTONDOWN, m_nTimerInterval, 0);
        }
    }   
    m_LastMousePoint = point;
}*/
/*
void HGridCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TRACE0("HGridCtrl::OnLButtonUp\n");

    CWnd::OnLButtonUp(nFlags, point);

    m_bLMouseButtonDown = false;

#ifndef _WIN32_WCE_NO_CURSOR
    ClipCursor(NULL);
#endif

    if (GetCapture()->GetSafeHwnd() == GetSafeHwnd())
    {
        ReleaseCapture();
        KillTimer(m_nTimerID);
        m_nTimerID = 0;
    }

    CPoint pointClickedRel;
    pointClickedRel = pointClicked( m_idCurrentCell.row, m_idCurrentCell.col, point);

    // m_MouseMode == MOUSE_PREPARE_EDIT only if user clicked down on current cell
    // and then didn't move mouse before clicking up (releasing button)
    if (m_MouseMode == MOUSE_PREPARE_EDIT)
    {
        OnEditCell(m_idCurrentCell.row, m_idCurrentCell.col, pointClickedRel, VK_LBUTTON);
    }
#ifndef GRIDCONTROL_NO_DRAGDROP
    // m_MouseMode == MOUSE_PREPARE_DRAG only if user clicked down on a selected cell
    // and then didn't move mouse before clicking up (releasing button)
    else if (m_MouseMode == MOUSE_PREPARE_DRAG) 
    {
        CGridCellBase* pCell = GetCell(m_idCurrentCell.row, m_idCurrentCell.col);
        if (pCell)
            pCell->OnClick( pointClicked( m_idCurrentCell.row, m_idCurrentCell.col, point) );
        SendMessageToParent(m_LeftClickDownCell.row, m_LeftClickDownCell.col, NM_CLICK);
	    SendMessageToParent(m_LeftClickDownCell.row, m_LeftClickDownCell.col, GVN_SELCHANGING);
        resetSelectedRange();
	    SendMessageToParent(m_idCurrentCell.row, m_idCurrentCell.col, GVN_SELCHANGED);
    }
#endif
    else if (m_MouseMode == MOUSE_SIZING_COL)
    {
        CRect rect;
        GetClientRect(rect);
        CRect invertedRect(m_LastMousePoint.x, rect.top, m_LastMousePoint.x + 2, rect.bottom);
        
        CDC* pDC = GetDC();
        if (pDC)
        {
            pDC->InvertRect(&invertedRect);
            ReleaseDC(pDC);
        }
        
        if (m_LeftClickDownPoint != point && (point.x != 0 || point.y != 0)) // 0 pt fix by email1@bierling.net
        {   
            CPoint start;
			//Used for merge cells
			//by Huang Wei
			m_LeftClickDownCell=GetMergeCellID(m_LeftClickDownCell);
            if (!GetCellOrigin(m_LeftClickDownCell, &start))
                return;

            int nColumnWidth = max(point.x - start.x, m_bAllowColHide? 0 : 1);
			//Used for merge cells
			//by Huang Wei
			int mergewidth=GetMergeCellWidth(m_LeftClickDownCell)-GetColumnWidth(m_LeftClickDownCell.col);
            SetColumnWidth(m_LeftClickDownCell.col, nColumnWidth-mergewidth);

            ResetScrollBars();
            Invalidate();
        }
    }
    else if (m_MouseMode == MOUSE_SIZING_ROW)
    {
        CRect rect;
        GetClientRect(rect);
        CRect invertedRect(rect.left, m_LastMousePoint.y, rect.right, m_LastMousePoint.y + 2);

        CDC* pDC = GetDC();
        if (pDC)
        {
            pDC->InvertRect(&invertedRect);
            ReleaseDC(pDC);
        }
        
        if (m_LeftClickDownPoint != point  && (point.x != 0 || point.y != 0)) // 0 pt fix by email1@bierling.net
        {
            CPoint start;
			//Used for merge cells
			//by Huang Wei
			m_LeftClickDownCell=GetMergeCellID(m_LeftClickDownCell);
            if (!GetCellOrigin(m_LeftClickDownCell, &start))
                return;
            
            int nRowHeight = max(point.y - start.y, m_bAllowRowHide? 0 : 1);
			//Used for merge cells
			//by Huang Wei
			int mergeheight=GetMergeCellHeight(m_LeftClickDownCell)-GetRowHeight(m_LeftClickDownCell.row);

            SetRowHeight(m_LeftClickDownCell.row, nRowHeight-mergeheight);
            ResetScrollBars();
            Invalidate();
        }
    }
    else
    {
	    SendMessageToParent(m_idCurrentCell.row, m_idCurrentCell.col, GVN_SELCHANGED);

        CGridCellBase* pCell = GetCell(m_idCurrentCell.row, m_idCurrentCell.col);
        if (pCell)
            pCell->OnClick( pointClicked( m_idCurrentCell.row, m_idCurrentCell.col, point) );
        SendMessageToParent(m_LeftClickDownCell.row, m_LeftClickDownCell.col, NM_CLICK);
    }
    
    m_MouseMode = MOUSE_NOTHING;
    
#ifndef _WIN32_WCE_NO_CURSOR
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
#endif
    
    if (!isValid(m_LeftClickDownCell))
        return;
    
    CWnd *pOwner = GetOwner();
    if (pOwner && IsWindow(pOwner->m_hWnd))
        pOwner->PostMessage(WM_COMMAND, MAKELONG(GetDlgCtrlID(), BN_CLICKED),
        (LPARAM) GetSafeHwnd());
}

#ifndef _WIN32_WCE
void HGridCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
    CWnd::OnRButtonDown(nFlags, point);
    m_bRMouseButtonDown = true;

#ifdef GRIDCONTROL_USE_TITLETIPS
	TRACE0("Hiding TitleTip\n");
    m_TitleTip.Hide();  // hide any titletips
#endif
}

// EFW - Added to forward right click to parent so that a context
// menu can be shown without deriving a new grid class.
void HGridCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
    CWnd::OnRButtonUp(nFlags, point);

    m_bRMouseButtonDown = false;

    HCellID FocusCell;
    FocusCell = cellFromPt(point);

    EndEditing();        // Auto-destroy any InPlaceEdit's

    // If not a valid cell, pass -1 for row and column
    if(!isValid(FocusCell))
        SendMessageToParent(-1, -1, NM_RCLICK);
    else
    {
        SetFocusCell(-1,-1);
        SetFocusCell(max(FocusCell.row, m_nFixedRows),
            max(FocusCell.col, m_nFixedCols));

        // tell the cell about it
        CGridCellBase* pCell = GetCell(FocusCell.row, FocusCell.col);
        if (pCell)
            pCell->OnRClick( pointClicked( FocusCell.row, FocusCell.col, point) );

        SendMessageToParent(FocusCell.row, FocusCell.col, NM_RCLICK);
    }
}
#endif

#if !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)
/////////////////////////////////////////////////////////////////////////////
// CGridCtrl printing

// EFW - New print margin support functions
//打印边界设置支持 ok
void HGridCtrl::setPrintMarginInfo(int nHeaderHeight, int nFooterHeight,
    int nLeftMargin, int nRightMargin, int nTopMargin,
    int nBottomMargin, int nGap)
{
    // If any parameter is -1, keep the existing setting
    if(nHeaderHeight > -1)
        m_nHeaderHeight = nHeaderHeight;
    if(nFooterHeight > -1)
        m_nFooterHeight = nFooterHeight;
    if(nLeftMargin > -1)
        m_nLeftMargin = nLeftMargin;
    if(nRightMargin > -1)
        m_nRightMargin = nRightMargin;
    if(nTopMargin > -1)
        m_nTopMargin = nTopMargin;
    if(nBottomMargin > -1)
        m_nBottomMargin = nBottomMargin;
    if(nGap > -1)
        m_nGap = nGap;
}

//ok
void HGridCtrl::getPrintMarginInfo(int &nHeaderHeight, int &nFooterHeight,
    int &nLeftMargin, int &nRightMargin, int &nTopMargin,
    int &nBottomMargin, int &nGap)
{
    nHeaderHeight = m_nHeaderHeight;
    nFooterHeight = m_nFooterHeight;
    nLeftMargin = m_nLeftMargin;
    nRightMargin = m_nRightMargin;
    nTopMargin = m_nTopMargin;
    nBottomMargin = m_nBottomMargin;
    nGap = m_nGap;
}

void HGridCtrl::Print(CPrintDialog* pPrntDialog =NULL)
{
    CDC dc;

    if (pPrntDialog == NULL)
    {
        CPrintDialog printDlg(false);
        if (printDlg.DoModal() != IDOK)             // Get printer settings from user
            return;

        dc.Attach(printDlg.GetPrinterDC());         // attach a printer DC
    }
    else
        dc.Attach(pPrntDialog->GetPrinterDC());     // attach a printer DC
        
    dc.m_bPrinting = true;

    CString strTitle;
    strTitle.LoadString(AFX_IDS_APP_TITLE);

    if( strTitle.IsEmpty() )
    {
        CWnd *pParentWnd = GetParent();
        while (pParentWnd)
        {
            pParentWnd->GetWindowText(strTitle);
            if (strTitle.GetLength())  // can happen if it is a CView, CChildFrm has the title
                break;
            pParentWnd = pParentWnd->GetParent();
        }
    }

    DOCINFO di;                                 // Initialise print doc details
    memset(&di, 0, sizeof (DOCINFO));
    di.cbSize = sizeof (DOCINFO);
    di.lpszDocName = strTitle;

    bool bPrintingOK = dc.StartDoc(&di);        // Begin a new print job

    CPrintInfo Info;
    Info.m_rectDraw.SetRect(0,0, dc.GetDeviceCaps(HORZRES), dc.GetDeviceCaps(VERTRES));

    OnBeginPrinting(&dc, &Info);                // Initialise printing
    for (UINT page = Info.GetMinPage(); page <= Info.GetMaxPage() && bPrintingOK; page++)
    {
        dc.StartPage();                         // begin new page
        Info.m_nCurPage = page;
        OnPrint(&dc, &Info);                    // Print page
        bPrintingOK = (dc.EndPage() > 0);       // end page
    }
    OnEndPrinting(&dc, &Info);                  // Clean up after printing

    if (bPrintingOK)
        dc.EndDoc();                            // end a print job
    else
        dc.AbortDoc();                          // abort job.

    dc.Detach();                                // detach the printer DC
}

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl printing overridables - for Doc/View print/print preview framework

// EFW - Various changes in the next few functions to support the
// new print margins and a few other adjustments.
void HGridCtrl::OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo)
{
    // OnBeginPrinting() is called after the user has committed to
    // printing by OK'ing the Print dialog, and after the framework
    // has created a CDC object for the printer or the preview view.

    // This is the right opportunity to set up the page range.
    // Given the CDC object, we can determine how many rows will
    // fit on a page, so we can in turn determine how many printed
    // pages represent the entire document.

    Q_ASSERT(pDC && pInfo);
    if (!pDC || !pInfo) return;

    // Get a DC for the current window (will be a screen DC for print previewing)
    CDC *pCurrentDC = GetDC();        // will have dimensions of the client area
    if (!pCurrentDC) return;

    CSize PaperPixelsPerInch(pDC->GetDeviceCaps(LOGPIXELSX), pDC->GetDeviceCaps(LOGPIXELSY));
    CSize ScreenPixelsPerInch(pCurrentDC->GetDeviceCaps(LOGPIXELSX), pCurrentDC->GetDeviceCaps(LOGPIXELSY));

    // Create the printer font
    int nFontSize = -10;
    CString strFontName = "Arial";
    m_PrinterFont.CreateFont(nFontSize, 0,0,0, FW_NORMAL, 0,0,0, DEFAULT_CHARSET,
                             OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
                             DEFAULT_PITCH | FF_DONTCARE, strFontName);

    CFont *pOldFont = pDC->SelectObject(&m_PrinterFont);

    // Get the average character width (in GridCtrl units) and hence the margins
    m_CharSize = pDC->GetTextExtent(_T("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSATUVWXYZ"),52);
    m_CharSize.cx /= 52;
    int nMargins = (m_nLeftMargin+m_nRightMargin)*m_CharSize.cx;

    // Get the page sizes (physical and logical)
    m_PaperSize = CSize(pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));

    if( m_bWysiwygPrinting)
    {
        m_LogicalPageSize.cx = ScreenPixelsPerInch.cx * m_PaperSize.cx / PaperPixelsPerInch.cx * 3 / 4;
        m_LogicalPageSize.cy = ScreenPixelsPerInch.cy * m_PaperSize.cy / PaperPixelsPerInch.cy * 3 / 4;
    }
    else
    {
        m_PaperSize = CSize(pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));

        m_LogicalPageSize.cx = GetVirtualWidth()+nMargins;
#ifdef _WIN32_WCE
        m_LogicalPageSize.cy = (m_LogicalPageSize.cx * m_PaperSize.cy) / m_PaperSize.cx;
#else
        m_LogicalPageSize.cy = MulDiv(m_LogicalPageSize.cx, m_PaperSize.cy, m_PaperSize.cx);
#endif
    }

    m_nPageHeight = m_LogicalPageSize.cy - GetFixedRowHeight()
                       - (m_nHeaderHeight+m_nFooterHeight + 2*m_nGap)*m_CharSize.cy;

    // Get the number of pages. Assumes no row is bigger than the page size.
    int nTotalRowHeight = 0;
    m_nNumPages = 1;
    for (int row = GetFixedRowCount(); row < GetRowCount(); row++)
    {
        nTotalRowHeight += GetRowHeight(row);
        if (nTotalRowHeight > m_nPageHeight) {
            m_nNumPages++;
            nTotalRowHeight = GetRowHeight(row);
        }
    }

    // now, figure out how many additional pages must print out if rows ARE bigger
    //  than page size
    int iColumnOffset = 0;
    int i1;
    for( i1=0; i1 < GetFixedColumnCount(); i1++)
    {
        iColumnOffset += GetColumnWidth( i1);
    }
    m_nPageWidth = m_LogicalPageSize.cx - iColumnOffset
                    - nMargins;
    m_nPageMultiplier = 1;

    if( m_bWysiwygPrinting)
    {
        int iTotalRowWidth = 0;
        for( i1 = GetFixedColumnCount(); i1 < GetColumnCount(); i1++)
        {
            iTotalRowWidth += GetColumnWidth( i1);
            if( iTotalRowWidth > m_nPageWidth)
            {
                m_nPageMultiplier++;
                iTotalRowWidth = GetColumnWidth( i1);
            }
        }
        m_nNumPages *= m_nPageMultiplier;
    }

    // Set up the print info
    pInfo->SetMaxPage(m_nNumPages);
    pInfo->m_nCurPage = 1;                        // start printing at page# 1

    ReleaseDC(pCurrentDC);
    pDC->SelectObject(pOldFont);
}

void HGridCtrl::OnPrint(CDC *pDC, CPrintInfo *pInfo)
{
    if (!pDC || !pInfo)
        return;

    //CRect rcPage(pInfo->m_rectDraw);
    CFont *pOldFont = pDC->SelectObject(&m_PrinterFont);

    // Set the page map mode to use GridCtrl units, and setup margin
    pDC->SetMapMode(MM_ANISOTROPIC);
    pDC->SetWindowExt(m_LogicalPageSize);
    pDC->SetViewportExt(m_PaperSize);
    pDC->SetWindowOrg(-m_nLeftMargin * m_CharSize.cx, 0);

    // Header
    pInfo->m_rectDraw.top    = 0;
    pInfo->m_rectDraw.left   = 0;
    pInfo->m_rectDraw.right  = m_LogicalPageSize.cx - (m_nLeftMargin + m_nRightMargin) * m_CharSize.cx;
    pInfo->m_rectDraw.bottom = m_nHeaderHeight * m_CharSize.cy;
    PrintHeader(pDC, pInfo);
    pDC->OffsetWindowOrg(0, -m_nHeaderHeight * m_CharSize.cy);

    // Gap between header and column headings
    pDC->OffsetWindowOrg(0, -m_nGap * m_CharSize.cy);

    pDC->OffsetWindowOrg(0, -GetFixedRowHeight());

    // We need to find out which row to start printing for this page.
    int nTotalRowHeight = 0;
    UINT nNumPages = 1;
    m_nCurrPrintRow = GetFixedRowCount();


    // Not only the row, but we need to figure out column, too

    // Can print 4 pages, where page 1 and 2 represent the same rows but
    // with different WIDE columns.
    //
    // .......
    // .1 .2 .  If representing page 3  -->    iPageIfIgnoredWideCols = 2
    // .......                                 iWideColPageOffset = 0
    // .3 .4 .  If representing page 2  -->    iPageIfIgnoredWideCols = 1
    // .......                                 iWideColPageOffset = 1

    int iPageIfIgnoredWideCols = pInfo->m_nCurPage / m_nPageMultiplier;
    int iWideColPageOffset = pInfo->m_nCurPage - ( iPageIfIgnoredWideCols * m_nPageMultiplier);
    if( iWideColPageOffset > 0)
        iPageIfIgnoredWideCols++;

    if( iWideColPageOffset == 0)
        iWideColPageOffset = m_nPageMultiplier;
    iWideColPageOffset--;

    // calculate current print row based on iPageIfIgnoredWideCols
    while(  m_nCurrPrintRow < GetRowCount()
            && (int)nNumPages < iPageIfIgnoredWideCols)
    {
        nTotalRowHeight += GetRowHeight(m_nCurrPrintRow);
        if (nTotalRowHeight > m_nPageHeight) {
            nNumPages++;
            if ((int)nNumPages == iPageIfIgnoredWideCols) break;
            nTotalRowHeight = GetRowHeight(m_nCurrPrintRow);
        }
        m_nCurrPrintRow++;
    }

    m_nPrintColumn = GetFixedColumnCount();
    int iTotalRowWidth = 0;
    int i1, i2;

    // now, calculate which print column to start displaying
    for( i1 = 0; i1 < iWideColPageOffset; i1++)
    {
        for( i2 = m_nPrintColumn; i2 < GetColumnCount(); i2++)
        {
            iTotalRowWidth += GetColumnWidth( i2);
            if( iTotalRowWidth > m_nPageWidth)
            {
                m_nPrintColumn = i2;
                iTotalRowWidth = 0;
                break;
            }
        }
    }


    PrintRowButtons( pDC, pInfo);   // print row buttons on each page
    int iColumnOffset = 0;
    for( i1=0; i1 < GetFixedColumnCount(); i1++)
    {
        iColumnOffset += GetColumnWidth( i1);
    }

    // Print the column headings
    pInfo->m_rectDraw.bottom = GetFixedRowHeight();

    if( m_nPrintColumn == GetFixedColumnCount())
    {
        // have the column headings fcn draw the upper left fixed cells
        //  for the very first columns, only
        pDC->OffsetWindowOrg( 0, +GetFixedRowHeight());

        m_nPageWidth += iColumnOffset;
        m_nPrintColumn = 0;
        PrintColumnHeadings(pDC, pInfo);
        m_nPageWidth -= iColumnOffset;
        m_nPrintColumn = GetFixedColumnCount();

        pDC->OffsetWindowOrg( -iColumnOffset, -GetFixedRowHeight());
    }
    else
    {
        // changed all of this here to match above almost exactly same
        pDC->OffsetWindowOrg( 0, +GetFixedRowHeight());

        m_nPageWidth += iColumnOffset;

        // print from column 0 ... last column that fits on the current page
        PrintColumnHeadings(pDC, pInfo);
        
        m_nPageWidth -= iColumnOffset;
 
        pDC->OffsetWindowOrg( -iColumnOffset, -GetFixedRowHeight());
    }


    if (m_nCurrPrintRow >= GetRowCount()) return;

    // Draw as many rows as will fit on the printed page.
    // Clip the printed page so that there is no partially shown
    // row at the bottom of the page (the same row which will be fully
    // shown at the top of the next page).

    bool bFirstPrintedRow = true;
    CRect rect;
    rect.bottom = -1;

	//Used for merge cells
	//by Huang Wei
	int Row=m_nCurrPrintRow;
	CRect range;
    range.bottom = -1;
    while (Row < GetRowCount())
    {
        range.top = range.bottom+1;
        range.bottom = range.top + GetRowHeight(Row) - 1;

        if (range.bottom > m_nPageHeight) 
		{
			range.bottom=range.top;
			break;            
		}
        Row++;
    }

    while (m_nCurrPrintRow < GetRowCount())
    {
        rect.top = rect.bottom+1;
        rect.bottom = rect.top + GetRowHeight(m_nCurrPrintRow) - 1;

        if (rect.bottom > m_nPageHeight) break;            // Gone past end of page

        rect.right = -1;

        // modified to allow printing of wide grids on multiple pages
        for (int col = m_nPrintColumn; col < GetColumnCount(); col++)
        {
            rect.left = rect.right+1;
            rect.right =    rect.left
                            + GetColumnWidth( col)
                            - 1;

            if( rect.right > m_nPageWidth)
                break;

            CGridCellBase* pCell = GetCell(m_nCurrPrintRow, col);
            if (pCell)
			{      
				//Used for merge cells
				//by Huang Wei
				int row=m_nCurrPrintRow;
				if(!pCell->IsMerged())
				{
					if(!pCell->IsMergeWithOthers())
					{
						pCell->PrintCell(pDC, row, col, rect);
					}
				}
				else
				{
					CRect mergerect=rect;
                    if(cellRangeRect(pCell->m_MergeRange,&mergerect))
					{
						//mergerect.DeflateRect(0,0,1,1);
						mergerect.OffsetRect(rect.TopLeft()-mergerect.TopLeft());
						pCell->PrintCell(pDC, row, col, mergerect);
					}
				}
			}

            //if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_HORZ)
            //{
            //    int Overlap = (col == 0)? 0:1;
            //    pDC->MoveTo(rect.left-Overlap, rect.bottom);
            //    pDC->LineTo(rect.right, rect.bottom);
            //    if (m_nCurrPrintRow == 0) {
            //        pDC->MoveTo(rect.left-Overlap, rect.top);
            //        pDC->LineTo(rect.right, rect.top);
            //    }
            //}
            //if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_VERT)
            //{
            //    int Overlap = (bFirstPrintedRow)? 0:1;
            //    pDC->MoveTo(rect.right, rect.top-Overlap);
            //    pDC->LineTo(rect.right, rect.bottom);
            //    if (col == 0) {
            //        pDC->MoveTo(rect.left, rect.top-Overlap);
            //        pDC->LineTo(rect.left, rect.bottom);
            //    }
            //}

        }
        m_nCurrPrintRow++;
        bFirstPrintedRow = false;
    }

	//Used for merge cells
	//by Huang Wei
	CRect white_rect(CPoint(0,0),m_LogicalPageSize);
	white_rect.top=range.bottom+1;
	pDC->FillSolidRect(white_rect,RGB(255,255,255));

    // Footer
    pInfo->m_rectDraw.bottom = m_nFooterHeight * m_CharSize.cy;
    pDC->SetWindowOrg( -m_nLeftMargin * m_CharSize.cx,
        -m_LogicalPageSize.cy + m_nFooterHeight * m_CharSize.cy);
    PrintFooter(pDC, pInfo);

    // SetWindowOrg back for next page
    pDC->SetWindowOrg(0,0);

    pDC->SelectObject(pOldFont);
}


// added by M.Fletcher 12/17/00
void HGridCtrl::PrintFixedRowCells(int nStartColumn, int nStopColumn, int& row, CRect& rect,
                                   CDC *pDC, bool& bFirst)
{
   // print all cells from nStartColumn to nStopColumn on row
   for (int col =nStartColumn; col < nStopColumn; col++)
   {
      rect.left = rect.right+1;
      rect.right =    rect.left + GetColumnWidth( col) - 1;

      if( rect.right > m_nPageWidth)
         break;
      
      CGridCellBase* pCell = GetCell(row, col);
      if (pCell)
	  {
			//Used for merge cells
			//by Huang Wei
			int row=m_nCurrPrintRow;
			if(!pCell->IsMerged())
			{
				if(!pCell->IsMergeWithOthers())
				{
					pCell->PrintCell(pDC, row, col, rect);
				}
			}
			else
			{
				CRect mergerect=rect;
                if(cellRangeRect(pCell->m_MergeRange,&mergerect))
				{
					//mergerect.DeflateRect(0,0,1,1);
					mergerect.OffsetRect(rect.TopLeft()-mergerect.TopLeft());
					pCell->PrintCell(pDC, row, col, mergerect);
				}
			}
	  }
      
      //if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_HORZ)
      //{
      //   int Overlap = (col == 0)? 0:1;
      //   
      //   pDC->MoveTo(rect.left-Overlap, rect.bottom);
      //   pDC->LineTo(rect.right, rect.bottom);
      //   
      //   if (row == 0)
      //   {
      //      pDC->MoveTo(rect.left-Overlap, rect.top);
      //      pDC->LineTo(rect.right, rect.top);
      //    }
      //}

      //if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_VERT)
      //{
      //   int Overlap = (row == 0)? 0:1;

      //   pDC->MoveTo(rect.right, rect.top-Overlap);
      //   pDC->LineTo(rect.right, rect.bottom);
      //   
      //   if( bFirst)
      //   {
      //      pDC->MoveTo(rect.left-1, rect.top-Overlap);
      //      pDC->LineTo(rect.left-1, rect.bottom);
      //      bFirst = false;
      //    }

      // }

   } // end of column cells loop


} // end of HGridCtrl::PrintFixedRowCells

void HGridCtrl::PrintColumnHeadings(CDC *pDC, CPrintInfo* pInfo)
{
    CFont *pOldFont = pDC->SelectObject(&m_PrinterFont);

    CRect rect;
    rect.bottom = -1;

    bool bFirst = true;
    bool bOriginal;
    

    // modified to allow column hdr printing of multi-page wide grids
    for (int row = 0; row < GetFixedRowCount(); row++)
    {
        rect.top = rect.bottom+1;
        rect.bottom = rect.top + GetRowHeight(row) - 1;

        rect.right = -1;
 
        // if printColumn > fixedcolumncount we are on page 2 or more
        // lets printout those fixed cell headings again the 1 or more that would be missed
        // added by M.Fletcher 12/17/00
        if(m_nPrintColumn>= GetFixedColumnCount())
        {
           bOriginal=bFirst;
           // lets print the missing fixed cells on left first out to last fixed column
           PrintFixedRowCells(0,GetFixedColumnCount(), row, rect, pDC, bFirst);
           bFirst=bOriginal;
        }

        // now back to normal business print cells in heading after all fixed columns
        PrintFixedRowCells(m_nPrintColumn, GetColumnCount(), row, rect, pDC, bFirst);
        
    } // end of Row Loop

    pDC->SelectObject(pOldFont);
} // end of HGridCtrl::PrintColumnHeadings


*/
/*****************************************************************************
Prints line of row buttons on each page of the printout.  Assumes that
the window origin is setup before calling

*****************************************************************************/
/*
void HGridCtrl::PrintRowButtons(CDC *pDC, CPrintInfo* pInfo)
{
    CFont *pOldFont = pDC->SelectObject(&m_PrinterFont);

    CRect rect;
    rect.right = -1;

    bool bFirst = true;
    for( int iCol = 0; iCol < GetFixedColumnCount(); iCol++)
    {
        rect.left = rect.right+1;
        rect.right =    rect.left
                        + GetColumnWidth( iCol)
                        - 1;

        rect.bottom = -1;
        for( int iRow = m_nCurrPrintRow; iRow < GetRowCount(); iRow++)
        {
            rect.top = rect.bottom+1;
            rect.bottom = rect.top + GetRowHeight( iRow) - 1;

            if( rect.bottom > m_nPageHeight)
                break;

            CGridCellBase* pCell = GetCell(iRow, iCol);
            if (pCell)
                pCell->PrintCell(pDC, iRow, iCol, rect);

            if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_HORZ)
            {
                int Overlap = (iCol == 0)? 0:1;
                pDC->MoveTo(rect.left-Overlap, rect.bottom);
                pDC->LineTo(rect.right, rect.bottom);
                if( bFirst) {
                    pDC->MoveTo(rect.left-Overlap, rect.top-1);
                    pDC->LineTo(rect.right, rect.top-1);
                    bFirst = false;
                }
            }
            if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_VERT)
            {
                int Overlap = (iRow == 0)? 0:1;
                pDC->MoveTo(rect.right, rect.top-Overlap);
                pDC->LineTo(rect.right, rect.bottom);
                if (iCol == 0) {
                    pDC->MoveTo(rect.left, rect.top-Overlap);
                    pDC->LineTo(rect.left, rect.bottom);
                }
            }

        }
    }
    pDC->SelectObject(pOldFont);
}

void HGridCtrl::PrintHeader(CDC *pDC, CPrintInfo *pInfo)
{
    // print App title on top right margin
    CString strRight;
    strRight.LoadString(AFX_IDS_APP_TITLE);

    // print parent window title in the centre (Gert Rijs)
    CString strCenter;
    CWnd *pParentWnd = GetParent();
    while (pParentWnd)
    {
        pParentWnd->GetWindowText(strCenter);
        if (strCenter.GetLength())  // can happen if it is a CView, CChildFrm has the title
            break;
        pParentWnd = pParentWnd->GetParent();
    }

    CFont   BoldFont;
    LOGFONT lf;

    //create bold font for header and footer
    VERIFY(m_PrinterFont.GetLogFont(&lf));
    lf.lfWeight = FW_BOLD;
    VERIFY(BoldFont.CreateFontIndirect(&lf));

    CFont *pNormalFont = pDC->SelectObject(&BoldFont);
    int nPrevBkMode = pDC->SetBkMode(TRANSPARENT);

    CRect   rc(pInfo->m_rectDraw);
    if( !strCenter.IsEmpty() )
        pDC->DrawText( strCenter, &rc, DT_CENTER | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER);
    if( !strRight.IsEmpty() )
        pDC->DrawText( strRight, &rc, DT_RIGHT | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER);

    pDC->SetBkMode(nPrevBkMode);
    pDC->SelectObject(pNormalFont);
    BoldFont.DeleteObject();

    // draw ruled-line across top
    pDC->SelectStockObject(BLACK_PEN);
    pDC->MoveTo(rc.left, rc.bottom);
    pDC->LineTo(rc.right, rc.bottom);
}

//print footer with a line and date, and page number
void HGridCtrl::PrintFooter(CDC *pDC, CPrintInfo *pInfo)
{
    // page numbering on left
    CString strLeft;
    strLeft.Format(_T("Page %d of %d"), pInfo->m_nCurPage, pInfo->GetMaxPage() );

    // date and time on the right
    CString strRight;
    COleDateTime t = COleDateTime::GetCurrentTime();
    strRight = t.Format(_T("%c"));
    
    CRect rc(pInfo->m_rectDraw);

    // draw ruled line on bottom
    pDC->SelectStockObject(BLACK_PEN);
    pDC->MoveTo(rc.left, rc.top);
    pDC->LineTo(rc.right, rc.top);

    CFont BoldFont;
    LOGFONT lf;

    //create bold font for header and footer
    m_PrinterFont.GetLogFont(&lf);
    lf.lfWeight = FW_BOLD;
    BoldFont.CreateFontIndirect(&lf);

    CFont *pNormalFont = pDC->SelectObject(&BoldFont);
    int nPrevBkMode = pDC->SetBkMode(TRANSPARENT);

    // EFW - Bug fix - Force text color to black.  It doesn't always
    // get set to a printable color when it gets here.
    pDC->SetTextColor(RGB(0, 0, 0));

    if( !strLeft.IsEmpty() )
        pDC->DrawText( strLeft, &rc, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER);
    if( !strRight.IsEmpty() )
        pDC->DrawText( strRight, &rc, DT_RIGHT | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER);

    pDC->SetBkMode(nPrevBkMode);
    pDC->SelectObject(pNormalFont);
    BoldFont.DeleteObject();
}

void HGridCtrl::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
    m_PrinterFont.DeleteObject();
}

#endif  // !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)


#ifndef _WIN32_WCE
/////////////////////////////////////////////////////////////////////////////
// CGridCtrl persistance

bool HGridCtrl::Save(LPCTSTR filename, TCHAR chSeparator)
{
    CStdioFile File;
    CFileException ex;
	CString strSeparator(chSeparator);

    if (!File.Open(filename, CFile::modeWrite | CFile::modeCreate| CFile::typeText, &ex))
    {
        ex.ReportError();
        return false;
    }

    TRY
    {
        int nNumColumns = GetColumnCount();
        for (int i = 0; i < GetRowCount(); i++)
        {
            for (int j = 0; j < nNumColumns; j++)
            {
                File.WriteString(itemText(i,j));
                File.WriteString((j==(nNumColumns-1))? _T("\n"): strSeparator);
            }
        }

        File.Close();
    }

    CATCH (CFileException, e)
    {
        AfxMessageBox(_T("Unable to save grid list"));
        return false;
    }
    END_CATCH

    return true;
}

bool HGridCtrl::Load(LPCTSTR filename, TCHAR chSeparator)
{
    if (GetVirtualMode())
        return false;

    TCHAR *token, *end;
    TCHAR buffer[1024];
    CStdioFile File;
    CFileException ex;

    if (!File.Open(filename, CFile::modeRead | CFile::typeText))
    {
        ex.ReportError();
        return false;
    }

    DeleteAllItems();

    TRY
    {
        // Read Header off file
        File.ReadString(buffer, 1024);

        // Get first token
        for (token=buffer, end=buffer;
             *end && (*end != chSeparator) && (*end != _T('\n')); 
             end++)
            ;

        if ((*end == _T('\0')) && (token == end))
            token = NULL;

        *end = _T('\0');

        while (token)
        {
            InsertColumn(token);

            // Get next token
            for (token=++end; *end && (*end != chSeparator) && (*end != _T('\n'));
               end++)
                ;

            if ((*end == _T('\0')) && (token == end))
                token = NULL;

            *end = _T('\0');
        }

        // Read in rest of data
        int nItem = 0;
        while (File.ReadString(buffer, 1024))
        {
            // Get first token
            for (token=buffer, end=buffer;
              *end && (*end != chSeparator) && (*end != _T('\n')); end++)
                ;

            if ((*end == _T('\0')) && (token == end))
                token = NULL;

            *end = _T('\0');

            int nSubItem = 0;
            while (token)
            {
                if (!nSubItem)
                    InsertRow(token);
                else
                    SetItemText(nItem, nSubItem, token);

                // Get next token
                for (token=++end; *end && (*end != chSeparator) && (*end != _T('\n'));
                  end++)
                    ;

                if ((*end == _T('\0')) && (token == end))
                    token = NULL;

                *end = _T('\0');

                nSubItem++;
            }
            nItem++;
        }

        AutoSizeColumns(GetAutoSizeStyle());
        File.Close();
    }

    CATCH (CFileException, e)
    {
        AfxMessageBox(_T("Unable to load grid data"));
        return false;
    }
    END_CATCH

    return true;
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl overrideables

#ifndef GRIDCONTROL_NO_DRAGDROP
// This is no longer needed since I've changed to OLE drag and drop - but it's
// still cool code. :)
CImageList* HGridCtrl::CreateDragImage(CPoint *pHotSpot)
{
    CDC* pDC = GetDC();
    if (!pDC)
        return NULL;

    CRect rect;
    HCellID cell = GetFocusCell();
    if (!GetCellRect(cell.row, cell.col, rect))
        return NULL;
    
    // Translate coordinate system
    rect.BottomRight() = CPoint(rect.Width(), rect.Height());
    rect.TopLeft()     = CPoint(0, 0);
    *pHotSpot = rect.BottomRight(); 
    
    // Create a new imagelist (the caller of this function has responsibility
    // for deleting this list)
    CImageList* pList = new CImageList;
    if (!pList || !pList->Create(rect.Width(), rect.Height(), ILC_MASK, 1, 1))
    {    
        if (pList)
            delete pList;
        return NULL;
    }
    
    // Create mem DC and bitmap
    CDC MemDC;
    CBitmap bm;
    MemDC.CreateCompatibleDC(pDC);
    bm.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
    CBitmap* pOldBitmap = MemDC.SelectObject(&bm);
    MemDC.SetWindowOrg(0, 0);
    
    // Draw cell onto bitmap in memDC
    CGridCellBase* pCell = GetCell(cell.row, cell.col);
    if (pCell)
        pCell->Draw(&MemDC, cell.row, cell.col, rect, false);
    
    // Clean up
    MemDC.SelectObject(pOldBitmap);
    ReleaseDC(pDC);
    
    // Add the bitmap we just drew to the image list.
    pList->Add(&bm, GetDefaultCell(false, false)->GetBackClr());
    bm.DeleteObject();

    return pList;
}
#endif
*/
void HGridCtrl::onFixedRowClick(HCellID& cell)
{
    if (!isValid(cell))
        return;

    //不排序了
    /*
    if (GetHeaderSort())
    {
        CWaitCursor waiter;
        if (cell.col == GetSortColumn())
            SortItems(cell.col, !GetSortAscending());
        else
            SortItems(cell.col, true);
        Invalidate();
    }*/

	// Did the user click on a fixed column cell (so the cell was within the overlap of
	// fixed row and column cells) - (fix by David Pritchard)
    if (isFixedColumnSelection())
    {
        if (cell.col < fixedColumnCount())
        {
            m_MouseMode = MOUSE_SELECT_ALL;
            onSelecting(cell);
        }
        else 
        {
            m_MouseMode = MOUSE_SELECT_COL;
            onSelecting(cell);
        }
    }
}

void HGridCtrl::onFixedColumnClick(HCellID& cell)
{
    if (!isValid(cell))
        return;
	// Did the user click on a fixed row cell (so the cell was within the overlap of
	// fixed row and column cells) - (fix by David Pritchard)
    if (isFixedRowSelection())
    {
        if (cell.row < fixedRowCount())
        {
            m_MouseMode = MOUSE_SELECT_ALL;
            onSelecting(cell);
        }
        else
        {
            m_MouseMode = MOUSE_SELECT_ROW;
            onSelecting(cell);
        }
    }
}

// Gets the extent of the text pointed to by str (no CDC needed)
// By default this uses the selected font (which is a bigger font)
QSize HGridCtrl::textExtent(int nRow, int nCol, const QString& str)
{
    HGridCellBase* pCell = getCell(nRow, nCol);
    if (!pCell)
        return QSize(0, 0);
    else
        return pCell->textExtent(str);
}


// virtual
void HGridCtrl::onEditCell(int nRow, int nCol, QPoint point)
{
    // Can we do it?
    HGridCellBase* pCell = getCell(nRow, nCol);
    if (NULL == pCell || !isCellEditable(nRow, nCol))
        return;

    QRect rect;
    if (!cellRect(nRow,nCol, rect))
        return;

    if (pCell)
        pCell->edit(nRow, nCol, rect, point);

}

// virtual
void HGridCtrl::endEditing()
{
    HCellID cell = focusCell();
    if (!isValid(cell)) return;
    HGridCellBase *pCell = getCell(cell.row, cell.col);
    if (pCell)
        pCell->endEdit();
}

// virtual
void HGridCtrl::onEndEditCell(int nRow, int nCol, QString str)
{
    QString strCurrentText = itemText(nRow, nCol);
    if (strCurrentText != str)
    {
        setItemText(nRow, nCol, str);
        setModified(true, nRow, nCol);
        redrawCell(nRow, nCol);
    }

    HGridCellBase* pCell = getCell(nRow, nCol);
    if (pCell)
        pCell->OnEndEdit();

}

/*
// If this returns false then the editing isn't allowed
// virtual
bool HGridCtrl::ValidateEdit(int nRow, int nCol, LPCTSTR str)
{
    CGridCellBase* pCell = GetCell(nRow, nCol);
    Q_ASSERT(pCell);
    if (!pCell)
        return true;

    return pCell->ValidateEdit(str);
}

/*
//Merge the selected cells 
//by Huang Wei
HCellID HGridCtrl::GetMergeCellID(HCellID cell)
{
    CGridCellBase *pCell = (CGridCellBase*) GetCell(cell);
	if(pCell && pCell->IsMergeWithOthers())
		return pCell->GetMergeCellID();
	return cell;
}
//Merge the selected cells 
//by Huang Wei
CGridCellBase* HGridCtrl::GetCell(HCellID cell)
{
	return GetCell(cell.row,cell.col);
}
//Merge the selected cells 
//by Huang Wei
void HGridCtrl::MergeSelectedCells()
{
    HCellRange range=selectedCellRange();
	if(range.GetColSpan()<=1 && range.GetRowSpan()<=1)
		return;
	MergeCells(range.GetMinRow(), range.GetMinCol(), range.GetMaxRow(), range.GetMaxCol());
}
//Merge the selected cells 
//by Huang Wei
void HGridCtrl::MergeCells(int nStartRow, int nStartCol, int nEndRow, int nEndCol)
{
	for(int row=nStartRow;row<=nEndRow;row++)
	{
		for(int col=nStartCol;col<=nEndCol;col++)
		{
		    CGridCellBase *pCell = (CGridCellBase*) GetCell(row,col);
            pCell->Show(false);
			if(row==nStartRow && col==nStartCol)
			{
                HCellRange range(nStartRow,  nStartCol,  nEndRow,  nEndCol);
				pCell->SetMergeRange(range);
			}
			else
			{
                HCellID cell(nStartRow,nStartCol);
				pCell->SetMergeCellID(cell);
			}

		}
	}
	Invalidate();
}
//Merge the selected cells 
//by Huang Wei
void HGridCtrl::UnMergeSelectedCells()
{
    HCellRange range=selectedCellRange();
	UnMergeCells(range.GetMinRow(), range.GetMinCol(), range.GetMaxRow(), range.GetMaxCol());
}
//Merge the selected cells 
//by Huang Wei
void HGridCtrl::UnMergeCells(int nStartRow, int nStartCol, int nEndRow, int nEndCol)
{
	for(int row=nStartRow;row<=nEndRow;row++)
	{
		for(int col=nStartCol;col<=nEndCol;col++)
		{
		    CGridCellBase *pCell = (CGridCellBase*) GetCell(row,col);
			if(pCell->IsMerged())
			{
				for(int mergerow=pCell->GetMergeRange().GetMaxRow();mergerow>=pCell->GetMergeRange().GetMinRow();mergerow--)
					for(int mergecol=pCell->GetMergeRange().GetMaxCol();mergecol>=pCell->GetMergeRange().GetMinCol();mergecol--)
					{
						if(pCell->GetMergeRange().GetMaxRow()==-1 || pCell->GetMergeRange().GetMaxCol()==-1)
							break;
						CGridCellBase *pMergedCell = (CGridCellBase*) GetCell(mergerow,mergecol);
						pMergedCell->UnMerge();
					}

			}
			else
				pCell->UnMerge();
		}
	}
	Invalidate();
}
//Merge the selected cells 
//by Huang Wei
int  HGridCtrl::GetMergeCellWidth(HCellID cell)
{
    HCellID mergecell =GetMergeCellID(cell);
    CGridCellBase *pCell = (CGridCellBase*) GetCell(mergecell);
	if(!pCell->IsMerged())
		return GetColumnWidth(cell.col);
	int width=0;
	for(int mergecol=pCell->GetMergeRange().GetMaxCol();mergecol>=pCell->GetMergeRange().GetMinCol();mergecol--)
	{
		width+=GetColumnWidth(mergecol);
	}
	return width;
}
//Merge the selected cells 
//by Huang Wei
int  HGridCtrl::GetMergeCellHeight(HCellID cell)
{
    HCellID mergecell =GetMergeCellID(cell);
    CGridCellBase *pCell = (CGridCellBase*) GetCell(mergecell);
	if(!pCell->IsMerged())
		return GetRowHeight(cell.row);
	int height=0;
	for(int mergerow=pCell->GetMergeRange().GetMaxRow();mergerow>=pCell->GetMergeRange().GetMinRow();mergerow--)
	{
		height+=GetRowHeight(mergerow);
	}
	return height;
}
*/
