/////////////////////////////////////////////////////////////////////////////
// GridCtrl.h : header file
//
// MFC Grid Control - main header
//
// Written by Chris Maunder <cmaunder@mail.com>
// Copyright (c) 1998-2002. All Rights Reserved.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name and all copyright 
// notices remains intact. 
//
// An email letting me know how you are using it would be nice as well. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
// For use with CGridCtrl v2.20+
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRIDCTRL_H__519FA702_722C_11D1_ABBA_00A0243D1382__INCLUDED_)
#define AFX_GRIDCTRL_H__519FA702_722C_11D1_ABBA_00A0243D1382__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "hcellrange.h"
#include "hgridcell.h"
#include "hgridcelldef.h"
#include <afxtempl.h>


///////////////////////////////////////////////////////////////////////////////////
// Defines - these determine the features (and the final size) of the final code
///////////////////////////////////////////////////////////////////////////////////

//#define GRIDCONTROL_NO_TITLETIPS   // Do not use titletips for cells with large data
//#define GRIDCONTROL_NO_DRAGDROP    // Do not use OLE drag and drop
//#define GRIDCONTROL_NO_CLIPBOARD   // Do not use clipboard routines

#ifdef _WIN32_WCE
#   define GRIDCONTROL_NO_TITLETIPS   // Do not use titletips for cells with large data
#   define GRIDCONTROL_NO_DRAGDROP    // Do not use OLE drag and drop
#   define GRIDCONTROL_NO_CLIPBOARD   // Do not use clipboard routines
#   define GRIDCONTROL_NO_PRINTING    // Do not use printing routines
#   ifdef WCE_NO_PRINTING			  // Older versions of CE had different #def's
#       define _WIN32_WCE_NO_PRINTING
#   endif
#   ifdef WCE_NO_CURSOR
#       define _WIN32_WCE_NO_CURSOR
#   endif
#endif  // _WIN32_WCE

// Use this as the classname when inserting this control as a custom control
// in the MSVC++ dialog editor
#define GRIDCTRL_CLASSNAME    _T("MFCGridCtrl")  // Window class name
#define IDC_INPLACE_CONTROL   8                  // ID of inplace edit controls




class HGridCtrl;

/////////////////////////////////////////////////////////////////////////////
/*
 *原CGridCtrl类继承CWnd窗口基类,
 *在Qt中使用QAscrollArea类，主要是自带滚动条
 * 或者QWidget窗口类，本例子采用QWidget配合MFC的代码
*/

class HGridCtrl : public CWnd,public QWidget
{
    Q_OBJECT
    friend class HGridCell;
    friend class HGridCellBase;

// Construction
public:
    CGridCtrl(int nRows = 0, int nCols = 0, int nFixedRows = 0, int nFixedCols = 0);

    //QWidget不需要创建窗口函数
    //BOOL Create(const RECT& rect, CWnd* parent, UINT nID,DWORD dwStyle = WS_CHILD | WS_BORDER | WS_TABSTOP | WS_VISIBLE);

///////////////////////////////////////////////////////////////////////////////////
// Attributes
///////////////////////////////////////////////////////////////////////////////////
public:
    //行、列数目  Fixed 类似表头的行数和列数
    int  GetRowCount() const                    { return m_nRows; }
    int  GetColumnCount() const                 { return m_nCols; }
    int  GetFixedRowCount() const               { return m_nFixedRows; }
    int  GetFixedColumnCount() const            { return m_nFixedCols; }
    BOOL SetRowCount(int nRows = 10);
    BOOL SetColumnCount(int nCols = 10);
    BOOL SetFixedRowCount(int nFixedRows = 1);
    BOOL SetFixedColumnCount(int nFixedCols = 1);

    //行高、列宽
    int  GetRowHeight(int nRow) const;
    BOOL SetRowHeight(int row, int height);
    int  GetColumnWidth(int nCol) const;
    BOOL SetColumnWidth(int col, int width);

    //获取单元格的左上角位置或者矩形区域
    BOOL GetCellOrigin(int nRow, int nCol, LPPOINT p);
    BOOL GetCellOrigin(const CCellID& cell, LPPOINT p);
    BOOL GetCellRect(int nRow, int nCol, LPRECT pRect);
    BOOL GetCellRect(const CCellID& cell, LPRECT pRect);

    BOOL GetTextRect(const CCellID& cell, LPRECT pRect);
    BOOL GetTextRect(int nRow, int nCol, LPRECT pRect);

    //获取point点的行列号。如果TRUE是固定行列，如果是false则是开始到point的行列等...... 待定--huangw
    CCellID GetCellFromPt(CPoint point, BOOL bAllowFixedCellCheck = TRUE);

    //获取固定行的行高 固定列的列宽
    int  GetFixedRowHeight() const;
    int  GetFixedColumnWidth() const;

    //获取所有行行高，所有列列宽 ---huangw
    long GetVirtualWidth() const;
    long GetVirtualHeight() const;

    //获取行列单元格字符串的宽度
    CSize GetTextExtent(int nRow, int nCol, LPCTSTR str);
    // EFW - Get extent of current text in cell
    inline CSize GetCellTextExtent(int nRow, int nCol)  { return GetTextExtent(nRow, nCol, GetItemText(nRow,nCol)); }

