#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void new_file();
    bool maybe_saved();
    bool save();
    bool save_as();
    bool save_file(const QString &filename);
    bool load_file(const QString &filename);
    void close_event(QCloseEvent *event);


private slots:
    void on_actionnew_triggered();
    void on_actionsave_triggered();
    void on_actionsaveas_triggered();
    void on_actionopen_triggered();
    void on_actionclose_triggered();
    void on_actioncancel_triggered();
    void on_actioncopy_triggered();
    void on_actionpaste_triggered();
    void on_actionexit_triggered();

private:
    Ui::MainWindow *ui;
    bool is_Unsaved;
    QString curFilePath;
};
#endif // MAINWINDOW_H
