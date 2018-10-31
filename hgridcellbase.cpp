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

QSize HGridCellBase::textExtent(const QString& str, QPainter* painter)
{
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
