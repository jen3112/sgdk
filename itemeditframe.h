#ifndef ITEMEDITFRAME_H
#define ITEMEDITFRAME_H

#include <QFrame>

#include "data.h"

class QLayout;
class QLabel;
class QLineEdit;
class QComboBox;
class QCheckBox;

namespace SGDK
{

class itemEditFrame : public QFrame
{
  Q_OBJECT

  private:
    DATA::BoardCfg *Dt ;


    QLabel    *lblBrdNomb;
    QLabel    *lblSide;
    QLineEdit *edtBrdNomb;
    void setup_BrdNomb(QLayout *L);

    QComboBox *cbxKind;

    QLineEdit *edtIp ;
    void setup_Ip(QLayout *L);

    QLineEdit *edtPort ;
    void setup_Port (QLayout *L);

    QCheckBox *cbxState;
    void setup_State (QLayout *L);
    QCheckBox *cbxProcess;
    void setup_Process (QLayout *L);


    QCheckBox *cbx_wrwheel;
    void setup_wrwheel (QLayout *L);
    QCheckBox *cbx_wrzero;
    void setup_wrzeroes (QLayout *L);

    QCheckBox *cbx_isa_realtime;
    void setup_isa_real_time (QLayout *L);

    QLineEdit *edtBin;
    QLineEdit *edtCsv;
    QLineEdit *edtIsa;
    QLineEdit *edtIsaProgDir;
    QLineEdit *edtIsaProgNomb;
    QLineEdit *edt_max_timeout;
    QCheckBox *cbxBin;
    QCheckBox *cbxCsv;
    QCheckBox *cbxIsa;
    QLineEdit *edtDatchikDistance;
    QLineEdit *edtSide;
    QLineEdit *edtPower;
    QLineEdit *edtKvantToWeigth;
    QLineEdit *edtIsaPause;

    QComboBox *combo_box_mode;

    void setup_Side(QLayout *L);


  public:
    itemEditFrame(QWidget *parent = 0);
    virtual ~itemEditFrame();

    void attach ( DATA::BoardCfg *X ) ;
    void save ( void ) ;  // должна снимать данные с фрейма и сохранять в структуре

    bool isValid() const;  // проверка валидности данных

};

} // namespace SGDK
#endif // ITEMEDITFRAME_H
