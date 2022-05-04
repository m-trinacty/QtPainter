#include "painter.h"
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>
#include <QList>
#include <QPainterPath>
#include <QPolygon>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QStylePainter>
#include <QTextEdit>

#include <QCheckBox>
#include <QColorDialog>
#include <QFontComboBox>
#include <QMenu>
#include <QSignalMapper>
#include <QSpinBox>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>

#include <QPrintDialog>
#include <QtMath>
#include <algorithm>




Painter::Painter(QWidget *parent): QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    textEdit.setWindowFlags( Qt::SplashScreen |Qt::WindowStaysOnTopHint);
    createTEToolBar();
    createBrushToolBar();
    createShapeToolBar();
    createPolyToolBar();
    createPenToolBar();
    textEdit.mapToParent(QPoint(0,0));

    }

bool Painter::openImage(const QString &fileName)
{
    QImage loadedImage;
    if(!loadedImage.load(fileName))
        return false;

    QSize newSize= loadedImage.size().expandedTo(size());

    resizeImage(&loadedImage,newSize);
    image=loadedImage;
    modified=false;
    update();
    return true;
}

bool Painter::saveImage(const QString &fileName, const char *fileFormat)
{
    QImage actualImage=image;
    resizeImage(&actualImage,size());
    if(actualImage.save(fileName,fileFormat)){
        modified=false;
        return true;
    }
    return false;
}

void Painter::setPenColor(const QColor &newColor)
{
    myPenColor=newColor;
}

void Painter::setPenWidth(int newWidth)
{
    myPenWidth=newWidth;
}

void Painter::setDrawType(DrawTypes newDrawType)
{
    if(newDrawType==DrawTypes::Brush){
        brushToolBar.show();
    }
    else{
        brushToolBar.hide();
    }
    if(newDrawType==DrawTypes::Polygon){
        myPolyAppend=true;
    }
    else{
        myPolyAppend=false;
        polygonPoints.clear();
    }
    if(newDrawType==DrawTypes::Shape){
        shapeToolBar.show();
    }
    else{
        shapeToolBar.hide();
    }

    if(newDrawType==DrawTypes::Polygon){
        polyToolBar.show();
    }
    else{
        polyToolBar.hide();
    }
    if(newDrawType==DrawTypes::Pen){
        penToolBar.show();
    }
    else{
        penToolBar.hide();
    }

    mydrawType=newDrawType;
}

void Painter::setBrushStyle(QString newStyle)
{
    myBrushStyle=newStyle;

}

void Painter::setFontBold()
{
    if(myFontBold){
        textEdit.setFontWeight(QFont::Normal);
        myFontBold=false;
    }
    else{
        textEdit.setFontWeight(QFont::Bold);
        myFontBold=true;
    }
}

void Painter::setFontItalic()
{
    if(myFontItalic){
        myFontItalic=false;
        textEdit.setFontItalic(myFontItalic);
    }
    else{
        myFontItalic=true;
        textEdit.setFontItalic(myFontItalic);
    }

}


void Painter::setShapeFill()
{
    if(myShapeFilled){
        myShapeFilled=false;
    }
    else{
        myShapeFilled=true;
    }

}

void Painter::setShapeType(int newType)
{
    myShapeType=(Painter::ShapeTypes)newType;
}

void Painter::setPolyFill()
{
    if(myPolyFilled){
        myPolyFilled=false;
    }
    else{
        myPolyFilled=true;
    }
}

void Painter::setBrushWidth(int newWidth)
{
    myBrushWidth=newWidth;
}

void Painter::setFontFamily(const QFont &newfont)
{
    textEdit.setFont(newfont.family());
}



void Painter::clearImage()
{
    modified=true;
    image.fill(qRgb(255,255,255));
    update();
}

void Painter::print(QPrinter* printer)
{
    QPainter painter(printer);
    QRect rect = painter.viewport();
    QSize size = image.size();
    size.scale(rect.size(),Qt::KeepAspectRatio);
    painter.setViewport(rect.x(),rect.y(),size.width(),size.height());
    painter.setWindow(image.rect());
    painter.drawImage(0,0,image);
}

void Painter::filePrint()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog dialog(&printer,this);
    if(dialog.exec() ==QDialog::Accepted)
        print(&printer);
}

void Painter::filePrintPreview()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer,this);
    connect(&preview,SIGNAL(paintRequested(QPrinter*)),SLOT(print(QPrinter*)));
    preview.exec();

}



