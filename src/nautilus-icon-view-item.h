#ifndef NAUTILUS_ICON_VIEW_ITEM_H
#define NAUTILUS_ICON_VIEW_ITEM_H

#include <glib.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define NAUTILUS_TYPE_ICON_VIEW_ITEM (nautilus_icon_view_item_get_type())

G_DECLARE_FINAL_TYPE (NautilusIconViewItem, nautilus_icon_view_item, NAUTILUS, ICON_VIEW_ITEM, GtkBin)

NautilusIconViewItem *nautilus_icon_view_item_new (void);

void
nautilus_icon_view_item_set_max_width (NautilusIconViewItem *self,
                                       guint                 max_width);
guint
nautilus_icon_view_item_get_max_width (NautilusIconViewItem *self);

G_END_DECLS

#endif /* NAUTILUS_ICON_VIEW_ITEM_H */

