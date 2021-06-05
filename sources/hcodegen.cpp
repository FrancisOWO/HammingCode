#include "hcodegen.h"
#include "ui_hcodegen.h"
#include <QString>
#include <QPoint>
#include <QSpinBox>
#include <QValidator>
#include <QRegExp>
#include <QSequentialAnimationGroup>
#include <QStringList>
#include <QDebug>

//����ת��
static QString cvtStr2LocalQStr(const char *str)
{
    return QString::fromLocal8Bit(str);
}

//��Ϣ���±�->�������±�
int HCodeGen::cvtDno2Hno(int index)
{
    if(index < 0 || index >= DATA_MAX)
        return -1;
    return DataLink[index];
}

//У�����±�->�������±�
int HCodeGen::cvtPno2Hno(int index)
{
    if(index < 0 || index >= PARITY_MAX)
        return -1;
    return (1 << index) - 1;
}

//����У����Ĳ������±�->�������±�
int HCodeGen::cvtPrHno2Hno(int pno, int index)
{
    if(pno < 0 || pno >= parityBits){
        //qDebug() << "pno:"<< pno;
        return -1;
    }
    if(index < 0 || index >= PrHnoLen[pno]){
        //qDebug() << "index:"<< index;
        return -1;
    }
    return PrHnos[pno][index+1] - 1;
}

//�����ݿ��ȡ���ִ�
QString HCodeGen::getValStr(QPushButton *pbtn, int n)
{
    QString valStr;
    for(int i = 0; i < n; i++)
        valStr = valStr + pbtn[i].text();
    return valStr;
}

HCodeGen::HCodeGen(SubWindow *parent) :
    SubWindow(parent),
    ui(new Ui::HCodeGen)
{
    ui->setupUi(this);

    InitMembers();
    InitConnections();
}

HCodeGen::~HCodeGen()
{
    delete ui;
}

//��ʼ�����ݳ�Ա
void HCodeGen::InitMembers()
{
    //��ʽ��ʼ��
    InitStyles();
    QString labStyle = "background:#F4F4F4";
    //��������״̬
    stepStatus = INIT_STATUS;
    speedLevel = 1;      //����ԭ�ٲ���
    checkStatus = INIT_STATUS;
    ui->pbtnFillCodeIn->setEnabled(false);
    //������H��Ӧ����ϢλD/У��λP
    for(int i = 0; i < HAMM_MAX; i++)
        HammLink[i] = 0;        //����
    for(int base=1, i=1; base < HAMM_MAX; base<<=1, i++)
        HammLink[base-1] = -i;  //У��λPx�Ǹ���
    for(int i=2, cnt=1; i < HAMM_MAX; i++){
        if(HammLink[i] != 0)
            continue;
        HammLink[i] = cnt;      //��ϢλDx������
        DataLink[cnt-1] = i;
        cnt++;
    }
    //for(int i = 0; i < HAMM_MAX; i++)
    //  qDebug() << "H"+QString::number(i+1) << "=" << HammLink[i];

    //���ݿ�/��ǩ��ʼ��
    int blk_w = 30, blk_h = 30, delta_w = 10;
    int blk_x, blk_y = 10, ofs_x = 10;
    int lab_h = 16, lab_y = 2;
    QString valStr0 = "0", valStrUnknown = "?";
    //��Ϣ��
    blk_x = ofs_x;
    QString style0 = pbtnStyle0 + hoverStyle + pressStyle;
    for(int i = 0; i < DATA_MAX; i++){
        //���ݿ�
        setPbtnProp(&(DataBlk[i]), ui->wgtDataBlk, style0, valStr0);
        /*
        DataBlk[i].setParent(ui->wgtDataBlk);
        DataBlk[i].setStyleSheet(style0);
        DataBlk[i].setText(valStr0);*/
        DataBlk[i].setGeometry(blk_x, blk_y, blk_w, blk_h);
        //��ǩ
        setLabProp(&(DataLab[i]), ui->wgtDataLab, dataStyle,
                   Qt::AlignHCenter|Qt::AlignVCenter, "D" + QString::number(i+1));
        /*
        DataLab[i].setParent(ui->wgtDataLab);
        DataLab[i].setStyleSheet(dataStyle);
        DataLab[i].setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        DataLab[i].setText("D" + QString::number(i+1));*/
        DataLab[i].setGeometry(blk_x, lab_y, blk_w, lab_h);
        //��������
        blk_x += blk_w + delta_w;
    }
    //������
    blk_x = ofs_x;
    QString hammStr, hammStyle;
    for(int i = 0; i < HAMM_MAX; i++){
        //���ݿ�
        setPbtnProp(&(HammBlk[i]), ui->wgtHammBlk, unknownStyle, valStrUnknown);
        HammBlk[i].setGeometry(blk_x, blk_y, blk_w, blk_h);
        //��ǩ
        setLabProp(&(HammLab[i]), ui->wgtHammLab, labStyle,
                   Qt::AlignHCenter|Qt::AlignVCenter, "H" + QString::number(i+1));
        HammLab[i].setGeometry(blk_x, lab_y, blk_w, lab_h);
        //�������Ӧ����Ϣλ/У��λ��ǩ
        if(HammLink[i] > 0){     //��Ϣλ
            hammStr = "D" + QString::number(HammLink[i]);
            hammStyle = dataStyle;
        }
        else {    //У��λ
            hammStr = "P" + QString::number(-HammLink[i]);
            hammStyle = parityStyle;
        }
        setLabProp(&(HammLinkLab[i]), ui->wgtHammLink, hammStyle,
                   Qt::AlignHCenter|Qt::AlignVCenter, hammStr);
        HammLinkLab[i].setGeometry(blk_x, lab_y, blk_w, lab_h);
        //��У������
        setPbtnProp(&(CodeBlk[CODE_IN][i]), ui->wgtCodeInBlk, style0, valStr0);
        CodeBlk[CODE_IN][i].setGeometry(blk_x, blk_y, blk_w, blk_h);
        //��ǩ
        setLabProp(&(CodeLab[CODE_IN][i]), ui->wgtCodeInLab, labStyle,
                   Qt::AlignHCenter|Qt::AlignVCenter, "H" + QString::number(i+1));
        CodeLab[CODE_IN][i].setGeometry(blk_x, lab_y, blk_w, lab_h);

        setLabProp(&(HammLinkLab2[i]), ui->wgtCodeInLink, hammStyle,
                   Qt::AlignHCenter|Qt::AlignVCenter,hammStr);
        HammLinkLab2[i].setGeometry(blk_x, lab_y, blk_w, lab_h);
        //���������
        setPbtnProp(&(CodeBlk[CODE_OUT][i]), ui->wgtCodeOutBlk, unknownStyle, valStrUnknown);
        CodeBlk[CODE_OUT][i].setGeometry(blk_x, blk_y, blk_w, blk_h);
        //��ǩ
        setLabProp(&(CodeLab[CODE_OUT][i]), ui->wgtCodeOutLab, labStyle,
                   Qt::AlignHCenter|Qt::AlignVCenter, "H" + QString::number(i+1));
        CodeLab[CODE_OUT][i].setGeometry(blk_x, lab_y, blk_w, lab_h);

        //��������
        blk_x += blk_w + delta_w;
    }
    //У����
    blk_x = ofs_x;
    for(int i = 0; i < PARITY_MAX; i++){
        //���ݿ�
        setPbtnProp(&(ParityBlk[i]), ui->wgtPrBlk, unknownStyle, valStrUnknown);
        /*
        ParityBlk[i].setParent(ui->wgtPrBlk);
        ParityBlk[i].setStyleSheet(unknownStyle);
        ParityBlk[i].setText(valStrUnknown);*/
        ParityBlk[i].setGeometry(blk_x, blk_y, blk_w, blk_h);
        //��ǩ
        setLabProp(&(ParityLab[i]), ui->wgtPrLab, parityStyle,
                   Qt::AlignHCenter|Qt::AlignVCenter,"P" + QString::number(i+1));
        /*
        ParityLab[i].setParent(ui->wgtPrLab);
        ParityLab[i].setStyleSheet(parityStyle);
        ParityLab[i].setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ParityLab[i].setText("P" + QString::number(i+1));*/
        ParityLab[i].setGeometry(blk_x, lab_y, blk_w, lab_h);
        //��������
        blk_x += blk_w + delta_w;
    }
    //У��λ
    //  //��ͷ
    PrRowHead[0] = ui->labP1; PrRowHead[1] = ui->labP2;
    PrRowHead[2] = ui->labP3; PrRowHead[3] = ui->labP4;
    // //������
    wgtPrBlk[0] = ui->wgtP1Blk; wgtPrBlk[1] = ui->wgtP2Blk;
    wgtPrBlk[2] = ui->wgtP3Blk; wgtPrBlk[3] = ui->wgtP4Blk;
    // //��ǩ��
    wgtPrLab[0] = ui->wgtP1Lab; wgtPrLab[1] = ui->wgtP2Lab;
    wgtPrLab[2] = ui->wgtP3Lab; wgtPrLab[3] = ui->wgtP4Lab;
    // //ÿ����������ݿ����ǩ
    delta_w = 20;
    HammingBack converter;
    // // //�������ͽ��
    for(int i = 0; i < PARITY_MAX; i++){
        //����У�����Ӧ��������Ϣλ
        int dlen = converter.calChecHammlen(DATA_MAX, i+1);
        converter.calCheckHnoList(DATA_MAX, i+1, PrHnos[i], dlen);
        blk_x = ofs_x;
        for(int j = 0; j < OPRD_MAX+2; j++){
            //���ݿ�
            setPbtnProp(&(PrRowBlks[i][j]), wgtPrBlk[i], unknownStyle, valStrUnknown);
            /*
            PrRowBlks[i][j].setParent(wgtPrBlk[i]);
            PrRowBlks[i][j].setStyleSheet(unknownStyle);
            PrRowBlks[i][j].setText(valStrUnknown);*/
            PrRowBlks[i][j].setGeometry(blk_x, blk_y, blk_w, blk_h);
            //��ǩ
            setLabProp(&(PrRowLabs[i][j]), wgtPrLab[i], labStyle,
                       Qt::AlignHCenter|Qt::AlignVCenter,"H?");
            /*
            PrRowLabs[i][j].setParent(wgtPrLab[i]);
            PrRowLabs[i][j].setStyleSheet(labStyle);
            PrRowLabs[i][j].setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            PrRowLabs[i][j].setText("H?");*/
            PrRowLabs[i][j].setGeometry(blk_x, lab_y, blk_w, lab_h);
            //��������
            blk_x += blk_w + delta_w;
        }
    }
    // // //������
    QString labStyle0 = "border:0px";
    for(int i = 0; i < PARITY_MAX; i++){
        blk_x = ofs_x + blk_w;
        for(int j = 0; j < OPTR_MAX+1; j++){
            //���ݿ�
            PrRowOptrBlks[i][j].setParent(wgtPrBlk[i]);
            PrRowOptrBlks[i][j].setStyleSheet(labStyle0);
            PrRowOptrBlks[i][j].setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            PrRowOptrBlks[i][j].setGeometry(blk_x, blk_y, delta_w, blk_h);
            //��ǩ
            PrRowOptrLabs[i][j].setParent(wgtPrLab[i]);
            PrRowOptrLabs[i][j].setStyleSheet(labStyle0);
            PrRowOptrLabs[i][j].setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            PrRowOptrLabs[i][j].setGeometry(blk_x, lab_y, delta_w, lab_h);
            //��������
            blk_x += blk_w + delta_w;
            PrRowOptrBlks[i][j].setText("+");
            PrRowOptrLabs[i][j].setText("+");
        }
    }
    //���ÿɼ���
    int data_bits = ui->spinDataBit->value();
    if(data_bits < 3 || data_bits > 5)
        data_bits = 4;
    setBlkVis(data_bits);

}

