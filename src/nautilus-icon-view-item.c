#include "nautilus-icon-view-item.h"

struct _NautilusIconViewItem
{
    GtkFlowBoxChild parent_instance;
    GtkWidget *item_container;
};

G_DEFINE_TYPE (NautilusIconViewItem, nautilus_icon_view_item, GTK_TYPE_FLOW_BOX_CHILD)

enum
{
    PROP_0,
    PROP_FILE,
    N_PROPS
};

static GParamSpec *properties [N_PROPS];

NautilusIconViewItem *
nautilus_icon_view_item_new (void)
{
    return g_object_new (NAUTILUS_TYPE_ICON_VIEW_ITEM, NULL);
}

static void
nautilus_icon_view_item_finalize (GObject *object)
{
    NautilusIconViewItem *self = (NautilusIconViewItem *) object;

    G_OBJECT_CLASS (nautilus_icon_view_item_parent_class)->finalize (object);
}

static void
nautilus_icon_view_item_get_property (GObject    *object,
                                      guint       prop_id,
                                      GValue     *value,
                                      GParamSpec *pspec)
{
    NautilusIconViewItem *self = NAUTILUS_ICON_VIEW_ITEM (object);

    switch (prop_id)
    {
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
nautilus_icon_view_item_set_property (GObject      *object,
                                      guint         prop_id,
                                      const GValue *value,
                                      GParamSpec   *pspec)
{
    NautilusIconViewItem *self = NAUTILUS_ICON_VIEW_ITEM (object);

    switch (prop_id)
    {
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
nautilus_icon_view_item_class_init (NautilusIconViewItemClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = nautilus_icon_view_item_finalize;
    object_class->get_property = nautilus_icon_view_item_get_property;
    object_class->set_property = nautilus_icon_view_item_set_property;
}

static void
nautilus_icon_view_item_init (NautilusIconViewItem *self)
{
    NautilusIconView *self = NAUTILUS_ICON_VIEW (user_data);
    NautilusFile *file = NAUTILUS_FILE (item);
    NautilusIconViewPrivate *priv = nautilus_icon_view_get_instance_private (self);
    GtkFlowBoxChild *child;
    GtkBox *container;
    NautilusContainerMaxWidth *item_container;
    gint label_nat_size;
    gint icon_nat_size;
    GtkLabel *label;
    GtkWidget *icon;
    GtkStyleContext *style_context;

    container = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    self->item_container = nautilus_container_max_width_new ();

    icon = create_icon (self, file);
    gtk_box_pack_start (container, icon, FALSE, FALSE, 0);

    label = gtk_label_new (nautilus_file_get_display_name (file));
    gtk_label_set_ellipsize (label, PANGO_ELLIPSIZE_END);
    gtk_label_set_line_wrap (label, TRUE);
    gtk_label_set_line_wrap_mode (label, PANGO_WRAP_WORD_CHAR);
    gtk_label_set_lines (label, 4);
    gtk_label_set_justify (label, GTK_JUSTIFY_CENTER);
    gtk_widget_set_valign (GTK_WIDGET (label), GTK_ALIGN_START);
    gtk_box_pack_end (container, label, TRUE, TRUE, 0);

    style_context = gtk_widget_get_style_context (container);
    gtk_style_context_add_class (style_context, "icon-item-background");

    gtk_widget_show_all (container);
    gtk_widget_set_valign (container, GTK_ALIGN_START);
    gtk_widget_set_halign (container, GTK_ALIGN_CENTER);

    gtk_container_add (self->item_container, container);
    nautilus_container_max_width_set_max_width (NAUTILUS_CONTAINER_MAX_WIDTH (self->item_container),
                                                get_icon_size_for_zoom_level (priv->zoom_level));

    child = gtk_flow_box_child_new ();
    gtk_container_add (child, self->item_container);

    g_object_set_data (child, "file", file);
    g_object_set_data (child, "icon", icon);
    g_object_set_data (child, "label", label);

    gtk_widget_show_all (child);
}
