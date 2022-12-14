/*
 * libwebsockets - small server side websockets and web server implementation
 *
 * Copyright (C) 2010-2019 Andy Green <andy@warmcat.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation:
 *  version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA  02110-1301  USA
 */

#include "core/private.h"

#ifdef LWS_HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

signed char char_to_hex(const char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';

	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;

	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;

	return -1;
}
#if !defined(LWS_PLAT_OPTEE)

int lws_open(const char *__file, int __oflag, ...)
{
	va_list ap;
	int n;

	va_start(ap, __oflag);
	if (((__oflag & O_CREAT) == O_CREAT)
#if defined(O_TMPFILE)
		|| ((__oflag & O_TMPFILE) == O_TMPFILE)
#endif
	)
		/* last arg is really a mode_t.  But windows... */
		n = open(__file, __oflag, va_arg(ap, uint32_t));
	else
		n = open(__file, __oflag);
	va_end(ap);

	if (n != -1 && lws_plat_apply_FD_CLOEXEC(n)) {
		close(n);

		return -1;
	}

	return n;
}
#endif


void
lws_dll_add_front(struct lws_dll *d, struct lws_dll *phead)
{
	if (d->prev)
		return;

	/* our next guy is current first guy */
	d->next = phead->next;
	/* if there is a next guy, set his prev ptr to our next ptr */
	if (d->next)
		d->next->prev = d;
	/* our prev ptr is first ptr */
	d->prev = phead;
	/* set the first guy to be us */
	phead->next = d;
}

/* situation is:
 *
 *  HEAD: struct lws_dll * = &entry1
 *
 *  Entry 1: struct lws_dll  .pprev = &HEAD , .next = Entry 2
 *  Entry 2: struct lws_dll  .pprev = &entry1 , .next = &entry2
 *  Entry 3: struct lws_dll  .pprev = &entry2 , .next = NULL
 *
 *  Delete Entry1:
 *
 *   - HEAD = &entry2
 *   - Entry2: .pprev = &HEAD, .next = &entry3
 *   - Entry3: .pprev = &entry2, .next = NULL
 *
 *  Delete Entry2:
 *
 *   - HEAD = &entry1
 *   - Entry1: .pprev = &HEAD, .next = &entry3
 *   - Entry3: .pprev = &entry1, .next = NULL
 *
 *  Delete Entry3:
 *
 *   - HEAD = &entry1
 *   - Entry1: .pprev = &HEAD, .next = &entry2
 *   - Entry2: .pprev = &entry1, .next = NULL
 *
 */

void
lws_dll_remove(struct lws_dll *d)
{
	if (!d->prev) /* ie, not part of the list */
		return;

	/*
	 *  remove us
	 *
	 *  USp <-> us <-> USn  -->  USp <-> USn
	 */

	/* if we have a next guy, set his prev to our prev */
	if (d->next)
		d->next->prev = d->prev;

	/* set our prev guy to our next guy instead of us */
	if (d->prev)
		d->prev->next = d->next;

	/* we're out of the list, we should not point anywhere any more */
	d->prev = NULL;
	d->next = NULL;
}

int
lws_dll_foreach_safe(struct lws_dll *phead, int (*cb)(struct lws_dll *d))
{
	lws_start_foreach_dll_safe(struct lws_dll *, p, tp, phead->next) {
		if (cb(p))
			return 1;
	} lws_end_foreach_dll_safe(p, tp);

	return 0;
}

#if !(defined(LWS_PLAT_OPTEE) && !defined(LWS_WITH_NETWORK))

LWS_VISIBLE lws_usec_t
lws_now_usecs(void)
{
	struct timeval now;

	gettimeofday(&now, NULL);
	return (now.tv_sec * 1000000ll) + now.tv_usec;
}
#endif

int
lws_pthread_self_to_tsi(struct lws_context *context)
{
#if LWS_MAX_SMP > 1
	pthread_t ps = pthread_self();
	struct lws_context_per_thread *pt = &context->pt[0];
	int n;

	for (n = 0; n < context->count_threads; n++) {
		if (pthread_equal(ps, pt->self))
			return n;
		pt++;
	}

	return -1;
#else
	return 0;
#endif
}


/* lws_buflist */