void Painter::mousePressEvent(QMouseEvent *event)
{


    qDebug()<<(event->button());
    if((event->button() == Qt::LeftButton)){
        switch (mydrawType) {
        case DrawTypes::Pen:{
            lastPoint=event->pos();
        }
            break;
        case DrawTypes::Stamp:
            drawPoint(event->pos());
            break;

        case DrawTypes::Line:{
            lastPoint=event->pos();
            tempImage = image;

        }
            break;
        case DrawTypes::Brush:{
            drawBrush(event->pos());
        }
            break;
        case DrawTypes::FillArea:{
            fillColor(event->pos());
        }
            break;
        case DrawTypes::TextEdit:{
            if(!textEditing)
                lastPoint=event->pos();
        }
            break;

        case DrawTypes::Shape:{
            lastPoint=event->pos();

            tempImage = image;
        }
            break;

        case DrawTypes::Polygon:{

            if(myPolyAppend){
                tempImage = image;
                polygonPoints.append(event->pos());
                if(polygonPoints.length()<2){
                    drawPoint(polygonPoints.last());
                }
                else{
                    lastPoint=polygonPoints[polygonPoints.length()-2];
                    drawLine(polygonPoints.last());
                }
            }
        }
            break;
        default:
            break;
        }
        painting=true;

    }

}


void Painter::mouseMoveEvent(QMouseEvent *event)
{

    if(painting){
        switch (mydrawType) {
        case Painter::DrawTypes::Pen:{
            drawPen(event->pos());
        }
            break;
        case DrawTypes::Stamp:
            drawPoint(event->pos());
            break;
        case Painter::DrawTypes::Line:{
            drawLineMove(event->pos());
        }
            break;
        case DrawTypes::Shape:{
            image=tempImage;
            drawShape(event->pos());
        }
            break;
        default:
            break;
        }
    }
}

void Painter::mouseReleaseEvent(QMouseEvent *event)
{
    if((event->button()&Qt::LeftButton) && painting){
        switch (mydrawType) {
        case Painter::DrawTypes::Pen:{
            drawPen(event->pos());
            qDebug("Brush");
        }
            break;
        case Painter::DrawTypes::Stamp:{
            qDebug("Stamp");
            //drawPoint(event->pos());
        }
            break;
        case Painter::DrawTypes::Line:{
            qDebug("Line");
            drawLine(event->pos());
        }break;

        //case (int)Painter::DrawTypes::FillArea:break;
        case DrawTypes::TextEdit:{
            qDebug()<<" EUCLIDIAN:"<<euclidianDistance(lastPoint,event->pos());
            drawTextEditor(event->pos());
        }
            break;
        case DrawTypes::Shape:{
            qDebug()<<"Shape";
            drawShape(event->pos());
        }
            break;
        default:
            break;
        }
        painting=false;
    }

}

void Painter::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Return && textEditing){
        drawText();
    }
    if(event->key()==Qt::Key_Return && myPolyAppend){
        drawPolygon();
    }
    if(event->key()==Qt::Key_Return && (mydrawType==DrawTypes::Brush||mydrawType==DrawTypes::Pen||mydrawType==DrawTypes::Polygon||mydrawType==DrawTypes::Shape)){
        brushToolBar.hide();
        penToolBar.hide();
        polyToolBar.hide();
        shapeToolBar.hide();
    }
}

void Painter::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect dirtyRect=event->rect();
    painter.drawImage(dirtyRect,image,dirtyRect);

}

void Painter::resizeEvent(QResizeEvent *event)
{
    if(width()>image.width()||height()>image.height()){
        int newWidth=qMax(width()+128,image.width());
        int newHeight=qMax(height()+128,image.height());
        resizeImage(&image,QSize(newWidth,newHeight));
        update();
    }
    QWidget::resizeEvent(event);
}

