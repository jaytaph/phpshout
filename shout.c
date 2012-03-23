/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2010 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Joshua Thijssen <jthijssen@noxlogic.nl>                      |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "php.h"
#include "php_main.h"
#include "php_globals.h"
#include "zend_objects.h"
#include "zend_exceptions.h"
#include "ext/standard/info.h"
#include "php_shout.h"
#include "shout/shout.h"



typedef struct _php_shout_obj {
	zend_object zo;
        shout_t *shout;
} php_shout_obj;

/* The class entry pointers */
zend_class_entry *php_shout_exception_class_entry;
zend_class_entry *php_shout_sc_entry;

/* The object handlers */
static zend_object_handlers shout_object_handlers;

PHP_METHOD(shout, get_version) {
        if (zend_parse_parameters_none() == FAILURE) {
                RETURN_FALSE
        }

        RETURN_STRING(shout_version(NULL, NULL, NULL), 1);
}

/* {{{ proto shout::__construct()
 * Constructs a new shout object. */
PHP_METHOD(shout, __construct) {
	php_shout_obj *intern;
#if ZEND_MODULE_API_NO > 20060613
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, php_shout_exception_class_entry, &error_handling TSRMLS_CC);
#else
	php_set_error_handling(EH_THROW, php_shout_exception_class_entry TSRMLS_CC);
#endif

        if (zend_parse_parameters_none() == FAILURE) {
#if ZEND_MODULE_API_NO > 20060613
                zend_restore_error_handling(&error_handling TSRMLS_CC);
#else
                php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
#endif
		return;
	}

#if ZEND_MODULE_API_NO > 20060613
	zend_restore_error_handling(&error_handling TSRMLS_CC);
#else
	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
#endif

        intern = (php_shout_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        intern->shout = shout_new();
        if (intern->shout == NULL) {
                zend_throw_exception(php_shout_exception_class_entry, "Could not initialize shout structure!", 0 TSRMLS_CC);
                RETURN_FALSE
        }
}
/* }}} */


static void php_shout_get_handler_string(INTERNAL_FUNCTION_PARAMETERS, const char *(*func)(shout_t *)) {
        php_shout_obj *intern;

        if (zend_parse_parameters_none() == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_shout_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        unsigned char *tmp = (*func)(intern->shout);
        if (tmp == NULL) {
                RETURN_NULL();
        }
        RETURN_STRING(tmp, 1);
}
static void php_shout_get_handler_long(INTERNAL_FUNCTION_PARAMETERS, unsigned int(*func)(shout_t *)) {
        php_shout_obj *intern;

        if (zend_parse_parameters_none() == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_shout_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        RETURN_LONG((*func)(intern->shout));
}
static void php_shout_get_handler_short(INTERNAL_FUNCTION_PARAMETERS, unsigned short(*func)(shout_t *)) {
        php_shout_obj *intern;

        if (zend_parse_parameters_none() == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_shout_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        RETURN_LONG((*func)(intern->shout));
}

static void php_shout_get_handler_bool(INTERNAL_FUNCTION_PARAMETERS, unsigned int(*func)(shout_t *)) {
        php_shout_obj *intern;

        if (zend_parse_parameters_none() == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_shout_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        if ((*func)(intern->shout)) {
          RETURN_TRUE
        } else {
          RETURN_FALSE
        }
}



PHP_METHOD(shout, get_port) {
  php_shout_get_handler_short(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_port);
}
PHP_METHOD(shout, get_password) {
  php_shout_get_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_password);
}
PHP_METHOD(shout, get_mount) {
  php_shout_get_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_mount);
}
PHP_METHOD(shout, get_name) {
  php_shout_get_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_name);
}
PHP_METHOD(shout, get_url) {
  php_shout_get_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_url);
}
PHP_METHOD(shout, get_genre) {
  php_shout_get_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_genre);
}
PHP_METHOD(shout, get_agent) {
  php_shout_get_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_agent);
}
PHP_METHOD(shout, get_user) {
  php_shout_get_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_user);
}
PHP_METHOD(shout, get_description) {
  php_shout_get_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_description);
}
PHP_METHOD(shout, get_dumpfile) {
  php_shout_get_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_dumpfile);
}
PHP_METHOD(shout, get_public) {
  php_shout_get_handler_bool(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_public);
}
PHP_METHOD(shout, get_format) {
  php_shout_get_handler_long(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_format);
}
PHP_METHOD(shout, get_protocol) {
  php_shout_get_handler_long(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_protocol);
}
PHP_METHOD(shout, get_nonblocking) {
  php_shout_get_handler_bool(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_nonblocking);
}





