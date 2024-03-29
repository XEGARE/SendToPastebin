#include "popup.h"
#include <QPainter>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QFontDatabase>
#include <QFont>

static QFont RobotoNormal12("Roboto",12,QFont::Normal);

PopUp::PopUp(QWidget *parent): QWidget(parent)
{

	QFontDatabase::addApplicationFont(":/fonts/roboto_regular");
	QFontDatabase::addApplicationFont(":/fonts/roboto_medium");
	QFontDatabase::addApplicationFont(":/fonts/roboto_bold");

	setWindowFlags(Qt::FramelessWindowHint |        // ��������� ���������� ����
		Qt::Tool |                       // �������� ����� � �������� ���������� ����
		Qt::WindowStaysOnTopHint);       // ������������� ������ ���� ����
	setAttribute(Qt::WA_TranslucentBackground);     // ���������, ��� ��� ����� ����������
	setAttribute(Qt::WA_ShowWithoutActivating);     // ��� ������, ������ �� ���������� ������ �������������

	animation.setTargetObject(this);                // ������������� ������� ������ ��������
	animation.setPropertyName("popupOpacity");      // ������������� ����������� ��������
	connect(&animation,&QAbstractAnimation::finished,this,&PopUp::hide); /* ���������� ������ ���������
																		 * �������� � ����� �������
																		 * */

																		 // ��������� ������ �����������
	
	label.setFont(RobotoNormal12);
	label.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter); // ������������� �� ������
															 // � ����������� �����
	label.setStyleSheet("QLabel { color : white; "
		"margin-top: 6px;"
		"margin-bottom: 6px;"
		"margin-left: 10px;"
		"margin-right: 10px; }");

	// ���������� ��������� ������ � ����������, ...
	layout.addWidget(&label,0,0);
	setLayout(&layout); // ������� �������� � ������

						// �� ������� ������� ����� ����������� ������� �����������, ���� ��� ������
	timer=new QTimer();
	connect(timer,&QTimer::timeout,this,&PopUp::hideAnimation);
}

void PopUp::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event)

		/* � ������ ����������� ��� �����������,
		* ������� �������� ��������������� � ������ �����
		* */
		QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing); // �������� �����������

												   // �������������� ���. rect() ���������� ���������� ��������� ������� �����������, �� �����������
	QRect roundedRect;
	roundedRect.setX(rect().x() + 5);
	roundedRect.setY(rect().y() + 5);
	roundedRect.setWidth(rect().width() - 10);
	roundedRect.setHeight(rect().height() - 10);

	// ����� ����������� �� ������ ���� � ������ ���������������� 180 �� 255
	painter.setBrush(QBrush(QColor(0,0,0,180)));
	painter.setPen(Qt::NoPen); // ���� ����������� �� ����� �������

							   // ������������ ��� � ������������ ���� � 10px
	painter.drawRoundedRect(roundedRect,10,10);
}

void PopUp::setPopupText(const QString &text)
{
	label.setText(text);    // ������������� ����� � Label
	adjustSize();           // � ���������� �������� �����������
}

void PopUp::show()
{
	setWindowOpacity(0.0);  // ������������� ������������ � ����

	animation.setDuration(150);     // ����������� ������������ ��������
	animation.setStartValue(0.0);   // ��������� �������� ����� 0 (��������� ���������� ������)
	animation.setEndValue(1.0);     // �������� - ��������� ������������ ������

	setGeometry(QApplication::desktop()->availableGeometry().width() - 36 - width() + QApplication::desktop()->availableGeometry().x(),
		QApplication::desktop()->availableGeometry().height() - 36 - height() + QApplication::desktop()->availableGeometry().y(),
		width(),
		height());
	QWidget::show();                // ���������� ������, ������� ��������� ���������

	animation.start();              // � ��������� ��������
	timer->start(5000);             // � ����� �������� ������, ������� �������� ������� ����������� ����� 3 �������
}

void PopUp::hideAnimation()
{
	timer->stop();                  // ������������� ������
	animation.setDuration(1000);    // ����������� ������������ ��������
	animation.setStartValue(1.0);   // ��������� �������� ����� 1 (��������� ������������ ������)
	animation.setEndValue(0.0);     // �������� - ��������� ���������� ������
	animation.start();              // � ��������� ��������
}

void PopUp::hide()
{
	// ���� ������ ����������, �� �������� ���
	if(getPopupOpacity() == 0.0)
	{
		QWidget::hide();
	}
}

void PopUp::setPopupOpacity(float opacity)
{
	popupOpacity=opacity;

	setWindowOpacity(opacity);
}

float PopUp::getPopupOpacity() const
{
	return popupOpacity;
}