//�����ź����
void HCodeGen::InitConnections()
{
    //������Ϣ��λ�����ı����ݿ�״̬
    connect(ui->spinDataBit, SIGNAL(valueChanged(int)), this, SLOT(setBlkVis(int)));
    //������Ϣ�룬������Ϣ�����ݿ��ֵ
    connect(ui->lnDataCode, SIGNAL(textChanged(const QString &)), this, SLOT(updateDataBlk(const QString &)));
    for(int i = 0; i < DATA_MAX; i++)       //�����ť����ת���ݿ��ֵ
        connect(&(DataBlk[i]), &QPushButton::clicked, [=](){flipDataBlk(DataBlk, i);});
    for(int i = 0; i < HAMM_MAX; i++)       //�����ť����ת���ݿ��ֵ
        connect(&(CodeBlk[CODE_IN][i]), &QPushButton::clicked, [=](){flipDataBlk(CodeBlk[CODE_IN], i);});

    //�����ٶ�
    connect(ui->pbtnSpeed, SIGNAL(clicked()), this, SLOT(changeSpeed()));
    //��������
    connect(ui->pbtnHamStep, SIGNAL(clicked()), this, SLOT(updateStepStatus()));    //��һ������
    connect(ui->pbtnHamRestart, &QPushButton::clicked, [=](){       //״̬��ԭ
        setBlkVis(dataBits);
        setStepInit();
        setCheckBtnsEnabled(true);
    });
    connect(this, SIGNAL(moveFinished()), this, SLOT(setStepFinishStatus()));   //���ö�������״̬
    //һ�������ɺ�����
    connect(ui->pbtnHamFinal, SIGNAL(clicked()), this, SLOT(genAllBlk()));
    //�ú���������У������
    connect(ui->pbtnFillCodeIn, &QPushButton::clicked, [=](){
        QString valStr = ui->lnHammCode->text();
        ui->lnCodeIn->setText(valStr);
        int hamm_bits = dataBits + parityBits;
        for(int i = 0; i < hamm_bits; i++){
            int status = (valStr[i] == '1');
            setBlkStatus(&(CodeBlk[CODE_IN][i]), status);
        }
    });
    //������������
    connect(ui->pbtnCheckStep, SIGNAL(clicked()), this, SLOT(updateCheckStep()));
    //״̬��ԭ
    connect(ui->pbtnCheckRestart, &QPushButton::clicked, [=](){
        checkStatus = INIT_STATUS;
        stepStatusStr = cvtStr2LocalQStr("������\n(��������)");
        setCheckStepFinish();
        setCheckStatusInit();
        setGenBtnsEnabled(true);
        int hamm_bits = dataBits + parityBits;
        ui->spinDataBit->setEnabled(true);
        ui->lnDataCode->setReadOnly(false);
        for(int i = 0; i < dataBits; i++){
            DataBlk[i].setEnabled(true);
        }
        for(int i = 0; i < hamm_bits; i++){
            CodeBlk[CODE_IN][i].setEnabled(true);
        }
    });
    //һ���Եó����
    connect(ui->pbtnCheckFinal, &QPushButton::clicked, [=](){
        genAllCheck();
    });
}

