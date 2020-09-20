#ifndef SENDTOPASTEBIN_H
#define SENDTOPASTEBIN_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QSslConfiguration>
#include <QClipboard>
#include <QByteArray>
#include <QTimer>

#include <QDir>
#include <QStandardPaths>
#include <QSettings>
#include <QFont>

#include <QDesktopServices>
#include <QFontDatabase>

#include "popup.h"

static QString AppdataPath=QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).absoluteFilePath("SendToPastebin");
static QString ConfigFileName="config.ini";
static QString ConfigPath=QDir(AppdataPath).absoluteFilePath(ConfigFileName);

static QSettings SendToPastebinConfig(ConfigPath,QSettings::IniFormat);

class SendToPastebin: public QMainWindow
{
	Q_OBJECT

	public:
	SendToPastebin(QWidget *parent=0);
	~SendToPastebin();

	private:
	QSystemTrayIcon *trayIcon;
	QMenu *trayMenu;
	QAction *sendAction;
	QAction *sendCppAction;
	QAction *settingsAction;
	QAction *quitAction;

	QNetworkAccessManager *SendManager;
	QNetworkAccessManager *LoginManager;

	QTimer *timer;

	PopUp *popUp;

	QString ClipboardText;
	void GetClipboardText();

	void ReadConfigFile();
	void GetApiUserKey();

	QUrl pastebinpostapi="https://pastebin.com/api/api_post.php";
	QUrl pastebinloginapi="https://pastebin.com/api/api_login.php";

	QStringList ErrorsList={"Bad API request, use POST request, not GET","Bad API request, invalid api_dev_key","Bad API request, invalid login","Bad API request, account not active","Bad API request, invalid POST parameters","Bad API request, invalid api_option","Bad API request, maximum number of 25 unlisted pastes for your free account","Bad API request, maximum number of 10 private pastes for your free account","Bad API request, api_paste_code was empty","Bad API request, maximum paste file size exceeded","Bad API request, invalid api_paste_expire_date","Bad API request, invalid api_paste_private","Bad API request, invalid api_paste_format","Bad API request, invalid api_user_key","Bad API request, invalid or expired api_user_key"};

	QString api_dev_key;
	QString api_user_name;
	QString api_user_password;
	QString api_user_key;

	QString api_paste_format;

	private slots:
	void CloseApp();
	void SendManagerFinish(QNetworkReply *reply);
	void LoginManagerFinish(QNetworkReply *reply);
	void SendMessage();
	void SendCppMessage();
	void OpenSettings();

	void RestoreWindowTrigger(QSystemTrayIcon::ActivationReason reason);
};

#endif // SENDTOPASTEBIN_H
