#include "mediaStoreView.h"

#include "Media/MediaFactory.h"
#include "ui_mediaStoreView.h"

//#include <boost/regex.hpp>
#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QMainWindow>

//#include "opencv2/stitching/detail/autocalib.hpp"
//#include "opencv2/stitching/detail/blenders.hpp"
//#include "opencv2/stitching/detail/camera.hpp"
//#include "opencv2/stitching/detail/exposure_compensate.hpp"
//#include "opencv2/stitching/detail/matchers.hpp"
//#include "opencv2/stitching/detail/motion_estimators.hpp"
//#include "opencv2/stitching/detail/seam_finders.hpp"
//#include "opencv2/stitching/detail/util.hpp"
//#include "opencv2/stitching/detail/warpers.hpp"
//#include "opencv2/stitching/warpers.hpp"

//#include "ImageMedia.h"
//#include "MediaUtility.h"


#include <boost/format.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

namespace Limitless
{

int MediaStoreView::MediaTypeId=qRegisterMetaType<SharedMedia>();

class RequestLoadHandler
{
public:
	RequestLoadHandler(SharedMediaVector *mediaVector):m_mediaVector(mediaVector){};
	~RequestLoadHandler(){};

	std::shared_ptr<IMedia> requestLoad(std::string name, std::string location)
	{
		for(size_t i=0; i<m_mediaVector->size(); ++i)
		{
			SharedMedia media=m_mediaVector->operator[](i);

			if((media->name() == name) && (media->location() == location))
				return media;
		}

		SharedMedia media;
//		SharedMedia media=Media::requestLoad(name, location);
//		
//		if(media != SharedMedia())
//			m_mediaVector->push_back(media);

		return media;
	}

private:
	SharedMediaVector *m_mediaVector;
};

MediaStoreView::MediaStoreView(std::shared_ptr<MediaStore> mediaStore, QWidget *parent):
QDockWidget(parent),
ui(Ui::MediaStoreView()),
m_mediaStore(mediaStore)
{
//	ui.reset(new Ui::MediaStoreView());
	ui.setupUi(this);

	ui.mediaTree->setColumnCount(2);

	QStringList headerNames;

	headerNames.append("View");
	headerNames.append("Name");
//	headerNames.append("Location");
	ui.mediaTree->setHeaderLabels(headerNames);
	ui.mediaTree->setIndentation(5);
	ui.mediaTree->setColumnWidth(0, 35);
//	ui.mediaTree->setColumnWidth(1, 95);

	m_mediaStore->addUpdateCallback(std::bind(&MediaStoreView::mediaStoreUpdate, this));
}

MediaStoreView::~MediaStoreView()
{

}

void MediaStoreView::on_mediaLoad_clicked()
{
	QFileDialog dialog;
	QSettings settings;

	dialog.setFileMode(QFileDialog::ExistingFiles);
	
	QString lastDirectory=settings.value("loadMedia/lastDirectory", "").toString();

	if(!lastDirectory.isEmpty())
		dialog.setDirectory(lastDirectory);

	QString fileExtensions;
	std::vector<std::string> mediaTypes=MediaFactory::instance().names();
	std::string allExtensions="All (";

	for(size_t i=0; i<mediaTypes.size(); ++i)
	{
		std::vector<std::string> mediaExtensions=MediaFactory::instance().extensions(mediaTypes[i]);

		if(!mediaExtensions.empty())
		{
			std::string mediaExtentionList=mediaTypes[i];
			
			mediaExtentionList+=(boost::format(" (*.%s")%mediaExtensions[0]).str();
			allExtensions+=(boost::format(" *.%s")%mediaExtensions[0]).str();
			for(size_t j=1; j<mediaExtensions.size(); ++j)
			{
				mediaExtentionList+=(boost::format(" *.%s")%mediaExtensions[j]).str();
				allExtensions+=(boost::format(" *.%s")%mediaExtensions[j]).str();
			}
			mediaExtentionList+=")";

			if(i == 0)
				fileExtensions+=QString::fromStdString(mediaExtentionList);
			else
				fileExtensions+=QString(";; ")+QString::fromStdString(mediaExtentionList);
		}
	}
	allExtensions+=" );; ";

	if(fileExtensions.length() > 0)
		fileExtensions=QString::fromStdString(allExtensions)+fileExtensions;

	dialog.setNameFilter(fileExtensions);

	QStringList fileNames;

	if (dialog.exec())
	{
		settings.setValue("loadMedia/lastDirectory", dialog.directory().path());
		fileNames=dialog.selectedFiles();
		SharedMediaVector images;
		RequestLoadHandler requestLoadHandler(&images);

		for (int i=0; i<fileNames.size(); ++i)
		{
			QFileInfo fileInfo(fileNames.at(i));

			if(!fileInfo.exists())
				continue;

			std::vector<std::string> extHandlers=MediaFactory::instance().handlesExtension(fileInfo.suffix().toStdString());

			if(extHandlers.empty())
				continue;

			SharedMedia media=MediaFactory::instance().create(extHandlers[0]);

			media->load(fileInfo.absoluteFilePath().toStdString(), std::bind(&RequestLoadHandler::requestLoad, &requestLoadHandler, std::placeholders::_1, std::placeholders::_2));
			images.push_back(media);
		}

		std::string setName=(boost::format("Set%d")%m_mediaStore->mediaSets()).str();
		SharedMediaSet mediaSet(new MediaSet(setName, images));

		m_mediaStore->pushMediaSet(mediaSet);
	}
}

void MediaStoreView::on_mediaSave_clicked()
{
	SharedMediaVector selectedMedia=getSelected();

	for(size_t i=0; i<selectedMedia.size(); ++i)
	{
		selectedMedia[i]->save();
	}
}

void MediaStoreView::updateMediaTree()
{
	size_t mediaSetCount=m_mediaStore->mediaSets();

	if(mediaSetCount <= 0)
	{
		ui.mediaTree->clear();
		return;
	}

	size_t i;
	for(i=0; i<mediaSetCount; ++i)
	{
		SharedMediaSet mediaSet=m_mediaStore->mediaSet(i);
		QTreeWidgetItem *item=ui.mediaTree->topLevelItem(i);

		if(item != NULL)
		{
			if(item->text(0) != QString::fromStdString(mediaSet->name()))
			{
				item=new QTreeWidgetItem();

				item->setText(0, QString::fromStdString(mediaSet->name()));
				ui.mediaTree->insertTopLevelItem(i, item);
				ui.mediaTree->setFirstItemColumnSpanned(item, true);
			}
		}
		else
		{
			item=new QTreeWidgetItem();

			item->setText(0, QString::fromStdString(mediaSet->name()));
			ui.mediaTree->insertTopLevelItem(i, item);
			ui.mediaTree->setFirstItemColumnSpanned(item, true);
		}
		updateMediaSet(item, mediaSet);

	}

	int topLevelCount=ui.mediaTree->topLevelItemCount();
	
	for(int j=i; j<topLevelCount; ++j)
		delete ui.mediaTree->takeTopLevelItem(i);
}

void MediaStoreView::updateMediaSet(QTreeWidgetItem *item, SharedMediaSet mediaSet)
{
	size_t count=mediaSet->size();

	size_t i;
	for(i=0; i<count; ++i)
	{
		SharedMedia media=mediaSet->get(i);
		QTreeWidgetItem *mediaItem=item->child(i);

		if(mediaItem != NULL)
		{
//			if(mediaItem->text(0) == QString::fromStdString(media->name()))
			if(mediaItem->data(0, Qt::UserRole).value<SharedMedia>().get() == media.get())
				continue;
		}
			
		mediaItem=new QTreeWidgetItem();

//		QCheckBox *checkBox=new QCheckBox();
//
//		ui.mediaTree->setItemWidget(mediaItem, 0, checkBox);
		mediaItem->setText(1, QString::fromStdString(media->name()).arg(i));
//		mediaItem->setText(2, QString::fromStdString(media->location()));
		mediaItem->setData(0, Qt::UserRole, QVariant::fromValue(media));
		mediaItem->setFlags(mediaItem->flags()|Qt::ItemIsUserCheckable);
		mediaItem->setCheckState(0, Qt::Unchecked);

		item->insertChild(i, mediaItem);
	}

	int childCount=item->childCount();

	for(int j=i; j<childCount; ++j)
		delete item->takeChild(i);
}

void MediaStoreView::mediaStoreUpdate()
{
	updateMediaTree();
}

void MediaStoreView::on_mediaTree_itemClicked(QTreeWidgetItem * item, int column)
{
	const QTreeWidgetItem *root=ui.mediaTree->invisibleRootItem();
	std::vector<QTreeWidgetItem *> selected;
	SharedMediaVector selectedMedia;
	SharedMediaVector selectedMediaToDisplay;
	SharedMediaVector selectedMediaView;

	SharedMedia itemMedia=getMedia(item);

	if(itemMedia != SharedMedia())
	{
		SharedMediaSet mediaSet=std::dynamic_pointer_cast<MediaSet>(itemMedia);

		if(mediaSet != SharedMediaSet())
		{
		}
		else
			emit mediaSelected(itemMedia, item->isSelected());
	}
/*
	getSelected(root, selected);

	if(selected.size() > 0)
	{
		for(size_t i=0; i<selected.size(); ++i)
		{
			SharedMedia media=getMedia(selected[i]);

			selectedMedia.push_back(media);
			if(media->displayType() == IMedia::Display_2D)
				selectedMediaToDisplay.push_back(media);
			else
				selectedMediaView.push_back(media);
		}
	}

	m_mediaStore->setSelectedMedia(selectedMedia);
//	m_view->displayMedia(selectedMediaToDisplay, false);

	if(!selectedMediaView.empty())
	{
		for(size_t i=0; i<selectedMediaView.size(); ++i)
		{
			IMedia *media=selectedMediaView[i].get();

			MediaViewMap::iterator iter=m_mediaViewMap.find(media);

			if(iter == m_mediaViewMap.end())
			{
				GLWidget *new3dView=new GLWidget(true);

				m_3dViews.push_back(new3dView);
				new3dView->displayMedia(selectedMediaView[i], false);
		
				QMainWindow *calibrationWidget=dynamic_cast<QMainWindow *>(parentWidget());

				if(calibrationWidget != NULL)
				{
					QDockWidget *dockWidget=new QDockWidget();

//					m_mediaViewMap[media]=new3dView;
					m_mediaViewMap[media]=dockWidget;
					dockWidget->setWidget(new3dView);
					calibrationWidget->addDockWidget(Qt::RightDockWidgetArea, dockWidget);
				}
			}
		}
	}
*/
}

void MediaStoreView::on_mediaTree_itemChanged(QTreeWidgetItem * item, int column)
{
	if(column == 0)
	{
		SharedMedia itemMedia=item->data(0, Qt::UserRole).value<SharedMedia>();

		SharedMediaVector::iterator iter=std::find_if(m_mediaDisplay.begin(), m_mediaDisplay.end(), 
			[itemMedia](const SharedMedia &displayMedia){return displayMedia.get() == itemMedia.get();});

		if(item->checkState(0) == Qt::Checked)
		{
//			if(iter == m_mediaDisplay.end())
//			{
//				if(itemMedia->displayType() == IMedia::Display_2D)
//					m_mediaDisplay.push_back(itemMedia);
//				else
//				{
//					IMedia *media=itemMedia.get();
//					MediaViewMap::iterator iter=m_mediaViewMap.find(media);
//
//					if(iter == m_mediaViewMap.end())
//					{
//						GLWidget *new3dView=new GLWidget(true);
//
//						m_3dViews.push_back(new3dView);
//						new3dView->displayMedia(itemMedia, false);
//
//						QMainWindow *calibrationWidget=dynamic_cast<QMainWindow *>(parentWidget());
//
//						if(calibrationWidget != NULL)
//						{
//							QDockWidget *dockWidget=new QDockWidget();
//
////							m_mediaViewMap[media]=new3dView;
//							m_mediaViewMap[media]=dockWidget;
//							dockWidget->setWidget(new3dView);
//							calibrationWidget->addDockWidget(Qt::RightDockWidgetArea, dockWidget);
//							dockWidget->setFloating(true);
//						}
//					}
//					else
//						iter->second->raise();
//				}
//			}
		}
		else
		{
//			if(iter != m_mediaDisplay.end())
//			{
//				if(itemMedia->displayType() == IMedia::Display_2D)
//					m_mediaDisplay.erase(iter);
//				else
//				{
//					IMedia *media=itemMedia.get();
//					MediaViewMap::iterator iter=m_mediaViewMap.find(media);
//
//					if(iter != m_mediaViewMap.end())
//						iter->second->close();
//				}
//			}
		}

//		m_view->displayMedia(m_mediaDisplay, false);
	}
}

void MediaStoreView::onCurrentMediaSelected(SharedMediaVector media)
{
	blockSignals(true);

	int topLevelCount=ui.mediaTree->topLevelItemCount();
	
	for(int i=0; i<topLevelCount; ++i)
	{
		QTreeWidgetItem *topLevelItem=ui.mediaTree->topLevelItem(i);

		for(int j=0; j<topLevelItem->childCount(); ++j)
		{
			QTreeWidgetItem *childItem=topLevelItem->child(j);

			SharedMediaVector::iterator iter=std::find_if(media.begin(), media.end(), 
				[childItem](const SharedMedia &inputMedia){return inputMedia.get() == childItem->data(0, Qt::UserRole).value<SharedMedia>().get();});

			if(iter != media.end())
				childItem->setSelected(true);
			else
				childItem->setSelected(false);
		}
	}

	blockSignals(false);
}

SharedMedia MediaStoreView::getMedia(QTreeWidgetItem *item)
{
	int topLevelItemIndex=ui.mediaTree->indexOfTopLevelItem(item);

	if(topLevelItemIndex != -1)
	{
		QTreeWidgetItem *topLevelItem=ui.mediaTree->topLevelItem(topLevelItemIndex);

		if(topLevelItem == item)
		{
			SharedMediaSet mediaSet=m_mediaStore->mediaSet(topLevelItemIndex);
			SharedMedia media=mediaSet->get(0);

			return media;
		}
	}
	else
	{
		QTreeWidgetItem *parent=item->parent();
		
		if(parent != NULL)
		{
			topLevelItemIndex=ui.mediaTree->indexOfTopLevelItem(parent);
			int childIndex=parent->indexOfChild(item);

			if((topLevelItemIndex != -1) && (childIndex != -1))
			{
				SharedMediaSet mediaSet=m_mediaStore->mediaSet(topLevelItemIndex);
				SharedMedia media=mediaSet->get(childIndex);

				return media;
			}
		}
	}
	return SharedMedia();
}

std::vector<SharedMedia> MediaStoreView::getSelected()
{
	QList<QTreeWidgetItem *> selectedItems=ui.mediaTree->selectedItems();
	SharedMediaVector selectedMedia;

	foreach(QTreeWidgetItem *item, selectedItems)
	{
		SharedMedia media=getMedia(item);

		if(media != SharedMedia())
			selectedMedia.push_back(media);
	}
	return selectedMedia;
}

void MediaStoreView::getSelected(const QTreeWidgetItem *parent, std::vector<QTreeWidgetItem *> &selected)
{
	int childCount=parent->childCount();

	for(int i=0; i<childCount; ++i)
	{
		QTreeWidgetItem *child=parent->child(i);

		if(child->childCount() > 0)
			getSelected(child, selected);
		else
		{
			if(child->isSelected())
				selected.push_back(child);
		}
	}
}

}//namspace Limitless