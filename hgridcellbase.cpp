 #include "hgridcellbase.h"
#include "hgridctrl.h"
 HGridCellBase::HGridCellBase(QObject* parent)
 {
     reset();
 }

HGridCellBase::~HGridCellBase()
{

}

void HGridCellBase::operator=( HGridCellBase& cell)
{
    if (this == &cell) return;

    setGrid(cell.grid());    // do first in case of dependencies

    setText(cell.text());
    setImage(cell.image());
    setData(cell.data());
    setState(cell.state());
    setFormat(cell.format());
    setTextClr(cell.textClr());
    setBackClr(cell.backClr());
    setFont(cell.font());
    setMargin(cell.margin());

    /*
    SetMergeCellID(cell.GetMergeCellID());
    SetMergeRange(cell.GetMergeRange());
    Show(cell.IsShow());
    */
}

HGridCellBase* HGridCellBase::defaultCell() const
{
    if (grid())
        return grid()->defaultCell(isFixedRow(), isFixedCol());
    return NULL;
}

void HGridCellBase::reset()
{
    m_nState  = 0;
}

bool HGridCellBase::draw(QPainter* painter, int nRow, int nCol, QRect rect, bool bEraseBkgnd)
{
    return true;
}

//对rect进行判断如果是图片就要重新计算rect,如果不是照片就正常返回
bool HGridCellBase::textRect( QRect& rect)
{
    if (image() >= 0)
    {

       HGridCtrl* pGrid = grid();
       QImageList* pImageList = pGrid->imageList();
       if(pImageList)
       {
           QImage *image1 = pImageList->value(image());
           if(!image1->isNull())
           {
               int nImageWidth = image1->width()+1;
               rect.setLeft(rect.left() + nImageWidth + margin());
           }
       }
    }

    return true;
}

QSize HGridCellBase::textExtent(const QString& szText, QPainter* painter)
{
    HGridCtrl* pGrid = grid();
    Q_ASSERT(pGrid);

    if (painter == NULL || szText.isEmpty())
    {
        HGridDefaultCell* pDefCell = (HGridDefaultCell*) defaultCell();
        Q_ASSERT(pDefCell);
        return QSize(pDefCell->width(), pDefCell->height());
    }

    CFont *pOldFont = NULL,
          *pFont = GetFontObject();
    if (pFont)
        pOldFont = pDC->SelectObject(pFont);

    QSize size;
    int nFormat = format();

    painter->setFont(font());
    QFontMetrics fontMetrics(font());
    // If the cell is a multiline cell, then use the width of the cell
    // to get the height
    if ((nFormat & QDT_WORDBREAK) && !(nFormat & QDT_SINGLELINE))
    {
        QString str = szText;
        int nMaxWidth = 0;
        //需要这样做吗？--huangw
        while (true)
        {
            int nPos = str.indexOf(_T('\n'));
            QString TempStr = (nPos < 0)? str : str.left(nPos);
            int nTempWidth = fontMetrics.width(TempStr);
            if (nTempWidth > nMaxWidth)
                nMaxWidth = nTempWidth;

            if (nPos < 0)
                break;
            str = str.mid(nPos + 1);    // Bug fix by Thomas Steinborn
        }

        QRect rect;
        rect.setRect(0,0, nMaxWidth+1, 0);
        QRect boundingRect;
        painter->drawText(rect,nFormat,szText,&boundingRect);
        size = boundingRect.size();
    }

    size = fontMetrics.size(nFormat,szText);

    size += QSize(4*margin(), 2*margin());

    // Kludge for vertical text
    LOGFONT *pLF = GetFont();
    if (pLF->lfEscapement == 900 || pLF->lfEscapement == -900)
    {
        int nTemp = size.cx;
        size.cx = size.cy;
        size.cy = nTemp;
        size += CSize(0, 4*GetMargin());
    }

    if (bReleaseDC)
        pGrid->ReleaseDC(pDC);

    return size;
    return QSize();
}

QSize HGridCellBase::cellExtent(QPainter* painter)
{
    return QSize();
}

bool HGridCellBase::validateEdit(QString& str)
{
    return true;
}

    // EFW - Added to print cells properly
bool HGridCellBase::printCell(QPainter* painter, int nRow, int nCol, QRect& rect)
{
    return true;
}

void HGridCellBase::OnEndEdit()
{

}