//��ʼ����ʽ��
void HCodeGen::InitStyles()
{
    pbtnStyle0 = "QPushButton{background:#F0F0F0}";     //ֵΪ0
    pbtnStyle1 = "QPushButton{background:#C0C0C0}";     //ֵΪ1
    unknownStyle = "background:#E8F0FF";    //ֵδ֪

    hoverStyle = "QPushButton:hover{background:#DCF0FC}";       //��ͣ
    pressStyle = "QPushButton:pressed{background:#D0E0FC}";     //��ѹ

    dataStyle = "background:#FFCCCC;border:1px solid grey";     //��Ϣ��
    parityStyle = "background:#CCCCFF;border:1px solid grey";   //У���
    moveStyle = "background:#FFCCCC;border:1px solid grey";     //�ƶ�
}

//���ÿɼ���
void HCodeGen::setBlkVis(int data_bits)
{
    //���������ָ���ʼ״̬
    setStepInit();
    //������Ϣ�������ֻ����ָ�����ȵ�0/1��
    dataBits = data_bits;
    QString rgx0 = QString("^[0-1]{%1}$").arg(data_bits);
    QValidator *vdt = new QRegExpValidator(QRegExp(rgx0));
    ui->lnDataCode->setValidator(vdt);
    //��Ϣ��
    QString valStr, valStr0 = "0", valUnknown = "?";
    for(int i = 0; i < data_bits; i++){
        valStr = valStr + valStr0;
        DataBlk[i].setVisible(true);
        DataLab[i].setVisible(true);
    }
    for(int i = data_bits; i < DATA_MAX; i++){
        DataBlk[i].setVisible(false);
        DataLab[i].setVisible(false);
    }
    ui->lnDataCode->setText(valStr);

    //У����
    HammingBack converter;
    parityBits = converter.calCheckLen(data_bits);     //У����λ��
    valStr = "";
    for(int i = 0; i < parityBits; i++){
        valStr = valStr + valUnknown;
        ParityBlk[i].setVisible(true);
        ParityLab[i].setVisible(true);
        PrRowHead[i]->setVisible(true);
        wgtPrLab[i]->setVisible(true);
        wgtPrBlk[i]->setVisible(true);
    }
    for(int i = parityBits; i < PARITY_MAX; i++){
        ParityBlk[i].setVisible(false);
        ParityLab[i].setVisible(false);
        PrRowHead[i]->setVisible(false);
        wgtPrLab[i]->setVisible(false);
        wgtPrBlk[i]->setVisible(false);
    }
    //����ı���
    ui->lnPrBit->setText(QString::number(parityBits));
    ui->lnPrCode->setText(valStr);

    //������
    int hamm_bits = data_bits + parityBits;
    ui->lnCodeInBit->setText(QString::number(hamm_bits));
    ui->lnCodeOutBit->setText(QString::number(hamm_bits));
    valStr = "";
    QString codeStr;
    for(int i = 0; i < hamm_bits; i++){
        valStr = valStr + valUnknown;
        codeStr = codeStr + valStr0;
        HammBlk[i].setVisible(true);
        HammLab[i].setVisible(true);
        HammLinkLab[i].setVisible(true);

        CodeBlk[CODE_IN][i].setVisible(true);
        setBlkStatus(&(CodeBlk[CODE_IN][i]), 0);
        CodeLab[CODE_IN][i].setVisible(true);
        HammLinkLab2[i].setVisible(true);
        CodeBlk[CODE_OUT][i].setVisible(true);
        CodeLab[CODE_OUT][i].setVisible(true);
    }
    for(int i = hamm_bits; i < HAMM_MAX; i++){
        HammBlk[i].setVisible(false);
        HammLab[i].setVisible(false);
        HammLinkLab[i].setVisible(false);

        CodeBlk[CODE_IN][i].setVisible(false);
        CodeLab[CODE_IN][i].setVisible(false);
        HammLinkLab2[i].setVisible(false);
        CodeBlk[CODE_OUT][i].setVisible(false);
        CodeBlk[CODE_OUT][i].setText("?");
        CodeBlk[CODE_OUT][i].setStyleSheet(unknownStyle);
        CodeLab[CODE_OUT][i].setVisible(false);
    }
    //����ı���
    ui->lnHammBit->setText(QString::number(hamm_bits));     //������λ��
    ui->lnHammCode->setText(valStr);
    ui->lnCodeIn->setText(codeStr);
    ui->lnCodeOut->setText(valStr);

    //У��λ
    updatePrBitLab();       //����У��λ�����Ʊ�ǩ
    for(int i = 0; i < parityBits; i++){
        //�������У��λ����ĺ�����λ��
        int hlen = converter.calChecHammlen(data_bits, i+1) - 1;
        PrHnoLen[i] = hlen;
        //��ʾ��������
        for(int j = 0; j < hlen; j++){
            PrRowBlks[i][j].setVisible(true);   //���ݿ�
            PrRowLabs[i][j].setVisible(true);   //��ǩ
            PrRowLabs[i][j].setText("H"+QString::number(PrHnos[i][j+1]));
            PrRowLabs[i][j].setStyleSheet(dataStyle);
            //���ò�����
            PrRowOptrBlks[i][j].setVisible(true);
            PrRowOptrLabs[i][j].setVisible(true);
            PrRowOptrBlks[i][j].setText("+");
            PrRowOptrLabs[i][j].setText("+");
        }
        PrRowOptrBlks[i][hlen-1].setText("=");
        PrRowOptrLabs[i][hlen-1].setText("=");
        //��ʾ�����
        PrRowBlks[i][hlen].setVisible(true);
        PrRowLabs[i][hlen].setVisible(true);
        PrRowLabs[i][hlen].setText("H"+QString::number(PrHnos[i][0]));
        PrRowLabs[i][hlen].setStyleSheet(parityStyle);
        //���ض����
        for(int j = hlen; j < OPTR_MAX+1; j++){
            PrRowOptrBlks[i][j].setVisible(false);  //������(���ݿ���)
            PrRowOptrLabs[i][j].setVisible(false);  //������(��ǩ��)
        }
        for(int j = hlen+1; j < OPRD_MAX+2; j++){
            PrRowBlks[i][j].setVisible(false);   //���ݿ�
            PrRowLabs[i][j].setVisible(false);   //��ǩ
        }
    }
    //���ºϷ������
    updateLegalCodeTable();
}

