#include "ffmpegInputAttributeView.h"
#include "ffmpegInput.h"

#include <QtWidgets/QFileDialog>
#include <QtCore/QString>

#include <boost/filesystem.hpp>

FfmpegInputAttributeView::FfmpegInputAttributeView(FfmpegInput *input, QWidget *parent):
QWidget(parent),
m_input(input)
{
	ui.setupUi(this);

	ui.ok->setEnabled(false);

	if(m_input->exists("location"))
	{
		QString fileName=QString::fromStdString(m_input->attribute("location")->toString());

		ui.fileName->setText(fileName);
		checkFileExists(fileName);
	}

	bool loop=input->attribute("loop")->toBool();
	ui.loop->setCheckState(loop?Qt::Checked:Qt::Unchecked);
}

FfmpegInputAttributeView::~FfmpegInputAttributeView()
{

}

void FfmpegInputAttributeView::on_fileBrowse_clicked()
{
	QFileDialog dialog;

	dialog.setFileMode(QFileDialog::ExistingFile);

	if(m_input->exists("directory"))
		dialog.setDirectory(QString::fromStdString(m_input->attribute("directory")->toString()));

	QStringList filters;
	QString knownFilters;
	std::vector<FfmpegInput::Format> formats=m_input->getFormats();

	knownFilters="Files ( ";
	for(FfmpegInput::Format &format:formats)
	{
		if(!format.extension.empty())
			knownFilters+=QString("*.%1 ").arg(QString::fromStdString(format.extension));
	}
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

void FfmpegInputAttributeView::on_ok_clicked()
{
	m_input->setAttribute("location", ui.fileName->text().toStdString());

	emit accepted();
	close();
}

void FfmpegInputAttributeView::on_cancel_clicked()
{
	emit rejected();
	close();
}

void FfmpegInputAttributeView::on_fileName_textChanged(const QString &text)
{
	checkFileExists(text);
}

void FfmpegInputAttributeView::on_loop_stateChanged(int state)
{
	if(state == Qt::Checked)
		m_input->setAttribute("loop", true);
	else
		m_input->setAttribute("loop", false);
}

void FfmpegInputAttributeView::checkFileExists(QString fileName)
{
	boost::filesystem::path filePath=fileName.toStdString();

	if(boost::filesystem::exists(filePath))
		ui.ok->setEnabled(true);
	else
		ui.ok->setEnabled(false);
}