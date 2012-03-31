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

PHP_METHOD(shout, getVersion) {
        if (zend_parse_parameters_none() == FAILURE) {
                RETURN_FALSE
        }

        RETURN_STRING(shout_version(NULL, NULL, NULL), 1);
}

/* {{{ proto shout::__construct()
 * Constructs a new shout object. */
PHP_METHOD(shout, __construct) {
	php_shout_obj *intern;

        if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

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

/* {{{ proto long shout::__getErrno()
 * Returns error number */
PHP_METHOD(shout, getErrno) { php_shout_get_handler_long(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_errno); }
/* }}} */

/* {{{ proto string shout::__getError()
 * Returns error string */
PHP_METHOD(shout, getError) { php_shout_get_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_error); }
/* }}} */

/* {{{ proto long shout::getHost()
 * Returns the current set host */
PHP_METHOD(shout, getHost) { php_shout_get_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_host); }
/* }}} */

/* {{{ proto long shout::setHost(string)
 * Sets the host to connect. Returns status code */
PHP_METHOD(shout, setHost) { php_shout_set_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_set_host); }
/* }}} */

/* {{{ proto long shout::getPort()
 * Returns the current set port */
PHP_METHOD(shout, getPort) { php_shout_get_handler_short(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_port); }
/* }}} */

/* {{{ proto long shout::setPort(long)
 * Sets the port number to connect. Returns status code */
PHP_METHOD(shout, setPort) { php_shout_set_handler_short(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_set_port); }
/* }}} */

/* {{{ proto string shout::getPassword()
 * Returns the current set password */
PHP_METHOD(shout, getPassword) { php_shout_get_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_password); }
/* }}} */

/* {{{ proto long shout::setPassword(string)
 * Sets the password to connect. Returns status code */
PHP_METHOD(shout, setPassword) { php_shout_set_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_set_password); }
/* }}} */

/* {{{ proto string shout::getMount()
 * Returns the current set mount */
PHP_METHOD(shout, getMount) { php_shout_get_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_mount); }
/* }}} */

/* {{{ proto long shout::setMount(string)
 * Sets the mount to connect to. Returns status code */
PHP_METHOD(shout, setMount) { php_shout_set_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_set_mount); }
/* }}} */

/* {{{ proto string shout::getName()
 * Returns the current set name */
PHP_METHOD(shout, getName) { php_shout_get_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_name); }
/* }}} */

/* {{{ proto long shout::setName(string)
 * Sets the name. Returns status code */
PHP_METHOD(shout, setName) { php_shout_set_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_set_name); }
/* }}} */

/* {{{ proto string shout::getUrl()
 * Returns the current set url */
PHP_METHOD(shout, getUrl) { php_shout_get_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_url); }
/* }}} */

/* {{{ proto long shout::setUrl(string)
 * Sets the url. Returns status code */
PHP_METHOD(shout, setUrl) { php_shout_set_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_set_url); }
/* }}} */

/* {{{ proto string shout::getGenre()
 * Returns the current set genre */
PHP_METHOD(shout, getGenre) { php_shout_get_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_genre); }
/* }}} */

/* {{{ proto long shout::setGenre(string)
 * Sets the genre. Returns status code */
PHP_METHOD(shout, setGenre) { php_shout_set_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_set_genre); }
/* }}} */

/* {{{ proto string shout::getAgent()
 * Returns the current set agent */
PHP_METHOD(shout, getAgent) { php_shout_get_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_agent); }
/* }}} */

/* {{{ proto long shout::setAgent(string)
 * Sets the agent. Returns status code */
PHP_METHOD(shout, setAgent) { php_shout_set_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_set_agent); }
/* }}} */

/* {{{ proto string shout::getUser()
 * Returns the current set user */
PHP_METHOD(shout, getUser) { php_shout_get_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_user); }
/* }}} */

/* {{{ proto long shout::setUser(string)
 * Sets the user. Returns status code */
PHP_METHOD(shout, setUser) { php_shout_set_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_set_user); }
/* }}} */

/* {{{ proto string shout::getDescription()
 * Returns the current set description */
PHP_METHOD(shout, getDescription) { php_shout_get_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_description); }
/* }}} */

/* {{{ proto long shout::setDescription(string)
 * Sets the description of the feed. Returns status code */
PHP_METHOD(shout, setDescription) { php_shout_set_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_set_description); }
/* }}} */

/* {{{ proto string shout::getDumpfile()
 * Returns the current set dumpfile */
PHP_METHOD(shout, getDumpfile) { php_shout_get_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_dumpfile); }
/* }}} */

/* {{{ proto long shout::setDumpfile(string)
 * Sets the dumpfile. Returns status code */
PHP_METHOD(shout, setDumpfile) { php_shout_set_handler_string(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_set_dumpfile); }
/* }}} */

/* {{{ proto bool shout::getPublic()
 * Returns true when the stream is public, false otherwise */
PHP_METHOD(shout, getPublic) { php_shout_get_handler_bool(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_public); }
/* }}} */

/* {{{ proto long shout::setPublic(bool)
 * Sets the stream to be public or not. Returns status code */
PHP_METHOD(shout, setPublic) { php_shout_set_handler_bool(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_set_public); }
/* }}} */

