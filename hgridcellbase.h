#ifndef HGRIDCELLBASE_H_
#define HGRIDCELLBASE_H_
#include "hgridcelldef.h"
#include "hcellrange.h"
class HGridCtrl;
class HGridCellBase : public QObject
{
    Q_OBJECT
    friend class HGridCtrl;
public:
    HGridCellBase(QObject* parent = 0);
    virtual ~HGridCellBase();

// 单元格相关属性
public:
    virtual void setText(const QString&)       = 0 ; //设置文字
    virtual void setImage(int)                 = 0 ; //设置图片
    virtual void setData(const QVariant & )    = 0 ; //设置数据
    virtual void setState(quint32 nState)      { m_nState = nState; }
    virtual void setFormat(quint32 )           = 0 ; //格式
    virtual void setTextClr(const QColor&)      = 0 ; //文字颜色
    virtual void setBackClr(const QColor& )     = 0 ; //背景颜色
    virtual void setFont(const QFont& )         = 0 ;
    virtual void setMargin( uint )             = 0 ;
    virtual void setGrid(HGridCtrl* )          = 0 ;
    virtual void setCoords( int, int)          = 0 ; //row,col

    
    
    virtual QString    text()       const      = 0 ;
    virtual QString    tipText()    const      { return text(); } // may override TitleTip return
    virtual int        image()      const      = 0 ;
    virtual QVariant   data()       const      = 0 ;
    virtual quint32    state()      const      { return m_nState;  }
    virtual quint32    format()     const      = 0 ;
    virtual QColor     textClr()    const      = 0 ;
    virtual QColor     backClr()    const      = 0 ;
    virtual QFont      font()       const      = 0 ;
    virtual HGridCtrl* grid()    const      = 0 ;
    virtual int        margin()   const  =0;

    
    //virtual CWnd     * GetEditWnd()    const                = 0 ;
    //virtual CFont    * GetFontObject() const                = 0 ;

    virtual bool isDefaultFont()       const   = 0 ;
    virtual bool isEditing()           const   = 0 ;
    virtual bool isFocused()           const   { return (m_nState & GVIS_FOCUSED);  }
    virtual bool isFixed()             const   { return (m_nState & GVIS_FIXED);    }
    virtual bool isFixedCol()          const   { return (m_nState & GVIS_FIXEDCOL); }
    virtual bool isFixedRow()          const   { return (m_nState & GVIS_FIXEDROW); }
    virtual bool isSelected()          const   { return (m_nState & GVIS_SELECTED); }
    virtual bool isReadOnly()          const   { return (m_nState & GVIS_READONLY); }
    virtual bool isModified()          const   { return (m_nState & GVIS_MODIFIED); }
    virtual bool isDropHighlighted()   const   { return (m_nState & GVIS_DROPHILITED); }

public:
	//单元格边框相关属性
    virtual void   setBorderStyle(short style)          = 0 ;//设置边框风格，实线，虚线等
    virtual void   setBorderColor(const QColor&)        = 0 ;
    virtual short  borderStyle()                  const = 0 ;
    virtual QColor borderColor()                  const = 0 ;

    virtual bool   isDrawBorder()           = 0 ;
    virtual	bool   isDrawBorderBottom()     = 0 ;
    virtual	bool   isDrawBorderRight()      = 0 ;
    virtual	bool   isDrawBorderTop()        = 0 ;
    virtual	bool   isDrawBorderLeft()       = 0 ;

    //风格
    virtual void setBorderBottomStyle(ushort style)     = 0 ;
    virtual void setBorderRightStyle(ushort style)      = 0 ;
    virtual void setBorderTopStyle(ushort style)        = 0 ;
    virtual void setBorderLeftStyle(ushort style)       = 0 ;

    virtual ushort   borderBottomStyle()   const = 0 ;
    virtual ushort   borderRightStyle()    const = 0 ;
    virtual ushort   borderTopStyle()      const = 0 ;
    virtual ushort   borderLeftStyle()     const = 0 ;

    //颜色
    virtual void setBorderBottomColor(const QColor&)     = 0 ;
    virtual void setBorderRightColor(const QColor&)      = 0 ;
    virtual void setBorderTopColor(const QColor&)        = 0 ;
    virtual void setBorderLeftColor(const QColor&)       = 0 ;

	virtual QColor   borderBottomColor()   const = 0 ;
	virtual QColor   borderRightColor()    const = 0 ;
    virtual QColor   borderTopColor()      const = 0 ;
    virtual QColor   borderLeftColor()     const = 0 ;

	virtual	void   clearBorderColor()       = 0 ;
	virtual	void   clearBorderBottomColor() = 0 ;
	virtual	void   clearBorderRightColor()  = 0 ;
	virtual	void   clearBorderTopColor()    = 0 ;
	virtual	void   clearBorderLeftColor()   = 0 ;


    //设置网格类型
    /*virtual void    setCellType(int nType)                =0;
    virtual void    setCellTypeEx(long nType)             =0;
    virtual int     getCellType()              const      =0;
    virtual long    getCellTypeEx()            const      =0;
    virtual	bool    isPropertySet(long flag)              =0;
    virtual	long    getPropertyFlags()                    =0;
    virtual short   getBorderStyle()           const      =0;*/

public:
   virtual HGridCellBase* defaultCell() const;

// Operators
public:
    virtual void operator=( HGridCellBase& cell);

// Operations
public:

    virtual void reset();

    virtual bool draw(QPainter* painter, int nRow, int nCol, QRect rect, bool bEraseBkgnd = true);
    //如果是图片的话就获取对应的矩形，不是就算了。不能const引用，draw转过来的rect需要做判断 ---huangw
    virtual bool textRect( QRect& rect);    // i/o:  i=dims of cell rect; o=dims of text rect
    virtual bool tipTextRect( QRect&  rect) { return textRect( rect); }  // may override for btns, etc.
    virtual QSize textExtent(const QString& str, QPainter* painter = NULL);
    virtual QSize cellExtent(QPainter* painter);

    // Editing
    virtual bool edit( int /* nRow */, int /* nCol */, QRect /* rect */, QPoint /* point */,
                       uint /* nID */, uint /* nChar */) { Q_ASSERT( false); return false;}
    virtual bool validateEdit(QString& str);
    virtual void endEdit() {}

    // EFW - Added to print cells properly
    virtual bool printCell(QPainter* painter, int nRow, int nCol, QRect& rect);

    // add additional protected grid members required of cells
    //LRESULT SendMessageToParent(int nRow, int nCol, int nMessage);

protected:
    virtual void OnEndEdit();
public:
    /*bool IsMerged();
	void SetMergeRange(HCellRange range);
    void Show(bool IsShow);
	void UnMerge();
    virtual bool IsShow() ;
    virtual HCellRange GetMergeRange();
    virtual bool IsMergeWithOthers();
	virtual HCellID GetMergeCellID();
	virtual void SetMergeCellID(HCellID cell);*/
protected:
    quint32    m_nState;      // Cell state (selected/focus etc)
private:
/*
    HCellRange m_MergeRange;
    bool m_IsMergeWithOthers;
	HCellID m_MergeCellID;
    bool m_Hide;
    */
};

#endif
