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
	const char *key;
	const char *value;
	struct HTML_TAG_ATTRIBUTE *next;
} HTML_TAG_ATTRIBUTE;

typedef struct HTML_TAG {
	HTML_TAG_TYPE type;
	const char *tag;
	HTML_TAG_ATTRIBUTE *attributes;
	struct HTML_TAG *children;
	struct HTML_TAG *next;
} HTML_TAG;

typedef struct HTML {
	const char *doctype;
	HTML_TAG *tags;
	HTML_TAG *head;
	HTML_TAG *body;
} HTML;

HTML *html_create(const char *title);
void html_head_add(HTML *html, HTML_TAG *tag);
void html_body_add(HTML *html, HTML_TAG *tag);

HTML_TAG *html_tag_text(const char *text);
HTML_TAG *html_tag_single(const char *tag, HTML_TAG_ATTRIBUTE *attributes);
HTML_TAG *html_tag_double(const char *tag, HTML_TAG_ATTRIBUTE *attributes, HTML_TAG *children);
HTML_TAG_ATTRIBUTE *html_tag_attributes(unsigned int n, ...);

void html_write(HTML *html);
void html_write_tag(HTML_TAG *tag, unsigned int level);
void html_write_attributes(HTML_TAG_ATTRIBUTE *attribute);
void html_indent(unsigned int level);

#endif