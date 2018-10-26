 #include "hgridcellbase.h"

 HGridCellBase::HGridCellBase()
 {

 }

HGridCellBase::~HGridCellBase()
{

}

HGridCellBase* HGridCellBase::getDefaultCell() const
{
    if (getGrid())
        return getGrid()->getDefaultCell(IsFixedRow(), IsFixedCol());
    return NULL;
}

void HGridCellBase::reset()
{

}

bool HGridCellBase::draw(QPainter* painter, int nRow, int nCol, QRect rect, bool bEraseBkgnd)
{
    return true;
}
    //如果是图片的话就获取对应的矩形，不是就算了。不能const引用，draw转过来的rect需要做判断 ---huangw
bool HGridCellBase::getTextRect( QRect& rect)
{
    return true;
}

QSize HGridCellBase::getTextExtent(QString& str, QPainter* painter = NULL)
{
    return QSize();
}

QSize HGridCellBase::getCellExtent(QPainter* painter)
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

void HGridCellBase::OnMouseEnter()
{

}

void HGridCellBase::OnMouseOver()
{

}

void HGridCellBase::OnMouseLeave()
{

}

void HGridCellBase::OnClick( QPoint PointCellRelative)
{

}

void HGridCellBase::OnClickDown( QPoint PointCellRelative)
{

}

void HGridCellBase::OnRClick( QPoint PointCellRelative)
{

}

void HGridCellBase::OnDblClick( QPoint PointCellRelative)
{

}

bool HGridCellBase::OnSetCursor()
{
    return true;
}