PHP_METHOD(shout, get_host) {
        php_shout_obj *intern;

        if (zend_parse_parameters_none() == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_shout_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        RETURN_STRING(shout_get_host(intern->shout), 1);
}

PHP_METHOD(shout, set_host) {
        php_shout_obj *intern;
        unsigned char *host = NULL;
        long host_len = 0;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &host, &host_len) == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_shout_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        RETURN_LONG(shout_set_host(intern->shout, host));
}



/* {{{ */
static void shout_object_free(void *object TSRMLS_DC) {
	php_shout_obj *intern = (php_shout_obj *)object;

        // Free any internal data
        shout_free(intern->shout);

	zend_object_std_dtor(&intern->zo TSRMLS_CC);
	efree(object);
}
/* }}} */

/* {{{ */
static zend_object_value shout_object_new_ex(zend_class_entry *class_type, php_shout_obj **ptr TSRMLS_DC) {
	php_shout_obj *intern;
	zend_object_value retval;
	zval *tmp;

	intern = emalloc(sizeof(php_shout_obj));
	memset(intern, 0, sizeof(php_shout_obj));

	if (ptr) {
		*ptr = intern;
	}

	zend_object_std_init(&intern->zo, class_type TSRMLS_CC);
	zend_hash_copy(intern->zo.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	retval.handle = zend_objects_store_put(intern, NULL, (zend_objects_free_object_storage_t) shout_object_free, NULL TSRMLS_CC);
	retval.handlers = (zend_object_handlers *) &shout_object_handlers;
	return retval;
}
/* }}} */

/* {{{ */
static zend_object_value shout_object_new(zend_class_entry *class_type TSRMLS_DC) {
	return shout_object_new_ex(class_type, NULL TSRMLS_CC);
}
/* }}} */

static zend_function_entry shout_funcs[] = {
        PHP_ME(shout, __construct,      NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
        PHP_ME(shout, get_version,      NULL, ZEND_ACC_PUBLIC)

        PHP_ME(shout, get_host,         NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, set_host,         NULL, ZEND_ACC_PUBLIC)

        PHP_ME(shout, get_protocol,     NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, get_port,         NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, get_password,     NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, get_mount,        NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, get_name,         NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, get_url,          NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, get_genre,        NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, get_agent,        NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, get_user,         NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, get_description,  NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, get_dumpfile,     NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, get_public,       NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, get_format,       NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, get_nonblocking,  NULL, ZEND_ACC_PUBLIC)

                /*
  // Main
  void shout_init();
  void shout_shutdown();

  // Connection
  int shout_open(self);
  int shout_close(self);
  const char *shout_get_error(self);
  int shout_get_errno(self);

  int shout_send(	self, 	 data>, 	 len);
  ssize_t shout_send_raw(	self, 	 data, len);
  void shout_sync(self);
  int shout_delay(self);

  // Connection params
  int shout_set_host(	self, 	  host);
  const char *shout_get_host(self);
  int shout_set_port(self, port);
  int shout_set_user(	self, 	  user);
  const char *shout_get_user(self);
  int shout_set_pass(	self, pass)
  const char *shout_get_pass(self);
  int shout_set_protocol(	self, proto)
  int shout_set_format(	self, format)
  int shout_set_mount(	self, 	 mount);
  int shout_set_dumpfile(	self, dumpfile);
  int shout_set_agent(	self, 	 agent);

  // directory
  int shout_set_public(	self, makepublic)
  int shout_get_public(self);

  int shout_set_name(	self, name);
  int shout_set_url(	self, 	 url);
  int shout_set_description(	self, description)
  int shout_set_audio_info(	self, 	  name, 	  value);
  const char *shout_get_audio_info(	self, 	 name);


  // metadata
  shout_metadata_t *shout_metadata_new();
  void shout_metadata_free(	self);

  int shout_metadata_add(	self, 	  name, 	  value);
  int shout_set_metadata(	self, 	  metadata);
*/

	/* End of functions */
	{NULL, NULL, NULL}
};

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(shout)
{
	zend_class_entry ce;

        /* Set up the object handlers */
	memcpy(&shout_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

        // @TODO: should we do this here, or on the first activation of an object?
        // initialize shout
        shout_init();

	/*
	 * shout class
	 */
	INIT_CLASS_ENTRY(ce, "shout", shout_funcs);
	ce.create_object = shout_object_new;
	shout_object_handlers.clone_obj = NULL; // We do not allow clone (for now)
	//shout_object_handlers.read_property = shout_object_read_property;
	php_shout_sc_entry = zend_register_internal_class(&ce TSRMLS_CC);

        /*
         * Define class constants
         */
        zend_declare_class_constant_long(php_shout_sc_entry, ZEND_STRL("ERR_SUCCESS"),       SHOUTERR_SUCCESS TSRMLS_CC);
        zend_declare_class_constant_long(php_shout_sc_entry, ZEND_STRL("ERR_INSANE"),        SHOUTERR_INSANE TSRMLS_CC);
        zend_declare_class_constant_long(php_shout_sc_entry, ZEND_STRL("ERR_MALLOC"),        SHOUTERR_MALLOC TSRMLS_CC);
        zend_declare_class_constant_long(php_shout_sc_entry, ZEND_STRL("ERR_NOCONNECT"),     SHOUTERR_NOCONNECT TSRMLS_CC);
        zend_declare_class_constant_long(php_shout_sc_entry, ZEND_STRL("ERR_NOLOGIN"),       SHOUTERR_NOLOGIN TSRMLS_CC);
        zend_declare_class_constant_long(php_shout_sc_entry, ZEND_STRL("ERR_SOCKET"),        SHOUTERR_SOCKET TSRMLS_CC);
        zend_declare_class_constant_long(php_shout_sc_entry, ZEND_STRL("ERR_METADATA"),      SHOUTERR_METADATA TSRMLS_CC);
        zend_declare_class_constant_long(php_shout_sc_entry, ZEND_STRL("ERR_CONNECTED"),     SHOUTERR_CONNECTED TSRMLS_CC);
        zend_declare_class_constant_long(php_shout_sc_entry, ZEND_STRL("ERR_UNCONNECTED"),   SHOUTERR_UNCONNECTED TSRMLS_CC);
        zend_declare_class_constant_long(php_shout_sc_entry, ZEND_STRL("ERR_UNSUPPORTED"),   SHOUTERR_UNSUPPORTED TSRMLS_CC);

        zend_declare_class_constant_long(php_shout_sc_entry, ZEND_STRL("FORMAT_VORBIS"),   SHOUT_FORMAT_VORBIS TSRMLS_CC);
        zend_declare_class_constant_long(php_shout_sc_entry, ZEND_STRL("FORMAT_MP3"),      SHOUT_FORMAT_MP3 TSRMLS_CC);

        zend_declare_class_constant_long(php_shout_sc_entry, ZEND_STRL("PROTOCOL_HTTP"),         SHOUT_PROTOCOL_HTTP TSRMLS_CC);
        zend_declare_class_constant_long(php_shout_sc_entry, ZEND_STRL("PROTOCOL_XAUDIOCAST"),   SHOUT_PROTOCOL_XAUDIOCAST TSRMLS_CC);
        zend_declare_class_constant_long(php_shout_sc_entry, ZEND_STRL("PROTOCOL_ICY"),          SHOUT_PROTOCOL_ICY TSRMLS_CC);

        zend_declare_class_constant_string(php_shout_sc_entry, ZEND_STRL("AI_BITRATE"),     SHOUT_AI_BITRATE TSRMLS_CC);
        zend_declare_class_constant_string(php_shout_sc_entry, ZEND_STRL("AI_SAMPLERATE"),  SHOUT_AI_SAMPLERATE TSRMLS_CC);
        zend_declare_class_constant_string(php_shout_sc_entry, ZEND_STRL("AI_CHANNELS"),    SHOUT_AI_CHANNELS TSRMLS_CC);
        zend_declare_class_constant_string(php_shout_sc_entry, ZEND_STRL("AI_QUALITY"),     SHOUT_AI_QUALITY TSRMLS_CC);

	return SUCCESS;
}
/* }}} */

PHP_MSHUTDOWN_FUNCTION(shout) {
        shout_shutdown();

        return SUCCESS;
}

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(shout)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "shout support", "enabled");
	php_info_print_table_row(2, "Version", PHP_SHOUT_VERSION);
        php_info_print_table_row(2, "Shout Library Version", shout_version(NULL, NULL, NULL));
	php_info_print_table_end();
}
/* }}} */

zend_module_entry shout_module_entry = {
	STANDARD_MODULE_HEADER,
	"shout",
	shout_funcs,
	PHP_MINIT(shout),
	PHP_MSHUTDOWN(shout),
	NULL,
	NULL,
	PHP_MINFO(shout),
	PHP_SHOUT_VERSION,
	STANDARD_MODULE_PROPERTIES
};

//#ifdef COMPILE_DL_shout
ZEND_GET_MODULE(shout)
//#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */

