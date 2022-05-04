#ifndef PAINTER_H
#define PAINTER_H

#include <QMenuBar>
#include <QObject>
#include <QPrinter>
#include <QTextEdit>
#include <QToolBar>
#include <QWidget>

class Painter: public QWidget
{
    Q_OBJECT
public:
    Painter(QWidget *parent = nullptr);

    enum class DrawTypes{Pen,Brush, Stamp,Line,FillArea,TextEdit,Shape,Polygon};


    enum class ShapeTypes{Circle,Rectangle};

    bool openImage(const QString &fileName);
    bool saveImage(const QString &fileName,const char *fileFormat);

    void setPenColor(const QColor &newColor);
    void setDrawType(DrawTypes newDrawType);

    void setFontColor();
    void setFontBold();
    void setFontItalic();

    void setBrushColor(QColor &newColor);

    void setShapeFill();

    void setPolyFill();


    bool isModified() const {return modified;}

    QColor penColor() const {return myPenColor;}
    int penWidth() const {return myPenWidth;}

    DrawTypes drawType() const {return mydrawType;}

    QString brushStyle()const{return myBrushStyle;}
    int brushWidth() const {return myBrushWidth;}
    QColor brushColor() const {return myBrushColor;}

    QColor fontColor() const {return myFontColor;}
    int fontSize() const {return myFontSize;}
    bool fontBold()const {return myFontBold;}
    bool fontItalic() const {return myFontItalic;}

    bool shapeFill() const {return myShapeFilled;}
    ShapeTypes shapeType()const {return myShapeType;}

    bool polyFill() const {return myPolyFilled;}

public slots:
    void clearImage();

    void print(QPrinter* printer);
    void filePrint();
    void filePrintPreview();

    void setPenWidth(int newWidth);


    void setFontFamily(const QFont &newfont);
    void setFontSize(int newSize);

    void setBrushStyle(QString newStyle);
    void setBrushWidth(int newWidth);


    void setShapeType(int newType);


protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void drawPen(const QPoint &endPoint);
    void drawBrush(const QPoint &point);

    void drawLine(const QPoint &endPoint);
    void drawLineMove(const QPoint &endPoint);
    void drawPoint(const QPoint &point);

    void drawPolygon();

    void drawText();

    void drawShape(const QPoint &endPoint);

    void resizeImage(QImage *image,const QSize &newSize);

    void drawTextEditor(const QPoint &endPoint);

    void createPenToolBar();
    void createTEToolBar();
    void createBrushToolBar();
    void createShapeToolBar();
    void createPolyToolBar();

    qreal euclidianDistance(QPoint point1,QPoint point2);
    
    void fillColor(const QPoint &startPoint);

    bool modified =false;
    bool painting=false;


    DrawTypes mydrawType= DrawTypes::Pen;

    QToolBar penToolBar;
    int myPenWidth = 1;
    QColor myPenColor = Qt::black;

    QColor myFontColor = Qt::black;
    int myFontSize =11;
    bool myFontBold =false;
    bool myFontItalic =false;

    bool myPolyAppend = false;

    QImage image;
    QPoint lastPoint;

    QTextEdit textEdit;
    QToolBar textEditoolBar;
    bool textEditing=false;

    QToolBar brushToolBar;
    QColor myBrushColor= Qt::black;
    QString myBrushStyle="SolidPattern";
    int myBrushWidth=10;

    QToolBar shapeToolBar;
    bool myShapeFilled=false;
    ShapeTypes myShapeType = ShapeTypes::Circle;

    QToolBar polyToolBar;
    bool myPolyFilled= false;



    QImage tempImage;
    QList<QPoint> polygonPoints;

    QList<QPolygon> polyList;
    QList<QString> brushStyles{"NoBrush",
                                "SolidPattern",
                                "Dense1Pattern",
                                "Dense2Pattern",
                                "Dense3Pattern",
                                "Dense4Pattern",
                                   "Dense5Pattern",
                                   "Dense6Pattern",
                                   "Dense7Pattern",
                                   "HorPattern",
                                   "VerPattern",
                                   "CrossPattern",
                                   "BDiagPattern",
                                   "FDiagPattern",
                                   "DiagCrossPattern",
                                   "LinearGradientPattern",
                                   "ConicalGradientPattern",
                                   "RadialGradientPattern",
                                   "TexturePattern"};
};

#endif // PAINTER_H