//���ð�ť����
void HCodeGen::setPbtnProp(QPushButton *pbtn, QWidget *parent, QString style, QString text)
{
    pbtn->setParent(parent);
    pbtn->setStyleSheet(style);
    pbtn->setText(text);
}

//���ñ�ǩ����
void HCodeGen::setLabProp(QLabel *lab, QWidget *parent, QString style, Qt::Alignment align, QString text)
{
    lab->setParent(parent);
    lab->setStyleSheet(style);
    lab->setAlignment(align);
    lab->setText(text);
}

//���Ϸ������
void HCodeGen::updateLegalCodeTable()
{
    HammingBack converter;
    int n = (1 << dataBits);
    int hamm_bits = dataBits + parityBits;
    //������
    ui->tblLegalCode->setRowCount(n);
    ui->tblLegalCode->setColumnCount(hamm_bits);
    //�п�
    int width = 51;
    for(int i = 0; i < hamm_bits; i++)
        ui->tblLegalCode->setColumnWidth(i, width);
    //�б���
    QStringList Hheader;
    for(int i = 0; i < hamm_bits; i++)
        Hheader << "H" + QString::number(i+1);
    ui->tblLegalCode->setHorizontalHeaderLabels(Hheader);
    QColor colorD = "#FFCCCC", colorP = "#CCCCFF";
    for(int i = 0; i < hamm_bits; i++){
        if(HammLink[i] > 0)
            ui->tblLegalCode->horizontalHeaderItem(i)->setBackground(colorD);
        else
            ui->tblLegalCode->horizontalHeaderItem(i)->setBackground(colorP);
    }
    //�б���
    QStringList Vheader;
    for(int i = 0; i < n; i++)
            Vheader << QString::number(i);
    ui->tblLegalCode->setVerticalHeaderLabels(Vheader);
    ui->tblLegalCode->verticalHeader()->setMinimumWidth(30);
    //�������
    for(int i = 0; i < n; i++){
        std::string dataStr = converter.int2bstring(i, dataBits);
        HammingResult result = converter.calHammingResult(dataStr);
        for(int j = 0; j < hamm_bits; j++){
            QString text = QString::number(result.fullRes[j]-'0');
            ui->tblLegalCode->setItem(i, j, new QTableWidgetItem(text));
            if(HammLink[j] > 0)
                ui->tblLegalCode->item(i,j)->setBackground(colorD);
            else
                ui->tblLegalCode->item(i,j)->setBackground(colorP);
        }
    }
}

//������Ϣ��򣬸������ݿ鲹ȫ����������λ��
void HCodeGen::updateDataCode()
{
    //�����ݿ��ȡ���ִ�
    QString valStr = getValStr(DataBlk, dataBits);
    //Ϊ��ֹ��Ϣ���仯�������ݿ�仯����ʱ�������
    disconnect(ui->lnDataCode, SIGNAL(textChanged(const QString &)), this, SLOT(updateDataBlk(const QString &)));   //�������
    ui->lnDataCode->setText(valStr);    //������Ϣ���
    connect(ui->lnDataCode, SIGNAL(textChanged(const QString &)), this, SLOT(updateDataBlk(const QString &)));      //�ָ�����
}

//����У��λ�����Ʊ�ǩ
void HCodeGen::updatePrBitLab()
{
    QString valStr;
    for(int i = 0; i < parityBits; i++)
        valStr = valStr + "x";
    QLabel *labs[PARITY_MAX] = {ui->labP1Bit, ui->labP2Bit, ui->labP3Bit, ui->labP4Bit};
    for(int i = 0; i < parityBits; i++){
        int k = parityBits - i - 1;
        valStr[k] = '1';
        labs[i]->setText(valStr);
        labs[i]->setVisible(true);
        valStr[k] = 'x';
    }
    for(int i = parityBits; i < PARITY_MAX; i++){
        labs[i]->setVisible(false);
    }

}

//����/���ð�ť�飨���ɺ����밴ť�飩
void HCodeGen::setGenBtnsEnabled(bool flag)
{
    ui->pbtnHamFinal->setEnabled(flag);     //һ�������ɺ�����
    ui->pbtnHamStep->setEnabled(flag);      //��������
    ui->pbtnHamRestart->setEnabled(flag);   //���¿�ʼ
}

//����/���ð�ť�飨������ť�飩
void HCodeGen::setCheckBtnsEnabled(bool flag)
{
    ui->pbtnCheckStep->setEnabled(flag);     //һ���Լ�����
    ui->pbtnCheckFinal->setEnabled(flag);    //��������
    ui->pbtnCheckRestart->setEnabled(flag);  //���¿�ʼ
}

//�ı䶯�������ٶ�
void HCodeGen::changeSpeed()
{
    if(speedLevel == 1){
        speedLevel = 2;
        ui->pbtnSpeed->setText(cvtStr2LocalQStr("ԭ�ٲ���"));
    }
    else {
        speedLevel = 1;
        ui->pbtnSpeed->setText(cvtStr2LocalQStr("���ٲ���(x2)"));
    }
}

//�������ݿ�״̬δ֪
void HCodeGen::setBlkUnknown()
{
    QString textUnKnown = "?";
    //������
    QString valStr;
    int fullBits = dataBits + parityBits;
    for(int i = 0; i < fullBits; i++){
        valStr = valStr + textUnKnown;
        HammBlk[i].setText(textUnKnown);
        HammBlk[i].setStyleSheet(unknownStyle);
    }
    ui->lnHammCode->setText(valStr);    //������ʾ��
    //У����
    valStr = "";
    for(int i = 0; i < parityBits; i++){
        valStr = valStr + textUnKnown;
        ParityBlk[i].setText(textUnKnown);
        ParityBlk[i].setStyleSheet(unknownStyle);
    }
    ui->lnPrCode->setText(valStr);      //������ʾ��
    //У��λ
    for(int i = 0; i < PARITY_MAX; i++){
        for(int j = 0; j < OPRD_MAX+1; j++){
            PrRowBlks[i][j].setText(textUnKnown);
            PrRowBlks[i][j].setStyleSheet(unknownStyle);
        }
    }

}

