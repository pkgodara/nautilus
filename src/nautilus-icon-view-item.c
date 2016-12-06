#include "nautilus-icon-view-item.h"
#include "nautilus-container-max-width.h"
#include "nautilus-file.h"
#include "nautilus-thumbnails.h"

struct _NautilusIconViewItem
{
    GtkFlowBoxChild parent_instance;
    NautilusContainerMaxWidth *item_container;
    GtkBox *icon;
    guint icon_size;
    NautilusFile *file;
    GtkLabel *label;
};

G_DEFINE_TYPE (NautilusIconViewItem, nautilus_icon_view_item, GTK_TYPE_FLOW_BOX_CHILD)

enum
{
    PROP_0,
    PROP_FILE,
    PROP_ICON_SIZE,
    N_PROPS
};

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

    icon_pixbuf = nautilus_file_get_icon_pixbuf (self->file, self->icon_size,
                                                 TRUE, 1, flags);
    icon = gtk_image_new_from_pixbuf (icon_pixbuf);
    gtk_widget_set_hexpand (icon, TRUE);
    gtk_widget_set_vexpand (icon, TRUE);
    gtk_widget_set_valign (icon, GTK_ALIGN_CENTER);
    gtk_widget_set_halign (icon, GTK_ALIGN_CENTER);

    fixed_height_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_valign (fixed_height_box, GTK_ALIGN_CENTER);
    gtk_widget_set_halign (fixed_height_box, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request (fixed_height_box, self->icon_size, self->icon_size);

    if (nautilus_can_thumbnail (self->file) &&
        nautilus_file_should_show_thumbnail (self->file))
    {
        style_context = gtk_widget_get_style_context (fixed_height_box);
        gtk_style_context_add_class (style_context, "icon-background");
    }

    gtk_box_pack_start (fixed_height_box, icon, FALSE, FALSE, 0);

    gtk_widget_show_all (fixed_height_box);

    return fixed_height_box;
}

static void
update_icon (NautilusIconViewItem *self)
{
    GtkWidget *box;

    nautilus_container_max_width_set_max_width (NAUTILUS_CONTAINER_MAX_WIDTH (self->item_container),
                                                self->icon_size);
    box = gtk_bin_get_child (GTK_BIN (self->item_container));
    if (self->icon)
    {
        gtk_container_remove (GTK_CONTAINER (box), self->icon);
    }
    self->icon = create_icon (self);
    gtk_box_pack_start (box, self->icon, FALSE, FALSE, 0);
}

static void
constructed (NautilusIconViewItem *self)
{
    GtkBox *container;
    NautilusContainerMaxWidth *item_container;
    GtkLabel *label;
    GtkWidget *icon;
    GtkStyleContext *style_context;

    container = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    self->item_container = nautilus_container_max_width_new ();

    g_print ("file %s\n", nautilus_file_get_uri (self->file));
    self->icon = create_icon (self);
    gtk_box_pack_start (container, self->icon, FALSE, FALSE, 0);

    label = gtk_label_new (nautilus_file_get_display_name (self->file));
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
                                                self->icon_size);

    gtk_container_add (GTK_CONTAINER (self), self->item_container);
    gtk_widget_show_all (self);
}

static void
nautilus_icon_view_item_init (NautilusIconViewItem *self)
{
}


static void
nautilus_icon_view_item_class_init (NautilusIconViewItemClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = nautilus_icon_view_item_finalize;
    object_class->get_property = nautilus_icon_view_item_get_property;
    object_class->set_property = nautilus_icon_view_item_set_property;
    object_class->constructed = constructed;

    g_object_class_install_property (object_class,
                                     PROP_ICON_SIZE,
                                     g_param_spec_int ("icon-size",
                                                       "Icon size",
                                                       "The size in pixels of the icon",
                                                       NAUTILUS_CANVAS_ICON_SIZE_SMALL,
                                                       NAUTILUS_CANVAS_ICON_SIZE_LARGER,
                                                       NAUTILUS_CANVAS_ICON_SIZE_LARGE,
                                                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
    g_object_class_install_property (object_class,
                                     PROP_FILE,
                                     g_param_spec_object ("file",
                                                          "File",
                                                          "The file the icon item represents",
                                                          NAUTILUS_TYPE_FILE,
                                                          G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
}

NautilusIconViewItem *
nautilus_icon_view_item_new (NautilusFile *file,
                             guint         icon_size)
{
    return g_object_new (NAUTILUS_TYPE_ICON_VIEW_ITEM,
                         "file", file,
                         "icon-size", icon_size,
                         NULL);
}

void
nautilus_icon_view_item_set_icon_size (NautilusIconViewItem *self,
                                       guint                 icon_size)
{
  g_return_if_fail (NAUTILUS_IS_ICON_VIEW_ITEM (self));

  self->icon_size = icon_size;

  if (self->icon)
  {
    update_icon (self);
  }
}

NautilusFile *
nautilus_icon_view_item_get_file (NautilusIconViewItem *self)
{
    g_return_if_fail (NAUTILUS_IS_ICON_VIEW_ITEM (self));

    return self->file;
}

void
nautilus_icon_view_item_set_file (NautilusIconViewItem *self,
                                  NautilusFile         *file)
{
  g_return_if_fail (NAUTILUS_IS_ICON_VIEW_ITEM (self));
  g_print ("file HERE!!!!!");
  g_clear_object (&self->file);
  self->file = g_object_ref (file);

  if (self->icon)
  {
    update_icon (self);
  }

  if (self->label)
  {
      gtk_label_set_text (self->label,
                          nautilus_file_get_display_name (file));
  }
}
