/****************************************************************************
** Meta object code from reading C++ file 'reader.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Visualiseur/reader.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'reader.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Reader_t {
    QByteArrayData data[12];
    char stringdata[88];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_Reader_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_Reader_t qt_meta_stringdata_Reader = {
    {
QT_MOC_LITERAL(0, 0, 6),
QT_MOC_LITERAL(1, 7, 8),
QT_MOC_LITERAL(2, 16, 0),
QT_MOC_LITERAL(3, 17, 13),
QT_MOC_LITERAL(4, 31, 9),
QT_MOC_LITERAL(5, 41, 4),
QT_MOC_LITERAL(6, 46, 4),
QT_MOC_LITERAL(7, 51, 4),
QT_MOC_LITERAL(8, 56, 9),
QT_MOC_LITERAL(9, 66, 8),
QT_MOC_LITERAL(10, 75, 4),
QT_MOC_LITERAL(11, 80, 6)
    },
    "Reader\0finished\0\0workRequested\0sendPoint\0"
    "posX\0posY\0prob\0sendReset\0sendDate\0"
    "date\0doWork\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Reader[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x06,
       3,    0,   45,    2, 0x06,
       4,    3,   46,    2, 0x06,
       8,    0,   53,    2, 0x06,
       9,    1,   54,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
      11,    0,   57,    2, 0x0a,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Double,    5,    6,    7,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   10,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void Reader::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Reader *_t = static_cast<Reader *>(_o);
        switch (_id) {
        case 0: _t->finished(); break;
        case 1: _t->workRequested(); break;
        case 2: _t->sendPoint((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3]))); break;
        case 3: _t->sendReset(); break;
        case 4: _t->sendDate((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->doWork(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Reader::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Reader::finished)) {
                *result = 0;
            }
        }
        {
            typedef void (Reader::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Reader::workRequested)) {
                *result = 1;
            }
        }
        {
            typedef void (Reader::*_t)(int , int , double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Reader::sendPoint)) {
                *result = 2;
            }
        }
        {
            typedef void (Reader::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Reader::sendReset)) {
                *result = 3;
            }
        }
        {
            typedef void (Reader::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Reader::sendDate)) {
                *result = 4;
            }
        }
    }
}

const QMetaObject Reader::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Reader.data,
      qt_meta_data_Reader,  qt_static_metacall, 0, 0}
};


const QMetaObject *Reader::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Reader::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Reader.stringdata))
        return static_cast<void*>(const_cast< Reader*>(this));
    return QObject::qt_metacast(_clname);
}

int Reader::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void Reader::finished()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void Reader::workRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void Reader::sendPoint(int _t1, int _t2, double _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Reader::sendReset()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void Reader::sendDate(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