//���õ���������ʼ״̬
void HCodeGen::setStepInit()
{
    stepStatusStr = cvtStr2LocalQStr("����������\n(��������)");
    setStepFinishStatus();
    stepStatus = INIT_STATUS;
    ui->pbtnFillCodeIn->setEnabled(false);  //��ֹ�ú���������У������
    //��������
    ui->spinDataBit->setEnabled(true);      //���ֿ���޸�
    ui->lnDataCode->setReadOnly(false);     //�������޸�
    for(int i = 0; i < dataBits; i++)       //���ݿ�ɸı�
        DataBlk[i].setEnabled(true);
    int hamm_bits = dataBits + parityBits;
    for(int i = 0; i < hamm_bits; i++)      //���ݿ�ɸı�
        CodeBlk[CODE_IN][i].setEnabled(true);
    //�ָ���Ϣ�����ݿ�״̬
    QString valStr = getValStr(DataBlk, dataBits);
    updateDataBlk(valStr);
    //����δ֪���ݿ�
    setBlkUnknown();

}

//���µ�������״̬
void HCodeGen::updateStepStatus()
{
    setGenBtnsEnabled(false);  //���ð�ť
    ui->pbtnSpeed->setEnabled(false);
    stepStatus++;
    int hamm_bits = dataBits + parityBits;
    int lastParityStatus = hamm_bits;
    int finalStatus = lastParityStatus + parityBits;
    if(stepStatus == 0){
        setBlkVis(dataBits);
        stepStatusStr = cvtStr2LocalQStr("��������\n����D1");
        updateDataCode();   //������Ϣ���
        setBlkUnknown();    //����δ֪��
        setCheckBtnsEnabled(false);         //���ð�ť
        ui->pbtnFillCodeIn->setEnabled(false);
        //��������
        ui->spinDataBit->setEnabled(false); //���ֿ򲻿��޸�
        ui->lnDataCode->setReadOnly(true);  //����򲻿��޸�
        for(int i = 0; i < dataBits; i++)   //���ݿ鲻�ɸı�
            DataBlk[i].setEnabled(false);
        for(int i = 0; i < hamm_bits; i++)  //���ݿ鲻�ɸı�
            CodeBlk[CODE_IN][i].setEnabled(false);
        setStepFinishStatus();
        //���㺣����
        HammingBack converter;
        std::string valStr = ui->lnDataCode->text().toStdString();
        HammResult = converter.calHammingResult(valStr);
    }
    else if(stepStatus < dataBits){     //��Ϣ��������Ϣ��->������������Ϣ��
        stepStatusStr = cvtStr2LocalQStr("��������\n����D")+QString::number(stepStatus+1);
        moveDataBlk(stepStatus-1);
    }
    else if(stepStatus == dataBits){    //��Ϣ��������Ϣ��->������������Ϣ�飨���һ����
        stepStatusStr = cvtStr2LocalQStr("��������\n����P1");
        moveDataBlk(stepStatus-1);
    }
    else if(stepStatus < lastParityStatus){     //����У��λ
        int index = stepStatus-dataBits;
        stepStatusStr = cvtStr2LocalQStr("��������\n����P")+QString::number(index+1);
        genPrBlk(index-1);
    }
    else if(stepStatus == lastParityStatus){    //����У��λ�����һ����
        int index = stepStatus-dataBits;
        stepStatusStr = cvtStr2LocalQStr("��������\n����P1");
        genPrBlk(index-1);
    }
    else if(stepStatus < finalStatus){      //У��������У���->����������У���
        int index = stepStatus - lastParityStatus;
        stepStatusStr = cvtStr2LocalQStr("��������\n����P")+QString::number(index+1);
        moveParityrBlk(index-1);
    }
    else if(stepStatus == finalStatus){     //У��������У���->����������У��飨���һ����
        int index = stepStatus - lastParityStatus;
        stepStatusStr = cvtStr2LocalQStr("��������\n����");
        moveParityrBlk(index-1);
    }
    else if(stepStatus > 0){
        stepStatusStr = cvtStr2LocalQStr("��������\n����");
        setStepInit();      //�ָ���ʼ״̬
        setCheckBtnsEnabled(true);      //���ð�ť
        for(int i = 0; i < hamm_bits; i++)  //���ݿ�ɸı�
            CodeBlk[CODE_IN][i].setEnabled(true);
    }
}

//���õ�����������״̬
void HCodeGen::setStepFinishStatus()
{
    ui->pbtnHamStep->setText(stepStatusStr);    //���°�ť����
    setGenBtnsEnabled(true);            //���ð�ť
    ui->pbtnSpeed->setEnabled(true);
    //������һ���Ľ���
    int lastParityStatus = dataBits + parityBits;
    int finalStatus = lastParityStatus + parityBits;
    //qDebug() << "dataBits :" << dataBits;
    //qDebug() << "prBits :" << parityBits;
    //qDebug() << "status :" << stepStatus;
    if(stepStatus < 0)
        ;
    else if(stepStatus < dataBits){
        //�ı佫Ҫ�ƶ������ݿ�״̬
        DataBlk[stepStatus].setStyleSheet(dataStyle);
    }
    else if(stepStatus < lastParityStatus){
        ;
    }
    else if(stepStatus < finalStatus){
        int index = stepStatus - lastParityStatus;
        ParityBlk[index].setStyleSheet(parityStyle);
    }
    else if(stepStatus == finalStatus){
        //�����ú���������У������
        ui->pbtnFillCodeIn->setEnabled(true);        
    }

}

//����������ʼ��
void HCodeGen::setCheckStatusInit()
{    
    for(int i = 0; i < parityBits; i++){
        int hlen = PrHnoLen[i];
        //��ǩ
        for(int j = 0; j <= hlen; j++){
            if(j > 0)
                PrRowLabs[i][j].setStyleSheet(dataStyle);
            PrRowLabs[i][j].setText("H"+QString::number(PrHnos[i][j]));
            PrRowBlks[i][j].setText("?");
            PrRowBlks[i][j].setStyleSheet(unknownStyle);
        }
        PrRowLabs[i][0].setStyleSheet(parityStyle);
        //������
        PrRowOptrLabs[i][hlen-1].setText("+");
        PrRowOptrBlks[i][hlen-1].setText("+");
        PrRowOptrBlks[i][hlen].setText("=");
        PrRowOptrBlks[i][hlen].setVisible(true);
        //���
        PrRowBlks[i][hlen+1].setText("?");
        PrRowBlks[i][hlen+1].setStyleSheet(unknownStyle);
        PrRowBlks[i][hlen+1].setVisible(true);
    }
    int hamm_bits = dataBits + parityBits;
    QString valStr;
    for(int i = 0; i < hamm_bits; i++){
        int status = (CodeBlk[CODE_IN][i].text() == "1");
        setBlkStatus(&(CodeBlk[CODE_IN][i]), status);

        valStr = valStr + "?";
        CodeBlk[CODE_OUT][i].setText("?");
        CodeBlk[CODE_OUT][i].setStyleSheet(unknownStyle);
    }
    ui->lnCodeOut->setText(valStr);
}

