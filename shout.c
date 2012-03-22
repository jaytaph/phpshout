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
} php_shout_obj;

/* The class entry pointers */
zend_class_entry *php_shout_exception_class_entry;
zend_class_entry *php_shout_sc_entry;

/* The object handlers */
static zend_object_handlers shout_object_handlers;



/* {{{ */
static void shout_object_free(void *object TSRMLS_DC) {
	php_shout_obj *intern = (php_shout_obj *)object;

        // Free any internal data

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

/* {{{ */
static zend_object_value shout_object_clone(zval *this_ptr TSRMLS_DC) {
	php_shout_obj *new_obj = NULL;
	php_shout_obj *old_obj = (php_shout_obj *)zend_object_store_get_object(this_ptr TSRMLS_CC);
	zend_object_value        retval = shout_object_new_ex(old_obj->zo.ce, &new_obj TSRMLS_CC);

	zend_objects_clone_members(&new_obj->zo, retval, &old_obj->zo, Z_OBJ_HANDLE_P(this_ptr) TSRMLS_CC);

        // @TODO: Copy over data

	return retval;
}
/* }}} */


static zend_function_entry shout_funcs[] = {

	/* End of functions */
	{NULL, NULL, NULL}
};

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(shout)
{
	zend_class_entry ce;

        /* Set up the object handlers */
	memcpy(&shout_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

	/*
	 * shout class
	 */
	INIT_CLASS_ENTRY(ce, "shout", shout_funcs);
	ce.create_object = shout_object_new;
	shout_object_handlers.clone_obj = shout_object_clone;
	//shout_object_handlers.read_property = shout_object_read_property;
	php_shout_sc_entry = zend_register_internal_class(&ce TSRMLS_CC);

	return SUCCESS;
}
/* }}} */

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
	NULL,
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

