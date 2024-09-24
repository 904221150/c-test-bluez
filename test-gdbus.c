#include <stdio.h>

#include <pthread.h>
#include <glib.h>
#include <gio/gio.h>

#include "bluez-agent.h"
#include "agent-server.h"

// arm-openwrt-linux-gcc test-gdbus.c `pkg-config --cflags --libs gio-2.0 dbus-1 gobject-2.0 glib-2.0` -I /usr/include/glib-2.0 -I /usr/lib64/glib-2.0/include -o test-gdbus


GDBusConnection *conn;
GDBusProxy *objectManager;
GDBusProxy *adapter1;
const gchar *connected_path = "";
int flag_connect = -1;

static int checkConnected(GVariant *properties)
{
    const gchar *property_name;
    GVariant *property;
    GVariantIter j;
	g_variant_iter_init(&j, properties);
    while(g_variant_iter_next(&j, "{sv}", &property_name, &property)) {
        if(g_strstr_len(g_ascii_strdown(property_name, -1), -1, "connected")) {
            int connected;
            g_variant_get(property, "b", &connected);
            if(connected) {
                g_variant_unref(property);
                return 1;
            }
        }
    }
    g_variant_unref(property);
    return 0;
}

static int checkPaired(GVariant *properties)
{
    const gchar *property_name;
    GVariant *property;
    GVariantIter j;
	g_variant_iter_init(&j, properties);
    while(g_variant_iter_next(&j, "{sv}", &property_name, &property)) {
        if(g_strstr_len(g_ascii_strdown(property_name, -1), -1, "paired")) {
            int paired;
            g_variant_get(property, "b", &paired);
            if(paired) {
                g_variant_unref(property);
                return 1;
            }
        }
    }
    g_variant_unref(property);
    return 0;
}

static int pair_commond(const gchar *path, int paired)
{
    static int flag_test = 0;
    // if(flag_test == 0) {
        flag_test = 1;
        printf("test Pair\n");
        GError *error = NULL;
        GDBusProxy *device_test;
        GVariant *result;
        device_test = g_dbus_proxy_new_sync(conn,
	    			      G_DBUS_PROXY_FLAGS_NONE,
	    			      NULL,
	    			      "org.bluez",
	    			      path,
	    			      "org.bluez.Device1",
	    			      NULL,
	    			      &error);
	    if(error != NULL) {
            printf("error 1\n");
            flag_test = 0;
            return -1;
        }
	    
        if(paired == 0) {
            result = g_dbus_proxy_call_sync(device_test,
	        				"Pair",
	        				NULL,
	        				G_DBUS_CALL_FLAGS_NONE,
	        				-1,
	        				NULL,
	        				&error);
            if(error != NULL) {
                printf("error 2\n");
                flag_test = 0;
                return -1;
            }
            printf("test Pair success\n");
        }
        

        result = g_dbus_proxy_call_sync(device_test,
	    				"Connect",
	    				NULL,
	    				G_DBUS_CALL_FLAGS_NONE,
	    				-1,
	    				NULL,
	    				&error);
        if(error != NULL) {
            printf("error 3\n");
            flag_test = 0;
            return -1;
        }
        printf("test Connect success\n");
        return 0;
    // }
    // return 0;
}

static int remove_commond(const gchar *path)
{
    GError *error = NULL;
    GVariant *result;
    error = NULL;
	result = g_dbus_proxy_call_sync(adapter1,
					"RemoveDevice",
					g_variant_new("(o)", path),
					G_DBUS_CALL_FLAGS_NONE,
					-1,
					NULL,
					&error);
	if(error != NULL) {
        g_error("Failed to call RemoveDevice: %s", error->message); 
        g_error_free(error); 
        return -1;
    }
		
    printf("remove_commond success\n");
    return 0;
}

