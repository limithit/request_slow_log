#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

#define REQUEST_DEFAULT_SLOW_TIMEOUT   1000
typedef struct {
    ngx_msec_t timeout;
    ngx_flag_t enable;
} ngx_http_log_request_speed_conf_t;

static ngx_int_t ngx_http_log_request_speed_filter_init (ngx_conf_t*);
static ngx_int_t ngx_http_log_request_speed_header_filter(ngx_http_request_t*);
static char * ngx_http_log_request_speed_merge_loc_conf(ngx_conf_t*, void*, void*);
static void * ngx_http_log_request_speed_create_loc_conf(ngx_conf_t*);

static ngx_command_t ngx_http_log_request_speed_filter_commands[] = {
    { ngx_string("log_request_speed_filter"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_log_request_speed_conf_t, enable),
      NULL },
    { ngx_string("log_request_speed_filter_timeout"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_msec_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_log_request_speed_conf_t, timeout),
      NULL },
      ngx_null_command
};

static ngx_http_module_t  ngx_http_log_request_speed_filter_ctx = {
    NULL,                                  /* preconfiguration */
    ngx_http_log_request_speed_filter_init,
    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */
    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */
    ngx_http_log_request_speed_create_loc_conf, /* create location configuration */
    ngx_http_log_request_speed_merge_loc_conf   /* merge location configuration */
};

ngx_module_t  ngx_http_log_request_speed_filter_module = {
    NGX_MODULE_V1,
    &ngx_http_log_request_speed_filter_ctx, /* module context */
    ngx_http_log_request_speed_filter_commands,    /* module directives */
    NGX_HTTP_MODULE,                       /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};

static ngx_http_output_header_filter_pt ngx_http_next_header_filter;

static void *
ngx_http_log_request_speed_create_loc_conf(ngx_conf_t *cf)
{
    ngx_http_log_request_speed_conf_t  *conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_log_request_speed_conf_t));
    if (conf == NULL) {
        return NGX_CONF_ERROR;
    }

    conf->enable = NGX_CONF_UNSET;
    conf->timeout = NGX_CONF_UNSET_MSEC;

    return conf;
}

static char *
ngx_http_log_request_speed_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_log_request_speed_conf_t *prev = parent;
    ngx_http_log_request_speed_conf_t *conf = child;
    ngx_conf_merge_uint_value(conf->timeout, prev->timeout, REQUEST_DEFAULT_SLOW_TIMEOUT);
    ngx_conf_merge_value(conf->enable, prev->enable, 0);
    return NGX_CONF_OK;
}

static ngx_int_t
ngx_http_log_request_speed_header_filter(ngx_http_request_t *r)
{
	ngx_time_t *tp;
	ngx_msec_t ms;
	ngx_http_log_request_speed_conf_t *lrsl;

	tp = ngx_timeofday();

	ms = (ngx_msec_t) ((tp->sec - r->start_sec) * 1000
			+ (tp->msec - r->start_msec));

    lrsl = ngx_http_get_module_loc_conf(r, ngx_http_log_request_speed_filter_module);

    if (lrsl->enable == 1)
    {

        if (ms >= lrsl->timeout)
        {
            ngx_log_error(NGX_LOG_ALERT, r->connection->log, 0,
                "--- process request time: %i ms --- ", ms);
        }
    }
    return ngx_http_next_header_filter(r);
}

static ngx_int_t
ngx_http_log_request_speed_filter_init (ngx_conf_t *cf)
{
    ngx_http_next_header_filter = ngx_http_top_header_filter;
    ngx_http_top_header_filter = ngx_http_log_request_speed_header_filter;
    return NGX_OK;
}
