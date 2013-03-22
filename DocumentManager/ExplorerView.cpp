#include "ExplorerView.h"

namespace Docs {

class ExplorerViewPrivate {
public:
    ExplorerViewPrivate(ExplorerView *pq)
        : q(pq)
    {
    }

private:
    ExplorerView *q;

    friend class ExplorerView;
};

ExplorerView::ExplorerView(QWidget *parent)
    : QTreeView(parent)
    , d(new ExplorerViewPrivate(this))
{
}

ExplorerView::~ExplorerView()
{
    if (d) {
        delete d;
    }
}

} //namespace Docs