static int remove_all_devices() {
    GError *error = NULL;
    GVariant *result;
    error = NULL;
	result = g_dbus_proxy_call_sync(objectManager,
					"GetManagedObjects",
					NULL,
					G_DBUS_CALL_FLAGS_NONE,
					-1,
					NULL,
					&error);
	if(error != NULL)
		return -1;
    if(result) {
        result = g_variant_get_child_value(result, 0);
        const gchar *object_path;
        GVariantIter i;
	    GVariant *ifaces_and_properties;
		g_variant_iter_init(&i, result);
		while(g_variant_iter_next(&i, "{&o@a{sa{sv}}}", &object_path, &ifaces_and_properties)) {
            const gchar *interface_name;
	    	GVariant *properties;
	    	GVariantIter ii;
	    	g_variant_iter_init(&ii, ifaces_and_properties);
	    	while(g_variant_iter_next(&ii, "{&s@a{sv}}", &interface_name, &properties)) {
	    		if(g_strstr_len(g_ascii_strdown(interface_name, -1), -1, "device")) {
                    printf("object_path: %s\n", object_path);
                    remove_commond(object_path);
                }
            }
        }
    }
    return 0;
}

static void objectManager_handler (GDBusProxy *proxy,
                       gchar *sender_name,
                       gchar *signal_name,
                       GVariant *params,
                       gpointer user_data);

static gboolean properties_changed(GDBusProxy *pProxy, GVariant *params, gpointer user_data)
{
    const gchar *interface_name;
    GVariantIter  *changed_properties;
    GVariantIter  *invalidated_properties;
    g_variant_get(params, "(sa{sv}as)", &interface_name, &changed_properties, &invalidated_properties);

    const gchar *object_path = g_dbus_proxy_get_object_path(pProxy);
    printf("properties_changed object_path:    %s\n", object_path);
    printf("properties_changed connected_path: %s\n", connected_path);
    // printf("properties_changed interface_name: %s\n", interface_name);

    const gchar *property_name;
    GVariant *property_value;
    if(g_strcmp0(connected_path, object_path) == 0) {
        while (g_variant_iter_next (changed_properties, "{sv}", &property_name, &property_value))
        {
            printf("    changed property_name: %s\n", property_name);
            if(g_strcmp0(property_name, "Connected") == 0) {
                int connected;
                g_variant_get(property_value, "b", &connected);
                if(!connected) {
                    printf("disconnect \n");
                    connected_path = "";
                    flag_connect = -1;
                } else {
                    flag_connect = 0;
                }
            }
        }
    }
    

    // while (g_variant_iter_next (invalidated_properties, "s", &property_name))
    // {
    //     printf("    invalidated property_name: %s\n", property_name);
    // }

}

static void properties_handler (GDBusProxy *proxy,
                       gchar *sender_name,
                       gchar *signal_name,
                       GVariant *params,
                       gpointer user_data) {
                       
    const gchar *object_path = g_dbus_proxy_get_object_path(proxy);
    printf("properties_handler object_path:    %s\n", object_path);
    if (g_strcmp0(signal_name, "PropertiesChanged") == 0) {
        // ignore all irrelevant signals
        properties_changed(proxy, params, user_data);
        return;
    } 
}

