#ifndef EDIT_H
#define EDIT_H

#include <QTextEdit>

namespace SGDK {



class edit : public QTextEdit
{
    Q_OBJECT
public:
    edit(QWidget *parent = 0);
    virtual ~edit();
};

} // namespace SGDK

#endif // EDIT_H
