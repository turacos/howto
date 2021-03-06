/* 
 * SIMPLE HOW-TO FOR DBUS under LINUX SYSTEMD LAYER
 * How to use dbus api and call the method GETALL to retrieve all properties
 * it's like using the command line below: 
 * gdbus call --system --dest org.freedesktop.systemd1 \
 * --object-path /org/freedesktop/systemd1/unit/foo_barservice \
 *  -m org.freedesktop.DBus.Properties.GetAll org.freedesktop.systemd1.Service
 *
 * This command return all the information from a selected unit
 *
 * To compile:
 * gcc dump-sdbus.c -lsystemd -o dump-sdbus
 *
 * Copyright (C) 2020 RAMJANALLY Ghoulseine
 * This program is free software: you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software Foundation, 
 * either version 3 of the License, or (at your option) any later version. 
 *
 * Creation Date : 2020/04/13 
 * Modified Date : 2020/04/16
 * ref: https://www.freedesktop.org/wiki/Software/dbus/
 *
 */


#include <systemd/sd-bus.h>
#include <stdio.h>
#include <stdbool.h>

#define DUMP_START -1
#define DUMP_END -2

const char *__SD_BUS_DESTINATION = "org.freedesktop.systemd1";
const char *__SD_BUS_OBJECT = "/org/freedesktop/systemd1/unit/foobar_2eservice";
const char *__SD_BUS_INTERFACE =  "org.freedesktop.DBus.Properties";
const char *__SD_BUS_METHOD = "GetAll";
const char *__SD_BUS_METHOD_INPUT_PARAMETER_TYPES = "s";
const char *__SD_BUS_METHOD_INPUT_PARAMETER1 = "org.freedesktop.systemd1.Service";

bool 
is_container(char c) {
    static const char valid[] = {
      SD_BUS_TYPE_ARRAY,
      SD_BUS_TYPE_VARIANT,
      SD_BUS_TYPE_STRUCT,
      SD_BUS_TYPE_DICT_ENTRY
    };
    return !!memchr(valid, c, sizeof(valid));
}

static int 
print_value(sd_bus_message *m, char type)
{
  int r;
  union {
   uint8_t u8;
   uint16_t u16;
   int16_t s16;
   uint32_t u32;
   int32_t s32;
   uint64_t u64;
   int64_t s64;
   double d64;
   const char *string;
   int i;
  } value;

  r = sd_bus_message_read_basic(m, type, &value);
  if (r < 0)
     return r;

  switch (type) 
  {
    case SD_BUS_TYPE_BYTE:
         printf("%u", value.u8);
         break;

    case SD_BUS_TYPE_BOOLEAN:
         printf("%d", value.i);
         break;

    case SD_BUS_TYPE_INT16:
         printf("%i", value.s16);
         break;

    case SD_BUS_TYPE_UINT16:
         printf("%u", value.u16);
         break;

    case SD_BUS_TYPE_INT32:
         printf("%i", value.s32);
         break;

    case SD_BUS_TYPE_UINT32:
         printf("%u", value.u32);
         break;

    case SD_BUS_TYPE_INT64:
         printf("%" PRIi64, value.s64);
         break;

    case SD_BUS_TYPE_UINT64:
         printf("%" PRIu64, value.u64);
         break;

    case SD_BUS_TYPE_DOUBLE:
         printf("%g", value.d64);
         break;

    case SD_BUS_TYPE_STRING:
    case SD_BUS_TYPE_OBJECT_PATH:
    case SD_BUS_TYPE_SIGNATURE: 
    case SD_BUS_TYPE_VARIANT: 
    {
         printf("'%s'", value.string);
         break;
    }

    case SD_BUS_TYPE_UNIX_FD:
         printf("%i", value.i);
         break;

    default:
         printf("Type is not defined !");
  }
  return 0;
}

static int 
dump_info(sd_bus_message *m, int nb_containers) 
{
    int r;
    const char *contents = NULL;
    char type;
    bool f=false;

    if(nb_containers == DUMP_START)
	    f=true;

    r = sd_bus_message_peek_type(m, &type, &contents);
    if (r < 0)
       return r;
    if (r == 0)
    {
       if(nb_containers == DUMP_END)
          return 0;
    }

    while(1)
    {
      if(is_container(type) > 0)
      {
        r = sd_bus_message_enter_container(m, type, contents);
        if (r < 0)
          return r;
        nb_containers++;
	f=true;
      }
      r = sd_bus_message_peek_type(m, &type, &contents);
      if (r < 0)
         return r;

      if (r == 0)
      {
         r = sd_bus_message_exit_container(m);
         if (r < 0)
           return r;
         nb_containers--;
         break;
      }

      r = print_value(m,type);
      if(f!=true)
        printf(" ");

      f=false;
    }

    if(nb_containers == 0)
      printf("\n");

    if(nb_containers == DUMP_START)
        nb_containers = DUMP_END;

    r = dump_info(m,nb_containers);
    return r;
}

int 
main(void) 
{
        sd_bus* bus = NULL;
        sd_bus_error error = SD_BUS_ERROR_NULL;
        sd_bus_message *reply = NULL;

	int ret;

        ret = sd_bus_default_system(&bus);
        if (ret < 0)
	{
           fprintf(stderr, "Error dbus connection defaul system %s -  %s\n", error.name, error.message);
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
			__SD_BUS_METHOD_INPUT_PARAMETER1
			);

        if (ret < 0)
	{
           fprintf(stderr, "Error bus call method %s - %s\n", error.name, error.message);
	   return ret;
	}

        ret = dump_info(reply,DUMP_START);
	if ( ret < 0 )
           fprintf(stderr, "Error in dump call, program abort !\n");
	  

        sd_bus_error_free(&error);
        sd_bus_unref(bus);

        return ret;
}

