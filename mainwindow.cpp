#include "mainwindow.h"
#include "painter.h"

#include <QApplication>
#include <QColorDialog>
#include <QFileDialog>
#include <QImageWriter>
#include <QInputDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QCloseEvent>
#include <QSpinBox>
#include <QDialogButtonBox>


MainWindow::MainWindow(QWidget *parent):QMainWindow(parent),painterArea(new Painter(this))
{
    painterArea = new Painter;
    setCentralWidget(painterArea);

    createActions();
    createMenus();
    createToolBar();
    createnewForm();
    setWindowTitle(tr("Painter"));
    resize(500, 500);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(maybeSave()){
        event->accept();
    }
    else{
        event->ignore();
    }
}

void MainWindow::open()
{
    if(maybeSave()){
        QString fileName = QFileDialog::getOpenFileName(this,tr("openFile"),QDir::currentPath());
        if(!fileName.isEmpty())
            painterArea->openImage(fileName);
    }
}

void MainWindow::save()
{
    QAction *action = qobject_cast<QAction*>(sender());
    QByteArray fileFormat = action->data().toByteArray();
    saveFile(fileFormat);
}

void MainWindow::newPainter()
{
    qDebug()<<"newFile";
    maybeSave();

    if(newDialog->exec()==QDialog::Accepted)
    {
        painterArea->clearImage();
        resize(newWidth->value(),newHeight->value());
        newDialog->hide();
    }
}

void MainWindow::penColor()
{
    QColor newColor = QColorDialog::getColor(painterArea->penColor());
    if(newColor.isValid())
        painterArea->setPenColor(newColor);
}

void MainWindow::penWidth()
{
    bool ok;
    int newWidth=QInputDialog::getInt(this,tr("Painter"),tr("Select pen width:"),painterArea->penWidth(),1,50,1,&ok);
    if(ok)
        painterArea->setPenWidth(newWidth);
}



void MainWindow::drawPenType()
{
    painterArea->setDrawType(Painter::DrawTypes::Pen);
}
void MainWindow::drawBrushType()
{
    painterArea->setDrawType(Painter::DrawTypes::Brush);
}

void MainWindow::drawShapeType()
{
    painterArea->setDrawType(Painter::DrawTypes::Shape);
}

void MainWindow::drawPolygonType()
{
    painterArea->setDrawType(Painter::DrawTypes::Polygon);

}
void MainWindow::drawLineType()
{
    painterArea->setDrawType(Painter::DrawTypes::Line);
}

void MainWindow::drawStampType()
{
    painterArea->setDrawType(Painter::DrawTypes::Stamp);
}

void MainWindow::fillAreaType()
{
    painterArea->setDrawType(Painter::DrawTypes::FillArea);
}

void MainWindow::textEditType()
{
    painterArea->setDrawType(Painter::DrawTypes::TextEdit);
}


void MainWindow::about()
{
    QMessageBox::about(this, tr("About Painter"),
                tr("<p>The <b>Painter</b>"
                   "This is final project for subject: multiplatform programming"
                   "With this little painter you can draw continously like with pen "
                   "or use brush with different patterns (included in Qt), fill closed area "
                   "with color, draw circles,elipses,squares and rectangles, filled or unfilled "
                   "Draw different shapes with polygon drawing, straight lines, which will also "
                   "show you the way you moving with your mouse until you release and print the line. "
                   "With poly draw active, by clicking and creating path, then submiting with enter. "
                   "For creating and printig text on paint area you have to select [T] option and drag "
                   "somewhere on screen rectangle where will show up text editor, with options to change"
                   "font and other stuff. Then by clicking somewhere else out of text edit area you deactivate"
                   "text editing and by submiting with enter you will print the text on painter area"
                   "</p>"));
}

