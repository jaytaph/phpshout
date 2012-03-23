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


/*
 * Internal shout object data
 */
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



/* Getter and setter handlers for strings, shorts, longs and bools */
static void php_shout_get_handler_string(INTERNAL_FUNCTION_PARAMETERS, const char *(*func)(shout_t *)) {
        php_shout_obj *intern;

        if (zend_parse_parameters_none() == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_shout_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        const char *tmp = (*func)(intern->shout);
        if (tmp == NULL) {
                RETURN_NULL();
        }
        RETURN_STRING(tmp, 1);
}

static void php_shout_get_handler_long(INTERNAL_FUNCTION_PARAMETERS, int(*func)(shout_t *)) {
        php_shout_obj *intern;

        if (zend_parse_parameters_none() == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_shout_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        RETURN_LONG((*func)(intern->shout));
}

static void php_shout_get_handler_ulong(INTERNAL_FUNCTION_PARAMETERS, unsigned int(*func)(shout_t *)) {
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

static void php_shout_set_handler_string(INTERNAL_FUNCTION_PARAMETERS, int(*func)(shout_t *, const char *)) {
        php_shout_obj *intern;
        unsigned char *str = NULL;
        long str_len = 0;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_shout_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        int ret = (*func)(intern->shout, str);
        RETURN_LONG(ret);
}

static void php_shout_set_handler_long(INTERNAL_FUNCTION_PARAMETERS, int(*func)(shout_t *, unsigned int)) {
        php_shout_obj *intern;
        long val = 0;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &val) == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_shout_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        int ret = (*func)(intern->shout, val);
        RETURN_LONG(ret);
}

static void php_shout_set_handler_short(INTERNAL_FUNCTION_PARAMETERS, int(*func)(shout_t *, short unsigned int)) {
        php_shout_obj *intern;
        long val = 0;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &val) == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_shout_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        int ret = (*func)(intern->shout, (unsigned short)(val & 0xFFFF));
        RETURN_LONG(ret);
}

static void php_shout_set_handler_bool(INTERNAL_FUNCTION_PARAMETERS, int(*func)(shout_t *, unsigned int)) {
        php_shout_obj *intern;
        long bool = 0;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "b", &bool) == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_shout_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        int ret = (*func)(intern->shout, bool);
        RETURN_LONG(ret);
}


/* Define the php getters and setters */

/* {{{ proto long shout::__get_errno()
 * Returns error number */
PHP_METHOD(shout, get_errno) { php_shout_get_handler_long(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_errno); }
/* }}} */

/* {{{ proto string shout::__get_error()
 * Returns error string */
PHP_METHOD(shout, get_error) { php_shout_get_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_error); }
/* }}} */

/* {{{ proto long shout::get_host()
 * Returns the current set host */
PHP_METHOD(shout, get_host) { php_shout_get_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_host); }
/* }}} */

/* {{{ proto long shout::set_host(string)
 * Sets the host to connect. Returns status code */
PHP_METHOD(shout, set_host) { php_shout_set_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_set_host); }
/* }}} */

PHP_METHOD(shout, get_port) { php_shout_get_handler_short(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_port); }
/* }}} */

/* {{{ proto long shout::set_port(long)
 * Sets the port number to connect. Returns status code */
PHP_METHOD(shout, set_port) { php_shout_set_handler_short(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_set_port); }
/* }}} */

/* {{{ proto string shout::get_password()
 * Returns the current set password */
PHP_METHOD(shout, get_password) { php_shout_get_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_password); }
/* }}} */

/* {{{ proto long shout::set_password(string)
 * Sets the password to connect. Returns status code */
PHP_METHOD(shout, set_password) { php_shout_set_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_set_password); }
/* }}} */

/* {{{ proto string shout::get_mount()
 * Returns the current set mount */
PHP_METHOD(shout, get_mount) { php_shout_get_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_mount); }
/* }}} */

/* {{{ proto long shout::set_mount(string)
 * Sets the mount to connect to. Returns status code */
PHP_METHOD(shout, set_mount) { php_shout_set_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_set_mount); }
/* }}} */

/* {{{ proto string shout::get_name()
 * Returns the current set name */
PHP_METHOD(shout, get_name) { php_shout_get_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_name); }
/* }}} */

/* {{{ proto long shout::set_name(string)
 * Sets the name. Returns status code */
PHP_METHOD(shout, set_name) { php_shout_set_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_set_name); }
/* }}} */

/* {{{ proto string shout::get_url()
 * Returns the current set url */
