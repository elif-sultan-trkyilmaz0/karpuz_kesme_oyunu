#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QPoint>
#include <QVector>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QPropertyAnimation>
#include <QRandomGenerator>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QScreen>

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class Dialog : public QDialog {
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();

private:
    Ui::Dialog *ui;

    int sure;         // kalan süre
    int kesilen;      // kesilen karpuz sayısı
    int kacirilan;    // kaçırılan karpuz sayısı

    QVector<QPoint> konumlar;

    QTimer *sureTimer;
    QTimer *karpuzTimer;

    void karpuzEkle();
    void sureyiGuncelle();
    void oyunuBitir();
    void skoruKaydet();
    void maksimumSkoruGoster();
};

#endif // DIALOG_H