//������������״̬
void HCodeGen::updateCheckStep()
{
    setCheckBtnsEnabled(false);         //���ð�ť
    ui->pbtnSpeed->setEnabled(false);
    ui->pbtnFillCodeIn->setEnabled(false);
    static int errbit = -1;
    int hamm_bits = dataBits + parityBits;
    QString okStyle = "background:#CCFFCC", errStyle = "background:#FFCCCC";
    checkStatus++;
    HammingBack converter;
    if(checkStatus == 0){
        stepStatusStr = cvtStr2LocalQStr("����У��\n��1λ");
        setGenBtnsEnabled(false);
        //���ð�ť/����
        ui->spinDataBit->setEnabled(false);
        ui->lnDataCode->setReadOnly(true);
        for(int i = 0; i < dataBits; i++){
            DataBlk[i].setEnabled(false);
        }
        for(int i = 0; i < hamm_bits; i++){
            CodeBlk[CODE_IN][i].setEnabled(false);
        }

        setCheckStatusInit();
        setCheckStepFinish();
    }
    else if(checkStatus < parityBits){
        stepStatusStr = cvtStr2LocalQStr("����У��\n��") +
                QString::number(checkStatus+1) +  cvtStr2LocalQStr("λ");
        genCheckBlk(checkStatus - 1);
    }
    else if(checkStatus == parityBits){
        stepStatusStr = cvtStr2LocalQStr("����У��\n���");
        genCheckBlk(checkStatus - 1);
    }
    else if(checkStatus == parityBits + 1){
        QString valStr;
        for(int i = 0; i < parityBits; i++){
            int hlen = PrHnoLen[i];
            valStr = PrRowBlks[i][hlen+1].text() + valStr;
        }
        errbit = converter.bstring2int(valStr.toStdString());
        if(errbit > 0){
            CodeBlk[CODE_IN][errbit-1].setStyleSheet(errStyle);
            stepStatusStr = cvtStr2LocalQStr("����У��\nH") +
                    QString::number(errbit) + cvtStr2LocalQStr("����\n����");
        }
        else {
            stepStatusStr = cvtStr2LocalQStr("����У��\n�޴�");
        }
        for(int i = 0; i < hamm_bits; i++){
            if(i == errbit - 1)
                continue;
            CodeBlk[CODE_IN][i].setStyleSheet(okStyle);
        }
        setCheckStepFinish();
    }
    else if(checkStatus == parityBits + 2){
        stepStatusStr = cvtStr2LocalQStr("����У��\n����");
        QString valStr = ui->lnCodeIn->text();
        for(int i = 0; i < hamm_bits; i++){
            QString text = CodeBlk[CODE_IN][i].text();
            int status = (text == "1");
            if(i == errbit - 1){
                status = !status;
                if(status){
                    valStr[i] = '1';
                    text = "1";
                }
                else {
                    valStr[i] = '0';
                    text = "0";
                }
            }
            status += 2;    //���ɵ��״̬
            CodeBlk[CODE_OUT][i].setStyleSheet(okStyle);
            CodeBlk[CODE_OUT][i].setText(text);
        }
        ui->lnCodeOut->setText(valStr);
        setCheckStepFinish();
    }
    else {
        checkStatus = INIT_STATUS;
        stepStatusStr = cvtStr2LocalQStr("������\n(��������)");
        setCheckStatusInit();
        setCheckStepFinish();
        setGenBtnsEnabled(true);
        ui->spinDataBit->setEnabled(true);
        ui->lnDataCode->setReadOnly(false);
        for(int i = 0; i < dataBits; i++){
            DataBlk[i].setEnabled(true);
        }
        for(int i = 0; i < hamm_bits; i++){
            CodeBlk[CODE_IN][i].setEnabled(true);
        }
    }
}

//������������
void HCodeGen::setCheckStepFinish()
{
    ui->pbtnCheckStep->setText(stepStatusStr);
    setCheckBtnsEnabled(true);         //���ð�ť
    ui->pbtnSpeed->setEnabled(true);
}

//һ��������У����
void HCodeGen::genAllCheck()
{
    //״̬��ԭ
    checkStatus = INIT_STATUS;
    stepStatusStr = cvtStr2LocalQStr("������\n(��������)");
    setCheckStepFinish();
    setCheckStatusInit();
    setGenBtnsEnabled(true);
    int hamm_bits = dataBits + parityBits;
    ui->spinDataBit->setEnabled(true);
    ui->lnDataCode->setReadOnly(false);
    for(int i = 0; i < dataBits; i++){
        DataBlk[i].setEnabled(true);
    }
    for(int i = 0; i < hamm_bits; i++){
        CodeBlk[CODE_IN][i].setEnabled(true);
    }
    //У��λ����Ĳ�����
    QString startStyle, endStyle;
    for(int i = 0; i < parityBits; i++){
        int n = PrHnoLen[i] + 1;
        for(int j = 0; j < n; j++){
            int hno = PrHnos[i][j] - 1;
            QString text = CodeBlk[CODE_IN][hno].text();
            if(text == "0"){
                startStyle = pbtnStyle0 + hoverStyle + pressStyle;
                endStyle = pbtnStyle0;
            }
            else {
                startStyle = pbtnStyle1 + hoverStyle + pressStyle;
                endStyle = pbtnStyle1;
            }
            //���ý�������ʽ
            PrRowBlks[i][j].setStyleSheet(endStyle);
            PrRowBlks[i][j].setText(text);
        }
        getCheckResult(i);
    }

    //******************����װ
    //���
    QString okStyle = "background:#CCFFCC", errStyle = "background:#FFCCCC";
    HammingBack converter;
    QString valStr;
    for(int i = 0; i < parityBits; i++){
        int hlen = PrHnoLen[i];
        valStr = PrRowBlks[i][hlen+1].text() + valStr;
    }
    int errbit = converter.bstring2int(valStr.toStdString());
    if(errbit > 0){
        CodeBlk[CODE_IN][errbit-1].setStyleSheet(errStyle);
        stepStatusStr = cvtStr2LocalQStr("����У��\n����");
    }
    else {
        stepStatusStr = cvtStr2LocalQStr("����У��\n�޴�");
    }
    for(int i = 0; i < hamm_bits; i++){
        if(i == errbit - 1)
            continue;
        CodeBlk[CODE_IN][i].setStyleSheet(okStyle);
    }
    //����
    valStr = ui->lnCodeIn->text();
    for(int i = 0; i < hamm_bits; i++){
        QString text = CodeBlk[CODE_IN][i].text();
        int status = (text == "1");
        if(i == errbit - 1){
            status = !status;
            if(status){
                valStr[i] = '1';
                text = "1";
            }
            else {
                valStr[i] = '0';
                text = "0";
            }
        }
        status += 2;    //���ɵ��״̬
        CodeBlk[CODE_OUT][i].setStyleSheet(okStyle);
        CodeBlk[CODE_OUT][i].setText(text);
    }
    ui->lnCodeOut->setText(valStr);
}