void Painter::drawPen(const QPoint &endPoint)
{
    QPainter painter(&image);
    painter.setPen(QPen(myPenColor,myPenWidth,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
    painter.drawLine(lastPoint,endPoint);
    modified=true;

    int rad = (myPenWidth/2)+2;
    update(QRect(lastPoint,endPoint).normalized().adjusted(-rad,-rad,+rad,+rad));

    lastPoint=endPoint;

}

void Painter::drawBrush(const QPoint &point)
{
    QPainter painter(&image);
    QBrush *brush=new QBrush();
    brush->setStyle((Qt::BrushStyle)brushStyles.indexOf(myBrushStyle));
    brush->setColor(myPenColor);
    painter.setBrush(*brush);
    painter.setPen(Qt::NoPen);
    qDebug()<<"brush:"<<myBrushStyle<<"index:"<<brushStyles.indexOf(myBrushStyle);
    QRect *brushRect = new QRect(point.x()-(myBrushWidth/2),point.y()-(myBrushWidth/2),myBrushWidth,myBrushWidth);
    painter.drawRect(*brushRect);
    modified=true;

    int rad = (myBrushWidth/2)+2;
    update(brushRect->normalized().normalized().adjusted(-rad,-rad,+rad,+rad));

    //lastPoint=endPoint;

}

void Painter::drawLine(const QPoint &endPoint)
{
    QPainter painter(&image);
    painter.setPen(QPen(myPenColor,myPenWidth,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
    painter.drawLine(lastPoint,endPoint);
    modified=true;

    int rad = (myPenWidth/2)+2;
    QSize size = image.size();
    QPoint *rectEnd = new QPoint(size.rwidth(),size.rheight());
    update(QRect(QPoint(0,0),*rectEnd).normalized().adjusted(-rad,-rad,+rad,+rad));
    //lastPoint=endPoint;
}

void Painter::drawLineMove(const QPoint &endPoint)
{
    image=tempImage;
    drawLine(endPoint);
    qDebug("shouldBeDrawn");
    //image=actualImage;
}

void Painter::drawPoint(const QPoint &point)
{
    QPainter painter(&image);
    painter.setPen(QPen(myPenColor,myPenWidth,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
    painter.drawPoint(point);
    modified=true;

    int rad = (myPenWidth/2)+2;
    update(QRect(lastPoint,point).normalized().adjusted(-rad,-rad,+rad,+rad));

    lastPoint=point;
}
void Painter::resizeImage(QImage *image, const QSize &newSize)
{
    if(image->size()==newSize)
        return;

    QImage newImage(newSize,QImage::Format_RGB32);
    newImage.fill(qRgb(255,255,255));

    QPainter painter(&newImage);
    painter.drawImage(QPoint(0,0),*image);
    *image = newImage;
}

void Painter::drawTextEditor(const QPoint &endPoint)
{
    if(euclidianDistance(lastPoint,endPoint)>50 && !textEditing){
        textEdit.clear();
        textEdit.hide();
        //QSize *textRectSize= new QSize(qAbs(event->pos().x()-lastPoint.x()),qAbs(event->pos().y()-lastPoint.y()));
        QSize *textRectSize= new QSize(qAbs(endPoint.x()-lastPoint.x()),qAbs(endPoint.y()-lastPoint.y()));
        textEdit.setFontPointSize(myFontSize);
        textEdit.resize(*textRectSize);

        textEdit.move(mapToGlobal(lastPoint));
        textEditoolBar.move(mapToGlobal(QPoint(lastPoint.x(),lastPoint.y()-30)));
        textEdit.createStandardContextMenu(endPoint);
        textEdit.setPlaceholderText(tr("Write some text"));

        textEditoolBar.show();
        textEdit.show();
        textEditing=true;
    }
}


void Painter::drawShape(const QPoint &endPoint)
{
    image=tempImage;
    QPainter painter(&image);
    painter.setPen(QPen(myPenColor,myPenWidth,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
    QSize *shapeSize= new QSize((endPoint.x()-lastPoint.x()),(endPoint.y()-lastPoint.y()));
    if(myShapeFilled){
        painter.setBrush(myPenColor);
    }
    else{
        painter.setPen(QPen(myPenColor,myPenWidth,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
    }
    switch (myShapeType) {
    case ShapeTypes::Circle:{
        painter.drawEllipse(lastPoint.x(),lastPoint.y(),shapeSize->width(),shapeSize->height());
    }
        break;
    case ShapeTypes::Rectangle:{
        painter.drawRect(lastPoint.x(),lastPoint.y(),shapeSize->width(),shapeSize->height());
    }
        break;
    }


    int rad = (myPenWidth/2)+2;
    QSize size = image.size();
    QPoint *rectEnd = new QPoint(size.rwidth(),size.rheight());
    update(QRect(QPoint(0,0),*rectEnd).normalized().adjusted(-rad,-rad,+rad,+rad));

}

void Painter::drawPolygon()
{
    QPainter painter(&image);
    polyToolBar.hide();
    if(myPolyFilled){
        painter.setBrush(myPenColor);
    }
    else{
        painter.setPen(QPen(myPenColor,myPenWidth,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
    }
    painter.drawPolygon(polygonPoints.toVector().data(),polygonPoints.length());

    QSize size = image.size();
    QPoint *rectEnd = new QPoint(size.rwidth(),size.rheight());
    update(QRect(QPoint(0,0),*rectEnd).normalized());
    myPolyAppend=false;
    polygonPoints.clear();

}

void Painter::drawText()
{
    textEdit.hide();
    textEditoolBar.hide();
    QPainter painter(&image);
    QFont drawFont = textEdit.font();
    drawFont.setPointSizeF(textEdit.fontPointSize());
    drawFont.setWeight(textEdit.fontWeight());
    drawFont.setItalic(myFontItalic);
    painter.setFont(drawFont);
    painter.setPen(myFontColor);
    painter.drawText(QPoint(lastPoint.x(),lastPoint.y()+2*textEdit.fontPointSize()),textEdit.toPlainText());
    QSize size = image.size();
    QPoint *rectEnd = new QPoint(size.rwidth(),size.rheight());
    update(QRect(QPoint(0,0),*rectEnd).normalized());
    textEditing=false;

}

void Painter::setFontColor()
{
    QColor newColor = QColorDialog::getColor(fontColor());
    if(newColor.isValid()){
        myFontColor=newColor;
        textEdit.setTextColor(myFontColor);
    }

}

void Painter::setFontSize(int newSize)
{
    qDebug()<<"newSize:"<<newSize;
    myFontSize = fontSize()+ newSize;

    qDebug()<<"newFontSize:"<<myFontSize;
    textEdit.setFontPointSize(myFontSize);
}

void Painter::createTEToolBar()
{

    textEditoolBar.setWindowFlags( Qt::SplashScreen |Qt::WindowStaysOnTopHint);

    QAction* fontColorAct = new QAction(QIcon(":/paint-can.png"),tr("&Color"),this);
    connect(fontColorAct,&QAction::triggered,this,&Painter::setFontColor);
    textEditoolBar.addAction(fontColorAct);

    QSignalMapper* signalMapper = new QSignalMapper (this) ;

    QAction* fontSizeUpAct = new QAction(QIcon(":/plus.png"),tr("Up"),this);
    connect(fontSizeUpAct,SIGNAL(triggered()),signalMapper,SLOT(map()));
    textEditoolBar.addAction(fontSizeUpAct);

    QAction* fontSizeDownAct = new QAction(QIcon(":/minus.png"),tr("Down"),this);
    connect(fontSizeDownAct,SIGNAL(triggered()),signalMapper,SLOT(map()));
    textEditoolBar.addAction(fontSizeDownAct);

    signalMapper->setMapping(fontSizeUpAct,+1);
    signalMapper->setMapping(fontSizeDownAct,-1);
    connect(signalMapper,SIGNAL(mappedInt(int)),this,SLOT(setFontSize(int)));


    QAction* fontBoldAct = new QAction(QIcon(":/bold.png"),tr("Bold"),this);
    connect(fontBoldAct,&QAction::triggered,this,&Painter::setFontBold);
    textEditoolBar.addAction(fontBoldAct);

    QAction* fontItalicAct = new QAction(QIcon(":/italic.png"),tr("Italic"),this);
    connect(fontItalicAct,&QAction::triggered,this,&Painter::setFontItalic);
    textEditoolBar.addAction(fontItalicAct);

    QFontComboBox* fontBox =new QFontComboBox(&textEdit);
    fontBox->setEditable(false);

    //QAction* fontChangeAct=new QAction(this);
    connect(fontBox,&QFontComboBox::currentFontChanged,this,&Painter::setFontFamily);
    textEditoolBar.addWidget(fontBox);

    textEditoolBar.setIconSize(QSize(20,20));
    textEditoolBar.setFixedHeight(30);

}

void Painter::createBrushToolBar()
{
    brushToolBar.setWindowFlags(Qt::SplashScreen|Qt::WindowStaysOnTopHint);
    brushToolBar.setFloatable(true);

    QMenu *brushes = new QMenu(tr("&Brushes"),this);
    QSignalMapper* signalMapper = new QSignalMapper (this) ;

    for(int i =0;i<brushStyles.length();i++){
        QAction* newAction=new QAction(brushStyles[i],this);

        connect(newAction,SIGNAL(triggered()),signalMapper,SLOT(map()));
        signalMapper->setMapping(newAction,brushStyles[i]);

        connect(signalMapper,SIGNAL(mappedString(QString)),this,SLOT(setBrushStyle(QString)));
        brushes->addAction(newAction);
    }
    QToolButton* toolButton = new QToolButton();


    toolButton->setMenu(brushes);
    toolButton->setText(tr("Brushes"));
    toolButton->setPopupMode(QToolButton::InstantPopup);

    QSpinBox *brushSpinBox = new QSpinBox();
    brushSpinBox->setRange(1,300);
    brushSpinBox->setSingleStep(1);
    brushSpinBox->setValue(myBrushWidth);
    connect(brushSpinBox,SIGNAL(valueChanged(int)),this,SLOT(setBrushWidth(int)));

    brushToolBar.addWidget(toolButton);
    brushToolBar.addWidget(brushSpinBox);
}

void Painter::createShapeToolBar()
{
    shapeToolBar.setWindowFlags(Qt::SplashScreen|Qt::WindowStaysOnTopHint);

    shapeToolBar.setFloatable(true);

    QCheckBox *fillCheckbox = new QCheckBox(tr("Fill"),this);
    connect(fillCheckbox,&QCheckBox::stateChanged,this,&Painter::setShapeFill);
    shapeToolBar.addWidget(fillCheckbox);


    QSignalMapper* signalMapper = new QSignalMapper (this) ;


    QMenu *shapes = new QMenu(tr("&Shapes"),this);
    QAction* circleAct = new QAction(QIcon(":/circle.png"),tr("Circle"),this);
    connect(circleAct,SIGNAL(triggered()),signalMapper,SLOT(map()));
    connect(signalMapper,SIGNAL(mappedInt(int)),this,SLOT(setShapeType(int)));
    signalMapper->setMapping(circleAct,(int)ShapeTypes::Circle);


    QAction* rectAct = new QAction(QIcon(":/square.png"),tr("Rectangle"),this);
    connect(rectAct,SIGNAL(triggered()),signalMapper,SLOT(map()));
    connect(signalMapper,SIGNAL(mappedInt(int)),this,SLOT(setShapeType(int)));
    signalMapper->setMapping(rectAct,(int)ShapeTypes::Rectangle);

    shapes->addAction(circleAct);
    shapes->addAction(rectAct);

    QToolButton* toolButton = new QToolButton();

    toolButton->setMenu(shapes);
    toolButton->setText(tr("Shapes"));
    toolButton->setPopupMode(QToolButton::InstantPopup);


    shapeToolBar.addWidget(toolButton);

}

void Painter::createPolyToolBar()
{
    polyToolBar.setWindowFlags(Qt::SplashScreen|Qt::WindowStaysOnTopHint);
    polyToolBar.setFloatable(true);

    QCheckBox *fillCheckbox = new QCheckBox(tr("Fill"),this);
    connect(fillCheckbox,&QCheckBox::stateChanged,this,&Painter::setPolyFill);
    polyToolBar.addWidget(fillCheckbox);
}

void Painter::createPenToolBar()
{
    penToolBar.setWindowFlags(Qt::SplashScreen|Qt::WindowStaysOnTopHint);
    penToolBar.setFloatable(true);

    QSpinBox *penWidthSpinBox = new QSpinBox();
    penWidthSpinBox->setRange(1,300);
    penWidthSpinBox->setSingleStep(1);
    penWidthSpinBox->setValue(myPenWidth);
    connect(penWidthSpinBox,SIGNAL(valueChanged(int)),this,SLOT(setPenWidth(int)));
    penToolBar.addWidget(penWidthSpinBox);
}



qreal Painter::euclidianDistance(QPoint point1, QPoint point2)
{
    qreal x = point1.x()-point2.x();
    qreal y = point1.y()-point2.y();
    qreal distance;

    distance= qPow(x,2)+qPow(y,2);
    distance= qSqrt(distance);
    return distance;
}

void Painter::fillColor(const QPoint &startPoint)
{
    QPoint actualPoint= startPoint;
    QColor originalColor = image.pixelColor(startPoint);


    QSize imageSize =image.size();
    //int x=actualPoint.x(),y=actualPoint.y();
    enum{up,down,left,right,upLeft,upRight,downLeft,downRight};
    for (int x=startPoint.x();x >= 0 ;x--) {


        actualPoint.setX(x);
        if(image.pixelColor(actualPoint)!=originalColor)
            break;
        for (int y=startPoint.y();y >= 0 ;y--) {
            actualPoint.setY(y);
            if(image.pixelColor(actualPoint)!=originalColor)
                break;
            image.setPixelColor(actualPoint,myPenColor);

        }
        actualPoint.setY(startPoint.y());
    }
    qDebug()<<imageSize;
    actualPoint= startPoint;
    for (int x=startPoint.x()+1;x <= imageSize.rwidth() ;x++) {



        actualPoint.setX(x);
        if(image.pixelColor(actualPoint)!=originalColor){
            qDebug()<<"firstbreak";
            break;
        }
        for (int y=startPoint.y()+1;y <= imageSize.rheight() ;y++) {
            actualPoint.setY(y);
            //qDebug()<<"Painting other side";
            if(image.pixelColor(actualPoint)!=originalColor){
                qDebug()<<"Secondbreak";
                break;
            }
            image.setPixelColor(actualPoint,myPenColor);
            //qDebug()<<"Painting other side";

        }
        actualPoint.setY(startPoint.y());
    }

    ////////////////////////////////////////

    for (int x=startPoint.x()+1;x < imageSize.rwidth() ;x++) {

        qDebug()<<"rigth up";
        actualPoint.setX(x);
        if(image.pixelColor(actualPoint)!=originalColor)
            break;
        for (int y=startPoint.y();y >= 0 ;y--) {
            actualPoint.setY(y);
            if(image.pixelColor(actualPoint)!=originalColor)
                break;
            image.setPixelColor(actualPoint,myPenColor);

        }
        actualPoint.setY(startPoint.y());
    }
    qDebug()<<imageSize;
    actualPoint= startPoint;
    actualPoint.setY(startPoint.y()+1);
    for (int x=startPoint.x();x >=0 ;x--) {

        qDebug()<<"left ddown";


        actualPoint.setX(x);
        if(image.pixelColor(actualPoint)!=originalColor){
            qDebug()<<"firstbreak";
            break;
        }
        for (int y=startPoint.y()+1;y <= imageSize.rheight() ;y++) {
            actualPoint.setY(y);
            //qDebug()<<"Painting other side";
            if(image.pixelColor(actualPoint)!=originalColor){
                qDebug()<<"Secondbreak";
                break;
            }
            image.setPixelColor(actualPoint,myPenColor);
            //qDebug()<<"Painting other side";

        }
        actualPoint.setY(startPoint.y()+1);
    }
/*
 *
 * This commented attempt of filling area with color scans through whole image and when cross collor difference betwen  originall
 * color where was clicked, it add it to point list from which is then created polygon and then through QPainterPath filled
 * but it's not working properly, when therese more than one area to fill or when borderline of area has over draws
 *
 *
 */

//    QList<QPoint> path;

//    int raycast=0;
//    tempImage.fill(qRgb(255,255,255));

//    for(int xPos=0;xPos<imageSize.rwidth() ;xPos++ ) {
//        actualPoint.setX(xPos);
//        for (int yPos=0; yPos< imageSize.rheight();yPos++ ) {
//            actualPoint.setY(yPos);
//            if(image.pixelColor(actualPoint)!=originalColor){

//                    path.append(actualPoint);
//                    tempImage.setPixelColor(actualPoint,Qt::green);
//            }
//        }
//    }

//    qDebug()<<"euclidian path:"<<euclidianDistance(QPoint(49,49),QPoint(50,50));
//    QBrush brush;
//    brush.setColor(Qt::green);
//    brush.setStyle(Qt::BrushStyle::SolidPattern);

//    QPainter painter(&image);
//    QPolygon poly(path.toVector());
//    QPainterPath painterPath;

//    qDebug()<<(poly.containsPoint(startPoint,Qt::FillRule::OddEvenFill))<<"and"<<polyList.contains(poly);
//    if(poly.containsPoint(startPoint,Qt::FillRule::OddEvenFill)){
//        qDebug()<<"filling";
//        painterPath.addPolygon(poly);
//        painter.drawPolygon(poly);
//        painter.fillPath(painterPath,brush);
//    }


//    polyList.append(poly);
//    path.clear();
    int rad = (myPenWidth/2)+2;
    QPoint *rectEnd = new QPoint(imageSize.rwidth(),imageSize.rheight());
    update(QRect(QPoint(0,0),*rectEnd).normalized().adjusted(-rad,-rad,+rad,+rad));
}