int
lws_buflist_append_segment(struct lws_buflist **head, const uint8_t *buf,
			   size_t len)
{
	struct lws_buflist *nbuf;
	int first = !*head;
	void *p = *head;
	int sanity = 1024;

	assert(buf);
	assert(len);

	/* append at the tail */
	while (*head) {
		if (!--sanity) {
			lwsl_err("%s: buflist reached sanity limit\n", __func__);
			return -1;
		}
		if (*head == (*head)->next) {
			lwsl_err("%s: corrupt list points to self\n", __func__);
			return -1;
		}
		head = &((*head)->next);
	}

	lwsl_info("%s: len %u first %d %p\n", __func__, (uint32_t)len, first, p);

	nbuf = (struct lws_buflist *)lws_malloc(sizeof(**head) + len, __func__);
	if (!nbuf) {
		lwsl_err("%s: OOM\n", __func__);
		return -1;
	}

	nbuf->len = len;
	nbuf->pos = 0;
	nbuf->next = NULL;

	p = (void *)nbuf->buf;
	memcpy(p, buf, len);

	*head = nbuf;

	return first; /* returns 1 if first segment just created */
}

static int
lws_buflist_destroy_segment(struct lws_buflist **head)
{
	struct lws_buflist *old = *head;

	assert(*head);
	*head = old->next;
	old->next = NULL;
	lws_free(old);

	return !*head; /* returns 1 if last segment just destroyed */
}

void
lws_buflist_destroy_all_segments(struct lws_buflist **head)
{
	struct lws_buflist *p = *head, *p1;

	while (p) {
		p1 = p->next;
		p->next = NULL;
		lws_free(p);
		p = p1;
	}

	*head = NULL;
}

size_t
lws_buflist_next_segment_len(struct lws_buflist **head, uint8_t **buf)
{
	if (!*head) {
		if (buf)
			*buf = NULL;

		return 0;
	}

	if (!(*head)->len && (*head)->next)
		lws_buflist_destroy_segment(head);

	if (!*head) {
		if (buf)
			*buf = NULL;

		return 0;
	}

	assert((*head)->pos < (*head)->len);

	if (buf)
		*buf = (*head)->buf + (*head)->pos;

	return (*head)->len - (*head)->pos;
}

int
lws_buflist_use_segment(struct lws_buflist **head, size_t len)
{
	assert(*head);
	assert(len);
	assert((*head)->pos + len <= (*head)->len);

	(*head)->pos += len;
	if ((*head)->pos == (*head)->len)
		lws_buflist_destroy_segment(head);

	if (!*head)
		return 0;

	return (int)((*head)->len - (*head)->pos);
}

void
lws_buflist_describe(struct lws_buflist **head, void *id)
{
	struct lws_buflist *old;
	int n = 0;

	if (*head == NULL)
		lwsl_notice("%p: buflist empty\n", id);

	while (*head) {
		lwsl_notice("%p: %d: %llu / %llu (%llu left)\n", id, n,
			    (unsigned long long)(*head)->pos,
			    (unsigned long long)(*head)->len,
			    (unsigned long long)(*head)->len - (*head)->pos);
		old = *head;
		head = &((*head)->next);
		if (*head == old) {
			lwsl_err("%s: next points to self\n", __func__);
			break;
		}
		n++;
	}
}

LWS_EXTERN void *
lws_context_user(struct lws_context *context)
{
	return context->user_space;
}

LWS_VISIBLE void
lws_explicit_bzero(void *p, size_t len)
{
	volatile uint8_t *vp = p;

	while (len--)
		*vp++ = 0;
}

#if !(defined(LWS_PLAT_OPTEE) && !defined(LWS_WITH_NETWORK))

/**
 * lws_now_secs() - seconds since 1970-1-1
 *
 */
LWS_VISIBLE LWS_EXTERN unsigned long
lws_now_secs(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);

	return tv.tv_sec;
}

LWS_VISIBLE LWS_EXTERN int
lws_compare_time_t(struct lws_context *context, time_t t1, time_t t2)
{
	if (t1 < context->time_discontiguity)
		t1 += context->time_fixup;

	if (t2 < context->time_discontiguity)
		t2 += context->time_fixup;

	return (int)(t1 - t2);
}
#endif
LWS_VISIBLE extern const char *
lws_canonical_hostname(struct lws_context *context)
{
	return (const char *)context->canonical_hostname;
}

