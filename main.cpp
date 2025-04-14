#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickRenderControl>
#include <QQuickWindow>
#include <QQuickItem>
#include <QQmlComponent>
#include <QBuffer>
#include <QImage>
#include <QDir>
#include <QDebug>
#include <QTimer>
#include <QPainter>
#include <QQmlContext>
#include <QOpenGLFramebufferObject>
#include <QOffscreenSurface>
#include <QQuickGraphicsDevice>
#include <QQuickRenderTarget>
#include <QSurfaceFormat>
#include <QOpenGLContext>
#include <QOpenGLFunctions>

// 添加Qt配置条件
#if QT_CONFIG(opengl)
// OpenGL相关代码被启用
#endif

class OffscreenRenderer : public QObject
{
    Q_OBJECT

public:
    OffscreenRenderer(const QUrl &qmlSource, QObject *parent = nullptr)
        : QObject(parent)
        , m_renderControl(new QQuickRenderControl(this))
        , m_quickWindow(new QQuickWindow(m_renderControl))
        , m_engine(new QQmlEngine(this))
    {
        // 创建OpenGL上下文
        m_context = new QOpenGLContext();
        m_context->setFormat(QSurfaceFormat::defaultFormat());
        m_context->create();
        
        // 创建离屏表面
        m_offscreenSurface = new QOffscreenSurface();
        m_offscreenSurface->setFormat(m_context->format());
        m_offscreenSurface->create();
        
        // 使OpenGL上下文成为当前上下文
        m_context->makeCurrent(m_offscreenSurface);
        
        // 创建渲染目标
        m_quickWindow->setWidth(800);
        m_quickWindow->setHeight(600);
        m_quickWindow->setColor(Qt::white);
        
        // 设置渲染设备
        QQuickGraphicsDevice device = QQuickGraphicsDevice::fromOpenGLContext(m_context);
        m_quickWindow->setGraphicsDevice(device);

        // 确保引擎和窗口连接
        m_engine->rootContext()->setContextProperty("_window", m_quickWindow);
        
        // 加载QML
        QQmlComponent component(m_engine, qmlSource);
        if (component.isError()) {
            for (const QQmlError &error : component.errors())
                qWarning() << error.toString();
            return;
        }
        
        // 创建根项目
        m_rootItem = qobject_cast<QQuickItem*>(component.create());
        if (!m_rootItem) {
            qWarning() << "Failed to create root item";
            return;
        }
        
        // 设置尺寸
        m_rootItem->setParentItem(m_quickWindow->contentItem());
        m_rootItem->setWidth(m_quickWindow->width());
        m_rootItem->setHeight(m_quickWindow->height());
        
        // 初始化渲染控制
        m_renderControl->initialize();
        
        // 创建定时器，每秒触发一次渲染
        m_timer = new QTimer(this);
        connect(m_timer, &QTimer::timeout, this, &OffscreenRenderer::render);
        m_timer->start(1000); // 每秒更新一次
        
        // 立即执行一次渲染
        QTimer::singleShot(100, this, &OffscreenRenderer::render);
    }
    
    ~OffscreenRenderer() {
        if (m_context && m_context->makeCurrent(m_offscreenSurface)) {
            delete m_fbo;
            m_context->doneCurrent();
        }
        
        delete m_rootItem;
        delete m_quickWindow;
        delete m_renderControl;
        delete m_offscreenSurface;
        delete m_context;
    }

public slots:
    void render() {
        if (!m_rootItem || !m_context)
            return;
            
        // 使OpenGL上下文成为当前上下文
        m_context->makeCurrent(m_offscreenSurface);
            
        // 同步场景图
        m_renderControl->polishItems();
        m_renderControl->sync();
        
        // 创建帧缓冲对象
        QSize size = m_quickWindow->size();
        if (!m_fbo || m_fbo->size() != size) {
            delete m_fbo;
            QOpenGLFramebufferObjectFormat format;
            format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
            format.setSamples(4);
            m_fbo = new QOpenGLFramebufferObject(size, format);
        }
        
        // 设置渲染目标 - 使用Qt 6.7.2中正确的API
        QQuickRenderTarget renderTarget = QQuickRenderTarget::fromOpenGLTexture(m_fbo->texture(), size, m_fbo->format().samples());
        m_quickWindow->setRenderTarget(renderTarget);
        
        // 渲染
        m_renderControl->render();
        
        // 读取渲染结果
        QImage image = m_fbo->toImage();
        
        // 结束当前上下文
        m_context->doneCurrent();
        
        // 保存为BMP文件
        QString savePath = QDir::currentPath() + "/offscreen_output.bmp";
        if (image.save(savePath, "BMP")) {
            qDebug() << "Image saved to:" << savePath;
        } else {
            qWarning() << "Failed to save image to:" << savePath;
        }
    }
    
private:
    QQuickRenderControl *m_renderControl;
    QQuickWindow *m_quickWindow;
    QQmlEngine *m_engine;
    QQuickItem *m_rootItem;
    QTimer *m_timer;
    QOffscreenSurface *m_offscreenSurface = nullptr;
    QOpenGLContext *m_context = nullptr;
    QOpenGLFramebufferObject *m_fbo = nullptr;
};

int main(int argc, char *argv[])
{
    // 设置OpenGL上下文
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(4, 1);
    format.setStencilBufferSize(8);
    format.setDepthBufferSize(24);
    QSurfaceFormat::setDefaultFormat(format);
    
    QGuiApplication app(argc, argv); // 创建 Qt 应用程序对象
    
    // 创建离屏渲染器
    OffscreenRenderer renderer(QUrl(QStringLiteral("qrc:/main.qml")));
    
    return app.exec(); // 启动 Qt 事件循环
}

#include "main.moc"
