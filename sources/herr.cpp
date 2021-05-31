#include <qtextcodec.h>
#include "herr.h"
#include "ui_herr.h"
#include "HammingBack.h"

HErr::HErr(SubWindow *parent) :
    SubWindow(parent),
    ui(new Ui::HErr)
{
    ui->setupUi(this);
    ui->resTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    InitMembers();
    InitConnections();
}

HErr::~HErr()
{
    delete ui;
}

//��ʼ�����ݳ�Ա
void HErr::InitMembers()
{
    QTextCodec *codec = QTextCodec::codecForName("GBK");

    //����������ʾ�İ�ť��ʽ
    hoverStyle = "QPushButton:hover{background:#DCF0FC}";
    pressStyle = "QPushButton:pressed{background:#D0E0FC}";
    pbtnStyle0 = "QPushButton{background:#F0F0F0}";
    pbtnStyle1 = "QPushButton{background:#C0C0C0}";

    //int nRowCount = ui->resTable->rowCount();
    ui->resTable->setRowCount(4);
    ui->resTable->setItem(0, 0, new QTableWidgetItem(codec->toUnicode("����2������1�ĺ�������")));
    ui->resTable->setItem(1, 0, new QTableWidgetItem(codec->toUnicode("����2��������ȷ�����С��������")));
    ui->resTable->setItem(2, 0, new QTableWidgetItem(codec->toUnicode("����2��������")));
    ui->resTable->setItem(3, 0, new QTableWidgetItem(codec->toUnicode("������ȷ��������1�ĺ�������")));
    ui->resTable->setItem(3, 1, new QTableWidgetItem(codec->toUnicode("5")));
    for (int i = 0; i < 4; i++){
        ui->resTable->setItem(i, 1, new QTableWidgetItem(codec->toUnicode(" ")));
        ui->resTable->item(i,1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }
    updateResultTable();


    //�������ݶԱ����ݿ�
    int block_w = 30, block_h = 30;
    int delta_x = 10, delta_y = 10;
    for(int i = 0; i < BIT_MAX; i++){
        //�ɵ��
        DataIn1[i].setEnabled(false);
        DataIn2[i].setEnabled(true);
        //����λ��
        DataIn1[i].setGeometry(delta_x + i*(block_w +delta_x), delta_y, block_w, block_h);
        DataIn2[i].setGeometry(delta_x + i*(block_w +delta_x), delta_y, block_w, block_h);
        //������
        DataIn1[i].setParent(ui->wgtIn1);
        DataIn2[i].setParent(ui->wgtIn2);
    }

    //ͬ�������еķ��
    updateNumBit(8);
}

//�����ź����
void HErr::InitConnections()
{
    connect(ui->spinIn1, SIGNAL(valueChanged(int)), this, SLOT(updateSpinRelated(int)));
    connect(ui->genRandom, &QPushButton::clicked, [=](){this->newRandomData();updateResultTable();});

    //����2���Ե�bit���
    for(int i = 0; i < BIT_MAX; i++)
        connect(&(DataIn2[i]), &QPushButton::clicked, [=](){updateData2ByBit(i);updateResultTable();});

    //resultTable�����б������Ĳۺ����ж������Ķ�
}

void HErr::updateSpinRelated(int d)
{
    this->ui->label_check->setNum(d - 1 > 0 ? d - 1 : 0);
    this->ui->label_correct->setNum((d - 1) / 2);

    //resultTable�����еĲۺ����ж������Ķ�
    updateResultTable();
}

void HErr::newRandomData()
{
    HammingBack hb;
    QString data = QString::fromStdString(hb.randomBstring());
    this->ui->check_num1->setText(data);
    this->ui->check_num2->setText(data);

    //�����������ͼ����а�
    for(int i = 0; i < BIT_MAX; i++){
        setBlockStatus(&DataIn1[i], (data.toStdString().at(i) - '0'));
        setBlockStatus(&DataIn2[i], (data.toStdString().at(i) - '0'));
        //DataIn1[i].setText(data.at(i));
        //DataIn2[i].setText(data.at(i));
    }
}

//�������ݿ�״̬
void HErr::setBlockStatus(QPushButton *pbtn, int status)
{
    QString style0 = pbtnStyle0 + hoverStyle + pressStyle;
    QString style1 = pbtnStyle1 + hoverStyle + pressStyle;
    if(status == 0){
        pbtn->setStyleSheet(style0);
        pbtn->setText("0");
    }
    else {
        pbtn->setStyleSheet(style1);
        pbtn->setText("1");
    }
}

void HErr::updateResultTable()
{
    int d = this->ui->spinIn1->value();

    QString num1 = this->ui->check_num1->text();
    QString num2 = this->ui->check_num2->text();
    //����2������1�ĺ�������
    HammingBack hb;

    int dFrom2To1 = hb.calHammingDistance(num1.toStdString(), num2.toStdString());
    int dFrom2ToOther = d > dFrom2To1 ? d - dFrom2To1 : 0;

    QTextCodec *codec = QTextCodec::codecForName("GBK");
    QString moreNear = dFrom2To1 < dFrom2ToOther ? codec->toUnicode("����1") : codec->toUnicode("������ȷ��");
    if (dFrom2To1 == dFrom2ToOther)
        moreNear = codec->toUnicode("��������1��������ȷ����ͬ");

    //ά�����
    ui->resTable->item(0, 1)->setText(QString::number(dFrom2To1));
    ui->resTable->item(1, 1)->setText(QString::number(dFrom2ToOther));
    ui->resTable->item(2, 1)->setText(moreNear);
    ui->resTable->item(3, 1)->setText(QString::number(d));
}

void HErr::updateData2ByBit(int i)
{
    //������2��״̬��Ϊ��ǰ״̬ȡ��
    setBlockStatus(&this->DataIn2[i], !(this->DataIn2[i].text().toInt()));

    QString data2 = this->ui->check_num2->text();
    std::string std_data2 = data2.toStdString();
    //0��1��1��0
    std_data2.at(i) = std_data2.at(i) + (std_data2.at(i) - '0' ? -1 : 1);

    this->ui->check_num2->setText(QString::fromStdString(std_data2));
}

//���������������
void HErr::updateNumBit(int val)
{
    QString style0 = pbtnStyle0 + hoverStyle + pressStyle;
    QString style1 = pbtnStyle1 + hoverStyle + pressStyle;

    //��������������ݿ�
    int deftVal = 0;
    QString deftValStr = QString::number(deftVal);
    for(int i = 0; i < val; i++){
        //Ĭ��ֵ
        DataIn1[i].setText(deftValStr);
        DataIn2[i].setText(deftValStr);
        //Ĭ����ʽ
        DataIn1[i].setStyleSheet(style0);
        DataIn2[i].setStyleSheet(style0);
        //�ɼ�
        DataIn1[i].setVisible(true);
        DataIn2[i].setVisible(true);
    }
    for(int i = val; i < BIT_MAX; i++){
        //���ɼ�
        DataIn1[i].setVisible(false);
        DataIn2[i].setVisible(false);
    }
}
