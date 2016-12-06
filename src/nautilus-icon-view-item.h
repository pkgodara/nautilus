#ifndef NAUTILUS_ICON_VIEW_ITEM_H
#define NAUTILUS_ICON_VIEW_ITEM_H

#include <glib.h>
#include <gtk/gtk.h>

#include "nautilus-file.h"

G_BEGIN_DECLS

#define NAUTILUS_TYPE_ICON_VIEW_ITEM (nautilus_icon_view_item_get_type())

G_DECLARE_FINAL_TYPE (NautilusIconViewItem, nautilus_icon_view_item, NAUTILUS, ICON_VIEW_ITEM, GtkFlowBoxChild)

NautilusIconViewItem * nautilus_icon_view_item_new (NautilusFile *file,
                                                    guint         icon_size);

void nautilus_icon_view_item_set_icon_size (NautilusIconViewItem *item,
                                            guint                 icon_size);

void nautilus_icon_view_item_set_file (NautilusIconViewItem *item,
                                       NautilusFile         *file);

NautilusFile * nautilus_icon_view_item_get_file (NautilusIconViewItem *item);

G_END_DECLS

#endif /* NAUTILUS_ICON_VIEW_ITEM_H */