PHP_METHOD(shout, get_url) { php_shout_get_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_url); }
/* }}} */

/* {{{ proto long shout::set_url(string)
 * Sets the url. Returns status code */
PHP_METHOD(shout, set_url) { php_shout_set_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_set_url); }
/* }}} */

/* {{{ proto string shout::get_genre()
 * Returns the current set genre */
PHP_METHOD(shout, get_genre) { php_shout_get_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_genre); }
/* }}} */

/* {{{ proto long shout::set_genre(string)
 * Sets the genre. Returns status code */
PHP_METHOD(shout, set_genre) { php_shout_set_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_set_genre); }
/* }}} */

/* {{{ proto string shout::get_agent()
 * Returns the current set agent */
PHP_METHOD(shout, get_agent) { php_shout_get_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_agent); }
/* }}} */

/* {{{ proto long shout::set_agent(string)
 * Sets the agent. Returns status code */
PHP_METHOD(shout, set_agent) { php_shout_set_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_set_agent); }
/* }}} */

/* {{{ proto string shout::get_user()
 * Returns the current set user */
PHP_METHOD(shout, get_user) { php_shout_get_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_user); }
/* }}} */

/* {{{ proto long shout::set_user(string)
 * Sets the user. Returns status code */
PHP_METHOD(shout, set_user) { php_shout_set_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_set_user); }
/* }}} */

/* {{{ proto string shout::get_description()
 * Returns the current set description */
PHP_METHOD(shout, get_description) { php_shout_get_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_description); }
/* }}} */

/* {{{ proto long shout::set_description(string)
 * Sets the description of the feed. Returns status code */
PHP_METHOD(shout, set_description) { php_shout_set_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_set_description); }
/* }}} */

/* {{{ proto string shout::get_dumpfile()
 * Returns the current set dumpfile */
PHP_METHOD(shout, get_dumpfile) { php_shout_get_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_dumpfile); }
/* }}} */

/* {{{ proto long shout::set_dumpfile(string)
 * Sets the dumpfile. Returns status code */
PHP_METHOD(shout, set_dumpfile) { php_shout_set_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_set_dumpfile); }
/* }}} */

/* {{{ proto bool shout::get_public()
 * Returns true when the stream is public, false otherwise */
PHP_METHOD(shout, get_public) { php_shout_get_handler_bool(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_public); }
/* }}} */

/* {{{ proto long shout::set_public(bool)
 * Sets the stream to be public or not. Returns status code */
PHP_METHOD(shout, set_public) { php_shout_set_handler_bool(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_set_public); }
/* }}} */

/* {{{ proto long shout::get_format()
 * Returns the current set format */
PHP_METHOD(shout, get_format) { php_shout_get_handler_ulong(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_format); }
/* }}} */

/* {{{ proto long shout::set_format(long)
 * Sets the format of the feed. Returns status code */
PHP_METHOD(shout, set_format) { php_shout_set_handler_long(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_set_format); }
/* }}} */

/* {{{ proto long shout::get_protocol()
 * Returns the current set protocol */
PHP_METHOD(shout, get_protocol) { php_shout_get_handler_ulong(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_protocol); }
/* }}} */

/* {{{ proto long shout::set_protocol(long)
 * Sets the stream protocol. Returns status code */
PHP_METHOD(shout, set_protocol) { php_shout_set_handler_long(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_set_protocol); }
/* }}} */

/* {{{ proto bool shout::get_format()
 * Returns true when the stream is nonblocking, false otherwise */
PHP_METHOD(shout, get_nonblocking) { php_shout_get_handler_bool(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_nonblocking); }
/* }}} */

/* {{{ proto long shout::set_nonblocking(bool)
 * Sets the stream to be nonblock or not. Returns status code */
PHP_METHOD(shout, set_nonblocking) { php_shout_set_handler_bool(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_set_nonblocking); }
/* }}} */

/* {{{ proto long shout::get_connected()
 * Returns status code if the stream is connected or not*/
