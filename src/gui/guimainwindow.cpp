/* Copyright (c) 2024 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "guimainwindow.h"
#include "./ui_guimainwindow.h"

GuiMainWindow::GuiMainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::GuiMainWindow)
{
    ui->setupUi(this);

    XProcess::setDebugPrivilege(true);

#ifdef USE_YARA
    XYara::initialize();
#endif
    setWindowTitle(XOptions::getTitle(X_APPLICATIONDISPLAYNAME, X_APPLICATIONVERSION));

    g_xOptions.setName(X_OPTIONSFILE);

    g_xOptions.addID(XOptions::ID_VIEW_STYLE, "Fusion");
    g_xOptions.addID(XOptions::ID_VIEW_LANG, "System");
    g_xOptions.addID(XOptions::ID_VIEW_STAYONTOP, false);

    DIEOptionsWidget::setDefaultValues(&g_xOptions);
    SearchSignaturesOptionsWidget::setDefaultValues(&g_xOptions);
    XHexViewOptionsWidget::setDefaultValues(&g_xOptions);
    XDisasmViewOptionsWidget::setDefaultValues(&g_xOptions);
    XOnlineToolsOptionsWidget::setDefaultValues(&g_xOptions);
    XInfoDBOptionsWidget::setDefaultValues(&g_xOptions);

    g_xOptions.addID(XOptions::ID_SCAN_ENGINE_EMPTY, "");
#ifdef USE_YARA
    g_xOptions.addID(XOptions::ID_SCAN_YARARULESPATH, "$data/yara_rules");
#endif
    g_xOptions.load();

    g_xShortcuts.setName(X_SHORTCUTSFILE);
    g_xShortcuts.setNative(g_xOptions.isNative(), g_xOptions.getApplicationDataPath());

    g_xShortcuts.addGroup(XShortcuts::GROUPID_STRINGS);
    g_xShortcuts.addGroup(XShortcuts::GROUPID_SIGNATURES);
    g_xShortcuts.addGroup(XShortcuts::GROUPID_HEX);
    g_xShortcuts.addGroup(XShortcuts::GROUPID_DISASM);
    g_xShortcuts.addGroup(XShortcuts::GROUPID_ARCHIVE);
    g_xShortcuts.addGroup(XShortcuts::GROUPID_SCAN);
    g_xShortcuts.load();

    ui->widgetProcesses->setGlobal(&g_xShortcuts, &g_xOptions);

    connect(&g_xOptions, SIGNAL(errorMessage(QString)), this, SLOT(errorMessageSlot(QString)));

    createMenus();

    adjustWindow();

    ui->widgetProcesses->reload();
}

GuiMainWindow::~GuiMainWindow()
{
    g_xOptions.save();
    g_xShortcuts.save();

    delete ui;
#ifdef USE_YARA
    XYara::finalize();
#endif
}

void GuiMainWindow::createMenus()
{
    QMenu *pMenuFile = new QMenu(tr("File"), ui->menubar);
    QMenu *pMenuTools = new QMenu(tr("Tools"), ui->menubar);
    QMenu *pMenuHelp = new QMenu(tr("Help"), ui->menubar);

    ui->menubar->addAction(pMenuFile->menuAction());
    ui->menubar->addAction(pMenuTools->menuAction());
    ui->menubar->addAction(pMenuHelp->menuAction());

    QAction *pActionExit = new QAction(tr("Exit"), this);
    QAction *pActionOptions = new QAction(tr("Options"), this);
    QAction *pActionAbout = new QAction(tr("About"), this);
    // QAction *pActionShortcuts = new QAction(tr("Shortcuts"), this);

    pMenuFile->addAction(pActionExit);
    // pMenuTools->addAction(pActionShortcuts);
    pMenuTools->addAction(pActionOptions);
    pMenuHelp->addAction(pActionAbout);

    connect(pActionExit, SIGNAL(triggered()), this, SLOT(actionExitSlot()));
    connect(pActionOptions, SIGNAL(triggered()), this, SLOT(actionOptionsSlot()));
    connect(pActionAbout, SIGNAL(triggered()), this, SLOT(actionAboutSlot()));
    // connect(pActionShortcuts, SIGNAL(triggered()), this, SLOT(actionShortcutsSlot()));
}

void GuiMainWindow::actionExitSlot()
{
    this->close();
}

void GuiMainWindow::actionOptionsSlot()
{
    DialogOptions dialogOptions(this, &g_xOptions, XOptions::GROUPID_FILE);
    dialogOptions.exec();

    adjustWindow();
}

void GuiMainWindow::actionAboutSlot()
{
    DialogAbout dialogAbout(this);
    dialogAbout.exec();
}

void GuiMainWindow::adjustWindow()
{
    ui->widgetProcesses->adjustView();

    g_xOptions.adjustWindow(this);
}

void GuiMainWindow::errorMessageSlot(const QString &sText)
{
    QMessageBox::critical(this, tr("Error"), sText);
}