void MainWindow::createActions()
{
    newPainterAct = new QAction(tr("New"));
    connect(newPainterAct,&QAction::triggered,this,&MainWindow::newPainter);

    openAct = new QAction(tr("&Open..."),this);
    openAct->setShortcuts(QKeySequence::Open);
    connect(openAct,&QAction::triggered,this,&MainWindow::open);

    foreach(QByteArray format, QImageWriter::supportedImageFormats()){
        QString text = tr("%1...").arg(QString(format).toUpper());

        QAction *action = new QAction(text,this);
        action->setData(format);
        connect(action,&QAction::triggered,this,&MainWindow::save);
        saveAsActs.append(action);
    }

    printAct = new QAction(tr("&Print..."),this);
    connect(printAct,&QAction::triggered,painterArea,&Painter::filePrintPreview);

    exitAct = new QAction(tr("&Exit..."),this);
    exitAct->setShortcuts(QKeySequence::Quit);
    connect(exitAct,&QAction::triggered,this,&MainWindow::close);

    penColorAct = new QAction(tr("&Pen Color..."),this);
    connect(penColorAct,&QAction::triggered,this,&MainWindow::penColor);

    penWidthAct = new QAction(tr("&Pen Width..."),this);
    connect(penWidthAct,&QAction::triggered,this,&MainWindow::penWidth);

    drawPenAct = new QAction(QIcon(":/pen.png"),tr("&Pen"),this);
    connect(drawPenAct,&QAction::triggered,this,&MainWindow::drawPenType);


    drawBrushAct = new QAction(QIcon(":/paint-brush.png"),tr("&Brush"),this);
    connect(drawBrushAct,&QAction::triggered,this,&MainWindow::drawBrushType);

    drawStampAct= new QAction(QIcon(":/stamp.png"),tr("&Stamp"),this);
    connect(drawStampAct,&QAction::triggered,this,&MainWindow::drawStampType);

    drawLineAct=new QAction(QIcon(":/line.png"),tr("&Line"),this);
    connect(drawLineAct,&QAction::triggered,this,&MainWindow::drawLineType);

    fillAreaAct = new QAction(QIcon(":/paint-bucket.png"),tr("&Fill Area"),this);
    connect(fillAreaAct,&QAction::triggered,this,&MainWindow::fillAreaType);

    textEditAct = new QAction(QIcon(":/text.png"),tr("&Text Edit"),this);
    connect(textEditAct,&QAction::triggered,this,&MainWindow::textEditType);

    drawShapeAct = new QAction(QIcon(":/shape.png"),tr("&Shape"),this);
    connect(drawShapeAct,&QAction::triggered,this,&MainWindow::drawShapeType);

    drawPolyAct = new QAction(QIcon(":/polygon.png"),tr("&Polygon"),this);
    connect(drawPolyAct,&QAction::triggered,this,&MainWindow::drawPolygonType);


    clearScreenAct = new QAction(tr("&Clear Screen.."),this);
    clearScreenAct->setShortcut(tr("Ctrl+L"));
    connect(clearScreenAct,&QAction::triggered,painterArea,&Painter::clearImage);

    aboutAct = new QAction(tr("&About..."),this);
    connect(aboutAct,/*&QAction::triggered*/SIGNAL(triggered()),this,/* &MainWindow::aboutAct*/SLOT(about()));

    aboutQtAct=new QAction(tr("&About &Qt"),this);
    connect(aboutQtAct,/*&QAction::triggered*/SIGNAL(triggered()),this, /*&MainWindow::aboutQtAct*/SLOT(about()));

}

void MainWindow::createMenus()
{
    saveAsMenu = new QMenu(tr("&Save As"),this);
    for(QAction *action : qAsConst(saveAsActs))
        saveAsMenu->addAction(action);

    fileMenu = new QMenu(tr("&File"),this);

    fileMenu->addAction(newPainterAct);
    fileMenu->addAction(openAct);
    fileMenu->addMenu(saveAsMenu);
    fileMenu->addSeparator();
    fileMenu->addAction(clearScreenAct);
    fileMenu->addSeparator();
    fileMenu->addAction(printAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    optionMenu = new QMenu(tr("&Paint"),this);
    optionMenu->addAction(drawPenAct);
    optionMenu->addAction(drawBrushAct);
    optionMenu->addAction(penColorAct);
    optionMenu->actions().takeLast()->setIcon(QIcon(":/paint-can.png"));
    optionMenu->addAction(drawStampAct);
    optionMenu->addAction(drawLineAct);
    optionMenu->addAction(fillAreaAct);
    optionMenu->addAction(textEditAct);
    optionMenu->addAction(drawShapeAct);
    optionMenu->addAction(drawPolyAct);



    helpMenu= new QMenu(tr("&Help"),this);
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(optionMenu);
    menuBar()->addMenu(helpMenu);




}

void MainWindow::createToolBar()
{
    //move to createtoolbar
    mainToolBar= addToolBar(tr("Tools"));
    mainToolBar->addAction(drawPenAct);
    mainToolBar->addAction(drawBrushAct);
    mainToolBar->addAction(penColorAct);
    mainToolBar->actions().takeLast()->setIcon(QIcon(":/paint-can.png"));
    mainToolBar->addAction(drawStampAct);
    mainToolBar->addAction(drawLineAct);
    mainToolBar->addAction(fillAreaAct);
    mainToolBar->addAction(textEditAct);
    mainToolBar->addAction(drawShapeAct);
    mainToolBar->addAction(drawPolyAct);

}

void MainWindow::createnewForm()
{

    newDialog = new QDialog();
    newForm = new QFormLayout();

    newWidth= new QSpinBox();
    newHeight= new QSpinBox();

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

    newWidth->setMaximum(1200);
    newWidth->setValue(500);
    newHeight->setMaximum(1200);
    newHeight->setValue(500);

    newForm->addRow(tr("Width"),newWidth);
    newForm->addRow(tr("Height"),newHeight);
    newForm->addWidget(buttonBox);;


    connect(buttonBox,SIGNAL(accepted()),newDialog,SLOT(accept()));

    newDialog->setLayout(newForm);

}

bool MainWindow::maybeSave()
{
    if(painterArea->isModified()){
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this,tr("Painter"), tr("The image has been modified.\nDo you want to save your changes?"),QMessageBox::Save | QMessageBox::Discard| QMessageBox::Cancel);
    if(ret==QMessageBox::Save)
        return saveFile("png");
    else if(ret == QMessageBox::Cancel)
        return false;
    }
    return true;
}

bool MainWindow::saveFile(const QByteArray &fileFormat)
{
    QString initialPath = QDir::currentPath()+"/untitled."+fileFormat;

    QString fileName = QFileDialog::getSaveFileName(this,tr("Save As"),initialPath,tr("%1 Files (*.%2);;All Files (*)").arg(QString::fromLatin1(fileFormat.toUpper())).arg(QString::fromLatin1(fileFormat)));
    if(fileName.isEmpty())
        return false;

    return painterArea->saveImage(fileName,fileFormat.constData());
}


