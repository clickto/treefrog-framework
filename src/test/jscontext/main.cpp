#include <TfTest/TfTest>
#include <QJSEngine>
#include <QJSValue>
#include "../../tjscontext.h"
#include "../../tjsinstance.h"
#include "../../tjsloader.h"
#include "../../treactcomponent.h"


class JSContext : public QObject
{
    Q_OBJECT
    QString jsxTransform(const QString &jsx);
    QString jsxTransformFile(const QString &file);

private slots:
    void initTestCase();
    void eval_data();
    void eval();

#if QT_VERSION > 0x050400
    void callAsConstructor_data();
    void callAsConstructor();
    void require_data();
    void require();
    void callFunc_data();
    void callFunc();
    void callFunc1_data();
    void callFunc1();
    void transform_data();
    void transform();
    void load_data();
    void load();
    void react_data();
    void react();
    void reactjsx_data();
    void reactjsx();
    void reactjsxCommonJs_data();
    void reactjsxCommonJs();
    void reactComponent_data();
    void reactComponent();
    void benchmark();
#endif
};


void JSContext::initTestCase()
{
    TJSContext::setSearchPaths({"."});
}


void JSContext::eval_data()
{
    QTest::addColumn<QString>("expr");
    QTest::addColumn<QString>("output");

    QTest::newRow("01") << "1 + 2" << "3";
    QTest::newRow("02") << "'hello' + ' world!'" << "hello world!";
    QTest::newRow("03") << "(function(a){return a*a;})(5)" << "25";
    QTest::newRow("04") << "(function(s){return s+s;})('orange')" << "orangeorange";
}


void JSContext::eval()
{
    QFETCH(QString, expr);
    QFETCH(QString, output);

    TJSContext js;
    auto result = js.evaluate(expr).toString();
    QCOMPARE(result, output);
}

#if QT_VERSION > 0x050400

void JSContext::callAsConstructor_data()
{
    QTest::addColumn<QStringList>("modules");
    QTest::addColumn<QString>("className");
    QTest::addColumn<QString>("arg");
    QTest::addColumn<QString>("method");
    QTest::addColumn<QString>("methodArg");
    QTest::addColumn<QString>("output");

    QTest::newRow("01") << QStringList({"./js/hello"}) << "Hello" << "Taro" << "hello" << QString()
                        << "My name is Taro";
    QTest::newRow("02") << QStringList({"./js/hello"}) << "Hello" << QString() << "hello" << QString()
                        << "My name is ";
}


void JSContext::callAsConstructor()
{
    QFETCH(QStringList, modules);
    QFETCH(QString, className);
    QFETCH(QString, arg);
    QFETCH(QString, method);
    QFETCH(QString, methodArg);
    QFETCH(QString, output);

    TJSContext::setSearchPaths({"."});
    TJSContext js(modules);
    auto instance = js.callAsConstructor(className, arg);
    QCOMPARE(instance.isError(), false);
    auto result = instance.call(method, methodArg).toString();
    QCOMPARE(result, output);
}


void JSContext::require_data()
{
    QTest::addColumn<QString>("module");
    QTest::addColumn<QString>("className");
    QTest::addColumn<QString>("arg");
    QTest::addColumn<QString>("method");
    QTest::addColumn<QString>("methodArg");
    QTest::addColumn<QString>("output");

    QTest::newRow("01") << "./js/mobile-detect" << "MobileDetect" << "Mozilla/5.0 (Linux; U; Android 4.0.3; en-in; SonyEricssonMT11i Build/4.1.A.0.562) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 Mobile Safari/534.30"
                        << "version" << "Android"
                        << "4.03";
}


void JSContext::require()
{
    QFETCH(QString, module);
    QFETCH(QString, className);
    QFETCH(QString, arg);
    QFETCH(QString, method);
    QFETCH(QString, methodArg);
    QFETCH(QString, output);

    TJSContext::setSearchPaths({"."});
    TJSContext js;
    js.import(className, module);
    auto instance = js.callAsConstructor(className, arg);
    QCOMPARE(instance.isError(), false);
    auto result = instance.call(method, methodArg).toString();
    QCOMPARE(result, output);
}


void JSContext::callFunc_data()
{
    QTest::addColumn<QString>("func");
    QTest::addColumn<QString>("output");

    QTest::newRow("01") << "function(){return 1+2;}" << "3";
    QTest::newRow("02") << "function(){return 'hello'+' world!';}" << "hello world!";
    QTest::newRow("03") << "function(){return \"hello\"+\" world.\";}" << "hello world.";
}


void JSContext::callFunc()
{
    QFETCH(QString, func);
    QFETCH(QString, output);

    TJSContext js;
    auto result = js.call(func).toString();
    QCOMPARE(result, output);
}