//һ�������ɺ�����
void HCodeGen::genAllBlk()
{
    //setBlkVis���dataֵ��ȫ0�������¸�ֵ
    QString text = ui->lnDataCode->text();
    setBlkVis(dataBits);
    ui->lnDataCode->setText(text);

    setStepInit();      //����������ԭ
    setCheckBtnsEnabled(true);  //���ð�ť

    HammingBack converter;
    //��ȡ��Ϣ�룬���㺣����
    ///���������ִ�λ�����ܲ��㣬Ӧͨ�����ݿ��ȡ��Ϣ��
    QString dataStr = getValStr(DataBlk, dataBits);
    updateDataCode();       //������Ϣ������򣬲���λ��
    HammResult = converter.calHammingResult(dataStr.toStdString());
    //������
    int fullBits = dataBits + parityBits;
    QString valStr = QString::fromStdString(HammResult.fullRes);
    ui->lnHammCode->setText(valStr);            //���ݿ�
    for(int i = 0; i < fullBits; i++){
        int status = HammResult.fullRes[i] - '0';
        setBlkStatus(&(HammBlk[i]), status);    //���ݿ�
    }
    //У����
    valStr = QString::fromStdString(HammResult.check);
    ui->lnPrCode->setText(valStr);              //���ݿ�
    for(int i = 0; i < parityBits; i++){
        int status = HammResult.check[i] - '0';
        setBlkStatus(&(ParityBlk[i]), status);  //���ݿ�
        int hlen = PrHnoLen[i];
        setBlkStatus(&(PrRowBlks[i][hlen]), status);
        for(int j = 0; j < hlen; j++){
            int hno = cvtPrHno2Hno(i, j);
            //qDebug() << hno;
            status = HammResult.fullRes[hno] - '0';
            setBlkStatus(&(PrRowBlks[i][j]), status);
        }
    }
    //�����ú���������У������
    ui->pbtnFillCodeIn->setEnabled(true);
}

//�������ݿ�״̬
void HCodeGen::setBlkStatus(QPushButton *pbtn, int status)
{
    //��ť��ʽ
    QString style[4];
    style[0] = pbtnStyle0 + hoverStyle + pressStyle;
    style[1] = pbtnStyle1 + hoverStyle + pressStyle;
    style[2] = pbtnStyle0;
    style[3] = pbtnStyle1;
    if(status >= 0 && status < 4)
        pbtn->setStyleSheet(style[status]);
    if(status == 0 || status == 2){
        pbtn->setText("0");
    }
    else if(status == 1 || status == 3){
        pbtn->setText("1");
    }
    else {
        pbtn->setText("?");
        pbtn->setStyleSheet(unknownStyle);
    }
}

//��ת���ݿ�
void HCodeGen::flipDataBlk(QPushButton *pbtns, int dno)
{
    //��ť��ʽ
    QString style0 = pbtnStyle0 + hoverStyle + pressStyle;
    QString style1 = pbtnStyle1 + hoverStyle + pressStyle;
    //������Ϣ��
    int bits = dataBits;
    if(pbtns == CodeBlk[CODE_IN]){
        bits = dataBits + parityBits;
    }
    QString valStr = getValStr(pbtns, bits);
    if(pbtns[dno].text() == "0"){
        valStr[dno] = '1';
        pbtns[dno].setText("1");
        pbtns[dno].setStyleSheet(style1);
    }
    else {
        valStr[dno] = '0';
        pbtns[dno].setText("0");
        pbtns[dno].setStyleSheet(style0);
    }
    ///������Ϣ�������ǰ��Ϊ��ֹ�������´������ݿ���£�����ʱ�������
    if(pbtns == DataBlk){
        disconnect(ui->lnDataCode, SIGNAL(textChanged(const QString &)), this, SLOT(updateDataBlk(const QString &)));   //�������
        ui->lnDataCode->setText(valStr);    //������Ϣ��
        connect(ui->lnDataCode, SIGNAL(textChanged(const QString &)), this, SLOT(updateDataBlk(const QString &)));      //�ָ�����
    }
    else if(pbtns == CodeBlk[CODE_IN]){
        ui->lnCodeIn->setText(valStr);
    }
}

//������Ϣ�����ݿ�
void HCodeGen::updateDataBlk(const QString &dataStr)
{
    int len = dataStr.length();
    for(int i = 0; i < len; i++){
        if(dataStr[i] == '0'){   //˳���ж�
            DataBlk[i].setStyleSheet(pbtnStyle0);
            DataBlk[i].setText("0");
        }
        else {
            DataBlk[i].setStyleSheet(pbtnStyle1);
            DataBlk[i].setText("1");
        }
    }
}

//�ƶ���Ϣ�����ݿ�
void HCodeGen::moveDataBlk(int dno)
{
    //���˷Ƿ�ֵ
    if(dno < 0 || dno >= dataBits)
        return;
    //��������/�յ����ʽ
    QString startStyle, endStyle;
    if(DataBlk[dno].text() == "0"){
        startStyle = pbtnStyle0 + hoverStyle + pressStyle;
        endStyle = pbtnStyle0;
    }
    else {
        startStyle = pbtnStyle1 + hoverStyle + pressStyle;
        endStyle = pbtnStyle1;
    }
    //ƽ�ƶ���
    QPropertyAnimation *pMove = nullptr;
    int duration_ms = 800;
    int hno = cvtDno2Hno(dno);
    moveStyle = dataStyle;     //�ƶ���ʽ
    //��ȡ����ָ��
    pMove = moveBlk(&(DataBlk[dno]), &(HammBlk[hno]), duration_ms, startStyle, endStyle);
    connect(pMove, &QPropertyAnimation::finished, [=](){
        //���ú������
        QString val = ui->lnHammCode->text();
        val[hno] = HammResult.data[dno];
        ui->lnHammCode->setText(val);
        //���Ͷ��������ź�
        emit moveFinished();
    });
    pMove->start(QAbstractAnimation::DeleteWhenStopped);
}

//У����
void HCodeGen::getCheckResult(int pno)
{
    if(pno < 0 || pno >= parityBits)
        return;
    int hlen = PrHnoLen[pno];
    int status = 0;
    for(int i = 0; i <= hlen; i++){
        if(PrRowBlks[pno][i].text() == "1")
            status = !status;
    }
    status += 2;    //���ɰ�ѹ��ʽ
    setBlkStatus(&(PrRowBlks[pno][hlen+1]), status);

}

