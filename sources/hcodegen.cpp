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

//编码转换
static QString cvtStr2LocalQStr(const char *str)
{
    return QString::fromLocal8Bit(str);
}

//构造函数
HCodeGen::HCodeGen(SubWindow *parent) :
    SubWindow(parent),
    ui(new Ui::HCodeGen)
{
    ui->setupUi(this);

    InitMembers();
    InitConnections();
}

//析构函数
HCodeGen::~HCodeGen()
{
    delete ui;
}

/******************** 转换函数 ********************/

//信息码下标->海明码下标
int HCodeGen::cvtDno2Hno(int index)
{
    if(index < 0 || index >= DATA_MAX)
        return -1;
    return DataLink[index];
}

//校验码下标->海明码下标
int HCodeGen::cvtPno2Hno(int index)
{
    if(index < 0 || index >= PARITY_MAX)
        return -1;
    return (1 << index) - 1;
}

//计算校验码的操作数下标->海明码下标
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

//从数据块获取数字串
QString HCodeGen::getValStr(QPushButton *pbtn, int n)
{
    QString valStr;
    for(int i = 0; i < n; i++)
        valStr = valStr + pbtn[i].text();
    return valStr;
}

/******************** 初始化 ********************/

//初始化数据成员
void HCodeGen::InitMembers()
{
    //样式初始化
    InitStyles();
    QString labStyle = "background:#F4F4F4";
    //标志位初始化
    rowChanged = -1;
    hammGenFlag = 0;
    for(int i = 0; i < HAMM_MAX; i++)
        codeChangeFlag[i] = 0;
    //单步动画状态
    speedLevel = 1;             //原速播放
    stepStatus = INIT_STATUS;
    checkStatus = INIT_STATUS;
    ui->pbtnFillCodeIn->setEnabled(false);
    //海明码H对应的信息位D/校验位P
    for(int i = 0; i < HAMM_MAX; i++)
        HammLink[i] = 0;        //清零
    for(int base=1, i=1; base < HAMM_MAX; base<<=1, i++)
        HammLink[base-1] = -i;  //校验位Px记负数
    for(int i=2, cnt=1; i < HAMM_MAX; i++){
        if(HammLink[i] != 0)
            continue;
        HammLink[i] = cnt;      //信息位Dx记正数
        DataLink[cnt-1] = i;
        cnt++;
    }
    //for(int i = 0; i < HAMM_MAX; i++)
    //  qDebug() << "H"+QString::number(i+1) << "=" << HammLink[i];

    //数据块/标签初始化
    int blk_w = 30, blk_h = 30, delta_w = 10;
    int blk_x, blk_y = 10, ofs_x = 10;
    int lab_h = 16, lab_y = 2;
    QString valStr0 = "0", valStrUnknown = "?";
    //信息码
    blk_x = ofs_x;
    QString style0 = pbtnStyle0 + hoverStyle + pressStyle;
    for(int i = 0; i < DATA_MAX; i++){
        //数据块
        setPbtnProp(&(DataBlk[i]), ui->wgtDataBlk, style0, valStr0);
        DataBlk[i].setGeometry(blk_x, blk_y, blk_w, blk_h);
        //标签
        setLabProp(&(DataLab[i]), ui->wgtDataLab, dataStyle,
                   Qt::AlignHCenter|Qt::AlignVCenter, "D" + QString::number(i+1));
        DataLab[i].setGeometry(blk_x, lab_y, blk_w, lab_h);
        //更新坐标
        blk_x += blk_w + delta_w;
    }
    //海明码
    blk_x = ofs_x;
    QString hammStr, hammStyle;
    for(int i = 0; i < HAMM_MAX; i++){
        //数据块
        setPbtnProp(&(HammBlk[i]), ui->wgtHammBlk, unknownStyle, valStrUnknown);
        HammBlk[i].setGeometry(blk_x, blk_y, blk_w, blk_h);
        //标签
        setLabProp(&(HammLab[i]), ui->wgtHammLab, labStyle,
                   Qt::AlignHCenter|Qt::AlignVCenter, "H" + QString::number(i+1));
        HammLab[i].setGeometry(blk_x, lab_y, blk_w, lab_h);
        //海明码对应的信息位/校验位标签
        if(HammLink[i] > 0){     //信息位
            hammStr = "D" + QString::number(HammLink[i]);
            hammStyle = dataStyle;
        }
        else {    //校验位
            hammStr = "P" + QString::number(-HammLink[i]);
            hammStyle = parityStyle;
        }
        setLabProp(&(HammLinkLab[i]), ui->wgtHammLink, hammStyle,
                   Qt::AlignHCenter|Qt::AlignVCenter, hammStr);
        HammLinkLab[i].setGeometry(blk_x, lab_y, blk_w, lab_h);
        //待校验数据
        CodeInStatus[i] = CODE_OK;
        setPbtnProp(&(CodeBlk[CODE_IN][i]), ui->wgtCodeInBlk, okStyle, valStr0);
        CodeBlk[CODE_IN][i].setGeometry(blk_x, blk_y, blk_w, blk_h);
        //标签
        setLabProp(&(CodeLab[CODE_IN][i]), ui->wgtCodeInLab, labStyle,
                   Qt::AlignHCenter|Qt::AlignVCenter, "H" + QString::number(i+1));
        CodeLab[CODE_IN][i].setGeometry(blk_x, lab_y, blk_w, lab_h);

        setLabProp(&(HammLinkLab2[i]), ui->wgtCodeInLink, hammStyle,
                   Qt::AlignHCenter|Qt::AlignVCenter,hammStr);
        HammLinkLab2[i].setGeometry(blk_x, lab_y, blk_w, lab_h);
        //纠错后数据
        setPbtnProp(&(CodeBlk[CODE_OUT][i]), ui->wgtCodeOutBlk, unknownStyle, valStrUnknown);
        CodeBlk[CODE_OUT][i].setGeometry(blk_x, blk_y, blk_w, blk_h);
        //标签
        setLabProp(&(CodeLab[CODE_OUT][i]), ui->wgtCodeOutLab, labStyle,
                   Qt::AlignHCenter|Qt::AlignVCenter, "H" + QString::number(i+1));
        CodeLab[CODE_OUT][i].setGeometry(blk_x, lab_y, blk_w, lab_h);

        //更新坐标
        blk_x += blk_w + delta_w;
    }
    //校验码
    blk_x = ofs_x;
    for(int i = 0; i < PARITY_MAX; i++){
        //数据块
        setPbtnProp(&(ParityBlk[i]), ui->wgtPrBlk, unknownStyle, valStrUnknown);
        ParityBlk[i].setGeometry(blk_x, blk_y, blk_w, blk_h);
        //标签
        setLabProp(&(ParityLab[i]), ui->wgtPrLab, parityStyle,
                   Qt::AlignHCenter|Qt::AlignVCenter,"P" + QString::number(i+1));
        ParityLab[i].setGeometry(blk_x, lab_y, blk_w, lab_h);
        //更新坐标
        blk_x += blk_w + delta_w;
    }
    //校验位
    //  //行头
    PrRowHead[0] = ui->labP1; PrRowHead[1] = ui->labP2;
    PrRowHead[2] = ui->labP3; PrRowHead[3] = ui->labP4;
    // //计算区
    wgtPrBlk[0] = ui->wgtP1Blk; wgtPrBlk[1] = ui->wgtP2Blk;
    wgtPrBlk[2] = ui->wgtP3Blk; wgtPrBlk[3] = ui->wgtP4Blk;
    // //标签区
    wgtPrLab[0] = ui->wgtP1Lab; wgtPrLab[1] = ui->wgtP2Lab;
    wgtPrLab[2] = ui->wgtP3Lab; wgtPrLab[3] = ui->wgtP4Lab;
    // //每个区域的数据块与标签
    delta_w = 20;
    HammingBack converter;
    // // //操作数和结果
    for(int i = 0; i < PARITY_MAX; i++){
        //计算校验码对应的所有信息位
        int dlen = converter.calChecHammlen(DATA_MAX, i+1);
        converter.calCheckHnoList(DATA_MAX, i+1, PrHnos[i], dlen);
        blk_x = ofs_x;
        for(int j = 0; j < OPRD_MAX+2; j++){
            //数据块
            setPbtnProp(&(PrRowBlks[i][j]), wgtPrBlk[i], unknownStyle, valStrUnknown);
            PrRowBlks[i][j].setGeometry(blk_x, blk_y, blk_w, blk_h);
            //标签
            setLabProp(&(PrRowLabs[i][j]), wgtPrLab[i], labStyle,
                       Qt::AlignHCenter|Qt::AlignVCenter,"H?");
            PrRowLabs[i][j].setGeometry(blk_x, lab_y, blk_w, lab_h);
            //更新坐标
            blk_x += blk_w + delta_w;
        }
    }
    // // //操作符
    QString labStyle0 = "border:0px";
    for(int i = 0; i < PARITY_MAX; i++){
        blk_x = ofs_x + blk_w;
        for(int j = 0; j < OPTR_MAX+1; j++){
            //数据块
            setLabProp(&(PrRowOptrBlkLabs[i][j]), wgtPrBlk[i], labStyle0,
                       Qt::AlignHCenter|Qt::AlignVCenter, "+");
            PrRowOptrBlkLabs[i][j].setGeometry(blk_x, blk_y, delta_w, blk_h);
            //标签
            setLabProp(&(PrRowOptrLabs[i][j]), wgtPrLab[i], labStyle0,
                       Qt::AlignHCenter|Qt::AlignVCenter, "+");
            PrRowOptrLabs[i][j].setGeometry(blk_x, lab_y, delta_w, lab_h);
            //更新坐标
            blk_x += blk_w + delta_w;
        }
    }
    //设置可见性
    int data_bits = ui->spinDataBit->value();
    if(data_bits < 3 || data_bits > 5)
        data_bits = 4;
    setBlkVis(data_bits);

    //表格对应行高亮显示
    QString valStr = ui->lnDataCode->text();
    rowChanged = converter.bstring2int(valStr.toStdString());
    changeCodeRowStyle(rowChanged, STYLE_HLIGHT);
}