void JSContext::callFunc1_data()
{
    QTest::addColumn<QString>("func");
    QTest::addColumn<QString>("arg");
    QTest::addColumn<QString>("output");

    QTest::newRow("01") << "function(a){return a*a;}" << "4" << "16";
    QTest::newRow("02") << "function(s){return 'Hello '+s;}" << "world" << "Hello world";
}


void JSContext::callFunc1()
{
    QFETCH(QString, func);
    QFETCH(QString, arg);
    QFETCH(QString, output);

    TJSContext js;
    auto result = js.call(func, arg).toString();
    QCOMPARE(result, output);
}


void JSContext::transform_data()
{
    QTest::addColumn<QString>("jsx");
    QTest::addColumn<QString>("output");

    QTest::newRow("01") << "<HelloWorld />" << "React.createElement(HelloWorld, null)";
    QTest::newRow("02") << "<HelloWorld/>" << "React.createElement(HelloWorld, null)";
    QTest::newRow("03") << "<hello/>" << "React.createElement(\"hello\", null)";
}


void JSContext::transform()
{
    QFETCH(QString, jsx);
    QFETCH(QString, output);

    TJSContext js;
    js.import("JSXTransformer", "JSXTransformer");
    auto result = js.call("JSXTransformer.transform", QJSValue(jsx)).property("code").toString();
    QCOMPARE(result, output);
}


void JSContext::benchmark()
{
    TJSContext js;
    js.import("JSXTransformer", "JSXTransformer");

    QBENCHMARK {
        auto res = js.call("JSXTransformer.transform", QString("<HelloWorld />"));
        //qDebug() << res.property("code").toString();
    }
}


void JSContext::load_data()
{
    QTest::addColumn<QStringList>("files");
    QTest::addColumn<QString>("variable");
    QTest::addColumn<QString>("result");

    QTest::newRow("01") << QStringList({"./js/main.js"}) << "sub('world')" << "Hello world";
    QTest::newRow("02") << QStringList({"./js/main.js"}) << "sub2('world')" << "Hello world";
    QTest::newRow("03") << QStringList({"./js/main.js"}) << "sub2('世界', 'ja')" << tr("こんにちは 世界");
}


void JSContext::load()
{
    QFETCH(QStringList, files);
    QFETCH(QString, variable);
    QFETCH(QString, result);

    TJSContext::setSearchPaths({"."});
    TJSContext js(files);  // commonJs mode
    QString output = js.evaluate(variable).toString();
    qDebug() << qPrintable(output);
    QCOMPARE(output, result);
}


void JSContext::react_data()
{
    QTest::addColumn<QString>("variable");
    QTest::addColumn<QString>("result");

    QTest::newRow("01") << "JSXTransformer.transform('<HelloWorld />')['code']"
                        << "React.createElement(HelloWorld, null)";
    QTest::newRow("02") << "ReactDOMServer.renderToString(React.createElement('div'))"
                        << "<div data-reactroot=\"\" data-reactid=\"1\" data-react-checksum=\"2058293082\"></div>";
}


void JSContext::react()
{
    QFETCH(QString, variable);
    QFETCH(QString, result);

    TJSContext::setSearchPaths({"."});
    TJSContext js;
    js.import("JSXTransformer", "JSXTransformer");
    js.import("React", "react");
    js.import("ReactDOMServer", "react-dom-server");

    QString output = js.evaluate(variable).toString();
    QCOMPARE(output, result);
}

void JSContext::reactjsx_data()
{
    QTest::addColumn<QString>("jsxfile");
    QTest::addColumn<QString>("func");
    QTest::addColumn<QString>("result");

    QTest::newRow("01") << QString()
                        << "ReactDOMServer.renderToString(<div/>)"
                        << "<div data-reactroot=\"\" data-reactid=\"1\" data-react-checksum=\"2058293082\"></div>";
    QTest::newRow("02") << "./js/react_samlple.jsx"
                        << "ReactDOMServer.renderToString(React.createElement(MyComponent, null))"
                        << "<div data-reactroot=\"\" data-reactid=\"1\" data-react-checksum=\"1078334326\">Hello World</div>";
    QTest::newRow("03") << "./js/react_samlple.jsx"
                        << "ReactDOMServer.renderToString(<MyComponent/>)"
                        << "<div data-reactroot=\"\" data-reactid=\"1\" data-react-checksum=\"1078334326\">Hello World</div>";
    QTest::newRow("04") << QString()
                        << "ReactDOMServer.renderToString(<ReactBootstrap.Button/>)"
                        << "<button class=\"btn btn-default\" type=\"button\" data-reactroot=\"\" data-reactid=\"1\" data-react-checksum=\"-1068949636\"></button>";
    QTest::newRow("05") << QString("js/react_bootstrap_sample.jsx")
                        << "ReactDOMServer.renderToString(<Button/>)"
                        << "<button class=\"btn btn-default\" type=\"button\" data-reactroot=\"\" data-reactid=\"1\" data-react-checksum=\"-1068949636\"></button>";
    QTest::newRow("06") << QString("js/react_bootstrap_sample.jsx")
                        << "ReactDOMServer.renderToString(SimpleButton)"
                        << "<button class=\"btn btn-default\" type=\"button\" data-reactroot=\"\" data-reactid=\"1\" data-react-checksum=\"-2012470818\">Sample</button>";
    QTest::newRow("07") << QString("js/react_bootstrap_sample.jsx")
                        << "ReactDOMServer.renderToString(HelloButton)"
                        << "<button class=\"btn btn-lg btn-primary\" type=\"button\" data-reactroot=\"\" data-reactid=\"1\" data-react-checksum=\"1572807667\">Hello</button>";
}


