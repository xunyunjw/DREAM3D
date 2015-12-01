/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#ifndef DREAM3DToolbox_H
#define DREAM3DToolbox_H

#include <QtWidgets/QMainWindow>

//-- UIC generated Header
#include "ui_DREAM3DToolbox.h"

class DREAM3DToolboxMenu;

class DREAM3DToolbox : public QMainWindow, private Ui::DREAM3DToolbox
{
    Q_OBJECT

  public:
    virtual ~DREAM3DToolbox();

    static DREAM3DToolbox* Instance(QWidget* parent = 0, Qt::WindowFlags flags = 0);

    enum ToolboxTabs
    {
      FilterLibrary,
      FilterList,
      Bookmarks
    };

    void setupGui();

    BookmarksToolboxWidget* getBookmarksWidget();
    FilterListToolboxWidget* getFilterListWidget();
    FilterLibraryToolboxWidget* getFilterLibraryWidget();

    void readSettings();
    void readWindowSettings(DREAM3DSettings& prefs);

    void writeSettings();
    void writeWindowSettings(DREAM3DSettings& prefs);

    DREAM3DToolboxMenu* getToolboxMenu();
    void setToolboxMenu(DREAM3DToolboxMenu* menu);

    void setCurrentTab(ToolboxTabs tab);

  protected:
    DREAM3DToolbox(QWidget* parent = 0, Qt::WindowFlags flags = 0);
    void closeEvent(QCloseEvent* event);

  protected slots:
    void actionShowFilterLibrary_toggled(bool enabled);
    void actionShowFilterList_toggled(bool enabled);
    void actionShowBookmarks_toggled(bool enabled);

  private:
    static DREAM3DToolbox*                              self;

    DREAM3DToolboxMenu*                                 m_ToolboxMenu;

    DREAM3DToolbox(const DREAM3DToolbox&); // Copy Constructor Not Implemented
    void operator=(const DREAM3DToolbox&); // Operator '=' Not Implemented
};

#endif // DREAM3DToolbox_H
