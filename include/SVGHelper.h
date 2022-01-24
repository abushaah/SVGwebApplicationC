#ifndef SVGHELPER_H
#define SVGHELPER_H

#include "SVGParser.h"

// svg parser function that loops through and creates the structs
void get_element_names(xmlNode* a_node, SVG* svg, Group** group, int* groupIdx);

// struct creation functions
Attribute* otherAttributes (char *name, char *content);
Rectangle* rectAttributes(xmlNode *cur_node);
Circle* circAttributes(xmlNode *cur_node);
Path* pathAttributes (xmlNode *cur_node);
Group* groupAttributes (xmlNode *cur_node);

// list within a struct creation functions
void firstOtherAttributes(xmlNode *cur_node, List* otherAttributesList);
void freeGroup(Group** group, int groupIdx);

// validity check functions
int numberWithUnits(float* number, char* units, char* value);
void verifyCopy(char* field, char* data, size_t fLength, size_t dLength);
int validChar(char* word);

#endif
