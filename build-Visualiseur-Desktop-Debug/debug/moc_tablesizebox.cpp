/****************************************************************************
** Meta object code from reading C++ file 'tablesizebox.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Visualiseur/tablesizebox.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tablesizebox.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_TableSizeBox_t {
    QByteArrayData data[9];
    char stringdata[77];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_TableSizeBox_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_TableSizeBox_t qt_meta_stringdata_TableSizeBox = {
    {
QT_MOC_LITERAL(0, 0, 12),
QT_MOC_LITERAL(1, 13, 15),
QT_MOC_LITERAL(2, 29, 0),
QT_MOC_LITERAL(3, 30, 3),
QT_MOC_LITERAL(4, 34, 3),
QT_MOC_LITERAL(5, 38, 3),
QT_MOC_LITERAL(6, 42, 3),
QT_MOC_LITERAL(7, 46, 10),
QT_MOC_LITERAL(8, 57, 18)
    },
    "TableSizeBox\0sendTableValues\0\0p1X\0p1Y\0"
    "p2X\0p2Y\0viewValues\0checkAndSendValues\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TableSizeBox[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    4,   29,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
       7,    0,   38,    2, 0x0a,
       8,    0,   39,    2, 0x0a,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,    3,    4,    5,    6,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void TableSizeBox::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TableSizeBox *_t = static_cast<TableSizeBox *>(_o);
        switch (_id) {
        case 0: _t->sendTableValues((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 1: _t->viewValues(); break;
        case 2: _t->checkAndSendValues(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (TableSizeBox::*_t)(int , int , int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TableSizeBox::sendTableValues)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject TableSizeBox::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_TableSizeBox.data,
      qt_meta_data_TableSizeBox,  qt_static_metacall, 0, 0}
};


const QMetaObject *TableSizeBox::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TableSizeBox::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TableSizeBox.stringdata))
        return static_cast<void*>(const_cast< TableSizeBox*>(this));
    return QWidget::qt_metacast(_clname);
}

int TableSizeBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void TableSizeBox::sendTableValues(int _t1, int _t2, int _t3, int _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
