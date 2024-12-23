#ifndef CFGEDITFRAME_H
#define CFGMEDITFRAME_H

#include <QFrame>

#include "data.h"

class QLayout;
class QLabel;
class QLineEdit;
class QComboBox;
class QCheckBox;

namespace SGDK
{

class cfgEditFrame : public QFrame
{
  Q_OBJECT

  private:


    QLineEdit *edt_arch_dir;
    void setup_arch_dir (QLayout *L);

    QLineEdit *edt_update_timer_interval_normal;
    void setup_update_timer_interval_normal (QLayout *L);

    QLineEdit *edt_update_timer_interval_max;
    void setup_update_timer_interval_max (QLayout *L);

    QLineEdit *edt_max_arhive_nomb;
    void setup_max_arhive_nomb (QLayout *L);

    QLineEdit *edt_etalon_com_port_nomb;
    void setup_etalon_com_port_nomb(QLayout *L);

    QLineEdit *edt_min_dac_value;
    void setup_min_dac_value(QLayout *L);

    QLineEdit *edt_max_dac_value;
    void setup_max_dac_value(QLayout *L);

    QLineEdit *edt_recomended_min_dac_value;
    void setup_recomended_min_dac_value(QLayout *L);

    QLineEdit *edt_recomended_max_dac_value;
    void setup_recomended_max_dac_value(QLayout *L);

  public:
    cfgEditFrame(QWidget *parent = 0);
    virtual ~cfgEditFrame();

    void attach (  ) ;
    void save ( void ) ;  // должна снимать данные с фрейма и сохранять в структуре

    bool isValid() const;  // проверка валидности данных

};

} // namespace SGDK
#endif // ITEMEDITFRAME_H