    //设置网格背景颜色、网格线颜色
    void     SetGridBkColor(COLORREF clr)         { m_crGridBkColour = clr;           }
    COLORREF GetGridBkColor() const               { return m_crGridBkColour;          }
    void     SetGridLineColor(COLORREF clr)       { m_crGridLineColour = clr;         }
    COLORREF GetGridLineColor() const             { return m_crGridLineColour;        }

    //提示窗口背景颜色、提示文字颜色
	void	 SetTitleTipBackClr(COLORREF clr = CLR_DEFAULT) { m_crTTipBackClr = clr;  }
	COLORREF GetTitleTipBackClr()				            { return m_crTTipBackClr; }
	void	 SetTitleTipTextClr(COLORREF clr = CLR_DEFAULT) { m_crTTipTextClr = clr;  }
	COLORREF GetTitleTipTextClr()				            { return m_crTTipTextClr; }

    // ***************************************************************************** //
    // These have been deprecated. Use GetDefaultCell and then set the colors
    //
    void     SetTextColor(COLORREF clr)      { m_cellDefault.SetTextClr(clr);        }
    COLORREF GetTextColor()                  { return m_cellDefault.GetTextClr();    }
    void     SetTextBkColor(COLORREF clr)    { m_cellDefault.SetBackClr(clr);        }
    COLORREF GetTextBkColor()                { return m_cellDefault.GetBackClr();    }
    void     SetFixedTextColor(COLORREF clr) { m_cellFixedRowDef.SetTextClr(clr); 
                                               m_cellFixedColDef.SetTextClr(clr); 
                                               m_cellFixedRowColDef.SetTextClr(clr); }
    COLORREF GetFixedTextColor() const       { return m_cellFixedRowDef.GetTextClr(); }
    void     SetFixedBkColor(COLORREF clr)   { m_cellFixedRowDef.SetBackClr(clr); 
                                               m_cellFixedColDef.SetBackClr(clr); 
                                               m_cellFixedRowColDef.SetBackClr(clr); }
    COLORREF GetFixedBkColor() const         { return m_cellFixedRowDef.GetBackClr(); }
    void     SetGridColor(COLORREF clr)      { SetGridLineColor(clr);                }
    COLORREF GetGridColor()                  { return GetGridLineColor();            }
    void     SetBkColor(COLORREF clr)        { SetGridBkColor(clr);                  }
    COLORREF GetBkColor()                    { return GetGridBkColor();              }

    void     SetDefCellMargin( int nMargin)  { m_cellDefault.SetMargin(nMargin); 
                                               m_cellFixedRowDef.SetMargin(nMargin); 
                                               m_cellFixedColDef.SetMargin(nMargin); 
                                               m_cellFixedRowColDef.SetMargin(nMargin); }
    int      GetDefCellMargin() const        { return m_cellDefault.GetMargin();     }

    int      GetDefCellHeight() const        { return m_cellDefault.GetHeight();     }
    void     SetDefCellHeight(int nHeight)   { m_cellDefault.SetHeight(nHeight); 
                                               m_cellFixedRowDef.SetHeight(nHeight); 
                                               m_cellFixedColDef.SetHeight(nHeight); 
                                               m_cellFixedRowColDef.SetHeight(nHeight); }
    int      GetDefCellWidth() const         { return m_cellDefault.GetWidth();     }
    void     SetDefCellWidth(int nWidth)     { m_cellDefault.SetWidth(nWidth); 
                                               m_cellFixedRowDef.SetWidth(nWidth); 
                                               m_cellFixedColDef.SetWidth(nWidth); 
                                               m_cellFixedRowColDef.SetWidth(nWidth); }

    // ***************************************************************************** //

    //获取固定选择行
    int GetSelectedCount() const                  { return (int)m_SelectedCellMap.GetCount(); }

    //设置焦点 主要是重绘操作
    CCellID SetFocusCell(CCellID cell);
    CCellID SetFocusCell(int nRow, int nCol);
    CCellID GetFocusCell() const                  { return m_idCurrentCell;           }

   //设置为打印模式
    void SetVirtualMode(BOOL bVirtual);
    BOOL GetVirtualMode() const                   { return m_bVirtualMode;            }

    //回调函数
    void SetCallbackFunc(GRIDCALLBACK pCallback,
                         LPARAM lParam)           { m_pfnCallback = pCallback; m_lParam = lParam; }
    GRIDCALLBACK GetCallbackFunc()                { return m_pfnCallback;             }

    //网格设置图片
    void SetImageList(CImageList* pList)          { m_pImageList = pList;             }
    CImageList* GetImageList() const              { return m_pImageList;              }

    //设置网格线类型
    void SetGridLines(int nWhichLines = GVL_BOTH);
    int  GetGridLines() const                     { return m_nGridLines;              }

    //设置可编辑
    void SetEditable(BOOL bEditable = TRUE)       { m_bEditable = bEditable;          }
    BOOL IsEditable() const                       { return m_bEditable;               }   
    void SetListMode(BOOL bEnableListMode = TRUE);
    BOOL GetListMode() const                      { return m_bListMode;               }

