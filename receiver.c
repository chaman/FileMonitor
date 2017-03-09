#include <gio/gio.h>
#include <stdlib.h>


static void
on_signal_received(GDBusConnection     *connection,
                   const gchar         *sender_name,
                   const gchar         *object_path,
                   const gchar         *interface_name,
                   const gchar         *signal_name,
                   GVariant            *parameters,
                   gpointer            *user_data)
{
    gchar *msg;
    g_variant_get(parameters,"(s)",&msg);
    g_print("%s\n",msg);

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
    g_dbus_connection_signal_subscribe( connection,
                                        "org.filemon.Signal",
                                        "org.filemon.Type",
                                        "FileChange",
                                        "/org/filemon/Object",
                                        NULL,
                                        G_DBUS_SIGNAL_FLAGS_NONE,
                                        on_signal_received,
                                        NULL,
                                        NULL );
                                        

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

    guint owner_id;
    GMainLoop *loop;
    g_type_init ();
    
    
    owner_id = g_bus_own_name(G_BUS_TYPE_SESSION,
                              "org.filemon.Slot",
                              G_BUS_NAME_OWNER_FLAGS_REPLACE,
                              on_bus_acquired,
                              on_name_acquired,
                              on_name_lost,
                              NULL,
                              NULL);
    
                                       

    loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);
    g_bus_unown_name(owner_id);

    return 0;
}
