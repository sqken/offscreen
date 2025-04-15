#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQuickItem>
#include <QQuickWindow>
#include <QImage>
#include <QDir>
#include <QTimer>
#include <QDebug>
#include <QQmlContext>
#include <QFile>
#include <QQuickRenderControl>
#include <QOffscreenSurface>
#include <QDateTime>
#include <QFontDatabase>

// 加载字体函数
void loadFont() {
    QString path = ":/assets_0/Component/DriodSansFallback.ttf";
    int id = QFontDatabase::addApplicationFont(path);
    if (id == -1) {
        qWarning() << "Error loading the font:" << path;
        return;
    }
    QStringList families = QFontDatabase::applicationFontFamilies(id);
    if (!families.isEmpty()) {
        qDebug() << "set default font" << path;
        QGuiApplication::setFont(QFont(families.at(0)));
    }
}

class OffscreenRenderer : public QObject
{
    Q_OBJECT

public:
    OffscreenRenderer(QObject *parent = nullptr)
        : QObject(parent), m_pendingSave(false)
    {
        // 创建QML引擎
        m_engine = new QQmlEngine(this);
        
        // 检查资源文件
        QFile imageFile(":/assets_0/Component/safe_belt.png");
        if (imageFile.exists()) {
            qDebug() << "图片文件存在:" << imageFile.fileName();
        } else {
            qWarning() << "图片文件不存在:" << imageFile.fileName();
        }
        
        // 加载QML
        QQmlComponent component(m_engine, QUrl(QStringLiteral("qrc:/main.qml")));
        if (component.isError()) {
            for (const QQmlError &error : component.errors()) {
                qWarning() << "QML错误:" << error.toString();
            }
            return;
        }
        
        // 创建顶级窗口
        m_window = new QQuickWindow();
        m_window->setWidth(800);
        m_window->setHeight(600);
        m_window->setColor(Qt::white);
        
        // 创建根项目
        m_rootItem = qobject_cast<QQuickItem*>(component.create());
        if (!m_rootItem) {
            qWarning() << "创建根项目失败";
            if (component.isError()) {
                for (const QQmlError &error : component.errors()) {
                    qWarning() << "QML创建错误:" << error.toString();
                }
            }
            return;
        }
        
        qDebug() << "QML根项目创建成功";
        
        // 设置尺寸和父项
        m_rootItem->setParentItem(m_window->contentItem());
        m_rootItem->setWidth(m_window->width());
        m_rootItem->setHeight(m_window->height());
        
        // 使窗口可见但不显示
        m_window->setVisibility(QWindow::Hidden);
        
        // 连接QML中的内容变动信号
        QObject::connect(m_rootItem, SIGNAL(contentChanged()), 
                         this, SLOT(onContentChanged()));
        
        // 创建防抖定时器，用于避免过于频繁的保存
        m_debounceTimer = new QTimer(this);
        m_debounceTimer->setSingleShot(true);
        connect(m_debounceTimer, &QTimer::timeout, this, &OffscreenRenderer::render);
        
        // 立即执行一次渲染，确保初始状态被保存
        QTimer::singleShot(100, this, &OffscreenRenderer::render);
    }
    
    ~OffscreenRenderer() {
        delete m_rootItem;
        delete m_window;
    }

public slots:
    // 当QML内容变动时调用
    void onContentChanged() {
        qDebug() << "内容已变动，计划渲染...";
        
        // 设置待保存标志
        m_pendingSave = true;
        
        // 启动防抖定时器，100ms内多次变动只触发一次渲染
        if (!m_debounceTimer->isActive()) {
            m_debounceTimer->start(100);  // 100ms 防抖时间
        }
    }
    
    void render() {
        if (!m_rootItem || !m_window) {
            qWarning() << "渲染失败：根项目或窗口为空";
            return;
        }
        
        // 如果没有待保存的内容，则不进行保存
        if (!m_pendingSave) {
            return;
        }
        
        qDebug() << "开始渲染...";
        
        // 确保窗口更新
        m_window->update();
        
        // 让QML引擎处理待处理的操作
        QCoreApplication::processEvents();
        
        // 以下是关键操作，使用QQuickWindow内置的抓取功能
        QImage image = m_window->grabWindow();
        
        // 确保图像不为空
        if (image.isNull()) {
            qWarning() << "生成的图像为空!";
            return;
        }
        
        qDebug() << "图像获取成功，大小:" << image.size() << "格式:" << image.format();
        
        // 获取当前时间作为日志信息
        QString timeStr = QDateTime::currentDateTime().toString("HH:mm:ss.zzz");
        
        // 保存为BMP文件
        QString savePath = QDir::currentPath() + "/offscreen_output.bmp";
        if (image.save(savePath, "BMP")) {
            qDebug() << timeStr << "图像保存成功:" << savePath;
            // 重置待保存标志
            m_pendingSave = false;
        } else {
            qWarning() << timeStr << "保存图像失败:" << savePath;
        }
    }
    
private:
    QQmlEngine *m_engine;
    QQuickWindow *m_window;
    QQuickItem *m_rootItem;
    QTimer *m_debounceTimer;
    bool m_pendingSave;  // 标记是否有待保存的内容
};

int main(int argc, char *argv[])
{
    // 启用高DPI支持
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    
    QGuiApplication app(argc, argv);
    
    // 加载字体
    loadFont();
    
    // 获取当前目录
    qDebug() << "当前工作目录:" << QDir::currentPath();
    
    // 检查资源文件
    QDir resourceDir(":/");
    qDebug() << "资源目录内容:";
    for (const QString &entry : resourceDir.entryList()) {
        qDebug() << "  " << entry;
    }
    
    QDir assetsDir(":/assets_0");
    if (assetsDir.exists()) {
        qDebug() << "资源assets_0目录内容:";
        for (const QString &entry : assetsDir.entryList()) {
            qDebug() << "  " << entry;
        }
    } else {
        qWarning() << "资源assets_0目录不存在!";
    }
    
    // 创建离屏渲染器
    OffscreenRenderer renderer;
    
    return app.exec();
}

#include "main.moc"