    //单行、单列选择
    void SetSingleRowSelection(BOOL bSing = TRUE) { m_bSingleRowSelection = bSing;    }
    BOOL GetSingleRowSelection()                  { return m_bSingleRowSelection & m_bListMode; }
    void SetSingleColSelection(BOOL bSing = TRUE) { m_bSingleColSelection = bSing;    }
    BOOL GetSingleColSelection()                  { return m_bSingleColSelection;     }

    //设置可选择
    void EnableSelection(BOOL bEnable = TRUE)     { ResetSelectedRange(); m_bEnableSelection = bEnable; ResetSelectedRange(); }
    BOOL IsSelectable() const                     { return m_bEnableSelection;        }

    //设置表头行列选择
    void SetFixedColumnSelection(BOOL bSelect)    { m_bFixedColumnSelection = bSelect;}
    BOOL GetFixedColumnSelection()                { return m_bFixedColumnSelection;   }
    void SetFixedRowSelection(BOOL bSelect)       { m_bFixedRowSelection = bSelect;   }
    BOOL GetFixedRowSelection()                   { return m_bFixedRowSelection;      }

    //允许拖拽
    void EnableDragAndDrop(BOOL bAllow = TRUE)    { m_bAllowDragAndDrop = bAllow;     }
    BOOL GetDragAndDrop() const                   { return m_bAllowDragAndDrop;       }

    //允许调整行列大小
    void SetRowResize(BOOL bResize = TRUE)        { m_bAllowRowResize = bResize;      }
    BOOL GetRowResize() const                     { return m_bAllowRowResize;         }
    void SetColumnResize(BOOL bResize = TRUE)     { m_bAllowColumnResize = bResize;   }
    BOOL GetColumnResize() const                  { return m_bAllowColumnResize;      }

    //设置表头按照鼠标点击排序
    void SetHeaderSort(BOOL bSortOnClick = TRUE)  { m_bSortOnClick = bSortOnClick;    }
    BOOL GetHeaderSort() const                    { return m_bSortOnClick;            }

    //设置Tab键处理 按Tab键后选择表格往后移动一格
    void SetHandleTabKey(BOOL bHandleTab = TRUE)  { m_bHandleTabKey = bHandleTab;     }
    BOOL GetHandleTabKey() const                  { return m_bHandleTabKey;           }

    //设置双缓冲绘制方法
    void SetDoubleBuffering(BOOL bBuffer = TRUE)  { m_bDoubleBuffer = bBuffer;        }
    BOOL GetDoubleBuffering() const               { return m_bDoubleBuffer;           }

    //允许提示
    void EnableTitleTips(BOOL bEnable = TRUE)     { m_bTitleTips = bEnable;           }
    BOOL GetTitleTips()                           { return m_bTitleTips;              }

    //设置列排序
    void SetSortColumn(int nCol);
    int  GetSortColumn() const                    { return m_nSortColumn;             }

    //设置降序升序
    void SetSortAscending(BOOL bAscending)        { m_bAscending = bAscending;        }
    BOOL GetSortAscending() const                 { return m_bAscending;              }

    //设置跟踪??? 没用到
    void SetTrackFocusCell(BOOL bTrack)           { m_bTrackFocusCell = bTrack;       }
    BOOL GetTrackFocusCell()                      { return m_bTrackFocusCell;         }

    //设置框架焦点 没用到
    void SetFrameFocusCell(BOOL bFrame)           { m_bFrameFocus = bFrame;           }
    BOOL GetFrameFocusCell()                      { return m_bFrameFocus;             }

    //设置自动调整大小 没用到
    void SetAutoSizeStyle(int nStyle = GVS_BOTH)  { m_nAutoSizeColumnStyle = nStyle;  }
    int  GetAutoSizeStyle()                       { return m_nAutoSizeColumnStyle; }

    //设置忽略隐藏列 忽略隐藏的列并向左看第一个可见列
    void EnableHiddenColUnhide(BOOL bEnable = TRUE){ m_bHiddenColUnhide = bEnable;    }
    BOOL GetHiddenColUnhide()                     { return m_bHiddenColUnhide;        }
    void EnableHiddenRowUnhide(BOOL bEnable = TRUE){ m_bHiddenRowUnhide = bEnable;    }
    BOOL GetHiddenRowUnhide()                     { return m_bHiddenRowUnhide;        }

    //允许行列隐藏 如当鼠标移动列，如果允许隐藏列，则列宽为0，如果不允许则列宽是最小是1
    void EnableColumnHide(BOOL bEnable = TRUE)    { m_bAllowColHide = bEnable;        }
    BOOL GetColumnHide()                          { return m_bAllowColHide;           }
    void EnableRowHide(BOOL bEnable = TRUE)       { m_bAllowRowHide = bEnable;        }
    BOOL GetRowHide()                             { return m_bAllowRowHide;           }

///////////////////////////////////////////////////////////////////////////////////
// default Grid cells. Use these for setting default values such as colors and fonts
///////////////////////////////////////////////////////////////////////////////////
public:
    CGridCellBase* GetDefaultCell(BOOL bFixedRow, BOOL bFixedCol) const;

///////////////////////////////////////////////////////////////////////////////////
// Grid cell Attributes
///////////////////////////////////////////////////////////////////////////////////
public:
    CGridCellBase* GetCell(int nRow, int nCol) const;   // Get the actual cell!

