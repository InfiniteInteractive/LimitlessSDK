#ifndef _Limitless_QtComponents_MediaStoreView_h_
#define _Limitless_QtComponents_MediaStoreView_h_

#include <QtWidgets/QDockWidget>
#include <QtWidgets/QTreeWidget>

#include "media/MediaStore.h"
#include <memory>


namespace Ui
{
	class MediaStoreView;
}

namespace Limitless
{

Q_DECLARE_METATYPE(SharedMedia)



class MediaStoreView: public QDockWidget
{
	Q_OBJECT

public:
	MediaStoreView(std::shared_ptr<MediaStore> mediaStore, QWidget *parent=0);
	~MediaStoreView();

	void updateMediaTree();
	void updateMediaSet(QTreeWidgetItem *item, SharedMediaSet mediaSet);

	void mediaStoreUpdate();
	
public slots:
	void on_mediaLoad_clicked();
	void on_mediaSave_clicked();
	void on_mediaTree_itemClicked(QTreeWidgetItem * item, int column);
	void on_mediaTree_itemChanged(QTreeWidgetItem * item, int column);

	void onCurrentMediaSelected(SharedMediaVector media);

signals:
	void mediaSelected(SharedMedia media, bool selected);

private:
	SharedMedia getMedia(QTreeWidgetItem *item);

	std::vector<SharedMedia> getSelected();
	void getSelected(const QTreeWidgetItem *parent, std::vector<QTreeWidgetItem *> &selected);

//	std::unique_ptr<Ui::MediaStoreView> ui;
	Ui::MediaStoreView &ui;
	
	typedef std::map<IMedia *, QDockWidget *> MediaViewMap;
	MediaViewMap m_mediaViewMap;

	std::shared_ptr<MediaStore> m_mediaStore;

	SharedMediaVector m_mediaDisplay;

	static int MediaTypeId;
};

}//namespace Limitless

#endif // _Limitless_QtComponents_MediaStoreView_h_
