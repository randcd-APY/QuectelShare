#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
//#include <QTextCodec>

#include "ModelManager.h"
#include "ObjectModel.h"

int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

//    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    qmlRegisterUncreatableType<ObjectModel, 1>("com.quectel.model", 1, 0,
                                               "ObjectModel",
                                               "Cannot create ObjectModel");
//    QQmlApplicationEngine engine;

//    ModelManager* modelMgr = new ModelManager(&app);
//    engine.rootContext()->setContextProperty("modelMgr", modelMgr);

//    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
//    if (engine.rootObjects().isEmpty())
//        return -1;

//    // 初始化数据
//    modelMgr->initData();

//    int r = app.exec();

//    if (modelMgr) {
//        modelMgr->deleteLater();
//        modelMgr = nullptr;
//    }

//    return r;

    qmlRegisterType<ModelManager>("ModelManager", 1, 0, "ModelManager");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