    //设置修改标志
    void SetModified(BOOL bModified = TRUE, int nRow = -1, int nCol = -1);
    BOOL GetModified(int nRow = -1, int nCol = -1);

    //设置行列是否是固定行列
    BOOL IsCellFixed(int nRow, int nCol);

    //通过行列结构方式设置行列信息
    BOOL   SetItem(const GV_ITEM* pItem);
    BOOL   GetItem(GV_ITEM* pItem);
    BOOL   SetItemText(int nRow, int nCol, LPCTSTR str);
    // The following was virtual. If you want to override, use 
    //  CGridCellBase-derived class's GetText() to accomplish same thing
    CString GetItemText(int nRow, int nCol) const;

    // EFW - 06/13/99 - Added to support printf-style formatting codes.
    // Also supports use with a string resource ID
#if !defined(_WIN32_WCE) || (_WIN32_WCE >= 210) //win ce环境
    //格式化相关字符串 类似printf,使用setItemTextFmt(0,0,"%s,%d","abc",1);
    BOOL   SetItemTextFmt(int nRow, int nCol, LPCTSTR szFmt, ...);
    BOOL   SetItemTextFmtID(int nRow, int nCol, UINT nID, ...);
#endif

    //设置行列Data
    BOOL   SetItemData(int nRow, int nCol, LPARAM lParam);
    LPARAM GetItemData(int nRow, int nCol) const;

    //设置行列图片
    BOOL   SetItemImage(int nRow, int nCol, int iImage);
    int    GetItemImage(int nRow, int nCol) const;

    //设置行列状态
    BOOL   SetItemState(int nRow, int nCol, UINT state);
    UINT   GetItemState(int nRow, int nCol) const;

    //设置格式，居中，对齐等
    BOOL   SetItemFormat(int nRow, int nCol, UINT nFormat);
    UINT   GetItemFormat(int nRow, int nCol) const;

    //设置背景颜色
    BOOL   SetItemBkColour(int nRow, int nCol, COLORREF cr = CLR_DEFAULT);
    COLORREF GetItemBkColour(int nRow, int nCol) const;

    //设置前景颜色 就是文字颜色
    BOOL   SetItemFgColour(int nRow, int nCol, COLORREF cr = CLR_DEFAULT);
    COLORREF GetItemFgColour(int nRow, int nCol) const;

    //设置字体
    BOOL SetItemFont(int nRow, int nCol, const LOGFONT* lf);
    const LOGFONT* GetItemFont(int nRow, int nCol);

    //是否编辑
    BOOL IsItemEditing(int nRow, int nCol);

    //单元格显示不同类型控件，如果checkbox,button等
    /*
     * 比如创建button,首先button必须是HGridCellBase子类，如HGridCellButton
     * 然后setCellType(0,0,RUNTIME_CLASS(HGridCellButton));然后创建了一个button
    */
    BOOL SetCellType(int nRow, int nCol, CRuntimeClass* pRuntimeClass);
    BOOL SetDefaultCellType( CRuntimeClass* pRuntimeClass);

///////////////////////////////////////////////////////////////////////////////////
// Operations
///////////////////////////////////////////////////////////////////////////////////
public:

    //插入行列，在原有的基础上再插入
    int  InsertColumn(LPCTSTR strHeading, UINT nFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE,int nColumn = -1);
    int  InsertRow(LPCTSTR strHeading, int nRow = -1);

    //删除行列
    BOOL DeleteColumn(int nColumn);
    BOOL DeleteRow(int nRow);
    BOOL DeleteNonFixedRows();
    BOOL DeleteAllItems();

    //清除表格范围
	void ClearCells(CCellRange Selection);

    //自动调整行列，扩展行列到合适位置
    BOOL AutoSizeRow(int nRow, BOOL bResetScroll = TRUE);
    BOOL AutoSizeColumn(int nCol, UINT nAutoSizeStyle = GVS_DEFAULT, BOOL bResetScroll = TRUE);
    void AutoSizeRows();
    void AutoSizeColumns(UINT nAutoSizeStyle = GVS_DEFAULT);
    void AutoSize(UINT nAutoSizeStyle = GVS_DEFAULT);
    void ExpandColumnsToFit(BOOL bExpandFixed = TRUE);
    void ExpandLastColumn();
    void ExpandRowsToFit(BOOL bExpandFixed = TRUE);
    void ExpandToFit(BOOL bExpandFixed = TRUE);

    //自动填充
    void Refresh();
    void AutoFill();   // Fill grid with blank cells

    //可视化 不知道干啥的
    void EnsureVisible(CCellID &cell)       { EnsureVisible(cell.row, cell.col); }
    void EnsureVisible(int nRow, int nCol);
    BOOL IsCellVisible(int nRow, int nCol);
    BOOL IsCellVisible(CCellID cell);