//关联信号与槽
void HCodeGen::InitConnections()
{
    //输入信息码位数，改变数据块状态
    connect(ui->spinDataBit, SIGNAL(valueChanged(int)), this, SLOT(setBlkVis(int)));
    //输入信息码，更新信息码数据块的值
    connect(ui->lnDataCode, SIGNAL(textChanged(const QString &)), this, SLOT(updateDataBlk(const QString &)));
    for(int i = 0; i < DATA_MAX; i++)       //点击按钮，反转数据块的值
        connect(&(DataBlk[i]), &QPushButton::clicked, [=](){flipDataBlk(DataBlk, i);});
    for(int i = 0; i < HAMM_MAX; i++)       //点击按钮，反转数据块的值
        connect(&(CodeBlk[CODE_IN][i]), &QPushButton::clicked, [=](){flipDataBlk(CodeBlk[CODE_IN], i, BLK_CHECK);});

    //动画速度
    connect(ui->pbtnSpeed, SIGNAL(clicked()), this, SLOT(changeSpeed()));
    //单步动画
    connect(ui->pbtnHamStep, SIGNAL(clicked()), this, SLOT(updateGenStatus()));    //下一步动画
    connect(ui->pbtnHamRestart, &QPushButton::clicked, [=](){       //状态还原
        setBlkVis(dataBits);
        setGenInit();
        setCheckBtnsEnabled(true);
        hammGenFlag = 0;
    });
    connect(this, SIGNAL(moveFinished()), this, SLOT(setGenFinish()));   //设置动画结束状态
    //一次性生成海明码
    connect(ui->pbtnHamFinal, SIGNAL(clicked()), this, SLOT(getFinalHamm()));
    //用海明码填充待校验数据
    connect(ui->pbtnFillCodeIn, &QPushButton::clicked, [=](){
        QString valStr = ui->lnHammCode->text();
        ui->lnCodeIn->setText(valStr);
        int hamm_bits = dataBits + parityBits;
        for(int i = 0; i < hamm_bits; i++){
            CodeBlk[CODE_IN][i].setStyleSheet(okStyle);
            codeChangeFlag[i] = 0;
            CodeInStatus[i] = CODE_OK;
            if(valStr[i] == '1')
                CodeBlk[CODE_IN][i].setText("1");
            else
                CodeBlk[CODE_IN][i].setText("0");
            /*
            int status = (valStr[i] == '1');
            setBlkStatus(&(CodeBlk[CODE_IN][i]), status);
            */
        }
    });
    //检错纠错单步动画
    connect(ui->pbtnCheckStep, SIGNAL(clicked()), this, SLOT(updateCheckStatus()));
    //状态还原
    connect(ui->pbtnCheckRestart, &QPushButton::clicked, [=](){
        checkStatus = INIT_STATUS;
        stepStatusStr = cvtStr2LocalQStr("检错纠错\n(单步动画)");
        setCheckFinish();
        setCheckInit();
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
        if(hammGenFlag)
            ui->pbtnFillCodeIn->setEnabled(true);
    });
    //一次性得出结果
    connect(ui->pbtnCheckFinal, &QPushButton::clicked, [=](){
        getFinalCheck();
        updateCheckRow();
    });
}

