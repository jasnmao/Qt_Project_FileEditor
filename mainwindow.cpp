#include "mainwindow.h"
#include "ui_mainwindow.h"

#include<QMessageBox>
#include<QPushButton>
#include<QFileDialog>
#include<QTextStream>
#include<QCloseEvent>
#include<QFrame>
MainWindow::MainWindow(QWidget *parent)//构造函数
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //是否未保存
    is_Unsaved=true;
    //当前文件路径
    curFilePath=tr("未命名的文件.txt");
    setWindowTitle(curFilePath);
    //查找对话框
    findDialog=new QDialog(this);
    findDialog->setWindowTitle(tr("查找"));
    //查找对话框中的文本输入框
    findLineEdit=new QLineEdit(this);
    //查找按钮
    QPushButton *btn=new QPushButton(tr("查找下一个"),this);
    //布局控件
    QVBoxLayout *layout=new QVBoxLayout(findDialog);
    layout->addWidget(findLineEdit);
    layout->addWidget(btn);
    //关联槽函数
    connect(btn,&QPushButton::clicked,this,&MainWindow::showFindText);
    //设置常规标签
    statusLabel=new QLabel;
    statusLabel->setMinimumSize(150,20);
    statusLabel->setFrameShape(QFrame::WinPanel);
    statusLabel->setFrameShadow(QFrame::Sunken);
    ui->statusbar->addWidget(statusLabel);
    statusLabel->setText(tr("欢迎访问Qt文本编辑器"));
    //设置永久标签并关联到网页
    perLabel=new QLabel;
    perLabel->setFrameStyle(QFrame::Box | QFrame::Sunken);
    perLabel->setText(tr("<a href=\"http://www.baidu.com\">百度</a>"));
    perLabel->setTextFormat(Qt::RichText);
    perLabel->setOpenExternalLinks(true);
    ui->statusbar->addWidget(perLabel);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//新建文件
void MainWindow::new_file(){
    if(maybe_saved()){
        is_Unsaved=true;
        curFilePath=tr("未命名的文件.txt");
        setWindowTitle(curFilePath);
        //显示文本编辑控件
        ui->textEdit->clear();
        ui->textEdit->setVisible(true);
    }
}
//保存状态函数
bool MainWindow::maybe_saved(){
    //如果检测到文本编辑框被改动
    if(ui->textEdit->document()->isModified()){
        QMessageBox msgbox;
        //设置警告窗口
        msgbox.setWindowTitle(tr("警告"));
        msgbox.setIcon(QMessageBox::Warning);
        msgbox.setText(curFilePath+tr("尚未保存，是否保存？"));
        //是，否和取消三个按钮
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
        else{
            ui->textEdit->document()->setModified(false);
            return true;
        }
    }
    return true;
}

bool MainWindow::save(){
    //如果未保存，首先要进行另存为操作
    if(is_Unsaved){
        return save_as();
    }
    //否则，直接保存即可
    else{
        return save_file(curFilePath);
    }
}

bool MainWindow::save_as(){
    //获取用户选择的文件夹地址
    QString filename=QFileDialog::getSaveFileName(this,tr("另存为"),curFilePath);
    if(filename.isEmpty()){
        return false;
    }
    return save_file(filename);
}
//保存文件函数
bool MainWindow::save_file(const QString &filename){
    //获取文件
    QFile file(filename);
    //文件需要以只写方式和文本模式打开
    if(!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this,tr("多文档编辑器"),tr("无法写入文件%1:\n%2").arg(filename).arg(file.errorString()));
        return false;
    }
    //文件输出流
    QTextStream out(&file);
    //设置鼠标为等待图标
    QApplication::setOverrideCursor(Qt::WaitCursor);
    //写入文件
    out<<ui->textEdit->toPlainText();
    QApplication::restoreOverrideCursor();
    //设置为已保存
    is_Unsaved=false;
    //获取目前文件的路径并显示
    curFilePath=QFileInfo(filename).canonicalFilePath();
    setWindowTitle(curFilePath);
    return true;
}
//加载文件
bool MainWindow::load_file(const QString &filename){
    QFile file(filename);
    //文件需要以只读和文本模式打开
    if(!file.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::warning(this,tr("多文档编辑器"),tr("无法读取文件%1:\n%2").arg(filename).arg(file.errorString()));
        return false;
    }
    //文件输入流
    QTextStream in(&file);

    QApplication::setOverrideCursor(Qt::WaitCursor);
    //一次读入所有内容
    ui->textEdit->setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();

    curFilePath=QFileInfo(filename).canonicalFilePath();
    setWindowTitle(curFilePath);
    return true;
}
//关闭界面
void MainWindow::close_event(QCloseEvent *event){
    if(maybe_saved()){//先询问是否保存
        event->accept();
    }else{
        event->ignore();
    }
}
//查找文本
void MainWindow::showFindText(){
    QString str=findLineEdit->text();
    //获取查找到的文本位置
    bool ret=ui->textEdit->find(str,QTextDocument::FindBackward);
    if(!ret){
        QMessageBox::warning(this,tr("查找"),tr("找不到%1").arg(str));
    }
}
//各个菜单行动的槽函数
void MainWindow::on_actionnew_triggered(){
    new_file();
}

void MainWindow::on_actionsave_triggered(){
    save();
}

void MainWindow::on_actionsaveas_triggered(){
    save_as();
}

void MainWindow::on_actionopen_triggered(){//打开新文件的函数
    if(maybe_saved()){//先询问保存目前的文档
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

void MainWindow::on_actionquit_triggered(){//退出函数
    on_actionclose_triggered();//先关闭文件
    qApp->quit();
}

void MainWindow::on_actioncancel_triggered(){//撤销函数
    ui->textEdit->undo();
}

void MainWindow::on_actioncopy_triggered(){
    ui->textEdit->copy();
}

void MainWindow::on_actionpaste_triggered(){
    ui->textEdit->paste();
}

void MainWindow::on_actionfind_triggered(){
    findDialog->show();
}









































