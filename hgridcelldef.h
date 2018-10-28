#ifndef HGRIDCTRLDEF_H_
#define HGRIDCTRLDEF_H_
//c++
#include <exception>
using namespace std;
//qt define
#include <QApplication>
#include <QtGlobal>
#include <QObject>
#include <QWidget>
#include <QColor>
#include <QRgb>
#include <QFont>
#include <QSize>
#include <QMap>
#include <QPainter>
#include <QKeyEvent>
#include <QClipboard>
typedef unsigned int HWPARAM;
typedef qlonglong HLPARAM; //统一64位平台

//颜色定义 参考windos系统
/*
 * 基础颜色参考windows系统，防止与windows已有定义冲突，所有在原定义前面加入Q
*/
const Q_DECL_UNUSED QRgb  QCLR_NONE                         = 0xFFFFFFFFL; //no
const Q_DECL_UNUSED QRgb  QCLR_DEFAULT                      = 0xFF000000L; //no
const Q_DECL_UNUSED QRgb  QCOLOR_SCROLLBAR                  = 0xC0C0C0;    // Scrollbar
const Q_DECL_UNUSED QRgb  QCOLOR_BACKGROUND                 = 0x008080;    //Background
const Q_DECL_UNUSED QRgb  QCOLOR_ACTIVECAPTION              = 0x000080;    //ActiveTitle
const Q_DECL_UNUSED QRgb  QCOLOR_INACTIVECAPTION            = 0x808080;    //InactiveTitle
const Q_DECL_UNUSED QRgb  QCOLOR_MENU                       = 0xC0C0C0;    //Menu
const Q_DECL_UNUSED QRgb  QCOLOR_WINDOW                     = 0xFFFFFF;    //Window
const Q_DECL_UNUSED QRgb  QCOLOR_WINDOWFRAME                = 0x000000;    //WindowFrame
const Q_DECL_UNUSED QRgb  QCOLOR_MENUTEXT                   = 0xC0C0C0;    //MenuText
const Q_DECL_UNUSED QRgb  QCOLOR_WINDOWTEXT                 = 0x000000;    //WindowText
const Q_DECL_UNUSED QRgb  QCOLOR_CAPTIONTEXT                = 0xFFFFFF;    //TitleText
const Q_DECL_UNUSED QRgb  QCOLOR_ACTIVEBORDER               = 0xC0C0C0;    //ActiveBorder
const Q_DECL_UNUSED QRgb  QCOLOR_INACTIVEBORDER             = 0xC0C0C0;    //InactiveBorder
const Q_DECL_UNUSED QRgb  QCOLOR_APPWORKSPACE               = 0x808080;    //AppWorkspace
const Q_DECL_UNUSED QRgb  QCOLOR_HIGHLIGHT                  = 0x000080;    //Highlight
const Q_DECL_UNUSED QRgb  QCOLOR_HIGHLIGHTTEXT              = 0xFFFFFF;    //HighlightText
const Q_DECL_UNUSED QRgb  QCOLOR_BTNFACE                    = 0xC0C0C0;    //ButtonFace
const Q_DECL_UNUSED QRgb  QCOLOR_BTNSHADOW                  = 0x808080;    //ButtonShadow
const Q_DECL_UNUSED QRgb  QCOLOR_GRAYTEXT                   = 0x808080;    //GrayText
const Q_DECL_UNUSED QRgb  QCOLOR_BTNTEXT                    = 0x000000;    //ButtonText
const Q_DECL_UNUSED QRgb  QCOLOR_INACTIVECAPTIONTEXT        = 0xC0C0C0;    //InactiveTitleText
const Q_DECL_UNUSED QRgb  QCOLOR_BTNHIGHLIGHT               = 0xFFFFFF;    //ButtonHighlight
const Q_DECL_UNUSED QRgb  QCOLOR_3DDKSHADOW                 = 0x000000;    //ButtonDkShadow
const Q_DECL_UNUSED QRgb  QCOLOR_3DLIGHT                    = 0xC0C0C0;    //ButtonLight
const Q_DECL_UNUSED QRgb  QCOLOR_INFOTEXT                   = 0x000000;    //InfoText
const Q_DECL_UNUSED QRgb  QCOLOR_INFOBK                     = 0xFFFFFF;    //InfoWindow
const Q_DECL_UNUSED QRgb  QCOLOR_HOTLIGHT                   = 0x0000FF;    //HotTrackingColor
const Q_DECL_UNUSED QRgb  QCOLOR_GRADIENTACTIVECAPTION      = 0x000080;    //GradientActiveTitle
const Q_DECL_UNUSED QRgb  QCOLOR_GRADIENTINACTIVECAPTION    = 0x808080;    //GradientInactiveTitle
const Q_DECL_UNUSED QRgb  QCOLOR_MENUHILIGHT                = 0xFF9933;    //MenuHilight
const Q_DECL_UNUSED QRgb  QCOLOR_MENUBAR                    = 0xF0F0F0;    //MenuBar