//初始化样式表
void HCodeGen::InitStyles()
{
    pbtnStyle0 = "QPushButton{background:#F0F0F0}";     //值为0
    pbtnStyle1 = "QPushButton{background:#C0C0C0}";     //值为1
    unknownStyle = "background:#E8F0FF";    //值未知

    hoverStyle = "QPushButton:hover{background:#DCF0FC}";       //悬停
    pressStyle = "QPushButton:pressed{background:#D0E0FC}";     //按压

    dataStyle = "background:#FFCCCC;border:1px solid grey";     //信息块
    parityStyle = "background:#CCCCFF;border:1px solid grey";   //校验块
    moveStyle = "background:#FFCCCC;border:1px solid grey";     //移动

    okStyle = "background:#CCFFCC";     //无错
    errStyle = "background:#FFA8C0";    //有错
    chgStyle = "background:#F0D0FF";    //被修改
}

/******************** 动画设置 ********************/

//改变动画播放速度
void HCodeGen::changeSpeed()
{
    if(speedLevel == 1){
        speedLevel = 2;
        ui->pbtnSpeed->setText(cvtStr2LocalQStr("原速播放"));
    }
    else {
        speedLevel = 1;
        ui->pbtnSpeed->setText(cvtStr2LocalQStr("倍速播放(x2)"));
    }
}

/******************** 属性设置 ********************/

//设置按钮属性
void HCodeGen::setPbtnProp(QPushButton *pbtn, QWidget *parent, QString style, QString text)
{
    pbtn->setParent(parent);
    pbtn->setStyleSheet(style);
    pbtn->setText(text);
}

//设置标签属性
void HCodeGen::setLabProp(QLabel *lab, QWidget *parent, QString style, Qt::Alignment align, QString text)
{
    lab->setParent(parent);
    lab->setStyleSheet(style);
    lab->setAlignment(align);
    lab->setText(text);
}

/******************** 合法编码表 ********************/

