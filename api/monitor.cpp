/**************
FILE          : monitor.cpp
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
              : - monitor.h: SaXGUI::SCCMonitor header definitions
              : - monitor.cpp: configure monitor system
              : ----
              :
STATUS        : Status: Development
**************/
#include "monitor.h"

//====================================
// Globals
//------------------------------------
extern SaXGUI::SCCWidgetProfile* SaXWidgetProfile;

namespace SaXGUI {
//====================================
// Constructor
//------------------------------------
SCCMonitor::SCCMonitor (
	Q3WidgetStack* stack, Q3Dict<QString>* text,
	Q3Dict<SaXImport> section, QWidget* parent
) : SCCDialog ( stack,text,section,parent ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	//=====================================
	// check for graphics cards
	//-------------------------------------
	SaXManipulateCard saxCard ( mSection["Card"] );
	mCard = saxCard.getDevices();

	//=====================================
	// Initialize 3D warning state
	//-------------------------------------
	mShow3DMessage = true;

	//=====================================
	// create monitor dialog
	//-------------------------------------
	mMonitorTab = new QTabWidget ( mDialogFrame );
	mCheck3D    = new QCheckBox  ( mText["Activate3D"],mDialogFrame );

	//=====================================
	// connect widgets
	//-------------------------------------
	QObject::connect (
		mCheck3D , SIGNAL (clicked        ( void )),
		this     , SLOT   (slotActivate3D ( void ))
	);
	//=====================================
	// add widgets to the layout
	//-------------------------------------
	mDialogLayout -> addWidget  ( mMonitorTab );
	mDialogLayout -> addSpacing ( 15 );
	mDialogLayout -> addWidget  ( mCheck3D );

	//=====================================
	// create arrangement dialog
	//-------------------------------------
	mMonitorArrange = 0;
	if ( mCard > 1 ) {
		mMonitorArrange = new SCCMonitorArrange ( text,section,this );
	}
	//=====================================
	// create basic display tabs
	//-------------------------------------
	for ( int n=0;n<mCard;n++) {
		QString displayName;
		QString tabNumber;
		QString translation = mText["Display"];
		QTextOStream (&tabNumber) << (n + 1);
		translation.replace ( QRegExp("\%1"), tabNumber);
		QTextOStream (&displayName) << translation;
		SCCMonitorDisplay* display = new SCCMonitorDisplay (
			text,section,n,this
		);
		mMonitorTab -> addTab (
			display, displayName
		);
		display -> init();
		display -> import();
		//=====================================
		// connect monitors activate checkbox
		//-------------------------------------
		QObject::connect (
			display , SIGNAL (sigActivate  ( void )),
			this    , SLOT   (slotActivate ( void ))
		);
		//=====================================
		// connect monitors dualhead checkbox
		//-------------------------------------
		if ( mMonitorArrange ) {
			QObject::connect (
				display, SIGNAL (
					sigDualModeChanged ( SCCMonitorDisplay* )
				),
				mMonitorArrange, SLOT (
					slotDualModeChanged ( SCCMonitorDisplay* )
				)
			);
		}
		//=====================================
		// connect monitor dual popup slotOk
		//-------------------------------------
		if ( mMonitorArrange ) {
			SCCMonitorDual* mMonitorDual = display->getDualData();
			QObject::connect (
				mMonitorDual, SIGNAL (
					sigDualModeChanged (SCCMonitorDisplay*)
				),
				mMonitorArrange, SLOT (
					slotDualModeChanged ( SCCMonitorDisplay* )
				)
			);
		}
		//=====================================
		// connect monitor model popup slotOk
		//-------------------------------------
		if ( mMonitorArrange ) {
			SCCMonitorModel* mMonitorModel = display->getMonitorData();
			QObject::connect (
				mMonitorModel, SIGNAL (
					sigMonitorChanged  (SCCMonitorDisplay*)
				),
				mMonitorArrange, SLOT (
					slotMonitorChanged (SCCMonitorDisplay*)
				)
			);
		}
		mMonitorDisplay.append ( display );
	}
	//=====================================
	// finish creation of arrangement dialog
	//-------------------------------------
	if ( mMonitorArrange ) {
		mMonitorTab -> addTab (
			mMonitorArrange,mText["Arrange"]
		);
		mMonitorArrange -> init();
		mMonitorArrange -> import();
	}
}
//====================================
// init
//------------------------------------
void SCCMonitor::init ( void ) {
	//====================================
	// create desktop manipulator
	//------------------------------------
	SaXManipulateDesktop saxDesktop (
		mSection["Desktop"],mSection["Card"],mSection["Path"]
	);
	//====================================
	// check 3D box
	//------------------------------------
	bool has3DCapabilities = false;
	bool isMultiheaded     = false;
	for ( int n=0;n<mCard;n++) {
		if ( saxDesktop.selectDesktop (n) ) {
		if ( saxDesktop.is3DCard()) {
			has3DCapabilities = true;
			break;
		}
		}
	}
	int count = 0;
	SCCMonitorDisplay* it;
	foreach (it,mMonitorDisplay) {
		if (it->isEnabled()) {
			count++;
		}
		if (count > 1) {
			isMultiheaded = true;
			break;
		}
	}
	mCheck3D -> setDisabled ( false );
	if ( (! has3DCapabilities) || (isMultiheaded) ) {
		mCheck3D -> setDisabled ( true  );
		mCheck3D -> setChecked  ( false );
		#if 0
		if (mShow3DMessage) {
			SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);
			QString message = mText["3DInfo"];
			if (isMultiheaded) {
				message = mText["XineramaInfo"];
			}
			SCCMessage* mMessageBox = new SCCMessage (
				qApp->mainWidget(), mTextPtr, SaXMessage::OK,
				message, "MessageCaption", SaXMessage::Warning
			);
			mMessageBox -> showMessage();
			mShow3DMessage = false;
		}
		#endif
	}
	//====================================
	// check monitor arrangement
	//------------------------------------
	if ( mMonitorArrange ) {
		mMonitorTab -> setTabEnabled ( mMonitorArrange,true );
		if (! isMultiheaded) {
			mMonitorTab -> setTabEnabled ( mMonitorArrange,false );
		}
	}
	//====================================
	// set init state
	//------------------------------------
	needInitData = false;
}
//====================================
// import
//------------------------------------
void SCCMonitor::import ( void ) {
	//====================================
	// create manipulators
	//------------------------------------
	SaXManipulateDesktop saxDesktop (
		mSection["Desktop"],mSection["Card"],mSection["Path"]
	);
	SaXManipulateLayout saxLayout (
		mSection["Layout"],mSection["Card"]
	);
	//====================================
	// handle 3D data import
	//------------------------------------
	m3DEnabled = false;
	if (saxDesktop.is3DEnabled()) {
		m3DEnabled = true;
	}
	//====================================
	// handle monitorDisplay's state
	//------------------------------------
	for (int dev=0;dev<mCard;dev++) {
	if (saxLayout.getXOrgLayout ( dev ).isEmpty()) {
		mMonitorDisplay.at(dev)->setEnabled ( false );
	}
	}
	//====================================
	// set widgets to imported data
	//------------------------------------
	mCheck3D -> setChecked ( m3DEnabled );

	//====================================
	// set arrangement if needed
	//------------------------------------
	if ( mMonitorArrange ) {
		mMonitorArrange -> setArrangement ( mCard,mMonitorDisplay );
	}
	//====================================
	// set import state
	//------------------------------------
	needImportData = false;
}
//====================================
// slotActivate
//------------------------------------
void SCCMonitor::slotActivate ( void ) {
	int count = 0;
	SCCMonitorDisplay* it;
	foreach (it,mMonitorDisplay) {
	if (it->isEnabled()) {
		count++;
	}
	}
	if ( count == 0) {
		SCCMessage* mMessageBox = new SCCMessage (
			qApp->mainWidget(), mTextPtr, SaXMessage::OK, "PageFault",
			"MessageCaption", SaXMessage::Warning
		);
		SCCMonitorDisplay* current; 
		current = (SCCMonitorDisplay*)mMonitorTab->currentPage();
		mMessageBox -> showMessage();
		current -> setEnabled();
	}
	if ( mMonitorArrange ) {
		mMonitorArrange -> setCleanArrangement ( count,mMonitorDisplay );
	}
	init();
}
//====================================
// slotActivate3D
//------------------------------------
void SCCMonitor::slotActivate3D ( void ) {
	m3DEnabled = false;
	if (mCheck3D -> isChecked()) {
		m3DEnabled = true;
	}
}
//====================================
// exportData
//------------------------------------
bool SCCMonitor::exportData ( void ) {
	int  card = 0;
	//====================================
	// save particular monitor data
	//------------------------------------
	SCCMonitorDisplay* display;
	foreach (display, mMonitorDisplay) {
		//====================================
		// remove current layout
		//------------------------------------
		if ( mMonitorArrange ) {
			QString screenID;
			QTextOStream (&screenID) << "Screen:Screen[" << card << "]";
			mSection["Layout"]->removeEntry( screenID );
		}
		//====================================
		// check for monitor display's
		//------------------------------------
		//SCCMonitorDisplay* display = (SCCMonitorDisplay*)it.current();
		//if (display->isEnabled()) {
		//====================================
		// create manipulators...
		//------------------------------------
		SaXManipulateCard saxCard (
			mSection["Card"]
		);
		SaXManipulateDesktop saxDesktop ( 
			mSection["Desktop"],mSection["Card"],mSection["Path"]
		);
		//====================================
		// select card and desktop
		//------------------------------------
		saxDesktop.selectDesktop ( card );
		saxCard.selectCard ( card );

		//====================================
		// check for framebuffer driver
		//------------------------------------
		if (saxCard.getCardDriver() == "fbdev") {
			int color = display->getColorDepth();
			QString resolution = display->getResolution().at(0);
			int mode = saxDesktop.getFBKernelMode (resolution,color);
			log (L_INFO,"SCCMonitor::Bootloader setup for fbdev: %dbit %s %d\n",
				color,resolution.ascii(),mode
			);
			if (! saxDesktop.setFBKernelMode ( mode )) {
				log (L_ERROR,"SCCMonitor::Bootloader update failed\n");
			}
		}
		//====================================
		// save resolution list
		//------------------------------------
		int colorDepths[4] = {8,15,16,24};
		for (int n=0;n<4;n++) {
			int color = colorDepths[n];
			QString modesKey;
			QTextOStream (&modesKey) << "Modes:" << color;
			mSection["Desktop"] -> removeEntry ( modesKey );
			QList<QString> resList = display->getResolution();
			QString it;
			foreach (it,resList) {
				QStringList tokens = QStringList::split ( "x",it );
				int xaxis = tokens.first().toInt();
				int yaxis = tokens.last().toInt();
				saxDesktop.addResolution (
					color,xaxis,yaxis
				);
			}
		}
		//====================================
		// save monitor settings
		//------------------------------------
		SCCMonitorModel* monitorData = display->getMonitorData();
		saxDesktop.setMonitorVendor (monitorData->getVendorName());
		saxDesktop.setMonitorName   (monitorData->getModelName());
		saxDesktop.setHsyncRange (
			(double)monitorData->getHSmin(),
			(double)monitorData->getHSmax()
		);
		saxDesktop.setVsyncRange (
			(double)monitorData->getVSmin(),
			(double)monitorData->getVSmax()
		);
		int sizeX = monitorData->getSizeX();
		int sizeY = monitorData->getSizeY();
		if ((sizeX > 0) && (sizeY > 0)) {
			saxDesktop.setDisplaySize ( sizeX,sizeY );
		}
		saxDesktop.disableDPMS();
		if (monitorData->isDPMSEnabled()) {
			saxDesktop.enableDPMS();
		}
		if (saxDesktop.willCalculateModelines()) {
			QString algorithm = saxDesktop.getModelineAlgorithm();
			if (algorithm != "XServerPool") {
				saxDesktop.calculateModelines ( true );
			}
		}
		//====================================
		// save card options
		//------------------------------------
		SCCMonitorCard* cardData = display->getCardData();
		Q3Dict<QString> optList = cardData->getOptions();
		mSection["Card"] -> removeEntry ("Option");
		mSection["Card"] -> removeEntry ("RawData");
		Q3DictIterator<QString> io (optList);
		for (; io.current(); ++io) {
			saxCard.addCardOption ( io.currentKey(),*io.current());
		}
		int rotateID = cardData->getRotate();
		mSection["Card"] -> removeEntry ("Rotate");
		saxCard.setRotate ( rotateID );

		//====================================
		// check for dual head mode
		//------------------------------------
		QString profile = saxDesktop.getDualHeadProfile();
		if ((! profile.isEmpty()) && (! saxDesktop.isXineramaMode())) {
			SaXImportProfile* pProfile = SaXWidgetProfile->getProfile (profile);
			SaXImport* mImport = pProfile -> getImport ( SAX_CARD );
			if ( mImport ) {
				//====================================
				// pick up card profile data
				//------------------------------------
				QString driver = saxCard.getCardDriver();
				Q3Dict<QString> profileDriverOptions;
				SaXManipulateCard saxProfileCard ( mImport );
				profileDriverOptions = saxProfileCard.getOptions();
				Q3DictIterator<QString> it ( profileDriverOptions );

				//====================================
				// delete profile data keys
				//------------------------------------
				QString externalDisplaySize = "";
				for (; it.current(); ++it) {
					if (it.currentKey() == "SaXExternal") {
						QRegExp sizeExp ("DisplaySize&([0-9]+ [0-9]+)");
						int pos = sizeExp.search (*it.current());
						if (pos >= 0) {
							externalDisplaySize = sizeExp.cap(1);
						}
					}
					saxCard.removeCardOption (it.currentKey());
				}
				#if 1 // to be removed as soon as possible
				//====================================               
				// fix broken cards                                     
				//------------------------------------
				fixBrokenCards (saxCard);
				#endif
				//====================================
				// init special modelines
				//------------------------------------
				if (saxDesktop.willCalculateModelines()) {
					//mSection["Desktop"] -> removeEntry ("SpecialModeline");
					saxDesktop.setExtraModelineString (
						monitorData->getExtraModeline()
					);
				}
				//====================================
				// setup profile data
				//------------------------------------
				if (display->isDualModeEnabled()) {
					it.toFirst();
					SCCMonitorDual* dualData;
					SCCMonitorDualModel* dualModel;
					dualData  = display->getDualData();
					dualModel = dualData->getDualModelData();
					//====================================
					// call ok slot from dualhead setup
					//------------------------------------
					dualData->slotOk();
					//====================================
					// setup dualhead options
					//------------------------------------
					saxCard.addCardOption ("SaXDualHead",0);

					//====================================
					// Add external monitor section
					//------------------------------------
					bool haveExternal = false;
					for (; it.current(); ++it) {
						QString key = it.currentKey();
						QString val = *it.current();
						if (key == "SaXExternal") {
							saxCard.addCardExternal ("EXT");
							saxCard.addCardExternalOption (
								"DisplaySize",externalDisplaySize
							);
							haveExternal = true;
						}
					}
					it.toFirst();
					for (; it.current(); ++it) {
						QString key = it.currentKey();
						QString val = *it.current();
						//====================================
						// setup profile "off"-line data
						//------------------------------------
						if (val == "off") {
							saxCard.addCardOption ( key,val );
							continue;
						}
						//====================================
						// setup profile meta data
						//------------------------------------
						if (key == "SaXDualMonitorVendor") {
							QString vendor = dualModel ->getVendorName();
							saxCard.addCardOption ( key,vendor );
							if (haveExternal) {
								saxCard.addCardExternalOption ("VendorName",vendor);
							}
						}
						if (key == "SaXDualMonitorModel") {
							QString model = dualModel ->getModelName();
							saxCard.addCardOption ( key,model );
							if (haveExternal) {
								saxCard.addCardExternalOption ("ModelName",model);
							}
						}
						if (key == "SaXDualResolution") {
							QString resolution = dualData->getResolution();
							saxCard.addCardOption ( key,resolution );
							if (haveExternal) {
								saxCard.addCardExternalOption (
									"PreferredMode",resolution
								);
							}
						}
						if (key == "SaXDualOrientation") {
							int orientation  = dualData->getLayout();
							QString position = DUAL_LEFTOF_KEY;
							switch (orientation) {
								case DUAL_ABOVEOF:
									position = DUAL_ABOVEOF_KEY;
								break;
								case DUAL_RIGHTOF:
									position = DUAL_RIGHTOF_KEY;
								break;
								case DUAL_BELOWOF:
									position = DUAL_BELOWOF_KEY;
								break;
								default:
								break;
							}
							saxCard.addCardOption ( key,position );
							if ((haveExternal) && (dualData->getMode()!=DUAL_CLONE)) {
								saxCard.addCardExternalOption (
									position,"Monitor[0]"
								);
							}
						}
						if (key == "SaXDualMode") {
							int mode = dualData->getMode();
							QString dualMode = DUAL_TRADITIONAL_KEY;
							switch (mode) {
								case DUAL_CLONE:
									dualMode = DUAL_CLONE_KEY;
								break;
								case DUAL_XINERAMA:
									dualMode = DUAL_XINERAMA_KEY;
								break;
								default:
								break;
							}
							saxCard.addCardOption ( key,dualMode );
						}
						if (key == "SaXDualHSync") {
							QString hsync;
							int hsmax = dualModel->getHSmax();
							int hsmin = dualModel->getHSmin();
							QTextOStream (&hsync) << hsmin << "-" << hsmax;
							saxCard.addCardOption ( key,hsync );
							if (haveExternal) {
								saxCard.addCardExternalOption ("HorizSync",hsync);
							}
						}
						if (key == "SaXDualVSync") {
							QString vsync;
							int hsmax = dualModel->getHSmax();
							int vsmax = dualModel->getVSmax();
							int vsmin = dualModel->getVSmin();
							QTextOStream (&vsync) << vsmin << "-" << vsmax;
							saxCard.addCardOption ( key,vsync );
							if (haveExternal) {
								saxCard.addCardExternalOption ("VertRefresh",vsync);
							}
							//====================================
							// calculate one modeline for each res
							//------------------------------------
							if (saxDesktop.willCalculateModelines()) {
								QString algorithm=saxDesktop.getModelineAlgorithm();
								if (algorithm != "XServerPool") {
									QList<QString> rList;
									rList=dualData->getResolutionList();
									QString ir;
									foreach (ir,rList) {
										QStringList xy = QStringList::split ("x",ir);
										int x = xy.first().toInt();
										int y = xy.last().toInt();
										log (L_INFO,
											"Add special mode: %dx%d@%d\n",x,y,vsmax
										);
										saxDesktop.addExtraModeline (
											x,y,vsmax,hsmax
										);
									}
								}
							}
						}
						//====================================
						// setup profile IntelNext
						//------------------------------------
						if ((driver == "intel") || (driver == "radeonhd") || (driver == "radeon")) {
							// search monitor-* outputs and re-add them
							QRegExp itemExp ("monitor-(.*)");
							int pos = itemExp.search (key);
							if (pos >= 0) {
								saxCard.addCardOption ( key,val );
							}
						}
						//====================================
						// setup profile NVidia data
						//------------------------------------
						if (key == "TwinView") {
							saxCard.addCardOption ( key,0 );
						}
						if (
							(key == "ConnectedMonitor") ||
							(key == "TwinViewXineramaInfoOrder")
						) {
							QStringList cs = QStringList::split (",",val);
							QString channelA = cs.first();
							QString channelB = cs.last();
							if ((channelA == "AUTO") || (channelB == "AUTO")) {
								saxCard.removeCardOption ( key );
							} else {
								saxCard.addCardOption ( key,val );
							}
						}
						if ((key== "MetaModes") && (driver == "nvidia")) {
							QString channelA = "AUTO:";
							QString channelB = "AUTO:";
							if (profileDriverOptions["ConnectedMonitor"]) {
								QStringList cs = QStringList::split (",",
									*profileDriverOptions["ConnectedMonitor"]
								);
								channelA = cs.first() + ":";
								channelB = cs.last()  + ":";
							}
							if ((channelA=="AUTO:") || (channelB=="AUTO:")) {
								channelA = "";
								channelB = "";
							}
							QList<QString> rList1=display->getResolution();
							QString resolution1 = rList1.at(0);
							QList<QString> rList2=dualData->getResolutionList();
							QString resolution2 = rList2.at(0);
							//====================================
							// add primary meta mode
							//------------------------------------
							QString metaItem;
							QString resolution;
							QTextOStream (&metaItem) <<
								channelA << resolution1 << "," <<
								channelB << resolution2;
							resolution.append (";"+metaItem);
							//====================================
							// add secondary meta modes
							//------------------------------------
							bool startMeta = false;
							QString ir;
							foreach (ir,rList1) {
								if (! startMeta) {
									startMeta = true; continue;
								}
								QString metaItem;
								int cmpr = compareResolution (ir,resolution2);
								if (cmpr >= 0) {
									QTextOStream (&metaItem) <<
										channelA << ir << "," <<
										channelB << resolution2;
								} else {
									QTextOStream (&metaItem) <<
										channelA << ir << "," <<
										channelB << ir;
								}
								resolution.append (";"+metaItem);
							}
							resolution.replace(QRegExp("^;"),"");
							saxCard.addCardOption ( key,resolution );
						}
						if (key == "SecondMonitorHorizSync") {
							QString hsync;
							int hsmax = dualModel->getHSmax();
							int hsmin = dualModel->getHSmin();
							QTextOStream (&hsync) << hsmin << "-" << hsmax;
							saxCard.addCardOption ( key,hsync );
						}
						if (key == "SecondMonitorVertRefresh") {
							QString vsync;
							int vsmax = dualModel->getVSmax();
							int vsmin = dualModel->getVSmin();
							QTextOStream (&vsync) << vsmin << "-" << vsmax;
							saxCard.addCardOption ( key,vsync );
						}
						if (key == "TwinViewOrientation") {
							int orientation  = dualData->getLayout();
							QString position = DUAL_LEFTOF_KEY;
							switch (orientation) {
								case DUAL_ABOVEOF:
									position = DUAL_ABOVEOF_KEY;
								break;
								case DUAL_RIGHTOF:
									position = DUAL_RIGHTOF_KEY;
								break;
								case DUAL_BELOWOF:
									position = DUAL_BELOWOF_KEY;
								break;
								default:
								break;
							}
							saxCard.addCardOption ( key,position );
							int mode = dualData->getMode();
							if ( mode == DUAL_CLONE ) {
								saxCard.removeCardOption (key);
								saxCard.addCardOption (key,DUAL_CLONE_KEY);
							}
						}
						//====================================
						// setup profile SiS data
						//------------------------------------
						if ((key == "MergedFB") && (driver == "sis")) {
							saxCard.addCardOption ( key,0 );
						}
						if (key == "MergedFBAuto") {
							saxCard.addCardOption ( key,0 );
						}
						if (key == "MergedXineramaScreen0") {
							saxCard.addCardOption ( key,val );
						}
						if (key == "MergedNonRectangular") {
							saxCard.addCardOption ( key,val );
						}
						if ((key== "MetaModes") && (driver == "sis")) {
							QList<QString> rList1=display->getResolution();
							QString resolution1 = rList1.at(0);
							QList<QString> rList2=dualData->getResolutionList();
							QString resolution2 = rList2.at(0);
							//====================================
							// add primary meta mode
							//------------------------------------
							QString metaItem;
							QString resolution;
							QTextOStream (&metaItem) <<
								resolution1 << "-" << resolution2;
							resolution.append (";"+metaItem);
							//====================================
							// add secondary meta modes
							//------------------------------------
							bool startMeta = false;
							QString ir;
							foreach (ir,rList1) {
								if (! startMeta) {
									startMeta = true; continue;
								}
								QString metaItem;
								int cmpr = compareResolution (ir,resolution2);
								if (cmpr >= 0) {
									QTextOStream (&metaItem) <<
										ir << "-" << resolution2;
								} else {
									QTextOStream (&metaItem) <<
										ir << "-" << ir;
								}
								resolution.append (";"+metaItem);
							}
							resolution.replace(QRegExp("^;"),"");
							saxCard.addCardOption ( key,resolution );
						}
						//====================================
						// setup profile Matrox data
						//------------------------------------
						if ((key == "MergedFB") && (driver == "mga")) {
							saxCard.addCardOption ( key,0 );
						}
						if (key == "Monitor2HSync") {
							QString hsync;
							int hsmax = dualModel->getHSmax();
							int hsmin = dualModel->getHSmin();
							QTextOStream (&hsync) << hsmin << "-" << hsmax;
							saxCard.addCardOption ( key,hsync );
						}
						if (key == "Monitor2VRefresh") {
							QString vsync;
							int vsmax = dualModel->getVSmax();
							int vsmin = dualModel->getVSmin();
							QTextOStream (&vsync) << vsmin << "-" << vsmax;
							saxCard.addCardOption ( key,vsync );
						}
						if ((key== "MetaModes") && (driver == "mga")) {
							QList<QString> rList1=display->getResolution();
							QString resolution1 = rList1.at(0);
							QList<QString> rList2=dualData->getResolutionList();
							QString resolution2 = rList2.at(0);
							//====================================
							// add primary meta mode
							//------------------------------------
							QString metaItem;
							QTextOStream (&metaItem) <<
								resolution1 << "-" << resolution2;
							saxCard.addCardOption ( key,metaItem );
							//====================================
							// add virtual size
							//------------------------------------
							for (int color=8;color<=24;color+=8) {
								saxDesktop.removeVirtualResolution (color);
							}
							int mode = dualData->getMode();
							if ( mode != DUAL_CLONE ) {
								QStringList r1 = QStringList::split (
									"x",resolution1
								);
								QStringList r2 = QStringList::split (
									"x",resolution2
								);
								int x1 = r1.first().toInt();
								int y1 = r1.last().toInt();
								int x2 = r2.first().toInt();
								int y2 = r2.last().toInt();
								int orientation  = dualData->getLayout();
								int x = 0;
								int y = 0;
								switch (orientation) {
									case DUAL_ABOVEOF:
									case DUAL_BELOWOF:
										y = y1 + y2;
										if (x1 > x2) x = x1; else x = x2;
									break;
									case DUAL_RIGHTOF:
									case DUAL_LEFTOF:
										x = x1 + x2;
										if (y1 > y2) y = y1; else y = y2;
									break;
									default:
									break;
								}
								for (int color=8;color<=24;color+=8) {
									saxDesktop.setVirtualResolution (color,x,y);
								}
							}
						}
						if (key == "Monitor2Position") {
							int orientation  = dualData->getLayout();
							QString position = DUAL_LEFTOF_KEY;
							switch (orientation) {
								case DUAL_ABOVEOF:
									position = DUAL_ABOVEOF_KEY;
								break;
								case DUAL_RIGHTOF:
									position = DUAL_RIGHTOF_KEY;
								break;
								case DUAL_BELOWOF:
									position = DUAL_BELOWOF_KEY;
								break;
								default:
								break;
							}
							saxCard.addCardOption ( key,position );
							int mode = dualData->getMode();
							if ( mode == DUAL_CLONE ) {
								saxCard.removeCardOption (key);
								saxCard.addCardOption (key,DUAL_CLONE_KEY);
							}
						}
						//====================================
						// setup profile Radeon data
						//------------------------------------
						if ((key== "MonitorLayout") && (driver == "radeon")) {
							saxCard.addCardOption ( key,val );
						}
						if ((key == "MergedFB") && (driver == "radeon")) {
							saxCard.addCardOption ( key,0 );
						}
						if (key == "CRT2HSync") {
							QString hsync;
							int hsmax = dualModel->getHSmax();
							int hsmin = dualModel->getHSmin();
							QTextOStream (&hsync) << hsmin << "-" << hsmax;
							saxCard.addCardOption ( key,hsync );
						}
						if (key == "CRT2VRefresh") {
							QString vsync;
							int vsmax = dualModel->getVSmax();
							int vsmin = dualModel->getVSmin();
							QTextOStream (&vsync) << vsmin << "-" << vsmax;
							saxCard.addCardOption ( key,vsync );
						}
						if (key == "IgnoreEDID") {
							saxCard.addCardOption ( key,0 );
						}
						if ((key== "MetaModes") && (driver == "radeon")) {
							QList<QString> rList1=display->getResolution();
							QString resolution1 = rList1.at(0);
							QList<QString> rList2=dualData->getResolutionList();
							QString resolution2 = rList2.at(0);
							//====================================
							// add primary meta mode
							//------------------------------------
							QString metaItem;
							QString resolution;
							QTextOStream (&metaItem) <<
								resolution1 << "," << resolution2;
							resolution.append (";"+metaItem);
							//====================================
							// add secondary meta modes
							//------------------------------------
							bool startMeta = false;
							QString ir;
							foreach (ir,rList1) {
								if (! startMeta) {
									startMeta = true; continue;
								}
								QString metaItem;
								int cmpr = compareResolution (ir,resolution2);
								if (cmpr >= 0) {
									QTextOStream (&metaItem) <<
										ir << "," << resolution2;
								} else {
									QTextOStream (&metaItem) <<
										ir << "," << ir;
								}
								resolution.append (";"+metaItem);
							}
							resolution.replace(QRegExp("^;"),"");
							saxCard.addCardOption ( key,resolution );
						}
						if (key == "CRT2Position") {
							int orientation  = dualData->getLayout();
							QString position = DUAL_LEFTOF_KEY;
							switch (orientation) {
								case DUAL_ABOVEOF:
									position = DUAL_ABOVEOF_KEY;
								break;
								case DUAL_RIGHTOF:
									position = DUAL_RIGHTOF_KEY;
								break;
								case DUAL_BELOWOF:
									position = DUAL_BELOWOF_KEY;
								break;
								default:
								break;
							}
							saxCard.addCardOption ( key,position );
							int mode = dualData->getMode();
							if ( mode == DUAL_CLONE ) {
								saxCard.removeCardOption (key);
								saxCard.addCardOption (key,DUAL_CLONE_KEY);
							}
						}
						//====================================
						// setup profile Intel data
						//------------------------------------
						if ((key== "MonitorLayout") && (driver == "i810")) {
							saxCard.addCardOption ( key,val );
						}
						if ((key== "MetaModes") && (driver == "i810")) {
							QList<QString> rList1=display->getResolution();
							QString resolution1 = rList1.at(0);
							QList<QString> rList2=dualData->getResolutionList();
							QString resolution2 = rList2.at(0);
							//====================================
							// add primary meta mode
							//------------------------------------
							QString metaItem;
							QString resolution;
							QTextOStream (&metaItem) <<
								resolution1 << "-" << resolution2;
							resolution.append (";"+metaItem);
							//====================================
							// add secondary meta modes
							//------------------------------------
							bool startMeta = false;
							QString ir;
							foreach (ir,rList1) {
								if (! startMeta) {
									startMeta = true; continue;
								}
								QString metaItem;
								int cmpr = compareResolution (ir,resolution2);
								if (cmpr >= 0) {
									QTextOStream (&metaItem) <<
										ir << "-" << resolution2;
								} else {
									QTextOStream (&metaItem) <<
										ir << "-" << ir;
								}
								resolution.append (";"+metaItem);
							}
							resolution.replace(QRegExp("^;"),"");
							saxCard.addCardOption ( key,resolution );
						}
						if (key == "Clone") {
							int orientation  = dualData->getLayout();
							QString position = DUAL_LEFTOF_KEY;
							switch (orientation) {
								case DUAL_ABOVEOF:
									position = DUAL_ABOVEOF_KEY;
								break;
								case DUAL_RIGHTOF:
									position = DUAL_RIGHTOF_KEY;
								break;
								case DUAL_BELOWOF:
									position = DUAL_BELOWOF_KEY;
								break;
								default:
								break;
							}
							saxCard.removeCardOption (key);
							saxCard.removeCardOption ("MergedFB");
							int mode = dualData->getMode();
							if ( mode == DUAL_CLONE ) {
								saxCard.addCardOption ("Clone","yes");
							} else {
								saxCard.addCardOption ("MergedFB","yes");
								saxCard.addCardOption (
									"SecondPosition",position
								);
							}
						}
						//====================================
						// setup profile FGLRX data
						//------------------------------------
						if (key== "ForceMonitors") {
							saxCard.addCardOption ( key,val );
						}
						if (key == "DesktopSetup") {
							int orientation  = dualData->getLayout();
							QString position = DUAL_FGLRX_LEFTOF_KEY;
							switch (orientation) {
								case DUAL_ABOVEOF:
									position = DUAL_FGLRX_ABOVEOF_KEY;
								break;
								case DUAL_RIGHTOF:
									position = DUAL_FGLRX_RIGHTOF_KEY;
								break;
								case DUAL_BELOWOF:
									position = DUAL_FGLRX_BELOWOF_KEY;
								break;
								default:
								break;
							}
							saxCard.addCardOption ( key,position );
							int mode = dualData->getMode();
							if ( mode == DUAL_CLONE ) {
								saxCard.removeCardOption (key);
								saxCard.addCardOption (key,DUAL_CLONE_KEY);
							}
						}
						if (key == "HSync2") {
							QString hsync;
							int hsmax = dualModel->getHSmax();
							int hsmin = dualModel->getHSmin();
							QTextOStream (&hsync) << hsmin << "-" << hsmax;
							saxCard.addCardOption ( key,hsync );
						}
						if (key == "VRefresh2") {
							QString vsync;
							int vsmax = dualModel->getVSmax();
							int vsmin = dualModel->getVSmin();
							QTextOStream (&vsync) << vsmin << "-" << vsmax;
							saxCard.addCardOption ( key,vsync );
						}
						if (key == "Mode2") {
							QString resolution;
							QList<QString> rList2=dualData->getResolutionList();
							QString ir;
							foreach(ir,rList2) {
								resolution.append(","+ir);
							}
							resolution.replace(QRegExp("^,"),"");
							saxCard.addCardOption ( key,resolution );
						}
					}
				}
			}
		}
		//}
		card++;
	}
	//====================================
	// save arrangement
	//------------------------------------
	if ( mMonitorArrange ) {
		//====================================
		// create manipulators...
		//------------------------------------
		SaXManipulateLayout saxLayout (
			mSection["Layout"],mSection["Card"]
		);
		//====================================
		// save multihead state
		//------------------------------------
		int multiheadMode = mMonitorArrange->getMultiheadMode();
		switch (multiheadMode) {
			case 0:
				saxLayout.setXOrgMultiheadMode ( SAX_TRADITIONAL );
			break;
			case 1:
				saxLayout.setXOrgMultiheadMode ( SAX_CLONE );
			break;
			case 2:
				saxLayout.setXOrgMultiheadMode ( SAX_XINERAMA );
			break;
			default:
			break;
		};
		//====================================
		// save XOrg layout
		//------------------------------------
		Q3Dict<QString> arrangeDict = mMonitorArrange->getArrangement();
		Q3DictIterator<QString> it (arrangeDict);
		for (; it.current(); ++it) {
			QString item (it.currentKey());
			item.replace (QRegExp("Screen:Screen\\["),"");
			item.replace (QRegExp("\\]"),"");
			int index = item.toInt();

			int count = 0;
			int left=-1,right=-1,above=-1,below=-1;
			QStringList tokens = QStringList::split (" ", *it.current());
			for (QStringList::Iterator it=tokens.begin();it!=tokens.end();++it){
				QString value (*it);
				int id = -1;
				if (value != "<none>") {
					value.replace (QRegExp("Screen\\["),"");
					value.replace (QRegExp("\\]"),"");
					id = value.toInt();
				}
				switch (count) {
					case 0: left  = id; break;
					case 1: right = id; break;
					case 2: above = id; break;
					case 3: below = id; break;
					default:
					break;
				}
				count++;
			}
			saxLayout.setXOrgLayout (
				index,left,right,above,below
			);
		}
	}
	//====================================
	// save 3D status
	//------------------------------------
	SaXManipulateDesktop saxDesktop (
		mSection["Desktop"],mSection["Card"],mSection["Path"]
	);
	saxDesktop.disable3D();
	if (mCheck3D->isChecked()) {
		saxDesktop.enable3D();
	}
	//====================================
	// save default color and pref mode
	//------------------------------------
	card = 0;
	foreach (display, mMonitorDisplay) {
		SaXManipulateDesktop saxDesktop (
			mSection["Desktop"],mSection["Card"],mSection["Path"]
		);
		SaXManipulateCard saxCard (
			mSection["Card"]
		);
		saxDesktop.selectDesktop ( card );
		saxCard.selectCard ( card );
		if (saxCard.getCardDriver() != "fbdev") {
			saxDesktop.setColorDepth ( display->getColorDepth() );
			saxDesktop.setPreferredMode (
				display->getResolution().at(0)
			);
		}
		card++;
	}
	return true;
}

