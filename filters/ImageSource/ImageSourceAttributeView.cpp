#include "ImageSourceAttributeView.h"
#include "imageSource.h"

#include <QtWidgets/QFileDialog>
#include <QtCore/QString>

#include <boost/filesystem.hpp>

ImageSourceAttributeView::ImageSourceAttributeView(ImageSource *source, QWidget *parent):
QWidget(parent),
m_source(source)
{
	ui.setupUi(this);

	ui.ok->setEnabled(false);

	if(m_source->exists("location"))
	{
		QString fileName=QString::fromStdString(m_source->attribute("location")->toString());

		ui.fileName->setText(fileName);
		checkFileExists(fileName);
	}
}

ImageSourceAttributeView::~ImageSourceAttributeView()
{

}

void ImageSourceAttributeView::on_fileBrowse_clicked()
{
	QFileDialog dialog;

	dialog.setFileMode(QFileDialog::ExistingFile);

	if(m_source->exists("directory"))
		dialog.setDirectory(QString::fromStdString(m_source->attribute("directory")->toString()));

	QStringList filters;
	QString knownFilters;

	knownFilters="Png Files ( *.png )";
	knownFilters+=")";
	filters << knownFilters << "Any files (*)";
	dialog.setNameFilters(filters);

	QStringList fileNames;

	if(dialog.exec())
	{
		QStringList fileNames=dialog.selectedFiles();

		if(fileNames.size() > 0)
		{
			ui.fileName->setText(fileNames[0]);
			checkFileExists(fileNames[0]);
		}
	}
}

void ImageSourceAttributeView::on_ok_clicked()
{
	m_source->setAttribute("location", ui.fileName->text().toStdString());

	emit accepted();
	close();
}

void ImageSourceAttributeView::on_cancel_clicked()
{
	emit rejected();
	close();
}

void ImageSourceAttributeView::on_fileName_textChanged(const QString &text)
{
	checkFileExists(text);
}

void ImageSourceAttributeView::checkFileExists(QString fileName)
{
	boost::filesystem::path filePath=fileName.toStdString();

	if(boost::filesystem::exists(filePath))
		ui.ok->setEnabled(true);
	else
		ui.ok->setEnabled(false);
}