enum eMouseModes { MOUSE_NOTHING, MOUSE_SELECT_ALL, MOUSE_SELECT_COL, MOUSE_SELECT_ROW,
                   MOUSE_SELECT_CELLS, MOUSE_SCROLLING_CELLS,
                   MOUSE_OVER_ROW_DIVIDE, MOUSE_SIZING_ROW,
                   MOUSE_OVER_COL_DIVIDE, MOUSE_SIZING_COL,
                   MOUSE_PREPARE_EDIT,
#ifndef GRIDCONTROL_NO_DRAGDROP
                   MOUSE_PREPARE_DRAG, MOUSE_DRAGGING
#endif
};

//比较函数
typedef int (CALLBACK *QPFNLVCOMPARE)(HLPARAM, HLPARAM, HLPARAM);
//相关列表定义
typedef  QList<QImage*>  QImageList;




// Cell states
#define GVIS_FOCUSED            0x0001
#define GVIS_SELECTED           0x0002
#define GVIS_DROPHILITED        0x0004
#define GVIS_READONLY           0x0008
#define GVIS_FIXED              0x0010
#define GVIS_FIXEDROW           0x0020
#define GVIS_FIXEDCOL           0x0040
#define GVIS_MODIFIED           0x0080

/*
#define LVIF_TEXT               0x0001
#define LVIF_IMAGE              0x0002
#define LVIF_PARAM              0x0004
#define LVIF_STATE              0x0008
*/
// Cell data mask
#define GVIF_TEXT               0x0001
#define GVIF_IMAGE              0x0002
#define GVIF_PARAM              0x0004
#define GVIF_STATE              0x0008
#define GVIF_BKCLR              (GVIF_STATE<<1)
#define GVIF_FGCLR              (GVIF_STATE<<2)
#define GVIF_FORMAT             (GVIF_STATE<<3)
#define GVIF_FONT               (GVIF_STATE<<4)
#define GVIF_MARGIN             (GVIF_STATE<<5)
#define GVIF_ALL                (GVIF_TEXT|GVIF_IMAGE|GVIF_PARAM|GVIF_STATE|GVIF_BKCLR|GVIF_FGCLR| \
                                 GVIF_FORMAT|GVIF_FONT|GVIF_MARGIN)

// Used for Get/SetItem calls.
typedef struct _GV_ITEM {
    int        row,col;     // Row and Column of item
    uint       mask;        // Mask for use in getting/setting cell data
    uint       nState;      // cell state (focus/hilighted etc)
    quint32    nFormat;     // Format of cell 表示网格格式：边框，单选，Alignment等
    int        iImage;      // index of the list view item icon
    QColor     crBkClr;     // Background colour (or CLR_DEFAULT)
    QColor     crFgClr;     // Forground colour (or CLR_DEFAULT)
    quint32    lParam;      // 32-bit value to associate with item 选择状态
    QFont      lfFont;      // Cell font
    uint       nMargin;     // Internal cell margin
    QString    strText;     // Text in cell
} GV_ITEM;

//防windows定义类似Qt宏
#define QMAKELONG(a, b) ((long)(((ushort)(((ulong)(a)) & 0xffff)) | ((quint32)((ushort)(((quint32)(b)) & 0xffff))) << 16))


///////////////////////////////////////////////////////////////////////////////////
// Conditional includes
///////////////////////////////////////////////////////////////////////////////////

#ifndef GRIDCONTROL_NO_TITLETIPS
#   include "TitleTip.h"
#endif

#ifndef GRIDCONTROL_NO_DRAGDROP
#   include "GridDropTarget.h"
#   undef QT_NO_CLIPBOARD     // Force clipboard functions on
#endif

#ifndef QT_NO_CLIPBOARD
#   include <afxole.h>
#endif


///////////////////////////////////////////////////////////////////////////////////
// Helper functions
///////////////////////////////////////////////////////////////////////////////////

// Handy functions
#define IsSHIFTpressed() ( (GetKeyState(VK_SHIFT) & (1 << (sizeof(SHORT)*8-1))) != 0   )
#define IsCTRLpressed()  ( (GetKeyState(VK_CONTROL) & (1 << (sizeof(SHORT)*8-1))) != 0 )

// Backwards compatibility for pre 2.20 grid versions
#define DDX_GridControl(pDX, nIDC, rControl)  DDX_Control(pDX, nIDC, rControl)


///////////////////////////////////////////////////////////////////////////////////
// Structures
///////////////////////////////////////////////////////////////////////////////////

// This structure sent to Grid's parent in a WM_NOTIFY message
typedef struct tagNM_GRIDVIEW {
    NMHDR hdr;
    int   iRow;
    int   iColumn;
} NM_GRIDVIEW;

