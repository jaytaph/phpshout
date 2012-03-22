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
#include "php_asn1.h"
#include "libtasn1.h"

// memory allocation macro for DER storage
#define DER_ALLOC(s, s_len, str_len) {         \
                s = (char *)emalloc(str_len);  \
                s_len = str_len-1;             \
                if (! s) {                     \
                        RETURN_FALSE           \
                }                              \
        }


typedef struct _php_asn1_obj {
	zend_object zo;
        ASN1_TYPE definitions;
        ASN1_TYPE structure;
        char *der;
        int der_len;
        int der_pos;
} php_asn1_obj;

/* The class entry pointers */
zend_class_entry *php_asn1_exception_class_entry;
zend_class_entry *php_asn1_sc_entry;

/* The object handlers */
static zend_object_handlers asn1_object_handlers;


// Maximum length for a DER length
#define MAX_DER_LEN  256


/* {{{ proto ASN1::__construct()
 * Constructs a new ASN1 object. */
PHP_METHOD(ASN1, __construct) {
	php_asn1_obj *intern;
#if ZEND_MODULE_API_NO > 20060613
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, php_asn1_exception_class_entry, &error_handling TSRMLS_CC);
#else
	php_set_error_handling(EH_THROW, php_asn1_exception_class_entry TSRMLS_CC);
#endif

        if (zend_parse_parameters_none() == FAILURE) {
#if ZEND_MODULE_API_NO > 20060613
                zend_restore_error_handling(&error_handling TSRMLS_CC);
#else
                php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
#endif
		return;
	}

	intern = (php_asn1_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        intern->definitions = ASN1_TYPE_EMPTY;
        intern->structure = ASN1_TYPE_EMPTY;
        intern->der = NULL;
        intern->der_len = 0;
        intern->der_pos = 0;

#if ZEND_MODULE_API_NO > 20060613
	zend_restore_error_handling(&error_handling TSRMLS_CC);
#else
	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
#endif
}
/* }}} */

/* {{{ proto static ASN1::get_error_string()
 * Returns textual representation for error */
PHP_METHOD(ASN1, get_error_string) {
        long error_code;
        const char *error_str;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &error_code) == FAILURE) {
		RETURN_FALSE
	}

        error_str = asn1_strerror(error_code);

        if (error_str) {
                RETURN_STRING(error_str, 1)
        }

        RETURN_NULL();
}
/* }}} */

/* {{{ proto static ASN1::get_version()
 * Returns current library version */
PHP_METHOD(ASN1, get_version) {

        if (zend_parse_parameters_none() == FAILURE) {
                RETURN_FALSE
        }

        RETURN_STRING(asn1_check_version(NULL), 1)
}
/* }}} */

/* {{{ proto static ASN1::check_version($required_version)
 * Check if the required version i */
PHP_METHOD(ASN1, check_version) {
	char *version = NULL;
	int version_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &version, &version_len) == FAILURE) {
		RETURN_FALSE
	}

        if (! asn1_check_version(version)) {
                RETURN_FALSE
        }

        RETURN_TRUE
}
/* }}} */


/* {{{ proto ASN1::asn1_parser2tree
 *  Return status code */
PHP_METHOD(ASN1, parser2tree) {
	php_asn1_obj *intern;
        char *filename = NULL;
	int filename_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &filename, &filename_len) == FAILURE) {
                RETURN_FALSE
        }

	intern = (php_asn1_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);

        int result = asn1_parser2tree(filename, &intern->definitions, NULL);
        RETURN_LONG(result)
}
/* }}} */


PHP_METHOD(ASN1, dump_structure) {
        php_asn1_obj *intern;
        char *filename = NULL;
        int filename_len = 0;
        char *tag = NULL;
        int tag_len = 0;
        int what = ASN1_PRINT_ALL;
        int structure = 0;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|ll", &filename, &filename_len, &tag, &tag_len, &what, &structure) == FAILURE) {
                RETURN_FALSE
        }

        if (what <= 0 || what >= ASN1_PRINT_ALL) {
                what = ASN1_PRINT_ALL;
        }

        FILE *f = fopen(filename, "w");
        if (!f) {
                RETURN_FALSE
        }

        intern = (php_asn1_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        asn1_print_structure(f, structure ? intern->definitions : intern->structure, tag, what);
        fclose(f);

        RETURN_TRUE
}

