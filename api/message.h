/**************
FILE          : message.h
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : SaX2 GUI system using libsax to provide
              : configurations for a wide range of X11 capabilities
              : //.../
              : - message.h: SaXGUI::SCCMessage header definitions
              : - message.cpp: provide support for popup windows
              : ----
              :
STATUS        : Status: Development
**************/
#ifndef SCCMESSAGE_H
#define SCCMESSAGE_H 1

#undef Above
#undef Below
#undef None
#undef KeyPress
#undef KeyRelease
#undef FocusIn
#undef FocusOut
#undef FontChange
#undef Bool


#include <qwidget.h>
#include <q3widgetstack.h>
#include <qapplication.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <q3frame.h>
#include <q3hbox.h>
#include <q3boxlayout.h>
#include <q3dict.h>
#include <qdialog.h>
//Added by qt3to4:
#include <QShowEvent>
#include <QEvent>
#include <QCloseEvent>

#include "common/log.h"
#include "wrapper.h"

namespace SaXMessage {
//=====================================
// Defines [Box types]...
//-------------------------------------
enum Type {
	OK,            // standard Ok box
	OK_CANCEL,     // standard Ok Cancel box
	YES_NO,        // standard Yes No box
	OPT_ANY,       // special option box for value type ANY
	OPT_INT,       // special option box for value type INT
	OPT_STRING,    // special option box for value type STRING
	INTRO,         // Ok,Cancel,Change introduction box
	FINISH         // Test,Save,Cancel finish box
};

//=====================================
// Defines [Box Icons]...
//-------------------------------------
enum Icon {
	NoIcon,        // no icon to display
	Information,   // information icon to display
	Warning,       // warning icon to display
	Critical       // critical error to display
};
}

namespace SaXGUI {
//=========================================
// Class Button
//-----------------------------------------
class SCCButton : public QPushButton {
	Q_OBJECT

	private:
	QString mButtonText;

	private slots:
	void slotClicked (void);

	signals:
	void clickedButton (QPushButton*);

	public:
	SCCButton ( const QString &, QWidget*, const char* =0 );
};

//=====================================
// Class SCCMessage
//-------------------------------------
class SCCMessage : public QDialog {
	Q_OBJECT

	private:
	Q3Dict<QString>*  mTextPtr;
	SaXMessage::Icon mIcon;

	private:
	QString mMessage;
	QString mClickedText;

	private:
	Q3Frame*     mMainFrame;
	Q3BoxLayout* mMainLayout;
	Q3BoxLayout* mIconLayout;
	Q3BoxLayout* mWorkLayout;
	Q3BoxLayout* mTitleLayout;
	Q3BoxLayout* mDoneLayout;

	private:
	QSpinBox*   mSpinBox;
	QComboBox*  mComboBox;
	QLineEdit*  mLineEdit;

	private:
	void createOkBox    ( void );
	void createYesNoBox ( void );
	void createOkCancelBox  ( void );
	void createOptionAnyBox ( void );
	void createOptionIntBox ( void );
	void createOptionStringBox ( void );
	void createIntroBox  ( void );
	void createFinishBox ( QString* );

	private:
	void setIcon ( SaXMessage::Icon );
	void setText ( const QString& );
	void addMainLayout    ( void );
	void addSpecialLayout ( void );

	public:
	QString showMessage ( void );
	void setSpinValue   ( int );
	void setLineValue   ( const QString& );
	void setComboValue  ( const QString& );
	void setComboList   ( const QStringList& );
	int getSpinValue    ( void );
	QString getComboValue ( void );
	QString getLineValue  ( void );

	public slots:
	void closeEvent ( QCloseEvent* );

	private slots:
	void slotButton ( QPushButton* );

	protected:
	virtual void showEvent   ( QShowEvent * );
	virtual bool eventFilter ( QObject*, QEvent* );

	public:
	SCCMessage (
		QWidget*, Q3Dict<QString>*, SaXMessage::Type, const QString&,
		const QString& = 0, SaXMessage::Icon = SaXMessage::Information,
		QString* = 0, bool =true, const char* =0
	);
};
} // end namespace
#endif
