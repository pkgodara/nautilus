#include "nautilus-icon-view-item.h"

struct _NautilusIconViewItem
{
    GtkBin parent_instance;
    guint max_width;
};

G_DEFINE_TYPE (NautilusIconViewItem, nautilus_icon_view_item, GTK_TYPE_BIN)

enum
{
    PROP_0,
    PROP_FILE,
    PROP_MAX_WIDTH,
    N_PROPS
};

static GParamSpec *properties [N_PROPS];

void
nautilus_icon_view_item_set_max_width (NautilusIconViewItem *self,
                                       guint                 max_width)
{
    self->max_width = max_width;
    gtk_widget_queue_allocate (GTK_WIDGET (self));
}

guint
nautilus_icon_view_item_get_max_width (NautilusIconViewItem *self)
{
    return self->max_width;
}

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
        case PROP_MAX_WIDTH:
        {
            g_value_set_int (value, self->max_width);
        }

        default:
        {
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        }
        break;
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
        case PROP_MAX_WIDTH:
        {
            nautilus_icon_view_item_set_max_width (self, g_value_get_int (value));
        }
        break;

        default:
        {
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        }
        break;
    }
}

static void
get_preferred_width (GtkWidget *widget,
                     gint      *minimum_size,
                     gint      *natural_size)
{
    GtkWidget *child;
    NautilusIconViewItem *self;
    GtkStyleContext *style_context;
    GtkBorder padding;

    self = NAUTILUS_ICON_VIEW_ITEM (widget);
    child = gtk_bin_get_child (GTK_BIN (self));

    *natural_size = 0;
    *minimum_size = 0;
    gtk_widget_get_preferred_width (child, minimum_size, natural_size);

    *minimum_size = self->max_width == -1 ? *minimum_size : 96;
    *natural_size = self->max_width == -1 ? *natural_size :
                    MAX (*minimum_size, MIN (self->max_width, *natural_size));

    style_context = gtk_widget_get_style_context (child);
    gtk_style_context_get_padding (style_context,
                                   gtk_widget_get_state_flags (child),
                                   &padding);
    *minimum_size += padding.left + padding.right;
    *natural_size += padding.left + padding.right;
}

static void
get_preferred_height (GtkWidget *widget,
                      gint      *minimum_size,
                      gint      *natural_size)
{
    GtkWidget *child;
    NautilusIconViewItem *self;
    gint minimum_width = 0;
    gint natural_width = 0;
    GtkStyleContext *style_context;
    GtkBorder padding;

    self = NAUTILUS_ICON_VIEW_ITEM (widget);
    child = gtk_bin_get_child (GTK_BIN (self));

    get_preferred_width (widget, &minimum_width, &natural_width);
    natural_width = self->max_width == -1 ? natural_width : MIN (self->max_width, natural_width);

    gtk_widget_get_preferred_height_for_width (child, natural_width, minimum_size, natural_size);

    style_context = gtk_widget_get_style_context (child);
    gtk_style_context_get_padding (style_context,
                                   gtk_widget_get_state_flags (child),
                                   &padding);
    *minimum_size += padding.top + padding.bottom;
    *natural_size += padding.top + padding.bottom;

    g_print ("preferred height %d %d\n", *minimum_size, *natural_size);
}

static void
get_preferred_height_for_width (GtkWidget *widget,
                                gint       width,
                                gint      *minimum_size,
                                gint      *natural_size)
{
    get_preferred_height (widget, minimum_size, natural_size);
}

static void
size_allocate (GtkWidget     *widget,
               GtkAllocation *allocation)
{
    GTK_WIDGET_CLASS (nautilus_icon_view_item_parent_class)->size_allocate (widget, allocation);
}

static void
get_preferred_width_for_height (GtkWidget *widget,
                                gint       height,
                                gint      *minimum_size,
                                gint      *natural_size)
{
    get_preferred_width (widget, minimum_size, natural_size);
    g_print ("get preffered width for height %d\n", *natural_size);
}

static void
constructed (GObject *obj)
{
    NautilusIconViewItem *self = NAUTILUS_ICON_VIEW_ITEM (obj);

    G_OBJECT_CLASS (nautilus_icon_view_item_parent_class)->constructed (obj);

    /* We want our parent to gives our preferred width */
    gtk_widget_set_halign (GTK_WIDGET (self), GTK_ALIGN_CENTER);
    self->max_width = -1;
}

static void
nautilus_icon_view_item_class_init (NautilusIconViewItemClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    object_class->finalize = nautilus_icon_view_item_finalize;
    object_class->get_property = nautilus_icon_view_item_get_property;
    object_class->set_property = nautilus_icon_view_item_set_property;
    object_class->constructed = constructed;

    widget_class->get_preferred_width = get_preferred_width;
    widget_class->get_preferred_width_for_height = get_preferred_width_for_height;
    widget_class->get_preferred_height = get_preferred_height;
    widget_class->get_preferred_height_for_width = get_preferred_height_for_width;
    widget_class->size_allocate = size_allocate;
}

static void
nautilus_icon_view_item_init (NautilusIconViewItem *self)
{
}