PHP_METHOD(ASN1, create_element) {
        php_asn1_obj *intern;
        char *name = NULL;
        int name_len = 0;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
                RETURN_FALSE
        }

        printf("Creating: %s \n", name);

        intern = (php_asn1_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        int result = asn1_create_element (intern->definitions, name, &intern->structure);

        RETURN_LONG(result)
}


PHP_METHOD(ASN1, delete_element) {
        php_asn1_obj *intern;
        char *name = NULL;
        int name_len = 0;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_asn1_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        int result = asn1_delete_element (intern->structure, name);

        RETURN_LONG(result)
}

PHP_METHOD(ASN1, count_elements) {
        php_asn1_obj *intern;
        int count = 0;

        if (zend_parse_parameters_none() == FAILURE) {
          RETURN_FALSE
        }

        intern = (php_asn1_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        int result = asn1_number_of_elements(intern->structure, "", &count);

        if (result != ASN1_SUCCESS) {
                RETURN_FALSE
        }

        RETURN_LONG(count)
}

PHP_METHOD(ASN1, write_element) {
        php_asn1_obj *intern;
        char *name = NULL;
        int name_len = 0;
        char *value = NULL;
        int value_len = 0;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &name, &name_len, &value, &value_len) == FAILURE) {
                RETURN_FALSE
        }

        // @TODO: we could check the tag. If the tag is a ASN1_TAG_BOOLEAN, we could check if our value is boolean too,
        // if not, we return FALSE (and a notice?)

        printf("Writing: %s with %s\n", name, value);

        intern = (php_asn1_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        int result = asn1_write_value (intern->structure, name, value, value_len);

        RETURN_LONG(result)
}

PHP_METHOD(ASN1, find_from_oid) {
        php_asn1_obj *intern;
        char *oid = NULL;
        int oid_len = 0;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &oid, &oid_len) == FAILURE) {
                RETURN_FALSE
        }

        printf("Find_from_oid: %s \n", oid);

        intern = (php_asn1_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        const char *result = asn1_find_structure_from_oid(intern->structure, oid);

        if (! result) {
                RETURN_NULL()
        }

        RETURN_STRING(result, 1)
}

PHP_METHOD(ASN1, read_tag) {
        php_asn1_obj *intern;
        char *tag = NULL;
        int tag_len = 0;
        int value, class = 0;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &tag, &tag_len) == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_asn1_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        int result = asn1_read_tag(intern->structure, tag, &value, &class);
        if (result != ASN1_SUCCESS) {
                RETURN_FALSE
        }

        object_init(return_value);
        zend_update_property_long(NULL, return_value, ZEND_STRS("value"), value TSRMLS_CC);
        zend_update_property_long(NULL, return_value, ZEND_STRS("class"), class TSRMLS_CC);
}

PHP_METHOD(ASN1, read_value) {
        php_asn1_obj *intern;
        char *tag = NULL;
        int tag_len = 0;

        char value[1024];
        int value_len = 1024-1;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &tag, &tag_len) == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_asn1_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        int result = asn1_read_value(intern->structure, tag, &value, &value_len);
        if (result != ASN1_SUCCESS) {
                RETURN_FALSE
        }


        int v, c = 0;
        result = asn1_read_tag(intern->structure, tag, &v, &c);
        switch (v) {
                case  ASN1_TAG_BOOLEAN :
                        if (value) {
                                RETURN_TRUE
                        } else {
                                RETURN_FALSE
                        }
                        break;
                case ASN1_TAG_INTEGER :
                        RETURN_STRINGL(value, value_len, 1);
                        break;
                case ASN1_TAG_NULL :
                        RETURN_NULL();
                        break;
                default :
                        RETURN_STRINGL(value, value_len-1, 1);
                        break;
        }

        // Failsafe return
        RETURN_FALSE
}

