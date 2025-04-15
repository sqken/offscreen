#ifndef OFFSCREENRENDERER_H
#define OFFSCREENRENDERER_H

#include <QObject>
#include <QQmlEngine>
#include <QQuickWindow>
#include <QQuickItem>
#include <QTimer>

class OffscreenRenderer : public QObject
{
    Q_OBJECT

public:
    explicit OffscreenRenderer(QObject *parent = nullptr);
    ~OffscreenRenderer();

public slots:
    // 当QML内容变动时调用
    void onContentChanged();
    void render();
    
private:
    QQmlEngine *m_engine;
    QQuickWindow *m_window;
    QQuickItem *m_rootItem;
    QTimer *m_debounceTimer;
    bool m_pendingSave;  // 标记是否有待保存的内容
};

#endif // OFFSCREENRENDERER_H 