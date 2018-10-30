#include "hgridcell.h"

HGridCell::HGridCell(HGridCellBase *parent) : HGridCellBase(parent)
{

}

/*
HGridCell::HGridCell()
{
    m_plfFont = NULL;
    CGridCell::Reset();
}*/

HGridCell::~HGridCell()
{
    //delete m_plfFont;
}

/////////////////////////////////////////////////////////////////////////////
void HGridCell::operator=( HGridCell& cell)
{
    if (this != &cell) HGridCellBase::operator=(cell);
}

void HGridCell::reset()
{
    HGridCellBase::reset();

    m_strText  = "";
    m_nImage   = -1;
    m_pGrid    = NULL;
    m_bEditing = false;
    //m_pEditWnd = NULL;

    m_nFormat = Qt::AlignLeft;           // Use default from CGridDefaultCell
    m_crBkClr = QColor(Qt::white);     // Background colour (or CLR_DEFAULT)
    m_crFgClr = QColor(Qt::black);     // Forground colour (or CLR_DEFAULT)
    m_nMargin = (uint)-1;              // Use default from CGridDefaultCell
    m_plfFont = QFont(QStringLiteral("宋体"),10, QFont::Normal);
    m_lParam  = QVariant();
    m_Size    = QSize(40,25);            // Default Size
    m_bBorder       = true;
    m_bLeftBorder   = false;
    m_bTopBorder    = false;
    m_bRightBorder  = false;
    m_bBottomBorder = false;

    m_nBorderStyle       = Qt::NoPen;
    m_nTopBorderStyle    = Qt::NoPen;
    m_nRightBorderStyle  = Qt::NoPen;
    m_nBottomBorderStyle = Qt::NoPen;

    m_crBoderClr       = QColor(Qt::black);
    m_crLeftBoderClr   = QColor(Qt::black);
    m_crTopBoderClr    = QColor(Qt::black);
    m_crRightBoderClr  = QColor(Qt::black);
    m_crBottomBoderClr = QColor(Qt::black);
}

/////////////////////////////////////////////////////////////////////////////
// GridCell Operations

/*
 * 编辑的时候是生成一个QLineEdit 移动到对应的rect里面
*/
bool HGridCell::edit(int nRow, int nCol, const QRect& rect, const QPoint& point, uint nID, uint nChar)
{
    if ( m_bEditing )
    {
        //if (m_pEditWnd)
        //    m_pEditWnd->SendMessage ( WM_CHAR, nChar );
    }
    else
    {
        quint32 dwStyle = format();
        m_bEditing = true;

        // InPlaceEdit auto-deletes itself
        //HGridCtrl* pGrid = grid();
        //m_pEditWnd = new CInPlaceEdit(pGrid, rect, dwStyle, nID, nRow, nCol, GetText(), nChar);
    }
    return true;
}

void HGridCell::endEdit()
{
    //if (m_pEditWnd)
     //   ((CInPlaceEdit*)m_pEditWnd)->EndEdit();
}

void HGridCell::onEndEdit()
{
    m_bEditing = false;
    //m_pEditWnd = NULL;
}