//填充合法编码表
void HCodeGen::updateLegalCodeTable()
{
    HammingBack converter;
    int n = (1 << dataBits);
    int hamm_bits = dataBits + parityBits;
    //行列数
    ui->tblLegalCode->setRowCount(n);
    ui->tblLegalCode->setColumnCount(hamm_bits);
    //列宽
    int width = 51;
    for(int i = 0; i < hamm_bits; i++)
        ui->tblLegalCode->setColumnWidth(i, width);
    //列标题
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
    //行标题
    QStringList Vheader;
    for(int i = 0; i < n; i++)
            Vheader << QString::number(i);
    ui->tblLegalCode->setVerticalHeaderLabels(Vheader);
    ui->tblLegalCode->verticalHeader()->setMinimumWidth(30);
    //表格内容
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

//改变行样式
void HCodeGen::changeCodeRowStyle(int row, int choice)
{
    QColor colorD = "#FFCCCC", colorP = "#CCCCFF", colorOK = "#CCFFCC";
    QTableWidget *pTbl = ui->tblLegalCode;
    int row_n = pTbl->rowCount();
    if(row < 0 || row >= row_n)
        return;
    int col_n = pTbl->columnCount();
    int hamm_bits = dataBits + parityBits;
    if(hamm_bits > col_n)
        return;
    for(int i = 0; i < hamm_bits; i++){
        if(choice == STYLE_NORMAL){
            if(HammLink[i] > 0)
                pTbl->item(row, i)->setBackground(colorD);
            else
                pTbl->item(row, i)->setBackground(colorP);
        }
        else if(choice == STYLE_HLIGHT){
            pTbl->item(row, i)->setBackground(colorOK);
        }
    }
}

//更新纠错后的编码对应的行
void HCodeGen::updateCheckRow()
{
    int hamm_bits = dataBits + parityBits;
    QString dataStr;
    for(int i = 0; i < hamm_bits; i++){
        if(HammLink[i] > 0)
            dataStr = dataStr + CodeBlk[CODE_OUT][i].text();
    }
    HammingBack converter;
    //取消之前的编码高亮显示
    if(rowChanged >= 0)
        changeCodeRowStyle(rowChanged, STYLE_NORMAL);
    //表格中对应的合法编码高亮显示
    rowChanged =  converter.bstring2int(dataStr.toStdString());
    changeCodeRowStyle(rowChanged, STYLE_HLIGHT);

}

/******************* 组件状态更新 *******************/

//更新信息码框，根据数据块补全输入框的数据位数
void HCodeGen::updateDataCode()
{
    //从数据块获取数字串
    QString valStr = getValStr(DataBlk, dataBits);
    //取消之前的编码高亮显示
    HammingBack converter;
    if(rowChanged >= 0)
        changeCodeRowStyle(rowChanged, STYLE_NORMAL);
    //表格中对应的合法编码高亮显示
    rowChanged = converter.bstring2int(valStr.toStdString());
    changeCodeRowStyle(rowChanged, STYLE_HLIGHT);

    //为防止信息码框变化带动数据块变化，暂时解除连接
    disconnect(ui->lnDataCode, SIGNAL(textChanged(const QString &)), this, SLOT(updateDataBlk(const QString &)));   //解除连接
    ui->lnDataCode->setText(valStr);    //设置信息码框
    connect(ui->lnDataCode, SIGNAL(textChanged(const QString &)), this, SLOT(updateDataBlk(const QString &)));      //恢复连接
}

//更新校验位二进制标签
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

//启用/禁用按钮组（生成海明码按钮组）
void HCodeGen::setGenBtnsEnabled(bool flag)
{
    ui->pbtnHamFinal->setEnabled(flag);     //一次性生成海明码
    ui->pbtnHamStep->setEnabled(flag);      //单步动画
    ui->pbtnHamRestart->setEnabled(flag);   //重新开始
}

//启用/禁用按钮组（检错纠错按钮组）
void HCodeGen::setCheckBtnsEnabled(bool flag)
{
    ui->pbtnCheckStep->setEnabled(flag);     //一次性检错纠错
    ui->pbtnCheckFinal->setEnabled(flag);    //单步动画
    ui->pbtnCheckRestart->setEnabled(flag);  //重新开始
}

/****************** 数据块状态设置 ******************/

//设置数据块状态未知
void HCodeGen::setBlkUnknown()
{
    QString textUnKnown = "?";
    //海明码
    QString valStr;
    int fullBits = dataBits + parityBits;
    for(int i = 0; i < fullBits; i++){
        valStr = valStr + textUnKnown;
        HammBlk[i].setText(textUnKnown);
        HammBlk[i].setStyleSheet(unknownStyle);
    }
    ui->lnHammCode->setText(valStr);    //设置显示框
    //校验码
    valStr = "";
    for(int i = 0; i < parityBits; i++){
        valStr = valStr + textUnKnown;
        ParityBlk[i].setText(textUnKnown);
        ParityBlk[i].setStyleSheet(unknownStyle);
    }
    ui->lnPrCode->setText(valStr);      //设置显示框
    //校验位
    for(int i = 0; i < PARITY_MAX; i++){
        for(int j = 0; j < OPRD_MAX+1; j++){
            PrRowBlks[i][j].setText(textUnKnown);
            PrRowBlks[i][j].setStyleSheet(unknownStyle);
        }
    }

}

//设置数据块状态
void HCodeGen::setBlkStatus(QPushButton *pbtn, int status)
{
    //按钮样式
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

//反转数据块
void HCodeGen::flipDataBlk(QPushButton *pbtns, int dno, int blktype)
{
    //按钮样式
    QString style0 = pbtnStyle0 + hoverStyle + pressStyle;
    QString style1 = pbtnStyle1 + hoverStyle + pressStyle;
    //计算信息码
    int bits = dataBits;
    if(pbtns == CodeBlk[CODE_IN]){
        bits = dataBits + parityBits;
    }
    QString valStr = getValStr(pbtns, bits);
    if(pbtns[dno].text() == "0"){
        valStr[dno] = '1';
        pbtns[dno].setText("1");
        if(blktype == BLK_NORMAL)
            pbtns[dno].setStyleSheet(style1);
    }
    else {
        valStr[dno] = '0';
        pbtns[dno].setText("0");
        if(blktype == BLK_NORMAL)
            pbtns[dno].setStyleSheet(style0);
    }
    if(blktype == BLK_CHECK){
        codeChangeFlag[dno] = !codeChangeFlag[dno];
        if(codeChangeFlag[dno]){
            CodeBlk[CODE_IN][dno].setStyleSheet(chgStyle);
        }
        else {
            if(CodeInStatus[dno] == CODE_OK)
                CodeBlk[CODE_IN][dno].setStyleSheet(okStyle);
            else
                CodeBlk[CODE_IN][dno].setStyleSheet(errStyle);
        }
    }

    ///设置信息码输入框前，为防止输入框更新带动数据块更新，需暂时解除连接
    if(pbtns == DataBlk){
        disconnect(ui->lnDataCode, SIGNAL(textChanged(const QString &)), this, SLOT(updateDataBlk(const QString &)));   //解除连接
        ui->lnDataCode->setText(valStr);    //设置信息码
        connect(ui->lnDataCode, SIGNAL(textChanged(const QString &)), this, SLOT(updateDataBlk(const QString &)));      //恢复连接
    }
    else if(pbtns == CodeBlk[CODE_IN]){
        ui->lnCodeIn->setText(valStr);
    }
}

/******************* 海明码生成动画 *******************/

//设置数据块/标签可见性
void HCodeGen::setBlkVis(int data_bits)
{
    //单步动画恢复初始状态
    setGenInit();
    //限制信息码输入框只能输指定长度的0/1串
    dataBits = data_bits;
    QString rgx0 = QString("^[0-1]{%1}$").arg(data_bits);
    QValidator *vdt = new QRegExpValidator(QRegExp(rgx0));
    ui->lnDataCode->setValidator(vdt);
    //信息码
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

    //校验码
    HammingBack converter;
    parityBits = converter.calCheckLen(data_bits);     //校验码位数
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
    //填充文本框
    ui->lnPrBit->setText(QString::number(parityBits));
    ui->lnPrCode->setText(valStr);

    //海明码
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
        CodeBlk[CODE_IN][i].setText("0");
        CodeBlk[CODE_IN][i].setStyleSheet(okStyle);
        CodeInStatus[i] = CODE_OK;
        codeChangeFlag[i] = 0;
        //setBlkStatus(&(CodeBlk[CODE_IN][i]), 0);

        CodeLab[CODE_IN][i].setVisible(true);
        HammLinkLab2[i].setVisible(true);
        CodeBlk[CODE_OUT][i].setVisible(true);
        CodeBlk[CODE_OUT][i].setText("?");
        CodeBlk[CODE_OUT][i].setStyleSheet(unknownStyle);

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
        CodeLab[CODE_OUT][i].setVisible(false);
    }
    //填充文本框
    ui->lnHammBit->setText(QString::number(hamm_bits));     //海明码位数
    ui->lnHammCode->setText(valStr);
    ui->lnCodeIn->setText(codeStr);
    ui->lnCodeOut->setText(valStr);

    //校验位
    updatePrBitLab();       //更新校验位二进制标签
    for(int i = 0; i < parityBits; i++){
        //求出计算校验位所需的海明码位数
        int hlen = converter.calChecHammlen(data_bits, i+1) - 1;
        PrHnoLen[i] = hlen;
        //显示操作数块
        for(int j = 0; j < hlen; j++){
            PrRowBlks[i][j].setVisible(true);   //数据块
            PrRowLabs[i][j].setVisible(true);   //标签
            PrRowLabs[i][j].setText("H"+QString::number(PrHnos[i][j+1]));
            PrRowLabs[i][j].setStyleSheet(dataStyle);
            //设置操作符
            PrRowOptrBlkLabs[i][j].setVisible(true);
            PrRowOptrLabs[i][j].setVisible(true);
            PrRowOptrBlkLabs[i][j].setText("+");
            PrRowOptrLabs[i][j].setText("+");
        }
        PrRowOptrBlkLabs[i][hlen-1].setText("=");
        PrRowOptrLabs[i][hlen-1].setText("=");
        //显示结果块
        PrRowBlks[i][hlen].setVisible(true);
        PrRowLabs[i][hlen].setVisible(true);
        PrRowLabs[i][hlen].setText("H"+QString::number(PrHnos[i][0]));
        PrRowLabs[i][hlen].setStyleSheet(parityStyle);
        //隐藏多余块
        for(int j = hlen; j < OPTR_MAX+1; j++){
            PrRowOptrBlkLabs[i][j].setVisible(false);  //操作符(数据块区)
            PrRowOptrLabs[i][j].setVisible(false);  //操作符(标签区)
        }
        for(int j = hlen+1; j < OPRD_MAX+2; j++){
            PrRowBlks[i][j].setVisible(false);   //数据块
            PrRowLabs[i][j].setVisible(false);   //标签
        }
    }
    //更新合法编码表
    updateLegalCodeTable();

    //从数据块获取数字串
    valStr = getValStr(DataBlk, dataBits);
    //取消之前的编码高亮显示
    if(rowChanged >= 0)
        changeCodeRowStyle(rowChanged, STYLE_NORMAL);
    //表格中对应的合法编码高亮显示
    rowChanged = converter.bstring2int(valStr.toStdString());
    changeCodeRowStyle(rowChanged, STYLE_HLIGHT);
}

//海明码生成动画初始化
void HCodeGen::setGenInit()
{
    hammGenFlag = 0;
    stepStatusStr = cvtStr2LocalQStr("海明码生成\n(单步动画)");
    setGenFinish();
    stepStatus = INIT_STATUS;
    ui->pbtnFillCodeIn->setEnabled(false);  //禁止用海明码填充待校验数据
    //允许输入
    ui->spinDataBit->setEnabled(true);      //数字框可修改
    ui->lnDataCode->setReadOnly(false);     //输入框可修改
    for(int i = 0; i < dataBits; i++)       //数据块可改变
        DataBlk[i].setEnabled(true);
    int hamm_bits = dataBits + parityBits;
    for(int i = 0; i < hamm_bits; i++)      //数据块可改变
        CodeBlk[CODE_IN][i].setEnabled(true);
    //恢复信息码数据块状态
    QString valStr = getValStr(DataBlk, dataBits);
    updateDataBlk(valStr);
    //设置未知数据块
    setBlkUnknown();

}

//更新海明码生成动画状态
void HCodeGen::updateGenStatus()
{
    setGenBtnsEnabled(false);  //禁用按钮
    ui->pbtnSpeed->setEnabled(false);
    stepStatus++;
    int hamm_bits = dataBits + parityBits;
    int lastParityStatus = hamm_bits;
    int finalStatus = lastParityStatus + parityBits;
    if(stepStatus == 0){
        QString str = ui->lnDataCode->text();
        setBlkVis(dataBits);
        ui->lnDataCode->setText(str);
        stepStatus = 0;

        stepStatusStr = cvtStr2LocalQStr("单步生成\n填入D1");
        updateDataCode();   //更新信息码框
        setBlkUnknown();    //设置未知块
        setCheckBtnsEnabled(false);         //禁用按钮
        ui->pbtnFillCodeIn->setEnabled(false);
        //禁用输入
        ui->spinDataBit->setEnabled(false); //数字框不可修改
        ui->lnDataCode->setReadOnly(true);  //输入框不可修改
        for(int i = 0; i < dataBits; i++)   //数据块不可改变
            DataBlk[i].setEnabled(false);
        for(int i = 0; i < hamm_bits; i++)  //数据块不可改变
            CodeBlk[CODE_IN][i].setEnabled(false);
        setGenFinish();
        //计算海明码
        HammingBack converter;
        std::string valStr = ui->lnDataCode->text().toStdString();
        HammResult = converter.calHammingResult(valStr);
    }
    else if(stepStatus < dataBits){     //信息码区的信息块->海明码区的信息块
        stepStatusStr = cvtStr2LocalQStr("单步生成\n填入D")+QString::number(stepStatus+1);
        moveDataBlk(stepStatus-1);
    }
    else if(stepStatus == dataBits){    //信息码区的信息块->海明码区的信息块（最后一个）
        stepStatusStr = cvtStr2LocalQStr("单步生成\n计算P1");
        moveDataBlk(stepStatus-1);
    }
    else if(stepStatus < lastParityStatus){     //计算校验位
        int index = stepStatus-dataBits;
        stepStatusStr = cvtStr2LocalQStr("单步生成\n计算P")+QString::number(index+1);
        genPrBlk(index-1);
    }
    else if(stepStatus == lastParityStatus){    //计算校验位（最后一个）
        int index = stepStatus-dataBits;
        stepStatusStr = cvtStr2LocalQStr("单步生成\n填入P1");
        genPrBlk(index-1);
    }
    else if(stepStatus < finalStatus){      //校验码区的校验块->海明码区的校验块
        int index = stepStatus - lastParityStatus;
        stepStatusStr = cvtStr2LocalQStr("单步生成\n填入P")+QString::number(index+1);
        moveParityrBlk(index-1);
    }
    else if(stepStatus == finalStatus){     //校验码区的校验块->海明码区的校验块（最后一个）
        int index = stepStatus - lastParityStatus;
        stepStatusStr = cvtStr2LocalQStr("单步生成\n结束");
        moveParityrBlk(index-1);
        hammGenFlag = 1;
    }
    else if(stepStatus > 0){
        hammGenFlag = 0;
        stepStatusStr = cvtStr2LocalQStr("单步生成\n结束");
        setGenInit();      //恢复初始状态
        setCheckBtnsEnabled(true);      //启用按钮
        for(int i = 0; i < hamm_bits; i++)  //数据块可改变
            CodeBlk[CODE_IN][i].setEnabled(true);
    }
}

//海明码生成动画单步结束
void HCodeGen::setGenFinish()
{
    ui->pbtnHamStep->setText(stepStatusStr);    //更新按钮文字
    setGenBtnsEnabled(true);            //启用按钮
    ui->pbtnSpeed->setEnabled(true);
    //设置下一步的界面
    int lastParityStatus = dataBits + parityBits;
    int finalStatus = lastParityStatus + parityBits;
    //qDebug() << "dataBits :" << dataBits;
    //qDebug() << "prBits :" << parityBits;
    //qDebug() << "status :" << stepStatus;
    if(stepStatus < 0)
        ;
    else if(stepStatus < dataBits){
        //改变将要移动的数据块状态
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
        //允许用海明码填充待校验数据
        ui->pbtnFillCodeIn->setEnabled(true);        
    }

}

//一次性生成海明码
void HCodeGen::getFinalHamm()
{
    //setBlkVis会把data值置全0，需重新赋值
    QString text = ui->lnDataCode->text();
    setBlkVis(dataBits);
    ui->lnDataCode->setText(text);

    setGenInit();      //单步动画还原
    setCheckBtnsEnabled(true);  //启用按钮

    HammingBack converter;
    //获取信息码，计算海明码
    ///输入框的数字串位数可能不足，应通过数据块获取信息码
    QString dataStr = getValStr(DataBlk, dataBits);
    updateDataCode();       //更新信息码输入框，补足位数
    HammResult = converter.calHammingResult(dataStr.toStdString());
    //海明码
    int fullBits = dataBits + parityBits;
    QString valStr = QString::fromStdString(HammResult.fullRes);
    ui->lnHammCode->setText(valStr);            //数据框
    for(int i = 0; i < fullBits; i++){
        int status = HammResult.fullRes[i] - '0';
        setBlkStatus(&(HammBlk[i]), status);    //数据块
    }
    //校验码
    valStr = QString::fromStdString(HammResult.check);
    ui->lnPrCode->setText(valStr);              //数据框
    for(int i = 0; i < parityBits; i++){
        int status = HammResult.check[i] - '0';
        setBlkStatus(&(ParityBlk[i]), status);  //数据块
        int hlen = PrHnoLen[i];
        setBlkStatus(&(PrRowBlks[i][hlen]), status);
        for(int j = 0; j < hlen; j++){
            int hno = cvtPrHno2Hno(i, j);
            //qDebug() << hno;
            status = HammResult.fullRes[hno] - '0';
            setBlkStatus(&(PrRowBlks[i][j]), status);
        }
    }
    //允许用海明码填充待校验数据
    ui->pbtnFillCodeIn->setEnabled(true);
    hammGenFlag = 1;
}

/******************* 检错纠错动画 *******************/
//纠错
void HCodeGen::correctCode(QString &valStr, int hamm_bits, int errbit)
{
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
        status += 2;    //不可点击状态
        CodeBlk[CODE_OUT][i].setStyleSheet(okStyle);
        CodeBlk[CODE_OUT][i].setText(text);
    }
}

