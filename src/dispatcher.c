/*
 * Copyright (C) 2003 by Dave J. Andruczyk <djandruczyk at yahoo dot com>
 *
 * Linux Megasquirt tuning software
 * 
 * 
 * This software comes under the GPL (GNU Public License)
 * You may freely copy,distribute etc. this as long as the source code
 * is made available for FREE.
 * 
 * No warranty is made or implied. You use this program at your own risk.
 */

#include <comms.h>
#include <config.h>
#include <conversions.h>
#include <dataio.h>
#include <datalogging_gui.h>
#include <defines.h>
#include <debugging.h>
#include <dispatcher.h>
#include <enums.h>
#include <gui_handlers.h>
#include <interrogate.h>
#include <logviewer_gui.h>
#include <notifications.h>
#include <post_process.h>
#include <runtime_sliders.h>
#include <runtime_gui.h>
#include <rtv_map_loader.h>
#include <serialio.h>
#include <stdlib.h>
#include <string.h>
#include <structures.h>
#include <tabloader.h>
#include <threads.h>
#include <unistd.h>


gboolean thread_protect = TRUE;
extern GAsyncQueue *dispatch_queue;
extern gboolean connected;			/* valid connection with MS */
extern gboolean offline;			/* Offline mode */
extern gboolean interrogated;			/* valid connection with MS */


/*!
 \brief dispatcher() is a GTK+ timeout that runs 30 tiems per second checking
 for message on the dispatch queue which handles gui operations after a thread
 function runs, This will attempt to handle multiple messages at a time if the
 queue has multiple message queued up.
 \param data (gpointer) unused
 \returns TRUE 
 */
