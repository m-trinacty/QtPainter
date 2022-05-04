#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QList>
#include <QToolBar>
#include <QFormLayout>
#include <QSpinBox>
#include <QDialogButtonBox>

class Painter;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void open();
    void save();
    void newPainter();
    void penColor();
    void penWidth();
    void drawPenType();
    void drawLineType();
    void drawStampType();
    void drawBrushType();
    void drawShapeType();
    void drawPolygonType();

    void fillAreaType();
    void textEditType();
    void about();

private:
    void createActions();
    void createMenus();
    void createToolBar();
    void createnewForm();
    bool maybeSave();
    bool saveFile(const QByteArray &fileFormat);

    Painter *painterArea;


    QMenu *saveAsMenu;
    QMenu *fileMenu;
    QMenu *optionMenu;
    QMenu *helpMenu;

    QToolBar *mainToolBar;
    QFormLayout *newForm;

    QDialog *newDialog;
    QSpinBox *newWidth;
    QSpinBox *newHeight;
    QDialogButtonBox *buttonBox;

    QAction *newPainterAct;
    QAction *openAct;
    QList<QAction *> saveAsActs;
    QAction *exitAct;
    QAction *penColorAct;
    QAction *penWidthAct;
    QAction *printAct;
    QAction *clearScreenAct;
    QAction *aboutAct;
    QAction *aboutQtAct;

    QAction* drawPenAct;
    QAction* drawBrushAct;
    QAction* drawStampAct;
    QAction* drawLineAct;
    QAction* fillAreaAct;
    QAction* textEditAct;
    QAction* drawShapeAct;
    QAction* drawPolyAct;
};

#endif // MAINWINDOW_H