//检错纠错动画初始化
void HCodeGen::setCheckInit()
{    
    for(int i = 0; i < parityBits; i++){
        int hlen = PrHnoLen[i];
        //标签
        for(int j = 0; j <= hlen; j++){
            if(j > 0)
                PrRowLabs[i][j].setStyleSheet(dataStyle);
            PrRowLabs[i][j].setText("H"+QString::number(PrHnos[i][j]));
            PrRowBlks[i][j].setText("?");
            PrRowBlks[i][j].setStyleSheet(unknownStyle);
        }
        PrRowLabs[i][0].setStyleSheet(parityStyle);
        //操作符
        PrRowOptrLabs[i][hlen-1].setText("+");
        PrRowOptrBlkLabs[i][hlen-1].setText("+");
        PrRowOptrBlkLabs[i][hlen].setText("=");
        PrRowOptrBlkLabs[i][hlen].setVisible(true);
        //结果
        PrRowBlks[i][hlen+1].setText("?");
        PrRowBlks[i][hlen+1].setStyleSheet(unknownStyle);
        PrRowBlks[i][hlen+1].setVisible(true);
    }
    int hamm_bits = dataBits + parityBits;
    QString valStr;
    for(int i = 0; i < hamm_bits; i++){
        //CodeBlk[CODE_IN][i].setStyleSheet(okStyle);
        /*
        int status = (CodeBlk[CODE_IN][i].text() == "1");
        setBlkStatus(&(CodeBlk[CODE_IN][i]), status);
        */
        valStr = valStr + "?";
        CodeBlk[CODE_OUT][i].setText("?");
        CodeBlk[CODE_OUT][i].setStyleSheet(unknownStyle);
    }
    ui->lnCodeOut->setText(valStr);
}

