#include "sendtopastebin.h"
#include <QDebug>

static QFont RobotoNormal10("Roboto",10,QFont::Normal);

SendToPastebin::SendToPastebin(QWidget *parent): QMainWindow(parent)
{
	QFontDatabase::addApplicationFont(":/fonts/roboto_regular");
	QFontDatabase::addApplicationFont(":/fonts/roboto_medium");
	QFontDatabase::addApplicationFont(":/fonts/roboto_bold");

	if(!QDir(AppdataPath).exists()) QDir().mkpath(AppdataPath);

	trayIcon=new QSystemTrayIcon(this);
	trayIcon->setIcon(QIcon(":/icons/icons/send.svg"));

	trayMenu=new QMenu(this);
	sendAction=new QAction("Загрузить на Pastebin",this);
	sendAction->setFont(RobotoNormal10);
	sendAction->setIcon(QIcon(":/icons/icons/upload.svg"));
	sendCppAction=new QAction("Загрузить на Pastebin как C++",this);
	sendCppAction->setFont(RobotoNormal10);
	sendCppAction->setIcon(QIcon(":/icons/icons/cpp.svg"));
	settingsAction=new QAction("Открыть файл настроек",this);
	settingsAction->setFont(RobotoNormal10);
	settingsAction->setIcon(QIcon(":/icons/icons/settings.svg"));
	quitAction=new QAction("Закрыть",this);
	quitAction->setFont(RobotoNormal10);
	quitAction->setIcon(QIcon(":/icons/icons/exit.svg"));
	trayMenu->addAction(sendAction);
	trayMenu->addAction(sendCppAction);
	trayMenu->addAction(settingsAction);
	trayMenu->addAction(quitAction);
	trayIcon->setContextMenu(trayMenu);
	trayIcon->show();

	SendManager=new QNetworkAccessManager(this);
	connect(SendManager,&QNetworkAccessManager::finished,this,&SendToPastebin::SendManagerFinish);

	LoginManager=new QNetworkAccessManager(this);
	connect(LoginManager,&QNetworkAccessManager::finished,this,&SendToPastebin::LoginManagerFinish);

	connect(trayIcon,&QSystemTrayIcon::activated,this,&SendToPastebin::RestoreWindowTrigger);
	connect(sendAction,&QAction::triggered,this,&SendToPastebin::SendMessage);
	connect(sendCppAction,&QAction::triggered,this,&SendToPastebin::SendCppMessage);
	connect(settingsAction,&QAction::triggered,this,&SendToPastebin::OpenSettings);
	connect(quitAction,&QAction::triggered,this,&SendToPastebin::CloseApp);

	timer=new QTimer(this);
	connect(timer,&QTimer::timeout,this,&SendToPastebin::GetClipboardText);
	timer->start(100);
	popUp=new PopUp();

	ReadConfigFile();
}

void SendToPastebin::ReadConfigFile()
{
	if(QFile::exists(ConfigPath))
	{
		api_dev_key=SendToPastebinConfig.value("api_dev_key").toString();

		if(!api_dev_key.isEmpty())
		{
			api_user_name=SendToPastebinConfig.value("api_user_name").toString();
			api_user_password=SendToPastebinConfig.value("api_user_password").toString();
			api_user_key=SendToPastebinConfig.value("api_user_key").toString();

			if(api_user_key.isEmpty()) if(!api_user_name.isEmpty() && !api_user_password.isEmpty()) GetApiUserKey();
		}
		else
		{
			popUp->setPopupText("api_dev_key не найден в конфиге");
			popUp->show();
		}
	}
	else
	{
		popUp->setPopupText("Не найден файл конфига");
		popUp->show();
	}
}

void SendToPastebin::RestoreWindowTrigger(QSystemTrayIcon::ActivationReason reason)
{
	if(reason==QSystemTrayIcon::DoubleClick) SendMessage();
}

SendToPastebin::~SendToPastebin()
{
	trayIcon->hide();
	trayIcon->deleteLater();
	trayMenu->deleteLater();
	sendAction->deleteLater();
	settingsAction->deleteLater();
	quitAction->deleteLater();
	SendManager->deleteLater();
	timer->deleteLater();
}


void SendToPastebin::GetClipboardText()
{
	ClipboardText=QApplication::clipboard()->text();
}

void SendToPastebin::SendManagerFinish(QNetworkReply *reply)
{
	QString data=reply->readAll();
	qDebug() << data;
	if(!data.isEmpty())
	{
		if(!ErrorsList.contains(data))
		{
			QApplication::clipboard()->setText(data);
			popUp->setPopupText("Загружено на Pastebin!\nСсылка сохранена в буфер.");
			popUp->show();
		}
		else
		{
			popUp->setPopupText("Ошибка загрузки на Pastebin!\n"+data);
			popUp->show();
		}
	}
}

void SendToPastebin::LoginManagerFinish(QNetworkReply *reply)
{
	QString data=reply->readAll();
	qDebug() << data;
	if(!data.isEmpty())
	{
		if(!ErrorsList.contains(data))
		{
			api_user_key=data;
			SendToPastebinConfig.setValue("api_user_key",api_user_key);
		}
		else
		{
			popUp->setPopupText("api_user_key получен не был!\n"+data);
			popUp->show();
		}
	}	
}

void SendToPastebin::CloseApp()
{
	QApplication::exit();
}

void SendToPastebin::SendMessage()
{
	if(!ClipboardText.isEmpty())
	{
		if(!api_dev_key.isEmpty())
		{
			QNetworkRequest request;
			request.setUrl(pastebinpostapi);
			request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
			request.setAttribute(QNetworkRequest::FollowRedirectsAttribute,true);
			request.setRawHeader("Content-type","application/x-www-form-urlencoded");

			QByteArray data;
			QUrlQuery params;

			params.addQueryItem("api_option","paste");
			if(!api_user_key.isEmpty()) params.addQueryItem("api_user_key",api_user_key);
			params.addQueryItem("api_paste_private","1");
			params.addQueryItem("api_paste_format",api_paste_format);
			params.addQueryItem("api_dev_key",api_dev_key);
			params.addQueryItem("api_paste_code",ClipboardText);

			data.append(params.toString());
			qDebug() << params.toString() << "\n" << data;

			SendManager->post(request,data);
		}
		else
		{
			popUp->setPopupText("api_dev_key неизвестен, отправки не будет!");
			popUp->show();
		}
	}
	api_paste_format.clear();
}

void SendToPastebin::SendCppMessage()
{
	api_paste_format="cpp";
	SendMessage();
}

void SendToPastebin::OpenSettings()
{
	if(QFile::exists(ConfigPath)) QDesktopServices::openUrl(QUrl(ConfigPath,QUrl::TolerantMode));
	else
	{
		popUp->setPopupText("Не найден файл конфига");
		popUp->show();
	}
}

void SendToPastebin::GetApiUserKey()
{
	QNetworkRequest request;
	request.setUrl(pastebinloginapi);
	request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
	request.setAttribute(QNetworkRequest::FollowRedirectsAttribute,true);
	request.setRawHeader("Content-type","application/x-www-form-urlencoded");

	QByteArray data;
	QUrlQuery params;
	params.addQueryItem("api_dev_key",api_dev_key);
	params.addQueryItem("api_user_name",api_user_name);
	params.addQueryItem("api_user_password",api_user_password);
	data.append(params.toString());

	LoginManager->post(request,data);
}