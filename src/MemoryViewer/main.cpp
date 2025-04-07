#include "MemoryViewer.h"

#include <QApplication>
#include <QLocale>
#include <QSettings>
#include <QStyleFactory>
#include <QTranslator>

auto messageHandler = qInstallMessageHandler(nullptr);
void myMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& message) {
    QMessageLogContext& context_ = const_cast<QMessageLogContext&>(context);
    if(context.file) {
        std::string_view file{context.file};
        if(auto last = file.find_last_of(R"(\/)"); std::string_view::npos != last)
            context_.file += last + 1;
    }
    // if(App) {
    //     auto color = [](QtMsgType type) -> QColor {
    //         switch(type) {
    //         case QtCriticalMsg: /**/ return {255, 000, 000};
    //         case QtDebugMsg: /*   */ return {196, 196, 196};
    //         case QtFatalMsg: /*   */ return {255, 000, 000};
    //         case QtInfoMsg: /*    */ return {128, 255, 255};
    //         case QtWarningMsg: /* */ return {255, 128, 000};
    //         }
    //     }(type);
    //     emit App->logging(color, message);
    // }

    // static QRegularExpression re{Ru"(.+38;2;(\d+);(\d+);(\d+)m.+)"_s};
    // QString data{context_.function};
    // data.replace(QRegularExpression(Ru"((\w+\:\:))"_s), "");
    // context_.function = data.toUtf8().data();
    messageHandler(type, context_, message);
};

int main(int argc, char* argv[]) {
    qInstallMessageHandler(myMessageHandler);
    qSetMessagePattern(QLatin1String{
        "%{if-critical}\x1b[38;2;255;0;0m"
        "C %{endif}"
        "%{if-debug}\x1b[38;2;196;196;196m"
        "D %{endif}"
        "%{if-fatal}\x1b[1;38;2;255;0;0m"
        "F %{endif}"
        "%{if-info}\x1b[38;2;128;255;255m"
        "I %{endif}"
        "%{if-warning}\x1b[38;2;255;128;0m"
        "W %{endif}"
        // "%{time HH:mm:ss.zzz} "
        // "%{appname} %{pid} %{threadid} "
        // "%{type} "
        // "%{file}:%{line} %{function} "
        "%{if-category}%{category}%{endif}%{message} "
        "\x1b[38;2;64;64;64m <- %{function} <- %{file} : %{line}\x1b[0m"});

    QApplication a{argc, argv};
    QApplication::setApplicationName(u"MapViewer"_s);
    QApplication::setOrganizationName(u"XrSofr"_s);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for(const QString& locale: uiLanguages) {
        const QString baseName = u"MemoryViewer_"_s + QLocale(locale).name();
        if(translator.load(u":/i18n/"_s + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    QSettings settings;
    settings.beginGroup(u"MemoryViewer"_s);
    MemoryViewer w{settings.value(u"fileNameAndPath"_s).toString()};
    w.show();
    return a.exec();
};