    //可编辑和可选择
    BOOL IsCellEditable(int nRow, int nCol) const;
    BOOL IsCellEditable(CCellID &cell) const;
    BOOL IsCellSelected(int nRow, int nCol) const;
    BOOL IsCellSelected(CCellID &cell) const;

    // SetRedraw stops/starts redraws on things like changing the # rows/columns
    // and autosizing, but not for user-intervention such as resizes
    //重绘操作
    void SetRedraw(BOOL bAllowDraw, BOOL bResetScrollBars = FALSE);
    BOOL RedrawCell(int nRow, int nCol, CDC* pDC = NULL);
    BOOL RedrawCell(const CCellID& cell, CDC* pDC = NULL);
    BOOL RedrawRow(int row);
    BOOL RedrawColumn(int col);

#ifndef _WIN32_WCE
    //表格存储 ???
    BOOL Save(LPCTSTR filename, TCHAR chSeparator = _T(','));
    BOOL Load(LPCTSTR filename, TCHAR chSeparator = _T(','));
#endif

///////////////////////////////////////////////////////////////////////////////////
// Cell Ranges
///////////////////////////////////////////////////////////////////////////////////
 public:

    //多表格(区域表格)操作
    CCellRange GetCellRange() const;
    CCellRange GetSelectedCellRange() const;
    void SetSelectedRange(const CCellRange& Range, BOOL bForceRepaint = FALSE, BOOL bSelectCells = TRUE);
    void SetSelectedRange(int nMinRow, int nMinCol, int nMaxRow, int nMaxCol,
                          BOOL bForceRepaint = FALSE, BOOL bSelectCells = TRUE);
    BOOL IsValid(int nRow, int nCol) const;
    BOOL IsValid(const CCellID& cell) const;
    BOOL IsValid(const CCellRange& range) const;

///////////////////////////////////////////////////////////////////////////////////
// Clipboard, drag and drop, and cut n' paste operations
// 剪切 拖拽等操作
///////////////////////////////////////////////////////////////////////////////////
#ifndef GRIDCONTROL_NO_CLIPBOARD
    virtual void CutSelectedText();
    virtual COleDataSource* CopyTextFromGrid();
    virtual BOOL PasteTextToGrid(CCellID cell, COleDataObject* pDataObject, BOOL bSelectPastedCells=TRUE);
#endif

#ifndef GRIDCONTROL_NO_DRAGDROP
 public:
    virtual void OnBeginDrag();
    virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
    virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
    virtual void OnDragLeave();
    virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
#endif

#ifndef GRIDCONTROL_NO_CLIPBOARD
    virtual void OnEditCut();
    virtual void OnEditCopy();
    virtual void OnEditPaste();
#endif
    virtual void OnEditSelectAll();

///////////////////////////////////////////////////////////////////////////////////
// Misc.排序 比较等
///////////////////////////////////////////////////////////////////////////////////
public:
    CCellID GetNextItem(CCellID& cell, int nFlags) const;

	BOOL SortItems(int nCol, BOOL bAscending, LPARAM data = 0);
    BOOL SortTextItems(int nCol, BOOL bAscending, LPARAM data = 0);
    BOOL SortItems(PFNLVCOMPARE pfnCompare, int nCol, BOOL bAscending, LPARAM data = 0);

	void SetCompareFunction(PFNLVCOMPARE pfnCompare);

	// in-built sort functions
	static int CALLBACK pfnCellTextCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static int CALLBACK pfnCellNumericCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

///////////////////////////////////////////////////////////////////////////////////
// Printing
///////////////////////////////////////////////////////////////////////////////////
#if !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)
public:
    void Print(CPrintDialog* pPrntDialog = NULL);

    // EFW - New printing support functions
    void EnableWysiwygPrinting(BOOL bEnable = TRUE) { m_bWysiwygPrinting = bEnable;     }
    BOOL GetWysiwygPrinting()                       { return m_bWysiwygPrinting;        }

    void SetShadedPrintOut(BOOL bEnable = TRUE)     {   m_bShadedPrintOut = bEnable;    }
    BOOL GetShadedPrintOut(void)                    {   return m_bShadedPrintOut;       }

    // Use -1 to have it keep the existing value
    void SetPrintMarginInfo(int nHeaderHeight, int nFooterHeight,
        int nLeftMargin, int nRightMargin, int nTopMargin,
        int nBottomMargin, int nGap);

    void GetPrintMarginInfo(int &nHeaderHeight, int &nFooterHeight,
        int &nLeftMargin, int &nRightMargin, int &nTopMargin,
        int &nBottomMargin, int &nGap);

///////////////////////////////////////////////////////////////////////////////////
// Printing overrides for derived classes
///////////////////////////////////////////////////////////////////////////////////
public:
    virtual void OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo);
    virtual void OnPrint(CDC *pDC, CPrintInfo *pInfo);
    virtual void OnEndPrinting(CDC *pDC, CPrintInfo *pInfo);

#endif // #if !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)