#if defined(LWS_WITH_SOCKS5)
LWS_VISIBLE int
lws_set_socks(struct lws_vhost *vhost, const char *socks)
{
	char *p_at, *p_colon;
	char user[96];
	char password[96];

	if (!socks)
		return -1;

	vhost->socks_user[0] = '\0';
	vhost->socks_password[0] = '\0';

	p_at = strrchr(socks, '@');
	if (p_at) { /* auth is around */
		if ((unsigned int)(p_at - socks) > (sizeof(user)
			+ sizeof(password) - 2)) {
			lwsl_err("Socks auth too long\n");
			goto bail;
		}

		p_colon = strchr(socks, ':');
		if (p_colon) {
			if ((unsigned int)(p_colon - socks) > (sizeof(user)
				- 1) ) {
				lwsl_err("Socks user too long\n");
				goto bail;
			}
			if ((unsigned int)(p_at - p_colon) > (sizeof(password)
				- 1) ) {
				lwsl_err("Socks password too long\n");
				goto bail;
			}

			lws_strncpy(vhost->socks_user, socks, p_colon - socks + 1);
			lws_strncpy(vhost->socks_password, p_colon + 1,
				p_at - (p_colon + 1) + 1);
		}

		lwsl_info(" Socks auth, user: %s, password: %s\n",
			vhost->socks_user, vhost->socks_password );

		socks = p_at + 1;
	}

	lws_strncpy(vhost->socks_proxy_address, socks,
		    sizeof(vhost->socks_proxy_address));

	p_colon = strchr(vhost->socks_proxy_address, ':');
	if (!p_colon && !vhost->socks_proxy_port) {
		lwsl_err("socks_proxy needs to be address:port\n");
		return -1;
	} else {
		if (p_colon) {
			*p_colon = '\0';
			vhost->socks_proxy_port = atoi(p_colon + 1);
		}
	}

	lwsl_info(" Socks %s:%u\n", vhost->socks_proxy_address,
			vhost->socks_proxy_port);

	return 0;

bail:
	return -1;
}
#endif



LWS_VISIBLE LWS_EXTERN int
lws_get_count_threads(struct lws_context *context)
{
	return context->count_threads;
}

static const unsigned char e0f4[] = {
	0xa0 | ((2 - 1) << 2) | 1, /* e0 */
	0x80 | ((4 - 1) << 2) | 1, /* e1 */
	0x80 | ((4 - 1) << 2) | 1, /* e2 */
	0x80 | ((4 - 1) << 2) | 1, /* e3 */
	0x80 | ((4 - 1) << 2) | 1, /* e4 */
	0x80 | ((4 - 1) << 2) | 1, /* e5 */
	0x80 | ((4 - 1) << 2) | 1, /* e6 */
	0x80 | ((4 - 1) << 2) | 1, /* e7 */
	0x80 | ((4 - 1) << 2) | 1, /* e8 */
	0x80 | ((4 - 1) << 2) | 1, /* e9 */
	0x80 | ((4 - 1) << 2) | 1, /* ea */
	0x80 | ((4 - 1) << 2) | 1, /* eb */
	0x80 | ((4 - 1) << 2) | 1, /* ec */
	0x80 | ((2 - 1) << 2) | 1, /* ed */
	0x80 | ((4 - 1) << 2) | 1, /* ee */
	0x80 | ((4 - 1) << 2) | 1, /* ef */
	0x90 | ((3 - 1) << 2) | 2, /* f0 */
	0x80 | ((4 - 1) << 2) | 2, /* f1 */
	0x80 | ((4 - 1) << 2) | 2, /* f2 */
	0x80 | ((4 - 1) << 2) | 2, /* f3 */
	0x80 | ((1 - 1) << 2) | 2, /* f4 */

	0,			   /* s0 */
	0x80 | ((4 - 1) << 2) | 0, /* s2 */
	0x80 | ((4 - 1) << 2) | 1, /* s3 */
};

LWS_EXTERN int
lws_check_byte_utf8(unsigned char state, unsigned char c)
{
	unsigned char s = state;

	if (!s) {
		if (c >= 0x80) {
			if (c < 0xc2 || c > 0xf4)
				return -1;
			if (c < 0xe0)
				return 0x80 | ((4 - 1) << 2);
			else
				return e0f4[c - 0xe0];
		}

		return s;
	}
	if (c < (s & 0xf0) || c >= (s & 0xf0) + 0x10 + ((s << 2) & 0x30))
		return -1;

	return e0f4[21 + (s & 3)];
}