gboolean dispatcher(gpointer data)
{
	extern struct Firmware_Details * firmware;
	gint len=0;
	gint i=0;
	gint val=-1;
	gint count = 0;
	GtkWidget *widget = NULL;
	struct Io_Message *message = NULL;
	struct Text_Message *t_message = NULL;
	struct Widget_Update *w_update = NULL;
	extern gint temp_units;
	extern gboolean paused_handlers;
	extern gint mem_view_style[];
	extern GHashTable *dynamic_widgets;

	if (!dispatch_queue) /*queue not built yet... */
		return TRUE;
	/* Endless Loop, wiat for message, processs and repeat... */
trypop:
	message = g_async_queue_try_pop(dispatch_queue);
	if (!message)
	{
		//printf("no messages waiting, returning\n");
		return TRUE;
	}

	/* NOTE if !connected we ABORT All dispatchers as they all
	 * depend on a "connected" status.
	 */
	if ((!connected) && (!offline)) // Raise error window.... 
	{
		//printf("NOT connected, not offline\n");
		no_ms_connection();
	}

	if (message->funcs != NULL)
	{
		len = message->funcs->len;
		for (i=0;i<len;i++)
		{
			thread_protect = TRUE;
			val = g_array_index(message->funcs,UpdateFunction, i);

			switch ((UpdateFunction)val)
			{
				case UPD_LOGBAR:
					t_message = (struct Text_Message *)message->payload;
					update_logbar(t_message->view_name,t_message->tagname,t_message->msg,t_message->count,t_message->clear);
					dealloc_textmessage(t_message);
					message->payload = NULL;
					break;
				case UPD_WIDGET:
					widget = NULL;
					w_update = (struct Widget_Update *)message->payload;
					if (NULL == (widget = g_hash_table_lookup(dynamic_widgets,w_update->widget_name)))
						break;
					switch (w_update->type)
					{
						case MTX_ENTRY:
							gtk_entry_set_text(GTK_ENTRY(widget),w_update->msg);
							break;
						case MTX_LABEL:
							gtk_label_set_text(GTK_LABEL(widget),w_update->msg);
							break;
//						case MTX_SPINBUTTON:
//							gtk_label_set_text(GTK_LABEL(widget),w_update->msg);
//							break;
					}
					dealloc_w_update(w_update);
					message->payload = NULL;
					break;
				case UPD_POPULATE_DLOGGER:
					if (connected)
						populate_dlog_choices();
					break;
				case UPD_LOAD_RT_SLIDERS:
					if (connected)
					{
						load_sliders();
						reset_temps(GINT_TO_POINTER(temp_units));
					}
					break;
				case UPD_LOAD_REALTIME_MAP:
					if (connected)
						load_realtime_map();
					break;
				case UPD_LOAD_GUI_TABS:
					if (connected)
					{
						load_gui_tabs();
						reset_temps(GINT_TO_POINTER(temp_units));
					}
					break;
				case UPD_READ_VE_CONST:
					if (connected)
						io_cmd(IO_READ_VE_CONST,NULL);
					break;
				case UPD_REENABLE_INTERROGATE_BUTTON:
					gtk_widget_set_sensitive(GTK_WIDGET(g_hash_table_lookup(dynamic_widgets, "interrogate_button")),TRUE);
					break;
				case UPD_REALTIME:
					if (connected)
						update_runtime_vars();
					break;
				case UPD_VE_CONST:
					paused_handlers = TRUE;
					if (connected)
						update_ve_const();
					paused_handlers = FALSE;
					break;
				case UPD_STORE_RED:
					set_group_color(RED,"burners");
					break;
				case UPD_STORE_BLACK:
					set_group_color(BLACK,"burners");
					for (i=0;i<firmware->total_pages;i++)
						set_reqfuel_color(BLACK,i);
					break;
				case UPD_LOGVIEWER:
					if (connected)
						update_logview_traces(FALSE);
					break;
				case UPD_RAW_MEMORY:
					if (connected)
						update_raw_memory_view(mem_view_style[message->offset],message->offset);
					break;
				case UPD_DATALOGGER:
					if (connected)
						run_datalog();
					break;
				case UPD_COMMS_STATUS:
					update_comms_status();
					break;
				case UPD_WRITE_STATUS:
					update_write_status();
					break;

			}
			thread_protect = FALSE;

			gdk_threads_enter();
			while (gtk_events_pending())
				gtk_main_iteration();
			gdk_threads_leave();
		}
	}
	dealloc_message(message);
	//printf ("deallocation of dispatch message complete\n");
	count++;
	/* try to handle up to 4 messages at a time.  If this is 
	 * set too high, we can cause the timeout to hog the gui if it's
	 * too low, things can fall behind. (GL redraw ;( )
	 * */
	if(count < 3)
	{
		//printf("trying to handle another message\n");
		goto trypop;
	}
	//printf("returning\n");
	return TRUE;
}


/*!
 \brief dealloc_textmessage() deallocates the structure used to pass a text
 message from the thread to here..
 \param message (struct Text_Message *) pointer to message data
 */
void dealloc_textmessage(struct Text_Message * message)
{
        if (message->view_name)
                g_free(message->view_name);
        if (message->tagname)
                g_free(message->tagname);
        if (message->msg)
                g_free(message->msg);
	g_free(message);
	message = NULL;

}


/*!
 \brief dealloc_message() deallocates the structure used to pass an I/O
 message from a thread to here..
 \param message (struct Io_Message *) pointer to message data
 */
void dealloc_message(struct Io_Message * message)
{
        if (message->out_str)
                g_free(message->out_str);
        if (message->funcs)
                g_array_free(message->funcs,TRUE);
        if (message->payload)
                g_free(message->payload);
        g_free(message);

}


/*!
 \brief dealloc_w_update() deallocates the structure used to pass an I/O
 widget update message from a thread to here..
 \param w_update (struct Widget_Update *) pointer to message data
 */
void dealloc_w_update(struct Widget_Update * w_update)
{
        if (w_update->widget_name)
                g_free(w_update->widget_name);
        if (w_update->msg)
                g_free(w_update->msg);
        g_free(w_update);
	w_update = NULL;

}