// Implementation
public:
    //Merge the selected cells    合并单元格
	//by Huang Wei
	CGridCellBase* GetCell(CCellID cell);
	CCellID GetMergeCellID(CCellID cell);
	void UnMergeSelectedCells();
	void MergeSelectedCells();
	void UnMergeCells(int nStartRow, int nStartCol, int nEndRow, int nEndCol);
	void MergeCells(int nStartRow, int nStartCol, int nEndRow, int nEndCol);
	int  GetMergeCellWidth(CCellID cell);
	int  GetMergeCellHeight(CCellID cell);
	BOOL GetCellOriginNoMerge(int nRow, int nCol, LPPOINT p);
	BOOL GetCellOriginNoMerge(const CCellID& cell, LPPOINT p);



    virtual ~CGridCtrl();

protected:
    BOOL RegisterWindowClass();
    BOOL Initialise();
    void SetupDefaultCells();

    LRESULT SendMessageToParent(int nRow, int nCol, int nMessage) const;
    LRESULT SendDisplayRequestToParent(GV_DISPINFO* pDisplayInfo) const;
    LRESULT SendCacheHintToParent(const CCellRange& range) const;

    BOOL InvalidateCellRect(const int row, const int col);
    BOOL InvalidateCellRect(const CCellID& cell);
    BOOL InvalidateCellRect(const CCellRange& cellRange);
    void EraseBkgnd(CDC* pDC);

    BOOL GetCellRangeRect(const CCellRange& cellRange, LPRECT lpRect);

    BOOL SetCell(int nRow, int nCol, CGridCellBase* pCell);

    int  SetMouseMode(int nMode) { int nOldMode = m_MouseMode; m_MouseMode = nMode; return nOldMode; }
    int  GetMouseMode() const    { return m_MouseMode; }

    BOOL MouseOverRowResizeArea(CPoint& point);
    BOOL MouseOverColumnResizeArea(CPoint& point);

    CCellID GetTopleftNonFixedCell(BOOL bForceRecalculation = FALSE);
    CCellRange GetUnobstructedNonFixedCellRange(BOOL bForceRecalculation = FALSE);
    CCellRange GetVisibleNonFixedCellRange(LPRECT pRect = NULL, BOOL bForceRecalculation = FALSE);

    BOOL IsVisibleVScroll() { return ( (m_nBarState & GVL_VERT) > 0); } 
    BOOL IsVisibleHScroll() { return ( (m_nBarState & GVL_HORZ) > 0); }
    void ResetSelectedRange();
    void ResetScrollBars();
    void EnableScrollBars(int nBar, BOOL bEnable = TRUE);
    int  GetScrollPos32(int nBar, BOOL bGetTrackPos = FALSE);
    BOOL SetScrollPos32(int nBar, int nPos, BOOL bRedraw = TRUE);

    BOOL SortTextItems(int nCol, BOOL bAscending, int low, int high);
    BOOL SortItems(PFNLVCOMPARE pfnCompare, int nCol, BOOL bAscending, LPARAM data,
                   int low, int high);

    CPoint GetPointClicked(int nRow, int nCol, const CPoint& point);

	void ValidateAndModifyCellContents(int nRow, int nCol, LPCTSTR strText);

// Overrrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CGridCtrl)
    protected:
    virtual void PreSubclassWindow();
    //}}AFX_VIRTUAL

protected:
#if !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)
    // Printing
	virtual void PrintFixedRowCells(int nStartColumn, int nStopColumn, int& row, CRect& rect,
                                    CDC *pDC, BOOL& bFirst);
    virtual void PrintColumnHeadings(CDC *pDC, CPrintInfo *pInfo);
    virtual void PrintHeader(CDC *pDC, CPrintInfo *pInfo);
    virtual void PrintFooter(CDC *pDC, CPrintInfo *pInfo);
    virtual void PrintRowButtons(CDC *pDC, CPrintInfo* /*pInfo*/);
#endif

#ifndef GRIDCONTROL_NO_DRAGDROP
    // Drag n' drop
    virtual CImageList* CreateDragImage(CPoint *pHotSpot);    // no longer necessary
#endif

    // Mouse Clicks
    virtual void  OnFixedColumnClick(CCellID& cell);
    virtual void  OnFixedRowClick(CCellID& cell);

    // Editing
    virtual void  OnEditCell(int nRow, int nCol, CPoint point, UINT nChar);
    virtual void  OnEndEditCell(int nRow, int nCol, CString str);
	virtual BOOL  ValidateEdit(int nRow, int nCol, LPCTSTR str);
    virtual void  EndEditing();

    // Drawing
    virtual void  OnDraw(CDC* pDC);

    // CGridCellBase Creation and Cleanup
    virtual CGridCellBase* CreateCell(int nRow, int nCol);
    virtual void DestroyCell(int nRow, int nCol);