//����У��
void HCodeGen::genCheckBlk(int pno)
{
    //���˷Ƿ�ֵ
    if(pno < 0 || pno >= parityBits)
        return;
    //���ж�����
    QSequentialAnimationGroup *pSeqGrp = new QSequentialAnimationGroup;
    int n = PrHnoLen[pno] + 1;
    QPropertyAnimation **pMove = new QPropertyAnimation *[n];
    int duration_ms = 1200;
    //������������Ϣλ->У��λ�Ĳ�����
    QString startStyle, endStyle;   //����/�յ����ʽ
    moveStyle = dataStyle;     //�ƶ���ʽ
    for(int i = 0; i < n; i++){
        int hno = PrHnos[pno][i] - 1;
        if(CodeBlk[CODE_IN][hno].text() == "0"){
            startStyle = pbtnStyle0 + hoverStyle + pressStyle;
            endStyle = pbtnStyle0;
        }
        else {
            startStyle = pbtnStyle1 + hoverStyle + pressStyle;
            endStyle = pbtnStyle1;
        }
        //qDebug() << "add anim" << QString::number(k);
        //��ȡ����ָ��
        pMove[i] = moveBlk(&(CodeBlk[CODE_IN][hno]), &(PrRowBlks[pno][i]), duration_ms, startStyle, endStyle);
        pSeqGrp->addAnimation(pMove[i]);
    }
    //���ж�����������ͷŶ����鼰��ָ�����飬����У����
    connect(pSeqGrp, &QSequentialAnimationGroup::finished, [=](){
        for(int i = 0; i < n; i++)
            delete pMove[i];
        delete []pMove;
        //����У����
        getCheckResult(pno);
        setCheckStepFinish();    //��������
    });
    //�������ж�����
    pSeqGrp->start(QAbstractAnimation::DeleteWhenStopped);
}

//����У��λ������У��λ���ݿ�
void HCodeGen::genPrBlk(int pno)
{
    //���˷Ƿ�ֵ
    if(pno < 0 || pno >= parityBits)
        return;
    //���ж�����
    QSequentialAnimationGroup *pSeqGrp = new QSequentialAnimationGroup;
    int n = PrHnoLen[pno];
    QPropertyAnimation **pMove = new QPropertyAnimation *[n];
    int duration_ms = 800;
    //������������Ϣλ->У��λ�Ĳ�����
    QString startStyle, endStyle;   //����/�յ����ʽ
    moveStyle = dataStyle;     //�ƶ���ʽ
    for(int i = 0; i < n; i++){
        int hno = PrHnos[pno][i+1] - 1;
        if(HammBlk[hno].text() == "0"){
            startStyle = pbtnStyle0 + hoverStyle + pressStyle;
            endStyle = pbtnStyle0;
        }
        else {
            startStyle = pbtnStyle1 + hoverStyle + pressStyle;
            endStyle = pbtnStyle1;
        }
        //qDebug() << "add anim" << QString::number(k);
        //��ȡ����ָ��
        pMove[i] = moveBlk(&(HammBlk[hno]), &(PrRowBlks[pno][i]), duration_ms, startStyle, endStyle);
        pSeqGrp->addAnimation(pMove[i]);
    }
    //���ж�����������ͷŶ����鼰��ָ�����飬�����������У��������
    connect(pSeqGrp, &QSequentialAnimationGroup::finished, [=](){
        for(int i = 0; i < n; i++)
            delete pMove[i];
        delete []pMove;
        //�����������У��������
        movePrBlk(pno);
    });
    //�������ж�����
    pSeqGrp->start(QAbstractAnimation::DeleteWhenStopped);
}

//�ƶ�У����飺У��λ��->У������
void HCodeGen::movePrBlk(int pno)
{
    //���˷Ƿ�ֵ
    if(pno < 0 || pno >= parityBits)
        return;
    //У��λ���ļ�����->У��������У���
    QPropertyAnimation *pMove = nullptr;
    int duration_ms = 800;
    int n = PrHnoLen[pno];
    int prStatus = HammResult.check[pno] - '0';
    setBlkStatus(&(PrRowBlks[pno][n]), prStatus); //������ʼ���ֵ�������������鸳ֵ
    //����/�յ����ʽ
    QString startStyle, endStyle;
    if(PrRowBlks[pno][n].text() == "0"){
        startStyle = pbtnStyle0;
        endStyle = pbtnStyle0;
    }
    else {
        startStyle = pbtnStyle1;
        endStyle = pbtnStyle1;
    }
    moveStyle = parityStyle;     //�ƶ���ʽ
    //��ȡ����ָ��
    pMove = moveBlk(&(PrRowBlks[pno][n]), &(ParityBlk[pno]), duration_ms, startStyle, endStyle);
    connect(pMove, &QPropertyAnimation::finished, [=](){
        //����У�����
        QString val = ui->lnPrCode->text();
        val[pno] = HammResult.check[pno];
        ui->lnPrCode->setText(val);
        //���ͽ����ź�
        emit moveFinished();
    });
    //��������
    pMove->start(QAbstractAnimation::DeleteWhenStopped);
}

//�ƶ�У����飺У������->��������
void HCodeGen::moveParityrBlk(int pno)
{
    //���˷Ƿ�ֵ
    if(pno < 0 || pno >= parityStyle)
        return;
    //ƽ�ƶ���
    QPropertyAnimation *pMove = nullptr;
    int duration_ms = 800;
    int hno = cvtPno2Hno(pno);
    //��������/�յ����ʽ
    QString startStyle, endStyle;
    if(ParityBlk[pno].text() == "0"){
        startStyle = pbtnStyle0;
        endStyle = pbtnStyle0;
    }
    else {
        startStyle = pbtnStyle1;
        endStyle = pbtnStyle1;
    }
    moveStyle = parityStyle;     //�ƶ���ʽ
    pMove = moveBlk(&(ParityBlk[pno]), &(HammBlk[hno]), duration_ms, startStyle, endStyle);
    connect(pMove, &QPropertyAnimation::finished, [=](){
        //���ú������
        QString val = ui->lnHammCode->text();
        val[hno] = HammResult.check[pno];
        ui->lnHammCode->setText(val);
        //���Ͷ��������ź�
        emit moveFinished();
    });
    pMove->start(QAbstractAnimation::DeleteWhenStopped);
}

//�ƶ����ݿ�
QPropertyAnimation *HCodeGen::moveBlk(QPushButton *pStart, QPushButton *pEnd,
                        int duration_ms, QString startStyle, QString endStyle)
{
    QPoint pt_start = pStart->mapTo(this, QPoint(0,0)); //���
    QPoint pt_end = pEnd->mapTo(this, QPoint(0,0));     //�յ�
    QString blkText = pStart->text();   //��ǩ
    int blk_w = pStart->width();
    int blk_h = pStart->height();
    //���ö�����
    QPushButton *pBlk = new QPushButton;
    pBlk->setParent(this);
    pBlk->setStyleSheet(moveStyle);
    pBlk->setText(blkText);
    pBlk->setGeometry(pt_start.x(), pt_start.y(), blk_w, blk_h);
    pBlk->show();
    //ƽ�ƶ���
    QPropertyAnimation *pMove = new QPropertyAnimation(pBlk, "pos");
    duration_ms /= speedLevel;      //������ʱ
    pMove->setDuration(duration_ms);    //��ʱ
    pMove->setStartValue(pt_start);     //���
    pMove->setEndValue(pt_end);         //�յ�
    pMove->setEasingCurve(QEasingCurve::InOutQuad); //��������
    //�����������ͷŶ����飬�������ݿ���ʽ
    connect(pMove, &QPropertyAnimation::finished, [=](){
        delete pBlk;
        pStart->setStyleSheet(startStyle);
        pEnd->setText(blkText);
        pEnd->setStyleSheet(endStyle);
    });
    return pMove;
}
