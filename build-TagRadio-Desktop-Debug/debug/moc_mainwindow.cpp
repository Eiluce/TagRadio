/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Visualiseur/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[28];
    char stringdata[259];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_MainWindow_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10),
QT_MOC_LITERAL(1, 11, 15),
QT_MOC_LITERAL(2, 27, 0),
QT_MOC_LITERAL(3, 28, 4),
QT_MOC_LITERAL(4, 33, 4),
QT_MOC_LITERAL(5, 38, 4),
QT_MOC_LITERAL(6, 43, 12),
QT_MOC_LITERAL(7, 56, 15),
QT_MOC_LITERAL(8, 72, 2),
QT_MOC_LITERAL(9, 75, 2),
QT_MOC_LITERAL(10, 78, 2),
QT_MOC_LITERAL(11, 81, 2),
QT_MOC_LITERAL(12, 84, 19),
QT_MOC_LITERAL(13, 104, 12),
QT_MOC_LITERAL(14, 117, 14),
QT_MOC_LITERAL(15, 132, 18),
QT_MOC_LITERAL(16, 151, 15),
QT_MOC_LITERAL(17, 167, 15),
QT_MOC_LITERAL(18, 183, 3),
QT_MOC_LITERAL(19, 187, 3),
QT_MOC_LITERAL(20, 191, 3),
QT_MOC_LITERAL(21, 195, 3),
QT_MOC_LITERAL(22, 199, 9),
QT_MOC_LITERAL(23, 209, 8),
QT_MOC_LITERAL(24, 218, 12),
QT_MOC_LITERAL(25, 231, 7),
QT_MOC_LITERAL(26, 239, 4),
QT_MOC_LITERAL(27, 244, 13)
    },
    "MainWindow\0pointProbValues\0\0posX\0posY\0"
    "prob\0goToNextDate\0startWithValues\0aX\0"
    "aY\0tX\0tY\0sendPointProbValues\0setPointProb\0"
    "resetPositions\0readMatrixFromFile\0"
    "changeTableSize\0modifyTableSize\0p1X\0"
    "p1Y\0p2X\0p2Y\0pauseRead\0stopRead\0"
    "fileFinished\0setDate\0date\0clickNextDate\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   84,    2, 0x06,
       6,    0,   91,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
       7,    4,   92,    2, 0x0a,
      12,    0,  101,    2, 0x0a,
      13,    3,  102,    2, 0x0a,
      14,    0,  109,    2, 0x0a,
      15,    0,  110,    2, 0x0a,
      16,    0,  111,    2, 0x0a,
      17,    4,  112,    2, 0x0a,
      22,    0,  121,    2, 0x0a,
      23,    0,  122,    2, 0x0a,
      24,    0,  123,    2, 0x0a,
      25,    1,  124,    2, 0x0a,
      27,    0,  127,    2, 0x0a,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Double,    3,    4,    5,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,    8,    9,   10,   11,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Double,    3,    4,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,   18,   19,   20,   21,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   26,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        switch (_id) {
        case 0: _t->pointProbValues((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3]))); break;
        case 1: _t->goToNextDate(); break;
        case 2: _t->startWithValues((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 3: _t->sendPointProbValues(); break;
        case 4: _t->setPointProb((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3]))); break;
        case 5: _t->resetPositions(); break;
        case 6: _t->readMatrixFromFile(); break;
        case 7: _t->changeTableSize(); break;
        case 8: _t->modifyTableSize((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 9: _t->pauseRead(); break;
        case 10: _t->stopRead(); break;
        case 11: _t->fileFinished(); break;
        case 12: _t->setDate((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 13: _t->clickNextDate(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (MainWindow::*_t)(int , int , double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::pointProbValues)) {
                *result = 0;
            }
        }
        {
            typedef void (MainWindow::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::goToNextDate)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, 0, 0}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::pointProbValues(int _t1, int _t2, double _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MainWindow::goToNextDate()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
