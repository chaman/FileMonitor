#include <glib.h>
#include <gio/gio.h>
#include <stdlib.h>

GDBusConnection *conn;
GFile *file;
GFileMonitor *mon;


gboolean 
send_msg(gchar *m) 
{

    GDBusMessage *msg;
    
    msg = g_dbus_message_new_signal("/org/filemon/Object",
                                    "org.filemon.Type",
                                    "FileChange");
                        
    if(NULL == msg) {
        exit(1);
    }
    
    g_dbus_message_set_body(msg,
                            g_variant_new("(s)",m));

    return g_dbus_connection_send_message(conn,
                                          msg,
                                          G_DBUS_SEND_MESSAGE_FLAGS_NONE,
                                          NULL,
                                          NULL);
}



void
callback(GFileMonitor        *mon,
         GFile               *first,
         GFile               *second,
         GFileMonitorEvent    event,
         gpointer             udata)
{
    
    gchar *m;
    m = NULL;
    g_assert(first != NULL);
    gchar *filepath;
    filepath = g_file_get_path(first);
    switch(event) {
        case G_FILE_MONITOR_EVENT_CHANGED:
            m = g_strdup_printf("File Content Changed: %s ", filepath);
            break;
        case G_FILE_MONITOR_EVENT_DELETED:
            m = g_strdup_printf("File Deleted: %s ", filepath);
            break;
        case G_FILE_MONITOR_EVENT_CREATED:
            m = g_strdup_printf("File Created: %s ", filepath);
            break;
        case G_FILE_MONITOR_EVENT_ATTRIBUTE_CHANGED:
            m = g_strdup_printf("File Attrib. Changed: %s ", filepath);
            break;
        case G_FILE_MONITOR_EVENT_MOVED:
            m = g_strdup_printf("File Moved: src-%s -> dst-%s", filepath, second != NULL ? g_file_get_path(second):"NULL");
            break;
        case G_FILE_MONITOR_EVENT_CHANGES_DONE_HINT:
        case G_FILE_MONITOR_EVENT_PRE_UNMOUNT:
        case G_FILE_MONITOR_EVENT_UNMOUNTED:
            break;
    }
    
    if (NULL != m) {
        if(!send_msg(m)) {
             g_print ("Signal Emit Failed!!!!");
             exit(1);
        } 
        g_free (m);
    }
    g_free(filepath);
}


static void
on_bus_acquired(GDBusConnection *connection,
                const gchar     *name,
                gpointer         user_data)
{

}

static void
on_name_acquired(GDBusConnection *connection,
                 const gchar     *name,
                 gpointer         user_data)
{
    
    conn = connection;
    mon = g_file_monitor(file, G_FILE_MONITOR_SEND_MOVED, NULL, NULL);
    g_assert(mon != NULL);

    //"changed" signal, callback signature documentation below
    //https://developer.gnome.org/gio/stable/GFileMonitor.html#GFileMonitor-changed
    g_signal_connect(mon, "changed", G_CALLBACK(callback), NULL);

}

static void
on_name_lost(GDBusConnection *connection,
             const gchar     *name,
             gpointer         user_data)
{

}

int
main (int argc, char *argv[])
{

    if(argc < 2) {
        g_print ("Usage: emitter <directory>\n");
        return 1;
    }


    guint owner_id;
    GMainLoop *loop;
    
    g_type_init();
    
    file = g_file_new_for_path(argv[1]);

    if( !g_file_query_exists (file, NULL)) {
        g_print ("Invalid File Path!\nPlease provide a valid path.\n");
        exit(1);

    }
    
    owner_id = g_bus_own_name(G_BUS_TYPE_SESSION,
                              "org.filemon.Signal",
                              G_BUS_NAME_OWNER_FLAGS_REPLACE,
                              on_bus_acquired,
                              on_name_acquired,
                              on_name_lost,
                              NULL,
                              NULL);
    
                                       

    loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);
    g_bus_unown_name(owner_id);
    g_object_unref(mon);

    return 0;
}
