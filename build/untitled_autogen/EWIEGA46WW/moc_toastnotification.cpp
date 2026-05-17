/****************************************************************************
** Meta object code from reading C++ file 'toastnotification.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../toastnotification.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'toastnotification.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ToastWidget_t {
    QByteArrayData data[6];
    char stringdata0[82];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ToastWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ToastWidget_t qt_meta_stringdata_ToastWidget = {
    {
QT_MOC_LITERAL(0, 0, 11), // "ToastWidget"
QT_MOC_LITERAL(1, 12, 18), // "informationClicked"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 14), // "warningClicked"
QT_MOC_LITERAL(4, 47, 12), // "errorClicked"
QT_MOC_LITERAL(5, 60, 21) // "newChatMessageClicked"

    },
    "ToastWidget\0informationClicked\0\0"
    "warningClicked\0errorClicked\0"
    "newChatMessageClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ToastWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x06 /* Public */,
       3,    0,   35,    2, 0x06 /* Public */,
       4,    0,   36,    2, 0x06 /* Public */,
       5,    0,   37,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ToastWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ToastWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->informationClicked(); break;
        case 1: _t->warningClicked(); break;
        case 2: _t->errorClicked(); break;
        case 3: _t->newChatMessageClicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ToastWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ToastWidget::informationClicked)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ToastWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ToastWidget::warningClicked)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ToastWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ToastWidget::errorClicked)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ToastWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ToastWidget::newChatMessageClicked)) {
                *result = 3;
                return;
            }
        }
    }
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject ToastWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_ToastWidget.data,
    qt_meta_data_ToastWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ToastWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ToastWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ToastWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ToastWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void ToastWidget::informationClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ToastWidget::warningClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ToastWidget::errorClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void ToastWidget::newChatMessageClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
struct qt_meta_stringdata_ToastNotification_t {
    QByteArrayData data[12];
    char stringdata0[194];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ToastNotification_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ToastNotification_t qt_meta_stringdata_ToastNotification = {
    {
QT_MOC_LITERAL(0, 0, 17), // "ToastNotification"
QT_MOC_LITERAL(1, 18, 18), // "informationClicked"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 14), // "warningClicked"
QT_MOC_LITERAL(4, 53, 12), // "errorClicked"
QT_MOC_LITERAL(5, 66, 21), // "newChatMessageClicked"
QT_MOC_LITERAL(6, 88, 12), // "processQueue"
QT_MOC_LITERAL(7, 101, 15), // "updatePositions"
QT_MOC_LITERAL(8, 117, 20), // "onInformationClicked"
QT_MOC_LITERAL(9, 138, 16), // "onWarningClicked"
QT_MOC_LITERAL(10, 155, 14), // "onErrorClicked"
QT_MOC_LITERAL(11, 170, 23) // "onNewChatMessageClicked"

    },
    "ToastNotification\0informationClicked\0"
    "\0warningClicked\0errorClicked\0"
    "newChatMessageClicked\0processQueue\0"
    "updatePositions\0onInformationClicked\0"
    "onWarningClicked\0onErrorClicked\0"
    "onNewChatMessageClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ToastNotification[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x06 /* Public */,
       3,    0,   65,    2, 0x06 /* Public */,
       4,    0,   66,    2, 0x06 /* Public */,
       5,    0,   67,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   68,    2, 0x08 /* Private */,
       7,    0,   69,    2, 0x08 /* Private */,
       8,    0,   70,    2, 0x08 /* Private */,
       9,    0,   71,    2, 0x08 /* Private */,
      10,    0,   72,    2, 0x08 /* Private */,
      11,    0,   73,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ToastNotification::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ToastNotification *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->informationClicked(); break;
        case 1: _t->warningClicked(); break;
        case 2: _t->errorClicked(); break;
        case 3: _t->newChatMessageClicked(); break;
        case 4: _t->processQueue(); break;
        case 5: _t->updatePositions(); break;
        case 6: _t->onInformationClicked(); break;
        case 7: _t->onWarningClicked(); break;
        case 8: _t->onErrorClicked(); break;
        case 9: _t->onNewChatMessageClicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ToastNotification::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ToastNotification::informationClicked)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ToastNotification::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ToastNotification::warningClicked)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ToastNotification::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ToastNotification::errorClicked)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ToastNotification::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ToastNotification::newChatMessageClicked)) {
                *result = 3;
                return;
            }
        }
    }
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject ToastNotification::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ToastNotification.data,
    qt_meta_data_ToastNotification,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ToastNotification::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ToastNotification::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ToastNotification.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ToastNotification::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void ToastNotification::informationClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ToastNotification::warningClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ToastNotification::errorClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void ToastNotification::newChatMessageClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
