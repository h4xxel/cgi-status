#ifndef HTML_H
#define HTML_H

static const char html_doctype_html5[]="html";
static const char html_doctype_xhtml11[]="html PUBLIC \"-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\"";

typedef enum HTML_TAG_TYPE {
	HTML_TAG_TYPE_TEXT,
	HTML_TAG_TYPE_SINGLE,
	HTML_TAG_TYPE_DOUBLE,
} HTML_TAG_TYPE;

typedef struct HTML_TAG_ATTRIBUTE {
	char *key;
	char *value;
	struct HTML_TAG_ATTRIBUTE *next;
} HTML_TAG_ATTRIBUTE;

typedef struct HTML_TAG {
	HTML_TAG_TYPE type;
	char *tag;
	HTML_TAG_ATTRIBUTE *attributes;
	struct HTML_TAG *children;
	struct HTML_TAG *next;
} HTML_TAG;

typedef struct HTML {
	char *doctype;
	HTML_TAG *tags;
} HTML;

void html_write(HTML *html);
void html_write_tag(HTML_TAG *tag, unsigned int level);
void html_write_attributes(HTML_TAG_ATTRIBUTE *attribute);
void html_indent(unsigned int level);

#endif