// Attributes
protected:
    // General attributes
    COLORREF    m_crFixedTextColour, m_crFixedBkColour;
    COLORREF    m_crGridBkColour, m_crGridLineColour;
    COLORREF    m_crWindowText, m_crWindowColour, m_cr3DFace,     // System colours
                m_crShadow;
    COLORREF    m_crTTipBackClr, m_crTTipTextClr;                 // Titletip colours - FNA
    
    BOOL        m_bVirtualMode;
    LPARAM      m_lParam;                                           // lParam for callback
    GRIDCALLBACK m_pfnCallback;                                     // The callback function

    int         m_nGridLines;
    BOOL        m_bEditable;
    BOOL        m_bModified;
    BOOL        m_bAllowDragAndDrop;
    BOOL        m_bListMode;
    BOOL        m_bSingleRowSelection;
    BOOL        m_bSingleColSelection;
    BOOL        m_bAllowDraw;
    BOOL        m_bEnableSelection;
    BOOL        m_bFixedRowSelection, m_bFixedColumnSelection;
    BOOL        m_bSortOnClick;
    BOOL        m_bHandleTabKey;
    BOOL        m_bDoubleBuffer;
    BOOL        m_bTitleTips;
    int         m_nBarState;
    BOOL        m_bWysiwygPrinting;
    BOOL        m_bHiddenColUnhide, m_bHiddenRowUnhide;
    BOOL        m_bAllowColHide, m_bAllowRowHide;
    BOOL        m_bAutoSizeSkipColHdr;
    BOOL        m_bTrackFocusCell;
    BOOL        m_bFrameFocus;
    UINT        m_nAutoSizeColumnStyle;

    // Cell size details
    int         m_nRows, m_nFixedRows, m_nCols, m_nFixedCols;
    CUIntArray  m_arRowHeights, m_arColWidths;
    int         m_nVScrollMax, m_nHScrollMax;

    // Fonts and images
    CRuntimeClass*   m_pRtcDefault; // determines kind of Grid Cell created by default
    CGridDefaultCell m_cellDefault;  // "default" cell. Contains default colours, font etc.
    CGridDefaultCell m_cellFixedColDef, m_cellFixedRowDef, m_cellFixedRowColDef;
    CFont       m_PrinterFont;  // for the printer
    CImageList* m_pImageList;

    // Cell data
    CTypedPtrArray<CObArray, GRID_ROW*> m_RowData;

    // Mouse operations such as cell selection
    int         m_MouseMode;
    BOOL        m_bLMouseButtonDown, m_bRMouseButtonDown;
    CPoint      m_LeftClickDownPoint, m_LastMousePoint;
    CCellID     m_LeftClickDownCell, m_SelectionStartCell;
    CCellID     m_idCurrentCell, m_idTopLeftCell;
    int         m_nTimerID;
    int         m_nTimerInterval;
    int         m_nResizeCaptureRange;
    BOOL        m_bAllowRowResize, m_bAllowColumnResize;
    int         m_nRowsPerWheelNotch;
    CMap<DWORD,DWORD, CCellID, CCellID&> m_SelectedCellMap, m_PrevSelectedCellMap;

#ifndef GRIDCONTROL_NO_TITLETIPS
    CTitleTip   m_TitleTip;             // Title tips for cells
#endif

    // Drag and drop
    CCellID     m_LastDragOverCell;
#ifndef GRIDCONTROL_NO_DRAGDROP
    CGridDropTarget m_DropTarget;       // OLE Drop target for the grid
#endif

    // Printing information
    CSize       m_CharSize;
    int         m_nPageHeight;
    CSize       m_LogicalPageSize,      // Page size in gridctrl units.
                m_PaperSize;            // Page size in device units.
    // additional properties to support Wysiwyg printing
    int         m_nPageWidth;
    int         m_nPrintColumn;
    int         m_nCurrPrintRow;
    int         m_nNumPages;
    int         m_nPageMultiplier;

    // sorting
    int          m_bAscending;
    int          m_nSortColumn;
	PFNLVCOMPARE m_pfnCompare;

    // EFW - Added to support shaded/unshaded printout.  If true, colored
    // cells will print as-is.  If false, all text prints as black on white.
    BOOL        m_bShadedPrintOut;

    // EFW - Added support for user-definable margins.  Top and bottom are in 
    // lines.  Left, right, and gap are in characters (avg width is used).
    int         m_nHeaderHeight, m_nFooterHeight, m_nLeftMargin,
                m_nRightMargin, m_nTopMargin, m_nBottomMargin, m_nGap;

protected:
    void SelectAllCells();
    void SelectColumns(CCellID currentCell, BOOL bForceRedraw=FALSE, BOOL bSelectCells=TRUE);
    void SelectRows(CCellID currentCell, BOOL bForceRedraw=FALSE, BOOL bSelectCells=TRUE);
    void SelectCells(CCellID currentCell, BOOL bForceRedraw=FALSE, BOOL bSelectCells=TRUE);
    void OnSelecting(const CCellID& currentCell);

    // Generated message map functions
    //{{AFX_MSG(CGridCtrl)
    afx_msg void OnPaint();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg UINT OnGetDlgCode();
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
    //}}AFX_MSG
#ifndef _WIN32_WCE_NO_CURSOR
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
#endif
#ifndef _WIN32_WCE
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);    // EFW - Added
    afx_msg void OnSysColorChange();
#endif
#ifndef _WIN32_WCE_NO_CURSOR
    afx_msg void OnCaptureChanged(CWnd *pWnd);