LWS_EXTERN int
lws_check_utf8(unsigned char *state, unsigned char *buf, size_t len)
{
	unsigned char s = *state;

	while (len--) {
		unsigned char c = *buf++;

		if (!s) {
			if (c >= 0x80) {
				if (c < 0xc2 || c > 0xf4)
					return 1;
				if (c < 0xe0)
					s = 0x80 | ((4 - 1) << 2);
				else
					s = e0f4[c - 0xe0];
			}
		} else {
			if (c < (s & 0xf0) ||
			    c >= (s & 0xf0) + 0x10 + ((s << 2) & 0x30))
				return 1;
			s = e0f4[21 + (s & 3)];
		}
	}

	*state = s;

	return 0;
}


char *
lws_strdup(const char *s)
{
	char *d = lws_malloc(strlen(s) + 1, "strdup");

	if (d)
		strcpy(d, s);

	return d;
}

static const char *hex = "0123456789ABCDEF";

LWS_VISIBLE LWS_EXTERN const char *
lws_sql_purify(char *escaped, const char *string, int len)
{
	const char *p = string;
	char *q = escaped;

	while (*p && len-- > 2) {
		if (*p == '\'') {
			*q++ = '\'';
			*q++ = '\'';
			len --;
			p++;
		} else
			*q++ = *p++;
	}
	*q = '\0';

	return escaped;
}

LWS_VISIBLE LWS_EXTERN const char *
lws_json_purify(char *escaped, const char *string, int len)
{
	const char *p = string;
	char *q = escaped;

	if (!p) {
		escaped[0] = '\0';
		return escaped;
	}

	while (*p && len-- > 6) {
		if (*p == '\t') {
			p++;
			*q++ = '\\';
			*q++ = 't';
			continue;
		}

		if (*p == '\n') {
			p++;
			*q++ = '\\';
			*q++ = 'n';
			continue;
		}

		if (*p == '\r') {
			p++;
			*q++ = '\\';
			*q++ = 'r';
			continue;
		}

		if (*p == '\"' || *p == '\\' || *p < 0x20) {
			*q++ = '\\';
			*q++ = 'u';
			*q++ = '0';
			*q++ = '0';
			*q++ = hex[((*p) >> 4) & 15];
			*q++ = hex[(*p) & 15];
			len -= 5;
			p++;
		} else
			*q++ = *p++;
	}
	*q = '\0';

	return escaped;
}

LWS_VISIBLE LWS_EXTERN void
lws_filename_purify_inplace(char *filename)
{
	while (*filename) {

		if (*filename == '.' && filename[1] == '.') {
			*filename = '_';
			filename[1] = '_';
		}

		if (*filename == ':' ||
		    *filename == '\\' ||
		    *filename == '$' ||
		    *filename == '%')
			*filename = '_';

		filename++;
	}
}

LWS_VISIBLE LWS_EXTERN const char *
lws_urlencode(char *escaped, const char *string, int len)
{
	const char *p = string;
	char *q = escaped;

	while (*p && len-- > 3) {
		if (*p == ' ') {
			*q++ = '+';
			p++;
			continue;
		}
		if ((*p >= '0' && *p <= '9') ||
		    (*p >= 'A' && *p <= 'Z') ||
		    (*p >= 'a' && *p <= 'z')) {
			*q++ = *p++;
			continue;
		}
		*q++ = '%';
		*q++ = hex[(*p >> 4) & 0xf];
		*q++ = hex[*p & 0xf];

		len -= 2;
		p++;
	}
	*q = '\0';

	return escaped;
}

LWS_VISIBLE LWS_EXTERN int
lws_urldecode(char *string, const char *escaped, int len)
{
	int state = 0, n;
	char sum = 0;

	while (*escaped && len) {
		switch (state) {
		case 0:
			if (*escaped == '%') {
				state++;
				escaped++;
				continue;
			}
			if (*escaped == '+') {
				escaped++;
				*string++ = ' ';
				len--;
				continue;
			}
			*string++ = *escaped++;
			len--;
			break;
		case 1:
			n = char_to_hex(*escaped);
			if (n < 0)
				return -1;
			escaped++;
			sum = n << 4;
			state++;
			break;

		case 2:
			n = char_to_hex(*escaped);
			if (n < 0)
				return -1;
			escaped++;
			*string++ = sum | n;
			len--;
			state = 0;
			break;
		}

	}
	*string = '\0';

	return 0;
}