// This is sent to the Grid from child in-place edit controls
typedef struct tagGV_DISPINFO {
    NMHDR   hdr;
    GV_ITEM item;
} GV_DISPINFO;

// This is sent to the Grid from child in-place edit controls
typedef struct tagGV_CACHEHINT {
    NMHDR      hdr;
    HCellRange range;
} GV_CACHEHINT;

// storage typedef for each row in the grid
//typedef CTypedPtrArray<CObArray, CGridCellBase*> GRID_ROW;
//typedef QList<HGridCellBase*> GRID_ROW;


// For virtual mode callback
typedef bool (CALLBACK* GRIDCALLBACK)(GV_DISPINFO *, LPARAM);

///////////////////////////////////////////////////////////////////////////////////
// Defines
///////////////////////////////////////////////////////////////////////////////////
//绘制文字 仿windows
#define QDT_TOP                      0x00000000
#define QDT_LEFT                     0x00000000
#define QDT_CENTER                   0x00000001
#define QDT_RIGHT                    0x00000002
#define QDT_VCENTER                  0x00000004
#define QDT_BOTTOM                   0x00000008
#define QDT_WORDBREAK                0x00000010
#define QDT_SINGLELINE               0x00000020
#define QDT_EXPANDTABS               0x00000040
#define QDT_TABSTOP                  0x00000080
#define QDT_NOCLIP                   0x00000100
#define QDT_EXTERNALLEADING          0x00000200
#define QDT_CALCRECT                 0x00000400
#define QDT_NOPREFIX                 0x00000800
#define QDT_INTERNAL                 0x00001000

#if(WINVER >= 0x0400)
#define QDT_EDITCONTROL              0x00002000
#define QDT_PATH_ELLIPSIS            0x00004000
#define QDT_END_ELLIPSIS             0x00008000
#define QDT_MODIFYSTRING             0x00010000
#define QDT_RTLREADING               0x00020000
#define QDT_WORD_ELLIPSIS            0x00040000
#if(WINVER >= 0x0500)
#define QDT_NOFULLWIDTHCHARBREAK     0x00080000
#if(_WIN32_WINNT >= 0x0500)
#define QDT_HIDEPREFIX               0x00100000
#define QDT_PREFIXONLY               0x00200000

// Grid line/scrollbar selection
#define GVL_NONE                0L      // Neither
#define GVL_HORZ                1L      // Horizontal line or scrollbar
#define GVL_VERT                2L      // Vertical line or scrollbar
#define GVL_BOTH                3L      // Both

// Autosizing option
#define GVS_DEFAULT             0
#define GVS_HEADER              1       // Size using column fixed cells data only
#define GVS_DATA                2       // Size using column non-fixed cells data only
#define GVS_BOTH                3       // Size using column fixed and non-fixed

// Cell Searching options
#define GVNI_FOCUSED            0x0001
#define GVNI_SELECTED           0x0002
#define GVNI_DROPHILITED        0x0004
#define GVNI_READONLY           0x0008
#define GVNI_FIXED              0x0010
#define GVNI_MODIFIED           0x0020

#define GVNI_ABOVE              0x0100
#define GVNI_BELOW              0x0200
#define GVNI_TOLEFT             0x0400
#define GVNI_TORIGHT            0x0800
#define GVNI_ALL                (GVNI_BELOW|GVNI_TORIGHT|GVNI_TOLEFT)
#define GVNI_AREA               (GVNI_BELOW|GVNI_TORIGHT)

// Hit test values (not yet implemented)
#define GVHT_DATA               0x0000
#define GVHT_TOPLEFT            0x0001
#define GVHT_COLHDR             0x0002
#define GVHT_ROWHDR             0x0004
#define GVHT_COLSIZER           0x0008
#define GVHT_ROWSIZER           0x0010
#define GVHT_LEFT               0x0020
#define GVHT_RIGHT              0x0040
#define GVHT_ABOVE              0x0080
#define GVHT_BELOW              0x0100

// Messages sent to the grid's parent (More will be added in future)
#define GVN_BEGINDRAG           LVN_BEGINDRAG        // LVN_FIRST-9
#define GVN_BEGINLABELEDIT      LVN_BEGINLABELEDIT   // LVN_FIRST-5
#define GVN_BEGINRDRAG          LVN_BEGINRDRAG
#define GVN_COLUMNCLICK         LVN_COLUMNCLICK
#define GVN_DELETEITEM          LVN_DELETEITEM
#define GVN_ENDLABELEDIT        LVN_ENDLABELEDIT     // LVN_FIRST-6
#define GVN_SELCHANGING         LVN_ITEMCHANGING
#define GVN_SELCHANGED          LVN_ITEMCHANGED
#define GVN_GETDISPINFO         LVN_GETDISPINFO
#define GVN_ODCACHEHINT         LVN_ODCACHEHINT
#endif
