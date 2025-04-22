#include "mainwindow.h"
#include "ui_mainwindow.h"

#include<QMessageBox>
#include<QPushButton>
#include<QFileDialog>
#include<QTextStream>
#include<QCloseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    is_Unsaved=true;
    curFilePath=tr("未命名的文件.txt");
    setWindowTitle(curFilePath);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::new_file(){
    if(maybe_saved()){
        is_Unsaved=true;
        curFilePath=tr("未命名的文件.txt");
        setWindowTitle(curFilePath);

        ui->textEdit->clear();
        ui->textEdit->setVisible(true);
    }
}

bool MainWindow::maybe_saved(){
    if(ui->textEdit->document()->isModified()){
        QMessageBox msgbox;
        msgbox.setWindowTitle(tr("警告"));
        msgbox.setIcon(QMessageBox::Warning);
        msgbox.setText(curFilePath+tr("尚未保存，是否保存？"));
        QPushButton *yesbtn=msgbox.addButton(tr("是(&Y)"),QMessageBox::YesRole);
        msgbox.addButton(tr("否(&N)"),QMessageBox::NoRole);
        QPushButton *rejectbtn=msgbox.addButton(tr("取消"),QMessageBox::RejectRole);
        msgbox.exec();

        if(msgbox.clickedButton()==yesbtn){
            return save();
        }
        else if(msgbox.clickedButton()==rejectbtn){
            return false;
        }
        return true;
    }
}

bool MainWindow::save(){
    if(is_Unsaved){
        return save_as();
    }
    else{
        return save_file(curFilePath);
    }
}

bool MainWindow::save_as(){
    QString filename=QFileDialog::getSaveFileName(this,tr("另存为"),curFilePath);
    if(filename.isEmpty()){
        return false;
    }
    return save_file(filename);
}

bool MainWindow::save_file(const QString &filename){
    QFile file(filename);
    if(!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this,tr("多文档编辑器"),tr("无法写入文件%1:\n%2").arg(filename).arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);

    QApplication::setOverrideCursor(Qt::WaitCursor);
    out<<ui->textEdit->toPlainText();
    QApplication::restoreOverrideCursor();
    is_Unsaved=false;

    curFilePath=QFileInfo(filename).canonicalFilePath();
    setWindowTitle(curFilePath);
    return true;
}
bool MainWindow::load_file(const QString &filename){
    QFile file(filename);
    if(!file.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::warning(this,tr("多文档编辑器"),tr("无法读取文件%1:\n%2").arg(filename).arg(file.errorString()));
        return false;
    }
    QTextStream in(&file);

    QApplication::setOverrideCursor(Qt::WaitCursor);
    ui->textEdit->setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();

    curFilePath=QFileInfo(filename).canonicalFilePath();
    setWindowTitle(curFilePath);
    return true;
}
void MainWindow::close_event(QCloseEvent *event){
    if(maybe_saved()){
        event->accept();
    }else{
        event->ignore();
    }
}

void MainWindow::on_actionnew_triggered(){
    new_file();
}

void MainWindow::on_actionsave_triggered(){
    save();
}

void MainWindow::on_actionsaveas_triggered(){
    save_as();
}

void MainWindow::on_actionopen_triggered(){
    if(maybe_saved()){
        QString filename=QFileDialog::getOpenFileName(this);
        if(!filename.isEmpty()){
            load_file(filename);
            ui->textEdit->setVisible(true);
        }
    }
}

void MainWindow::on_actionclose_triggered(){
    if(maybe_saved()){
        ui->textEdit->setVisible(false);
    }
}

void MainWindow::on_actionexit_triggered(){
    on_actionclose_triggered();
    qApp->quit();
}

void MainWindow::on_actioncancel_triggered(){
    ui->textEdit->undo();
}

void MainWindow::on_actioncopy_triggered(){
    ui->textEdit->copy();
}

void MainWindow::on_actionpaste_triggered(){
    ui->textEdit->paste();
}











































