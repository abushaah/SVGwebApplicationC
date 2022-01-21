#ifndef SVGHELPER_H
#define SVGHELPER_H

#include "SVGParser.h"

void get_element_names(xmlNode * a_node, SVG * svg);
Attribute* otherAttributes (char *name, char *content);

Rectangle* rectAttributes(xmlNode *cur_node);
Circle* circAttributes(xmlNode *cur_node);
void firstOtherAttributes(xmlNode *cur_node, List* otherAttributesList);

int numberWithUnits(float* number, char* units, char* value);
void verifyCopy(char* field, xmlChar * data, size_t fLength, size_t dLength);
int validChar(char* word);

#endif