LWS_VISIBLE LWS_EXTERN int
lws_finalize_startup(struct lws_context *context)
{
	struct lws_context_creation_info info;

	info.uid = context->uid;
	info.gid = context->gid;

#if defined(LWS_HAVE_SYS_CAPABILITY_H) && defined(LWS_HAVE_LIBCAP)
	memcpy(info.caps, context->caps, sizeof(info.caps));
	info.count_caps = context->count_caps;
#endif

	if (lws_check_opt(context->options, LWS_SERVER_OPTION_EXPLICIT_VHOSTS))
		lws_plat_drop_app_privileges(&info);

	return 0;
}

LWS_VISIBLE LWS_EXTERN void
lws_get_effective_uid_gid(struct lws_context *context, int *uid, int *gid)
{
	*uid = context->uid;
	*gid = context->gid;
}

int
lws_snprintf(char *str, size_t size, const char *format, ...)
{
	va_list ap;
	int n;

	if (!size)
		return 0;

	va_start(ap, format);
	n = vsnprintf(str, size, format, ap);
	va_end(ap);

	if (n >= (int)size)
		return (int)size;

	return n;
}

char *
lws_strncpy(char *dest, const char *src, size_t size)
{
	strncpy(dest, src, size - 1);
	dest[size - 1] = '\0';

	return dest;
}

int
lws_timingsafe_bcmp(const void *a, const void *b, uint32_t len)
{
	const uint8_t *pa = a, *pb = b;
	uint8_t sum = 0;

	while (len--)
		sum |= (*pa++ ^ *pb++);

	return sum;
}


typedef enum {
	LWS_TOKZS_LEADING_WHITESPACE,
	LWS_TOKZS_QUOTED_STRING,
	LWS_TOKZS_TOKEN,
	LWS_TOKZS_TOKEN_POST_TERMINAL
} lws_tokenize_state;

