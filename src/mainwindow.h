#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "obswidget.h"
#include "capturedevicepicker.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void fitSizeToContent();

    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private slots:
    void sidebarToggled(bool currentlyShown);

private:
    Ui::MainWindow *ui;

    ObsWidget *_obsWidget = nullptr;
    CaptureDevicePicker *_capturePicker = nullptr;
};
#endif // MAINWINDOW_H
