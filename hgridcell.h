#ifndef HGRIDCELL_H
#define HGRIDCELL_H

class HGridCellBase;
class HInPlaceEdit;
#include "hgridcellbase.h"

class HGridCell : public HGridCellBase
{
    Q_OBJECT
public:
    HGridCell(HGridCellBase *parent = 0);
    virtual ~HGridCell();

// Attributes
public:
    void operator=( HGridCell& cell);

    virtual void  setText(const QString& szText)            { m_strText = szText;  }
    virtual void  setImage(int nImage)                      { m_nImage = nImage;   }
    virtual void  setData(const QVariant& var)              { m_lParam = var;      } //设置数据
    virtual void  setGrid(HGridCtrl* pGrid)                 { m_pGrid = pGrid;     }
    // virtual void SetState(const DWORD nState);  -  use base class version
    virtual void  setFormat(quint32 nFormat)                { m_nFormat = nFormat; }  //格式
    virtual void  setTextClr(const QColor& clr)             { m_crFgClr = clr;     }
    virtual void  setBackClr(const QColor& clr)             { m_crBkClr = clr;     }
    virtual void  setFont(const QFont& font)                { m_plfFont = font;    }
    virtual void  setMargin(uint nMargin)                   { m_nMargin = nMargin; }
    //virtual CWnd* GetEditWnd() const                      { return m_pEditWnd;   }
    virtual void  setCoords(int /*nRow*/, int /*nCol*/) {}  // don't need to know the row and
                                                            // column for base implementation


    virtual QString     text()    const                     { return m_strText; }
    virtual int         image()   const                     { return m_nImage;  }
    virtual QVariant    data()    const                     { return m_lParam;  }
    virtual HGridCtrl*  grid()    const                     { return m_pGrid;   }
    virtual quint32     format()  const;
    virtual QColor      textClr() const                     { return m_crFgClr; } // TODO: change to use default cell
    virtual QColor      backClr() const                     { return m_crBkClr; }
    virtual QFont       font()    const                     { return m_plfFont; }
    virtual int         margin()  const                     { return m_nMargin; }

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


    virtual	void   clearBorderColor()       {return;}
    virtual	void   clearBorderBottomColor() {return ;}
    virtual	void   clearBorderRightColor()  {;}
    virtual	void   clearBorderTopColor()    {;}
    virtual	void   clearBorderLeftColor()   {;}

    virtual bool   isDefaultFont()       const               { return false; }
    virtual bool   isEditing()           const               { return m_bEditing; }
    virtual void   reset();

// editing cells
public:
    virtual bool edit(int nRow, int nCol, const QRect& rect, const QPoint& point);
    virtual void endEdit();
    virtual void OnEndEdit();

protected:
    QString    m_strText;      // Cell text (or binary data if you wish...)
    QVariant   m_lParam;       // 32-bit value to associate with item
    int        m_nImage;       // Index of the list view item icon
    quint32    m_nFormat;    //格式
    quint32    m_nTextOption; //文字环绕标志 多行
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
    HInPlaceEdit *m_pEditWnd;
};

// This class is for storing grid default values. It's a little heavy weight, so
// don't use it in bulk

class HGridDefaultCell : public HGridCell
{
// Construction/Destruction
public:
    HGridDefaultCell();
    virtual ~HGridDefaultCell();

public:
    virtual quint32 style() const                         { return m_dwStyle;         }
    virtual void    setStyle(quint32 dwStyle)             { m_dwStyle = dwStyle;      }
    virtual int     width() const                         { return m_Size.width();    }
    virtual int     height() const                        { return m_Size.height();   }
    virtual void    setWidth(int nWidth)                  { m_Size.setWidth(nWidth);  }
    virtual void    setHeight(int nHeight)                { m_Size.setHeight(nHeight);}

    // Disable these properties
    virtual void     setData(HLPARAM )                    { Q_ASSERT(false);          }
    virtual void     setState(quint32 )                   { Q_ASSERT(false);          }
    virtual quint32  state() const                        { return HGridCell::state()|GVIS_READONLY; }
    virtual void     setCoords( int row, int col)         { Q_ASSERT(false);          }
    virtual void     setFont(const QFont& plf);//            { m_Font = plf;             }
    virtual QFont    font() const                         { return m_Font;            }
protected:
    QSize   m_Size;       // Default Size
    QFont   m_Font;       // Cached font
    quint32 m_dwStyle;    // Cell Style - unused
};

#endif // HGRIDCELL_H