static gboolean interfaces_added(GDBusProxy *pProxy, GVariant *params, gpointer user_data)
{
    const gchar *path;
    const gchar *interface_name;
    GVariantIter  *ifaces_and_properties;
    GVariantIter  *properties;
    GError *error = NULL;
    
    g_variant_get(params, "(oa{sa{sv}})", &path, &ifaces_and_properties);

    GDBusProxy *properties1;
    properties1 = g_dbus_proxy_new_sync(conn,
				      G_DBUS_PROXY_FLAGS_NONE,
				      NULL,
				      "org.bluez",
				      path,
				      "org.freedesktop.DBus.Properties",
				      NULL,
				      &error);
	if(error != NULL) {
        printf("error\n");
        return 1;
    }

    // g_signal_connect(properties, "g-properties-changed", G_CALLBACK(properties_handler), NULL);
    g_signal_connect(properties1, "g-signal", G_CALLBACK(properties_handler), NULL);

    GDBusProxy *device1;
    device1 = g_dbus_proxy_new_sync(conn,
				      G_DBUS_PROXY_FLAGS_NONE,
				      NULL,
				      "org.bluez",
				      path,
				      "org.bluez.Device1",
				      NULL,
				      &error);
	if(error != NULL) {
        printf("error\n");
        return 1;
    }

    GVariant *rssi = NULL;
    gint16 rssi_num;

    // rssi = g_dbus_proxy_get_cached_property(device1, "RSSI");
    // if(rssi != NULL) {
    //     g_variant_get(rssi, "n", &rssi_num);
    //     printf("interfaces_added: odject path is \"%s\", RSSI: %d\n", path, rssi_num);
    // } else {
    //      printf("interfaces_added: odject path is \"%s\"\n", path);
    // }

    // while (g_variant_iter_next (ifaces_and_properties, "{sa{sv}}", &interface_name, &properties))
    // {
    //     printf("    interface_name: %s\n", interface_name);
    // }

    

    return TRUE;
}

static void objectManager_handler (GDBusProxy *proxy,
                       gchar *sender_name,
                       gchar *signal_name,
                       GVariant *params,
                       gpointer user_data) {
    // printf("objectManager_handler signal_name:%s\n", signal_name);
    if (g_strcmp0(signal_name, "InterfacesAdded") == 0) {
        // ignore all irrelevant signals
        interfaces_added(proxy, params, user_data);
        return;
    } 
                    
}

void* myFunction(void* arg) {
    printf("");
    GError *error = NULL;
    GVariant *result;
    

    while(1) {
        error = NULL;
	    result = g_dbus_proxy_call_sync(objectManager,
	    				"GetManagedObjects",
	    				NULL,
	    				G_DBUS_CALL_FLAGS_NONE,
	    				-1,
	    				NULL,
	    				&error);
	    if(error != NULL)
	    	return NULL;
        if(result && flag_connect != 0) {
            result = g_variant_get_child_value(result, 0);
            const gchar *object_path;
            GVariantIter i;
	        GVariant *ifaces_and_properties;
	    	g_variant_iter_init(&i, result);
	    	while(g_variant_iter_next(&i, "{&o@a{sa{sv}}}", &object_path, &ifaces_and_properties)) {
	    		const gchar *interface_name;
	    		GVariant *properties;
	    		GVariantIter ii;
	    		g_variant_iter_init(&ii, ifaces_and_properties);
	    		while(g_variant_iter_next(&ii, "{&s@a{sv}}", &interface_name, &properties)) {
	    			if(g_strstr_len(g_ascii_strdown(interface_name, -1), -1, "device")) {
                        const gchar *property_name;
                        GVariant *property;
                        GVariantIter j;
	    		        g_variant_iter_init(&j, properties);
                        while(g_variant_iter_next(&j, "{sv}", &property_name, &property)) {
                            if(g_strstr_len(g_ascii_strdown(property_name, -1), -1, "name")) {
                                const gchar *name;
                                g_variant_get(property, "s", &name);
                                if(g_strcmp0(name, "Mi Silent Mouse") == 0) {
                                    printf("checkPaired(properties): %d, checkConnected(properties): %d\n", checkPaired(properties), checkConnected(properties));
                                    if(checkConnected(properties) == 0) {
                                        flag_connect = pair_commond(object_path, checkPaired(properties));
                                        // pair_commond(object_path, checkPaired(properties));
                                        if(flag_connect == 0) {
                                            connected_path = object_path;
                                            printf("connected_path: %s\n", connected_path);
                                        } else {
                                            printf("remove_commond\n");
                                            remove_commond(object_path);
                                        }
                                        
                                    }
                                }
                            }
                        }
                        g_variant_unref(property);
                    }
	    			g_variant_unref(properties);
	    		}
	    		g_variant_unref(ifaces_and_properties);
	    	}
	    	g_variant_unref(result);
        }
        sleep(1);
    }
    

    return NULL;
}