//====================================
// compareResolution
//------------------------------------
int SCCMonitor::compareResolution ( const QString& r1,const QString& r2 ) {
	QStringList tokens1 = QStringList::split ( "x",r1 );
	int x1 = tokens1.first().toInt();
	int y1 = tokens1.last().toInt();
	int p1 = x1 * y1;
	QStringList tokens2 = QStringList::split ( "x",r2 );
	int x2 = tokens2.first().toInt();
	int y2 = tokens2.last().toInt();
	int p2 = x2 * y2;
	if ( p1 == p2 ) {
		return 0;
	}
	if ( p1 > p2 ) {
		return 1;
	}
	return -1;
}
//====================================
// setCommonButtonWidth
//------------------------------------
void SCCMonitor::setCommonButtonWidth ( void ) {
	SCCMonitorDisplay* display;
	foreach (display,mMonitorDisplay) {
//		SCCMonitorDisplay* display = it;
		display->setCommonButtonWidth();
	}
}
//====================================
// setRequestedDialog
//------------------------------------
void SCCMonitor::setRequestedDialog ( QString* dialog ) {
	mRequestedDialog = dialog;
	if (mRequestedDialog) {
		mShow3DMessage = false;
	}
}
#if 1 // to be removed as soon as possible
//====================================
// fixBrokenCards
//------------------------------------
void SCCMonitor::fixBrokenCards ( SaXManipulateCard& saxCard ) {
	SaXImportSysp* desktop = new SaXImportSysp (SYSP_DESKTOP);
	desktop->doImport();
	QString rawDefinition (desktop->getItem("RawDef"));
	if ((!rawDefinition.isNull()) && (rawDefinition != "None")) {
		QRegExp itemExp ("\"MonitorLayout\" \"(.*)\"");
		int pos = itemExp.search (rawDefinition);
		if (pos >= 0) {
			QString mLayout = itemExp.cap(1);
			QStringList tokens = QStringList::split ( ",",mLayout );
			QString layoutA = tokens.first();
			QString layoutB = tokens.last();
			log (L_INFO,"Using RawDef MonitorLayout: %s,%s\n",
				layoutA.ascii(),layoutB.ascii()
			);
			saxCard.removeCardOption ("MonitorLayout");
			saxCard.addCardOption ("MonitorLayout",
				layoutA+","+layoutB
			);
		}
	}
}
#endif
} // end namespace
