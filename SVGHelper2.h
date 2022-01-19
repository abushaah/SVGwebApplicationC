#include "SVGParser.h"

static void get_element_names(xmlNode * a_node, SVG * svg);
void verifyCopy(char* field, xmlChar * data, size_t fLength, size_t dLength);