PHP_METHOD(shout, get_connected) {
        php_shout_obj *intern;

        if (zend_parse_parameters_none() == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_shout_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        if (shout_get_connected(intern->shout) == SHOUTERR_CONNECTED) {
                RETURN_TRUE
        } else {
                RETURN_FALSE
        }
}
/* }}} */

/* {{{ proto long shout::open()
 * Opens connection to the server. Returns status code */
PHP_METHOD(shout, open) {
        php_shout_obj *intern;

        if (zend_parse_parameters_none() == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_shout_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        RETURN_LONG(shout_open(intern->shout));
}
/* }}} */

/* {{{ proto long shout::close()
 * Closes connection to the server. Returns status code */
PHP_METHOD(shout, close) {
        php_shout_obj *intern;

        if (zend_parse_parameters_none() == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_shout_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        RETURN_LONG(shout_close(intern->shout));
}
/* }}} */

/* {{{ proto shout::get_connected()
 * Syncs / delays until it's time to send new data to the server */
PHP_METHOD(shout, sync) {
        php_shout_obj *intern;

        if (zend_parse_parameters_none() == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_shout_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        shout_sync(intern->shout);

        return;
}
/* }}} */

/* {{{ proto long shout::send(string)
 * Sends a string of data to the server. Returns status code*/
PHP_METHOD(shout, send) {
        php_shout_obj *intern;
        unsigned char *data = NULL;
        long data_len = 0;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &data, &data_len) == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_shout_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        int ret = shout_send(intern->shout, data, data_len);
        RETURN_LONG(ret);
}
/* }}} */

/* {{{ proto long shout::delay()
 * Returns the number of milliseconds before sending new data to the server. */
PHP_METHOD(shout, delay) {
        php_shout_obj *intern;

        if (zend_parse_parameters_none() == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_shout_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        int ret = shout_delay(intern->shout);
        RETURN_LONG(ret);
}
/* }}} */

/* {{{ proto string shout::get_audio_info(string)
 * Returns the value for the key in the audio info dictionary */
PHP_METHOD(shout, get_audio_info) {
        php_shout_obj *intern;
        unsigned char *key = NULL;
        long key_len = 0;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_shout_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        char *val = shout_get_audio_info(intern->shout, key);
        RETURN_STRING(val, 1);
}
/* }}} */

/* {{{ proto long shout::set_audio_info(string, string)
 * Set the value for the key in the audio info dictionary. Returns status */
PHP_METHOD(shout, set_audio_info) {
        php_shout_obj *intern;
        unsigned char *key = NULL;
        unsigned char *val = NULL;
        long key_len = 0;
        long val_len = 0;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &key, &key_len, &val, &val_len) == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_shout_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        int ret = shout_set_audio_info(intern->shout, key, val);
        RETURN_LONG(ret);
}
/* }}} */



/* {{{ proto long shout::get_queue_length()
 * Returns the value of the write queue */
PHP_METHOD(shout, get_queue_length) {
        php_shout_obj *intern;

        if (zend_parse_parameters_none() == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_shout_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        int len = shout_queuelen(intern->shout);
        RETURN_LONG(len);
}
/* }}} */



/* {{{ */
static void shout_object_free(void *object TSRMLS_DC) {
	php_shout_obj *intern = (php_shout_obj *)object;

        /* Free any internal data */
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
        PHP_ME(shout, get_errno,        NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, get_error,        NULL, ZEND_ACC_PUBLIC)

        PHP_ME(shout, open,             NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, close,            NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, get_connected,    NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, send,             NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, sync,             NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, delay,            NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, get_queue_length, NULL, ZEND_ACC_PUBLIC)

        PHP_ME(shout, get_host,         NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, set_host,         NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, get_protocol,     NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, set_protocol,     NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, get_port,         NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, set_port,         NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, get_password,     NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, set_password,     NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, get_mount,        NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, set_mount,        NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, get_name,         NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, set_name,         NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, get_url,          NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, set_url,          NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, get_genre,        NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, set_genre,        NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, get_agent,        NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, set_agent,        NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, get_user,         NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, set_user,         NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, get_description,  NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, set_description,  NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, get_dumpfile,     NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, set_dumpfile,     NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, get_public,       NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, set_public,       NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, get_format,       NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, set_format,       NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, get_nonblocking,  NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, set_nonblocking,  NULL, ZEND_ACC_PUBLIC)

        PHP_ME(shout, set_audio_info,  NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, get_audio_info,  NULL, ZEND_ACC_PUBLIC)

	/* End of functions */
	{NULL, NULL, NULL}
};

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(shout)
{
	zend_class_entry ce;

        /* Set up the object handlers */
	memcpy(&shout_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

        /* @TODO: should we do this here, or on the first activation of an object? */
        shout_init();

	/*
	 * shout class
	 */
	INIT_CLASS_ENTRY(ce, "shout", shout_funcs);
	ce.create_object = shout_object_new;
	shout_object_handlers.clone_obj = NULL; /* We do not allow clone (for now) */
	/*shout_object_handlers.read_property = shout_object_read_property;*/
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

