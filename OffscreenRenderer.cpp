#include "OffscreenRenderer.h"
#include <QQmlComponent>
#include <QQuickWindow>
#include <QQuickItem>
#include <QImage>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>
#include <QDateTime>

OffscreenRenderer::OffscreenRenderer(QObject *parent)
    : QObject(parent), m_pendingSave(false)
{
    // 创建QML引擎
    m_engine = new QQmlEngine(this);
    
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

OffscreenRenderer::~OffscreenRenderer() {
    delete m_rootItem;
    delete m_window;
}

// 当QML内容变动时调用
void OffscreenRenderer::onContentChanged() {
    // 设置待保存标志
    m_pendingSave = true;
    
    // 启动防抖定时器，100ms内多次变动只触发一次渲染
    if (!m_debounceTimer->isActive()) {
        m_debounceTimer->start(100);  // 100ms 防抖时间
    }
}

void OffscreenRenderer::render() {
    if (!m_rootItem || !m_window) {
        qWarning() << "渲染失败：根项目或窗口为空";
        return;
    }
    
    // 如果没有待保存的内容，则不进行保存
    if (!m_pendingSave) {
        return;
    }
    
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
    
    // 保存为BMP文件
    QString savePath = QDir::currentPath() + "/offscreen_output.bmp";
    if (image.save(savePath, "BMP")) {
        // 重置待保存标志
        m_pendingSave = false;
    } else {
        qWarning() << "保存图像失败:" << savePath;
    }
} 