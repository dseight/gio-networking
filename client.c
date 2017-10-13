#include <glib.h>
#include <gio/gio.h>

static gboolean on_socket_event(GSocket *socket, GIOCondition condition,
    gpointer user_data)
{
    if (condition & (G_IO_ERR | G_IO_HUP | G_IO_NVAL)) {
        return G_SOURCE_REMOVE;
    }

    if (condition & G_IO_OUT) {
        char str[] = "Hello!\n";
        gssize size = g_socket_send(socket, str, sizeof(str), NULL, NULL);

        if (size < 0) {
            g_print("# connection closed\n");
            return G_SOURCE_REMOVE;
        }
    }

    return G_SOURCE_CONTINUE;
}

/* "name" is unused and added for symmetry with add_listening_port() */
static gboolean add_host_connection(GSocketClient *client, const char *host_and_port,
    const char *name, GSocketSourceFunc handler, GError **error)
{
    GSocketConnection *connection = g_socket_client_connect_to_host(client,
        host_and_port, 0, NULL, error);

    if (connection == NULL) {
        return FALSE;
    }

    GSocket *socket = g_socket_connection_get_socket(connection);
    GSource *source = g_socket_create_source(socket, G_IO_IN | G_IO_OUT, NULL);

    g_source_set_callback(source, (GSourceFunc)handler, NULL, NULL);
    g_source_attach(source, g_main_context_default());
    g_source_unref(source);

    return TRUE;
}

int main(void)
{
    GError *error = NULL;
    GSocketClient *client = g_socket_client_new();

    if (!add_host_connection(client, "127.0.0.1:3701", "Client 1",
            on_socket_event, &error)) {
        g_print("%s\n", error->message);
        return 1;
    }
    if (!add_host_connection(client, "127.0.0.1:3702", "Client 2",
            on_socket_event, &error)) {
        g_print("%s\n", error->message);
        return 1;
    }

    g_print("# Clients started...\n");

    g_main_loop_run(g_main_loop_new(NULL, FALSE));

    return 0;
}
