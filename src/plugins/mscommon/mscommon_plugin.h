/*
 * Copyright (C) 2003 by Dave J. Andruczyk <djandruczyk at yahoo dot com>
 *
 * Linux Megasquirt tuning software
 * 
 * 
 * This software comes under the GPL (GNU Public License)
 * You may freely copy,distribute, etc. this as long as all the source code
 * is made available for FREE.
 * 
 * No warranty is made or implied. You use this program at your own risk.
 */

#ifndef __MSCOMMON_PLUGIN_H__
#define __MSCOMMON_PLUGIN_H__

#include <gtk/gtk.h>
#include <defines.h>
#include <configfile.h>
#include <enums.h>
#include <threads.h>

/* Prototypes */
void plugin_init(gconstpointer *);
void plugin_shutdown(void);
void register_common_enums(void);
/* Prototypes */

/* Function Pointers */
void (*error_msg_f)(const gchar *);
gboolean (*get_symbol_f)(const gchar *,void **);
void (*cleanup_f)(void *);
void (*io_cmd_f)(const gchar *,void *);
void (*dbg_func_f)(gint,gchar *);
GList *(*get_list_f)(gchar *);
OutputData *(*initialize_outputdata_f)(void);
void (*set_title_f)(const gchar *);
void (*set_widget_sensitive_f)(gpointer, gpointer);
void (*thread_update_logbar_f)(const gchar *, const gchar *, gchar *, gboolean, gboolean);
void (*update_logbar_f)(const gchar *, const gchar *, gchar *, gboolean, gboolean, gboolean);
void (*process_rt_vars_f)(void * );
void (*thread_update_widget_f)(const gchar *, WidgetType, gchar *);
gboolean (*queue_function_f)(const gchar * );
gboolean (*lookup_precision_f)(const gchar *, gint *);
gboolean (*lookup_current_value_f)(const gchar *, gfloat *);
gboolean (*lookup_current_value_f)(const gchar *, gfloat *);
gfloat (*direct_lookup_data_f)(gchar *, gint );
gint (*direct_reverse_lookup_f)(gchar *, gint );
gint (*direct_reverse_lookup_f)(gchar *, gint );
gint (*reverse_lookup_f)(gconstpointer *, gint );
gfloat (*lookup_data_f)(gconstpointer *, gint );
gint (*translate_string_f)(const gchar *);
GtkWidget *(*lookup_widget_f)(const gchar *);
void (*set_group_color_f)(GuiColor, const gchar * );
gint (*get_multiplier_f)(DataSize);
guint (*get_bitshift_f)(guint);
GtkWidget *(*spin_button_handler_f)(GtkWidget *, gpointer);
gboolean (*set_file_api_f)(ConfigFile *, gint, gint );
gboolean (*get_file_api_f)(ConfigFile *, gint *, gint * );
void (*stop_tickler_f)(gint);
void (*start_tickler_f)(gint);
gchar **(*parse_keys_f)(const gchar *, gint *, const gchar * );
gint (*get_multiplier_f)(DataSize );
glong (*get_extreme_from_size_f)(DataSize, Extreme);
gfloat (*convert_after_upload_f)(GtkWidget *);
gint (*convert_before_download_f)(GtkWidget *, gfloat);
GdkColor (*get_colors_from_hue_f)(gfloat, gfloat, gfloat);
void (*mem_alloc_f)(void);
void *(*eval_create_f)(char *);
void (*eval_destroy_f)( void *);
double (*eval_x_f)(void *, double);
void (*thread_widget_set_sensitive_f)(const gchar *, gboolean);
void (*get_table_f)(gpointer, gpointer, gpointer);
void (*free_multi_source_f)(gpointer);
void (*flush_serial_f)(gint, gint);
void (*_set_sized_data_f)(guint8 *, gint, DataSize, gint, gboolean);
gint (*_get_sized_data_f)(guint8 *, gint, DataSize, gboolean);
gboolean (*std_entry_handler_f)(GtkWidget *, gpointer);
gboolean (*entry_changed_handler_f)(GtkWidget *, gpointer);
gboolean (*search_model_f)(GtkTreeModel *, GtkWidget *, GtkTreeIter *);
void (*set_reqfuel_color_f)(GuiColor, gint);
void (*alter_widget_state_f)(gpointer, gpointer);
void (*bind_to_lists_f)(GtkWidget *, gchar * );
void (*warn_user_f)(const gchar *);
gboolean (*key_event_f)(GtkWidget *, GdkEventKey *, gpointer );
gboolean (*focus_out_handler_f)(GtkWidget *, GdkEventFocus *, gpointer );
guint32 (*create_value_change_watch_f)(const gchar *, gboolean, const gchar *, gpointer);
void (*remove_from_lists_f)(gchar *, gpointer);
void (*remove_watch_f)(guint32);
void (*thread_refresh_widget_range_f)(gint, gint, gint);
void (*update_ve3d_if_necessary_f)(gint, gint);
void (*thread_refresh_widget_f)(GtkWidget *);
char *(*regex_wrapper_f)(char *, char *, int *);
GtkWidget *(*mtx_gauge_face_new_wrapper_f)(void);
void (*mtx_gauge_face_import_xml_wrapper_f)(GtkWidget *, gchar *);
void (*mtx_gauge_face_set_value_wrapper_f)(GtkWidget *, gfloat);
/* Function Pointers */

#endif