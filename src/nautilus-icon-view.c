/* nautilus-icon-view.c
 *
 * Copyright (C) 2016 Carlos Soriano <csoriano@gnome.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>
#include <glib.h>

#include "nautilus-icon-view.h"
#include "nautilus-icon-view-item.h"
#include "nautilus-files-view.h"
#include "nautilus-file.h"
#include "nautilus-directory.h"
#include "nautilus-global-preferences.h"

typedef struct
{
    GtkWidget *flow_box;
    GtkWidget *view_icon;
    GListModel *model;
    GActionGroup *view_action_group;
    gint zoom_level;
} NautilusIconViewPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (NautilusIconView, nautilus_icon_view, NAUTILUS_TYPE_FILES_VIEW)

static gint
get_default_zoom_level ()
{
    NautilusCanvasZoomLevel default_zoom_level;

    default_zoom_level = g_settings_get_enum (nautilus_icon_view_preferences,
                                              NAUTILUS_PREFERENCES_ICON_VIEW_DEFAULT_ZOOM_LEVEL);

    return NAUTILUS_CANVAS_ZOOM_LEVEL_LARGE;
}

static void
real_begin_loading (NautilusFilesView *self)
{
}

static void
real_clear (NautilusFilesView *self)
{
    NautilusIconViewPrivate *priv = nautilus_icon_view_get_instance_private (self);

    g_list_store_remove_all (G_LIST_STORE (priv->model));
}


static void
real_file_changed (NautilusFilesView *self,
                   NautilusFile      *file,
                   NautilusDirectory *directory)
{
}

static GList *
real_get_selection (NautilusFilesView *self)
{
    return NULL;
}


static GList *
real_get_selection_for_file_transfer (NautilusFilesView *self)
{
    return NULL;
}

static gboolean
real_is_empty (NautilusFilesView *self)
{
    NautilusIconViewPrivate *priv = nautilus_icon_view_get_instance_private (self);

    return g_list_model_get_n_items (priv->model) == 0;
}

static void
real_end_file_changes (NautilusFilesView *self)
{
}

static void
real_remove_file (NautilusFilesView *self,
                  NautilusFile      *file,
                  NautilusDirectory *directory)
{
    NautilusIconViewPrivate *priv;
    NautilusFile *current_file;
    guint i = 0;

    priv = nautilus_icon_view_get_instance_private (self);

    while (current_file = g_list_model_get_item (priv->model, i))
    {
        if (current_file == file)
        {
            g_list_store_remove (G_LIST_STORE (priv->model), i);
            break;
        }
        i++;
    }
}

static void
real_set_selection (NautilusFilesView *self,
                    GList             *selection)
{
    nautilus_files_view_notify_selection_changed (self);
}

static void
real_select_all (NautilusFilesView *self)
{
}

static void
real_reveal_selection (NautilusFilesView *self)
{
}

static void
real_update_actions_state (NautilusFilesView *self)
{
    NAUTILUS_FILES_VIEW_CLASS (nautilus_icon_view_parent_class)->update_actions_state (self);
}

static void
real_bump_zoom_level (NautilusFilesView *self,
                      int                zoom_increment)
{
    NautilusIconViewPrivate *priv = nautilus_icon_view_get_instance_private (self);
    NautilusCanvasZoomLevel new_level;

    new_level = priv->zoom_level + zoom_increment;

    if (new_level >= NAUTILUS_CANVAS_ZOOM_LEVEL_SMALL &&
        new_level <= NAUTILUS_CANVAS_ZOOM_LEVEL_LARGER)
    {
        g_action_group_change_action_state (priv->view_action_group,
                                            "zoom-to-level",
                                            g_variant_new_int32 (new_level));
    }
}

static guint
real_get_zoom_level (NautilusFilesView *self)
{
    NautilusIconViewPrivate *priv = nautilus_icon_view_get_instance_private (self);

    return priv->zoom_level;
}

static guint
get_icon_size_for_zoom_level (NautilusCanvasZoomLevel zoom_level)
{
    switch (zoom_level)
    {
        case NAUTILUS_CANVAS_ZOOM_LEVEL_SMALL:
        {
            return NAUTILUS_CANVAS_ICON_SIZE_SMALL;
        }
        break;

        case NAUTILUS_CANVAS_ZOOM_LEVEL_STANDARD:
        {
            return NAUTILUS_CANVAS_ICON_SIZE_STANDARD;
        }
        break;

        case NAUTILUS_CANVAS_ZOOM_LEVEL_LARGE:
        {
            return NAUTILUS_CANVAS_ICON_SIZE_LARGE;
        }
        break;

        case NAUTILUS_CANVAS_ZOOM_LEVEL_LARGER:
        {
            return NAUTILUS_CANVAS_ICON_SIZE_LARGER;
        }
        break;
    }
    g_return_val_if_reached (NAUTILUS_CANVAS_ICON_SIZE_STANDARD);
}

static void
set_icon_size (NautilusIconView *self,
               gint              icon_size)
{
    NautilusIconViewPrivate *priv = nautilus_icon_view_get_instance_private (self);
    g_autoptr (GList) items;
    GList *l;
    g_autoptr (GList) box_children;
    NautilusIconViewItem *item;

    items = gtk_container_get_children (priv->flow_box);

    for (l = items; l; l = l->next)
    {
        item = NAUTILUS_ICON_VIEW_ITEM (l->data);
        nautilus_icon_view_item_set_icon_size (item, get_icon_size_for_zoom_level (priv->zoom_level));
    }
}

static void
set_zoom_level (NautilusIconView *self,
                guint             new_level)
{
    NautilusIconViewPrivate *priv;
    guint icon_size;

    priv = nautilus_icon_view_get_instance_private (self);

    priv->zoom_level = new_level;

    icon_size = get_icon_size_for_zoom_level (new_level);
    set_icon_size (self, icon_size);
}
static void
real_zoom_to_level (NautilusFilesView *files_view,
                    guint              new_level)
{
}

static void
real_restore_standard_zoom_level (NautilusFilesView *self)
{
}

static gfloat
real_get_zoom_level_percentage (NautilusFilesView *files_view)
{
    NautilusIconView *self;
    NautilusIconViewPrivate *priv;

    self = NAUTILUS_ICON_VIEW (files_view);
    priv = nautilus_icon_view_get_instance_private (self);

    return (gfloat) get_icon_size_for_zoom_level (priv->zoom_level) /
           NAUTILUS_CANVAS_ICON_SIZE_LARGE;
}

static gboolean
real_can_zoom_in (NautilusFilesView *self)
{
    return TRUE;
}

static gboolean
real_can_zoom_out (NautilusFilesView *self)
{
    return TRUE;
}

static void
real_click_policy_changed (NautilusFilesView *self)
{
}

static int
real_compare_files (NautilusFilesView *self,
                    NautilusFile      *file1,
                    NautilusFile      *file2)
{
    if (file1 < file2)
    {
        return -1;
    }

    if (file1 > file2)
    {
        return +1;
    }

    return 0;
}

static gboolean
real_using_manual_layout (NautilusFilesView *self)
{
    return FALSE;
}

static void
real_end_loading (NautilusFilesView *self,
                  gboolean           all_files_seen)
{
}

static char *
real_get_first_visible_file (NautilusFilesView *self)
{
    return NULL;
}

static void
real_scroll_to_file (NautilusFilesView *self,
                     const char        *uri)
{
}

static void
real_sort_directories_first_changed (NautilusFilesView *self)
{
}

static gpointer *
convert_glist_to_array (GList *list)
{
    gpointer *array;
    GList *l;
    int i = 0;

    g_return_val_if_fail (list != NULL, NULL);

    array = g_malloc (g_list_length (list) * sizeof (list->data));

    for (l = list; l != NULL; l = l->next, i++)
    {
        array[i] = l->data;
    }

    return array;
}

static void
real_add_files (NautilusFilesView *self,
                GList             *files,
                NautilusDirectory *directory)
{
    NautilusIconViewPrivate *priv;
    g_autofree gpointer *array = NULL;

    priv = nautilus_icon_view_get_instance_private (self);

    clock_t start = clock ();

    g_print ("add files %d\n", g_list_length (files));
    array = convert_glist_to_array (files);
    g_list_store_splice (G_LIST_STORE (priv->model),
                         g_list_model_get_n_items (priv->model),
                         0, array, g_list_length (files));
    clock_t end = clock ();
    double elapsed_time = (end - start) / (double) CLOCKS_PER_SEC;
    g_print ("add file finished %d %f\n", g_list_model_get_n_items (priv->model), elapsed_time);
}


static guint
real_get_view_id (NautilusFilesView *self)
{
    return NAUTILUS_VIEW_GRID_ID;
}

static GIcon *
real_get_icon (NautilusFilesView *self)
{
    NautilusIconViewPrivate *priv;

    priv = nautilus_icon_view_get_instance_private (self);

    return priv->view_icon;
}

static void
real_select_first (NautilusFilesView *self)
{
}

static void
action_zoom_to_level (GSimpleAction *action,
                      GVariant      *state,
                      gpointer       user_data)
{
    NautilusIconView *self = NAUTILUS_ICON_VIEW (user_data);

    set_zoom_level (self, g_variant_get_int32 (state));
    g_simple_action_set_state (G_SIMPLE_ACTION (action), state);
}

static GtkWidget *
create_widget_func (gpointer item,
                    gpointer user_data)
{
    NautilusIconView *self = NAUTILUS_ICON_VIEW (user_data);
    NautilusFile *file = NAUTILUS_FILE (item);
    NautilusIconViewPrivate *priv = nautilus_icon_view_get_instance_private (self);
    NautilusIconViewItem *child;

    g_print ("is it a file? %s\n", nautilus_file_get_uri (file));
    child = nautilus_icon_view_item_new (file,
                                         get_icon_size_for_zoom_level (priv->zoom_level));
    gtk_widget_show_all (child);

    return GTK_WIDGET (child);
}

static void
on_child_activated (GtkFlowBox      *flow_box,
                    GtkFlowBoxChild *child,
                    gpointer         user_data)
{
    NautilusIconView *self = NAUTILUS_ICON_VIEW (user_data);
    NautilusIconViewPrivate *priv = nautilus_icon_view_get_instance_private (self);
    NautilusFile *file;
    g_autoptr (GList) list = NULL;

    file = nautilus_icon_view_item_get_file (child);
    list = g_list_append (list, file);

    nautilus_files_view_activate_files (NAUTILUS_FILES_VIEW (self), list, 0, TRUE);
}

NautilusIconView *
nautilus_icon_view_new (NautilusWindowSlot *slot)
{
    return g_object_new (NAUTILUS_TYPE_ICON_VIEW,
                         "window-slot", slot,
                         NULL);
}

static void
nautilus_icon_view_finalize (GObject *object)
{
    NautilusIconView *self = (NautilusIconView *) object;
    NautilusIconViewPrivate *priv = nautilus_icon_view_get_instance_private (self);

    G_OBJECT_CLASS (nautilus_icon_view_parent_class)->finalize (object);
}

const GActionEntry icon_view_entries[] =
{
    { "zoom-to-level", NULL, NULL, "3", action_zoom_to_level }
};

static void
nautilus_icon_view_class_init (NautilusIconViewClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    NautilusFilesViewClass *files_view_class = NAUTILUS_FILES_VIEW_CLASS (klass);

    object_class->finalize = nautilus_icon_view_finalize;

    files_view_class->add_files = real_add_files;
    files_view_class->begin_loading = real_begin_loading;
    files_view_class->bump_zoom_level = real_bump_zoom_level;
    files_view_class->can_zoom_in = real_can_zoom_in;
    files_view_class->can_zoom_out = real_can_zoom_out;
    files_view_class->click_policy_changed = real_click_policy_changed;
    files_view_class->clear = real_clear;
    files_view_class->file_changed = real_file_changed;
    files_view_class->get_selection = real_get_selection;
    files_view_class->get_selection_for_file_transfer = real_get_selection_for_file_transfer;
    files_view_class->is_empty = real_is_empty;
    files_view_class->remove_file = real_remove_file;
    files_view_class->update_actions_state = real_update_actions_state;
    files_view_class->reveal_selection = real_reveal_selection;
    files_view_class->select_all = real_select_all;
    files_view_class->set_selection = real_set_selection;
    files_view_class->compare_files = real_compare_files;
    files_view_class->sort_directories_first_changed = real_sort_directories_first_changed;
    files_view_class->end_file_changes = real_end_file_changes;
    files_view_class->using_manual_layout = real_using_manual_layout;
    files_view_class->end_loading = real_end_loading;
    files_view_class->get_view_id = real_get_view_id;
    files_view_class->get_first_visible_file = real_get_first_visible_file;
    files_view_class->scroll_to_file = real_scroll_to_file;
    files_view_class->get_icon = real_get_icon;
    files_view_class->select_first = real_select_first;
    files_view_class->restore_standard_zoom_level = real_restore_standard_zoom_level;
    files_view_class->get_zoom_level_percentage = real_get_zoom_level_percentage;
}

static void
nautilus_icon_view_init (NautilusIconView *self)
{
    NautilusIconViewPrivate *priv;
    GtkWidget *content_widget;

    priv = nautilus_icon_view_get_instance_private (self);
    priv->view_icon = g_themed_icon_new ("view-grid-symbolic");
    priv->model = g_list_store_new (NAUTILUS_TYPE_FILE);
    priv->flow_box = gtk_flow_box_new ();
    gtk_flow_box_set_activate_on_single_click (priv->flow_box, FALSE);
    gtk_flow_box_set_max_children_per_line (priv->flow_box, 20);
    gtk_flow_box_set_selection_mode (priv->flow_box, GTK_SELECTION_MULTIPLE);
    gtk_flow_box_bind_model (GTK_FLOW_BOX (priv->flow_box), priv->model,
                             create_widget_func, self, NULL);
    gtk_flow_box_set_homogeneous (priv->flow_box, FALSE);
    gtk_flow_box_set_row_spacing (priv->flow_box, 4);
    gtk_flow_box_set_column_spacing (priv->flow_box, 8);
    gtk_widget_set_valign (priv->flow_box, GTK_ALIGN_START);
    gtk_widget_set_margin_top (priv->flow_box, 10);
    gtk_widget_set_margin_start (priv->flow_box, 10);
    gtk_widget_set_margin_bottom (priv->flow_box, 10);
    gtk_widget_set_margin_end (priv->flow_box, 10);

    g_signal_connect (priv->flow_box, "child-activated", (GCallback) on_child_activated, self);

    gtk_widget_show (priv->flow_box);

    content_widget = nautilus_files_view_get_content_widget (NAUTILUS_FILES_VIEW (self));

    gtk_container_add (GTK_CONTAINER (content_widget), priv->flow_box);

    priv->view_action_group = nautilus_files_view_get_action_group (NAUTILUS_FILES_VIEW (self));
    g_action_map_add_action_entries (G_ACTION_MAP (priv->view_action_group),
                                     icon_view_entries,
                                     G_N_ELEMENTS (icon_view_entries),
                                     self);
    priv->zoom_level = get_default_zoom_level ();
    g_action_group_change_action_state (priv->view_action_group,
                                        "zoom-to-level",
                                        g_variant_new_int32 (priv->zoom_level));
}
