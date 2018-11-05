// InPlaceEdit.cpp : implementation file
//
//
/////////////////////////////////////////////////////////////////////////////
#include "hinplaceedit.h"
#include "hgridctrl.h"
#include "hgridcell.h"
/////////////////////////////////////////////////////////////////////////////
// HInPlaceEdit

HInPlaceEdit::HInPlaceEdit(QWidget* pParent, const QRect& rect, quint32 dwStyle,
                           int nRow, int nColumn, QString sInitText)
    :QLineEdit(pParent)
{
    m_sInitText     = sInitText;
    m_nRow          = nRow;
    m_nColumn       = nColumn;

    m_Rect = rect;  // For bizarre CE bug.
    
    setFont(((HGridCtrl*)pParent)->font());
    setAlignment(QDT_LEFT | QDT_CENTER);
    setText(sInitText);
    setFocus();
    connect(this,&HInPlaceEdit::endEditCell,(HGridCtrl*)pParent,&HGridCtrl::onEndEditCell);

}

HInPlaceEdit::~HInPlaceEdit()
{
}

void HInPlaceEdit::onEditingFinished()
{
    endEdit();
}

void HInPlaceEdit::onTextChanged(const QString &text)
{

}

////////////////////////////////////////////////////////////////////////////
// CInPlaceEdit implementation

void HInPlaceEdit::endEdit()
{
    QString str;
    static bool bAlreadyEnding = false;
    if(bAlreadyEnding)
        return;
    bAlreadyEnding = true;
    str = text();
    emit endEditCell(m_nRow, m_nColumn, str);
    bAlreadyEnding = false;
}