int
lws_tokenize(struct lws_tokenize *ts)
{
	const char *rfc7230_delims = "(),/:;<=>?@[\\]{}";
	lws_tokenize_state state = LWS_TOKZS_LEADING_WHITESPACE;
	char c, flo = 0, d_minus = '-', d_dot = '.', s_minus = '\0',
	     s_dot = '\0';
	signed char num = -1;
	int utf8 = 0;

	/* for speed, compute the effect of the flags outside the loop */

	if (ts->flags & LWS_TOKENIZE_F_MINUS_NONTERM) {
		d_minus = '\0';
		s_minus = '-';
	}
	if (ts->flags & LWS_TOKENIZE_F_DOT_NONTERM) {
		d_dot = '\0';
		s_dot = '.';
	}

	ts->token = NULL;
	ts->token_len = 0;

	while (ts->len) {
		c = *ts->start++;
		ts->len--;

		utf8 = lws_check_byte_utf8((unsigned char)utf8, c);
		if (utf8 < 0)
			return LWS_TOKZE_ERR_BROKEN_UTF8;

		if (!c)
			break;

		/* whitespace */

		if (c == ' ' || c == '\t' || c == '\n' || c == '\r' ||
		    c == '\f') {
			switch (state) {
			case LWS_TOKZS_LEADING_WHITESPACE:
			case LWS_TOKZS_TOKEN_POST_TERMINAL:
				continue;
			case LWS_TOKZS_QUOTED_STRING:
				ts->token_len++;
				continue;
			case LWS_TOKZS_TOKEN:
				/* we want to scan forward to look for = */

				state = LWS_TOKZS_TOKEN_POST_TERMINAL;
				continue;
			}
		}

		/* quoted string */

		if (c == '\"') {
			if (state == LWS_TOKZS_QUOTED_STRING)
				return LWS_TOKZE_QUOTED_STRING;

			/* starting a quoted string */

			if (ts->flags & LWS_TOKENIZE_F_COMMA_SEP_LIST) {
				if (ts->delim == LWSTZ_DT_NEED_DELIM)
					return LWS_TOKZE_ERR_COMMA_LIST;
				ts->delim = LWSTZ_DT_NEED_DELIM;
			}

			state = LWS_TOKZS_QUOTED_STRING;
			ts->token = ts->start;
			ts->token_len = 0;

			continue;
		}

		/* token= aggregation */

		if (c == '=' && (state == LWS_TOKZS_TOKEN_POST_TERMINAL ||
				 state == LWS_TOKZS_TOKEN)) {
			if (num == 1)
				return LWS_TOKZE_ERR_NUM_ON_LHS;
			/* swallow the = */
			return LWS_TOKZE_TOKEN_NAME_EQUALS;
		}

		/* optional token: aggregation */

		if ((ts->flags & LWS_TOKENIZE_F_AGG_COLON) && c == ':' &&
		    (state == LWS_TOKZS_TOKEN_POST_TERMINAL ||
		     state == LWS_TOKZS_TOKEN))
			/* swallow the : */
			return LWS_TOKZE_TOKEN_NAME_COLON;

		/* aggregate . in a number as a float */

		if (c == '.' && !(ts->flags & LWS_TOKENIZE_F_NO_FLOATS) &&
		    state == LWS_TOKZS_TOKEN && num == 1) {
			if (flo)
				return LWS_TOKZE_ERR_MALFORMED_FLOAT;
			flo = 1;
			ts->token_len++;
			continue;
		}

		/*
		 * Delimiter... by default anything that:
		 *
		 *  - isn't matched earlier, or
		 *  - is [A-Z, a-z, 0-9, _], and
		 *  - is not a partial utf8 char
		 *
		 * is a "delimiter", it marks the end of a token and is itself
		 * reported as a single LWS_TOKZE_DELIMITER each time.
		 *
		 * However with LWS_TOKENIZE_F_RFC7230_DELIMS flag, tokens may
		 * contain any noncontrol character that isn't defined in
		 * rfc7230_delims, and only characters listed there are treated
		 * as delimiters.
		 */

		if (!utf8 &&
		     ((ts->flags & LWS_TOKENIZE_F_RFC7230_DELIMS &&
		     strchr(rfc7230_delims, c) && c > 32) ||
		    ((!(ts->flags & LWS_TOKENIZE_F_RFC7230_DELIMS) &&
		     (c < '0' || c > '9') && (c < 'A' || c > 'Z') &&
		     (c < 'a' || c > 'z') && c != '_') &&
		     c != s_minus && c != s_dot) ||
		    c == d_minus || c == d_dot
		    )) {
			switch (state) {
			case LWS_TOKZS_LEADING_WHITESPACE:
				if (ts->flags & LWS_TOKENIZE_F_COMMA_SEP_LIST) {
					if (c != ',' ||
					    ts->delim != LWSTZ_DT_NEED_DELIM)
						return LWS_TOKZE_ERR_COMMA_LIST;
					ts->delim = LWSTZ_DT_NEED_NEXT_CONTENT;
				}

				ts->token = ts->start - 1;
				ts->token_len = 1;
				return LWS_TOKZE_DELIMITER;

			case LWS_TOKZS_QUOTED_STRING:
				ts->token_len++;
				continue;

			case LWS_TOKZS_TOKEN_POST_TERMINAL:
			case LWS_TOKZS_TOKEN:
				/* report the delimiter next time */
				ts->start--;
				ts->len++;
				goto token_or_numeric;
			}
		}

		/* anything that's not whitespace or delimiter is payload */

		switch (state) {
		case LWS_TOKZS_LEADING_WHITESPACE:

			if (ts->flags & LWS_TOKENIZE_F_COMMA_SEP_LIST) {
				if (ts->delim == LWSTZ_DT_NEED_DELIM)
					return LWS_TOKZE_ERR_COMMA_LIST;
				ts->delim = LWSTZ_DT_NEED_DELIM;
			}

			state = LWS_TOKZS_TOKEN;
			ts->token = ts->start - 1;
			ts->token_len = 1;
			if (c < '0' || c > '9')
				num = 0;
			else
				if (num < 0)
					num = 1;
			continue;
		case LWS_TOKZS_QUOTED_STRING:
		case LWS_TOKZS_TOKEN:
			if (c < '0' || c > '9')
				num = 0;
			else
				if (num < 0)
					num = 1;
			ts->token_len++;
			continue;
		case LWS_TOKZS_TOKEN_POST_TERMINAL:
			/* report the new token next time */
			ts->start--;
			ts->len++;
			goto token_or_numeric;
		}
	}

	/* we ran out of content */

	if (utf8) /* ended partway through a multibyte char */
		return LWS_TOKZE_ERR_BROKEN_UTF8;

	if (state == LWS_TOKZS_QUOTED_STRING)
		return LWS_TOKZE_ERR_UNTERM_STRING;

	if (state != LWS_TOKZS_TOKEN_POST_TERMINAL &&
	    state != LWS_TOKZS_TOKEN) {
		if ((ts->flags & LWS_TOKENIZE_F_COMMA_SEP_LIST) &&
		     ts->delim == LWSTZ_DT_NEED_NEXT_CONTENT)
			return LWS_TOKZE_ERR_COMMA_LIST;

		return LWS_TOKZE_ENDED;
	}

	/* report the pending token */

token_or_numeric:

	if (num != 1)
		return LWS_TOKZE_TOKEN;
	if (flo)
		return LWS_TOKZE_FLOAT;

	return LWS_TOKZE_INTEGER;
}