void JSContext::reactjsx()
{
    QFETCH(QString, jsxfile);
    QFETCH(QString, func);
    QFETCH(QString, result);

    TJSContext::setSearchPaths({"."});
    auto *js = TJSLoader().loadJSModule("reactmodule");

    // Loads JSX
    if (!jsxfile.isEmpty()) {
        js->evaluate(jsxTransformFile(jsxfile), jsxfile);
    }
    QString fn = jsxTransform(func);
    QString output = js->evaluate(fn).toString();
    QCOMPARE(output, result);
}


void JSContext::reactjsxCommonJs_data()
{
    QTest::addColumn<QString>("jsfile");
    QTest::addColumn<QString>("result");

    QTest::newRow("01") << "./js/react_bootstrap_require.js"
                        << "<button class=\"btn btn-default\" type=\"button\" data-reactroot=\"\" data-reactid=\"1\" data-react-checksum=\"-1068949636\"></button>";
    QTest::newRow("02") << "./js/react_bootstrap_require2.js"
                        << "<div class=\"commentBox\" data-reactroot=\"\" data-reactid=\"1\" data-react-checksum=\"1960517848\">Hello, world! I am a CommentBox.</div>";
}


void JSContext::reactjsxCommonJs()
{
    QFETCH(QString, jsfile);
    QFETCH(QString, result);

    TJSContext::setSearchPaths({"."});
    TJSContext js;

    // Loads JSX
    QString output = js.import(jsfile).toString();
    QCOMPARE(output, result);
}


void JSContext::reactComponent_data()
{
    QTest::addColumn<QString>("jsxfile");
    QTest::addColumn<QString>("component");
    QTest::addColumn<QString>("result");

    QTest::newRow("01") << "js/react_samlple.jsx" << "<MyComponent/>"
                        << "<div data-reactroot=\"\" data-reactid=\"1\" data-react-checksum=\"1078334326\">Hello World</div>";
    QTest::newRow("02") << "js/react_bootstrap_sample.jsx" << "<Button/>"
                        << "<button class=\"btn btn-default\" type=\"button\" data-reactroot=\"\" data-reactid=\"1\" data-react-checksum=\"-1068949636\"></button>";
    QTest::newRow("03") << "js/react_bootstrap_sample.jsx" << "SimpleButton"
                        << "<button class=\"btn btn-default\" type=\"button\" data-reactroot=\"\" data-reactid=\"1\" data-react-checksum=\"-2012470818\">Sample</button>";
    QTest::newRow("04") << "js/react_bootstrap_sample.jsx" << "HelloButton"
                        << "<button class=\"btn btn-lg btn-primary\" type=\"button\" data-reactroot=\"\" data-reactid=\"1\" data-react-checksum=\"1572807667\">Hello</button>";
}


void JSContext::reactComponent()
{
    QFETCH(QString, jsxfile);
    QFETCH(QString, component);
    QFETCH(QString, result);

    TJSContext::setSearchPaths({"../../../defaults", "."});
    TReactComponent comp;
    comp.import("ReactBootstrap", "react-bootstrap");
    comp.import(jsxfile);

    // Loads JSX
    QString output = comp.renderToString(component);
    QCOMPARE(output, result);
}

#endif

QString JSContext::jsxTransform(const QString &jsx)
{
    auto val = TJSLoader().loadJSModule("Jtf", "JSXTransformer")->call("Jtf.transform", jsx);
    return val.property("code").toString();
}

QString JSContext::jsxTransformFile(const QString &file)
{
    QFile script(file);
    if (!script.open(QIODevice::ReadOnly)) {
        // open error
        printf("open error: %s\n", qPrintable(file));
        return QString();
    }

    QTextStream stream(&script);
    QString contents = stream.readAll();
    script.close();
    return jsxTransform(contents);
}

TF_TEST_MAIN(JSContext)
#include "main.moc"
