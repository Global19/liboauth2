#ifndef _OAUTH2_NGINX_H_
#define _OAUTH2_NGINX_H_

/***************************************************************************
 *
 * Copyright (C) 2018-2019 - ZmartZone IT BV - www.zmartzone.eu
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @Author: Hans Zandbelt - hans.zandbelt@zmartzone.eu
 *
 **************************************************************************/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <oauth2/http.h>
#include <oauth2/log.h>
#include <oauth2/util.h>

#include <ngx_core.h>
#include <ngx_http.h>
#include <ngx_http_request.h>

// module

#define OAUTH2_NGINX_MODULE(module)                                            \
	extern ngx_module_t ngx_##module##_module;                             \
                                                                               \
	ngx_module_t *ngx_modules[] = {&ngx_##module##_module, NULL};          \
                                                                               \
	char *ngx_module_names[] = {OAUTH2_TOSTRING(ngx_##module##_module),    \
				    NULL};                                     \
                                                                               \
	char *ngx_module_order[] = {NULL};

// functions

#define OAUTH2_NGINX_CFG_FUNC_START(type, member, module, primitive)           \
	static char *ngx_##module##_set_##primitive(                           \
	    ngx_conf_t *cf, ngx_command_t *cmd, void *conf)                    \
	{                                                                      \
		const char *rv = NULL;                                         \
		type *cfg = (type *)conf;                                      \
		ngx_str_t *value = cf->args->elts;
// fprintf(stderr, " ## %s: %p (log=%p)\n", __FUNCTION__, cfg, cf->log);

#define OAUTH2_NGINX_CFG_FUNC_END(cf, rv)                                      \
	if (rv)                                                                \
		ngx_log_error(NGX_LOG_ERR, cf->log, 0, rv);                    \
	return rv ? NGX_CONF_ERROR : NGX_CONF_OK;                              \
	}

#define OAUTH2_NGINX_CFG_FUNC_ARGS1(type, member, module, primitive)           \
	OAUTH2_NGINX_CFG_FUNC_START(type, member, module, primitive)           \
	char *v1 = cf->args->nelts > 1                                         \
		       ? oauth2_strndup((const char *)value[1].data,           \
					(size_t)value[1].len)                  \
		       : NULL;                                                 \
	rv = module##_set_##primitive(cfg->cfg, v1);                           \
	oauth2_mem_free(v1);                                                   \
	OAUTH2_NGINX_CFG_FUNC_END(cf, rv)

#define OAUTH2_NGINX_CFG_FUNC_ARGS2(type, member, module, primitive)           \
	OAUTH2_NGINX_CFG_FUNC_START(type, member, module, primitive)           \
	char *v1 = cf->args->nelts > 1                                         \
		       ? oauth2_strndup((const char *)value[1].data,           \
					(size_t)value[1].len)                  \
		       : NULL;                                                 \
	char *v2 = cf->args->nelts > 2                                         \
		       ? oauth2_strndup((const char *)value[2].data,           \
					(size_t)value[2].len)                  \
		       : NULL;                                                 \
	rv = module##_set_##primitive(cfg->cfg, v1, v2);                       \
	oauth2_mem_free(v2);                                                   \
	oauth2_mem_free(v1);                                                   \
	OAUTH2_NGINX_CFG_FUNC_END(cf, rv)

// commands

#define OAUTH2_NGINX_CMD(module, directive, primitive, take)                   \
	{                                                                      \
		ngx_string(directive), NGX_HTTP_LOC_CONF | take,               \
		    ngx_##module##_set_##primitive, NGX_HTTP_LOC_CONF_OFFSET,  \
		    0, NULL                                                    \
	}

#define OAUTH2_NGINX_CMD_TAKE1(module, directive, primitive)                   \
	OAUTH2_NGINX_CMD(module, directive, primitive, NGX_CONF_TAKE1)

#define OAUTH2_NGINX_CMD_TAKE12(module, directive, primitive)                  \
	OAUTH2_NGINX_CMD(module, directive, primitive, NGX_CONF_TAKE12)

#define OAUTH2_NGINX_CMD_TAKE23(module, directive, primitive)                  \
	OAUTH2_NGINX_CMD(module, directive, primitive, NGX_CONF_TAKE23)

#define OAUTH2_NGINX_CMD_TAKE34(module, directive, primitive)                  \
	OAUTH2_NGINX_CMD(module, directive, primitive,                         \
			 NGX_CONF_TAKE3 | NGX_CONF_TAKE4)

// logging

void oauth2_nginx_log(oauth2_log_sink_t *sink, const char *filename,
		      unsigned long line, const char *function,
		      oauth2_log_level_t level, const char *msg);

// requests

typedef struct oauth2_nginx_request_context_t {
	oauth2_log_t *log;
	ngx_http_request_t *r;
} oauth2_nginx_request_context_t;

oauth2_nginx_request_context_t *
oauth2_nginx_request_context_init(ngx_http_request_t *r);
void oauth2_nginx_request_context_free(void *rec);

#endif /* _OAUTH2_NGINX_H_ */