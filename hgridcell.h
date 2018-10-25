#ifndef HGRIDCELL_H
#define HGRIDCELL_H

class HGridCellBase;
#include "hgridcellbase.h"

class HGridCell : public HGridCellBase
{
    Q_OBJECT
public:
    explicit HGridCell(QObject *parent = 0);
public:
    HGridCell();
    virtual ~HGridCell();

// Attributes
public:
    void operator=( HGridCell& cell);

    virtual void  setText(const QString& szText)            { m_strText = szText;  }
    virtual void  setImage(int nImage)                      { m_nImage = nImage;   }
    virtual void  setData(const QVariant& var)              { m_lParam = var;   }
    virtual void  setGrid(HGridCtrl* pGrid)                 { m_pGrid = pGrid;     }
    // virtual void SetState(const DWORD nState);  -  use base class version
    virtual void  setFormat(quint32 nFormat)                { m_nFormat = nFormat; }
    virtual void  setTextClr(const QColor& clr)             { m_crFgClr = clr;     }
    virtual void  setBackClr(const QColor& clr)             { m_crBkClr = clr;     }
    virtual void  setFont(const QFont& font)                { m_plfFont = font;    }
    virtual void  setMargin(uint nMargin)                   { m_nMargin = nMargin; }
    //virtual CWnd* GetEditWnd() const             { return m_pEditWnd;   }
    virtual void  setCoords(int /*nRow*/, int /*nCol*/) {}  // don't need to know the row and
                                                            // column for base implementation
    virtual void setWidth(int width)                        { m_Size.setWidth(width); }
    virtual void setHeight(int height)                      { m_Size.setHeight(height); }

    virtual QString     text()    const                     { return m_strText; }
    virtual int         image()   const                     { return m_nImage;  }
    virtual QVariant    data()    const                     { return m_lParam;  }
    virtual HGridCtrl*  grid()    const                     { return m_pGrid;   }
    virtual quint32     format()  const;
    virtual QColor      textClr() const                     { return m_crFgClr; } // TODO: change to use default cell
    virtual QColor      backClr() const                     { return m_crBkClr; }
    virtual QFont       font()    const                     { return m_plfFont; }
    virtual uint        margin()  const                     { return m_nMargin; }
    virtual int         width()   const                     { return m_Size.width(); }
    virtual int         height()  const                     { return m_Size.height(); }

    //单元格边框相关属性
    virtual void   setBorderStyle(short style)              { m_nBorderStyle = style; }
    virtual void   setBorderColor(const QColor& clr)        { m_crBoderClr = clr; }
    virtual short  borderStyle() const                      { return m_nBorderStyle; }
    virtual QColor borderColor() const                      { return m_crBoderClr; }

    virtual bool   isDrawBorder()                           { return m_bBorder; }
    virtual	bool   isDrawBorderBottom()                     { return m_bBottomBorder; }
    virtual	bool   isDrawBorderRight()                      { return m_bRightBorder; }
    virtual	bool   isDrawBorderTop()                        { return m_bTopBorder; }
    virtual	bool   isDrawBorderLeft()                       { return m_bLeftBorder; }

    //风格
    virtual void setBorderBottomStyle(ushort style)         { m_nBottomBorderStyle = style; }
    virtual void setBorderRightStyle(ushort style)          { m_nRightBorderStyle = style; }
    virtual void setBorderTopStyle(ushort style)            { m_nTopBorderStyle = style; }
    virtual void setBorderLeftStyle(ushort style)           { m_nLeftBorderStyle = style; }

    virtual ushort   borderBottomStyle() const              { return m_nBottomBorderStyle;}
    virtual ushort   borderRightStyle() const               { return m_nRightBorderStyle; }
    virtual ushort   borderTopStyle() const                 { return m_nTopBorderStyle;   }
    virtual ushort   borderLeftStyle() const                { return m_nLeftBorderStyle;  }

    //颜色
    virtual void setBorderBottomColor(const QColor& clr)    { m_crBottomBoderClr = clr;   }
    virtual void setBorderRightColor(const QColor& clr)     { m_crRightBoderClr = clr;    }
    virtual void setBorderTopColor(const QColor& clr)       { m_crTopBoderClr = clr;      }
    virtual void setBorderLeftColor(const QColor& clr)      { m_crLeftBoderClr = clr;     }

    virtual QColor   borderBottomColor()   const            { return m_crBottomBoderClr;  }
    virtual QColor   borderRightColor()    const            { return m_crRightBoderClr;   }
    virtual QColor   borderTopColor()      const            { return m_crTopBoderClr;     }
    virtual QColor   borderLeftColor()     const            { return m_crLeftBoderClr;    }


    virtual	void   clearBorderColor()       = {};
    virtual	void   clearBorderBottomColor() = {};
    virtual	void   clearBorderRightColor()  = {};
    virtual	void   clearBorderTopColor()    = {};
    virtual	void   clearBorderLeftColor()   = {};

    virtual bool   isEditing() const           { return m_bEditing; }
    virtual void   reset();

// editing cells
public:
    virtual BOOL edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
    virtual void endEdit();
protected:
    virtual void onEndEdit();

protected:
    QString    m_strText;      // Cell text (or binary data if you wish...)
    QVariant   m_lParam;       // 32-bit value to associate with item
    int        m_nImage;       // Index of the list view item icon
    quint32    m_nFormat;    //格式
    QColor     m_crFgClr;    //前景颜色就是文字颜色
    QColor     m_crBkClr;    //背景颜色
    QFont      m_plfFont;    //字体
    uint       m_nMargin;    //页边
    QSize      m_Size;            // Default Size

    bool        m_bBorder;      //设置边框
    short       m_nBorderStyle; //边框类型
    QColor      m_crBoderClr;   //边框颜色

    bool        m_bLeftBorder;         //设置左边框
    short       m_nLeftBorderStyle;    //设置左边框类型
    QColor      m_crLeftBoderClr;      //边框颜色
    bool        m_bTopBorder;          //设置上边框
    short       m_nTopBorderStyle;     //设置上边框类型
    QColor      m_crTopBoderClr;       //上边框颜色
    bool        m_bRightBorder;        //设置右边框
    short       m_nRightBorderStyle;   //设置右边框类型
    QColor      m_crRightBoderClr;     //右边框颜色
    bool        m_bBottomBorder;       //设置左边框
    short       m_nBottomBorderStyle;  //设置左边框类型
    QColor      m_crBottomBoderClr;    //边框颜色

    bool       m_bEditing;     // Cell being edited?

    HGridCtrl* m_pGrid;        // Parent grid control
    //CWnd*      m_pEditWnd;
};

#endif // HGRIDCELL_H