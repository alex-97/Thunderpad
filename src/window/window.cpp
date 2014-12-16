//
//  This file is part of Thunderpad
//
//  Copyright (c) 2013-2014 Alex Spataru <alex.racotta@gmail.com>
//  Please check the license.txt file for more information.
//

#include "window.h"

#include "menubar.h"
#include "toolbar.h"
#include "statusbar.h"
#include "searchdialog.h"

Window::Window() {
    setObjectName("window");
    setAttribute(Qt::WA_DeleteOnClose);

    // Create the core components
    // NOTE: The text editor must be initialized first or the application may crash
    m_editor = new Editor(this);
    m_menu = new MenuBar(this);
    m_about_dlg = new About(this);
    m_toolbar = new ToolBar(this);
    m_statusbar = new StatusBar(this);
    m_updater = new QSimpleUpdater(this);
    m_search_dialog = new SearchDialog(this);

    // Display the components correctly
    updateTitle();
    setCentralWidget(editor());

    // Connect slots between the text editor and window
    connect(editor(), SIGNAL(updateTitle()), this, SLOT(updateTitle()));
    connect(editor(), SIGNAL(textChanged()), this, SLOT(updateTitle()));
    connect(editor(), SIGNAL(settingsChanged()), this, SIGNAL(settingsChanged()));
    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(close()));
    connect(this, SIGNAL(updateSettings()), editor(), SLOT(updateSettings()));

    // Create a new QSettings instance
    m_settings = new QSettings(APP_COMPANY, APP_NAME);
    updateSettings();

    // Resize the window
    setMinimumSize(420, 420);
    resize(m_settings->value("size", QSize(640, 420)).toSize());
    move(m_settings->value("position", QPoint(200, 200)).toPoint());

    // Show the window
    m_settings->value("maximized", false).toBool() ? showMaximized() : showNormal();
}

Editor *Window::editor() const {
    return m_editor;
}

ToolBar *Window::toolbar() const {
    return m_toolbar;
}

void Window::closeEvent(QCloseEvent *event) {
    // Save the window state
    m_settings->setValue("size", size());
    m_settings->setValue("position", pos());

    // Close the window only if the user has saved
    // his/her work or discharged the changes
    m_editor->maybeSave() ? event->accept() : event->ignore();
}

void Window::openFile(const QString &file_name) {
    Q_ASSERT(!file_name.isEmpty());

    // Open the file in the same window
    if (m_editor->documentTitle().isEmpty() && !m_editor->document()->isModified())
        m_editor->readFile(file_name);

    // Open the file in another window
    else {
        Window *_window = new Window();
        configureWindow(_window);
        _window->editor()->readFile(file_name);
    }
}

void Window::newFile() {
    Window *_window = new Window();
    configureWindow(_window);
}

void Window::open() {
    // Get a list of selected file
    QStringList _files = QFileDialog::getOpenFileNames(this, tr("Open"), QDir::homePath());

    // Open each file
    for (int i = 0; i < _files.count(); ++i)
        if (!_files.at(i).isEmpty())
            openFile(_files.at(i));
}

void Window::setReadOnly(bool ro) {
    m_editor->setReadOnly(ro);
    m_toolbar->setReadOnly(ro);
    emit readOnlyChanged(ro);
}

void Window::setWordWrap(bool ww) {
    m_settings->setValue("wordwrap-enabled", ww);
    syncSettings();
}

void Window::setToolbarText(bool tt) {
    m_settings->setValue("toolbar-text", tt);
    syncSettings();
}

void Window::setToolbarEnabled(bool tb) {
    m_settings->setValue("toolbar-enabled", tb);
    syncSettings();
}

void Window::setStatusBarEnabled(bool sb) {
    m_settings->setValue("statusbar-enabled", sb);
    syncSettings();
}

void Window::setHCLineEnabled(bool hc) {
    m_settings->setValue("hc-line-enabled", hc);
    syncSettings();
}

void Window::setLineNumbersEnabled(bool ln) {
    m_settings->setValue("line-numbers-enabled", ln);
    syncSettings();
}

void Window::setColorscheme(const QString &colorscheme) {
    Q_ASSERT(!colorscheme.isEmpty());

    m_settings->setValue("color-scheme", colorscheme);
    syncSettings();
}

void Window::showFindReplaceDialog() {
    m_search_dialog->show();
}

void Window::setIconTheme(const QString &theme) {
    Q_ASSERT(!theme.isEmpty());

    m_settings->setValue("icon-theme", theme);
    syncSettings();
}

void Window::aboutThunderpad() {
    m_about_dlg->show();
}

void Window::license() {
    QDesktopServices::openUrl(QUrl("http://www.gnu.org/copyleft/gpl.html"));
}

void Window::donate() {
    QDesktopServices::openUrl(QUrl("http://www.thunderpad.sf.net/donate"));
}

void Window::viewHelp() {
    QDesktopServices::openUrl(QUrl("http://thunderpad.sf.net/support"));
}

void Window::sendFeedback() {
    QDesktopServices::openUrl(QUrl("mailto:alex_spataru@outlook.com"));
}

void Window::reportBug() {
    QDesktopServices::openUrl(
                QUrl("https://github.com/alex-97/thunderpad/issues/new"));
}

void Window::makeContribution() {
    QDesktopServices::openUrl(QUrl("http://thunderpad.sf.net/contribute"));
}

void Window::officialWebsite() {
    QDesktopServices::openUrl(QUrl("http://thunderpad.sf.net"));
}

void Window::updateTitle() {
    QString _title;

    // Use "untitled" when we have no saved document
    if (m_editor->documentTitle().isEmpty())
        _title = tr("Untitled");

    // Use the file name in the title
    else
        _title = shortFileName(m_editor->documentTitle());

    // Add a "*" if the document was modifed
    if (m_editor->document()->isModified())
        setWindowTitle(_title + "* - " + APP_NAME);

    else
        setWindowTitle(_title + " - " + APP_NAME);

    // Configure the behavior of the 'smart' save actions
    if (!m_editor->documentTitle().isEmpty() &&
            !m_editor->document()->isModified()) {
        m_menu->setSaveEnabled(false);
        m_toolbar->setSaveEnabled(false);
    }

    else {
        m_menu->setSaveEnabled(true);
        m_toolbar->setSaveEnabled(true);
    }
}

void Window::syncSettings() {
    emit updateSettings();
    emit settingsChanged();
}

void Window::saveWindowState() {
    m_settings->setValue("maximized", isMaximized());

    if (!isMaximized()) {
        m_settings->setValue("size", size());
        m_settings->setValue("position", pos());
    }
}

void Window::configureWindow(Window *window) {
    Q_ASSERT(window != NULL);

    window->saveWindowState();
    connect(window, SIGNAL(checkForUpdates()), this, SIGNAL(checkForUpdates()));

    // Ensure that all windows are connected together and synced together
    foreach (QWidget *_widget, QApplication::topLevelWidgets()) {
        if (_widget->objectName() == "window") {
            connect(_widget, SIGNAL(settingsChanged()), window,
                    SIGNAL(updateSettings()));
            connect(window, SIGNAL(settingsChanged()), _widget,
                    SIGNAL(updateSettings()));
        }
    }

    window->resize(size());
    window->move(window->x() + 45, window->y() + 45);
    m_settings->setValue("position", QPoint(window->x(), window->y()));
}

QString Window::shortFileName(const QString &file) {
    Q_ASSERT(!file.isEmpty());
    return QFileInfo(file).fileName();
}