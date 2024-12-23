/****************************************************************************
** Meta object code from reading C++ file 'graph.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../graph.h"
#include <QtGui/qtextcursor.h>
#include <QScreen>
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'graph.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SGDK__graph_t {
    const uint offsetsAndSize[26];
    char stringdata0[211];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_SGDK__graph_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_SGDK__graph_t qt_meta_stringdata_SGDK__graph = {
    {
QT_MOC_LITERAL(0, 11), // "SGDK::graph"
QT_MOC_LITERAL(12, 11), // "start_graph"
QT_MOC_LITERAL(24, 0), // ""
QT_MOC_LITERAL(25, 18), // "update_graph_timer"
QT_MOC_LITERAL(44, 19), // "gr_check_box_change"
QT_MOC_LITERAL(64, 11), // "metrol_zero"
QT_MOC_LITERAL(76, 10), // "metrol_fix"
QT_MOC_LITERAL(87, 11), // "metrol_auto"
QT_MOC_LITERAL(99, 11), // "metrol_next"
QT_MOC_LITERAL(111, 11), // "metrol_save"
QT_MOC_LITERAL(123, 21), // "metr_check_box_change"
QT_MOC_LITERAL(145, 31), // "metrol_interval_time_is_changed"
QT_MOC_LITERAL(177, 33) // "metrol_interval_weigth_is_cha..."

    },
    "SGDK::graph\0start_graph\0\0update_graph_timer\0"
    "gr_check_box_change\0metrol_zero\0"
    "metrol_fix\0metrol_auto\0metrol_next\0"
    "metrol_save\0metr_check_box_change\0"
    "metrol_interval_time_is_changed\0"
    "metrol_interval_weigth_is_changed"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SGDK__graph[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   80,    2, 0x0a,    1 /* Public */,
       3,    0,   81,    2, 0x0a,    2 /* Public */,
       4,    0,   82,    2, 0x0a,    3 /* Public */,
       5,    0,   83,    2, 0x0a,    4 /* Public */,
       6,    0,   84,    2, 0x0a,    5 /* Public */,
       7,    0,   85,    2, 0x0a,    6 /* Public */,
       8,    0,   86,    2, 0x0a,    7 /* Public */,
       9,    0,   87,    2, 0x0a,    8 /* Public */,
      10,    0,   88,    2, 0x0a,    9 /* Public */,
      11,    0,   89,    2, 0x0a,   10 /* Public */,
      12,    0,   90,    2, 0x0a,   11 /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void SGDK::graph::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<graph *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->start_graph(); break;
        case 1: _t->update_graph_timer(); break;
        case 2: _t->gr_check_box_change(); break;
        case 3: _t->metrol_zero(); break;
        case 4: _t->metrol_fix(); break;
        case 5: _t->metrol_auto(); break;
        case 6: _t->metrol_next(); break;
        case 7: _t->metrol_save(); break;
        case 8: _t->metr_check_box_change(); break;
        case 9: _t->metrol_interval_time_is_changed(); break;
        case 10: _t->metrol_interval_weigth_is_changed(); break;
        default: ;
        }
    }
    (void)_a;
}

const QMetaObject SGDK::graph::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_SGDK__graph.offsetsAndSize,
    qt_meta_data_SGDK__graph,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_SGDK__graph_t
, QtPrivate::TypeAndForceComplete<graph, std::true_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>


>,
    nullptr
} };


const QMetaObject *SGDK::graph::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SGDK::graph::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SGDK__graph.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int SGDK::graph::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 11;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