LWS_VISIBLE LWS_EXTERN int
lws_tokenize_cstr(struct lws_tokenize *ts, char *str, int max)
{
	if (ts->token_len + 1 >= max)
		return 1;

	memcpy(str, ts->token, ts->token_len);
	str[ts->token_len] = '\0';

	return 0;
}

LWS_VISIBLE LWS_EXTERN void
lws_tokenize_init(struct lws_tokenize *ts, const char *start, int flags)
{
	ts->start = start;
	ts->len = 0x7fffffff;
	ts->flags = flags;
	ts->delim = LWSTZ_DT_NEED_FIRST_CONTENT;
}

#if LWS_MAX_SMP > 1

ptw32_handle_t null_ptw32_handle;
void
lws_mutex_refcount_init(struct lws_mutex_refcount *mr)
{
	pthread_mutex_init(&mr->lock, NULL);
	mr->last_lock_reason = NULL;
	mr->lock_depth = 0;
	mr->metadata = 0;
	mr->lock_owner = null_ptw32_handle;
}

void
lws_mutex_refcount_destroy(struct lws_mutex_refcount *mr)
{
	pthread_mutex_destroy(&mr->lock);
}

void
lws_mutex_refcount_lock(struct lws_mutex_refcount *mr, const char *reason)
{
	/* if true, this sequence is atomic because our thread has the lock
	 *
	 *  - if true, only guy who can race to make it untrue is our thread,
	 *    and we are here.
	 *
	 *  - if false, only guy who could race to make it true is our thread,
	 *    and we are here
	 *
	 *  - it can be false and change to a different tid that is also false
	 */
	if (mr->lock_owner.p == pthread_self().p) {
		/* atomic because we only change it if we own the lock */
		mr->lock_depth++;
		return;
	}

	pthread_mutex_lock(&mr->lock);
	/* atomic because only we can have the lock */
	mr->last_lock_reason = reason;
	mr->lock_owner = pthread_self();
	mr->lock_depth = 1;
	//lwsl_notice("tid %d: lock %s\n", mr->tid, reason);
}

void
lws_mutex_refcount_unlock(struct lws_mutex_refcount *mr)
{
	if (--mr->lock_depth)
		/* atomic because only thread that has the lock can unlock */
		return;

	mr->last_lock_reason = "free";
	mr->lock_owner = null_ptw32_handle;
	//lwsl_notice("tid %d: unlock %s\n", mr->tid, mr->last_lock_reason);
	pthread_mutex_unlock(&mr->lock);
}

#endif /* SMP */


const char *
lws_cmdline_option(int argc, const char **argv, const char *val)
{
	int n = (int)strlen(val), c = argc;

	while (--c > 0) {

		if (!strncmp(argv[c], val, n)) {
			if (!*(argv[c] + n) && c < argc - 1) {
				/* coverity treats unchecked argv as "tainted" */
				if (!argv[c + 1] || strlen(argv[c + 1]) > 1024)
					return NULL;
				return argv[c + 1];
			}

			return argv[c] + n;
		}
	}

	return NULL;
}

