#include "nautilus-icon-view-item.h"

struct _NautilusIconViewItem
{
    GtkFlowBoxChild parent_instance;
    GtkWidget *item_container;
    guint icon_size;
};

G_DEFINE_TYPE (NautilusIconViewItem, nautilus_icon_view_item, GTK_TYPE_FLOW_BOX_CHILD)

enum
{
    PROP_0,
    PROP_FILE,
    PROP_ICON_SIZE,
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
        case PROP_FILE:
        {
            g_value_set_object (value, self->file);
        }
        break;

        case PROP_ICON_SIZE:
        {
            g_value_set_int (value, self->icon_size);
        }
        break;

        default:
        {
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        }
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
        case PROP_FILE:
        {
            nautilus_icon_view_item_set_file (self, g_value_get_object (value));
        }
        break;

        case PROP_ICON_SIZE:
        {
            nautilus_icon_view_item_set_icon_size (self, g_value_get_int (value));
        }
        break;

        default:
        {
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        }
    }
}

static void
nautilus_icon_view_item_class_init (NautilusIconViewItemClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = nautilus_icon_view_item_finalize;
    object_class->get_property = nautilus_icon_view_item_get_property;
    object_class->set_property = nautilus_icon_view_item_set_property;

    g_object_class_install_property (object_class,
                                     PROP_ICON_SIZE,
                                     g_param_spec_int ("icon-size",
                                                       "Icon size",
                                                       "The size in pixels of the icon",
                                                       NAUTILUS_CANVAS_ICON_SIZE_SMALL,
                                                       NAUTILUS_CANVAS_ICON_SIZE_LARGER,
                                                       NAUTILUS_CANVAS_ICON_SIZE_LARGE,
                                                       G_PARAM_READWRITE));
    g_object_class_install_property (object_class,
                                     PROP_FILE,
                                     g_param_spec_object ("icon-size",
                                                          "Icon size",
                                                          "The size in pixels of the icon",
                                                          NAUTILUS_TYPE_FILE,
                                                          G_PARAM_READWRITE));
}

static GtkWidget *
create_icon (NautilusIconViewItem *self)
{
    NautilusFileIconFlags flags;
    g_autoptr (GdkPixbuf) icon_pixbuf;
    GtkImage *icon;
    GtkWidget *fixed_height_box;
    GtkStyleContext *style_context;

    flags = NAUTILUS_FILE_ICON_FLAGS_USE_THUMBNAILS |
            NAUTILUS_FILE_ICON_FLAGS_FORCE_THUMBNAIL_SIZE |
            NAUTILUS_FILE_ICON_FLAGS_USE_EMBLEMS |
            NAUTILUS_FILE_ICON_FLAGS_USE_ONE_EMBLEM;

    icon_pixbuf = nautilus_file_get_icon_pixbuf (self->file, priv->icon_size,
                                                 TRUE, 1, flags);
    icon = gtk_image_new_from_pixbuf (icon_pixbuf);
    gtk_widget_set_hexpand (icon, TRUE);
    gtk_widget_set_vexpand (icon, TRUE);
    gtk_widget_set_valign (icon, GTK_ALIGN_CENTER);
    gtk_widget_set_halign (icon, GTK_ALIGN_CENTER);

    fixed_height_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_valign (fixed_height_box, GTK_ALIGN_CENTER);
    gtk_widget_set_halign (fixed_height_box, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request (fixed_height_box, get_icon_size_for_zoom_level (priv->zoom_level),
                                 get_icon_size_for_zoom_level (priv->zoom_level));


    style_context = gtk_widget_get_style_context (fixed_height_box);
    gtk_style_context_add_class (style_contet, "icon-background");

    gtk_box_pack_start (fixed_height_box, icon, FALSE, FALSE, 0);

    gtk_widget_show_all (fixed_height_box);

    return fixed_height_box;
}

static void
update_icon (NautilusIconViewItem *self)
{
    GtkWidget *new_icon;
    GtkWidget *old_icon;
    GtkWidget *box;
    GtkWidget *label;
    NautilusFile *file;
    gint label_nat_size;
    gint icon_nat_size;

    file = g_object_get_data (flow_box_item, "file");
    old_icon = g_object_get_data (flow_box_item, "icon");
    label = g_object_get_data (flow_box_item, "label");

    nautilus_container_max_width_set_max_width (NAUTILUS_CONTAINER_MAX_WIDTH (self->item_container),
                                                get_icon_size_for_zoom_level (priv->zoom_level));
    box = gtk_bin_get_child (GTK_BIN (self->item_container));
    gtk_container_remove (GTK_CONTAINER (box), old_icon);
    new_icon = create_icon (self);
    gtk_box_pack_start (box, new_icon, FALSE, FALSE, 0);
    g_object_set_data (flow_box_item, "icon", new_icon);
}

static void
nautilus_icon_view_item_init (NautilusIconViewItem *self)
{
    NautilusFile *file = NAUTILUS_FILE (item);
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

    icon = create_icon (self);
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

void
nautilus_icon_view_item_set_icon_size (NautilusIconViewItem *self,
                                       guint                 icon_size)
{
  g_return_if_fail (NAUTILUS_IS_ICON_VIEW_ITEM (self));

  priv->icon_size = icon_size;
  update_icon (self);
}

void
nautilus_icon_view_item_set_file (NautilusIconViewItem *self,
                                  NautilusFile         *file)
{
  g_return_if_fail (NAUTILUS_IS_ICON_VIEW_ITEM (self));

  g_clear_object (self->file);
  self->file = g_object_ref (file);
}