PHP_METHOD(ASN1, decode) {
        php_asn1_obj *intern;
        char *der = NULL;
        int der_len = 0;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &der, &der_len) == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_asn1_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        int result = asn1_der_decoding(&intern->structure, der, der_len, NULL);

        if (result != ASN1_SUCCESS) {
                RETURN_FALSE
        }

        RETURN_TRUE
}

PHP_METHOD(ASN1, der_encode_length) {
        php_asn1_obj *intern;
        int length = 0;
        unsigned char *der = NULL;
        int der_len = 0;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &length) == FAILURE) {
                RETURN_FALSE
        }

        DER_ALLOC(der, der_len, MAX_DER_LEN)

        intern = (php_asn1_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        asn1_length_der(length, der, &der_len);

        RETURN_STRINGL(der, der_len, 0);
}

PHP_METHOD(ASN1, der_encode_octet) {
        php_asn1_obj *intern;
        char *str = NULL;
        int str_len = 0;
        char *der = NULL;
        int der_len = 0;


        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
                RETURN_FALSE
        }

        DER_ALLOC(der, der_len, str_len + MAX_DER_LEN)

        intern = (php_asn1_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        asn1_octet_der(str, str_len, der, &der_len);

        RETURN_STRINGL(der, der_len, 0);
}

PHP_METHOD(ASN1, der_encode_bit) {
        php_asn1_obj *intern;
        char *str = NULL;
        int str_len = 0;
        int bit_len = 0;
        char *der = NULL;
        int der_len = 0;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &str, &str_len, &bit_len) == FAILURE) {
                RETURN_FALSE
        }

        // Can't have more useful bits than the length of the actual string
        if (str_len - 1 < (bit_len >> 3)) {
                php_error_docref(NULL TSRMLS_CC, E_WARNING, "Number of bits is higher than the length of the string");
                RETURN_FALSE
        }

        DER_ALLOC(der, der_len, str_len + MAX_DER_LEN)

        intern = (php_asn1_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        asn1_bit_der(str, bit_len + 1, der, &der_len);

        RETURN_STRINGL(der, der_len, 0);
}

PHP_METHOD(ASN1, der_stream_tell) {
          php_asn1_obj *intern;
        char *str = NULL;

        if (zend_parse_parameters_none() == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_asn1_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        RETURN_LONG(intern->der_pos);
}

PHP_METHOD(ASN1, der_stream_reset) {
        php_asn1_obj *intern;
        char *der = NULL;
        int der_len = 0;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &der, &der_len) == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_asn1_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        intern->der = der;
        intern->der_len = der_len;
        intern->der_pos = 0;

        return;
}