//更新检错纠错动画状态
void HCodeGen::updateCheckStatus()
{
    setCheckBtnsEnabled(false);         //禁用按钮
    ui->pbtnSpeed->setEnabled(false);
    ui->pbtnFillCodeIn->setEnabled(false);
    static int errbit = -1;
    int hamm_bits = dataBits + parityBits;
    checkStatus++;
    HammingBack converter;
    if(checkStatus == 0){
        stepStatusStr = cvtStr2LocalQStr("单步校验\n第1位");
        setGenBtnsEnabled(false);
        //禁用按钮/输入
        ui->spinDataBit->setEnabled(false);
        ui->lnDataCode->setReadOnly(true);
        for(int i = 0; i < dataBits; i++){
            DataBlk[i].setEnabled(false);
        }
        for(int i = 0; i < hamm_bits; i++){
            CodeBlk[CODE_IN][i].setEnabled(false);
        }

        setCheckInit();
        setCheckFinish();
    }
    //校验
    else if(checkStatus < parityBits){
        stepStatusStr = cvtStr2LocalQStr("单步校验\n第") +
                QString::number(checkStatus+1) +  cvtStr2LocalQStr("位");
        genCheckBlk(checkStatus - 1);
    }
    else if(checkStatus == parityBits){
        stepStatusStr = cvtStr2LocalQStr("单步校验\n检错");
        genCheckBlk(checkStatus - 1);
    }
    //检错
    else if(checkStatus == parityBits + 1){
        QString valStr;
        for(int i = 0; i < parityBits; i++){
            int hlen = PrHnoLen[i];
            valStr = PrRowBlks[i][hlen+1].text() + valStr;
        }
        errbit = converter.bstring2int(valStr.toStdString());
        if(errbit > 0){
            CodeBlk[CODE_IN][errbit-1].setStyleSheet(errStyle);
            stepStatusStr = cvtStr2LocalQStr("单步校验\nH") +
                    QString::number(errbit) + cvtStr2LocalQStr("出错\n纠错");
        }
        else {
            stepStatusStr = cvtStr2LocalQStr("单步校验\n无错！");
        }
        for(int i = 0; i < hamm_bits; i++){
            if(i == errbit - 1)
                continue;
            CodeBlk[CODE_IN][i].setStyleSheet(okStyle);
        }
        setCheckFinish();
    }
    //纠错
    else if(checkStatus == parityBits + 2){
        stepStatusStr = cvtStr2LocalQStr("单步校验\n结束");
        QString valStr = ui->lnCodeIn->text();
        correctCode(valStr, hamm_bits, errbit);     //纠错
        ui->lnCodeOut->setText(valStr);
        updateCheckRow();
        setCheckFinish();
        if(hammGenFlag)
            ui->pbtnFillCodeIn->setEnabled(true);
    }
    else {
        checkStatus = INIT_STATUS;
        stepStatusStr = cvtStr2LocalQStr("检错纠错\n(单步动画)");
        setCheckInit();
        setCheckFinish();
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

//检错纠错动画单步结束
void HCodeGen::setCheckFinish()
{
    ui->pbtnCheckStep->setText(stepStatusStr);
    setCheckBtnsEnabled(true);         //启用按钮
    ui->pbtnSpeed->setEnabled(true);
}

//一次性生成校验结果
void HCodeGen::getFinalCheck()
{
    //状态还原
    checkStatus = INIT_STATUS;
    stepStatusStr = cvtStr2LocalQStr("检错纠错\n(单步动画)");
    setCheckFinish();
    setCheckInit();
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
    //校验位区域的操作数
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
            //设置结束块样式
            PrRowBlks[i][j].setStyleSheet(endStyle);
            PrRowBlks[i][j].setText(text);
        }
        getCheckResult(i);
    }

    //检错
    HammingBack converter;
    QString valStr;
    for(int i = 0; i < parityBits; i++){
        int hlen = PrHnoLen[i];
        valStr = PrRowBlks[i][hlen+1].text() + valStr;
    }
    int errbit = converter.bstring2int(valStr.toStdString());
    if(errbit > 0){
        int k = errbit-1;
        CodeInStatus[k] = CODE_ERR;
        CodeBlk[CODE_IN][k].setStyleSheet(errStyle);
    }
    for(int i = 0; i < hamm_bits; i++){
        codeChangeFlag[i] = 0;
        if(i == errbit - 1)
            continue;
        CodeInStatus[i] = CODE_OK;
        CodeBlk[CODE_IN][i].setStyleSheet(okStyle);
    }
    //纠错
    valStr = ui->lnCodeIn->text();
    correctCode(valStr, hamm_bits, errbit);
    ui->lnCodeOut->setText(valStr);

    if(hammGenFlag)
        ui->pbtnFillCodeIn->setEnabled(true);
}

//生成校验结果
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
    status += 2;    //不可按压样式
    setBlkStatus(&(PrRowBlks[pno][hlen+1]), status);

}

//一位校验动画
void HCodeGen::genCheckBlk(int pno)
{
    //过滤非法值
    if(pno < 0 || pno >= parityBits)
        return;
    //串行动画组
    QSequentialAnimationGroup *pSeqGrp = new QSequentialAnimationGroup;
    int n = PrHnoLen[pno] + 1;
    QPropertyAnimation **pMove = new QPropertyAnimation *[n];
    int duration_ms = 1200;
    //海明码区的信息位->校验位的操作数
    QString startStyle, endStyle;   //起点块/终点块样式
    for(int i = 0; i < n; i++){
        int hno = PrHnos[pno][i] - 1;
        startStyle = CodeBlk[CODE_IN][hno].styleSheet();
        if(CodeBlk[CODE_IN][hno].text() == "0"){
            endStyle = pbtnStyle0;
        }
        else {
            endStyle = pbtnStyle1;
        }
        //qDebug() << "add anim" << QString::number(k);
        moveStyle = (i == 0) ? parityStyle : dataStyle;     //移动样式
        //获取动画指针
        pMove[i] = moveBlk(&(CodeBlk[CODE_IN][hno]), &(PrRowBlks[pno][i]), duration_ms, startStyle, endStyle);
        pSeqGrp->addAnimation(pMove[i]);
    }
    //串行动画组结束后，释放动画块及其指针数组，计算校验结果
    connect(pSeqGrp, &QSequentialAnimationGroup::finished, [=](){
        for(int i = 0; i < n; i++)
            delete pMove[i];
        delete []pMove;
        //计算校验结果
        getCheckResult(pno);
        setCheckFinish();    //单步结束
    });
    //启动串行动画组
    pSeqGrp->start(QAbstractAnimation::DeleteWhenStopped);
}

/****************** 数据块移动/更新 ******************/

//更新信息码数据块
void HCodeGen::updateDataBlk(const QString &dataStr)
{
    //更新数据块
    int len = dataStr.length();
    for(int i = 0; i < len; i++){
        if(dataStr[i] == '0'){   //顺序判断
            DataBlk[i].setStyleSheet(pbtnStyle0);
            DataBlk[i].setText("0");
        }
        else {
            DataBlk[i].setStyleSheet(pbtnStyle1);
            DataBlk[i].setText("1");
        }
    }
    //计算新数据串
    QString valStr = dataStr;
    for(int i = len; i < dataBits; i++){
        valStr = valStr + DataBlk[i].text();
    }
    //取消之前的编码高亮显示
    HammingBack converter;
    if(rowChanged >= 0)
        changeCodeRowStyle(rowChanged, STYLE_NORMAL);
    //表格中对应的合法编码高亮显示
    rowChanged = converter.bstring2int(valStr.toStdString());
    changeCodeRowStyle(rowChanged, STYLE_HLIGHT);
}

//移动信息码数据块
void HCodeGen::moveDataBlk(int dno)
{
    //过滤非法值
    if(dno < 0 || dno >= dataBits)
        return;
    //设置起点块/终点块样式
    QString startStyle, endStyle;
    if(DataBlk[dno].text() == "0"){
        startStyle = pbtnStyle0 + hoverStyle + pressStyle;
        endStyle = pbtnStyle0;
    }
    else {
        startStyle = pbtnStyle1 + hoverStyle + pressStyle;
        endStyle = pbtnStyle1;
    }
    //平移动画
    QPropertyAnimation *pMove = nullptr;
    int duration_ms = 800;
    int hno = cvtDno2Hno(dno);
    moveStyle = dataStyle;     //移动样式
    //获取动画指针
    pMove = moveBlk(&(DataBlk[dno]), &(HammBlk[hno]), duration_ms, startStyle, endStyle);
    connect(pMove, &QPropertyAnimation::finished, [=](){
        //设置海明码框
        QString val = ui->lnHammCode->text();
        val[hno] = HammResult.data[dno];
        ui->lnHammCode->setText(val);
        //发送动画结束信号
        emit moveFinished();
    });
    pMove->start(QAbstractAnimation::DeleteWhenStopped);
}

//计算校验位，生成校验位数据块
void HCodeGen::genPrBlk(int pno)
{
    //过滤非法值
    if(pno < 0 || pno >= parityBits)
        return;
    //串行动画组
    QSequentialAnimationGroup *pSeqGrp = new QSequentialAnimationGroup;
    int n = PrHnoLen[pno];
    QPropertyAnimation **pMove = new QPropertyAnimation *[n];
    int duration_ms = 800;
    //海明码区的信息位->校验位的操作数
    QString startStyle, endStyle;   //起点块/终点块样式
    moveStyle = dataStyle;     //移动样式
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
        //获取动画指针
        pMove[i] = moveBlk(&(HammBlk[hno]), &(PrRowBlks[pno][i]), duration_ms, startStyle, endStyle);
        pSeqGrp->addAnimation(pMove[i]);
    }
    //串行动画组结束后，释放动画块及其指针数组，启动结果填入校验区动画
    connect(pSeqGrp, &QSequentialAnimationGroup::finished, [=](){
        for(int i = 0; i < n; i++)
            delete pMove[i];
        delete []pMove;
        //启动结果填入校验区动画
        movePrBlk(pno);
    });
    //启动串行动画组
    pSeqGrp->start(QAbstractAnimation::DeleteWhenStopped);
}

//移动校验码块：校验位区->校验码区
void HCodeGen::movePrBlk(int pno)
{
    //过滤非法值
    if(pno < 0 || pno >= parityBits)
        return;
    //校验位区的计算结果->校验码区的校验块
    QPropertyAnimation *pMove = nullptr;
    int duration_ms = 800;
    int n = PrHnoLen[pno];
    int prStatus = HammResult.check[pno] - '0';
    setBlkStatus(&(PrRowBlks[pno][n]), prStatus); //设置起始块的值，用来给动画块赋值
    //起点块/终点块样式
    QString startStyle, endStyle;
    if(PrRowBlks[pno][n].text() == "0"){
        startStyle = pbtnStyle0;
        endStyle = pbtnStyle0;
    }
    else {
        startStyle = pbtnStyle1;
        endStyle = pbtnStyle1;
    }
    moveStyle = parityStyle;     //移动样式
    //获取动画指针
    pMove = moveBlk(&(PrRowBlks[pno][n]), &(ParityBlk[pno]), duration_ms, startStyle, endStyle);
    connect(pMove, &QPropertyAnimation::finished, [=](){
        //设置校验码框
        QString val = ui->lnPrCode->text();
        val[pno] = HammResult.check[pno];
        ui->lnPrCode->setText(val);
        //发送结束信号
        emit moveFinished();
    });
    //启动动画
    pMove->start(QAbstractAnimation::DeleteWhenStopped);
}

//移动校验码块：校验码区->海明码区
void HCodeGen::moveParityrBlk(int pno)
{
    //过滤非法值
    if(pno < 0 || pno >= parityStyle)
        return;
    //平移动画
    QPropertyAnimation *pMove = nullptr;
    int duration_ms = 800;
    int hno = cvtPno2Hno(pno);
    //设置起点块/终点块样式
    QString startStyle, endStyle;
    if(ParityBlk[pno].text() == "0"){
        startStyle = pbtnStyle0;
        endStyle = pbtnStyle0;
    }
    else {
        startStyle = pbtnStyle1;
        endStyle = pbtnStyle1;
    }
    moveStyle = parityStyle;     //移动样式
    pMove = moveBlk(&(ParityBlk[pno]), &(HammBlk[hno]), duration_ms, startStyle, endStyle);
    connect(pMove, &QPropertyAnimation::finished, [=](){
        //设置海明码框
        QString val = ui->lnHammCode->text();
        val[hno] = HammResult.check[pno];
        ui->lnHammCode->setText(val);
        //发送动画结束信号
        emit moveFinished();
    });
    pMove->start(QAbstractAnimation::DeleteWhenStopped);
}

//移动数据块
QPropertyAnimation *HCodeGen::moveBlk(QPushButton *pStart, QPushButton *pEnd,
                        int duration_ms, QString startStyle, QString endStyle)
{
    QPoint pt_start = pStart->mapTo(this, QPoint(0,0)); //起点
    QPoint pt_end = pEnd->mapTo(this, QPoint(0,0));     //终点
    QString blkText = pStart->text();   //标签
    int blk_w = pStart->width();
    int blk_h = pStart->height();
    //设置动画块
    QPushButton *pBlk = new QPushButton;
    pBlk->setParent(this);
    pBlk->setStyleSheet(moveStyle);
    pBlk->setText(blkText);
    pBlk->setGeometry(pt_start.x(), pt_start.y(), blk_w, blk_h);
    pBlk->show();
    //平移动画
    QPropertyAnimation *pMove = new QPropertyAnimation(pBlk, "pos");
    duration_ms /= speedLevel;      //调整延时
    pMove->setDuration(duration_ms);    //延时
    pMove->setStartValue(pt_start);     //起点
    pMove->setEndValue(pt_end);         //终点
    pMove->setEasingCurve(QEasingCurve::InOutQuad); //动画曲线
    //动画结束后，释放动画块，重置数据块样式
    connect(pMove, &QPropertyAnimation::finished, [=](){
        delete pBlk;
        pStart->setStyleSheet(startStyle);
        pEnd->setText(blkText);
        pEnd->setStyleSheet(endStyle);
    });
    return pMove;
}
