#include <stdio.h>
#include "html.h"

void html_write(HTML *html) {
	printf("<!DOCTYPE %s>", html->doctype);
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
				printf("<%s ", tag->tag);
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