/* {{{ proto long shout::getFormat()
 * Returns the current set format */
PHP_METHOD(shout, getFormat) { php_shout_get_handler_ulong(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_format); }
/* }}} */

/* {{{ proto long shout::setFormat(long)
 * Sets the format of the feed. Returns status code */
PHP_METHOD(shout, setFormat) { php_shout_set_handler_long(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_set_format); }
/* }}} */

/* {{{ proto long shout::getProtocol()
 * Returns the current set protocol */
PHP_METHOD(shout, getProtocol) { php_shout_get_handler_ulong(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_protocol); }
/* }}} */

/* {{{ proto long shout::setProtocol(long)
 * Sets the stream protocol. Returns status code */
PHP_METHOD(shout, setProtocol) { php_shout_set_handler_long(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_set_protocol); }
/* }}} */

/* {{{ proto bool shout::getNonblocking()
 * Returns true when the stream is nonblocking, false otherwise */
PHP_METHOD(shout, getNonblocking) { php_shout_get_handler_bool(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_get_nonblocking); }
/* }}} */

/* {{{ proto long shout::setNonblocking(bool)
 * Sets the stream to be nonblock or not. Returns status code */
PHP_METHOD(shout, setNonblocking) { php_shout_set_handler_bool(INTERNAL_FUNCTION_PARAM_PASSTHRU, shout_set_nonblocking); }
/* }}} */

/* {{{ proto long shout::getConnected()
 * Returns status code if the stream is connected or not*/
PHP_METHOD(shout, getConnected) {
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

/* {{{ proto shout::sync()
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

/* {{{ proto string shout::getAudioInfo(string)
 * Returns the value for the key in the audio info dictionary */
PHP_METHOD(shout, getAudioInfo) {
        php_shout_obj *intern;
        char *key = NULL;
        long key_len = 0;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_shout_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        const char *val = shout_get_audio_info(intern->shout, key);
        RETURN_STRING(val, 1);
}
/* }}} */

/* {{{ proto long shout::setAudioInfo(string, string)
 * Set the value for the key in the audio info dictionary. Returns status */
PHP_METHOD(shout, setAudioInfo) {
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


/* {{{ proto long shout::setMetadata(array)
 * Set the stream metadata. Returns status */
PHP_METHOD(shout, setMetadata) {
        php_shout_obj *intern;
        zval *arr = NULL;
        zval **current;
        shout_metadata_t *meta;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &arr) == FAILURE) {
                RETURN_FALSE
        }

        /* Initialize metadata structure */
        meta = shout_metadata_new();

        /* Iterate array and populate metadata structure */
        for (zend_hash_internal_pointer_reset(HASH_OF(arr));
             zend_hash_get_current_data(HASH_OF(arr), (void**) &current) == SUCCESS;
             zend_hash_move_forward(HASH_OF(arr))) {

             char  *str_key = NULL;
             uint   str_key_len;
             ulong  num_key;
             char  *data_val;

             /* separate current value and convert to string */
             SEPARATE_ZVAL(current);
             convert_to_string_ex(current);

             /* Get key of current item, convert to string if needed */
             zend_hash_get_current_key_ex(HASH_OF(arr), &str_key, &str_key_len, &num_key, 0, NULL);
             if (!str_key) {
                     spprintf(&str_key, 0, "%ld", num_key);
                     str_key_len = strlen(str_key)+1;
             }

             /* add key => data to structure*/
             shout_metadata_add(meta, str_key, Z_STRVAL_PP(current));
        }

        /* Send meta data*/
        intern = (php_shout_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        int ret = shout_set_metadata(intern->shout, meta);
        shout_metadata_free(meta);

        RETURN_LONG(ret);
}


/* {{{ proto long shout::getQueueLength()
 * Returns the value of the write queue */
PHP_METHOD(shout, getQueueLength) {
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
        PHP_ME(shout, getVersion,      NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, getErrno,        NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, getError,        NULL, ZEND_ACC_PUBLIC)

        PHP_ME(shout, open,             NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, close,            NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, getConnected,    NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, send,             NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, sync,             NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, delay,            NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, getQueueLength, NULL, ZEND_ACC_PUBLIC)

        PHP_ME(shout, getHost,         NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, setHost,         NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, getProtocol,     NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, setProtocol,     NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, getPort,         NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, setPort,         NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, getPassword,     NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, setPassword,     NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, getMount,        NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, setMount,        NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, getName,         NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, setName,         NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, getUrl,          NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, setUrl,          NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, getGenre,        NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, setGenre,        NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, getAgent,        NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, setAgent,        NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, getUser,         NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, setUser,         NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, getDescription,  NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, setDescription,  NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, getDumpfile,     NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, setDumpfile,     NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, getPublic,       NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, setPublic,       NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, getFormat,       NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, setFormat,       NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, getNonblocking,  NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, setNonblocking,  NULL, ZEND_ACC_PUBLIC)

        PHP_ME(shout, setAudioInfo,  NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, getAudioInfo,  NULL, ZEND_ACC_PUBLIC)
        PHP_ME(shout, setMetadata,    NULL, ZEND_ACC_PUBLIC)

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

