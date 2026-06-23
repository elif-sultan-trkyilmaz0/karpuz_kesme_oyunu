#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::Dialog), sure(30), kesilen(0), kacirilan(0) {
    ui->setupUi(this);
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    this->setGeometry(screenGeometry); // pencereyi ekran boyutuna getirmek için kullandım
    ui->oyun_alani->setGeometry(0, 100, width(), height()-110);

    setWindowTitle("Karpuz Oyunu");

    ui->label_sure->setText("Süre: " + QString::number(sure));
    ui->label_kesilen_karpuz_sayisi->setText("Kesilen Karpuz Sayısı: 0");
    ui->label_kacirilan_karpuz_sayisi->setText("Kaçırılan Karpuz Sayısı: 0");

    //konum ayarı
    QFile file("konumlar.txt");
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            int x, y;
            in >> x >> y;
            konumlar.append(QPoint(x, y));
        }
    } else {
        qDebug() << "konumlar.txt AÇILAMADI!";
    }
    //sure ayarlama
    sureTimer = new QTimer(this);
    connect(sureTimer, &QTimer::timeout, this, &Dialog::sureyiGuncelle);
    sureTimer->start(1000);

    // Karpuz ekleme zamanlayıcısı
    karpuzTimer = new QTimer(this);
    connect(karpuzTimer, &QTimer::timeout, this, &Dialog::karpuzEkle);
    karpuzTimer->start(800);
}

Dialog::~Dialog() {
    delete ui;
}

void Dialog::karpuzEkle() {
    if (konumlar.isEmpty()) return;

    int index = QRandomGenerator::global()->bounded(konumlar.size());
    QPoint pos = konumlar[index];

    QPushButton *buton = new QPushButton(ui->oyun_alani);
    buton->setStyleSheet("border: none; padding: 0px; background: transparent;");
    buton->setIcon(QIcon(":/new/resimler/resimler/karpuz.png"));
    buton->setIconSize(QSize(80, 80));
    buton->setGeometry(pos.x(), pos.y(), 80, 80);
    buton->setFlat(true);
    buton->setProperty("kesildi", false);
    buton->show();

    QPropertyAnimation *anim = new QPropertyAnimation(buton, "geometry");
    anim->setDuration(6000);
    anim->setStartValue(QRect(pos.x(), pos.y(), 80, 80));
    anim->setEndValue(QRect(pos.x(), ui->oyun_alani->height(), 80, 80));

    // Karpuz kesilince kesilmiş karpuz resmi gelicek, karpuz oldugu yerde  durucak
    connect(buton, &QPushButton::clicked, this, [=]() {
        if (!buton->property("kesildi").toBool()) {
            buton->setIcon(QIcon(":/new/resimler/resimler/kesilmiskarpuz.png"));
            buton->setProperty("kesildi", true);
            kesilen++;
            ui->label_kesilen_karpuz_sayisi->setText("Kesilen Karpuz Sayısı: " + QString::number(kesilen));
            anim->stop();
        }
    });

    // Animasyonu başlatma
    anim->start(QAbstractAnimation::DeleteWhenStopped);

    connect(anim, &QPropertyAnimation::finished, this, [=]() {
        if (!buton->property("kesildi").toBool()) {
            kacirilan++;
            ui->label_kacirilan_karpuz_sayisi->setText("Kaçırılan Karpuz Sayısı: " + QString::number(kacirilan));
            buton->deleteLater();
        } else {

        }
    });
}

void Dialog::sureyiGuncelle() {
    sure--;
    ui->label_sure->setText("Süre: " + QString::number(sure));
    if (sure <= 0) {
        karpuzTimer->stop();
        sureTimer->stop();
        oyunuBitir();
    }
}

void Dialog::oyunuBitir() {
    skoruKaydet();
    maksimumSkoruGoster();
    close();
}

void Dialog::skoruKaydet() {
    QFile file("skorlar.txt");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << kesilen << "\n";
    }
}

void Dialog::maksimumSkoruGoster() {
    QFile file("skorlar.txt");
    int maksimum = 0;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            int s;
            in >> s;
            if (s > maksimum) maksimum = s;
        }
    }
    QString mesaj;

    if (kesilen >= maksimum) {
        mesaj = "Oyun Bitti! Tebrikler! Maksimum skoru geçtiniz!";
    } else {
        mesaj = "Oyun Bitti! Maksimum Skoru Geçemediniz!";
    }

    QMessageBox::information(this, "Bilgi",
                             mesaj + "\nKesilen Karpuz Sayısı: " + QString::number(kesilen) +
                                 "\nKaçırılan Karpuz Sayısı: " + QString::number(kacirilan) +
                                 "\nMaximum Skor: " + QString::number(maksimum));
}