static void bus_acquired_cb(GDBusConnection *connection,
                            const gchar     *bus_name,
                            gpointer         user_data)
{
    printf("bus_acquired_cb\n");
    GError *pError = NULL;

    /** Second step: Try to get a connection to the given bus. */
    AgentAgent1 *pInterface = agent_agent1_skeleton_new();

    /** Third step: Attach to dbus signals. */
    (void) g_signal_connect(pInterface, "handle-request-authorization", G_CALLBACK(request_authorization), NULL);

    /** Fourth step: Export interface skeleton. */
    (void) g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(pInterface),
                                            connection,
                                            "/org/bluez/agent1",
                                            &pError);
}

static void name_lost_cb(GDBusConnection* connection, 
                         const gchar* name,
                         gpointer user_data)
{
    g_error("Lost name on connection, or unable to own name: %s", name);
}

int main()
{
    printf("start\n");
    GMainLoop *mainloop;

    int rc = 0;
	
    
	GError *error = NULL;
    GVariant *result;

    //申请创建主循环
    mainloop = g_main_loop_new(NULL, FALSE);

    printf("start g_bus_get_sync\n");
    conn = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
	if(error != NULL)
		return 1;

    // (void)g_bus_own_name_on_connection(conn,
    //                      "org.bluez",
    //                      G_BUS_NAME_OWNER_FLAGS_NONE,
    //                      &bus_acquired_cb,
    //                      &name_lost_cb,
    //                      NULL,
    //                      NULL);
    const gchar* service_name = g_dbus_connection_get_unique_name(
            conn);
    bus_acquired_cb(conn, service_name, NULL);

    printf("start g_dbus_proxy_new_sync\n");
    objectManager = g_dbus_proxy_new_sync(conn,
				      G_DBUS_PROXY_FLAGS_NONE,
				      NULL,
				      "org.bluez",
				      "/",
				      "org.freedesktop.DBus.ObjectManager",
				      NULL,
				      &error);
	if(error != NULL)
		return 1;

    g_signal_connect(objectManager, "g-signal", G_CALLBACK(objectManager_handler), NULL);

    GDBusProxy *agentManager = g_dbus_proxy_new_sync(conn,
				      G_DBUS_PROXY_FLAGS_NONE,
				      NULL,
				      "org.bluez",
				      "/org/bluez",
				      "org.bluez.AgentManager1",
				      NULL,
				      &error);
	if(error != NULL)
		return 1;

    printf("start RegisterAgent\n");
    error = NULL;
	result = g_dbus_proxy_call_sync(agentManager,
					"RegisterAgent",
					g_variant_new("(os)", "/org/bluez/agent1", "NoInputNoOutput"),
					G_DBUS_CALL_FLAGS_NONE,
					-1,
					NULL,
					&error);
	if(error != NULL) {
        g_error("Failed to call RegisterAgent: %s", error->message); 
        g_error_free(error); 
        return 1;
    }


    adapter1 = g_dbus_proxy_new_sync(conn,
				      G_DBUS_PROXY_FLAGS_NONE,
				      NULL,
				      "org.bluez",
				      "/org/bluez/hci0",
				      "org.bluez.Adapter1",
				      NULL,
				      &error);
	if(error != NULL)
		return 1;

    remove_all_devices();

    printf("start g_dbus_proxy_call_sync\n");
    error = NULL;
	result = g_dbus_proxy_call_sync(adapter1,
					"StartDiscovery",
					NULL,
					G_DBUS_CALL_FLAGS_NONE,
					-1,
					NULL,
					&error);
	if(error != NULL)
		return 1;

    pthread_t myThread;
    if(pthread_create(&myThread, NULL, myFunction, NULL) != 0) {
        printf("Failed to create thread\n");
        return 1;
    }

    printf("start g_main_loop_run\n");

    g_main_loop_run(mainloop);
    if(objectManager)
		g_object_unref(objectManager);
    if(adapter1)
		g_object_unref(adapter1);
	if(conn)
		g_object_unref(conn);
	if(error)
		g_error_free(error);

}
