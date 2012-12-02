#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "html.h"

HTML *html_create(const char *title) {
	HTML *html;
	HTML_TAG *meta;
	if(!(html=malloc(sizeof(HTML))))
		return NULL;
	html->doctype=html_doctype_html5;
	meta=html_tag_single("meta", html_tag_attributes(2, "http-equiv", "Content-Type", "content", "text/html; charset=utf-8"));
	meta->next=html_tag_double("title", NULL, html_tag_text(title));
	html->head=html_tag_double("head", NULL, meta);
	html->body=html_tag_double("body", NULL, NULL);
	html->head->next=html->body;
	html->tags=html_tag_double("html", NULL, html->head);
	return html;
}

void html_head_add(HTML *html, HTML_TAG *tag) {
	HTML_TAG **t;
	for(t=&html->head->children; t&&*t; t=&(*t)->next);
	*t=tag;
	//for(html->head=tag; html->head->next; html->head=html->head->next);
}

void html_body_add(HTML *html, HTML_TAG *tag) {
	HTML_TAG **t;
	for(t=&html->body->children; t&&*t; t=&(*t)->next);
	*t=tag;
}

HTML_TAG *html_tag_text(const char *text) {
	HTML_TAG *tag;
	if(!(tag=malloc(sizeof(HTML_TAG))))
		return NULL;
	tag->type=HTML_TAG_TYPE_TEXT;
	tag->tag=text;
	tag->attributes=NULL;
	tag->children=NULL;
	tag->next=NULL;
	return tag;
}

HTML_TAG *html_tag_single(const char *text, HTML_TAG_ATTRIBUTE *attributes) {
	HTML_TAG *tag;
	if(!(tag=malloc(sizeof(HTML_TAG))))
		return NULL;
	tag->type=HTML_TAG_TYPE_SINGLE;
	tag->tag=text;
	tag->attributes=attributes;
	tag->children=NULL;
	tag->next=NULL;
	return tag;
}

HTML_TAG *html_tag_double(const char *text, HTML_TAG_ATTRIBUTE *attributes, HTML_TAG *children) {
	HTML_TAG *tag;
	if(!(tag=malloc(sizeof(HTML_TAG))))
		return NULL;
	tag->type=HTML_TAG_TYPE_DOUBLE;
	tag->tag=text;
	tag->attributes=attributes;
	tag->children=children;
	tag->next=NULL;
	return tag;
}

HTML_TAG_ATTRIBUTE *html_tag_attributes(unsigned int n, ...) {
	//TODO: add malloc null check
	HTML_TAG_ATTRIBUTE *attributes, *a;
	unsigned int i;
	if(n<1)
		return NULL;
	va_list argp;
	va_start(argp, n);
	attributes=malloc(sizeof(HTML_TAG_ATTRIBUTE));
	a=attributes;
	a->key=va_arg(argp, const char *);
	a->value=va_arg(argp, const char *);
	for(i=1; i<n; i++) {
		a->next=malloc(sizeof(HTML_TAG_ATTRIBUTE));
		a=a->next;
		a->key=va_arg(argp, const char *);
		a->value=va_arg(argp, const char *);
		a->next=NULL;
	}
	va_end(argp);
	return attributes;
}

void html_write(HTML *html) {
	printf("<!DOCTYPE %s>\n", html->doctype);
	html_write_tag(html->tags, 0);
}

void html_write_tag(HTML_TAG *tag, unsigned int level) {
	for(; tag; tag=tag->next) {
		switch(tag->type) {
			case HTML_TAG_TYPE_TEXT:
				html_indent(level);
				printf("%s\n", tag->tag);
				break;
			case HTML_TAG_TYPE_SINGLE:
				html_indent(level);
				printf("<%s", tag->tag);
				html_write_attributes(tag->attributes);
				printf(" />\n");
				break;
			case HTML_TAG_TYPE_DOUBLE:
				html_indent(level);
				printf("<%s", tag->tag);
				html_write_attributes(tag->attributes);
				printf(">\n");
				html_write_tag(tag->children, level+1);
				html_indent(level);
				printf("</%s>\n", tag->tag);
				break;
		}
	}
}

void html_write_attributes(HTML_TAG_ATTRIBUTE *attribute) {
	for(; attribute; attribute=attribute->next)
		printf(" %s=\"%s\"", attribute->key, attribute->value);
}

void html_indent(unsigned int level) {
	unsigned int i;
	for(i=0; i<level; i++)
		printf("\t");
}