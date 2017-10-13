#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>

typedef struct {
    guint16 port;
    const char *name;
    GSocketSourceFunc handler;
} ConnectionDescription;

/* Port descriptions are added only once on program startup
 * and will never be modified or removed */
static GArray *connection_descriptions;


static gboolean on_socket_event(GSocket *socket, GIOCondition condition,
    gpointer user_data)
{
    ConnectionDescription *cd = user_data;

    if (condition & (G_IO_ERR | G_IO_HUP | G_IO_NVAL)) {
        return G_SOURCE_REMOVE;
    }

    char str[100] = {};
    gssize size = g_socket_receive(socket, str, sizeof(str), NULL, NULL);
    if (size <= 0) {
        g_print("# %s connection closed\n", cd->name);
        return G_SOURCE_REMOVE;
    }
    g_print("%s", str);

    return G_SOURCE_CONTINUE;
}

static gboolean accept_connection(GSocketService *service,
    GSocketConnection *connection, GObject *source_object, gpointer user_data)
{
    GSocket *socket = g_socket_connection_get_socket(connection);

    /* Now search for connection description for specific socket */
    GSocketAddress *socket_address = g_socket_get_local_address(socket, NULL);
    guint16 port = g_inet_socket_address_get_port(G_INET_SOCKET_ADDRESS(socket_address));

    ConnectionDescription *cd = NULL;
    for (unsigned int i = 0; i < connection_descriptions->len; ++i) {
        ConnectionDescription *cd_tmp;
        cd_tmp = &g_array_index(connection_descriptions, ConnectionDescription, i);

        if (cd_tmp->port == port) {
            cd = cd_tmp;
            break;
        }
    }

    /* cd will never be NULL just because accept_connection invoked only
     * on ports that previously added with add_listening_port() */

    g_print("# %s incoming connection\n", cd->name);

    GSource *source = g_socket_create_source(socket, G_IO_IN, NULL);

    g_source_set_callback(source, (GSourceFunc)cd->handler, cd, NULL);
    g_source_attach(source, g_main_context_default());
    g_source_unref(source);

    return TRUE;
}

static gboolean add_listening_port(GSocketService *service, guint16 port,
    const char *name, GSocketSourceFunc handler, GError **error)
{
    if (connection_descriptions == NULL) {
        connection_descriptions = g_array_new(FALSE, FALSE, sizeof(ConnectionDescription));
    }

    g_array_append_val(connection_descriptions, ((ConnectionDescription){
        .port = port,
        .name = name,
        .handler = handler,
    }));

    return g_socket_listener_add_inet_port(G_SOCKET_LISTENER(service),
        port, NULL, error);
}

int main(void)
{
    GError *error = NULL;
    GSocketService *service = g_socket_service_new();

    if (!add_listening_port(service, 3701, "Service 1", on_socket_event, &error)) {
        g_print("%s\n", error->message);
        return 1;
    }

    if (!add_listening_port(service, 3702, "Service 2", on_socket_event, &error)) {
        g_print("%s\n", error->message);
        return 1;
    }

    /* Start listen for incoming connections */
    g_signal_connect(service, "incoming", G_CALLBACK(accept_connection), NULL);

    g_print("# Waiting for connection...\n");

    g_main_loop_run(g_main_loop_new(NULL, FALSE));

    return 0;
}