PHP_METHOD(ASN1, der_decode_length) {
        php_asn1_obj *intern;
        int tmp_pos = 0;

        if (zend_parse_parameters_none() == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_asn1_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        int length = asn1_get_length_der(intern->der + intern->der_pos, intern->der_len, &tmp_pos);
        intern->der_pos += tmp_pos;

        if (length < 0) {
                RETURN_FALSE
        }

        RETURN_LONG(length);
}

PHP_METHOD(ASN1, ber_decode_length) {
        php_asn1_obj *intern;
        int tmp_pos = 0;

        if (zend_parse_parameters_none() == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_asn1_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        int length = asn1_get_length_ber(intern->der + intern->der_pos, intern->der_len, &tmp_pos);
        intern->der_pos += tmp_pos;

        if (length < 0) {
                RETURN_FALSE
        }

        RETURN_LONG(length);
}

PHP_METHOD(ASN1, der_decode_tag) {
        php_asn1_obj *intern;
        unsigned char class;
        unsigned long tag;
        int tmp_pos = 0;

        if (zend_parse_parameters_none() == FAILURE) {
                RETURN_FALSE
        }

        intern = (php_asn1_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);

        printf ("DER    : %08X\n", intern->der);
        printf ("DER LEN: %d\n", intern->der_len);
        printf ("DER POS: %d\n", intern->der_pos);

        asn1_get_tag_der (intern->der + intern->der_pos, intern->der_len, &class, &tmp_pos, &tag);
        intern->der_pos += tmp_pos;

        object_init(return_value);
        add_property_long(return_value, "tag", tag TSRMLS_CC);
        add_property_long(return_value, "class", class TSRMLS_CC);
}

PHP_METHOD(ASN1, der_decode_octet) {
        php_asn1_obj *intern;
        char *str = NULL;
        int str_len = 0;
        int tmp_pos = 0;

        if (zend_parse_parameters_none() == FAILURE) {
                RETURN_FALSE
        }

        // Retrieve the length of the string we are decoding
        intern = (php_asn1_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);

        printf ("DER    : %08X\n", intern->der);
        printf ("DER    : %08X\n", intern->der + intern->der_pos);
        printf ("DER LEN: %d\n", intern->der_len);
        printf ("DER POS: %d\n", intern->der_pos);

        int ret_len = asn1_get_length_ber (intern->der + intern->der_pos, intern->der_len, &tmp_pos);
        //intern->der_pos += tmp_pos;

        printf ("RETLEN: %d\n", ret_len);

        DER_ALLOC(str, str_len, ret_len + MAX_DER_LEN)

        int ret = asn1_get_octet_der(intern->der + intern->der_pos, intern->der_len, &tmp_pos, str, str_len, &ret_len);
        printf ("TMP_POS: %d\n", tmp_pos);
        printf ("STR_LEN: %d\n", str_len);
        printf ("RET_LEN: %d\n", ret_len);

        intern->der_pos += tmp_pos;

        printf ("RETURN: %d\n", ret);
        if (ret != ASN1_SUCCESS) {
                free(str);
                RETURN_FALSE
        }

        printf("STR: %s\n",str);

        RETURN_STRINGL(str, str_len, 0);
}

PHP_METHOD(ASN1, der_decode_bit) {
        php_asn1_obj *intern;
        char *str = NULL;
        int str_len = 0;
        int tmp_pos = 0;

        if (zend_parse_parameters_none() == FAILURE) {
                RETURN_FALSE
        }

        // Retrieve the length of the string we are decoding
        intern = (php_asn1_obj *)zend_object_store_get_object(getThis() TSRMLS_CC);
        int ret_len = asn1_get_length_ber (intern->der + intern->der_pos, intern->der_len, &tmp_pos);

        DER_ALLOC(str, str_len, ret_len + MAX_DER_LEN)

        int ret = asn1_get_bit_der(intern->der + intern->der_pos, intern->der_len, &ret_len, str, str_len, &tmp_pos);
        intern->der_pos += tmp_pos;

        if (ret != ASN1_SUCCESS) {
                free(str);
                RETURN_FALSE
        }

        RETURN_STRINGL(str, str_len, 0);
}

/* {{{ */
static void asn1_object_free(void *object TSRMLS_DC) {
	php_asn1_obj *intern = (php_asn1_obj *)object;

        // Free any internal data
        asn1_delete_structure(&intern->definitions);
        asn1_delete_structure(&intern->structure);

	zend_object_std_dtor(&intern->zo TSRMLS_CC);
	efree(object);
}
/* }}} */

/* {{{ */
static zend_object_value asn1_object_new_ex(zend_class_entry *class_type, php_asn1_obj **ptr TSRMLS_DC) {
	php_asn1_obj *intern;
	zend_object_value retval;
	zval *tmp;

	intern = emalloc(sizeof(php_asn1_obj));
	memset(intern, 0, sizeof(php_asn1_obj));

	if (ptr) {
		*ptr = intern;
	}

	zend_object_std_init(&intern->zo, class_type TSRMLS_CC);
	zend_hash_copy(intern->zo.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	retval.handle = zend_objects_store_put(intern, NULL, (zend_objects_free_object_storage_t) asn1_object_free, NULL TSRMLS_CC);
	retval.handlers = (zend_object_handlers *) &asn1_object_handlers;
	return retval;
}
/* }}} */

/* {{{ */
static zend_object_value asn1_object_new(zend_class_entry *class_type TSRMLS_DC) {
	return asn1_object_new_ex(class_type, NULL TSRMLS_CC);
}
/* }}} */

/* {{{ */
static zend_object_value asn1_object_clone(zval *this_ptr TSRMLS_DC) {
	php_asn1_obj *new_obj = NULL;
	php_asn1_obj *old_obj = (php_asn1_obj *)zend_object_store_get_object(this_ptr TSRMLS_CC);
	zend_object_value        retval = asn1_object_new_ex(old_obj->zo.ce, &new_obj TSRMLS_CC);

	zend_objects_clone_members(&new_obj->zo, retval, &old_obj->zo, Z_OBJ_HANDLE_P(this_ptr) TSRMLS_CC);

        // @TODO: Copy over data
	//php_asn1_data_clone(&old_obj->definitions, &new_obj->definitions);

	return retval;
}
/* }}} */


static zend_function_entry asn1_funcs[] = {
  	PHP_ME(ASN1, __construct,      NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(ASN1, check_version,    NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
        PHP_ME(ASN1, get_version,      NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
        PHP_ME(ASN1, get_error_string, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
        PHP_ME(ASN1, parser2tree,      NULL, ZEND_ACC_PUBLIC)
        PHP_ME(ASN1, dump_structure,   NULL, ZEND_ACC_PUBLIC)
        PHP_ME(ASN1, create_element,   NULL, ZEND_ACC_PUBLIC)
        PHP_ME(ASN1, write_element,    NULL, ZEND_ACC_PUBLIC)
        PHP_ME(ASN1, delete_element,   NULL, ZEND_ACC_PUBLIC)
        PHP_ME(ASN1, count_elements,   NULL, ZEND_ACC_PUBLIC)
        PHP_ME(ASN1, find_from_oid,    NULL, ZEND_ACC_PUBLIC)
        PHP_ME(ASN1, read_tag,         NULL, ZEND_ACC_PUBLIC)
        PHP_ME(ASN1, read_value,       NULL, ZEND_ACC_PUBLIC)
        PHP_ME(ASN1, decode,           NULL, ZEND_ACC_PUBLIC)
        //PHP_ME(ASN1, encode,           NULL, ZEND_ACC_PUBLIC)


        PHP_ME(ASN1, der_stream_tell,     NULL, ZEND_ACC_PUBLIC)
        PHP_ME(ASN1, der_stream_reset,    NULL, ZEND_ACC_PUBLIC)
        PHP_ME(ASN1, der_encode_length,   NULL, ZEND_ACC_PUBLIC)
        PHP_ME(ASN1, der_encode_octet,    NULL, ZEND_ACC_PUBLIC)
        PHP_ME(ASN1, der_encode_bit,      NULL, ZEND_ACC_PUBLIC)
        PHP_ME(ASN1, ber_decode_length,   NULL, ZEND_ACC_PUBLIC)
        PHP_ME(ASN1, der_decode_length,   NULL, ZEND_ACC_PUBLIC)
        PHP_ME(ASN1, der_decode_tag,      NULL, ZEND_ACC_PUBLIC)
        PHP_ME(ASN1, der_decode_octet,    NULL, ZEND_ACC_PUBLIC)
        PHP_ME(ASN1, der_decode_bit,      NULL, ZEND_ACC_PUBLIC)
        //PHP_ME(ASN1, der_decode_element,       NULL, ZEND_ACC_PUBLIC)
        //PHP_ME(ASN1, der_decode_startend,       NULL, ZEND_ACC_PUBLIC)

	/* End of functions */
	{NULL, NULL, NULL}
};

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(asn1)
{
	zend_class_entry ce;

        /* Set up the object handlers */
	memcpy(&asn1_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

	/*
	 * ASN1 class
	 */
	INIT_CLASS_ENTRY(ce, "ASN1", asn1_funcs);
	ce.create_object = asn1_object_new;
	asn1_object_handlers.clone_obj = asn1_object_clone;
	//asn1_object_handlers.read_property = asn1_object_read_property;
	php_asn1_sc_entry = zend_register_internal_class(&ce TSRMLS_CC);

        /*
         * ASN1 constants (returned from read_tag)
         */
        zend_declare_class_constant_long(php_asn1_sc_entry, ZEND_STRL("CLASS_UNIVERSAL"),        ASN1_CLASS_UNIVERSAL TSRMLS_CC);
        zend_declare_class_constant_long(php_asn1_sc_entry, ZEND_STRL("CLASS_APPLICATION"),      ASN1_CLASS_APPLICATION TSRMLS_CC);
        zend_declare_class_constant_long(php_asn1_sc_entry, ZEND_STRL("CLASS_CONTEXT_SPECIFIC"), ASN1_CLASS_CONTEXT_SPECIFIC TSRMLS_CC);
        zend_declare_class_constant_long(php_asn1_sc_entry, ZEND_STRL("CLASS_PRIVATE"),          ASN1_CLASS_PRIVATE TSRMLS_CC);
        zend_declare_class_constant_long(php_asn1_sc_entry, ZEND_STRL("CLASS_STRUCTURED"),       ASN1_CLASS_STRUCTURED TSRMLS_CC);

        zend_declare_class_constant_long(php_asn1_sc_entry, ZEND_STRL("TAG_BOOLEAN"),         ASN1_TAG_BOOLEAN TSRMLS_CC);
        zend_declare_class_constant_long(php_asn1_sc_entry, ZEND_STRL("TAG_INTEGER"),         ASN1_TAG_INTEGER TSRMLS_CC);
        zend_declare_class_constant_long(php_asn1_sc_entry, ZEND_STRL("TAG_SEQUENCE"),        ASN1_TAG_SEQUENCE TSRMLS_CC);
        zend_declare_class_constant_long(php_asn1_sc_entry, ZEND_STRL("TAG_SET"),             ASN1_TAG_SET TSRMLS_CC);
        zend_declare_class_constant_long(php_asn1_sc_entry, ZEND_STRL("TAG_OCTET_STRING"),    ASN1_TAG_OCTET_STRING TSRMLS_CC);
        zend_declare_class_constant_long(php_asn1_sc_entry, ZEND_STRL("TAG_BIT_STRING"),      ASN1_TAG_BIT_STRING TSRMLS_CC);
        zend_declare_class_constant_long(php_asn1_sc_entry, ZEND_STRL("TAG_UTCTIME"),         ASN1_TAG_UTCTime TSRMLS_CC);
        zend_declare_class_constant_long(php_asn1_sc_entry, ZEND_STRL("TAG_GENERALIZEDTIME"), ASN1_TAG_GENERALIZEDTime TSRMLS_CC);
        zend_declare_class_constant_long(php_asn1_sc_entry, ZEND_STRL("TAG_OBJECT_ID"),       ASN1_TAG_OBJECT_ID TSRMLS_CC);
        zend_declare_class_constant_long(php_asn1_sc_entry, ZEND_STRL("TAG_ENUMERATED"),      ASN1_TAG_ENUMERATED TSRMLS_CC);
        zend_declare_class_constant_long(php_asn1_sc_entry, ZEND_STRL("TAG_NULL"),            ASN1_TAG_NULL TSRMLS_CC);
        zend_declare_class_constant_long(php_asn1_sc_entry, ZEND_STRL("TAG_GENERALSTRING"),   ASN1_TAG_GENERALSTRING TSRMLS_CC);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(asn1)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "asn.1 support", "enabled");
	php_info_print_table_row(2, "Version", PHP_ASN1_VERSION);
        php_info_print_table_row(2, "ASN1 Library Version", ASN1_VERSION);
	php_info_print_table_end();
}
/* }}} */

zend_module_entry asn1_module_entry = {
	STANDARD_MODULE_HEADER,
	"asn1",
	asn1_funcs,
	PHP_MINIT(asn1),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(asn1),
	PHP_ASN1_VERSION,
	STANDARD_MODULE_PROPERTIES
};

//#ifdef COMPILE_DL_ASN1
ZEND_GET_MODULE(asn1)
//#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */

