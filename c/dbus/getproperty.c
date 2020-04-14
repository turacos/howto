/* 
 * SIMPLE HOW-TO FOR DBUS under LINUX SYSTEMD LAYER
 * How to use dbus api and call the method "GET" to retrieve a specific property
 * It's like the command line below: 
 * gdbus call --system --dest org.freedesktop.systemd1 \
 * --object-path /org/freedesktop/systemd1/unit/foo_barservice \
 *  -m org.freedesktop.DBus.Properties.Get org.freedesktop.systemd1.Service ExecMainPID
 *
 * This command return the execution main PID for a specific service as foo_barservice
 *
 * To compile:
 * gcc getProperty.c -lsystemd -o getProperty
 * ./getProperty
 *
 * Author : RAMJANALLY Ghoulseine
 * Date : 2020/04/13 
 * ref: https://www.freedesktop.org/wiki/Software/dbus/
 */


#include <systemd/sd-bus.h>
#include <stdio.h>

const char *__SD_BUS_DESTINATION = "org.freedesktop.systemd1";
const char *__SD_BUS_OBJECT = "/org/freedesktop/systemd1/unit/foo_barservice";
const char *__SD_BUS_INTERFACE =  "org.freedesktop.DBus.Properties";
const char *__SD_BUS_METHOD = "Get";
const char *__SD_BUS_METHOD_INPUT_PARAMETER_TYPES = "ss";
const char *__SD_BUS_METHOD_INPUT_PARAMETER1 = "org.freedesktop.systemd1.Service";
const char *__SD_BUS_METHOD_INPUT_PARAMETER2 = "ExecMainPID";


int print_value(sd_bus_message *reply,const char *content)
{
     sd_bus_error error = SD_BUS_ERROR_NULL;
     int ret;

     switch(content[0])
     {

       case SD_BUS_TYPE_UINT32:
         ;
         int32_t pid;
         ret = sd_bus_message_read(reply, "u", &pid);
         if (ret < 0)
         {
           fprintf(stderr, "Error message read  %d -  %s\n",error, strerror(-ret));
           return ret;
         }
         printf("ExecMainPID: %d\n",pid);
         break;

       default:
         printf("the type is not defined\n");
	 return 1;
	 break;
     }
     return 0;
}


int main(void) 
{
        sd_bus* bus = NULL;
        sd_bus_error error = SD_BUS_ERROR_NULL;
        sd_bus_message *reply = NULL;

	const char *content;
	int ret;

        ret = sd_bus_default_system(&bus);
        if (ret < 0)
	{
           fprintf(stderr, "Error dbus connection defaul system %d -  %s\n",error, strerror(-ret));
	   return ret;
	}

        ret = sd_bus_call_method(bus,
			__SD_BUS_DESTINATION,
			__SD_BUS_OBJECT,
			__SD_BUS_INTERFACE,
			__SD_BUS_METHOD,
			&error,
			&reply,
			__SD_BUS_METHOD_INPUT_PARAMETER_TYPES,
			__SD_BUS_METHOD_INPUT_PARAMETER1,
			__SD_BUS_METHOD_INPUT_PARAMETER2
			);

        if (ret < 0)
	{
           fprintf(stderr, "Error bus call method %s\n", strerror(-ret));
	   return ret;
	}

	ret = sd_bus_message_peek_type(reply, NULL, &content);
        if (ret < 0)
	{
           fprintf(stderr, "Error message peek type %d -  %s\n",error, strerror(-ret));
	   return ret;
	}

        ret = sd_bus_message_enter_container(reply, SD_BUS_TYPE_VARIANT, content);
        if (ret < 0)
	{
           fprintf(stderr, "Error message enter container %d -  %s\n",error, strerror(-ret));
	   return ret;
	}

	// print the value returned by the call method Get
	ret = print_value(reply,content);

        ret = sd_bus_message_exit_container(reply);
        if (ret < 0)
	{
              fprintf(stderr, "Error message exit container  %d -  %s\n",error, strerror(-ret));
	      return ret;
	}

        sd_bus_error_free(&error);
        sd_bus_unref(bus);

        return 0;
}