#endif
#ifndef GRIDCONTROL_NO_CLIPBOARD
    afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
#endif
#if (_MFC_VER >= 0x0421) || (_WIN32_WCE >= 210)
    afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
#endif
#if !defined(_WIN32_WCE) && (_MFC_VER >= 0x0421)
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
#endif
    afx_msg LRESULT OnSetFont(WPARAM hFont, LPARAM lParam);
    afx_msg LRESULT OnGetFont(WPARAM hFont, LPARAM lParam);
    afx_msg LRESULT OnImeChar(WPARAM wCharCode, LPARAM lParam);
    afx_msg void OnEndInPlaceEdit(NMHDR* pNMHDR, LRESULT* pResult);
    DECLARE_MESSAGE_MAP()

    enum eMouseModes { MOUSE_NOTHING, MOUSE_SELECT_ALL, MOUSE_SELECT_COL, MOUSE_SELECT_ROW,
                       MOUSE_SELECT_CELLS, MOUSE_SCROLLING_CELLS,
                       MOUSE_OVER_ROW_DIVIDE, MOUSE_SIZING_ROW,
                       MOUSE_OVER_COL_DIVIDE, MOUSE_SIZING_COL,
                       MOUSE_PREPARE_EDIT,
#ifndef GRIDCONTROL_NO_DRAGDROP
                       MOUSE_PREPARE_DRAG, MOUSE_DRAGGING
#endif
    };
};

// Returns the default cell implementation for the given grid region
inline CGridCellBase* CGridCtrl::GetDefaultCell(BOOL bFixedRow, BOOL bFixedCol) const
{ 
    if (bFixedRow && bFixedCol) return (CGridCellBase*) &m_cellFixedRowColDef;
    if (bFixedRow)              return (CGridCellBase*) &m_cellFixedRowDef;
    if (bFixedCol)              return (CGridCellBase*) &m_cellFixedColDef;
    return (CGridCellBase*) &m_cellDefault;
}

inline CGridCellBase* CGridCtrl::GetCell(int nRow, int nCol) const
{
    if (nRow < 0 || nRow >= m_nRows || nCol < 0 || nCol >= m_nCols) 
        return NULL;

    if (GetVirtualMode())
    {
        CGridCellBase* pCell = GetDefaultCell(nRow < m_nFixedRows, nCol < m_nFixedCols);
        static GV_DISPINFO gvdi;
        gvdi.item.row     = nRow;
        gvdi.item.col     = nCol;
        gvdi.item.mask    = 0xFFFFFFFF;
        gvdi.item.nState  = 0;
        gvdi.item.nFormat = pCell->GetFormat();
        gvdi.item.iImage  = pCell->GetImage();
        gvdi.item.crBkClr = pCell->GetBackClr();
        gvdi.item.crFgClr = pCell->GetTextClr();
        gvdi.item.lParam  = pCell->GetData();
        memcpy(&gvdi.item.lfFont, pCell->GetFont(), sizeof(LOGFONT));
        gvdi.item.nMargin = pCell->GetMargin();
        gvdi.item.strText.Empty();

        // Fix the state bits
        if (IsCellSelected(nRow, nCol))   gvdi.item.nState |= GVIS_SELECTED;
        if (nRow < GetFixedRowCount())    gvdi.item.nState |= (GVIS_FIXED | GVIS_FIXEDROW);
        if (nCol < GetFixedColumnCount()) gvdi.item.nState |= (GVIS_FIXED | GVIS_FIXEDCOL);
        if (GetFocusCell() == CCellID(nRow, nCol)) gvdi.item.nState |= GVIS_FOCUSED;
        
        if (m_pfnCallback)
            m_pfnCallback(&gvdi, m_lParam);
        else
            SendDisplayRequestToParent(&gvdi);

        static CGridCell cell;
        cell.SetState(gvdi.item.nState);
        cell.SetFormat(gvdi.item.nFormat);
        cell.SetImage(gvdi.item.iImage);
        cell.SetBackClr(gvdi.item.crBkClr);
        cell.SetTextClr(gvdi.item.crFgClr);
        cell.SetData(gvdi.item.lParam);
        cell.SetFont(&(gvdi.item.lfFont));
        cell.SetMargin(gvdi.item.nMargin);
        cell.SetText(gvdi.item.strText);
        cell.SetGrid((CGridCtrl*)this);

        return (CGridCellBase*) &cell;
    }

    GRID_ROW* pRow = m_RowData[nRow];
    if (!pRow) return NULL;
    return pRow->GetAt(nCol);
}

inline BOOL CGridCtrl::SetCell(int nRow, int nCol, CGridCellBase* pCell)
{
    if (GetVirtualMode())
        return FALSE;

    if (nRow < 0 || nRow >= m_nRows || nCol < 0 || nCol >= m_nCols) 
        return FALSE;

    GRID_ROW* pRow = m_RowData[nRow];
    if (!pRow) return FALSE;

    pCell->SetCoords( nRow, nCol); 
    pRow->SetAt(nCol, pCell);

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRIDCTRL_H__519FA702_722C_11D1_ABBA_00A0243D1382__INCLUDED_)
